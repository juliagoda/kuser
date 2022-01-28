cmake_minimum_required(VERSION 3.16.0)
cmake_policy(SET CMP0063 NEW)
project(kuser)

set(VERSION "v17.0.0" )
set(RELEASE_NAME "Bleeding Edge")
set (QT5_MIN_VERSION "5.15")

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
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

include_directories (${QDBUS_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

include(CheckFunctionExists)
include(CheckIncludeFiles)
include(CheckLibraryExists)

include(TestBigEndian)

add_subdirectory(pics)
add_subdirectory(toolbar)
add_subdirectory(icon)
add_subdirectory(doc)

# use the project-specific _SOURCE_DIR variable (set by the project() call above), Alex
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

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ku_config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/ku_config.h)


set(kuser_SRCS
	ku_mainwidget.cpp
	ku_mainview.cpp
	ku_edituser.cpp
	ku_editgroup.cpp
	ku_usermodel.cpp
	ku_groupmodel.cpp
	ku_deluser.cpp
	ku_user.cpp
	ku_group.cpp
	ku_main.cpp
	ku_misc.cpp
	ku_pwdlg.cpp
	ku_adduser.cpp
	ku_configdlg.cpp
	ku_global.cpp
	sid.cpp
	ku_userfiles.cpp
	ku_groupfiles.cpp
	ku_userldap.cpp
	ku_groupldap.cpp
	ku_usersystem.cpp
	ku_groupsystem.cpp
	ku_selectconn.cpp
)

kconfig_add_kcfg_files(kuser_SRCS ku_prefs.kcfgc)

qt5_wrap_ui(kuser_SRCS ku_filessettings.ui ku_generalsettings.ui ku_ldapsettings.ui ku_ldapsamba.ui ku_passwordpolicy.ui)

add_executable(kuser ${kuser_SRCS})

target_link_libraries(kuser KF5::DocTools KF5::KIOCore KF5::KIOFileWidgets KF5::KIOWidgets KF5::KIONTLM KF5::Ldap KF5::Su Qt5::Widgets KF5::Completion KF5::I18n KF5::CoreAddons KF5::XmlGui KF5::ConfigCore KF5::ConfigGui)
if(HAVE_CRYPT_LIBRARY)
	target_link_libraries(kuser crypt)
endif(HAVE_CRYPT_LIBRARY)

message("INSTALL_TARGETS_DEFAULT_ARGS PATH: ${INSTALL_TARGETS_DEFAULT_ARGS}")
install(TARGETS kuser ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############
message("XDG_APPS_INSTALL_DIR PATH: ${XDG_APPS_INSTALL_DIR}")
message("KCFG_INSTALL_DIR PATH: ${KCFG_INSTALL_DIR}")

if(BUILD_WITH_QT6)
    install(PROGRAMS kuser.desktop DESTINATION share/applications)
    install(FILES kuserui.rc DESTINATION ${DATA_INSTALL_DIR}/kuser)
    install(FILES kuser.kcfg  DESTINATION share/config.kcfg)
else()
    install(PROGRAMS kuser.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
    install(FILES kuserui.rc DESTINATION ${DATA_INSTALL_DIR}/kuser)
    install(FILES kuser.kcfg  DESTINATION ${KCFG_INSTALL_DIR})
endif()
