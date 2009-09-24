/*
 *      HistoryTextView.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#include <string>

#include <pangomm/fontdescription.h>

#include "HistoryTextView.h"
#include "Application.h"
#include "MrimUtils.h"
#include <gtkmm/image.h>
#include <gtkmm/stock.h>

using namespace Glib;
using namespace Swift;


HistoryTextView::HistoryTextView() {
  appInstance->logEvent("HistoryTextView::HistoryTextView()", SEVERITY_DEBUG);
  // get default buffer
  buffer = get_buffer();

  // creating tags
  Glib::RefPtr<Gtk::TextBuffer::Tag> refTag;

  refTag = buffer->create_tag("myName");
  refTag->set_property("foreground_set", true);
  refTag->set_property("foreground", Glib::ustring("#0000FF"));
  refTag->set_property("weight", Pango::WEIGHT_BOLD);

  refTag = buffer->create_tag("companionName");
  refTag->set_property("foreground_set", true);
  refTag->set_property("foreground", Glib::ustring("#FF0000"));
  refTag->set_property("weight", Pango::WEIGHT_BOLD);

  refTag = buffer->create_tag("underline");
  refTag->set_property("underline", Pango::UNDERLINE_ERROR);

  refTag = buffer->create_tag("time");
  refTag->set_property("foreground_set", true);
  refTag->set_property("foreground", Glib::ustring("#7F7F7F"));
  set_accepts_tab(false);
}

Glib::ustring HistoryTextView::getFormattedTime() {
  appInstance->logEvent("HistoryTextView::getFormattedTime()", SEVERITY_DEBUG);
  gchar dtbuf[128];
  time_t st;
  time(&st);
  guint sz = strftime(dtbuf, sizeof(dtbuf), "%c", localtime (&st));
  return Glib::ustring(dtbuf);
}

void HistoryTextView::insertTime() {
  buffer->insert_with_tag(buffer->end(), getFormattedTime() + "  ", "time");
}

void HistoryTextView::newLine() {
  buffer->insert(buffer->end(), "\r\n");
}

void HistoryTextView::addOwnMessage(guint32 messageId, Glib::ustring body) {
  appInstance->logEvent("HistoryTextView::addOwnMessage()", SEVERITY_DEBUG);
  MessageBounds newMessage;
  newMessage.beginMark = buffer->create_mark(buffer->end());
  newMessage.endMark = buffer->create_mark(buffer->end());
  insertTime();
  buffer->insert_with_tag(buffer->end(), appInstance->mUser->getNickname() + ": ", "myName");
  buffer->move_mark(newMessage.beginMark, buffer->end());
  addMessage(body, true);
  buffer->move_mark(newMessage.endMark, buffer->end());
  history[messageId] = newMessage;
}

void HistoryTextView::addReceivedMessage(Glib::ustring from, Glib::ustring body) {
  appInstance->logEvent("HistoryTextView::addReceivedMessage()", SEVERITY_DEBUG);
  MrimContact c = appInstance->mUser->getContact(from);
  Glib::ustring author = from;
  if(c.getIndex() != 0) {
    author = c.getNickname();
  }
  insertTime();
  buffer->insert_with_tag(buffer->end(), author + ": ", "companionName");
  addMessage(body, false);
}

void HistoryTextView::addMessage(Glib::ustring msg, bool me) {
  appInstance->logEvent("HistoryTextView::addMessage()", SEVERITY_DEBUG);
  msg += "\r\n";
  std::vector<TextPart> parts;
  if(scanSmiles(msg, &parts)) {
    for(gint i = 0; i < parts.size(); i++) {
      bool insertText = true;
      if(parts[i].smile) {
        Glib::RefPtr<Gdk::PixbufAnimation> smilePba = appInstance->getSmileImage(parts[i].text);
        if(smilePba) {
          Gtk::Image* smileImg = manage(new Gtk::Image(smilePba));
          Glib::RefPtr<Gtk::TextChildAnchor> refAnchor = buffer->create_child_anchor(buffer->end());
          add_child_at_anchor(*smileImg, refAnchor);
          smileImg->show_all();
          insertText = false;
        }
      }
      if(insertText) {
        if(me) {
          buffer->insert_with_tag(buffer->end(), parts[i].text, "underline");
        }
        else {
          buffer->insert(buffer->end(), parts[i].text);
        }
      }
    }
  }
  else {
    if(me) {
      buffer->insert_with_tag(buffer->end(), msg, "underline");
    }
    else {
      buffer->insert(buffer->end(), msg);
    }
  }
}

void HistoryTextView::confirmMessage(guint32 messageId) {
  appInstance->logEvent("HistoryTextView::confirmMessage()", SEVERITY_DEBUG);
  history[messageId].beginMark->get_iter();
  if(history[messageId].beginMark) {
    buffer->remove_all_tags(history[messageId].beginMark->get_iter(), history[messageId].endMark->get_iter());
  }
}

void HistoryTextView::rejectMessage(guint32 messageId, Glib::ustring reason) {
  appInstance->logEvent("HistoryTextView::rejectMessage()", SEVERITY_DEBUG);
  if(history[messageId].beginMark) {
    Glib::ustring message = buffer->get_text(history[messageId].beginMark->get_iter(), history[messageId].endMark->get_iter());
    buffer->insert_with_tag(buffer->end(), ustring::compose(_("Maybe your message: \n%1\nwas not delivered. Server answer: %2"), message, reason), "underline");
    newLine();
  }
}

bool HistoryTextView::scanSmiles(Glib::ustring str, std::vector<TextPart> *textParts) {
  const gchar MRIM_SMILES_REGEXP[] = "<###[0-9]+###img[0-9]+>|<SMILE>\\s*id\\s*=\\s*([0-9]+)\\s+alt\\s*=\\s*'.+?'\\s*</SMILE>";
  TextPart part;
  std::string rawStr = str.raw();
  GError* err = NULL;
  GMatchInfo* matchInfo;
  GRegexCompileFlags cmpFlags = (GRegexCompileFlags)(G_REGEX_CASELESS | G_REGEX_MULTILINE | G_REGEX_DOTALL);
  GRegexMatchFlags matchFlags = (GRegexMatchFlags)0;
  GRegex* regex = g_regex_new(MRIM_SMILES_REGEXP, cmpFlags, matchFlags, NULL);
  g_regex_match(regex, str.c_str(), matchFlags, &matchInfo);
  gint curPos = 0;
  while(g_match_info_matches(matchInfo)) {
    gint start = -1, end = -1;
    g_match_info_fetch_pos(matchInfo, 0, &start, &end);
    gchar* matchedStr = g_match_info_fetch(matchInfo, 0);
    gchar* matchedStr1 = g_match_info_fetch(matchInfo, 1);
    if(start != -1 && end != -1) {
      // first add text
      part.text = rawStr.substr(curPos, start - curPos);
      part.smile = false;
      textParts->push_back(part);

      // then add smile id
      if(g_strrstr(matchedStr, "<###") == NULL) {
        part.text = Glib::ustring(matchedStr1);
      }
      else {
        part.text = Glib::ustring(matchedStr);
      }
      part.smile = true;
      textParts->push_back(part);
      curPos = end;
    }
    g_free(matchedStr);
    g_free(matchedStr1);
    g_match_info_next(matchInfo, NULL);
  }
  // check for remaining text
  // for instance, this case will be true if str doesn't have smile tags at all
  if(curPos < rawStr.size() - 1) {
    part.text = rawStr.substr(curPos);
    part.smile = false;
    textParts->push_back(part);
  }
  g_match_info_free(matchInfo);
  g_regex_unref(regex);
  if(err != NULL) {
    appInstance->logEvent(Glib::ustring(err->message), SEVERITY_DEBUG);
    g_error_free(err);
    return false;
  }
  return true;
}
