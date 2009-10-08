/*
 *      SmileDialog.h - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#ifndef SmileDialog_h
#define SmileDialog_h

namespace Swift {
  class SmileDialog;
};

#include <gtkmm/window.h>
#include <gtkmm/table.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/checkbutton.h>
#include <libglademm.h>

namespace Swift {
  class SmileDialog : public Gtk::Window {
    public:
      SmileDialog(BaseObjectType* baseObject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);

    private:
      Gtk::Table *table;
      Gtk::CheckButton *closeWindow;
      Gtk::SpinButton *smileNumber;

    protected:
      // signal handlers
      void onHide();
      void onSmileClicked(Glib::ustring smileId);
  };
};

#endif //SmileDialog_h
