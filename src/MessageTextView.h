/*
 *      MessageTextView.h - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#ifndef MessageTextView_h
#define MessageTextView_h

namespace Swift {
  class MessageTextView;
};

#include <map>
#include <algorithm>

#include <gtkmm/textview.h>
#include <gtkmm/image.h>


namespace Swift {

  typedef std::map<Gtk::Image*, Glib::ustring > SmileList;
  class MessageTextView : public Gtk::TextView {
    public:
      MessageTextView();
      void insertSmile(Glib::ustring smileId);
      Glib::ustring getCleanText();
      void cleanup();

    private:
      Glib::ustring _toSmileTag(Glib::ustring id);
      Glib::RefPtr<Gtk::TextBuffer> buffer;
      SmileList smiles;

    protected:
      // signal handlers
      bool onKeyPressEvent(GdkEventKey* event);
  };
};
#endif //MessageTextView_h
