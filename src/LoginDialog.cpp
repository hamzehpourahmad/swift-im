/*
 *      LoginDialog.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#include "LoginDialog.h"
#include "Application.h"

using namespace Swift;


LoginDialog::LoginDialog(BaseObjectType* baseObject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade) : Gtk::Dialog(baseObject) {
  appInstance->logEvent("LoginDialog::LoginDialog()", SEVERITY_DEBUG);
  // get widgets
  appInstance->xml->get_widget("loginButton", loginButton);
  appInstance->xml->get_widget("cancelLoginButton", cancelButton);
  appInstance->xml->get_widget("loginEntry", loginEntry);
  appInstance->xml->get_widget("passwordEntry", passwordEntry);

  // connecting signal handlers
  loginButton->signal_clicked().connect(sigc::mem_fun(*this, &LoginDialog::loginButtonOnClicked));
  cancelButton->signal_clicked().connect(sigc::mem_fun(*this, &LoginDialog::cancelButtonOnClicked));
  loginEntry->signal_activate().connect(sigc::mem_fun(*this, &LoginDialog::loginButtonOnClicked));
  passwordEntry->signal_activate().connect(sigc::mem_fun(*this, &LoginDialog::loginButtonOnClicked));
};

void LoginDialog::cancelButtonOnClicked() {
  hide();
}

void LoginDialog::loginButtonOnClicked() {
  appInstance->logEvent("LoginDialog::loginButtonOnClicked()", SEVERITY_DEBUG);
  hide();
  Glib::ustring login, password;
  login = loginEntry->get_text();
  password = passwordEntry->get_text();
  if(login.empty() || password.empty()) {
    return;
  }
  if(!appInstance->mConnection->connect()) {
    appInstance->showMessage("Connection error", "Cannot establish connection to server\n", "I can't connect to server. Are you really connected to the Internet?", Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
  }
  else {
    guint32 startupStatus;
    startupStatus = appInstance->mainWindow->statusCombo->getStatusCode();
    if(startupStatus == STATUS_OFFLINE) {
      startupStatus = STATUS_ONLINE;
    }
    appInstance->mUser->setStatus(startupStatus);
    appInstance->mUser->setAddress(login);
    appInstance->mUser->authorize(password);
  }
}

void LoginDialog::resetEntries() {
  appInstance->logEvent("LoginDialog::resetEntries()", SEVERITY_DEBUG);
  loginEntry->set_text("");
  passwordEntry->set_text("");
}
