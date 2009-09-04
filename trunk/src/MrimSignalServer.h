/*
 *      MrimSignalServer.h - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#ifndef MrimSignalServer_h
#define MrimSignalServer_h

#include <glibmm/ustring.h>
#include <sigc++/sigc++.h>

namespace Swift {
  typedef sigc::signal<void> SignalContactListReceive;
  typedef sigc::signal<void> SignalUserInfoReceive;
  typedef sigc::signal<void, guint32, Glib::ustring> SignalUserStatus;
  typedef sigc::signal<void, guint32> SignalLogout;
  typedef sigc::signal<void, guint32, guint32> SignalMessageStatus;
  typedef sigc::signal<void, guint32> SignalMessageSendTimeout;
  typedef sigc::signal<void, guint32, Glib::ustring, Glib::ustring, Glib::ustring> SignalMessageReceive;
  class MrimSignalServer {
    public:
      MrimSignalServer(){};
      ~MrimSignalServer(){};
      SignalContactListReceive signal_contact_list_receive();
      SignalUserInfoReceive signal_user_info_receive();
      SignalUserStatus signal_user_status();
      SignalLogout signal_logout();
      SignalMessageStatus signal_message_status();
      SignalMessageReceive signal_message_receive();
      SignalMessageSendTimeout signal_message_send_timeout();

    protected:
      SignalContactListReceive mSignalContactListReceive;
      SignalUserInfoReceive mSignalUserInfoReceive;
      SignalUserStatus mSignalUserStatus;
      SignalLogout mSignalLogout;
      SignalMessageStatus mSignalMessageStatus;
      SignalMessageSendTimeout mSignalMessageSendTimeout;
      SignalMessageReceive mSignalMessageReceive;
  };
};

#endif //MrimSignalServer_h
