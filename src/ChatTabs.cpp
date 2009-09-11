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
  Tabs::iterator it = getTab(pageNumber);
  if(it != tabs.end()) {
    MrimContact c = appInstance->mUser->getContact(it->address);
    setUrgencyHint(c, false);
    appInstance->chatWindow->set_title(it->tabCaption->get_text() + " - Swift-IM");
    appInstance->chatWindow->set_urgency_hint(false);
  }
}

/*
 * Returns tab associated with address or tabs.end() if no such tab exists
 */
Tabs::iterator ChatTabs::getTab(Glib::ustring address) {
  Tabs::iterator it;
  for(it = tabs.begin(); it != tabs.end(); it++) {
    if(it->address == address) {
      return it;
    }
  }
  return tabs.end();
}

/*
 * Returns tab that has specified pageNumber or tabs.end() if no such tab exists
 */
Tabs::iterator ChatTabs::getTab(gint pageNumber) {
  Gtk::Widget* child = get_nth_page(pageNumber);
  if(child) {
    for(Tabs::iterator it = tabs.begin(); it != tabs.end(); it++) {
      if(it->child == child) {
        return it;
      }
    }
  }
  return tabs.end();
}

/*
 * Returns current tab or tabs.end() if there isn't any tab
 */
Tabs::iterator ChatTabs::getCurrentTab() {
  return getTab(get_current_page());
}

/*
 * Closes all opened tabs.
 */
void ChatTabs::closeAll() {
  for(Tabs::iterator it = tabs.begin(); it != tabs.end(); it++) {
    closeTab(it);
  }
}

