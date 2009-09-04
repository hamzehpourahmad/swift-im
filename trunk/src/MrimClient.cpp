/*
 *      MrimClient.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
 *
 *      Copyright (c) 2009 Кожаев Галымжан <kozhayev(at)gmail(dot)com>
 *
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MrimClient.h"
#include "Application.h"
#include "MrimUtils.h"

using namespace Swift;

MrimClient::MrimClient() {
  commandNumber = 1;
  packetHeader.magic = CS_MAGIC;
  packetHeader.proto = PROTO_VERSION;
  packetHeader.from  = 0;
  packetHeader.fromport = 0;
  userAgentString = "Swift-IM v0.1";
  isSendingNow = false;
  connected = false;
  helloSent = false;
  userStatus = STATUS_OFFLINE;
}

guint32 MrimClient::sendMessage(Glib::ustring destination, Glib::ustring message, guint32 flags) {
  guint32 sendDataLength, result;
  MrimMessage sentMessage;
  time_t now;
  Glib::ustring rtf = " ";
  sendDataLength = destination.length() + message.length() + rtf.length() + 4 * sizeof(guint32);
  while(isSendingNow) {
    g_usleep(SEND_WAIT_TIME);
  }
  result = commandNumber;
  sendHeader(MRIM_CS_MESSAGE, sendDataLength);
  isSendingNow = true;
  sendUL(flags);
  sendLPS(destination);
  sendLPS(message);
  sendLPS(rtf);
  isSendingNow = false;
  sentMessage.seq = result;
  time(&now);
  sentMessage.time = now;
  messageList.push_back(sentMessage);
  return result;
}

bool MrimClient::sendHello() {
  if(helloSent) return true;
  mrim_packet_header_t header;
  if(!sendHeader(MRIM_CS_HELLO, 0)) {
    return false;
  }
  bool status = recvHeader(&header);
  if(!status || header.magic != CS_MAGIC) {
    return false;
  }
  pingPeriod = receiveConnectionParameters();
  startPing();
  helloSent = true;
  return true;
}

Glib::ustring MrimClient::login(Glib::ustring login, Glib::ustring password, guint32 networkStatus) {
  userInfo["LOGIN"] = login;
  guint32 sendDataLength = 0;
  mrim_packet_header_t header;
  Glib::ustring loginRejectReason = "ok";
  sendDataLength = login.length() + password.length() + userAgentString.length() + 4 * sizeof(guint32);
  while(isSendingNow) {
    g_usleep(SEND_WAIT_TIME);
  }
  if(!sendHeader(MRIM_CS_LOGIN2, sendDataLength)) {
    loginRejectReason = "Internal client error";
  }
  isSendingNow = true;
  if(!sendLPS(login)) {
    loginRejectReason = "Internal client error";
  }
  if(!sendLPS(password)) {
    loginRejectReason = "Internal client error";
  }
  if(!sendUL(networkStatus)) {
    loginRejectReason = "Internal client error";
  }
  if(!sendLPS(userAgentString)) {
    loginRejectReason = "Internal client error";
  }
  isSendingNow = false;
  // Reading header
  bool status = recvHeader(&header);
  if(!status || header.magic != CS_MAGIC) {
    loginRejectReason = "Internal client error";
  }
  if(header.msg == MRIM_CS_LOGIN_REJ) {
    readLPSFromSocket(&loginRejectReason, "cp1251");
  }
  if(loginRejectReason == "ok") {
    userStatus = networkStatus;
    startListen();
  }
  return loginRejectReason;
}

guint32 MrimClient::getUserStatus() {
  return userStatus;
}

bool MrimClient::sendLPS(Glib::ustring s) {
  const std::string mrimEncoding = "cp1251";
  const std::string clientEncoding = "utf-8";
  std::string converted;
  try {
    converted = Glib::convert(s, mrimEncoding, clientEncoding);
  }
  catch(Glib::ConvertError &err) {
    printf("%s\n", err.what().c_str());
    return false;
  }
  sendUL((guint32)converted.length());
  sendRawData(converted.c_str(), converted.length());
  return true;
}

bool MrimClient::sendUL(guint32 x) {
  return (sendRawData((char*) &x, sizeof(guint32)) != -1);
}

bool MrimClient::sendHeader(guint32 packetType, guint32 dataLength) {
  bool result = true;
  packetHeader.seq = commandNumber;
  ++commandNumber;
  packetHeader.msg = packetType;
  packetHeader.dlen = dataLength;
  while(isSendingNow) {
    g_usleep(SEND_WAIT_TIME);
  }
  isSendingNow = true;
  result = result && (sendRawData((char*) &packetHeader.magic, sizeof(packetHeader.magic)) != -1);
  result = result && (sendRawData((char*) &packetHeader.proto, sizeof(packetHeader.proto)) != -1);
  result = result && (sendRawData((char*) &packetHeader.seq, sizeof(packetHeader.seq)) != -1);
  result = result && (sendRawData((char*) &packetHeader.msg, sizeof(packetHeader.msg)) != -1);
  result = result && (sendRawData((char*) &packetHeader.dlen, sizeof(packetHeader.dlen)) != -1);
  result = result && (sendRawData((char*) &packetHeader.from, sizeof(packetHeader.from)) != -1);
  result = result && (sendRawData((char*) &packetHeader.fromport, sizeof(packetHeader.fromport)) != -1);
  result = result && (sendRawData(packetHeader.reserved, sizeof(packetHeader.reserved)) != -1);
  isSendingNow = false;
  return result;
}

bool MrimClient::recvHeader(mrim_packet_header_t* buffer) {
  if(buffer == NULL) {
    return false;
  }
  bool result = true;
  result = result && (recvRawData((char*) &((*buffer).magic), sizeof(buffer->magic)) != -1);
  result = result && (recvRawData((char*) &((*buffer).proto), sizeof(buffer->proto)) != -1);
  result = result && (recvRawData((char*) &((*buffer).seq), sizeof(buffer->seq)) != -1);
  result = result && (recvRawData((char*) &((*buffer).msg), sizeof(buffer->msg)) != -1);
  result = result && (recvRawData((char*) &((*buffer).dlen), sizeof(buffer->dlen)) != -1);
  result = result && (recvRawData((char*) &((*buffer).from), sizeof(buffer->from)) != -1);
  result = result && (recvRawData((char*) &((*buffer).fromport), sizeof(buffer->fromport)) != -1);
  result = result && (recvRawData((*buffer).reserved, sizeof(buffer->reserved)) != -1);
  return result;
}

bool MrimClient::setDestinationAddress() {
  gint s = socket(AF_INET, SOCK_STREAM, 0);
  if(s == -1) {
    return false;
  }
  hostent *h = gethostbyname(GET_IPPORT_HOST.c_str());
  if(h == NULL) {
    return false;
  }
  sockaddr_in addr;
  ((gulong*) &addr.sin_addr)[0] = ((gulong**) h->h_addr_list)[0][0];
  addr.sin_family = AF_INET;
  addr.sin_port = htons(GET_IPPORT_PORT);
  gint status = ::connect(s, (sockaddr*) &addr, sizeof(addr));
  if(status == -1) {
    return false;
  }
  char* buf = (char*) g_malloc0(32);
  gint sz = recv(s, buf, 32, 0);
  if(sz == -1) {
    return false;
  }
  gchar** result = g_strsplit(buf, ":", 2);
  destinationAddress.sin_family = AF_INET;
  destinationAddress.sin_addr.s_addr = inet_addr(result[0]);
  destinationAddress.sin_port = htons(atoi(result[1]));
  g_free(buf);
  g_strfreev(result);
  return true;
}

bool MrimClient::connect() {
  if(connected) return true;
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    return false;
  }
  if(!setDestinationAddress()) {
    return false;
  }
  int status = ::connect(sock,(sockaddr*) &destinationAddress, sizeof(destinationAddress));
  connected = (status != -1);
  if(!connected) {
    return false;
  }
  connected = sendHello();
  if(connected) {
    Glib::signal_io().connect(sigc::mem_fun(*this, &MrimClient::connectionHangupCallback), sock, Glib::IO_HUP);
  }
  return connected;
}

void MrimClient::checkMessageListWithCallback() {
  time_t now;
  MessageList::iterator it;
  time(&now);
  for(it = messageList.begin(); it != messageList.end(); it++) {
    if(now - it->time >= pingPeriod) {
      Application::getAppInstance()->sigServer.signal_message_send_timeout().emit(it->seq);
    }
  }
}

void MrimClient::pingCallback(MrimClient *client) {
  do {
    client->checkMessageListWithCallback();
    gulong interval = client->pingPeriod * 1000000;
    g_usleep(interval);
    client->sendHeader(MRIM_CS_PING, 0);
    if(!client->isPingingNow) break;
  } while(true);
}

void MrimClient::startPing() {
  isPingingNow = true;
  pingThread = g_thread_create(GThreadFunc(pingCallback), this, false, NULL);
}

void MrimClient::stopPing() {
  isPingingNow = false;
}

guint32 MrimClient::readLPSFromSocket(Glib::ustring *buffer, Glib::ustring fromEncoding) {
  const std::string clientEncoding = "utf-8";
  Glib::ustring converted;
  char* data;
  guint32 stringLength;
  buffer->clear();
  recvRawData((char*) &stringLength, sizeof(guint32));
  if(stringLength < 1) return 0;
  data = (char*) g_malloc0(stringLength + 1);
  recvRawData((char*) data, (int)stringLength);
  data[stringLength] = '\0';
  converted = data;
  g_free(data);
  if(!converted.is_ascii()) {
    try {
      converted = Glib::convert(converted, clientEncoding, fromEncoding);
    }
    catch(Glib::ConvertError &err) {
      printf("%s\n", err.what().c_str());
      return 0;
    }
  }
  *buffer = converted;
  *buffer = buffer->normalize();
  return stringLength;
}

UserInfo MrimClient::receiveUserInfo(guint32 dataLength) {
  Glib::ustring key, value;
  UserInfo result;
  // add user login to resulting user info records
  result["LOGIN"] = userInfo["LOGIN"];
  static int guintsSize = sizeof(guint32) * 2;
  while(dataLength > 0) {
    int readed = 0;
    readed += readLPSFromSocket(&key, "cp1251");
    readed += readLPSFromSocket(&value, "cp1251");
    result[key] = value;
    dataLength -= (guintsSize + readed);
  }
  return result;
}

bool MrimClient::checkContactsReceived(guint32 *dataLength) {
  *dataLength -= sizeof(guint32);
  guint32 contactListStatus;
  recvRawData((char*) &contactListStatus, sizeof(guint32));
  if(contactListStatus == GET_CONTACTS_OK) {
    return true;
  }
  return false;
}

GroupList MrimClient::receiveGroupList(guint32 *dataLength) {
  guint32 number;
  Glib::ustring str;
  MrimGroup group;
  GroupList result;
  gint u, s, i, j, n = groupMask.length();
  for(i = 0; i < groupsNumber; i++) {
    u = s = 0;
    for(j = 0; j < n; j++) {
      if(groupMask[j] == 'u') {
        recvRawData((char*) &number, sizeof(guint32));
        *dataLength -= sizeof(guint32);
        switch(u) {
          case 0:
            group.flag = number;
            break;
        }
        u++;
      }
      else if(groupMask[j] == 's') {
        *dataLength -= readLPSFromSocket(&str, "cp1251") + sizeof(guint32);
        switch(s) {
          case 0:
            group.name = str;
            break;
        }
        s++;
      }
      else if(groupMask[j] == 'z') {

      }
    }
    group.index = i;
    result.push_back(group);
    g_usleep(GROUP_FETCH_DELAY);
  }
  return result;
}

ContactList MrimClient::receiveContactList(guint32 *dataLength) {
  guint32 number;
  Glib::ustring str;
  MrimContact contact;
  ContactList result;
  gint u, s, j, n = contactsMask.length(), cc = 20;
  while(*dataLength > 0) {
    u = s = 0;
    for(j = 0; j < n; j++) {
      if(contactsMask[j] == 'u') {
        recvRawData((char*) &number, sizeof(guint32));
        *dataLength -= sizeof(guint32);
        switch(u) {
          case 0:
            contact.flag = number;
            break;
          case 1:
            contact.group = number;
            break;
          case 2:
            contact.serverFlags = number;
            break;
          case 3:
            contact.status = number;
            break;
        }
        u++;
      }
      else if(contactsMask[j] == 's') {
        int x = readLPSFromSocket(&str, "cp1251");
        *dataLength -= x + sizeof(guint32);
        switch(s) {
          case 0:
            contact.address = str;
            break;
          case 1:
            contact.nickname = str;
            break;
          default:;
        }
        s++;
      }
      else if(contactsMask[j] == 'z') {
        ;
      }
    }
    contact.index = cc++;
    result.push_back(contact);
    g_usleep(CONTACT_FETCH_DELAY);
  }
  return result;
}

guint32 MrimClient::receiveGroupsNumber(guint32 *dataLength) {
  guint32 groupsNumber;
  recvRawData((char*) &groupsNumber, sizeof(guint32));
  *dataLength -= sizeof(guint32);
  return groupsNumber;
}

Glib::ustring MrimClient::receiveGroupMask(guint32 *dataLength) {
  Glib::ustring groupMask;
  *dataLength -= readLPSFromSocket(&groupMask, "cp1251") + sizeof(guint32);
  return groupMask;
}

Glib::ustring MrimClient::receiveContactsMask(guint32 *dataLength) {
  Glib::ustring contactsMask;
  *dataLength -= readLPSFromSocket(&contactsMask, "cp1251") + sizeof(guint32);
  return contactsMask;
}

void MrimClient::receiveChangedStatus() {
  guint32 newStatus;
  Glib::ustring user;
  recvRawData((char*) &newStatus, sizeof(guint32));
  readLPSFromSocket(&user, "cp1251");
  Application::getAppInstance()->sigServer.signal_user_status().emit(newStatus, user);
}

guint32 MrimClient::receiveConnectionParameters() {
  mrim_connection_params_t connectionParameters;
  recvRawData((char*) &connectionParameters, sizeof(connectionParameters));
  return connectionParameters.ping_period;
}

guint32 MrimClient::receiveLogoutMessage() {
  guint32 logoutReason;
  recvRawData((char*) &logoutReason, sizeof(guint32));
  return logoutReason;
}

guint32 MrimClient::receiveMessageStatus(guint32 seq) {
  guint32 status;
  recvRawData((char*) &status, sizeof(guint32));
  MessageList::iterator it;
  for(it = messageList.begin(); it != messageList.end(); it++) {
    if(it->seq == seq) {
      messageList.erase(it);
      break;
    }
  }
  return status;
}

void MrimClient::sendMessageRecv(Glib::ustring from, guint32 msgId) {
  guint32 sentDataLength = from.length() + 2 * sizeof(guint32);
  while(isSendingNow) {
    g_usleep(SEND_WAIT_TIME);
  }
  sendHeader(MRIM_CS_MESSAGE_RECV, sentDataLength);
  isSendingNow = true;
  sendLPS(from);
  sendUL(msgId);
  isSendingNow = false;
}

void MrimClient::receiveMessage() {
  guint32 msgId, flags;
  Glib::ustring from, message, rtf = "";
  recvRawData((char*) &msgId, sizeof(guint32));
  recvRawData((char*) &flags, sizeof(guint32));
  readLPSFromSocket(&from, "cp1251");
  readLPSFromSocket(&message, "cp1251");
  if(flags & MESSAGE_FLAG_RTF) {
    readLPSFromSocket(&rtf, "cp1251");
  }
  if(flags != MESSAGE_FLAG_NORECV) {
    sendMessageRecv(from, msgId);
  }
  Application::getAppInstance()->sigServer.signal_message_receive().emit(flags, from, message, rtf);
}

bool MrimClient::listenCallback(Glib::IOCondition condition) {
  mrim_packet_header_t receivedPacketHeader;
  guint32 number;
  bool status;
  if((condition & Glib::IO_IN) || (condition & Glib::IO_PRI)) {
    // ready to read
    status = recvHeader(&receivedPacketHeader);
    if(status && receivedPacketHeader.magic == CS_MAGIC) {
      printf("Received new packet: %d\n", receivedPacketHeader.msg);
      switch(receivedPacketHeader.msg) {
        case MRIM_CS_USER_INFO:
          userInfo = receiveUserInfo(receivedPacketHeader.dlen);
          Application::getAppInstance()->sigServer.signal_user_info_receive().emit();
          break;

        case MRIM_CS_CONTACT_LIST2:
          if(checkContactsReceived(&receivedPacketHeader.dlen)) {
            groupsNumber = receiveGroupsNumber(&receivedPacketHeader.dlen);
            groupMask = receiveGroupMask(&receivedPacketHeader.dlen);
            contactsMask = receiveContactsMask(&receivedPacketHeader.dlen);
            groupList.clear();
            groupList = receiveGroupList(&receivedPacketHeader.dlen);
            contactList.clear();
            contactList = receiveContactList(&receivedPacketHeader.dlen);
            Application::getAppInstance()->sigServer.signal_contact_list_receive().emit();
          }
          break;

        case MRIM_CS_USER_STATUS:
          if(groupList.size() > 0) {
            receiveChangedStatus();
          }
          break;

        case MRIM_CS_CONNECTION_PARAMS:
          pingPeriod = receiveConnectionParameters();
          break;

        case MRIM_CS_LOGOUT:
          number = receiveLogoutMessage();
          Application::getAppInstance()->sigServer.signal_logout().emit(number);
          logout();
          break;

        case MRIM_CS_MESSAGE_STATUS:
          number = receiveMessageStatus(receivedPacketHeader.seq);
          Application::getAppInstance()->sigServer.signal_message_status().emit(receivedPacketHeader.seq, number);
          break;

        case MRIM_CS_MESSAGE_ACK:
          receiveMessage();
          break;

        default:
          char* data = (char*) g_malloc0(receivedPacketHeader.dlen);
          recvRawData(data, receivedPacketHeader.dlen);
          g_free(data);
          break;
      }
    }
  }
  return true;
}

void MrimClient::startListen() {
  Glib::RefPtr<Glib::IOChannel> channel;
  #ifdef G_OS_WIN32
  channel = Glib::IOChannel::create_from_win32_socket(sock);
  #else
  channel = Glib::IOChannel::create_from_fd(sock);
  #endif
  listenConnection = Glib::signal_io().connect(sigc::mem_fun(*this, &MrimClient::listenCallback), channel, Glib::IO_IN, G_PRIORITY_LOW);
}

void MrimClient::stopListen() {
  listenConnection.disconnect();
}

void MrimClient::disconnect() {
  connected = false;
  #ifdef G_OS_WIN32
  closesocket(sock);
  #else
  close(sock);
  #endif
}

void MrimClient::logout() {
  // @TODO: finish this function
  stopPing();
  stopListen();
  disconnect();
}

bool MrimClient::connectionHangupCallback(Glib::IOCondition condition) {
  if(condition & Glib::IO_HUP) {
    Application::getAppInstance()->showMessage("Connection error", "Connection to server has lost", "Check your connection status", Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
    logout();
  }
}

gint MrimClient::sendRawData(const char* data, guint length) {
  gint total = 0;
  gint n;
  while(total < length) {
    n = send(sock, data + total, length - total, 0);
    if(n <= 0) {
      return n;
    }
    total += n;
  }
  return total;
}

gint MrimClient::recvRawData(char* data, guint length) {
  gint total = 0;
  gint n;
  while(total < length) {
    n = recv(sock, data + total, length - total, 0);
    if(n <= 0) {
      return n;
    }
    total += n;
  }
  return total;
}

MrimClient::~MrimClient() {
  // Free avatar images
  ContactList::iterator it;
  for(it = contactList.begin(); it != contactList.end(); it++) {
    delete it->avatar;
  }
}

void MrimClient::changeStatus(guint32 newStatus) {
  guint32 sendDataLength;
  sendDataLength = sizeof(guint32);
  while(isSendingNow) {
    g_usleep(SEND_WAIT_TIME);
  }
  sendHeader(MRIM_CS_CHANGE_STATUS, sendDataLength);
  isSendingNow = true;
  sendUL(newStatus);
  isSendingNow = false;
  userStatus = newStatus;
}
