# po/CMakeLists.txt - this file is part of Swift-IM, cross-platform IM client for Mail.ru
# Copyright (c) 2009 Кожаев Галымжан <kozhayev(at)gmail(dot)com>

if(GETTEXT_FOUND AND NOT WIN32)
  SET(POT_FILE swift-im.pot)
  SET(PO_FILES ru.po)
  GETTEXT_CREATE_TRANSLATIONS(${POT_FILE} ${PO_FILES})
endif(GETTEXT_FOUND AND NOT WIN32)

