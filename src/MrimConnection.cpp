/*
 *      MrimConnection.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#include "MrimConnection.h"
#include "Application.h"

using namespace Swift;


MrimConnection::MrimConnection() {
  appInstance->logEvent("MrimConnection::MrimConnection()", SEVERITY_DEBUG);
  mCommandNumber = 1;
  mIsPinging = false;
  mConnected = false;
  mPingPeriod = 30;
  mSock = 0;

  // connecting signal handlers
  appInstance->sigServer->signal_hello_ack().connect(sigc::mem_fun(*this, &MrimConnection::onHelloAck));
  appInstance->sigServer->signal_logout().connect(sigc::mem_fun(*this, &MrimConnection::onLogout));
  appInstance->sigServer->signal_connection_params().connect(sigc::mem_fun(*this, &MrimConnection::onHelloAck));
  appInstance->sigServer->signal_login_ack().connect(sigc::mem_fun(*this, &MrimConnection::onLoginAck));
}

guint32 MrimConnection::getCommandNumber() {
  return mCommandNumber;
}

void MrimConnection::updateCommandNumber() {
  ++mCommandNumber;
}

bool MrimConnection::sendLPS(Glib::ustring s) {
  appInstance->logEvent("MrimConnection::sendLPS()", SEVERITY_DEBUG);
  const std::string mrimEncoding = "cp1251";
  const std::string clientEncoding = "utf-8";
  std::string converted;
  try {
    converted = Glib::convert(s, mrimEncoding, clientEncoding);
  }
  catch(Glib::ConvertError &err) {
    appInstance->logEvent("In MrimConnection::sendLPS " + err.what(), SEVERITY_WARNING);
    return false;
  }
  sendUL((guint32)converted.length());
  sendRawData(converted.c_str(), converted.length());
  return true;
}

bool MrimConnection::sendUL(guint32 x) {
  appInstance->logEvent("MrimConnection::sendUL()", SEVERITY_DEBUG);
  return (sendRawData((char*) &x, sizeof(guint32)) != -1);
}

bool MrimConnection::sendUIDL(UIDL x) {
  appInstance->logEvent("MrimConnection::sendUIDL()", SEVERITY_DEBUG);
  return true;
}

guint32 MrimConnection::readUL() {
  appInstance->logEvent("MrimConnection::readUL()", SEVERITY_DEBUG);
  guint32 result = 0;
  recvRawData((char*) &result, sizeof(guint32));
  return result;
}

UIDL MrimConnection::readUIDL() {
  appInstance->logEvent("MrimConnection::readUIDL()", SEVERITY_DEBUG);
  UIDL result;
  return result;
}

bool MrimConnection::setDestinationAddress() {
  appInstance->logEvent("MrimConnection::setDestinationAddress()", SEVERITY_DEBUG);
  gint s = socket(AF_INET, SOCK_STREAM, 0);
  if(s == -1) {
    appInstance->logEvent("In MrimConnection::setDestinationAddress socket() failed", SEVERITY_WARNING);
    return false;
  }
  hostent *h = gethostbyname(GET_IPPORT_HOST.c_str());
  if(h == NULL) {
    appInstance->logEvent("In MrimConnection::setDestinationAddress gethostbyname() failed", SEVERITY_WARNING);
    return false;
  }
  sockaddr_in addr;
  ((gulong*) &addr.sin_addr)[0] = ((gulong**) h->h_addr_list)[0][0];
  addr.sin_family = AF_INET;
  addr.sin_port = htons(GET_IPPORT_PORT);
  gint status = ::connect(s, (sockaddr*) &addr, sizeof(addr));
  if(status == -1) {
    appInstance->logEvent("In MrimConnection::setDestinationAddress connect() failed", SEVERITY_WARNING);
    return false;
  }
  char* buf = (char*) g_malloc0(32);
  gint sz = recv(s, buf, 32, 0);
  if(sz == -1) {
    appInstance->logEvent("In MrimConnection::setDestinationAddress recv() failed", SEVERITY_WARNING);
    return false;
  }
  gchar** result = g_strsplit(buf, ":", 2);
  mDestinationAddress.sin_family = AF_INET;
  mDestinationAddress.sin_addr.s_addr = inet_addr(result[0]);
  mDestinationAddress.sin_port = htons(atoi(result[1]));
  g_free(buf);
  g_strfreev(result);
  return true;
}

bool MrimConnection::connect() {
  appInstance->logEvent("MrimConnection::connect()", SEVERITY_DEBUG);
  if(mConnected) return true;
  mSock = socket(AF_INET, SOCK_STREAM, 0);
  if (mSock == -1) {
    appInstance->logEvent("In MrimConnection::connect socket() failed", SEVERITY_WARNING);
    return false;
  }
  if(!setDestinationAddress()) {
    return false;
  }
  gint status = ::connect(mSock,(sockaddr*) &mDestinationAddress, sizeof(mDestinationAddress));
  mConnected = (status != -1);
  if(!mConnected) {
    appInstance->logEvent("In MrimConnection::connect connect() failed", SEVERITY_WARNING);
    return false;
  }
  if(mConnected) {
    Glib::signal_io().connect(sigc::mem_fun(*this, &MrimConnection::connectionHangupCallback), mSock, Glib::IO_HUP);
    MrimPacket p(MRIM_CS_HELLO);
    p.send();
    // receive MRIM_CS_HELLO_ACK
    do {
      p = MrimPacket::receive();
    } while(p.getHeader().msg != MRIM_CS_HELLO_ACK);
    startListen();
  }
  return mConnected;
}

void MrimConnection::pingCallback() {
  appInstance->logEvent("MrimConnection::pingCallback()", SEVERITY_DEBUG);
  do {
    gulong interval = mPingPeriod * 1000000;
    g_usleep(interval);
    if(!mIsPinging) return;
    MrimPacket p(MRIM_CS_PING);
    p.send();
  } while(true);
}

void MrimConnection::startPing() {
  appInstance->logEvent("MrimConnection::startPing()", SEVERITY_DEBUG);
  if(!mIsPinging) {
    mIsPinging = true;
    mPingThread = Glib::Thread::create(sigc::mem_fun(*this, &MrimConnection::pingCallback), false);
  }
}

void MrimConnection::stopPing() {
  appInstance->logEvent("MrimConnection::stopPing()", SEVERITY_DEBUG);
  mIsPinging = false;
}

guint32 MrimConnection::readLPS(Glib::ustring *buffer, Glib::ustring fromEncoding) {
  appInstance->logEvent("MrimConnection::readLPS()", SEVERITY_DEBUG);
  const std::string clientEncoding = "utf-8";
  Glib::ustring converted;
  char* data;
  guint32 stringLength = readUL();
  if(stringLength < 1) return 0;
  buffer->clear();
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
      appInstance->logEvent("In MrimConnection::readLPS " + err.what(), SEVERITY_WARNING);
      return 0;
    }
  }
  *buffer = converted;
  *buffer = buffer->normalize();
  return stringLength;
}

bool MrimConnection::listenCallback(Glib::IOCondition condition) {
  //appInstance->logEvent("MrimConnection::listenCallback()", SEVERITY_DEBUG);
  if((condition & Glib::IO_IN) || (condition & Glib::IO_PRI)) {
    // ready to read
    MrimPacket::receive();
  }
  return true;
}

void MrimConnection::startListen() {
  appInstance->logEvent("MrimConnection::startListen()", SEVERITY_DEBUG);
  if(!mListenConnection) {
    Glib::RefPtr<Glib::IOChannel> channel;
    #ifdef G_OS_WIN32
    channel = Glib::IOChannel::create_from_win32_socket(mSock);
    #else
    channel = Glib::IOChannel::create_from_fd(mSock);
    #endif
    //mListenConnection = Glib::signal_io().connect(sigc::mem_fun(*this, &MrimConnection::listenCallback), channel, Glib::IO_IN, G_PRIORITY_LOW);
    mListenConnection = Glib::signal_io().connect(sigc::mem_fun(*this, &MrimConnection::listenCallback), channel, Glib::IO_IN, G_PRIORITY_LOW);
  }
}

void MrimConnection::stopListen() {
  appInstance->logEvent("MrimConnection::stopListen()", SEVERITY_DEBUG);
  mListenConnection.disconnect();
}

void MrimConnection::disconnect() {
  appInstance->logEvent("MrimConnection::disconnect()", SEVERITY_DEBUG);
  mConnected = false;
  mCommandNumber = 1;
  stopListen();
  stopPing();
  #ifdef G_OS_WIN32
  closesocket(mSock);
  #else
  close(mSock);
  #endif
}

bool MrimConnection::connectionHangupCallback(Glib::IOCondition condition) {
  appInstance->logEvent("MrimConnection::connectionHangupCallback()", SEVERITY_DEBUG);
  if(condition & Glib::IO_HUP) {
    appInstance->logEvent("Connection lost.", SEVERITY_WARNING);
    appInstance->showMessage("Connection error", "Connection to server has lost", "Check your connection status", Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
    disconnect();
  }
}

gint MrimConnection::sendRawData(const char* data, guint length) {
  appInstance->logEvent("MrimConnection::sendRawData()", SEVERITY_DEBUG);
  gint total = 0;
  gint n;
  while(total < length) {
    n = send(mSock, data + total, length - total, 0);
    if(n <= 0) {
      return n;
    }
    total += n;
  }
  return total;
}

gint MrimConnection::recvRawData(char* data, guint length) {
  //appInstance->logEvent("MrimConnection::recvRawData()", SEVERITY_DEBUG);
  gint total = 0;
  gint n;
  while(total < length) {
    n = recv(mSock, data + total, length - total, 0);
    if(n <= 0) {
      return n;
    }
    total += n;
  }
  return total;
}

void MrimConnection::readUnknownData(gchar *buffer, size_t len) {
  appInstance->logEvent("MrimConnection::readUnknownData()", SEVERITY_DEBUG);
  recvRawData(buffer, len);
}

bool MrimConnection::sendHeader(mrim_packet_header_t header) {
  appInstance->logEvent("MrimConnection::sendHeader()", SEVERITY_DEBUG);
  if(sendRawData((char*) &header.magic, sizeof(header.magic)) != -1) {
    if(sendRawData((char*) &header.proto, sizeof(header.proto)) != -1) {
      if(sendRawData((char*) &header.seq, sizeof(header.seq)) != -1) {
        if(sendRawData((char*) &header.msg, sizeof(header.msg)) != -1) {
          if(sendRawData((char*) &header.dlen, sizeof(header.dlen)) != -1) {
            if(sendRawData((char*) &header.from, sizeof(header.from)) != -1) {
              if(sendRawData((char*) &header.fromport, sizeof(header.fromport)) != -1) {
                if(sendRawData((char*) &header.reserved, sizeof(header.reserved)) != -1) {
                  return true;
                }
              }
            }
          }
        }
      }
    }
  }
  return false;
}

bool MrimConnection::readHeader(mrim_packet_header_t *header) {
  //appInstance->logEvent("MrimConnection::readHeader()", SEVERITY_DEBUG);
  if(recvRawData((char*) &(header->magic), sizeof(header->magic)) != -1) {
    if(recvRawData((char*) &(header->proto), sizeof(header->proto)) != -1) {
      if(recvRawData((char*) &(header->seq), sizeof(header->seq)) != -1) {
        if(recvRawData((char*) &(header->msg), sizeof(header->msg)) != -1) {
          if(recvRawData((char*) &(header->dlen), sizeof(header->dlen)) != -1) {
            if(recvRawData((char*) &(header->from), sizeof(header->from)) != -1) {
              if(recvRawData((char*) &(header->fromport), sizeof(header->fromport)) != -1) {
                if(recvRawData((char*) &(header->reserved), sizeof(header->reserved)) != -1) {
                  return true;
                }
              }
            }
          }
        }
      }
    }
  }
  return false;
}

void MrimConnection::onHelloAck(guint32 pingPeriod) {
  appInstance->logEvent("MrimConnection::onHelloAck()", SEVERITY_DEBUG);
  mPingPeriod = pingPeriod;
}

void MrimConnection::onLogout(guint32 reason) {
  appInstance->logEvent("MrimConnection::onLogout()", SEVERITY_DEBUG);
  // we must stop pinging, but we can still listen server.
  // so we don't need to call stopListen(), because user can login again.
  stopPing();
}

void MrimConnection::onLoginAck() {
  startPing();
}

guint32 MrimConnection::getPingPeriod() {
  return mPingPeriod;
}

void MrimConnection::onConnectionParamsReceive(guint32 pingPeriod) {
  appInstance->logEvent("MrimConnection::onConnectionParamsReceive()", SEVERITY_DEBUG);
  mPingPeriod = pingPeriod;
}
