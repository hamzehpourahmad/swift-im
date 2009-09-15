/*
 *      MrimPacket.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#include "MrimPacket.h"
#include "Application.h"

using namespace Swift;

MrimPacket::MrimPacket() {
  //appInstance->logEvent("MrimPacket::MrimPacket()", SEVERITY_DEBUG);
  memset(&header, 0, sizeof(header));
  connection = appInstance->mConnection;
  mType = 0;
}

MrimPacket::MrimPacket(guint32 type) {
  memset(&header, 0, sizeof(header));
  connection = appInstance->mConnection;
  mType = type;
}

void MrimPacket::setType(guint32 type) {
  mType = type;
}

bool MrimPacket::validate() {
  return header.magic == CS_MAGIC;
}

bool MrimPacket::send() {
  appInstance->logEvent("MrimPacket::send()", SEVERITY_DEBUG);
  Glib::StaticMutex mutex = GLIBMM_STATIC_MUTEX_INIT;
  mutex.lock();
  bool result = true;
  DATAArray::iterator it;
  ULArray::iterator ulit = ulList.begin();
  UIDLArray::iterator uidlit = uidlList.begin();
  LPSArray::iterator lpsit = lpsList.begin();
  prepareHeader();
  connection->updateCommandNumber();
  connection->sendHeader(header);
  for(it = dataQueue.begin(); it != dataQueue.end(); it++) {
    if(!result) break;
    switch(*it) {
      case MRIMDATATYPE_UL:
        if(ulit != ulList.end()) {
          // sendUL
          if(!connection->sendUL(*ulit)) {
            result = false;
          }
          ulit++;
        }
        break;
      case MRIMDATATYPE_UIDL:
        if(uidlit != uidlList.end()) {
          // send uidl
          if(!connection->sendUIDL(*uidlit)) {
            result = false;
          }
          uidlit++;
        }
        break;
      case MRIMDATATYPE_LPS:
        if(lpsit != lpsList.end()) {
          // send lps
          if(!connection->sendLPS(*lpsit)) {
            result = false;
          }
          lpsit++;
        }
        break;
    }
  }
  mutex.unlock();
  return result;
}

MrimPacket MrimPacket::receive() {
  //appInstance->logEvent("MrimPacket::receive()", SEVERITY_DEBUG);
  MrimPacket p;
  if(p.connection->readHeader(&(p.header)) && p.validate()) {
    p.receiveData();
  }
  return p;
}

void MrimPacket::prepareHeader() {
  appInstance->logEvent("MrimPacket::prepareHeader()", SEVERITY_DEBUG);
  header.magic = CS_MAGIC;
  header.proto = PROTO_VERSION;
  header.seq = connection->getCommandNumber();
  header.msg = mType;
  header.dlen = getDlen();
  header.from = 0;
  header.fromport = 0;
}

void MrimPacket::addUL(guint32 ul) {
  appInstance->logEvent("MrimPacket::addUL()", SEVERITY_DEBUG);
  ulList.push_back(ul);
  dataQueue.push_back(MRIMDATATYPE_UL);
}

void MrimPacket::addLPS(Glib::ustring lps) {
  appInstance->logEvent("MrimPacket::addLPS()", SEVERITY_DEBUG);
  lpsList.push_back(lps);
  dataQueue.push_back(MRIMDATATYPE_LPS);
}

void MrimPacket::addUIDL(UIDL uidl) {
  appInstance->logEvent("MrimPacket::addUIDL()", SEVERITY_DEBUG);
  uidlList.push_back(uidl);
  dataQueue.push_back(MRIMDATATYPE_UIDL);
}

guint32 MrimPacket::getDlen() {
  appInstance->logEvent("MrimPacket::getDlen()", SEVERITY_DEBUG);
  guint32 result = 0;
  LPSArray::iterator lpsit = lpsList.begin();
  for(lpsit = lpsList.begin(); lpsit != lpsList.end(); lpsit++) {
    result += getLPSLength(*lpsit) + sizeof(guint32);
  }
  result += uidlList.size() * sizeof(gchar) * 8;
  result += ulList.size() * sizeof(guint32);
  return result;
}

guint32 MrimPacket::getLPSLength(Glib::ustring lps) {
  const std::string mrimEncoding = "cp1251";
  const std::string clientEncoding = "utf-8";
  std::string converted;
  try {
    converted = Glib::convert(lps, mrimEncoding, clientEncoding);
  }
  catch(Glib::ConvertError &err) {
    appInstance->logEvent(err.what(), SEVERITY_WARNING);
  }
  return converted.length();
}

void MrimPacket::receiveData() {
  appInstance->logEvent("MrimPacket::receiveData()", SEVERITY_DEBUG);
  Glib::ustring reasonStr, address, from, message, rtf;
  guint32 pingPeriod, status, reason, msgid, flags;
  UserInfo t;
  GroupList gl;
  switch(header.msg) {
    case MRIM_CS_HELLO_ACK:
      appInstance->logEvent("New packet: MRIM_CS_HELLO_ACK", SEVERITY_DEBUG);
      receiveConnectionParameters(&pingPeriod);
      appInstance->sigServer->signal_hello_ack().emit(pingPeriod);
      break;

    case MRIM_CS_LOGIN_ACK:
      appInstance->logEvent("New packet: MRIM_CS_LOGIN_ACK", SEVERITY_DEBUG);
      appInstance->sigServer->signal_login_ack().emit();
      break;

    case MRIM_CS_LOGIN_REJ:
      appInstance->logEvent("New packet: MRIM_CS_LOGIN_REJ", SEVERITY_DEBUG);
      receiveLoginRejectReason(&reasonStr);
      appInstance->sigServer->signal_login_rej().emit(reasonStr);
      break;

    case MRIM_CS_USER_INFO:
      appInstance->logEvent("New packet: MRIM_CS_USER_INFO", SEVERITY_DEBUG);
      receiveUserInfo(&t);
      appInstance->sigServer->signal_user_info_receive().emit(t);
      break;

    case MRIM_CS_CONTACT_LIST2:
      appInstance->logEvent("New packet: MRIM_CS_CONTACT_LIST2", SEVERITY_DEBUG);
      receiveContactList(&gl);
      appInstance->sigServer->signal_contact_list_receive().emit(gl);
      break;

    case MRIM_CS_USER_STATUS:
      appInstance->logEvent("New packet: MRIM_CS_USER_STATUS", SEVERITY_DEBUG);
      receiveStatus(&status, &address);
      appInstance->sigServer->signal_user_status().emit(status, address);
      break;

    case MRIM_CS_CONNECTION_PARAMS:
      appInstance->logEvent("New packet: MRIM_CS_CONNECTION_PARAMS", SEVERITY_DEBUG);
      receiveConnectionParameters(&pingPeriod);
      appInstance->sigServer->signal_connection_params().emit(pingPeriod);
      break;

    case MRIM_CS_LOGOUT:
      appInstance->logEvent("New packet: MRIM_CS_LOGOUT", SEVERITY_DEBUG);
      receiveLogoutMessage(&reason);
      appInstance->sigServer->signal_logout().emit(reason);
      break;

    case MRIM_CS_MESSAGE_STATUS:
      appInstance->logEvent("New packet: MRIM_CS_MESSAGE_STATUS", SEVERITY_DEBUG);
      receiveMessageStatus(&status);
      appInstance->sigServer->signal_message_status().emit(header.seq, status);
      break;

    case MRIM_CS_MESSAGE_ACK:
      appInstance->logEvent("New packet: MRIM_CS_MESSAGE_ACK", SEVERITY_DEBUG);
      receiveMessage(&msgid, &flags, &from, &message, &rtf);
      appInstance->sigServer->signal_message_receive().emit(msgid, flags, from, message, rtf);
      break;

    default:
      appInstance->logEvent("New packet: UNKNOWN", SEVERITY_DEBUG);
      receiveUnknownData();
      break;
  }
}

void MrimPacket::receiveUserInfo(UserInfo *ui) {
  appInstance->logEvent("MrimPacket::receiveUserInfo()", SEVERITY_DEBUG);
  Glib::ustring key, value;
  static int guintsSize = sizeof(guint32) * 2;
  guint32 dataLength = header.dlen;
  while(dataLength > 0) {
    int readed = 0;
    readed += connection->readLPS(&key, "cp1251");
    readed += connection->readLPS(&value, "cp1251");
    (*ui)[key] = value;
    dataLength -= (guintsSize + readed);
  }
}

void MrimPacket::receiveContactList(GroupList *gl) {
  appInstance->logEvent("MrimPacket::receiveContactList()", SEVERITY_DEBUG);
  guint32 dataLength = header.dlen;
  Glib::ustring groupMask, contactsMask, lps;
  MrimContact contact;
  MrimGroup group;
  guint32 ul;
  guint32 status = connection->readUL();
  dataLength -= sizeof(guint32);
  if(status == GET_CONTACTS_OK) {
    // add "Unauthorized" group first
    group.setName("Waiting for authorization");
    group.setFlags(0);
    (*gl)[GROUP_INDEX_NOT_AUTHORIZED] = group;

    // reading groups
    guint32 groupsNumber = connection->readUL();
    dataLength -= sizeof(guint32);
    dataLength -= connection->readLPS(&groupMask, "cp1251") + sizeof(guint32);
    dataLength -= connection->readLPS(&contactsMask, "cp1251") + sizeof(guint32);
    gint n = groupMask.length();
    for(gint i = 0; i < groupsNumber; i++) {
      for(gint j = 0; j < n; j++) {
        if(groupMask[j] == 'u') {
          ul = connection->readUL();
          dataLength -= sizeof(guint32);
          group.setFlags(ul);
        }
        else if(groupMask[j] == 's') {
          dataLength -= connection->readLPS(&lps, "cp1251") + sizeof(guint32);
          group.setName(lps);
        }
        else if(groupMask[j] == 'z') {

        }
      }
      (*gl)[i] = group;
      g_usleep(GROUP_FETCH_DELAY);
    }

    // reading contact list
    gint u, s, cc = 20;
    n = contactsMask.length();
    while(dataLength > 0) {
      u = s = 0;
      for(gint j = 0; j < n; j++) {
        if(contactsMask[j] == 'u') {
          ul = connection->readUL();
          dataLength -= sizeof(guint32);
          switch(u) {
            case 0:
              contact.setFlag(ul);
              break;
            case 1:
              contact.setGroup(ul);
              break;
            case 2:
              contact.setServerFlags(ul);
              break;
            case 3:
              contact.setStatus(ul);
              break;
          }
          u++;
        }
        else if(contactsMask[j] == 's') {
          dataLength -= connection->readLPS(&lps, "cp1251") + sizeof(guint32);
          switch(s) {
            case 0:
              contact.setAddress(lps);
              break;
            case 1:
              contact.setNickname(lps);
              break;
            default:;
          }
          s++;
        }
        else if(contactsMask[j] == 'z') {
          ;
        }
      }
      contact.setIndex(cc++);
      if(contact.getServerFlags() == CONTACT_INTFLAG_NOT_AUTHORIZED) {
        // if contact isn't authorized
        (*gl)[GROUP_INDEX_NOT_AUTHORIZED].addContact(contact);
      }
      else {
        (*gl)[contact.getGroup()].addContact(contact);
      }
      g_usleep(CONTACT_FETCH_DELAY);
    }
  }
}

void MrimPacket::receiveStatus(guint32 *status, Glib::ustring *address) {
  appInstance->logEvent("MrimPacket::receiveStatus()", SEVERITY_DEBUG);
  *status = connection->readUL();
  connection->readLPS(address, "cp1251");
}

void MrimPacket::receiveConnectionParameters(guint32 *pingPeriod) {
  appInstance->logEvent("MrimPacket::receiveConnectionParameters()", SEVERITY_DEBUG);
  *pingPeriod = connection->readUL();
}

void MrimPacket::receiveLogoutMessage(guint32 *reason) {
  appInstance->logEvent("MrimPacket::receiveLogoutMessage()", SEVERITY_DEBUG);
  *reason = connection->readUL();
}

void MrimPacket::receiveMessageStatus(guint32 *status) {
  appInstance->logEvent("MrimPacket::receiveMessageStatus()", SEVERITY_DEBUG);
  *status = connection->readUL();
}

void MrimPacket::receiveMessage(guint32 *msgid, guint32 *flags, Glib::ustring *from, Glib::ustring *message, Glib::ustring *rtf) {
  appInstance->logEvent("MrimPacket::receiveMessage()", SEVERITY_DEBUG);
  *msgid = connection->readUL();
  *flags = connection->readUL();
  connection->readLPS(from, "cp1251");
  connection->readLPS(message, "cp1251");
  if(*flags & MESSAGE_FLAG_RTF) {
    connection->readLPS(rtf, "cp1251");
  }
}

void MrimPacket::receiveUnknownData() {
  appInstance->logEvent("MrimPacket::receiveUnknownData()", SEVERITY_DEBUG);
  gchar* data = (gchar*) g_malloc(header.dlen + 1);
  connection->readUnknownData(data, header.dlen);
  g_free(data);
}

void MrimPacket::receiveLoginRejectReason(Glib::ustring *reason) {
  appInstance->logEvent("MrimPacket::receiveLoginRejectReason()", SEVERITY_DEBUG);
  connection->readLPS(reason, "cp1251");
}

mrim_packet_header_t MrimPacket::getHeader() {
  return header;
}
