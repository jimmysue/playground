# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
FindLibYUV
---------

Find LibYUV includes and library.

Imported Targets
^^^^^^^^^^^^^^^^

An :ref:`imported target <Imported targets>` named
``LibYUV::LibYUV`` is provided if LibYUV has been found.

Result Variables
^^^^^^^^^^^^^^^^

This module defines the following variables:

``LibYUV_FOUND``
  True if LibYUV was found, false otherwise.
``LibYUV_INCLUDE_DIRS``
  Include directories needed to include LibYUV headers.
``LibYUV_LIBRARIES``
  Libraries needed to link to LibYUV.
``LibYUV_VERSION``
  The version of LibYUV found.
``LibYUV_VERSION_MAJOR``
  The major version of LibYUV.
``LibYUV_VERSION_MINOR``
  The minor version of LibYUV.
``LibYUV_VERSION_PATCH``
  The patch version of LibYUV.

Cache Variables
^^^^^^^^^^^^^^^

This module uses the following cache variables:

``LibYUV_LIBRARY``
  The location of the LibYUV library file.
``LibYUV_INCLUDE_DIR``
  The location of the LibYUV include directory containing ``uv.h``.

The cache variables should not be used by project code.
They may be set by end users to point at LibYUV components.
#]=======================================================================]

#-----------------------------------------------------------------------------
find_library(LibYUV_LIBRARY
  NAMES libyuv.a LibYUV
  )
mark_as_advanced(LibYUV_LIBRARY)

find_path(LibYUV_INCLUDE_DIR
  NAMES libyuv.h
  )
mark_as_advanced(LibYUV_INCLUDE_DIR)

#-----------------------------------------------------------------------------
# Extract version number if possible.
set(_LibYUV_H_REGEX "#[ \t]*define[ \t]+UV_VERSION_(MAJOR|MINOR|PATCH)[ \t]+[0-9]+")
if(LibYUV_INCLUDE_DIR AND EXISTS "${LibYUV_INCLUDE_DIR}/uv-version.h")
  file(STRINGS "${LibYUV_INCLUDE_DIR}/uv-version.h" _LibYUV_H REGEX "${_LibYUV_H_REGEX}")
elseif(LibYUV_INCLUDE_DIR AND EXISTS "${LibYUV_INCLUDE_DIR}/uv.h")
  file(STRINGS "${LibYUV_INCLUDE_DIR}/libyuv.h" _LibYUV_H REGEX "${_LibYUV_H_REGEX}")
else()
  set(_LibYUV_H "")
endif()
foreach(c MAJOR MINOR PATCH)
  if(_LibYUV_H MATCHES "#[ \t]*define[ \t]+UV_VERSION_${c}[ \t]+([0-9]+)")
    set(_LibYUV_VERSION_${c} "${CMAKE_MATCH_1}")
  else()
    unset(_LibYUV_VERSION_${c})
  endif()
endforeach()
if(DEFINED _LibYUV_VERSION_MAJOR AND DEFINED _LibYUV_VERSION_MINOR)
  set(LibYUV_VERSION_MAJOR "${_LibYUV_VERSION_MAJOR}")
  set(LibYUV_VERSION_MINOR "${_LibYUV_VERSION_MINOR}")
  set(LibYUV_VERSION "${LibYUV_VERSION_MAJOR}.${LibYUV_VERSION_MINOR}")
  if(DEFINED _LibYUV_VERSION_PATCH)
    set(LibYUV_VERSION_PATCH "${_LibYUV_VERSION_PATCH}")
    set(LibYUV_VERSION "${LibYUV_VERSION}.${LibYUV_VERSION_PATCH}")
  else()
    unset(LibYUV_VERSION_PATCH)
  endif()
else()
  set(LibYUV_VERSION_MAJOR "")
  set(LibYUV_VERSION_MINOR "")
  set(LibYUV_VERSION_PATCH "")
  set(LibYUV_VERSION "")
endif()
unset(_LibYUV_VERSION_MAJOR)
unset(_LibYUV_VERSION_MINOR)
unset(_LibYUV_VERSION_PATCH)
unset(_LibYUV_H_REGEX)
unset(_LibYUV_H)

#-----------------------------------------------------------------------------
include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibYUV
  FOUND_VAR LibYUV_FOUND
  REQUIRED_VARS LibYUV_LIBRARY LibYUV_INCLUDE_DIR
  VERSION_VAR LibYUV_VERSION
  )
set(LibYUV_FOUND ${LibYUV_FOUND})

#-----------------------------------------------------------------------------
# Provide documented result variables and targets.
if(LibYUV_FOUND)
  set(LibYUV_INCLUDE_DIRS ${LibYUV_INCLUDE_DIR})
  set(LibYUV_LIBRARIES ${LibYUV_LIBRARY})
  if(NOT TARGET LibYUV::LibYUV)
    add_library(LibYUV::LibYUV UNKNOWN IMPORTED)
    set_target_properties(LibYUV::LibYUV PROPERTIES
      IMPORTED_LOCATION "${LibYUV_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${LibYUV_INCLUDE_DIRS}"
      )
  endif()
endif()
