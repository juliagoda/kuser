message("ICON INSTALL DIR: ${ICON_INSTALL_DIR}")

if(BUILD_WITH_QT6)
    ecm_install_icons( share/icons   )
else()
    ecm_install_icons( ${ICON_INSTALL_DIR}   )
endif()
