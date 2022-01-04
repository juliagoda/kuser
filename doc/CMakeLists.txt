########### install files ###############
#
#
message("HTML_INSTALL_DIR PATH: ${HTML_INSTALL_DIR}")

if(BUILD_WITH_QT6)
    kdoctools_create_handbook(index.docbook INSTALL_DESTINATION share/doc/HTML/en SUBDIR kuser)
else()
    kdoctools_create_handbook(index.docbook INSTALL_DESTINATION ${HTML_INSTALL_DIR}/en SUBDIR kuser)
endif()
