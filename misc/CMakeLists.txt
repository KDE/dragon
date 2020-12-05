configure_file(dragonplayer_part.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/dragonplayer_part.desktop @ONLY)

install(PROGRAMS org.kde.dragonplayer.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES dragonplayer-opendvd.desktop dragonplayer-openaudiocd.desktop DESTINATION ${DATA_INSTALL_DIR}/solid/actions )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/dragonplayer_part.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
install(FILES dragonplayerrc DESTINATION ${CONFIG_INSTALL_DIR})
install(FILES dragonplayer_play_dvd.desktop DESTINATION ${SERVICES_INSTALL_DIR}/ServiceMenus)
install(FILES org.kde.dragonplayer.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR} )

ecm_install_icons(
    ICONS
        sc-apps-dragonplayer.svgz
        128-apps-dragonplayer.png
        64-apps-dragonplayer.png
        48-apps-dragonplayer.png
        32-apps-dragonplayer.png
        22-apps-dragonplayer.png
        16-apps-dragonplayer.png
    DESTINATION ${KDE_INSTALL_ICONDIR}
    THEME hicolor
)

ecm_install_icons(
    ICONS
        sc-actions-player-volume-muted.svgz
        48-actions-player-volume-muted.png
        32-actions-player-volume-muted.png
        22-actions-player-volume-muted.png
        16-actions-player-volume-muted.png
    DESTINATION ${KDE_INSTALL_ICONDIR}
    THEME oxygen
)
