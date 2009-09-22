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
#include "MrimUtils.h"

using namespace Glib;
using namespace Swift;

ChatWindow::ChatWindow(BaseObjectType* baseObject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade) : Gtk::Window(baseObject) {
  appInstance->logEvent("ChatWindow::ChatWindow()", SEVERITY_DEBUG);
  // get widgets
  appInstance->xml->get_widget_derived("chatTabs", chatTabs);
  appInstance->xml->get_widget("sendButton", sendButton);

  // connect signal handlers
  sendButton->signal_clicked().connect(sigc::mem_fun(*this, &ChatWindow::sendButtonOnClicked));
  signal_hide().connect(sigc::mem_fun(*this, &ChatWindow::onHide));
  signal_key_press_event().connect(sigc::mem_fun(*this, &ChatWindow::onKeyPressEvent), false);
  signal_window_state_event().connect(sigc::mem_fun(*this, &ChatWindow::onWindowStateEvent), false);
  appInstance->sigServer->signal_message_status().connect(sigc::mem_fun(*this, &ChatWindow::onMessageStatus));
  appInstance->sigServer->signal_message_receive().connect(sigc::mem_fun(*this, &ChatWindow::onMessageReceive));
}

/*
 * Callback function that executed when user clicks 'send' button.
 */
void ChatWindow::sendButtonOnClicked() {
  appInstance->logEvent("ChatWindow::sendButtonOnClicked()", SEVERITY_DEBUG);
  ChatTab curTab = chatTabs->getCurrentTab();
  Glib::ustring address = curTab.address;
  Glib::ustring message = curTab.messageText->get_buffer()->get_text();
  if(!address.empty() && !message.empty()) {
    guint32 messageId;
    appInstance->mClient->sendMessage(0, address, message, " ", &messageId);
    // save message in message queue
    mMessageQueue[messageId] = address;
    curTab.historyText->addOwnMessage(messageId, message);
    curTab.messageText->get_buffer()->set_text("");
    sigc::slot<bool> timeoutSlot = sigc::bind(sigc::mem_fun(*this, &ChatWindow::onMessageSendTimeout), messageId);
    // check message delivery status after 'pingPeriod' seconds.
    Glib::signal_timeout().connect(timeoutSlot, appInstance->mConnection->getPingPeriod() * 1000);
  }
}

void ChatWindow::onHide() {
  appInstance->logEvent("ChatWindow::onHide()", SEVERITY_DEBUG);
  // close all tabs before hiding
  chatTabs->closeAll();
}

/*
 * Callback on key press events.
 * Swicthes tab if user hits Ctrl+Tab.
 */
bool ChatWindow::onKeyPressEvent(GdkEventKey* event) {
  if(event->type == GDK_KEY_PRESS) {
    if((event->state & GDK_CONTROL_MASK) && event->keyval == GDK_Tab) {
      // switch tab
      chatTabs->switchTab();
    }
  }
  return false;
}

/*
 * Callback function on window state event.
 * 'Deurgent's window if it has restored.
 */
bool ChatWindow::onWindowStateEvent(GdkEventWindowState* event) {
  if((event->new_window_state ^ GDK_WINDOW_STATE_WITHDRAWN) || (event->new_window_state ^ GDK_WINDOW_STATE_ICONIFIED)) {
    set_urgency_hint(false);
  }
  return false;
}

/*
 * Checks whether message with 'messageId' succesfully delivered.
 * If message is still in mMessageQueue it is considered as undelivered message.
 * This function connected to timeout signal whenever user sends a message.
 * Must return false to prevent calling every timeout interval.
 */
bool ChatWindow::onMessageSendTimeout(guint32 messageId) {
  appInstance->logEvent("ChatWindow::onMessageSendTimeout()", SEVERITY_DEBUG);
  MessageQueue::iterator it = mMessageQueue.find(messageId);
  if(it != mMessageQueue.end()) {
    gint tabIndex = chatTabs->getTab(it->second);
    chatTabs->getNthTab(tabIndex).historyText->rejectMessage(messageId, _("Timeout period expired"));
    mMessageQueue.erase(it);
  }
  return false;
}

/*
 * Handler of SignalMessageStatus.
 * Erases message from mMessageQueue.
 */
void ChatWindow::onMessageStatus(guint32 messageId, guint32 status) {
  appInstance->logEvent("ChatWindow::onMessageStatus()", SEVERITY_DEBUG);
  MessageQueue::iterator it = mMessageQueue.find(messageId);
  if(it != mMessageQueue.end()) {
    gint tabIndex = chatTabs->getTab(it->second);
    if(status == MESSAGE_DELIVERED) {
      chatTabs->getNthTab(tabIndex).historyText->confirmMessage(messageId);
    }
    else {
      chatTabs->getNthTab(tabIndex).historyText->rejectMessage(messageId, MrimUtils::getMessageStatusByCode(status));
    }
    mMessageQueue.erase(it);
  }
}

/*
 * Handler of SignalMessageReceive.
 */
void ChatWindow::onMessageReceive(guint32 messageId, guint32 flags, Glib::ustring from, Glib::ustring message, Glib::ustring rtf) {
  appInstance->logEvent("ChatWindow::onMessageReceive()", SEVERITY_DEBUG);
  // send message receive confirmation
  if(!(flags & MESSAGE_FLAG_NORECV)) {
    appInstance->mClient->sendMessageRecv(from, messageId);
  }
  bool showWindow = false, makeUrgent = false;
  if(flags & MESSAGE_FLAG_OFFLINE) {
    // offline message
    showWindow = true;
    makeUrgent = true;
    chatTabs->createTab(from);
    chatTabs->getNthTab(chatTabs->getTab(from)).historyText->addReceivedMessage(from, message);
  }
  else if(flags & MESSAGE_FLAG_AUTHORIZE) {
    // authorization request
  }
  else if(flags & MESSAGE_FLAG_SYSTEM) {
    // system message
    showWindow = true;
    makeUrgent = true;
    chatTabs->createTab(from);
    chatTabs->getNthTab(chatTabs->getTab(from)).historyText->addReceivedMessage(from, message);
  }
  else if(flags & MESSAGE_FLAG_CONTACT) {
    // message contains contact list
    // format of message is following:
    // ADDRESS1;NICKNAME1;ADDRESS2;NICKNAME2;...
  }
  else if(flags & MESSAGE_FLAG_NOTIFY) {
    // notification message (e.g. 'Someone is writing a message to you')
    chatTabs->notifyWriting(from);
  }
  else if(flags & MESSAGE_FLAG_MULTICAST) {
    // message has been sent to multiple recipients
    showWindow = true;
    makeUrgent = true;
    chatTabs->createTab(from);
    chatTabs->getNthTab(chatTabs->getTab(from)).historyText->addReceivedMessage(from, message);
  }
  else {
    showWindow = true;
    makeUrgent = true;
    chatTabs->createTab(from);
    chatTabs->getNthTab(chatTabs->getTab(from)).historyText->addReceivedMessage(from, message);
  }

  if(showWindow && !is_visible()) {
    iconify();
    present();
  }
  if(makeUrgent && !property_is_active()) {
    set_urgency_hint(true);
    chatTabs->setUrgencyHint(appInstance->mUser->getContact(from), true);
  }
}
