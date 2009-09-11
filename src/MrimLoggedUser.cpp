/*
 *      MrimLoggedUser.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#include "MrimLoggedUser.h"
#include "Application.h"

using namespace Swift;

MrimLoggedUser::MrimLoggedUser() {
  appInstance->logEvent("MrimLoggedUser::MrimLoggedUser()", SEVERITY_DEBUG);
  mLogged = false;
  // connection signal handlers
  appInstance->sigServer->signal_login_ack().connect(sigc::mem_fun(*this, &MrimLoggedUser::onLoginAck));
  appInstance->sigServer->signal_login_rej().connect(sigc::mem_fun(*this, &MrimLoggedUser::onLoginRej));
  appInstance->sigServer->signal_user_status().connect(sigc::mem_fun(*this, &MrimLoggedUser::onStatusChange));
  appInstance->sigServer->signal_contact_list_receive().connect(sigc::mem_fun(*this, &MrimLoggedUser::onContactListReceive));
  appInstance->sigServer->signal_user_info_receive().connect(sigc::mem_fun(*this, &MrimLoggedUser::onUserInfoReceive));
}

void MrimLoggedUser::authorize(Glib::ustring password) {
  appInstance->logEvent("MrimLoggedUser::authorize()", SEVERITY_DEBUG);
  // login
  appInstance->mClient->login(password);
}

void MrimLoggedUser::setStatus(guint32 status) {
  MrimContact::setStatus(status);
  appInstance->mainWindow->statusCombo->restoreStatus();
}

MrimContact MrimLoggedUser::getContact(guint32 index) {
  ContactList::iterator it;
  MrimContact result;
  for(it = mContactList.begin(); it != mContactList.end(); it++) {
    if(it->getIndex() == index) {
      result = *(it);
      break;
    }
  }
  return result;
}

MrimContact MrimLoggedUser::getContact(Glib::ustring address) {
  ContactList::iterator it;
  MrimContact result;
  for(it = mContactList.begin(); it != mContactList.end(); it++) {
    if(it->getAddress() == address) {
      result = *(it);
      break;
    }
  }
  return result;
}

Glib::ustring MrimLoggedUser::getInfoRec(std::string key) {
  return mInfo[key];
}

void MrimLoggedUser::onLoginAck() {
  appInstance->logEvent("MrimLoggedUser::onLoginAck()", SEVERITY_DEBUG);
  mLogged = true;
}

void MrimLoggedUser::onLoginRej(Glib::ustring reason) {
  appInstance->logEvent("MrimLoggedUser::onLoginRej()", SEVERITY_DEBUG);
  appInstance->showMessage("Login error", "Login unsuccessfull\n", "Reason: \n" + reason, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
  setStatus(STATUS_OFFLINE);
}

void MrimLoggedUser::onStatusChange(guint32 status, Glib::ustring address) {
  appInstance->logEvent("MrimLoggedUser::onStatusChange()", SEVERITY_DEBUG);
  ContactList::iterator it;
  MrimContact result;
  for(it = mContactList.begin(); it != mContactList.end(); it++) {
    if(it->getAddress() == address) {
      it->setStatus(status);
      break;
    }
  }
}

void MrimLoggedUser::onContactListReceive(GroupList gl, ContactList cl) {
  appInstance->logEvent("MrimLoggedUser::onContactListReceive()", SEVERITY_DEBUG);
  mGroupList = gl;
  mContactList = cl;
  Glib::Thread* t = Glib::Thread::create(sigc::ptr_fun(&ContactsTreeWidget::loadContactList), 0, false, true, Glib::THREAD_PRIORITY_LOW);
}

void MrimLoggedUser::onUserInfoReceive(UserInfo ui) {
  appInstance->logEvent("MrimLoggedUser::onUserInfoReceive()", SEVERITY_DEBUG);
  mInfo = ui;
  setNickname(ui["MRIM.NICKNAME"]);
}

GroupList* MrimLoggedUser::getGroupList() {
  return &mGroupList;
}

ContactList* MrimLoggedUser::getContactList() {
  return &mContactList;
}

bool MrimLoggedUser::logged() {
  return mLogged;
}
