/*
 *      ChatTabs.h - this file is part of Swift-IM, cross-platform IM client for Mail.ru
 *
 *      Copyright (c) 2009 ������ �������� <kozhayev(at)gmail(dot)com>
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

#ifndef ChatTabs_h
#define ChatTabs_h

#include <map>

#include <gtkmm/notebook.h>
#include <libglademm.h>
#include <gtkmm/box.h>
#include <gtkmm/textview.h>
#include <gtkmm/button.h>

#include "MrimClient.h"
#include "HistoryTextView.h"
#include "MessageTextView.h"

namespace Swift {
  const gint DEFAULT_VPANEL_POSITION = 100;
  const gint DEFAULT_TEXTVIEW_MARGIN = 10;
  const gint DEFAULT_TEXTVIEW_BORDERWIDTH = 5;
  const guint NOTIFY_TIMEOUT_INTERVAL = 10000;
  struct ChatTab {
    HistoryTextView* historyText;
    MessageTextView* messageText;
    Gtk::Widget* child;
    Gtk::Label* notifyLabel;
    Gtk::Label* contactInfoLabel;
    Gtk::Label* tabCaption;
  };
  typedef std::map <Glib::ustring, ChatTab> Tabs;
  class ChatTabs : public Gtk::Notebook {
    public:
      ChatTabs(BaseObjectType* baseObject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
      ~ChatTabs();
      void createTab(Glib::ustring contactAddress);
      void showTab(Glib::ustring contactAddress);
      void closeTab(Glib::ustring contactAddress);
      void switchTab();
      void notifyWriting(Glib::ustring contactAddress);
      void updateStatus(MrimContact contact);
      void setUrgencyHint(MrimContact contact, bool urgent);
      std::map <Gtk::Widget*, Glib::ustring> tabMap;
      Tabs tabs;

    private:
      std::map <Glib::ustring, bool> isCreated, widgetsCreated;

    protected:
      // signal handlers
      void onSwitchTab(GtkNotebookPage *page, gint pageNumber);
      void onCloseTabClicked(Glib::ustring contactAddress);
      bool onNotifyWritingExpire(Glib::ustring contactAddress);
      void onHistoryTextScrollChanged(Gtk::Adjustment* adjustment);
  };
};

#endif //ChatTabs_h
