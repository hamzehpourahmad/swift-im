/*
 *      MrimContact.h - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#ifndef MrimContact_h
#define MrimContact_h

namespace Swift {
  class MrimContact;
};

#include <glibmm/refptr.h>
#include <gdkmm/pixbuf.h>

#include "MrimGroup.h"

namespace Swift {
  class MrimContact {
    public:
      MrimContact();
      MrimContact(guint32 index, guint32 flag, MrimGroup grp, Glib::ustring address, Glib::ustring nickname, guint32 serverFlags, guint32 status, Glib::RefPtr<Gdk::Pixbuf> avatar);
      void setIndex(guint32 index);
      void setFlag(guint32 flag);
      void setGroup(MrimGroup grp);
      void setAddress(Glib::ustring address);
      void setNickname(Glib::ustring nickname);
      void setServerFlags(guint32 flags);
      void setStatus(guint32 status);
      void setAvatar(Glib::RefPtr<Gdk::Pixbuf> avatar);
      guint32 getIndex();
      guint32 getFlag();
      MrimGroup getGroup();
      Glib::ustring getAddress();
      Glib::ustring getNickname();
      guint32 getServerFlags();
      guint32 getStatus();
      Glib::RefPtr<Gdk::Pixbuf> getAvatar();
    private:
      guint32 mIndex;
      guint32 mFlag;
      MrimGroup mGroup;
      Glib::ustring mAddress;
      Glib::ustring mNickname;
      guint32 mServerFlags;
      guint32 mStatus;
      Glib::RefPtr<Gdk::Pixbuf> mAvatar;
  };
};

#endif //MrimContact_h
