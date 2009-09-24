/*
 *      HistoryTextView.h - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#ifndef HistoryTextView_h
#define HistoryTextView_h

namespace Swift {
  class HistoryTextView;
};

#include <vector>
#include <map>

#include <gtkmm/textview.h>

namespace Swift {
  struct MessageBounds {
    Glib::RefPtr<Gtk::TextBuffer::Mark> beginMark;
    Glib::RefPtr<Gtk::TextBuffer::Mark> endMark;
  };
  struct TextPart {
    Glib::ustring text;
    bool smile;
  };
  typedef std::map <guint32, MessageBounds> MessageBoundsList;
  class HistoryTextView : public Gtk::TextView {
    public:
      HistoryTextView();
      void addOwnMessage(guint32 messageId, Glib::ustring body);
      void addReceivedMessage(Glib::ustring from, Glib::ustring body);
      void confirmMessage(guint32 messageId);
      void rejectMessage(guint32 messageId, Glib::ustring reason);
      Glib::ustring getFormattedTime();
    private:
      Glib::RefPtr<Gtk::TextBuffer> buffer;
      MessageBoundsList history;
      void addMessage(Glib::ustring msg, bool me);
      void insertTime();
      void newLine();
      bool scanSmiles(Glib::ustring str, std::vector<TextPart> *textParts);
  };
};

#endif //HistoryTextView_h
