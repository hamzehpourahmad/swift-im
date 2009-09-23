# CMakeLists.txt - this file is part of Swift-IM, cross-platform IM client for Mail.ru
# Copyright (c) 2009 Кожаев Галымжан <kozhayev(at)gmail(dot)com>

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(Swift-IM CXX)

# including some standard cmake modules
INCLUDE(FindPkgConfig)
INCLUDE(FindGettext)
INCLUDE(CheckIncludeFileCXX)

IF(WIN32)
  # setting CPack settings for NSIS installer on Windows
  SET(CPACK_PACKAGE_VENDOR "")
  SET(CPACK_PACKAGE_NAME "Swift-IM")
  SET(CPACK_GENERATOR "NSIS")
  SET(CPACK_PACKAGE_FILE_NAME "swift-im-0.2-setup")
  SET(CPACK_SYSTEM_NAME "Win32")
  SET(CPACK_PACKAGE_VERSION 0.2)
  SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
  SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "Swift-IM")
  
  # NSIS-specific settings
  SET(CPACK_NSIS_COMPRESSOR "/SOLID lzma")
  SET(CPACK_PACKAGE_ICON "${CMAKE_BINARY_DIR}\\\\logo.bmp")
  
  # include CPack module
  INCLUDE(CPack)
ENDIF(WIN32)

if(CMAKE_BUILD_TYPE STREQUAL "")
  # use Release as default build type
  SET(CMAKE_BUILD_TYPE "Release")
endif(CMAKE_BUILD_TYPE STREQUAL "")

#execute src/CMakeLists.txt
ADD_SUBDIRECTORY(src)

#execute po/CMakeLists.txt
ADD_SUBDIRECTORY(po)

if(GETTEXT_FOUND AND NOT WIN32)
  ADD_DEPENDENCIES(swift-im translations)
endif(GETTEXT_FOUND AND NOT WIN32)

INSTALL(DIRECTORY swift-im/ DESTINATION share PATTERN ".svn" EXCLUDE)
