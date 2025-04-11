install(PROGRAMS org.kde.dragonplayer.desktop DESTINATION ${KDE_INSTALL_APPDIR})
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
