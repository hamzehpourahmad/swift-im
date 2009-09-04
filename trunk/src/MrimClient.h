/*
 *      MrimClient.h - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#ifndef MrimClient_h
#define MrimClient_h

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <map>

#include <glibmm/main.h>
#include <glibmm/ustring.h>
#include <gtkmm/image.h>

#include "Protocol.h"
#include "SocketHeaders.h"

namespace Swift {
  const gulong CONTACT_FETCH_DELAY = 100000;
  const gulong GROUP_FETCH_DELAY = 100000;
  const gulong SEND_WAIT_TIME = 1000;
  const Glib::ustring GET_IPPORT_HOST = "mrim.mail.ru";
  const gint GET_IPPORT_PORT = 2042;
  class MrimClient;

  struct MrimGroup {
    guint32 index;
    guint32 flag;
    Glib::ustring name;
  };

  struct MrimContact {
    guint32 index;
    guint32 flag;
    guint32 group;
    Glib::ustring address;
    Glib::ustring nickname;
    guint32 serverFlags;
    guint32 status;
    Gtk::Image* avatar;
  };

  struct MrimMessage {
    guint32 seq;
    time_t time;
  };

  typedef std::map <Glib::ustring, Glib::ustring> UserInfo;
  typedef std::vector <MrimGroup> GroupList;
  typedef std::vector <MrimContact> ContactList;
  typedef std::vector <MrimMessage> MessageList;

  class MrimClient {
    public:
      // methods
      MrimClient();
      virtual ~MrimClient();
      bool connect();
      void disconnect();
      void logout();
      Glib::ustring login(Glib::ustring login, Glib::ustring password, guint32 networkStatus);
      void changeStatus(guint32 newStatus);
      guint32 sendMessage(Glib::ustring destination, Glib::ustring message, guint32 flags);
      guint32 getUserStatus();

      // members
      UserInfo userInfo;
      GroupList groupList;
      ContactList contactList;

    private:
      // methods
      gint sendRawData(const char* data, guint length);
      gint recvRawData(char* data, guint length);
      bool sendHello();
      void startPing();
      void stopPing();
      void startListen();
      void stopListen();
      bool setDestinationAddress();
      bool sendHeader(guint32 packetType, guint32 dataLength);
      bool recvHeader(mrim_packet_header_t* buffer);
      bool sendLPS(Glib::ustring s);
      bool sendUL(guint32 x);
      static void pingCallback(MrimClient *client);
      bool listenCallback(Glib::IOCondition condition);
      bool connectionHangupCallback(Glib::IOCondition condition);
      guint32 readLPSFromSocket(Glib::ustring *buffer, Glib::ustring fromEncoding);
      UserInfo receiveUserInfo(guint32 dataLength);
      bool checkContactsReceived(guint32 *dataLength);
      guint32 receiveGroupsNumber(guint32 *dataLength);
      Glib::ustring receiveGroupMask(guint32 *dataLength);
      Glib::ustring receiveContactsMask(guint32 *dataLength);
      GroupList receiveGroupList(guint32 *dataLength);
      ContactList receiveContactList(guint32 *dataLength);
      void receiveChangedStatus();
      guint32 receiveConnectionParameters();
      guint32 receiveLogoutMessage();
      guint32 receiveMessageStatus(guint32 seq);
      void checkMessageListWithCallback();
      void sendMessageRecv(Glib::ustring from, guint32 msgId);
      void receiveMessage();

      // members
      bool connected, helloSent;
      bool isPingingNow, isSendingNow;
      gint sock;
      guint32 pingPeriod, commandNumber, groupsNumber, userStatus;
      Glib::ustring groupMask, contactsMask, userAgentString;
      MessageList messageList;
      sockaddr_in destinationAddress;
      mrim_packet_header_t packetHeader;
      GThread *pingThread;
      sigc::connection listenConnection;
  };
};

#endif //MrimClient_h
