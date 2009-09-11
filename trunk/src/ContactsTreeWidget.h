/*
 *      ContactsTreeWidget.h - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#ifndef ContactsTreeWidget_h
#define ContactsTreeWidget_h

/*
 * Represents contactlist tree widget.
 */
namespace Swift {
  class ContactsTreeWidget;
};

#include <map>

#include <gtkmm/treestore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treemodelsort.h>
#include <libglademm.h>

#include "Application.h"
#include "MrimClient.h"
#include "ContactsTreeColumns.h"

namespace Swift {
  const guint32 GROUP_INDEX_NOT_AUTHORIZED = 1 << 15;
  class ContactsTreeWidget : public Gtk::TreeView {
    public:
      ContactsTreeWidget(BaseObjectType* baseObject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
      ~ContactsTreeWidget();
      void addContact(MrimContact contact);
      void addGroup(MrimGroup group);
      void loadAvatars();
      void removeGroup(MrimGroup group);
      void removeContact(MrimContact contact);
      void updateStatus(MrimContact contact);
      static void loadContactList();
      ContactsTreeColumns columns;
      
    private:
      bool onSearchEqual(const Glib::RefPtr<Gtk::TreeModel>& model, gint column, const Glib::ustring& key, const Gtk::TreeModel::iterator& iter);
      Glib::RefPtr<Gdk::Pixbuf> resizeAvatar(Glib::RefPtr<Gdk::Pixbuf> src);
      std::map <guint32, Gtk::TreeModel::Row> addedGroups;
      std::map <guint32, bool> addedGroupsFlag;
      std::map <guint32, Gtk::TreeModel::Row> addedContacts;
      std::map <guint32, bool> addedContactsFlag;

    protected:
      Glib::RefPtr<Gtk::TreeStore> treeModel;
      // signal handlers
      // GUI signals
      void contactsTreeOnActivate(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn *column);
      
      // mrim signals
      void onStatusChange(guint32 status, Glib::ustring address);
  };
};

#endif //ContactsTreeWidget_h
