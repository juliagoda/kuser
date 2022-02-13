cmake_minimum_required(VERSION 3.16.0)
cmake_policy(SET CMP0063 NEW)
project(kuser)

set(VERSION "v17.0.0" )
set (QT5_MIN_VERSION "5.12.8")

# search packages used by KDE
find_package(ECM 1.3.0 REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}/modules" ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})


if(BUILD_WITH_QT6 OR QT_VERSION_MAJOR EQUAL 6)
  message("QT6 has been choosen with BUILD_WITH_QT6")
  set(QT_VERSION_MAJOR 6 CACHE STRING "" FORCE)
  set(BUILD_WITH_QT6 ON CACHE BOOL "" FORCE)
elseif(BUILD_WITH_QT5 OR QT_VERSION_MAJOR EQUAL 5)
    message("QT5 has been choosen with BUILD_WITH_QT5")
  set(QT_VERSION_MAJOR 5 CACHE STRING "" FORCE)
  set(BUILD_WITH_QT5 ON CACHE BOOL "" FORCE)
else()
  # Automatically detect Qt version.
  find_package(QT NAMES Qt6 Qt5 COMPONENTS ${QT_COMPONENTS} REQUIRED)
  if(QT_FOUND AND QT_VERSION_MAJOR EQUAL 6)
    message("Automatically detected QT6")
    set(BUILD_WITH_QT6 ON CACHE BOOL "" FORCE)
    set(QT_VERSION_MAJOR 6 CACHE STRING "" FORCE)
  elseif(QT_FOUND AND QT_VERSION_MAJOR EQUAL 5)
    message("Automatically detected QT5")
    set(BUILD_WITH_QT5 ON CACHE BOOL "" FORCE)
    set(QT_VERSION_MAJOR 5 CACHE STRING "" FORCE)
  else()
    message(FATAL_ERROR "Missing Qt.")
  endif()
endif()

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0054 NEW)
endif(COMMAND cmake_policy)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(CheckIncludeFiles)
include(KDEInstallDirs)
include(ECMInstallIcons)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)

if(BUILD_WITH_QT6)
    message("Looking for Qt6 packages")
    find_package(Qt6 CONFIG REQUIRED COMPONENTS Widgets Gui Core)
else()
    message("Looking for Qt5 packages")
    find_package(Qt5 ${QT5_MIN_VERSION} CONFIG REQUIRED COMPONENTS Widgets Gui Core)
endif()

find_package(KF5 REQUIRED COMPONENTS KIO Su Ldap DocTools Completion I18n CoreAddons XmlGui Config)

include(CheckIncludeFiles)

add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )
add_definitions( -DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

include_directories (${QDBUS_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

include(CheckFunctionExists)
include(CheckIncludeFiles)
include(CheckLibraryExists)

include(TestBigEndian)

add_subdirectory(pics)
add_subdirectory(toolbar)
add_subdirectory(icon)
add_subdirectory(doc)

include_directories(${kuser_SOURCE_DIR})

set(KU_HOMEDIR_PERM 0700 CACHE STRING "Default home directory permissions")
set(KU_MAILBOX_GID 0 CACHE STRING "Default GID of the created mbox file")
set(KU_MAILBOX_PERM 0660 CACHE STRING "Default mbox file permissions")
set(KU_HOMETEMPLATE "/home/%u" CACHE STRING "Default home directory template")
set(KU_FIRSTUID 500 CACHE STRING "First UID of normal users")
set(KU_FIRSTGID 500 CACHE STRING "First GID of normal users")
set(KU_USERPRIVATEGROUP true CACHE STRING "User private groups by default")

check_include_files(shadow.h HAVE_SHADOW_H)
check_include_files(crypt.h HAVE_CRYPT_H)
check_include_files(sys/stat.h HAVE_SYS_STAT_H)
check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files(paths.h HAVE_PATHS_H)
check_library_exists(crypt crypt "" HAVE_CRYPT_LIBRARY)

set(CMAKE_REQUIRED_INCLUDES pwd.h)
check_function_exists(fgetpwent HAVE_FGETPWENT)
set(CMAKE_REQUIRED_INCLUDES grp.h)
check_function_exists(fgetgrent HAVE_FGETGRENT)
set(CMAKE_REQUIRED_INCLUDES shadow.h)
check_function_exists(fgetspent HAVE_FGETSPENT)

test_big_endian(WORDS_BIGENDIAN)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configs/ku_config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/ku_config.h)

add_subdirectory(src)
