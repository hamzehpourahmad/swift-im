/*
 *      MessageTextView.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#include "MessageTextView.h"
#include "Application.h"

using namespace Swift;

MessageTextView::MessageTextView() {
  buffer = get_buffer();
  set_accepts_tab(false);
  signal_key_press_event().connect(sigc::mem_fun(*this, &MessageTextView::onKeyPressEvent), false);
}

bool MessageTextView::onKeyPressEvent(GdkEventKey* event) {
  if(event->type == GDK_KEY_PRESS) {
    if((event->state & GDK_CONTROL_MASK) && event->keyval == GDK_Return) {
      // send message
      appInstance->chatWindow->sendButton->clicked();
      // stop executing of other handlers
      return true;
    }
  }
  return false;
}

void MessageTextView::insertSmile(Glib::ustring smileId) {
  Glib::RefPtr<Gdk::PixbufAnimation> smilePba = appInstance->getSmileImage(smileId);
  if(smilePba) {
    Gtk::Image* smileImg = manage(new Gtk::Image(smilePba));
    // add smile image and show
    Glib::RefPtr<Gtk::TextChildAnchor> refAnchor = buffer->create_child_anchor(buffer->get_insert()->get_iter());
    add_child_at_anchor(*smileImg, refAnchor);
    smileImg->show_all();

    // save anchor with associated smile id
    smiles[smileImg] = smileId;
  }
}

Glib::ustring MessageTextView::getCleanText() {
  Glib::ustring result;
  if(smiles.empty()) {
    // only text is presented
    result = buffer->get_text();
  }
  else {
    // iterate over each character and check whether current char is anchor
    Gtk::TextIter it;
    for(it = buffer->begin(); it != buffer->end(); it++) {
      Glib::RefPtr<Gtk::TextChildAnchor> refAnchor = it.get_child_anchor();
      if(refAnchor) {
        Glib::ListHandle<Gtk::Widget*> list = refAnchor->get_widgets();
        Gtk::Image* smile = (Gtk::Image*) *(list.begin());
        result += _toSmileTag(smiles[smile]);
      }
      else {
        result += it.get_char();
      }
    }
  }
  return result;
}

void MessageTextView::cleanup() {
  smiles.clear();
  buffer->set_text("");
}

Glib::ustring MessageTextView::_toSmileTag(Glib::ustring id) {
  if(id.find("<###") != Glib::ustring::npos) {
    return id;
  }
  return "<SMILE>id=" + id + " alt=''</SMILE>";
}
