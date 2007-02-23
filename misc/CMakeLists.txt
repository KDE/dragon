install(FILES videoplayer.desktop DESTINATION ${XDG_APPS_DIR} )
install(FILES videoplayerrc DESTINATION ${CONFIG_INSTALL_DIR})
install(FILES videoplayer_play_dvd.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
install(FILES videoplayerui.rc DESTINATION ${DATA_INSTALL_DIR}/videoplayer)

kde4_install_icons( ${ICON_INSTALL_DIR} )