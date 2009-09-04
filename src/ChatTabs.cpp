/*
 *      ChatTabs.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#include <gtkmm/paned.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <gtkmm/stock.h>
#include <gtkmm/image.h>
#include <gtkmm/alignment.h>
#include <gtkmm/eventbox.h>

#include "Application.h"
#include "ChatTabs.h"
#include "MrimUtils.h"

using namespace Swift;

ChatTabs::ChatTabs(BaseObjectType* baseObject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade) : Gtk::Notebook(baseObject) {
  // connecting signals
  signal_switch_page().connect(sigc::mem_fun(*this, &ChatTabs::onSwitchTab));
}

void ChatTabs::onSwitchTab(GtkNotebookPage *page, gint pageNumber) {
  Gtk::Label* tabCaption;
  Gtk::Widget* child = get_current()->get_child();
  MrimContact c = MrimUtils::getContactByAddress(&Application::getAppInstance()->mClient, tabMap[child]);
  setUrgencyHint(c, false);
  Gtk::HButtonBox* box = (Gtk::HButtonBox*) get_current()->get_tab_label();
  tabCaption = (Gtk::Label*) box->children().begin()->get_widget();
  Application::getAppInstance()->chatWindow->set_title(tabCaption->get_text() + " - Swift-IM");
  Application::getAppInstance()->chatWindow->set_urgency_hint(false);
}

void ChatTabs::createTab(Glib::ustring contactAddress) {
  if(isCreated[contactAddress] && tabs[contactAddress].child != NULL) {
    /*
     * Tab already created
     */
    return;
  }
  /*
   * Here is the hierarchy of widgets displayed on each tab
   * Note that "Send" button is the same for all tabs
   *
   * tabLabelBox (hbox)
   *    - tabCaption (Label) Displays nickname as a tab title
   *    - closeButton (Button)  Displays tab closing button
   * tabContentBox (VBox)
   *    - contactInfoBox (VBox)
   *        - contactInfoUpperPart (HBox)
   *            - avatar (Image)  Displays contact small avatar
   *            - contactInfoLabel (Label) Displays contact info containing nickname + email, status text (or x-status)
   *        - notifyLabel (Label) Displays typing notify message (e.g. "Contact "Someone" is typing a message")
   *    - vPanel (VPaned)
   *        - historyTextScroll (ScrolledWindow)
   *            - historyTextView (HistoryTextView) Displays message history
   *        - messageTextScroll (ScrolledWindow)
   *            - messageTextView (TextView) Type messages here
   *
   */
  ChatTab newTab;
  MrimContact c = MrimUtils::getContactByAddress(&Application::getAppInstance()->mClient, contactAddress);
  /*
   * 1. Creating new widgets on new tab
   */
  HistoryTextView* historyTextView;
  MessageTextView* messageTextView;
  Gtk::Label *contactInfoLabel, *notifyLabel;

  Gtk::HBox* tabLabelBox = manage(new Gtk::HBox());
  Gtk::Label* tabCaption = manage(new Gtk::Label(c.nickname));
  Gtk::Image* closeImage = manage(new Gtk::Image(Gtk::Stock::CLOSE, Gtk::ICON_SIZE_MENU));
  Gtk::Button* closeButton = manage(new Gtk::Button);

  Gtk::VBox* tabContentBox = manage(new Gtk::VBox());
  Gtk::VBox* contactInfoBox = manage(new Gtk::VBox());
  Gtk::VPaned* vPanel = manage(new Gtk::VPaned());

  Gtk::HBox* contactInfoUpperPart = manage(new Gtk::HBox());

  Gtk::ScrolledWindow* historyTextScroll = manage(new Gtk::ScrolledWindow());
  Gtk::ScrolledWindow* messageTextScroll = manage(new Gtk::ScrolledWindow());
  if(widgetsCreated[contactAddress]) {
    historyTextView = tabs[contactAddress].historyText;
    messageTextView = tabs[contactAddress].messageText;
    contactInfoLabel = tabs[contactAddress].contactInfoLabel;
    notifyLabel = tabs[contactAddress].notifyLabel;
  }
  else {
    historyTextView = new HistoryTextView();
    messageTextView = new MessageTextView();
    contactInfoLabel = new Gtk::Label(c.nickname + " <" + c.address + ">\n\n" + MrimUtils::getContactStatusByCode(c.status));
    notifyLabel = new Gtk::Label();
    widgetsCreated[contactAddress] = true;
  }

  /*
   * 2. Store created widgets and page
   */
  newTab.historyText = historyTextView;
  newTab.messageText = messageTextView;
  newTab.notifyLabel = notifyLabel;
  newTab.contactInfoLabel = contactInfoLabel;
  newTab.child = tabContentBox;
  newTab.tabCaption = tabCaption;
  tabs[contactAddress] = newTab;

  /*
   * 3. Setting widget properties
   */

  /*
   * vertical panel
   */
  vPanel->set_position(DEFAULT_VPANEL_POSITION);

  /*
   * close button
   */
  closeButton->add(*closeImage);
  closeButton->set_relief(Gtk::RELIEF_NONE);
  closeButton->set_focus_on_click(false);

  /*
   * Notify label
   */
  notifyLabel->set_use_markup(true);

  /*
   * Tab caption label
   */
  tabCaption->set_use_markup(true);

  // scrolled windows
  historyTextScroll->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  historyTextScroll->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
  messageTextScroll->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  messageTextScroll->set_shadow_type(Gtk::SHADOW_ETCHED_IN);

  // textviews
  historyTextView->set_editable(false);
  historyTextView->set_right_margin(DEFAULT_TEXTVIEW_MARGIN);
  historyTextView->set_left_margin(DEFAULT_TEXTVIEW_MARGIN);
  messageTextView->set_right_margin(DEFAULT_TEXTVIEW_MARGIN);
  messageTextView->set_left_margin(DEFAULT_TEXTVIEW_MARGIN);

  // 4. Packing widgets

  // packing tab label into hbox
  tabLabelBox->pack_start(*tabCaption);
  tabLabelBox->pack_start(*closeButton);

  // packing text views into scrolled windows
  historyTextScroll->add(*historyTextView);
  messageTextScroll->add(*messageTextView);

  // constructing contact info box

  contactInfoUpperPart->pack_start(*c.avatar, false, false, 5);
  contactInfoUpperPart->pack_start(*contactInfoLabel, false, false, 5);
  contactInfoBox->pack_start(*contactInfoUpperPart, false, false, 5);
  contactInfoBox->pack_start(*notifyLabel, false, false, 5);

  // packing scrolled windows into vertical panel
  vPanel->add1(*historyTextScroll);
  vPanel->add2(*messageTextScroll);

  // packing contact info box AND vertical panel into tab content box
  tabContentBox->pack_start(*contactInfoBox, false, false, 0);
  tabContentBox->pack_start(*vPanel);

  // 5. Connecting signals
  /*
   * closeButton's signal with extra argument
   */
  closeButton->signal_clicked().connect(sigc::bind<Glib::ustring>(sigc::mem_fun(*this, &ChatTabs::onCloseTabClicked), contactAddress));
  historyTextScroll->get_vadjustment()->signal_changed().connect(sigc::bind<Gtk::Adjustment*>(sigc::mem_fun(*this, &ChatTabs::onHistoryTextScrollChanged), historyTextScroll->get_vadjustment()));

  // 6. Show all stuff
  tabContentBox->show_all();
  tabLabelBox->show_all();

  // 7. Append created tab and set it reorderable
  append_page(*tabContentBox, *tabLabelBox);
  set_tab_reorderable(*tabContentBox);

  // 8. Store created tab into map
  tabMap[tabContentBox] = contactAddress;

  // set tab creation flag
  isCreated[contactAddress] = true;
}

