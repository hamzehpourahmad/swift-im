/*
 *      ContactsTreeWidget.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#include <iostream>

#include <gtkmm/image.h>

#include "ContactsTreeWidget.h"
#include "MrimUtils.h"
#include "MrimLoggedUser.h"

using namespace Swift;

ContactsTreeWidget::ContactsTreeWidget(BaseObjectType* baseObject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade) : Gtk::TreeView(baseObject) {
  appInstance->logEvent("ContactsTreeWidget::ContactsTreeWidget()", SEVERITY_DEBUG);
  treeModel = Gtk::TreeStore::create(columns);
  // sort by status codes.
  // STATUS_OFFLINE is 0, so it will always be at bottom of the list
  treeModel->set_sort_column(columns.contactStatus, Gtk::SORT_DESCENDING);
  set_model(treeModel);
  set_search_equal_func(sigc::mem_fun(*this, &ContactsTreeWidget::onSearchEqual));

  // appending columns
  gint colCount;
  Gtk::TreeViewColumn* pColumn;
  Gtk::CellRenderer* renderer;
  Gtk::CellRendererPixbuf *avatarRenderer, *statusRenderer;

  statusRenderer = Gtk::manage(new Gtk::CellRendererPixbuf);
  statusRenderer->property_xalign() = 0.0;
  colCount = append_column("Status", *statusRenderer);
  pColumn = get_column(colCount - 1);
  pColumn->set_resizable(true);
  pColumn->add_attribute(statusRenderer->property_pixbuf(), columns.statusImage);

  colCount = append_column("Nickname", columns.contactNickname);
  pColumn = get_column(colCount - 1);
  pColumn->set_resizable(true);
  renderer = pColumn->get_first_cell_renderer();
  renderer->property_xalign() = 0.0;

  avatarRenderer = Gtk::manage(new Gtk::CellRendererPixbuf);
  avatarRenderer->property_xalign() = 1.0;
  avatarRenderer->property_xpad() = 10;
  colCount = append_column("", *avatarRenderer);
  pColumn = get_column(colCount - 1);
  pColumn->add_attribute(avatarRenderer->property_pixbuf(), columns.contactAvatar);

  // connecting signal handlers
  signal_row_activated().connect(sigc::mem_fun(*this, &ContactsTreeWidget::contactsTreeOnActivate));
  appInstance->sigServer->signal_user_status().connect(sigc::mem_fun(*this, &ContactsTreeWidget::onStatusChange));
}

/*
 * Search function for widget.
 * Returns true if entered text matches contact address or nickname.
 */
bool ContactsTreeWidget::onSearchEqual(const Glib::RefPtr<Gtk::TreeModel>& model, gint column, const Glib::ustring& key, const Gtk::TreeModel::iterator& iter) {
  Gtk::TreeModel::Row row = *iter;
  if (row[columns.contactIndex] >= 20 && row[columns.contactIndex] != GROUP_INDEX_NOT_AUTHORIZED) {
    Glib::ustring x;
    x = row[columns.contactAddress];
    if (x.find(key) != Glib::ustring::npos) {
      return false;
    }
    x = row[columns.contactNickname];
    if (x.find(key) != Glib::ustring::npos) {
      return false;
    }
  }
  return true;
}

void ContactsTreeWidget::addGroup(MrimGroup group) {
  appInstance->logEvent("ContactsTreeWidget::addGroup()", SEVERITY_DEBUG);
  Gtk::TreeModel::Row row;
  row = *(treeModel->append());
  row[columns.contactIndex] = group.getIndex();
  row[columns.contactAddress] = "";
  row[columns.contactStatus] = 0;
  row[columns.contactNickname] = group.getName();
  addedGroups[group.getIndex()] = row;
  addedGroupsFlag[group.getIndex()] = true;
}

void ContactsTreeWidget::addContact(MrimContact contact) {
  appInstance->logEvent("ContactsTreeWidget::addContact()", SEVERITY_DEBUG);
  Gtk::TreeModel::Row row;
  if (!addedGroupsFlag[contact.getGroup().getIndex()]) {
    // if contact isn't in any group
    row = *(treeModel->append());
  } else {
    if (contact.getStatus() != STATUS_OFFLINE && contact.getStatus() != STATUS_UNDETERMINATED) {
      row = *(treeModel->prepend(addedGroups[contact.getGroup().getIndex()]->children()));
    } else {
      row = *(treeModel->append(addedGroups[contact.getGroup().getIndex()]->children()));
    }
  }
  row[columns.contactIndex] = contact.getIndex();
  row[columns.contactAddress] = contact.getAddress();
  row[columns.contactNickname] = contact.getNickname();
  row[columns.contactStatus] = contact.getStatus();
  row[columns.statusImage] = appInstance->getStatusImage(contact.getStatus());
  addedContacts[contact.getIndex()] = row;
  addedContactsFlag[contact.getIndex()] = true;
}

void ContactsTreeWidget::removeGroup(MrimGroup group) {
  appInstance->logEvent("ContactsTreeWidget::removeGroup()", SEVERITY_DEBUG);
  if (addedGroupsFlag[group.getIndex()]) {
    treeModel->erase(addedGroups[group.getIndex()]);
    addedGroupsFlag[group.getIndex()] = false;
  }
}

