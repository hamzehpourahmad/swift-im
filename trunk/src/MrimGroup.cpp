/*
 *      MrimGroup.cpp - this file is part of Swift-IM, cross-platform IM client for Mail.ru
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

#include "MrimGroup.h"

using namespace Swift;

MrimGroup::MrimGroup() {
  mFlags = 0;
  mName = "";
  mIndex = 0;
}

MrimGroup::MrimGroup(guint32 flags, Glib::ustring name, guint32 index) {
  mFlags = flags;
  mName = name;
  mIndex = index;
}

guint32 MrimGroup::getFlags() {
  return mFlags;
}

guint32 MrimGroup::getIndex() {
  return mIndex;
}

Glib::ustring MrimGroup::getName() {
  return mName;
}

void MrimGroup::setFlags(guint32 flags) {
  mFlags = flags;
}

void MrimGroup::setIndex(guint32 index) {
  mIndex = index;
}

void MrimGroup::setName(Glib::ustring name) {
  mName = name;
}

void MrimGroup::addContact(MrimContact c) {
  mCl.push_back(c);
}

ContactList* MrimGroup::contacts() {
  return &mCl;
}

void MrimGroup::removeContact(guint32 contactIndex) {
  for(ContactList::iterator it = mCl.begin(); it != mCl.end(); it++) {
    if(it->getIndex() == contactIndex) {
      mCl.erase(it);
      break;
    }
  }
}

void MrimGroup::debugPrint() {
  printf("Index: %d\n", mIndex);
  printf("Flags: %d\n", mFlags);
  printf("Name : %s\n", mName.c_str());
  printf("Contacts number: %d\n", mCl.size());
  for(int i = 0; i < mCl.size(); i++) {
    mCl[i].debugPrint();
  }
}
