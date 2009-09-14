/*
 *      AboutDialog.h - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#ifndef AboutDialog_h
#define AboutDialog_h

namespace Swift {
  class AboutDialog;
};

#include <gtkmm/dialog.h>
#include <libglademm.h>

namespace Swift {
  class AboutDialog : public Gtk::Dialog {
    public:
      AboutDialog(BaseObjectType* baseObject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);

    private:
      Gtk::Label *title, *link, *description, *caption1, *caption2;
      Gtk::Button *closeBtn, *linkBtn;
    protected:
      void onCloseClicked();
      void onLinkClicked();
  };
};

#endif //AboutDialog_h
