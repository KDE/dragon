configure_file(dragonplayer_part.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/dragonplayer_part.desktop @ONLY)

install(PROGRAMS org.kde.dragonplayer.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES dragonplayer-opendvd.desktop dragonplayer-openaudiocd.desktop DESTINATION ${KDE_INSTALL_DATADIR}/solid/actions )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/dragonplayer_part.desktop DESTINATION ${KDE_INSTALL_KSERVICESDIR} )
install(FILES dragonplayerrc DESTINATION ${KDE_INSTALL_CONFDIR})
install(FILES dragonplayer_play_dvd.desktop DESTINATION ${KDE_INSTALL_KSERVICESDIR}/ServiceMenus)
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
