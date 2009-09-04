/*
 *      MrimSignalClient.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#include "MrimSignalClient.h"
#include "Application.h"
#include "MrimClient.h"
#include "MrimUtils.h"

using namespace Swift;

void MrimSignalClient::onContactListReceive() {
  GThread* t = g_thread_create(GThreadFunc(ContactsTreeWidget::loadContactList), NULL, false, NULL);
  g_thread_set_priority(t, G_THREAD_PRIORITY_LOW);
}

void MrimSignalClient::onUserInfoReceive() {
  MrimClient* client = &(Application::getAppInstance()->mClient);
  Application::getAppInstance()->mainWindow->usernameLabel->set_label("<span size='medium' weight='bold'>" + client->userInfo["MRIM.NICKNAME"] + "</span>");
  Application::getAppInstance()->mainWindow->mailStatusLabel->set_label("<span size='small' weight='bold'>Messages unread: " + client->userInfo["MESSAGES.UNREAD"] + "</span>");
  Gtk::Image* img = MrimUtils::prepareAvatar(client->userInfo["LOGIN"]);
  Application::getAppInstance()->mainWindow->userAvatarImage->set(img->get_pixbuf());
  delete img;
}

void MrimSignalClient::onUserStatus(guint32 newStatus, Glib::ustring address) {
  MrimContact c = MrimUtils::getContactByAddress(&Application::getAppInstance()->mClient, address);
  c.status = newStatus;
  Application::getAppInstance()->mainWindow->contactsTree->updateStatus(c);
  Application::getAppInstance()->chatWindow->chatTabs->updateStatus(c);
  // sort contacts tree
}

void MrimSignalClient::onLogout(guint32 logoutCode) {
  Glib::ustring reason = "";
  if(logoutCode == LOGOUT_NO_RELOGIN_FLAG) {
    reason = "Reason:\nYou are already logged in using another device";
  }
  Application::getAppInstance()->showMessage("Login error", "You're logged out.", reason , Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
}

void MrimSignalClient::onMessageStatus(guint32 msgId, guint32 status) {
  MessageQueue::iterator it = Application::getAppInstance()->chatWindow->messageQueue.find(msgId);
  if(it != Application::getAppInstance()->chatWindow->messageQueue.end()) {
    if(status == MESSAGE_DELIVERED) {
      Application::getAppInstance()->chatWindow->chatTabs->tabs[it->second].historyText->confirmMessage(msgId);
    }
    else {
      Application::getAppInstance()->chatWindow->chatTabs->tabs[it->second].historyText->rejectMessage(msgId, MrimUtils::getMessageStatusByCode(status));
    }
    Application::getAppInstance()->chatWindow->messageQueue.erase(it);
  }
}

void MrimSignalClient::onMessageSendTimeout(guint32 msgId) {
  MessageQueue::iterator it = Application::getAppInstance()->chatWindow->messageQueue.find(msgId);
  if(it != Application::getAppInstance()->chatWindow->messageQueue.end()) {
    Application::getAppInstance()->chatWindow->chatTabs->tabs[it->second].historyText->rejectMessage(msgId, "Timeout period expired");
    Application::getAppInstance()->chatWindow->messageQueue.erase(it);
  }
}

void MrimSignalClient::onMessageReceive(guint32 flags, Glib::ustring from, Glib::ustring message, Glib::ustring rtf) {
  bool showWindow = false, makeUrgent = false;
  Application*  app = Application::getAppInstance();
  if(flags & MESSAGE_FLAG_OFFLINE) {
    // offline message
    showWindow = true;
    makeUrgent = true;
    app->chatWindow->chatTabs->createTab(from);
    app->chatWindow->chatTabs->tabs[from].historyText->addReceivedMessage(from, message);
  }
  else if(flags & MESSAGE_FLAG_AUTHORIZE) {
    // authorization request
  }
  else if(flags & MESSAGE_FLAG_SYSTEM) {
    // system message
    showWindow = true;
    makeUrgent = true;
    app->chatWindow->chatTabs->createTab(from);
    app->chatWindow->chatTabs->tabs[from].historyText->addReceivedMessage(from, message);
  }
  else if(flags & MESSAGE_FLAG_CONTACT) {
    // message contains contact list
    // format of message is following:
    // ADDRESS1;NICKNAME1;ADDRESS2;NICKNAME2;...
  }
  else if(flags & MESSAGE_FLAG_NOTIFY) {
    // notification message (e.g. 'Someone is writing a message to you')
    app->chatWindow->chatTabs->notifyWriting(from);
  }
  else if(flags & MESSAGE_FLAG_MULTICAST) {
    // message has been sent to multiple recipients
    showWindow = true;
    makeUrgent = true;
    app->chatWindow->chatTabs->createTab(from);
    app->chatWindow->chatTabs->tabs[from].historyText->addReceivedMessage(from, message);
  }
  else {
    showWindow = true;
    makeUrgent = true;
    app->chatWindow->chatTabs->createTab(from);
    app->chatWindow->chatTabs->tabs[from].historyText->addReceivedMessage(from, message);
  }

  if(showWindow && !app->chatWindow->is_visible()) {
    app->chatWindow->iconify();
    app->chatWindow->show();
  }
  if(makeUrgent && !app->chatWindow->property_is_active()) {
    app->chatWindow->set_urgency_hint(true);
    app->chatWindow->chatTabs->setUrgencyHint(MrimUtils::getContactByAddress(&Application::getAppInstance()->mClient, from), true);
  }
}
