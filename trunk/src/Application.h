/*
 *      Application.h - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#ifndef Application_h
#define Application_h

#include <string>
#include <map>

#include <libglademm.h>
#include <gtkmm/dialog.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/aboutdialog.h>
#include <glibmm/i18n.h>

#include "MainWindow.h"
#include "LoginDialog.h"
#include "ChatWindow.h"
#include "MrimSignalServer.h"
#include "MrimSignalClient.h"
#include "HttpClient.h"
#include "Utils.h"
#include "MrimClient.h"

namespace Swift {
  class Application {
    public:
      Application();
      virtual ~Application();
      static Application* getAppInstance();
      static void quit();
      void showMessage(const Glib::ustring title, const Glib::ustring message, const Glib::ustring secondary, Gtk::MessageType mType, Gtk::ButtonsType bType);

      MainWindow* mainWindow;
      LoginDialog* loginDialog;
      ChatWindow* chatWindow;
      Gtk::Dialog* aboutDialog;
      MrimClient mClient;
      Glib::RefPtr<Gnome::Glade::Xml> xml;
      MrimSignalClient sigClient;
      MrimSignalServer sigServer;
      HttpClient http;
      Utils utils;
      bool mrimLogged;
      void openUriOnBrowser(std::string uri);
      Glib::ustring getVariable(std::string key);

    private:
      std::map<std::string, Glib::ustring> variables;

      void initVariables();
    protected:

      // signal handlers
      void onAboutCloseClicked();
      void onLinkButtonClicked();
  };
};

#endif //Application_h
