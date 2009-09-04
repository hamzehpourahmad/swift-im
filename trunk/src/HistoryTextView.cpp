/*
 *      HistoryTextView.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#include <pangomm/fontdescription.h>

#include "HistoryTextView.h"
#include "Application.h"
#include "MrimUtils.h"

using namespace Swift;

HistoryTextView::HistoryTextView() {
  tagTable = Gtk::TextBuffer::TagTable::create();
  buffer = Gtk::TextBuffer::create(tagTable);
  set_buffer(buffer);
  ownTag = Gtk::TextBuffer::Tag::create();
  ownTag->property_foreground_set() = true;
  ownTag->property_foreground() = "#0000FF";
  ownTag->property_weight() = Pango::WEIGHT_BOLD;
  fromTag = Gtk::TextBuffer::Tag::create();
  fromTag->property_foreground_set() = true;
  fromTag->property_foreground() = "#FF0000";
  fromTag->property_weight() = Pango::WEIGHT_BOLD;
  underlinedTag = Gtk::TextBuffer::Tag::create();
  underlinedTag->property_underline() = Pango::UNDERLINE_ERROR;
  timeTag = Gtk::TextBuffer::Tag::create();
  timeTag->property_foreground_set() = true;
  timeTag->property_foreground() = "#7F7F7F";
  tagTable->add(ownTag);
  tagTable->add(fromTag);
  tagTable->add(timeTag);
  tagTable->add(underlinedTag);
  set_accepts_tab(false);
}

Glib::ustring HistoryTextView::getFormattedTime() {
  gchar dtbuf[128];
  time_t st;
  struct tm stm;
  time(&st);
  guint sz = strftime(dtbuf, sizeof(dtbuf), "%c", localtime (&st));
  Glib::ustring result;
  result = dtbuf;
  return result;
}

void HistoryTextView::insertTime() {
  buffer->insert_with_tag(buffer->end(), getFormattedTime() + "  ", timeTag);
}

void HistoryTextView::newLine() {
  buffer->insert(buffer->end(), "\r\n");
}

void HistoryTextView::addOwnMessage(guint32 messageId, Glib::ustring body) {
  MessageBounds newMessage;
  newMessage.beginMark = buffer->create_mark(buffer->end());
  newMessage.endMark = buffer->create_mark(buffer->end());
  insertTime();
  buffer->insert_with_tag(buffer->end(), Application::getAppInstance()->mClient.userInfo["MRIM.NICKNAME"] + ": ", ownTag);
  buffer->move_mark(newMessage.beginMark, buffer->end());
  buffer->insert_with_tag(buffer->end(), body, underlinedTag);
  buffer->move_mark(newMessage.endMark, buffer->end());
  newLine();
  history[messageId] = newMessage;
}

void HistoryTextView::addReceivedMessage(Glib::ustring from, Glib::ustring body) {
  MrimContact c = MrimUtils::getContactByAddress(&Application::getAppInstance()->mClient, from);
  Glib::ustring author = from;
  if(c.index != MRIMUTILS_CONTACT_NOT_FOUND) {
    author = c.nickname;
  }
  insertTime();
  buffer->insert_with_tag(buffer->end(), author + ": ", fromTag);
  buffer->insert(buffer->end(), body);
  newLine();
}

void HistoryTextView::confirmMessage(guint32 messageId) {
  history[messageId].beginMark->get_iter();
  if(history[messageId].beginMark) {
    buffer->remove_all_tags(history[messageId].beginMark->get_iter(), history[messageId].endMark->get_iter());
  }
}

void HistoryTextView::rejectMessage(guint32 messageId, Glib::ustring reason) {
  if(history[messageId].beginMark) {
    Glib::ustring message = buffer->get_text(history[messageId].beginMark->get_iter(), history[messageId].endMark->get_iter());
    buffer->insert_with_tag(buffer->end(), "Maybe your message: \n" + message + "\nwas not delivered. Server answer: " + reason, underlinedTag);
    newLine();
  }
}
