# src/CMakeLists.txt - this file is part of Swift-IM, cross-platform IM client for Mail.ru
# Copyright (c) 2009 Кожаев Галымжан <kozhayev(at)gmail(dot)com>

#checking pkg_config presence
if(NOT PKG_CONFIG_FOUND)
  MESSAGE(FATAL_ERROR "pkg-config not found!")
endif(NOT PKG_CONFIG_FOUND)

#checking required GTK and GTKMM libraries
pkg_check_modules(GTKMM REQUIRED gtkmm-2.4>=2.16)
pkg_check_modules(GTHREAD REQUIRED gthread-2.0>=2.20)
pkg_check_modules(LIBGLADEMM REQUIRED libglademm-2.4>=2.6)

#checking presence of some header files
CHECK_INCLUDE_FILE_CXX(locale.h HAVE_LOCALE_H)

#generate 'configure.h' header
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/Configure.h.cmake" "${CMAKE_CURRENT_SOURCE_DIR}/Configure.h")

#include directories containing header files
INCLUDE_DIRECTORIES(${GTKMM_INCLUDE_DIRS} ${GTHREAD_INCLUDE_DIRS} ${LIBGLADEMM_INCLUDE_DIRS})

#include linker directories
LINK_DIRECTORIES(${GTKMM_LIBRARY_DIRS} ${GTHREAD_LIBRARY_DIRS} ${LIBGLADEMM_LIBRARY_DIRS})

#adding sources
SET(PROJECT_SOURCES
    AboutDialog.cpp
    Application.cpp
    ChatTabs.cpp
    ChatWindow.cpp
    ContactsTreeColumns.cpp
    ContactsTreeWidget.cpp
    HistoryTextView.cpp
    HttpClient.cpp
    LoginDialog.cpp
    MainWindow.cpp
    MessageTextView.cpp
    MrimClient.cpp
    MrimConnection.cpp
    MrimContact.cpp
    MrimGroup.cpp
    MrimLoggedUser.cpp
    MrimPacket.cpp
    MrimSignalServer.cpp
    MrimUtils.cpp
    StatusComboColumns.cpp
    StatusComboWidget.cpp
    swift-im.cpp
    Utils.cpp
    )
SET(PROJECT_HEADERS
    AboutDialog.h
    Application.h
    ChatTabs.h
    ChatWindow.h
    Configure.h
    ContactsTreeColumns.h
    ContactsTreeWidget.h
    HistoryTextView.h
    HttpClient.h
    LoginDialog.h
    MainWindow.h
    MessageTextView.h
    MrimClient.h
    MrimConnection.h
    MrimContact.h
    MrimGroup.h
    MrimLoggedUser.h
    MrimPacket.h
    MrimSignalServer.h
    MrimUtils.h
    Protocol.h
    SocketHeaders.h
    StatusComboColumns.h
    StatusComboWidget.h
    Utils.h
    )

#adding target executable
ADD_EXECUTABLE(swift-im ${PROJECT_SOURCES} ${PROJECT_HEADERS})

#linking executable
TARGET_LINK_LIBRARIES(swift-im ${GTKMM_LIBRARIES})
TARGET_LINK_LIBRARIES(swift-im ${GTHREAD_LIBRARIES})
TARGET_LINK_LIBRARIES(swift-im ${LIBGLADEMM_LIBRARIES})

# linking with winsock2 and adding ld flags
# this case will be executed only in Win platforms
if(WIN32)
  TARGET_LINK_LIBRARIES(swift-im "ws2_32")
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}${GTKMM_LDFLAGS}${GTHREAD_LDFLAGS}${LIBGLADEMM_LDFLAGS}")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
endif(WIN32)

INSTALL(TARGETS swift-im RUNTIME DESTINATION bin)
