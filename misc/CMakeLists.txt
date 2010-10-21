install(PROGRAMS dragonplayer.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES dragonplayer-opendvd.desktop DESTINATION ${DATA_INSTALL_DIR}/solid/actions )
install(FILES dragonplayer_part.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
install(FILES dragonplayerrc DESTINATION ${CONFIG_INSTALL_DIR})
install(FILES dragonplayer_play_dvd.desktop DESTINATION ${SERVICES_INSTALL_DIR}/ServiceMenus)
install(FILES dragonplayerui.rc dragonlogo.png DESTINATION ${DATA_INSTALL_DIR}/dragonplayer)

kde4_install_icons(${ICON_INSTALL_DIR})
