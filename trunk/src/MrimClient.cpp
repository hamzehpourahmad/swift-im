/*
 *      MrimClient.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
 *
 *      Copyright (c) 2009 Галымжан Кожаев <kozhayev(at)gmail(dot)com>
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
  appInstance->logEvent("MrimClient::MrimClient()", SEVERITY_DEBUG);
  userAgentString = "Swift-IM v0.2";
}

bool MrimClient::sendMessage(guint32 flags, Glib::ustring to, Glib::ustring message, Glib::ustring rtf, guint32 *messageId) {
  appInstance->logEvent("MrimClient::sendMessage()", SEVERITY_DEBUG);
  if(message.empty()) {
    message = " ";
  }
  if(rtf.empty()) {
    rtf = " ";
  }
  MrimPacket p(MRIM_CS_MESSAGE);
  p.addUL(flags);
  p.addLPS(to);
  p.addLPS(message);
  p.addLPS(rtf);
  bool result = p.send();
  *messageId = 0;
  if(result) {
    mrim_packet_header_t h = p.getHeader();
    *messageId = h.seq;
  }
  return result;
}

bool MrimClient::login(Glib::ustring password) {
  appInstance->logEvent("MrimClient::login()", SEVERITY_DEBUG);
  MrimPacket p(MRIM_CS_LOGIN2);
  p.addLPS(appInstance->mUser->getAddress());
  p.addLPS(password);
  p.addUL(appInstance->mUser->getStatus());
  p.addLPS(userAgentString);
  return p.send();
}

bool MrimClient::sendMessageRecv(Glib::ustring from, guint32 messageId) {
  appInstance->logEvent("MrimClient::sendMessageRecv()", SEVERITY_DEBUG);
  MrimPacket p(MRIM_CS_MESSAGE_RECV);
  p.addLPS(from);
  p.addUL(messageId);
  return p.send();
}

bool MrimClient::changeStatus(guint32 newStatus) {
  appInstance->logEvent("MrimClient::changeStatus()", SEVERITY_DEBUG);
  if(newStatus == STATUS_UNDETERMINATED || newStatus == STATUS_OFFLINE)  {
    return false;
  }
  if(newStatus == STATUS_FLAG_INVISIBLE) {
    newStatus = STATUS_ONLINE | STATUS_FLAG_INVISIBLE;
  }
  MrimPacket p(MRIM_CS_CHANGE_STATUS);
  p.addUL(newStatus);
  return p.send();
}
