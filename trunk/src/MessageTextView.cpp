/*
 *      MessageTextView.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#include "MessageTextView.h"
#include "Application.h"

using namespace Swift;

MessageTextView::MessageTextView() {
  set_accepts_tab(false);
  signal_key_press_event().connect(sigc::mem_fun(*this, &MessageTextView::onKeyPressEvent), false);
}

MessageTextView::~MessageTextView() {

}

bool MessageTextView::onKeyPressEvent(GdkEventKey* event) {
  if(event->type == GDK_KEY_PRESS) {
    if((event->state & GDK_CONTROL_MASK) && event->keyval == GDK_Return) {
      // send message
      Application::getAppInstance()->chatWindow->sendButton->clicked();
      get_buffer()->set_text("");
    }
  }
  return false;
}
