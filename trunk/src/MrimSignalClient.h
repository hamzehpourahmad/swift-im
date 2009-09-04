/*
 *      MrimSignalClient.h - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#ifndef MrimSignalClient_h
#define MrimSignalClient_h

#include <glibmm/ustring.h>
#include <sigc++/sigc++.h>

namespace Swift {
  class MrimSignalClient : public sigc::trackable {
    public:
      // signal handlers
      virtual void onContactListReceive();
      virtual void onUserInfoReceive();
      virtual void onUserStatus(guint32 newStatus, Glib::ustring address);
      virtual void onLogout(guint32 logoutCode);
      virtual void onMessageStatus(guint32 msgId, guint32 status);
      virtual void onMessageReceive(guint32 flags, Glib::ustring from, Glib::ustring message, Glib::ustring rtf);
      virtual void onMessageSendTimeout(guint32 msgId);
  };
};

#endif //MrimSignalClient_h
