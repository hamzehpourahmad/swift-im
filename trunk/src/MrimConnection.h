/*
 *      MrimConnection.h - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#ifndef MrimConnection_h
#define MrimConnection_h

namespace Swift {
  class MrimConnection;
};

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <glibmm/main.h>
#include <glibmm/ustring.h>

#include "MrimPacket.h"
#include "Protocol.h"
#include "SocketHeaders.h"

namespace Swift {
  /* SignalConnectionHangup emitted when TCP connection lost.
   * Parameters:
   *  None
   * Handlers:
   *  - MrimConnection::onConnectionHangup()
   */
  typedef sigc::signal<void> SignalConnectionHangup;
  const gulong SEND_WAIT_TIME = 1000;
  const Glib::ustring GET_IPPORT_HOST = "mrim.mail.ru";
  const gint GET_IPPORT_PORT = 2042;

  class MrimConnection {
    public:
      // methods
      MrimConnection();
      bool connect();
      guint32 getCommandNumber();
      guint32 getPingPeriod();
      void updateCommandNumber();
      bool sendLPS(Glib::ustring s);
      bool sendUL(guint32 x);
      bool sendUIDL(UIDL x);
      bool sendHeader(mrim_packet_header_t header);
      guint32 readLPS(Glib::ustring *buffer, Glib::ustring fromEncoding);
      guint32 readUL();
      UIDL readUIDL();
      bool readHeader(mrim_packet_header_t *header);
      void readUnknownData(gchar *buffer, size_t len);
      SignalConnectionHangup signal_connection_hangup();

    private:
      // methods
      gint sendRawData(const char* data, guint length);
      gint recvRawData(char* data, guint length);
      void startPing();
      void stopPing();
      void startListen();
      void stopListen();
      bool setDestinationAddress();

      void pingCallback();
      bool listenCallback(Glib::IOCondition condition);

      // members
      bool mConnected, mIsPinging;
      gint mSock;
      guint32 mPingPeriod, mCommandNumber;
      sockaddr_in mDestinationAddress;
      Glib::Thread* mPingThread;
      sigc::connection mListenConnection;
      SignalConnectionHangup mSignalConnectionHangup;

    protected:
      void onConnectionHangup();
      // mrim signal handlers
      void onHelloAck(guint32 pingPeriod);
      void onLogout(guint32 reason);
      void onConnectionParamsReceive(guint32 pingPeriod);
      void onLoginAck();
  };
};

#endif //MrimConnection_h
