/*
 *      MrimLoggedUser.h - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#ifndef MrimLoggedUser_h
#define MrimLoggedUser_h

namespace Swift {
  class MrimLoggedUser;
};

#include <map>
#include <vector>

#include "MrimContact.h"
#include "MrimGroup.h"

namespace Swift {
  typedef std::map<Glib::ustring, Glib::ustring> UserInfo;
  typedef std::vector<MrimGroup> GroupList;
  typedef std::vector<MrimContact> ContactList;
  class MrimLoggedUser : public MrimContact {
    public:
      MrimLoggedUser();
      void authorize(Glib::ustring password);
      Glib::ustring getInfoRec(std::string key);
      bool logged();
      bool sendMessage(guint32 flags, Glib::ustring to, Glib::ustring message, Glib::ustring rtf, guint32 *messageId);
      void setStatus(guint32 status);
      MrimContact getContact(Glib::ustring address);
      MrimContact getContact(guint32 index);
      GroupList* getGroupList();
      ContactList* getContactList();

    private:
      UserInfo mInfo;
      bool mLogged;
      ContactList mContactList;
      GroupList mGroupList;

    protected:
      // signal handlers

      // mrim signals
      void onLoginAck();
      void onLoginRej(Glib::ustring reason);
      void onStatusChange(guint32 status, Glib::ustring address);
      void onContactListReceive(GroupList gl, ContactList cl);
      void onUserInfoReceive(UserInfo ui);
  };
};

#endif //MrimLoggedUser_h
