message("ICON INSTALL DIR: ${ICON_INSTALL_DIR}")

if(BUILD_WITH_QT6)
    ecm_install_icons( ICONS 16-apps-kuser.png 22-apps-kuser.png 32-apps-kuser.png 48-apps-kuser.png 64-apps-kuser.png 128-apps-kuser.png DESTINATION share/icons )
else()
    ecm_install_icons( ICONS 16-apps-kuser.png 22-apps-kuser.png 32-apps-kuser.png 48-apps-kuser.png 64-apps-kuser.png 128-apps-kuser.png DESTINATION ${ICON_INSTALL_DIR} )
endif()
