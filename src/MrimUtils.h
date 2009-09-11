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

#include <glibmm/ustring.h>
#include <gdkmm/pixbuf.h>

namespace Swift {
  class MrimUtils;
};

namespace Swift {
  class MrimUtils {
    public:
      static Glib::ustring getContactStatusByCode(guint32 statusCode);
      static Glib::ustring getMessageStatusByCode(guint32 statusCode);
      static Glib::RefPtr<Gdk::Pixbuf> prepareAvatar(Glib::ustring address);
    private:
      static Glib::RefPtr<Gdk::Pixbuf> getCachedAvatar(Glib::ustring address);
      static void cacheAvatar(Glib::ustring address, Glib::RefPtr<Gdk::Pixbuf> pb);
  };
};

#endif //MrimUtils_h