void ChatTabs::createTab(Glib::ustring contactAddress) {
  if(isCreated[contactAddress]) {
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
  MrimContact c = appInstance->mUser->getContact(contactAddress);
  /*
   * 1. Creating new widgets on new tab
   */
  HistoryTextView* historyTextView;
  MessageTextView* messageTextView;
  Gtk::Label *contactInfoLabel, *notifyLabel;

  Gtk::HBox* tabLabelBox = manage(new Gtk::HBox());
  Gtk::Label* tabCaption = manage(new Gtk::Label(c.getNickname()));
  Gtk::Image* closeImage = manage(new Gtk::Image(Gtk::Stock::CLOSE, Gtk::ICON_SIZE_MENU));
  closeImage->set_padding(0, 0);
  Gtk::Button* closeButton = manage(new Gtk::Button);

  Gtk::VBox* tabContentBox = manage(new Gtk::VBox());
  Gtk::VBox* contactInfoBox = manage(new Gtk::VBox());
  Gtk::VPaned* vPanel = manage(new Gtk::VPaned());

  Gtk::HBox* contactInfoUpperPart = manage(new Gtk::HBox());
  Gtk::Image* avatar = manage(new Gtk::Image(c.getAvatar()));
  Gtk::ScrolledWindow* historyTextScroll = manage(new Gtk::ScrolledWindow());
  Gtk::ScrolledWindow* messageTextScroll = manage(new Gtk::ScrolledWindow());
  if(widgetsCreated[contactAddress]) {
    Tabs::iterator it = getTab(contactAddress);
    historyTextView = it->historyText;
    messageTextView = it->messageText;
    contactInfoLabel = it->contactInfoLabel;
    notifyLabel = it->notifyLabel;
  }
  else {
    historyTextView = new HistoryTextView();
    messageTextView = new MessageTextView();
    contactInfoLabel = new Gtk::Label(c.getNickname() + " <" + c.getAddress() + ">\n\n" + MrimUtils::getContactStatusByCode(c.getStatus()));
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
  newTab.address = contactAddress;
  tabs.push_back(newTab);

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
  historyTextScroll->set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_ALWAYS);
  historyTextScroll->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
  messageTextScroll->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  messageTextScroll->set_shadow_type(Gtk::SHADOW_ETCHED_IN);

  // textviews
  historyTextView->set_editable(false);
  historyTextView->set_wrap_mode(Gtk::WRAP_WORD);
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

  contactInfoUpperPart->pack_start(*avatar, false, false, 5);
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
  closeButton->signal_clicked().connect(sigc::bind<Tabs::iterator>(sigc::mem_fun(*this, &ChatTabs::onCloseTabClicked), tabs.end() - 1));
  historyTextScroll->get_vadjustment()->signal_changed().connect(sigc::bind<Gtk::Adjustment*>(sigc::mem_fun(*this, &ChatTabs::onHistoryTextScrollChanged), historyTextScroll->get_vadjustment()));

  // 6. Show all stuff
  tabContentBox->show_all();
  tabLabelBox->show_all();

  // 7. Append created tab and set it reorderable
  append_page(*tabContentBox, *tabLabelBox);
  set_tab_reorderable(*tabContentBox);

  // set tab creation flag
  isCreated[contactAddress] = true;
}

void ChatTabs::onCloseTabClicked(Tabs::iterator it) {
  closeTab(it);
  // close window if there aren't tabs
  if(!get_n_pages()) {
    appInstance->chatWindow->hide();
  }
}

void ChatTabs::closeTab(Tabs::iterator it) {
  /*
  * Members of ChatTab struct won't be deleted. 
  * We will use these created widgets if tab for contactAddress will be created again.
  */
  if(isCreated[it->address]) {
    isCreated[it->address] = false;
    remove_page(page_num(*(it->child)));
    // don't delete child, because it's managed and gtkmm will take care of it
    // we must just nullify the pointer
    it->child = NULL;
  }
}

/*
 * Swicthes to tab associated with contactAddress
 */
void ChatTabs::showTab(Glib::ustring contactAddress) {
  if(!isCreated[contactAddress]) {
    // create tab
    createTab(contactAddress);
  }
  // set as current
  set_current_page(page_num(*(getTab(contactAddress)->child)));
}

void ChatTabs::notifyWriting(Glib::ustring contactAddress) {
  if(isCreated[contactAddress]) {
    MrimContact c = appInstance->mUser->getContact(contactAddress);
    Glib::ustring author = contactAddress;
    if(c.getIndex() != 0) {
      // if contact found
      author = c.getNickname();
    }
    getTab(contactAddress)->notifyLabel->set_label("<small>" + author + " is typing a message</small>");
    sigc::slot<bool> timeoutSlot = sigc::bind(sigc::mem_fun(*this, &ChatTabs::onNotifyWritingExpire), contactAddress);
    Glib::signal_timeout().connect(timeoutSlot, NOTIFY_TIMEOUT_INTERVAL);
  }
}

bool ChatTabs::onNotifyWritingExpire(Glib::ustring contactAddress) {
  if(getTab(contactAddress)->notifyLabel != NULL) {
    getTab(contactAddress)->notifyLabel->set_label("");
  }
  return false;
}

/*
 * This handler adds autoscroll feature
 */
void ChatTabs::onHistoryTextScrollChanged(Gtk::Adjustment* adjustment) {
  adjustment->set_value(adjustment->get_upper());
}

/*
 * We must delete pointers stored in ChatTab.
 * Note that ChatTab.child and ChatTab.tabCaption are managed, therefore
 *  gtkmm will delete these widgets automatically.
 */
ChatTabs::~ChatTabs() {
  Tabs::iterator it;
  for(it = tabs.begin(); it != tabs.end(); it++) {
    delete it->historyText;
    delete it->messageText;
    delete it->notifyLabel;
    delete it->contactInfoLabel;
  }
}

void ChatTabs::updateStatus(MrimContact contact) {
  if(isCreated[contact.getAddress()]) {
    getTab(contact.getAddress())->contactInfoLabel->set_label(contact.getNickname() + " <" + contact.getAddress() + ">\n\n" + MrimUtils::getContactStatusByCode(contact.getStatus()));
  }
}

void ChatTabs::switchTab() {
  set_current_page((get_current_page() + 1) % get_n_pages());
}

/*
 * Make tab for contact urgently (i.e. make caption bolder) or not depending on urgent param
 */
void ChatTabs::setUrgencyHint(MrimContact contact, bool urgent) {
  if(isCreated[contact.getAddress()]) {
    bool opened = getCurrentTab()->address == contact.getAddress();
    // set urgent only if tab isn't opened
    if(urgent && !opened) {
      getTab(contact.getAddress())->tabCaption->set_label("<b>" + contact.getNickname() + "</b>");
    }
    // 'deurgent' only if tab is opened
    else if(!urgent && opened) {
      getTab(contact.getAddress())->tabCaption->set_label(contact.getNickname());
    }
  }
}
