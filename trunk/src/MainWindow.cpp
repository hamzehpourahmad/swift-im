/*
 *      MainWindow.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#include "Application.h"
#include "MainWindow.h"
#include "ContactsTreeColumns.h"

using namespace Swift;

MainWindow::MainWindow(BaseObjectType* pBaseObject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade) : Gtk::Window(pBaseObject) {
  Gtk::ImageMenuItem *menu;
  // init widgets
  Application::getAppInstance()->xml->get_widget_derived("contactsTree", contactsTree);
  Application::getAppInstance()->xml->get_widget_derived("statusCombo", statusCombo);
  Application::getAppInstance()->xml->get_widget("userAvatarImage", userAvatarImage);
  Application::getAppInstance()->xml->get_widget("usernameLabel", usernameLabel);
  Application::getAppInstance()->xml->get_widget("mailStatusLabel", mailStatusLabel);

  // connecting signals
  Application::getAppInstance()->xml->get_widget("menuItemExit", menu);
  menu->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::menuItemExitOnActivate));
  Application::getAppInstance()->xml->get_widget("menuItemAbout", menu);
  menu->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::menuItemAboutOnActivate));
}

void MainWindow::menuItemExitOnActivate() {
  Application::quit();
}

void MainWindow::menuItemAboutOnActivate() {
  Application::getAppInstance()->aboutDialog->show();
}
