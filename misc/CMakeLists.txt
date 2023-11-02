configure_file(dragonplayer_part.json.in ${CMAKE_CURRENT_BINARY_DIR}/dragonplayer_part.json @ONLY)

install(PROGRAMS org.kde.dragonplayer.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES dragonplayer-opendvd.desktop dragonplayer-openaudiocd.desktop DESTINATION ${KDE_INSTALL_DATADIR}/solid/actions )
install(FILES dragonplayerrc DESTINATION ${KDE_INSTALL_CONFDIR})
install(FILES dragonplayer_play_dvd.desktop DESTINATION ${KDE_INSTALL_DATADIR}/kio/servicemenus)
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
