/*
 *      Utils.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#include <map>
#include <iostream>

#include <glib/gstdio.h>

#include "Utils.h"
#include "Application.h"

using namespace Swift;

Utils::Utils() {

}

Utils::~Utils() {
  // We don't need to free status images, because we're using RefPtr for storing it

}

Glib::RefPtr<Gdk::Pixbuf> Utils::getStatusImage(guint32 statusCode) {
  Glib::RefPtr<Gdk::Pixbuf> result;
  std::map <guint32, Glib::RefPtr<Gdk::Pixbuf> >::iterator it = statusImages.find(statusCode);
  if(it != statusImages.end()) {
    result = it->second;
  }
  else {
    result = statusImages[STATUS_UNDETERMINATED];
  }
  return result;
}

void Utils::loadStatusImages() {
  /*
   * Loading status images
   */
  std::vector<std::string> parts(4);
  std::map<guint32, std::string> statuses;
  std::map<guint32, std::string>::iterator it;
  parts.push_back(Application::getAppInstance()->getVariable("SWIFTIM_DATA_DIR"));
  parts.push_back("img");
  parts.push_back("status");
  statuses[STATUS_ONLINE] = "online.png";
  statuses[STATUS_OFFLINE] = "offline.png";
  statuses[STATUS_FLAG_INVISIBLE] = "invisible.png";
  statuses[STATUS_AWAY] = "away.png";
  statuses[STATUS_UNDETERMINATED] = "undeterminated.png";
  for(it = statuses.begin(); it != statuses.end(); it++) {
    parts.push_back(it->second);
    try {
      statusImages[it->first] = Gdk::Pixbuf::create_from_file(Glib::build_filename(parts));
    }
    catch(Glib::FileError& err) {
      std::cerr << "Error loading status image. Glib::FileError description: " << err.what() << std::endl;
    }
    catch(Gdk::PixbufError& err) {
      std::cerr << "Error loading status image. Gdk::PixbufError code: " << err.code() << std::endl;
    }
    parts.pop_back();
  }
}

Glib::ustring Utils::trimSlashes(Glib::ustring s) {
  Glib::ustring r = s;
  if(r[0] == '/' || r[0] == '\\') {
    r.erase(0, 1);
  }
  if(r[r.length() - 1] == '/' || r[r.length() - 1] == '\\') {
    r.erase(r.length() - 1, 1);
  }
  return r;
}

bool Utils::createDir(Glib::ustring path, int mode) {
  if(Glib::file_test(path, Glib::FILE_TEST_IS_DIR)) {
    return true;
  }
  return g_mkdir(path.c_str(), mode) == 0;
}
