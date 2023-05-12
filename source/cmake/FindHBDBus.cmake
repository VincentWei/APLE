# Copyright (C) 2023 Beijing FMSoft Technologies Co., Ltd.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1.  Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
# 2.  Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND ITS CONTRIBUTORS ``AS
# IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR ITS
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#[=======================================================================[.rst:
FindHBDBus
--------------

Find HBDBus headers and libraries.

Imported Targets
^^^^^^^^^^^^^^^^

``HBDBus::HBDBus``
  The HBDBus library, if found.

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables in your project:

``HBDBus_FOUND``
  true if (the requested version of) HBDBus is available.
``HBDBus_VERSION``
  the version of HBDBus.
``HBDBus_LIBRARIES``
  the libraries to link against to use HBDBus.
``HBDBus_INCLUDE_DIRS``
  where to find the HBDBus headers.
``HBDBus_COMPILE_OPTIONS``
  this should be passed to target_compile_options(), if the
  target is not used for linking

#]=======================================================================]

# TODO: Remove when cmake_minimum_version bumped to 3.14.

find_package(PkgConfig QUIET)
pkg_check_modules(PC_HBDBUS QUIET hbdbus)
set(HBDBus_COMPILE_OPTIONS ${PC_HBDBUS_CFLAGS_OTHER})
set(HBDBus_VERSION ${PC_HBDBUS_VERSION})

find_path(HBDBus_INCLUDE_DIR
    NAMES hbdbus/hbdbus.h
    HINTS ${PC_HBDBUS_INCLUDEDIR} ${PC_HBDBUS_INCLUDE_DIR}
)

find_library(HBDBus_LIBRARY
    NAMES ${HBDBus_NAMES} hbdbus
    HINTS ${PC_HBDBUS_LIBDIR} ${PC_HBDBUS_LIBRARY_DIRS}
)

if (HBDBus_INCLUDE_DIR AND NOT HBDBus_VERSION)
    if (EXISTS "${HBDBus_INCLUDE_DIR}/hbdbus/hbdbus-version.h")
        file(STRINGS ${HBDBus_INCLUDE_DIR}/hbdbus/hbdbus-version.h _ver_line
            REGEX "^#define HBDBUS_VERSION_STRING  *\"[0-9]+\\.[0-9]+\\.[0-9]+\""
            LIMIT_COUNT 1)
        string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+"
            HBDBus_VERSION "${_ver_line}")
        unset(_ver_line)
    endif ()
endif ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HBDBus
    FOUND_VAR HBDBus_FOUND
    REQUIRED_VARS HBDBus_LIBRARY HBDBus_INCLUDE_DIR
    VERSION_VAR HBDBus_VERSION
)

if (HBDBus_LIBRARY AND NOT TARGET HBDBus::HBDBus)
    add_library(HBDBus::HBDBus UNKNOWN IMPORTED GLOBAL)
    set_target_properties(HBDBus::HBDBus PROPERTIES
        IMPORTED_LOCATION "${HBDBus_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS "${HBDBus_COMPILE_OPTIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${HBDBus_INCLUDE_DIR}"
    )
endif ()

mark_as_advanced(HBDBus_INCLUDE_DIR HBDBus_LIBRARIES)

if (HBDBus_FOUND)
    set(HBDBus_LIBRARIES ${HBDBus_LIBRARY})
    set(HBDBus_INCLUDE_DIRS ${HBDBus_INCLUDE_DIR})
endif ()

