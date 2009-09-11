/*
 *      MrimContact.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#include "MrimContact.h"

using namespace Swift;

MrimContact::MrimContact() {
  mIndex = 0;
  mFlag = 0;
  mServerFlags = 0;
  mStatus = 0;
}

MrimContact::MrimContact(guint32 index, guint32 flag, MrimGroup grp, Glib::ustring address, Glib::ustring nickname, guint32 serverFlags, guint32 status, Glib::RefPtr<Gdk::Pixbuf> avatar) {
  mIndex = index;
  mFlag = flag;
  mGroup = grp;
  mAddress = address;
  mNickname = nickname;
  mServerFlags = serverFlags;
  mStatus = status;
  mAvatar = avatar;
}

void MrimContact::setIndex(guint32 index) {
  mIndex = index;
}

void MrimContact::setFlag(guint32 flag) {
  mFlag = flag;
}

void MrimContact::setGroup(MrimGroup grp) {
  mGroup = grp;
}

void MrimContact::setAddress(Glib::ustring address) {
  mAddress = address;
}

void MrimContact::setNickname(Glib::ustring nickname) {
  mNickname = nickname;
}

void MrimContact::setServerFlags(guint32 flags) {
  mServerFlags = flags;
}

void MrimContact::setStatus(guint32 status) {
  mStatus = status;
}

void MrimContact::setAvatar(Glib::RefPtr<Gdk::Pixbuf> avatar) {
  mAvatar = avatar;
}

guint32 MrimContact::getIndex() {
  return mIndex;
}

guint32 MrimContact::getFlag() {
  return mFlag;
}

MrimGroup MrimContact::getGroup() {
  return mGroup;
}

Glib::ustring MrimContact::getAddress() {
  return mAddress;
}

Glib::ustring MrimContact::getNickname() {
  return mNickname;
}

guint32 MrimContact::getServerFlags() {
  return mServerFlags;
}

guint32 MrimContact::getStatus() {
  return mStatus;
}

Glib::RefPtr<Gdk::Pixbuf> MrimContact::getAvatar() {
  return mAvatar;
}
