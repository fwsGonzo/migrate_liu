cmake_minimum_required(VERSION 2.8.9)
# default IncludeOS location #FIXME#
if (NOT DEFINED ENV{INCLUDEOS_PREFIX})
  set(ENV{INCLUDEOS_PREFIX} /usr/local)
endif()
include($ENV{INCLUDEOS_PREFIX}/includeos/pre.service.cmake)
project(service)

set(SERVICE_NAME "Live migration service")
set(BINARY       "Migrate")
set(SOURCES
    service.cpp
  )
set(LOCAL_INCLUDES ".")

set(LIBRARIES ../../LiveUpdate/build/libliveupdate.a)

set(DRIVERS
    virtionet
    silent_start
  )

# include service build script
include($ENV{INCLUDEOS_PREFIX}/includeos/post.service.cmake)

diskbuilder(disk)

# Uncomment this to build vanilla:
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.2")