void ChatTabs::onCloseTabClicked(Glib::ustring contactAddress) {
  closeTab(contactAddress);
  // close window if there aren't tabs
  if(!get_n_pages()) {
    Application::getAppInstance()->chatWindow->hide();
  }
}

void ChatTabs::closeTab(Glib::ustring contactAddress) {
  /*
  * Member widgets of ChatTab struct won't be deleted
  */
  if(isCreated[contactAddress] && tabs[contactAddress].child != NULL) {
    isCreated[contactAddress] = false;
    remove_page(page_num(*tabs[contactAddress].child));
    tabMap[tabs[contactAddress].child].clear();
    // don't delete child, because it's managed and gtkmm will take care of it
    // we must just nullify the pointer
    tabs[contactAddress].child = NULL;
  }
}

void ChatTabs::showTab(Glib::ustring contactAddress) {
  if(!isCreated[contactAddress] || tabs[contactAddress].child == NULL) {
    // create tab
    createTab(contactAddress);
  }
  // set as current
  set_current_page(page_num(*tabs[contactAddress].child));
}

void ChatTabs::notifyWriting(Glib::ustring contactAddress) {
  if(isCreated[contactAddress] && tabs[contactAddress].notifyLabel != NULL) {
    MrimContact c = MrimUtils::getContactByAddress(&Application::getAppInstance()->mClient, contactAddress);
    Glib::ustring author = contactAddress;
    if(c.index != MRIMUTILS_CONTACT_NOT_FOUND) {
      author = c.nickname;
    }
    tabs[contactAddress].notifyLabel->set_label("<small>" + author + " is typing a message</small>");
    sigc::slot<bool> timeoutSlot = sigc::bind(sigc::mem_fun(*this, &ChatTabs::onNotifyWritingExpire), contactAddress);
    Glib::signal_timeout().connect(timeoutSlot, NOTIFY_TIMEOUT_INTERVAL);
  }
}

bool ChatTabs::onNotifyWritingExpire(Glib::ustring contactAddress) {
  if(tabs[contactAddress].notifyLabel != NULL) {
    tabs[contactAddress].notifyLabel->set_label("");
  }
  return false;
}

void ChatTabs::onHistoryTextScrollChanged(Gtk::Adjustment* adjustment) {
  adjustment->set_value(adjustment->get_upper());
}

ChatTabs::~ChatTabs() {
  Tabs::iterator it;
  for(it = tabs.begin(); it != tabs.end(); it++) {
    delete it->second.historyText;
    delete it->second.messageText;
    delete it->second.notifyLabel;
    delete it->second.contactInfoLabel;
  }
}

void ChatTabs::updateStatus(MrimContact contact) {
  if(isCreated[contact.address] && tabs[contact.address].contactInfoLabel != NULL) {
    tabs[contact.address].contactInfoLabel->set_label(contact.nickname + " <" + contact.address + ">\n\n" + MrimUtils::getContactStatusByCode(contact.status));
  }
}

void ChatTabs::switchTab() {
  set_current_page((get_current_page() + 1) % get_n_pages());
}

void ChatTabs::setUrgencyHint(MrimContact contact, bool urgent) {
  if(isCreated[contact.address] && tabs[contact.address].tabCaption != NULL) {
    // set urgent only if tab isn't opened
    if(urgent && tabMap[get_current()->get_child()] != contact.address) {
      tabs[contact.address].tabCaption->set_label("<b>" + contact.nickname + "</b>");
    }
    else if(!urgent) {
      tabs[contact.address].tabCaption->set_label(contact.nickname);
    }
  }
}
