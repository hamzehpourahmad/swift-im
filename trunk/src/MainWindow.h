/*
 *      MainWindow.h - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#ifndef MainWindow_h
#define MainWindow_h

#include <gtkmm/window.h>
#include <libglademm.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/treeview.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/label.h>

#include "ContactsTreeWidget.h"
#include "StatusComboWidget.h"

namespace Swift {
  class MainWindow : public Gtk::Window {
    public:
      MainWindow(BaseObjectType* baseObject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
      ContactsTreeWidget* contactsTree;
      Gtk::Label *usernameLabel, *mailStatusLabel;
      Gtk::Image* userAvatarImage;
      StatusComboWidget* statusCombo;

    protected:
      // signal handlers
      void menuItemExitOnActivate();
      void menuItemAboutOnActivate();
  };
};

#endif //MainWindow_h
