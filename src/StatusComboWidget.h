/*
 *      StatusComboWidget.h - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#ifndef StatusComboWidget_h
#define StatusComboWidget_h

namespace Swift {
  class StatusComboWidget;
};

#include <map>

#include <libglademm.h>
#include <gtkmm/treestore.h>
#include <gtkmm/combobox.h>

#include "StatusComboColumns.h"

namespace Swift {
  class StatusComboWidget : public Gtk::ComboBox {
    public:
      StatusComboWidget(BaseObjectType* baseObject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
      guint32 getStatusCode();
      void restoreStatus();

    private:
      void setStatusByCode(guint32 newStatus);
      Glib::RefPtr<Gtk::TreeStore> treeModel;
      std::map<guint32, gint> rowNumbers;
      StatusComboColumns columns;
    protected:
      void onChanged();
  };
};

#endif //StatusComboWidget_h
