# CMakeLists.txt - this file is part of Swift-IM, cross-platform IM client for Mail.ru
# Copyright (c) 2009 Кожаев Галымжан <kozhayev(at)gmail(dot)com>

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(Swift-IM CXX)

#including some standard cmake modules
INCLUDE(FindPkgConfig)
INCLUDE(FindGettext)
INCLUDE(CheckIncludeFileCXX)

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

INSTALL(DIRECTORY swift-im DESTINATION share)
