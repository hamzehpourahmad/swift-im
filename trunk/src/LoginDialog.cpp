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
  // init widgets
  Application::getAppInstance()->xml->get_widget("loginButton", loginButton);
  Application::getAppInstance()->xml->get_widget("cancelLoginButton", cancelButton);
  Application::getAppInstance()->xml->get_widget("loginEntry", loginEntry);
  Application::getAppInstance()->xml->get_widget("passwordEntry", passwordEntry);

  // connecting signals
  loginButton->signal_clicked().connect(sigc::mem_fun(*this, &LoginDialog::loginButtonOnClicked));
  cancelButton->signal_clicked().connect(sigc::mem_fun(*this, &LoginDialog::cancelButtonOnClicked));
  loginEntry->signal_activate().connect(sigc::mem_fun(*this, &LoginDialog::loginButtonOnClicked));
  passwordEntry->signal_activate().connect(sigc::mem_fun(*this, &LoginDialog::loginButtonOnClicked));
};

void LoginDialog::cancelButtonOnClicked() {
  hide();
}

void LoginDialog::loginButtonOnClicked() {
  hide();
  if(!Application::getAppInstance()->mClient.connect()) {
    Application::getAppInstance()->showMessage("Connection error", "Cannot establish connection to server\n", "I can't connect to server. Are you really connected to the Internet?", Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
  }
  else {
    Glib::ustring login, password, result;
    guint32 startupStatus;
    login = loginEntry->get_text();
    password = passwordEntry->get_text();
    startupStatus = Application::getAppInstance()->mainWindow->statusCombo->getStatusCode();
    if(startupStatus == STATUS_OFFLINE) {
      startupStatus = STATUS_ONLINE;
    }
    result = Application::getAppInstance()->mClient.login(login, password, startupStatus);
    if(result != "ok") {
      Application::getAppInstance()->showMessage("Login error", "Login unsuccessfull\n", "Reason: \n" + result, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
    }
    else {
      Application::getAppInstance()->mrimLogged = true;
    }
  }
}

void LoginDialog::resetEntries() {
  loginEntry->set_text("");
  passwordEntry->set_text("");
}
