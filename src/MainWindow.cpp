/*
 *      MainWindow.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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
#include "MainWindow.h"
#include "ContactsTreeColumns.h"
#include "MrimUtils.h"

using namespace Glib;
using namespace Swift;


MainWindow::MainWindow(BaseObjectType* pBaseObject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade) : Gtk::Window(pBaseObject) {
  appInstance->logEvent("MainWindow::MainWindow()", SEVERITY_DEBUG);
  Gtk::ImageMenuItem *menu;
  // get widgets
  appInstance->xml->get_widget_derived("contactsTree", contactsTree);
  appInstance->xml->get_widget_derived("statusCombo", statusCombo);
  appInstance->xml->get_widget("userAvatarImage", userAvatarImage);
  appInstance->xml->get_widget("usernameLabel", usernameLabel);
  appInstance->xml->get_widget("mailStatusLabel", mailStatusLabel);

  // connecting signal handlers
  appInstance->xml->get_widget("menuItemExit", menu);
  menu->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::menuItemExitOnActivate));
  appInstance->xml->get_widget("menuItemAbout", menu);
  menu->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::menuItemAboutOnActivate));
  appInstance->sigServer->signal_user_info_receive().connect(sigc::mem_fun(*this, &MainWindow::onUserInfoReceive));
}

void MainWindow::menuItemExitOnActivate() {
  Application::quit();
}

void MainWindow::menuItemAboutOnActivate() {
  appInstance->aboutDialog->present();
}

void MainWindow::onUserInfoReceive(UserInfo ui) {
  appInstance->logEvent("MainWindow::onUserInfoReceive()", SEVERITY_DEBUG);
  usernameLabel->set_label("<span size='medium' weight='bold'>" + ui["MRIM.NICKNAME"] + "</span>");
  mailStatusLabel->set_label("<span size='small' weight='bold'>" + ustring::compose(_("Messages unread: %1"), ui["MESSAGES.UNREAD"]) + "</span>");
  appInstance->mUser->setAvatar(MrimUtils::prepareAvatar(appInstance->mUser->getAddress()));
  userAvatarImage->set(appInstance->mUser->getAvatar());
}
