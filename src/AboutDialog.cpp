/*
 *      AboutDialog.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#include "AboutDialog.h"
#include "Application.h"
#include "Utils.h"

using namespace Swift;


AboutDialog::AboutDialog(BaseObjectType* baseObject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade) : Gtk::Dialog(baseObject) {
  appInstance->logEvent("AboutDialog::AboutDialog()", SEVERITY_DEBUG);
  // get widgets
  appInstance->xml->get_widget("title", title);
  appInstance->xml->get_widget("linkButton", linkBtn);
  appInstance->xml->get_widget("link", link);
  appInstance->xml->get_widget("description", description);
  appInstance->xml->get_widget("caption1", caption1);
  appInstance->xml->get_widget("caption2", caption2);
  appInstance->xml->get_widget("linkButton", linkBtn);
  appInstance->xml->get_widget("closeButton", closeBtn);

  // setting properties
  title->set_use_markup(true);
  title->set_label("<span font=\"16\" weight=\"bold\">" + Glib::get_application_name() + " " + appInstance->getVersion() + "</span>");
  title->set_alignment(Gtk::ALIGN_CENTER);
  title->set_line_wrap(true);
  linkBtn->set_relief(Gtk::RELIEF_NONE);
  linkBtn->set_focus_on_click(false);
  link->set_use_markup(true);
  link->set_label("<span color=\"#0000FF\" weight=\"bold\">" + appInstance->getVariable("SWIFTIM_HOMEPAGE") + "</span>");
  link->set_line_wrap(true);
  description->set_use_markup(true);
  description->set_alignment(Gtk::ALIGN_CENTER);
  description->set_line_wrap(true);
  description->set_label(
  "<span weight=\"bold\">Cross-platform IM client for Mail.ru</span>\n\n"
  "Copyright (c) 2009 Кожаев Галымжан\n"
  "&lt;kozhayev(at)gmail(dot)com&gt;"
  );
  caption1->set_label("Information");
  caption2->set_label("License");

  // connecting signals
  closeBtn->signal_clicked().connect(sigc::mem_fun(*this, &AboutDialog::onCloseClicked));
  linkBtn->signal_clicked().connect(sigc::mem_fun(*this, &AboutDialog::onLinkClicked));
};

void AboutDialog::onCloseClicked() {
  hide();
}

void AboutDialog::onLinkClicked() {
  Utils::openUri(appInstance->getVariable("SWIFTIM_HOMEPAGE"));
}
