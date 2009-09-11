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
#include <sstream>

#include <gtkmm/main.h>
#include <gtkmm/messagedialog.h>
#include <glibmm/ustring.h>
#include <glibmm/spawn.h>

#include "Application.h"
#include "SocketHeaders.h"
#include "Configure.h"

using namespace Swift;

Application* applicationInstance;

/*
 * Returns pointer to instance of the class.
 */
Application* Application::getInstance() {
  return applicationInstance;
}

Application::Application() {
  initVariables();
  applicationInstance = this;
  // open log file in 'append' mode.
  logfile = Glib::IOChannel::create_from_file(getVariable("SWIFTIM_LOGFILEPATH"), "a");
  logEvent("Swift-IM started", SEVERITY_NOTICE);
  logEvent("Program install prefix: " + getVariable("SWIFTIM_INSTALL_PREFIX"), SEVERITY_NOTICE);
  logEvent("Application::Application()", SEVERITY_DEBUG);
  try {
    xml = Gnome::Glade::Xml::create(getVariable("SWIFTIM_DATA_DIR") + G_DIR_SEPARATOR + "ui.glade");
  }
  catch (const Gnome::Glade::XmlError& ex) {
    logEvent("Error loading interface definition file. " + ex.what(), SEVERITY_FATALERROR);
  }
  // enable threading. program will fail at this line if threads aren't supported.
  if(!Glib::thread_supported()) Glib::thread_init();
  loadResources();
  sigServer = new MrimSignalServer();
  mClient = new MrimClient();
  http = new HttpClient();
  mConnection = new MrimConnection();
  mUser = new MrimLoggedUser();
#ifdef G_OS_WIN32
  // init WinSock2 on Win32 platforms
  WSAData wdata;
  WSAStartup(MAKEWORD(2, 0), &wdata);
#endif
  // getting top-level windows
  xml->get_widget_derived("mainWindow", mainWindow);
  xml->get_widget_derived("loginDialog", loginDialog);
  xml->get_widget_derived("chatWindow", chatWindow);
  xml->get_widget("aboutDialog", aboutDialog);
  Gtk::Button* btn;
  xml->get_widget("aboutCloseButton", btn);
  btn->signal_clicked().connect(sigc::mem_fun(*this, &Application::onAboutCloseClicked));
  xml->get_widget("aboutLinkButton", btn);
  btn->signal_clicked().connect(sigc::mem_fun(*this, &Application::onLinkButtonClicked));
}

Application::~Application() {
  logEvent("Application::~Application()", SEVERITY_DEBUG);
#ifdef G_OS_WIN32
  // WSACleanup must be called under Win32.
  WSACleanup();
#endif
  delete sigServer;
  delete mClient;
  delete http;
  delete mConnection;
  delete mUser;
  delete mainWindow;
  delete loginDialog;
  delete chatWindow;
  delete aboutDialog;
}

void Application::quit() {
  Gtk::Main::quit();
}

void Application::showMessage(Glib::ustring title, Glib::ustring message, Glib::ustring secondary, Gtk::MessageType mType, Gtk::ButtonsType bType) {
  logEvent("Application::showMessage()", SEVERITY_DEBUG);
  Gtk::MessageDialog dialog(*mainWindow, message, false, mType, bType, true);
  dialog.set_secondary_text(secondary);
  dialog.set_title(title);
  dialog.run();
}

void Application::onAboutCloseClicked() {
  aboutDialog->hide();
}

void Application::onLinkButtonClicked() {
  Utils::openUri(getVariable("SWIFTIM_HOMEPAGE"));
}

/*
 * Initializes application variables.
 * INSTALL_PREFIX is define at compile-time.
 * Perhaps in future variables will be loaded from INI-file.
 */
