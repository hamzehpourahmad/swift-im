/*
 *      MrimClient.h - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#ifndef MrimClient_h
#define MrimClient_h

namespace Swift {
  class MrimClient;
};

#include "MrimPacket.h"
#include "MrimConnection.h"

namespace Swift {
  class MrimClient {
    public:
      // methods
      MrimClient();
      bool login(Glib::ustring password);
      bool changeStatus(guint32 newStatus);
      bool sendMessage(guint32 flags, Glib::ustring to, Glib::ustring message, Glib::ustring rtf, guint32 *messageId);
      bool sendMessageRecv(Glib::ustring from, guint32 messageId);

    private:
      Glib::ustring userAgentString;
  };
};

#endif //MrimClient_h