void ContactsTreeWidget::removeContact(MrimContact contact) {
  appInstance->logEvent("ContactsTreeWidget::removeContact()", SEVERITY_DEBUG);
  if (addedContactsFlag[contact.getIndex()]) {
    treeModel->erase(addedContacts[contact.getIndex()]);
    addedContactsFlag[contact.getIndex()] = false;
  }
}

void ContactsTreeWidget::updateStatus(MrimContact contact) {
  appInstance->logEvent("ContactsTreeWidget::updateStatus()", SEVERITY_DEBUG);
  if (addedContactsFlag[contact.getIndex()]) {
    Gtk::TreeModel::Row row;
    addedContacts[contact.getIndex()][columns.statusImage] = appInstance->getStatusImage(contact.getStatus());
  }
}

void ContactsTreeWidget::loadContactList() {
  appInstance->logEvent("ContactsTreeWidget::loadContactList()", SEVERITY_DEBUG);
  GroupList *gl = appInstance->mUser->getGroupList();
  ContactList *cl = appInstance->mUser->getContactList();
  ContactList::iterator clIt;
  GroupList::iterator glIt;
  appInstance->mainWindow->contactsTree->treeModel->clear();
  for (glIt = gl->begin(); glIt != gl->end(); glIt++) {
    appInstance->mainWindow->contactsTree->addGroup(*glIt);
  }
  for (clIt = cl->begin(); clIt != cl->end(); clIt++) {
    if (clIt->getServerFlags() == CONTACT_INTFLAG_NOT_AUTHORIZED) {
      MrimGroup g;
      g.setIndex(GROUP_INDEX_NOT_AUTHORIZED);
      g.setFlags(0);
      g.setName("Waiting for authorization");
      if (!appInstance->mainWindow->contactsTree->addedGroupsFlag[GROUP_INDEX_NOT_AUTHORIZED]) {
        appInstance->mainWindow->contactsTree->addGroup(g);
      }
      clIt->setGroup(g);
    }
    appInstance->mainWindow->contactsTree->addContact(*clIt);
  }
  appInstance->mainWindow->contactsTree->loadAvatars();
}

Glib::RefPtr<Gdk::Pixbuf> ContactsTreeWidget::resizeAvatar(Glib::RefPtr<Gdk::Pixbuf> src) {
  appInstance->logEvent("ContactsTreeWidget::resizeAvatar()", SEVERITY_DEBUG);
  gint WIDTH = Utils::parseGint(appInstance->getVariable("AVATAR_WIDTH"));
  gint HEIGHT = Utils::parseGint(appInstance->getVariable("AVATAR_HEIGHT"));
  guint32 BGCOLOR = Utils::parseGuint32(appInstance->getVariable("AVATAR_BGCOLOR"));
  Glib::RefPtr<Gdk::Pixbuf> bg = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, WIDTH, HEIGHT);
  bg->fill(BGCOLOR);
  gint w, h, tw, th, dx, dy;
  double sx, sy;
  w = src->get_width();
  h = src->get_height();
  if(w > h) {
    tw = WIDTH;
    th = (gint)(tw * h * 1.0 / w);
  }
  else {
    th = HEIGHT;
    tw = (gint)(th * w * 1.0 / h);
  }
  sx = tw * 1.0 / w;
  sy = th * 1.0 / h;
  dx = (WIDTH - tw) / 2;
  dy = (HEIGHT - th) / 2;
  src->scale(bg, dx, dy, tw, th, dx, dy, sx, sy, Gdk::INTERP_BILINEAR);
  return bg;
}

void ContactsTreeWidget::loadAvatars() {
  appInstance->logEvent("ContactsTreeWidget::loadAvatars()", SEVERITY_DEBUG);
  ContactList *cl = appInstance->mUser->getContactList();
  ContactList::iterator clIt;
  for (clIt = cl->begin(); clIt != cl->end(); clIt++) {
    if (addedContactsFlag[clIt->getIndex()]) {
      clIt->setAvatar(MrimUtils::prepareAvatar(clIt->getAddress()));
      addedContacts[clIt->getIndex()][columns.contactAvatar] = resizeAvatar(clIt->getAvatar());
    }
  }
}

ContactsTreeWidget::~ContactsTreeWidget() {
  /*
   * I think we should free contact list avatars images in MrimClient class's destructor
   * because it is more "right" to place ContactList destructing code in that class
   */
}

void ContactsTreeWidget::contactsTreeOnActivate(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn *column) {
  appInstance->logEvent("ContactsTreeWidget::contactsTreeOnActivate()", SEVERITY_DEBUG);
  ContactsTreeColumns columns;
  Gtk::TreeModel::iterator iter = treeModel->get_iter(path);
  if (iter) {
    Gtk::TreeModel::Row row = *iter;
    if (row[columns.contactIndex] >= 20 && row[columns.contactIndex] != GROUP_INDEX_NOT_AUTHORIZED) {
      appInstance->chatWindow->show();
      appInstance->chatWindow->chatTabs->showTab(row[columns.contactAddress]);
      appInstance->chatWindow->present();
    }
  }
}

void ContactsTreeWidget::onStatusChange(guint32 status, Glib::ustring address) {
  appInstance->logEvent("ContactsTreeWidget::onStatusChange()", SEVERITY_DEBUG);
  MrimContact c = appInstance->mUser->getContact(address);
  c.setStatus(status);
  updateStatus(c);
}