
file(GLOB HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/*.h")

set(SOURCES
    ku_mainwidget.cpp
    ku_mainview.cpp
    ku_edituser.cpp
    ku_editgroup.cpp
    ku_usermodel.cpp
    ku_groupmodel.cpp
    ku_deluser.cpp
    ku_user.cpp
    ku_group.cpp
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

set(UI
    ${PROJECT_SOURCE_DIR}/ui/ku_filessettings.ui
    ${PROJECT_SOURCE_DIR}/ui/ku_generalsettings.ui
    ${PROJECT_SOURCE_DIR}/ui/ku_ldapsamba.ui
    ${PROJECT_SOURCE_DIR}/ui/ku_ldapsettings.ui
    ${PROJECT_SOURCE_DIR}/ui/ku_passwordpolicy.ui
)

kconfig_add_kcfg_files(SOURCES ${PROJECT_SOURCE_DIR}/configs/ku_prefs.kcfgc)

qt5_wrap_ui(UIC ${UI})

add_executable(kuser ku_main.cpp ${SOURCES} ${HEADERS} ${UIC})

target_include_directories(kuser PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/ui)

target_link_libraries(kuser PUBLIC KF5::DocTools KF5::KIOCore KF5::KIOFileWidgets KF5::KIOWidgets KF5::KIONTLM KF5::Ldap KF5::Su Qt5::Widgets KF5::Completion KF5::I18n KF5::CoreAddons KF5::XmlGui KF5::ConfigCore KF5::ConfigGui)

if(HAVE_CRYPT_LIBRARY)
        target_link_libraries(kuser PUBLIC crypt)
endif(HAVE_CRYPT_LIBRARY)

message("INSTALL_TARGETS_DEFAULT_ARGS PATH: ${INSTALL_TARGETS_DEFAULT_ARGS}")
install(TARGETS kuser ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############
message("XDG_APPS_INSTALL_DIR PATH: ${XDG_APPS_INSTALL_DIR}")
message("KCFG_INSTALL_DIR PATH: ${KCFG_INSTALL_DIR}")

if(BUILD_WITH_QT6)
    install(PROGRAMS ${PROJECT_SOURCE_DIR}/kuser.desktop DESTINATION share/applications)
    install(FILES ${PROJECT_SOURCE_DIR}/configs/kuserui.rc DESTINATION ${DATA_INSTALL_DIR}/kuser)
    install(FILES ${PROJECT_SOURCE_DIR}/configs/kuser.kcfg  DESTINATION share/config.kcfg)
else()
    install(PROGRAMS ${PROJECT_SOURCE_DIR}/kuser.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
    install(FILES ${PROJECT_SOURCE_DIR}/configs/kuserui.rc DESTINATION ${DATA_INSTALL_DIR}/kuser)
    install(FILES ${PROJECT_SOURCE_DIR}/configs/kuser.kcfg  DESTINATION ${KCFG_INSTALL_DIR})
endif()
