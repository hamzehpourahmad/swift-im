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

using namespace Swift;

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

ContactsTreeWidget::ContactsTreeWidget(BaseObjectType* baseObject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade) : Gtk::TreeView(baseObject) {
  treeModel = Gtk::TreeStore::create(columns);
  treeModel->set_sort_column(columns.contactStatus, Gtk::SORT_DESCENDING);
  /*
  treeModelSort = Gtk::TreeModelSort::create(treeModel);
  treeModelSort->set_sort_column(columns.contactStatus, Gtk::SORT_DESCENDING);
  */
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
}

void ContactsTreeWidget::addGroup(MrimGroup group) {
  Gtk::TreeModel::Row row;
  row = *(treeModel->append());
  row[columns.contactIndex] = group.index;
  row[columns.contactAddress] = "";
  row[columns.contactStatus] = 0;
  row[columns.contactNickname] = group.name;
  addedGroups[group.index] = row;
  addedGroupsFlag[group.index] = true;
}

void ContactsTreeWidget::addContact(MrimContact contact) {
  Gtk::TreeModel::Row row;
  if (!addedGroupsFlag[contact.group]) {
    // if contact isn't in any group
    row = *(treeModel->append());
  } else {
    if (contact.status != STATUS_OFFLINE && contact.status != STATUS_UNDETERMINATED) {
      row = *(treeModel->prepend(addedGroups[contact.group]->children()));
    } else {
      row = *(treeModel->append(addedGroups[contact.group]->children()));
    }
  }
  row[columns.contactIndex] = contact.index;
  row[columns.contactAddress] = contact.address;
  row[columns.contactNickname] = contact.nickname;
  row[columns.contactStatus] = contact.status;
  row[columns.statusImage] = Application::getAppInstance()->utils.getStatusImage(contact.status);
  addedContacts[contact.index] = row;
  addedContactsFlag[contact.index] = true;
}

void ContactsTreeWidget::removeGroup(MrimGroup group) {
  if (addedGroupsFlag[group.index]) {
    treeModel->erase(addedGroups[group.index]);
    addedGroupsFlag[group.index] = false;
  }
}

void ContactsTreeWidget::removeContact(MrimContact contact) {
  if (addedContactsFlag[contact.index]) {
    treeModel->erase(addedContacts[contact.index]);
    addedContactsFlag[contact.index] = false;
  }
}

void ContactsTreeWidget::updateStatus(MrimContact contact) {
  if (addedContactsFlag[contact.index]) {
    Gtk::TreeModel::Row row;
    addedContacts[contact.index][columns.statusImage] = Application::getAppInstance()->utils.getStatusImage(contact.status);
  }
}

void ContactsTreeWidget::loadContactList() {
  Application* app = Application::getAppInstance();
  ContactList::iterator clIt;
  GroupList::iterator glIt;
  app->mainWindow->contactsTree->treeModel->clear();
  for (glIt = app->mClient.groupList.begin(); glIt != app->mClient.groupList.end(); glIt++) {
    app->mainWindow->contactsTree->addGroup(*glIt);
  }
  for (clIt = app->mClient.contactList.begin(); clIt != app->mClient.contactList.end(); clIt++) {
    if (clIt->serverFlags == CONTACT_INTFLAG_NOT_AUTHORIZED) {
      if (!app->mainWindow->contactsTree->addedGroupsFlag[GROUP_INDEX_NOT_AUTHORIZED]) {
        MrimGroup g;
        g.index = GROUP_INDEX_NOT_AUTHORIZED;
        g.flag  = 0;
        g.name  = "Waiting for authorization";
        app->mainWindow->contactsTree->addGroup(g);
      }
      clIt->group = GROUP_INDEX_NOT_AUTHORIZED;
    }
    app->mainWindow->contactsTree->addContact(*clIt);
  }
  app->mainWindow->contactsTree->loadAvatars();
}

Glib::RefPtr<Gdk::Pixbuf> ContactsTreeWidget::resizeAvatar(Glib::RefPtr<Gdk::Pixbuf> src) {
  gint w, h, th, tw;
  /*
  w - tw
  h - th
  */
  h = src->get_height();
  w = src->get_width();
  th = RESIZED_AVATAR_HEIGHT;
  tw = floor(th * w * 1.0 / h);
  return src->scale_simple(tw, th, Gdk::INTERP_BILINEAR);
}

void ContactsTreeWidget::loadAvatars() {
  ContactList::iterator clIt;
  Application* app = Application::getAppInstance();
  for (clIt = app->mClient.contactList.begin(); clIt != app->mClient.contactList.end(); clIt++) {
    if (addedContactsFlag[clIt->index]) {
      clIt->avatar = MrimUtils::prepareAvatar(clIt->address);
      addedContacts[clIt->index][columns.contactAvatar] = resizeAvatar(clIt->avatar->get_pixbuf());
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
  ContactsTreeColumns columns;
  Gtk::TreeModel::iterator iter = treeModel->get_iter(path);
  if (iter) {
    Gtk::TreeModel::Row row = *iter;
    if (row[columns.contactIndex] >= 20 && row[columns.contactIndex] != GROUP_INDEX_NOT_AUTHORIZED) {
      Application::getAppInstance()->chatWindow->show();
      Application::getAppInstance()->chatWindow->chatTabs->showTab(row[columns.contactAddress]);
      Application::getAppInstance()->chatWindow->present();
    }
  }
}
