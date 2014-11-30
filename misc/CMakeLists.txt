install(PROGRAMS org.kde.dragonplayer.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES dragonplayer-opendvd.desktop DESTINATION ${DATA_INSTALL_DIR}/solid/actions )
install(FILES dragonplayer_part.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
install(FILES dragonplayerrc DESTINATION ${CONFIG_INSTALL_DIR})
install(FILES dragonplayer_play_dvd.desktop DESTINATION ${SERVICES_INSTALL_DIR}/ServiceMenus)
install(FILES dragonplayerui.rc dragonlogo.png DESTINATION ${KXMLGUI_INSTALL_DIR}/dragonplayer)
install(FILES dragonplayer.appdata.xml DESTINATION ${SHARE_INSTALL_PREFIX}/appdata/ )

ecm_install_icons(${ICON_INSTALL_DIR})
