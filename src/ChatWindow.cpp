/*
 *      ChatWindow.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#include "ChatWindow.h"
#include "Application.h"

using namespace Swift;

ChatWindow::ChatWindow(BaseObjectType* baseObject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade) : Gtk::Window(baseObject) {
  // init widgets
  Application::getAppInstance()->xml->get_widget_derived("chatTabs", chatTabs);
  Application::getAppInstance()->xml->get_widget("sendButton", sendButton);

  // connect signals
  sendButton->signal_clicked().connect(sigc::mem_fun(*this, &ChatWindow::sendButtonOnClicked));
  signal_hide().connect(sigc::mem_fun(*this, &ChatWindow::onHide));
  signal_key_press_event().connect(sigc::mem_fun(*this, &ChatWindow::onKeyPressEvent), false);
  signal_window_state_event().connect(sigc::mem_fun(*this, &ChatWindow::onWindowStateEvent), false);
}

void ChatWindow::sendButtonOnClicked() {
  Gtk::Widget* curTabChild = chatTabs->get_current()->get_child();
  if(chatTabs->tabMap[curTabChild] != "") {
    Glib::ustring contactAddress = chatTabs->tabMap[curTabChild];
    Glib::ustring message = chatTabs->tabs[contactAddress].messageText->get_buffer()->get_text();
    if(message != "") {
      guint32 messageId = Application::getAppInstance()->mClient.sendMessage(contactAddress, message, 0);
      messageQueue[messageId] = contactAddress;
      chatTabs->tabs[contactAddress].historyText->addOwnMessage(messageId, message);
      chatTabs->tabs[contactAddress].messageText->get_buffer()->set_text("");
      //Application::getAppInstance()->sigServer.signal_message_receive().emit(0, "someaddress@somehost.com", "Received message! :)", "");
    }
  }
}

void ChatWindow::onHide() {
  // close all tabs before hiding
  Tabs::iterator it;
  for(it = chatTabs->tabs.begin(); it != chatTabs->tabs.end(); it++) {
    chatTabs->closeTab(it->first);
  }
}

bool ChatWindow::onKeyPressEvent(GdkEventKey* event) {
  if(event->type == GDK_KEY_PRESS) {
    if((event->state & GDK_CONTROL_MASK) && event->keyval == GDK_Tab) {
      // switch tab
      chatTabs->switchTab();
    }
  }
  return false;
}

bool ChatWindow::onWindowStateEvent(GdkEventWindowState* event) {
  if((event->new_window_state ^ GDK_WINDOW_STATE_WITHDRAWN) || (event->new_window_state ^ GDK_WINDOW_STATE_ICONIFIED)) {
    set_urgency_hint(false);
  }
  return false;
}
