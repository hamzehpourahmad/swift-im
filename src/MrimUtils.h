/*
 *      MrimUtils.h - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#ifndef MrimUtils_h
#define MrimUtils_h

#include "MrimClient.h"

namespace Swift {
  const guint MRIMUTILS_CONTACT_NOT_FOUND = 0;
  class MrimUtils {
    public:
      static MrimContact getContactByIndex(MrimClient* client, guint32 index);
      static MrimContact getContactByAddress(MrimClient* client, Glib::ustring address);
      static Glib::ustring getContactStatusByCode(guint32 statusCode);
      static Glib::ustring getMessageStatusByCode(guint32 statusCode);
      static Gtk::Image* prepareAvatar(Glib::ustring address);
    private:
      static bool getCachedAvatar(Glib::ustring address, Gtk::Image* img);
      static void cacheAvatar(Glib::ustring address, Gtk::Image* img);
  };
};

#endif //MrimUtils_h
