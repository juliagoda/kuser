message("DATA INSTALL DIR: ${DATA_INSTALL_DIR}")

if(BUILD_WITH_QT6)
    ecm_install_icons( share/kuser/icons  )
else()
    ecm_install_icons( ${DATA_INSTALL_DIR}/kuser/icons  )
endif()
