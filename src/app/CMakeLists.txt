set(videoplayer_common_SRCS
    theStream.cpp
    actions.cpp
    videoWindow.cpp )

set(videoplayer_app_SRCS
    ${videoplayer_common_SRCS}
    analyzer/analyzerBase.cpp
    analyzer/blockAnalyzer.cpp
    analyzer/fht.cpp
    audioView2.cpp
    stateChange.cpp
    textItem.cpp
    playDialog.cpp
    discSelectionDialog.cpp
    adjustSizeButton.cpp
    fullScreenToolBarHandler.cpp
    playlistFile.cpp
    main.cpp
    playerApplication.cpp
    timeLabel.cpp
    mainWindow.cpp
    recentlyPlayedList.cpp
    loadView.cpp
    ../mpris2/mpris2.cpp
    ../mpris2/mediaplayer2.cpp
    ../mpris2/mediaplayer2player.cpp )

kde4_add_ui_files(videoplayer_app_SRCS videoSettingsWidget.ui loadView.ui audioView2.ui)

kde4_add_executable(dragon ${videoplayer_app_SRCS} )

target_link_libraries(dragon
    ${KDE4_KIO_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_PHONON_LIBS}
    ${KDE4_SOLID_LIBS}
    ${KDE4_PLASMA_LIBS} )

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

install(TARGETS dragonpart DESTINATION ${PLUGIN_INSTALL_DIR} )
