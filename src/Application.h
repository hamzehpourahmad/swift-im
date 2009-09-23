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

/*
 * Represents running application
 * Communication between other classes can be done using 'appInstance' macro.
 * Instance of this class has pointers to instances of other classes.
 */

namespace Swift {
  class Application;
};

#include <string>
#include <map>

#include <libglademm.h>
#include <gtkmm/dialog.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/aboutdialog.h>
#include <glibmm/i18n.h>
#include <glibmm/keyfile.h>

#include "MainWindow.h"
#include "LoginDialog.h"
#include "ChatWindow.h"
#include "AboutDialog.h"
#include "MrimSignalServer.h"
#include "HttpClient.h"
#include "MrimClient.h"
#include "MrimConnection.h"
#include "MrimLoggedUser.h"

#define appInstance Application::getInstance()

namespace Swift {
  enum LogSeverity {
    SEVERITY_NOTICE,
    SEVERITY_DEBUG,
    SEVERITY_WARNING,
    SEVERITY_FATALERROR
  };
  class Application {
    public:
      Application();
      virtual ~Application();
      std::string getVersion();
      static void quit();
      void showMessage(const Glib::ustring title, const Glib::ustring message, const Glib::ustring secondary, Gtk::MessageType mType, Gtk::ButtonsType bType);
      void logEvent(Glib::ustring message, LogSeverity svty);
      static Application* getInstance();
      Glib::RefPtr<Gdk::Pixbuf> getStatusImage(guint32 statusCode);
      Glib::RefPtr<Gdk::Pixbuf> getSmileImage(Glib::ustring smileId);
      MainWindow* mainWindow;
      LoginDialog* loginDialog;
      ChatWindow* chatWindow;
      AboutDialog* aboutDialog;
      MrimClient* mClient;
      MrimConnection* mConnection;
      Glib::RefPtr<Gnome::Glade::Xml> xml;
      MrimSignalServer* sigServer;
      HttpClient* http;
      MrimLoggedUser* mUser;
      Glib::ustring getVariable(std::string key);

    private:
      std::map<std::string, Glib::ustring> variables;
      std::map <guint32, Glib::RefPtr<Gdk::Pixbuf> > images;
      std::map <Glib::ustring, Glib::RefPtr<Gdk::Pixbuf> > smiles;
      Glib::RefPtr<Glib::IOChannel> logfile;
      Glib::ustring getSeverityStr(LogSeverity svty);
      void initLocale();
      void initVariables();
      void loadResources();
  };
};

#endif //Application_h
