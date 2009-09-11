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

namespace Swift {
  class MrimSignalServer;
};

#include <glibmm/ustring.h>
#include <sigc++/sigc++.h>

#include "MrimLoggedUser.h"

namespace Swift {
  /* SignalContactListReceive emitted when MRIM_CS_CONTACT_LIST2 received.
   * Contacts tree should be updated.
   * Parameters:
   *  - GroupList gl - received group list.
   *  - ContactList cl - received contact list.
   * Handlers:
   *  - MrimLoggedUser::onContactListReceive()
   */
  typedef sigc::signal<void, GroupList, ContactList> SignalContactListReceive;
  
  /* SignalUserInfoReceive emitted when MRIM_CS_USER_INFO received.
   * User info (nickname, unread letter and etc) should be presented to user
   * Parameters:
   *  - UserInfo ui - received user info.
   * Handlers:
   *  - MrimLoggedUser::onUserInfoReceive()
   *  - MainWindow::onUserInfoReceive()
   */
  typedef sigc::signal<void, UserInfo> SignalUserInfoReceive;
  
  /* SignalUserStatus emitted when MRIM_CS_USER_STATUS received.
   * Contacts tree should be updated (change appropriate contact's status).
   * Parameters:
   *  guint32 status - new status
   *  Glib::ustring - email of contact, which has changed status
   * Handlers:
   *  - ChatTabs::onStatusChange()
   *  - MrimLoggedUser::onStatusChange()
   *  - ContactsTreeWidget::onStatusChange()
   */
  typedef sigc::signal<void, guint32, Glib::ustring> SignalUserStatus;
  
  /* SignalLogout emitted when MRIM_CS_LOGOUT received.
   * Connections should be closed.
   * Parameters:
   *  guint32 reason
   * Handlers:
   *  - MrimClient::onLogout()
   *  - MrimConnection::onLogout()
   */
  typedef sigc::signal<void, guint32> SignalLogout;
  
  /* SignalMessageStatus emitted when MRIM_CS_MESSAGE_STATUS received.
   * User should be noticed about message delivery status.
   * Parameters:
   *  guint32 messageId - Id of sended message
   *  guint32 status - message delivery status
   * Handlers:
   *  - ChatWindow::onMessageStatus()
   */
  typedef sigc::signal<void, guint32, guint32> SignalMessageStatus;
  
  /* SignalMessageReceive emitted when MRIM_CS_MESSAGE_ACK received.
   * Handlers should present new message to user or do other operations depending on flags. 
   * Also, MRIM_CS_MESSAGE_RECV should be sent, if flags don't contain MESSAGE_FLAG_NORECV.
   * Parameters:
   *  guint32 messagdId - Id of received message
   *  guint32 flags - message flags
   *  Glib::ustring from - sender address
   *  Glib::ustring message - message body in plain text
   *  Glib::ustring rtf - rtf-formatted version of message (only if MESSAGE_FLAG_RTF presents in flags)
   * Handlers:
   *  - ChatWindow::onMessageReceive()
   */
  typedef sigc::signal<void, guint32, guint32, Glib::ustring, Glib::ustring, Glib::ustring> SignalMessageReceive;
  
  /* SignalConnectionParams emitted when MRIM_CS_CONNECTION_PARAMS received.
   * Ping period should be updated.
   * Parameters:
   *  guint32 pingPeriod - new ping period
   * Handlers:
   *  - MrimConnection::onConnectionParamsReceive()
   */
  typedef sigc::signal<void, guint32> SignalConnectionParams;
  
  /* SignalHelloAck emitted when MRIM_CS_HELLO_ACK received.
   * Ping period should be updated.
   * Parameters:
   *  guint32 pingPeriod - initial ping period
   * Handlers:
   *  - MrimConnection::onHelloAck()
   */
  typedef sigc::signal<void, guint32> SignalHelloAck;
  
  /* SignalLoginAck emitted when MRIM_CS_LOGIN_ACK received.
   * Login succesfull.
   * Parameters:
   *  None
   * Handlers:
   *  - MrimLoggedUser::onLoginAck()
   */
  typedef sigc::signal<void> SignalLoginAck;
  
  /* SignalLoginRej emitted when MRIM_CS_LOGIN_REJ received.
   * Login unsuccesfull.
   * Parameters:
   *  Glib::ustring reason - reason of login reject.
   * Handlers:
   *  - MrimLoggedUser::onLoginRej()
   */
  typedef sigc::signal<void, Glib::ustring> SignalLoginRej;
  
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
      SignalConnectionParams signal_connection_params();
      SignalHelloAck signal_hello_ack();
      SignalLoginAck signal_login_ack();
      SignalLoginRej signal_login_rej();

    protected:
      SignalContactListReceive mSignalContactListReceive;
      SignalUserInfoReceive mSignalUserInfoReceive;
      SignalUserStatus mSignalUserStatus;
      SignalLogout mSignalLogout;
      SignalMessageStatus mSignalMessageStatus;
      SignalMessageReceive mSignalMessageReceive;
      SignalConnectionParams mSignalConnectionParams;
      SignalHelloAck mSignalHelloAck;
      SignalLoginAck mSignalLoginAck;
      SignalLoginRej mSignalLoginRej;
  };
};

#endif //MrimSignalServer_h
