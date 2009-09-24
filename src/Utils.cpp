/*
 *      Utils.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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
#include <sstream>

#include <glib/gstdio.h>

#include "Utils.h"
#include "Application.h"


using namespace Swift;

bool Utils::createDir(Glib::ustring path, int mode) {
  if(Glib::file_test(path, Glib::FILE_TEST_IS_DIR)) {
    return true;
  }
  return g_mkdir(path.c_str(), mode) == 0;
}

void Utils::openUri(std::string uri) {
  appInstance->logEvent("Utils::openUri()", SEVERITY_DEBUG);
#ifdef G_OS_WIN32
  ShellExecute(NULL, "open", uri.c_str(), NULL, NULL, SW_SHOWNORMAL);
#else
  std::vector<std::string> argv(2);
  argv[0] = "xdg-open";
  argv[1] = uri;
  Glib::spawn_async("./", argv, Glib::SPAWN_SEARCH_PATH);
#endif
}

