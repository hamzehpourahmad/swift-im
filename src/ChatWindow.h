/*
 *      ChatWindow.h - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#ifndef ChatWindow_h
#define ChatWindow_h

#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <libglademm.h>

#include "ChatTabs.h"

namespace Swift {
  typedef std::map <guint32, Glib::ustring> MessageQueue;
  class ChatWindow : public Gtk::Window {
    public:
      ChatWindow(BaseObjectType* baseObject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
      ChatTabs* chatTabs;
      Gtk::Button* sendButton;
      MessageQueue messageQueue;
    protected:
      bool onKeyPressEvent(GdkEventKey* event);
      void sendButtonOnClicked();
      void onHide();
      bool onWindowStateEvent(GdkEventWindowState* event);
  };
};

#endif //ChatWindow_h
