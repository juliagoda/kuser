message("DATA INSTALL DIR: ${DATA_INSTALL_DIR}")

if(BUILD_WITH_QT6)
    install( FILES group.png user.png   DESTINATION  share/kuser/pics/ )
else()
    install( FILES group.png user.png   DESTINATION  ${DATA_INSTALL_DIR}/kuser/pics/ )
endif()
