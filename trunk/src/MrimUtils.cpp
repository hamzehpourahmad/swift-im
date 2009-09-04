/*
 *      MrimUtils.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#include <iostream>
#include <string>

#include "Application.h"
#include "MrimUtils.h"

using namespace Swift;

MrimContact MrimUtils::getContactByIndex(MrimClient* client, guint32 index) {
  ContactList::iterator it;
  MrimContact result;
  result.index = MRIMUTILS_CONTACT_NOT_FOUND;
  for(it = client->contactList.begin(); it != client->contactList.end(); it++) {
    if(it->index == index) {
      result = *(it);
      break;
    }
  }
  return result;
}

MrimContact MrimUtils::getContactByAddress(MrimClient* client, Glib::ustring address) {
  ContactList::iterator it;
  MrimContact result;
  result.index = MRIMUTILS_CONTACT_NOT_FOUND;
  for(it = client->contactList.begin(); it != client->contactList.end(); it++) {
    if(it->address == address) {
      result = *(it);
      break;
    }
  }
  return result;
}

Glib::ustring MrimUtils::getContactStatusByCode(guint32 statusCode) {
  switch(statusCode) {
    case STATUS_OFFLINE:
      return "Offline";
    case STATUS_ONLINE:
      return "Online";
    case STATUS_AWAY:
      return "Away";
    case STATUS_UNDETERMINATED:
      return "Undeterminated";
    case STATUS_FLAG_INVISIBLE:
      return "Invisible";
  }
  return "Unknown";
}

Glib::ustring MrimUtils::getMessageStatusByCode(guint32 statusCode) {
  switch(statusCode) {
    case MESSAGE_DELIVERED:
      return "Message delivered";
    case MESSAGE_REJECTED_INTERR:
      return "Internal server error";
    case MESSAGE_REJECTED_NOUSER:
      return "User recipient doesn't exist";
    case MESSAGE_REJECTED_LIMIT_EXCEEDED:
      return "User recipient is offline and message is too large to store it into mailbox";
    case MESSAGE_REJECTED_TOO_LARGE:
      return "Message size exceeded limits";
    case MESSAGE_REJECTED_DENY_OFFMSG:
      return "User recipient is offline and his mailbox settings doesn't allow offline messages";
  }
  return "Unknown";
}

Gtk::Image* MrimUtils::prepareAvatar(Glib::ustring address) {
  Gtk::Image* img = new Gtk::Image();
  std::string addr = address;
  Application* app = Application::getAppInstance();
  // try to get from cache
  if (!getCachedAvatar(addr, img)) {
    // image isn't cached
    // by default, use 'NO AVATAR' image if avatar isn't present
    img->set(Gdk::Pixbuf::create_from_file(app->getVariable("SWIFTIM_DATA_DIR") + G_DIR_SEPARATOR + "img" + G_DIR_SEPARATOR + "noavatar.png"));
    // check whether avatar is present
    if (Application::getAppInstance()->http.checkAvatar(addr)) {
      // get avatar via HTTP
      Application::getAppInstance()->http.loadAvatar(addr, img, AVATAR_BIG);
      // cache avatar (i.e. save it on disk)
      cacheAvatar(addr, img);
    }
  }
  return img;
}


bool MrimUtils::getCachedAvatar(Glib::ustring address, Gtk::Image* img) {
  Glib::ustring avatarFilePath = Application::getAppInstance()->getVariable("SWIFTIM_USER_DATA_DIR") + G_DIR_SEPARATOR + address + G_DIR_SEPARATOR + "avatar";
  if (Glib::file_test(avatarFilePath, Glib::FILE_TEST_EXISTS)) {
    bool ok;
    try {
      Glib::RefPtr<Gdk::Pixbuf> pb = Gdk::Pixbuf::create_from_file(avatarFilePath);
      img->set(pb);
      ok = true;
    } catch (Glib::FileError& err) {
      std::cerr << "Error loading avatar. " << err.what() << std::endl;
      ok = false;
    } catch (Gdk::PixbufError& err) {
      std::cerr << "Error loading avatar. " << err.code() << std::endl;
      ok = false;
    }
    return ok;
  }
  return false;
}

void MrimUtils::cacheAvatar(Glib::ustring address, Gtk::Image* img) {
  Glib::ustring dir = Application::getAppInstance()->getVariable("SWIFTIM_USER_DATA_DIR") + G_DIR_SEPARATOR + address;
  if (Application::getAppInstance()->utils.createDir(dir)) {
    try {
      img->get_pixbuf()->save(dir + G_DIR_SEPARATOR + "avatar", "png");
    } catch (Glib::FileError& err) {
      std::cerr << "Error saving avatar. " << err.what() << std::endl;
    } catch (Gdk::PixbufError& err) {
      std::cerr << "Error saving avatar. " << err.code() << std::endl;
    }
  }
}
