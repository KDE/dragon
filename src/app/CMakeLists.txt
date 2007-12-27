set(videoplayer_common_SRCS
    theStream.cpp
    actions.cpp
    ../mxcl.library.cpp
    videoWindow.cpp )

set(videoplayer_app_SRCS
    ${videoplayer_common_SRCS}
    stateChange.cpp
    playDialog.cpp
    listView.cpp
    adjustSizeButton.cpp
    fullScreenAction.cpp
    fullScreenToolBarHandler.cpp
    playlistFile.cpp
    main.cpp
    mainWindow.cpp )
kde4_add_ui_files(videoplayer_app_SRCS videoSettingsWidget.ui)

kde4_add_executable(dragon ${videoplayer_app_SRCS} )

target_link_libraries(dragon
    ${KDE4_KUTILS_LIBS}
    ${KDE4_PHONON_LIBS}
    ${XINE_LIBRARY} )

install(TARGETS dragon DESTINATION ${BIN_INSTALL_DIR})

set(videoplayer_part_SRCS
    ${videoplayer_common_SRCS}
    part.cpp
    partToolBar.cpp )

kde4_add_plugin(dragonpart ${videoplayer_part_SRCS})

target_link_libraries(dragonpart
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KPARTS_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_PHONON_LIBS}
    ${XINE_LIBRARY} )

install(TARGETS dragonpart DESTINATION ${PLUGIN_INSTALL_DIR} )
