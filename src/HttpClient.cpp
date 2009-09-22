/*
 *      HttpClient.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#include "stdlib.h"
#include <string.h>

#include <gio/gio.h>
#include <glib.h>
#include <gdkmm/pixbuf.h>

#include "HttpClient.h"
#include "Application.h"
#include "MrimUtils.h"

using namespace Swift;

HttpClient::HttpClient() {
  appInstance->logEvent("HttpClient::HttpClient()", SEVERITY_DEBUG);
  connected = false;
}

bool HttpClient::connect(Glib::ustring host) {
  appInstance->logEvent("HttpClient::connect()", SEVERITY_DEBUG);
  if(!connected) {
    if(createTcpSocket()) {
      sockaddr_in addr;
      setDestinationAddress(&addr);
      if(::connect(sock, (sockaddr*) &addr, sizeof(addr)) >= 0) {
        serverHost = host;
        connected = true;
      }
    }
  }
  return connected;
}

bool HttpClient::checkAvatar(std::string contactAddress) {
  appInstance->logEvent("HttpClient::checkAvatar()", SEVERITY_DEBUG);
  //return false;
  if(!connected && !connect(HOST)) {
    return false;
  }
  bool result = false;
  std::pair<std::string, std::string> addr;
  addr = MrimUtils::splitAddress(contactAddress);
  Glib::ustring request = "HEAD /" + addr.second + "/" + addr.first + "/_mrimavatar HTTP/1.1 \r\nHost: " + getServerHost() + "\r\n\r\n";
  send(sock, request.c_str(), request.length(), 0);
  char *data = (char*) g_malloc0(HTTP_BUFFER_SIZE);
  gint sz = recvAll(data, HTTP_BUFFER_SIZE);
  if(sz != 0) {
    Glib::ustring header = data;
    if(header.substr(9, 3) == "200") {
      result = true;
    }
  }
  g_free(data);
  return result;
}

Glib::RefPtr<Gdk::Pixbuf> HttpClient::loadAvatar(std::string contactAddress, AvatarType atype) {
  appInstance->logEvent("HttpClient::loadAvatar()", SEVERITY_DEBUG);
  Glib::RefPtr<Gdk::Pixbuf> result;
  if(!connected && !connect(HOST)) {
    return result;
  }
  std::pair<std::string, std::string> addr;
  addr = MrimUtils::splitAddress(contactAddress);
  Glib::ustring avatarStr = (atype == AVATAR_BIG ? "_mrimavatar" : "_mrimavatarsmall");
  Glib::ustring request = "GET /" + addr.second + "/" + addr.first + "/" + avatarStr + " HTTP/1.1 \r\nHost: " + getServerHost() + "\r\n\r\n";
  send(sock, request.c_str(), request.length(), 0);
  char *data = (char*) g_malloc0(HTTP_BUFFER_SIZE);
  gint sz = recvAll(data, HTTP_BUFFER_SIZE);
  if(sz != 0) {
    char* dataOffset = strstr(data, "\r\n\r\n");
    if(dataOffset != NULL) {
      dataOffset += 4;
      gint dataLen = sz - (dataOffset - data);
      if(dataLen != 0) {
        GInputStream* imgStream = g_memory_input_stream_new_from_data(dataOffset, dataLen, NULL);
        GError* err = NULL;
        GdkPixbuf* imgPixBuf = gdk_pixbuf_new_from_stream(imgStream, NULL, &err);
        if(imgPixBuf != NULL) {
          result = Glib::wrap(imgPixBuf);
        }
      }
    }
  }
  g_free(data);
  return result;
}

Glib::ustring HttpClient::getServerHost() {
  return serverHost;
}

bool HttpClient::createTcpSocket() {
  appInstance->logEvent("HttpClient::createTcpSocket()", SEVERITY_DEBUG);
  if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    return false;
  }
  return true;
}

bool HttpClient::setDestinationAddress(sockaddr_in* address) {
  appInstance->logEvent("HttpClient::setDestinationAddress()", SEVERITY_DEBUG);
  gint s = socket(AF_INET, SOCK_STREAM, 0);
  if(s == -1) {
    return false;
  }
  hostent *h = gethostbyname(HOST.c_str());
  if(h == NULL) {
    return false;
  }
  ((gulong*) &((*address).sin_addr))[0] = ((gulong**) h->h_addr_list)[0][0];
  address->sin_family = AF_INET;
  address->sin_port = htons(PORT);
  return true;
}

gint HttpClient::recvAll(char* buf, guint len) {
  appInstance->logEvent("HttpClient::recvAll()", SEVERITY_DEBUG);
  gint total = 0;
  gint n;
  while(total < len) {
    n = recv(sock, buf + total, len - total, 0);
    if(n <= 0) {
      connected = false;
      break;
    }
    total += n;
  }
  return total;
}

void HttpClient::disconnect() {
  appInstance->logEvent("HttpClient::disconnect()", SEVERITY_DEBUG);
  #ifdef G_OS_WIN32
  closesocket(sock);
  #else
  close(sock);
  #endif
  connected = false;
}
