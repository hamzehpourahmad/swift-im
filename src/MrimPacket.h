/*
 *      MrimPacket.h - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#ifndef MrimPacket_h
#define MrimPacket_h

#include <vector>

#include <glibmm/ustring.h>

namespace Swift {
  class MrimPacket;
  struct UIDL {
    gchar s[8];
  };
  enum MrimDataType {
    MRIMDATATYPE_UL,
    MRIMDATATYPE_LPS,
    MRIMDATATYPE_UIDL
  };
  typedef std::vector<Glib::ustring> LPSArray;
  typedef std::vector<guint32> ULArray;
  typedef std::vector<UIDL> UIDLArray;
  typedef std::vector<MrimDataType> DATAArray;
};

#include "Protocol.h"
#include "MrimLoggedUser.h"
#include "MrimConnection.h"

namespace Swift {
  const gulong CONTACT_FETCH_DELAY = 100000;
  const gulong GROUP_FETCH_DELAY = 100000;
  class MrimPacket {
    public:
      MrimPacket();
      MrimPacket(guint32 type);
      bool validate();
      bool send();
      static MrimPacket receive();
      void setType(guint32 type);
      void addUL(guint32 ul);
      void addLPS(Glib::ustring lps);
      void addUIDL(UIDL uidl);
      mrim_packet_header_t getHeader();

    private:
      guint32 getDlen();
      guint32 getLPSLength(Glib::ustring lps);
      void prepareHeader();
      void receiveData();
      void receiveUserInfo(UserInfo *ui);
      void receiveContactList(GroupList *gl);
      void receiveStatus(guint32 *status, Glib::ustring *address);
      void receiveConnectionParameters(guint32 *pingPeriod);
      void receiveLogoutMessage(guint32 *reason);
      void receiveLoginRejectReason(Glib::ustring *reason);
      void receiveMessageStatus(guint32 *status);
      void receiveMessage(guint32 *msgid, guint32 *flags, Glib::ustring *from, Glib::ustring *message, Glib::ustring *rtf);
      void receiveUnknownData();

      guint32 mType;
      mrim_packet_header_t header;
      DATAArray dataQueue;
      LPSArray lpsList;
      ULArray ulList;
      UIDLArray uidlList;
      MrimConnection* connection;
  };
};

#endif //MrimPacket_h
