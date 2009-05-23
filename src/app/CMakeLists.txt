if(XINE_XCB_FOUND)
    add_definitions(-DHAVE_XINE)
endif(XINE_XCB_FOUND)

set(videoplayer_common_SRCS
    theStream.cpp
    actions.cpp
    videoWindow.cpp )

set(videoplayer_app_SRCS
    ${videoplayer_common_SRCS}
    stateChange.cpp
    playDialog.cpp
    discSelectionDialog.cpp
    adjustSizeButton.cpp
    fullScreenToolBarHandler.cpp
    playlistFile.cpp
    main.cpp
    timeLabel.cpp
    mainWindow.cpp
	recentlyPlayedList.cpp
    audioView.cpp
    loadView.cpp
    ../dbus/mpristypes.cpp
    ../dbus/playerDbusHandler.cpp
    ../dbus/rootDbusHandler.cpp
    ../dbus/trackListDbusHandler.cpp )

qt4_add_dbus_adaptor( videoplayer_app_SRCS ../dbus/org.kde.dragon.player.xml ../dbus/playerDbusHandler.h PlayerDbusHandler )
qt4_add_dbus_adaptor( videoplayer_app_SRCS ../dbus/root.xml ../dbus/rootDbusHandler.h RootDbusHandler RootDbusHandlerBase RootDbusHandlerBase )
qt4_add_dbus_adaptor( videoplayer_app_SRCS ../dbus/tracklist.xml ../dbus/trackListDbusHandler.h TrackListDbusHandler TrackListDbusAdaptor TrackListDbusAdaptor )

kde4_add_ui_files(videoplayer_app_SRCS videoSettingsWidget.ui loadView.ui)

kde4_add_executable(dragon ${videoplayer_app_SRCS} )

target_link_libraries(dragon
    ${KDE4_KUTILS_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_PHONON_LIBS}
    ${KDE4_SOLID_LIBS})

if(XINE_XCB_FOUND)
target_link_libraries(dragon
    ${XINE_LIBRARY})
endif(XINE_XCB_FOUND)

install(TARGETS dragon ${INSTALL_TARGETS_DEFAULT_ARGS})

set(videoplayer_part_SRCS
    ${videoplayer_common_SRCS}
    part.cpp
    partToolBar.cpp )

kde4_add_plugin(dragonpart ${videoplayer_part_SRCS})

target_link_libraries(dragonpart
    ${KDE4_KPARTS_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_PHONON_LIBS}
    ${KDE4_SOLID_LIBS})

if(XINE_XCB_FOUND)
target_link_libraries(dragonpart
    ${XINE_LIBRARY})
endif(XINE_XCB_FOUND)

install(TARGETS dragonpart DESTINATION ${PLUGIN_INSTALL_DIR} )
