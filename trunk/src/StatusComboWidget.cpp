/*
 *      StatusComboWidget.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
 *
 *      Copyright (c) 2009 Галымжан Кожаев <kozhayev(at)gmail(dot)com>
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

#include <gtkmm/imagemenuitem.h>

#include "StatusComboWidget.h"
#include "Application.h"
#include "MrimUtils.h"

using namespace Swift;

StatusComboWidget::StatusComboWidget(BaseObjectType* baseObject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade) : Gtk::ComboBox(baseObject) {
  treeModel = Gtk::TreeStore::create(columns);
  set_model(treeModel);
  Gtk::TreeModel::Row row;
  std::vector<guint32> statusCodes;
  statusCodes.push_back(STATUS_ONLINE);
  statusCodes.push_back(STATUS_AWAY);
  statusCodes.push_back(STATUS_FLAG_INVISIBLE);
  statusCodes.push_back(STATUS_OFFLINE);
  for(int i = 0; i < (int)statusCodes.size(); i++) {
    row = *(treeModel->append());
    row[columns.statusCode] = statusCodes[i];
    row[columns.statusTitle] = MrimUtils::getContactStatusByCode(statusCodes[i]);
    row[columns.statusIcon] = appInstance->getStatusImage(statusCodes[i]);
    set_active(row);
    rowNumbers[statusCodes[i]] = get_active_row_number();
  }
  pack_start(columns.statusIcon, false);
  pack_start(columns.statusTitle, false);
  setStatusByCode(STATUS_OFFLINE);
  signal_changed().connect(sigc::mem_fun(*this, &StatusComboWidget::onChanged));
}

void StatusComboWidget::onChanged() {
  Gtk::TreeModel::iterator iter = get_active();
  if(iter) {
    Gtk::TreeModel::Row row = *iter;
    if(row[columns.statusCode] == STATUS_OFFLINE) {
      /*
       * "logout"
       */
    }
    else if(row[columns.statusCode] != STATUS_UNDETERMINATED) {
      if(appInstance->mUser->logged()) {
        /*
         * changing status
         */
        appInstance->mUser->setStatus(row[columns.statusCode]);
        appInstance->mClient->changeStatus(row[columns.statusCode]);
      }
      else {
        /*
         * login
         */
        appInstance->loginDialog->resetEntries();
        appInstance->loginDialog->present();
      }
    }
  }
  restoreStatus();
}

guint32 StatusComboWidget::getStatusCode() {
  Gtk::TreeModel::iterator iter = get_active();
  if(iter) {
    Gtk::TreeModel::Row row = *iter;
    return row[columns.statusCode];
  }
  return 0;
}

void StatusComboWidget::setStatusByCode(guint32 newStatus) {
  set_active(rowNumbers[newStatus]);
}

void StatusComboWidget::restoreStatus() {
  setStatusByCode(appInstance->mUser->getStatus());
}
