/*
 *      MrimSignalServer.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#include "MrimSignalServer.h"

using namespace Swift;

SignalContactListReceive MrimSignalServer::signal_contact_list_receive() {
  return mSignalContactListReceive;
}

SignalUserInfoReceive MrimSignalServer::signal_user_info_receive() {
  return mSignalUserInfoReceive;
}

SignalUserStatus MrimSignalServer::signal_user_status() {
  return mSignalUserStatus;
}

SignalLogout MrimSignalServer::signal_logout() {
  return mSignalLogout;
}

SignalMessageStatus MrimSignalServer::signal_message_status() {
  return mSignalMessageStatus;
}

SignalMessageReceive MrimSignalServer::signal_message_receive() {
  return mSignalMessageReceive;
}

SignalConnectionParams MrimSignalServer::signal_connection_params() {
  return mSignalConnectionParams;
}

SignalHelloAck MrimSignalServer::signal_hello_ack() {
  return mSignalHelloAck;
}

SignalLoginAck MrimSignalServer::signal_login_ack() {
  return mSignalLoginAck;
}

SignalLoginRej MrimSignalServer::signal_login_rej() {
  return mSignalLoginRej;
}
