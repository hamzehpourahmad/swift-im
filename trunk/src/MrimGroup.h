/*
 *      MrimGroup.h - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#ifndef MrimGroup_h
#define MrimGroup_h

#include <map>
#include <vector>

#include <glibmm/ustring.h>

#include "MrimContact.h"

namespace Swift {
  class MrimGroup;
};

namespace Swift {
  /* "Not authorized" groups must be created manually.
   * This const defines index of such group.
   * Contacts, which have CONTACT_INTFLAG_NOT_AUTHORIZED in their server flags
   * are considered as unauthorized and should be added to this group
   */
  const guint32 GROUP_INDEX_NOT_AUTHORIZED = 1 << 15;

  // guint32 are indexes of group, so we can access to group with index X by writing groupList[X]
  typedef std::map<guint32, MrimGroup> GroupList;
  typedef std::vector<MrimContact> ContactList;
  class MrimGroup {
    public:
      MrimGroup();
      MrimGroup(guint32 flags, Glib::ustring name);
      guint32 getFlags();
      Glib::ustring getName();
      void setFlags(guint32 flags);
      void setName(Glib::ustring name);
      ContactList* contacts();
      gint onlineCount();
      void addContact(MrimContact c);
      void removeContact(guint32 contactIndex);
      void debugPrint();

    private:
      guint32 mFlags;
      Glib::ustring mName;
      ContactList mCl;
  };
};

#endif //MrimGroup_h
