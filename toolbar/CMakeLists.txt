message("DATA INSTALL DIR: ${DATA_INSTALL_DIR}")

if(BUILD_WITH_QT6)
    ecm_install_icons( ICONS 22-actions-user-group-delete.png 22-actions-user-group-properties.png DESTINATION share/kuser/icons  )
else()
    ecm_install_icons( ICONS 22-actions-user-group-delete.png 22-actions-user-group-properties.png DESTINATION ${DATA_INSTALL_DIR}/kuser/icons  )
endif()
