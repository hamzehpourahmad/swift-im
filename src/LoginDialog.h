/*
 *      LoginDialog.h - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#ifndef LoginDialog_h
#define LoginDialog_h

namespace Swift {
  class LoginDialog;
};

#include <string>

#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/button.h>
#include <gtkmm/treestore.h>
#include <gtkmm/combobox.h>
#include <libglademm.h>

namespace Swift {
  const std::string domains[4] = {
    "@mail.ru",
    "@list.ru",
    "@bk.ru",
    "@inbox.ru"
  };
  class DomainComboColumns : public Gtk::TreeModel::ColumnRecord {
    public:
      Gtk::TreeModelColumn <gint> domainIndex;
      Gtk::TreeModelColumn <std::string> domainTitle;
      DomainComboColumns() {
        add(domainIndex);
        add(domainTitle);
      };
  };
  
  class LoginDialog : public Gtk::Dialog {
    public:
      LoginDialog(BaseObjectType* baseObject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
      void resetEntries();
      Gtk::Button *loginButton;
      Gtk::Button *cancelButton;
      Gtk::Entry *loginEntry;
      Gtk::Entry *passwordEntry;
      Gtk::ComboBox* domainCombo;
      
    private:
      Glib::RefPtr<Gtk::TreeStore> mdlCombo;
      DomainComboColumns columns;
      
    protected:
      // signal handlers
      void loginButtonOnClicked();
      void cancelButtonOnClicked();
  };
};

#endif //LoginDialog_h
