/*
 *      HttpClient.h - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#ifndef HttpClient_h
#define HttpClient_h

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <string>

#include <glibmm/main.h>
#include <glibmm/ustring.h>
#include <gtkmm/image.h>

#include "SocketHeaders.h"

namespace Swift {
  enum AvatarType {
    AVATAR_SMALL,
    AVATAR_BIG
  };
  const guint HTTP_BUFFER_SIZE = 80000;
  const Glib::ustring HOST = "obraz.foto.mail.ru";
  const gint PORT = 80;
  struct DetailedAddress {
    std::string domain;
    std::string user;
  };
  class HttpClient {
    public:
      HttpClient();
      ~HttpClient(){};
      void disconnect();
      bool loadAvatar(std::string contactAddress, Gtk::Image *img, AvatarType atype);
      bool checkAvatar(std::string contactAddress);
      DetailedAddress parseAddress(std::string contactAddress);
    private:
      bool connect(Glib::ustring host);
      gint recvAll(char* buf, guint len);
      bool createTcpSocket();
      bool setDestinationAddress(sockaddr_in* address);
      Glib::ustring getServerHost();
      gint sock;
      bool connected;
    protected:
      Glib::ustring serverHost;
  };
};

#endif //HttpClient_h
