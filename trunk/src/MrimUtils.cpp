/*
 *      MrimUtils.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#include <string>

#include "MrimUtils.h"
#include "Application.h"
#include "Utils.h"

using namespace Swift;

Glib::ustring MrimUtils::getContactStatusByCode(guint32 statusCode) {
  if(statusCode & STATUS_FLAG_INVISIBLE) return _("Invisible");
  switch(statusCode) {
    case STATUS_OFFLINE:
      return _("Offline");
    case STATUS_ONLINE:
      return _("Online");
    case STATUS_AWAY:
      return _("Away");
    case STATUS_UNDETERMINATED:
      return _("Undeterminated");
  }
  return _("Unknown");
}

Glib::ustring MrimUtils::getMessageStatusByCode(guint32 statusCode) {
  switch(statusCode) {
    case MESSAGE_DELIVERED:
      return _("Message delivered");
    case MESSAGE_REJECTED_INTERR:
      return _("Internal server error");
    case MESSAGE_REJECTED_NOUSER:
      return _("Recipient user doesn't exist");
    case MESSAGE_REJECTED_LIMIT_EXCEEDED:
      return _("Recipient user is offline and message is too large to put it into mailbox");
    case MESSAGE_REJECTED_TOO_LARGE:
      return _("Message size exceeded limits");
    case MESSAGE_REJECTED_DENY_OFFMSG:
      return _("Recipient user is offline and his mailbox settings don't allow offline messages");
  }
  return _("Unknown");
}

Glib::RefPtr<Gdk::Pixbuf> MrimUtils::prepareAvatar(Glib::ustring address) {
  appInstance->logEvent("MrimUtils::prepareAvatar()", SEVERITY_DEBUG);
  // try to get from cache
  std::string addr = address;
  Glib::RefPtr<Gdk::Pixbuf> result = getCachedAvatar(addr);
  if (!result) {
    // image isn't cached
    // by default, use 'NO AVATAR' image if avatar isn't present
    result = Gdk::Pixbuf::create_from_file(appInstance->getVariable("SWIFTIM_DATA_DIR") + G_DIR_SEPARATOR + "img" + G_DIR_SEPARATOR + "noavatar.png");
    // check whether avatar is present
    if (appInstance->http->checkAvatar(addr)) {
      // get avatar via HTTP
      result = appInstance->http->loadAvatar(addr, AVATAR_BIG);
      // cache avatar (i.e. save it on disk)
      cacheAvatar(addr, result);
    }
  }
  return result;
}


Glib::RefPtr<Gdk::Pixbuf> MrimUtils::getCachedAvatar(Glib::ustring address) {
  appInstance->logEvent("MrimUtils::getCachedAvatar()", SEVERITY_DEBUG);
  Glib::ustring avatarFilePath = appInstance->getVariable("SWIFTIM_USER_DATA_DIR") + G_DIR_SEPARATOR + address + G_DIR_SEPARATOR + "avatar";
  Glib::RefPtr<Gdk::Pixbuf> result;
  if (Glib::file_test(avatarFilePath, Glib::FILE_TEST_EXISTS)) {
    try {
      result = Gdk::Pixbuf::create_from_file(avatarFilePath);
    } catch (Glib::FileError& err) {
      appInstance->logEvent("Error loading avatar. " + err.what(), SEVERITY_WARNING);
    } catch (Gdk::PixbufError& err) {
      appInstance->logEvent("Error loading avatar. " + err.code(), SEVERITY_WARNING);
    }
  }
  return result;
}

void MrimUtils::cacheAvatar(Glib::ustring address, Glib::RefPtr<Gdk::Pixbuf> pb) {
  appInstance->logEvent("MrimUtils::cacheAvatar()", SEVERITY_DEBUG);
  Glib::ustring dir = appInstance->getVariable("SWIFTIM_USER_DATA_DIR") + G_DIR_SEPARATOR + address;
  if (Utils::createDir(dir)) {
    try {
      pb->save(dir + G_DIR_SEPARATOR + "avatar", "png");
    } catch (Glib::FileError& err) {
      appInstance->logEvent("Error saving avatar. " + err.what(), SEVERITY_WARNING);
    } catch (Gdk::PixbufError& err) {
      appInstance->logEvent("Error saving avatar. " + err.code(), SEVERITY_WARNING);
    }
  }
}

std::pair<std::string, std::string> MrimUtils::splitAddress(std::string contactAddress) {
  appInstance->logEvent("MrimUtils::splitAddress()", SEVERITY_DEBUG);
  std::string user, domain;
  gint p = contactAddress.find("@");
  if(p != std::string::npos) {
    user = contactAddress.substr(0, p);
    gint point = contactAddress.rfind(".");
    domain = contactAddress.substr(p+1, point - p - 1);
  }
  return std::make_pair(user, domain);
}
