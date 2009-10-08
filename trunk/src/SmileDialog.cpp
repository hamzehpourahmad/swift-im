/*
 *      SmileDialog.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#include "Application.h"
#include "SmileDialog.h"

using namespace Swift;


SmileDialog::SmileDialog(BaseObjectType* pBaseObject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade) : Gtk::Window(pBaseObject) {
  appInstance->logEvent("SmileDialog::SmileDialog()", SEVERITY_DEBUG);
  appInstance->xml->get_widget("table", table);
  appInstance->xml->get_widget("closeWindow", closeWindow);
  appInstance->xml->get_widget("smileNumber", smileNumber);
  Gtk::Adjustment* adjt = smileNumber->get_adjustment();
  adjt->configure(1.0, 1.0, 10.0, 1.0, 1.0, 0);
  signal_hide().connect(sigc::mem_fun(*this, &SmileDialog::onHide));
  std::map <Glib::ustring, Glib::RefPtr<Gdk::PixbufAnimation> > smiles = appInstance->getSmilesAll();
  guint num = smiles.size();
  guint cols = 10;
  guint rows = num / cols + ((num % cols) ? 1 : 0);
  table->resize(rows, cols);
  guint i = 0, j = 0;
  for(std::map <Glib::ustring, Glib::RefPtr<Gdk::PixbufAnimation> >::iterator it = smiles.begin(); it != smiles.end(); it++, j++) {
    if(j % cols == 0) {
      j = 0;
      ++i;
    }
    Gtk::Image* img = manage(new Gtk::Image(it->second));
    Gtk::Button* btn = manage(new Gtk::Button);
    btn->add(*img);
    btn->set_relief(Gtk::RELIEF_NONE);
    btn->set_focus_on_click(false);
    btn->signal_clicked().connect(sigc::bind<Glib::ustring>(sigc::mem_fun(*this, &SmileDialog::onSmileClicked), it->first));
    table->attach(*btn, i, i  + 1, j, j + 1, Gtk::EXPAND, Gtk::EXPAND);
  }
  table->show_all_children();
}

void SmileDialog::onHide() {
  appInstance->chatWindow->smileButton->set_active(false);
}

void SmileDialog::onSmileClicked(Glib::ustring smileId) {
  ChatTab current = appInstance->chatWindow->chatTabs->getCurrentTab();
  gint num = smileNumber->get_value_as_int();
  for(gint i = 0; i < num; i++) current.messageText->insertSmile(smileId);
  if(closeWindow->get_active()) {
    hide();
  }
}
