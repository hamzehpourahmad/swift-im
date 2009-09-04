/*
 *      Application.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#include <vector>
#include <iostream>

#include <gtkmm/main.h>
#include <gtkmm/messagedialog.h>
#include <glibmm/ustring.h>
#include <glibmm/spawn.h>

#include "Application.h"
#include "SocketHeaders.h"
#include "Configure.h"

using namespace Swift;

Application* appInstance;

Application::Application() {
  mrimLogged = false;
  appInstance = this;
  initVariables();
  try {
    xml = Gnome::Glade::Xml::create(getVariable("SWIFTIM_DATA_DIR") + G_DIR_SEPARATOR + "ui.glade");
  }
  catch (const Gnome::Glade::XmlError& ex) {
    std::cerr << "Error loading interface definition file. " << ex.what() << std::endl;
    exit(1);
  }
  g_thread_init(NULL);

#ifdef G_OS_WIN32
  WSAData wdata;
  WSAStartup(MAKEWORD(2, 0), &wdata);
#endif

  utils.loadStatusImages();
  xml->get_widget_derived("mainWindow", mainWindow);
  xml->get_widget_derived("loginDialog", loginDialog);
  xml->get_widget_derived("chatWindow", chatWindow);
  xml->get_widget("aboutDialog", aboutDialog);
  Gtk::Button* btn;
  xml->get_widget("aboutCloseButton", btn);
  btn->signal_clicked().connect(sigc::mem_fun(*this, &Application::onAboutCloseClicked));
  xml->get_widget("aboutLinkButton", btn);
  btn->signal_clicked().connect(sigc::mem_fun(*this, &Application::onLinkButtonClicked));

  sigServer.signal_contact_list_receive().connect(sigc::mem_fun(sigClient, &MrimSignalClient::onContactListReceive));
  sigServer.signal_user_info_receive().connect(sigc::mem_fun(sigClient, &MrimSignalClient::onUserInfoReceive));
  sigServer.signal_user_status().connect(sigc::mem_fun(sigClient, &MrimSignalClient::onUserStatus));
  sigServer.signal_logout().connect(sigc::mem_fun(sigClient, &MrimSignalClient::onLogout));
  sigServer.signal_message_status().connect(sigc::mem_fun(sigClient, &MrimSignalClient::onMessageStatus));
  sigServer.signal_message_send_timeout().connect(sigc::mem_fun(sigClient, &MrimSignalClient::onMessageSendTimeout));
  sigServer.signal_message_receive().connect(sigc::mem_fun(sigClient, &MrimSignalClient::onMessageReceive));
}

Application::~Application() {
#ifdef G_OS_WIN32
  WSACleanup();
#endif
  delete mainWindow;
  delete loginDialog;
  delete chatWindow;
  delete aboutDialog;
}

Application* Application::getAppInstance() {
  return appInstance;
}

void Application::quit() {
  Gtk::Main::quit();
}

void Application::showMessage(Glib::ustring title, Glib::ustring message, Glib::ustring secondary, Gtk::MessageType mType, Gtk::ButtonsType bType) {
  Gtk::MessageDialog dialog(*mainWindow, message, false, mType, bType, true);
  dialog.set_secondary_text(secondary);
  dialog.set_title(title);
  dialog.run();
}

void Application::onAboutCloseClicked() {
  aboutDialog->hide();
}

void Application::onLinkButtonClicked() {
  openUriOnBrowser(getVariable("SWIFTIM_HOMEPAGE"));
}

void Application::openUriOnBrowser(std::string uri) {
#ifdef G_OS_WIN32
  ShellExecute(NULL, "open", uri.c_str(), NULL, NULL, SW_SHOWNORMAL);
#else
  std::vector<std::string> argv(2);
  argv[0] = "xdg-open";
  argv[1] = uri;
  Glib::spawn_async("./", argv, Glib::SPAWN_SEARCH_PATH);
#endif
}

void Application::initVariables() {
  Glib::ustring prefix = INSTALL_PREFIX;
  while(!prefix.empty() && (prefix[prefix.length()-1] == '/' || prefix[prefix.length()-1] == '\\')) {
    prefix.erase(prefix.length() - 1, 1);
  }
  variables["SWIFTIM_INSTALL_PREFIX"] = prefix;
  variables["SWIFTIM_DATA_DIR"] = variables["SWIFTIM_INSTALL_PREFIX"] + G_DIR_SEPARATOR + "share" + G_DIR_SEPARATOR + "swift-im";
  variables["SWIFTIM_USER_DATA_DIR"] = Glib::get_user_data_dir() + G_DIR_SEPARATOR + "swift-im";
  variables["SWIFTIM_HOMEPAGE"] = "http://code.google.com/p/swift-im/";
  utils.createDir(variables["SWIFTIM_USER_DATA_DIR"]);
}

Glib::ustring Application::getVariable(std::string key) {
  return variables[key];
}
