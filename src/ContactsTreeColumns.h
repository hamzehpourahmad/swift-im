/*
 *      ContactsTreeColumns.h - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#ifndef ContactsTreeColumns_h
#define ContactsTreeColumns_h

namespace Swift {
  class ContactsTreeColumns;
};

#include <gtkmm/treemodel.h>
#include <gtkmm/image.h>

namespace Swift {
  class ContactsTreeColumns : public Gtk::TreeModel::ColumnRecord {
    public:
      ContactsTreeColumns();
      Gtk::TreeModelColumn <guint32> contactIndex;
      Gtk::TreeModelColumn <Glib::ustring> contactAddress;
      Gtk::TreeModelColumn <Glib::ustring> contactNickname;
      Gtk::TreeModelColumn <guint32> contactStatus;
      Gtk::TreeModelColumn < Glib::RefPtr<Gdk::Pixbuf> > contactAvatar;
      Gtk::TreeModelColumn < Glib::RefPtr<Gdk::Pixbuf> > statusImage;
  };
};

#endif //ContactsTreeColumns_h