void Application::initVariables() {
  Glib::ustring prefix = INSTALL_PREFIX;
  // removing trailing slashes ('/' and '\')
  while(!prefix.empty() && (prefix[prefix.length()-1] == '/' || prefix[prefix.length()-1] == '\\')) {
    prefix.erase(prefix.length() - 1, 1);
  }
  variables["SWIFTIM_INSTALL_PREFIX"] = prefix;
  variables["SWIFTIM_DATA_DIR"] = variables["SWIFTIM_INSTALL_PREFIX"] + G_DIR_SEPARATOR + "share" + G_DIR_SEPARATOR + "swift-im";
  variables["SWIFTIM_USER_DATA_DIR"] = Glib::get_user_data_dir() + G_DIR_SEPARATOR + "swift-im";
  variables["SWIFTIM_HOMEPAGE"] = "http://code.google.com/p/swift-im/";
  
  // create user data dir if it doesn't exist.
  Utils::createDir(variables["SWIFTIM_USER_DATA_DIR"]);
  variables["SWIFTIM_LOGFILEPATH"] = variables["SWIFTIM_USER_DATA_DIR"] + G_DIR_SEPARATOR + "swift-im.log";
  variables["AVATAR_WIDTH"] = "50";
  variables["AVATAR_HEIGHT"] = "50";
  
  // this color is #ADD8E6FF in hex. last 'ff' is opacity level.
  variables["AVATAR_BGCOLOR"] = "2916673279";

  #ifdef G_OS_WIN32
  variables["ENDL"] = "\r\n";
  #else
  variables["ENDL"] = "\n";
  #endif
}

Glib::ustring Application::getVariable(std::string key) {
  return variables[key];
}

/*
 * Logs event. Messages have different severity.
 * Message will be printed to stdout and optionally writed into logfile.
 */
void Application::logEvent(Glib::ustring message, LogSeverity svty) {
  gchar dtbuf[128];
  time_t st;
  time(&st);
  guint sz = strftime(dtbuf, sizeof(dtbuf), "%c", localtime (&st));
  Glib::ustring curTime = dtbuf;
  message = getSeverityStr(svty) + ": " + message;
  message += getVariable("ENDL");
  if(logfile) {
    logfile->write(curTime + " " + message);
    logfile->flush();
  }
  std::cout << message;
  if(svty == SEVERITY_FATALERROR) {
    exit(1);
  }
}

/*
 * Helper function to obtain severity string.
 */
Glib::ustring Application::getSeverityStr(LogSeverity svty) {
  if(svty == SEVERITY_NOTICE) {
    return "(NOTICE)";
  }
  if(svty == SEVERITY_WARNING) {
    return "(*WARNING*)";
  }
  if(svty == SEVERITY_DEBUG) {
    return "(DEBUG)";
  }
  return "(*FATAL ERROR*)";
}

/*
 * Load application-specific resources: images, styles and etc
 */
void Application::loadResources() {
  logEvent("Application::loadResources()", SEVERITY_DEBUG);
  // Loading status images
  std::vector<std::string> parts(4);
  std::map<guint32, std::string> statuses;
  std::map<guint32, std::string>::iterator it;
  parts.push_back(getVariable("SWIFTIM_DATA_DIR"));
  parts.push_back("img");
  parts.push_back("status");
  statuses[STATUS_ONLINE] = "online.png";
  statuses[STATUS_OFFLINE] = "offline.png";
  statuses[STATUS_FLAG_INVISIBLE] = "invisible.png";
  statuses[STATUS_AWAY] = "away.png";
  statuses[STATUS_UNDETERMINATED] = "undeterminated.png";
  for(it = statuses.begin(); it != statuses.end(); it++) {
    parts.push_back(it->second);
    try {
      images[it->first] = Gdk::Pixbuf::create_from_file(Glib::build_filename(parts));
    }
    catch(Glib::FileError& err) {
      logEvent("Error loading status image. " + err.what(), SEVERITY_WARNING);
    }
    catch(Gdk::PixbufError& err) {
      logEvent("Error loading status image. " + err.what(), SEVERITY_WARNING);
    }
    parts.pop_back();
  }
}

/*
 * Returns status image representing status with 'statusCode'.
 * Images are preloaded by calling Application::loadResources().
 */
Glib::RefPtr<Gdk::Pixbuf> Application::getStatusImage(guint32 statusCode) {
  logEvent("Application::getStatusImage()", SEVERITY_DEBUG);
  Glib::RefPtr<Gdk::Pixbuf> result;
  std::map <guint32, Glib::RefPtr<Gdk::Pixbuf> >::iterator it = images.find(statusCode);
  if(it != images.end()) {
    result = it->second;
  }
  else {
    result = images[STATUS_UNDETERMINATED];
  }
  return result;
}
