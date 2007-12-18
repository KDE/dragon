set(videoplayer_SRCS
    videoWindow.cpp
    theStream.cpp
    actions.cpp
    stateChange.cpp
    playDialog.cpp
    listView.cpp
    adjustSizeButton.cpp
    fullScreenAction.cpp
    fullScreenToolBarHandler.cpp
    playlistFile.cpp
    ../mxcl.library.cpp
    main.cpp
    mainWindow.cpp )


kde4_add_executable(videoplayer ${videoplayer_SRCS} )

target_link_libraries(videoplayer 
    ${KDE4_KUTILS_LIBS}
    ${KDE4_PHONON_LIBS}
    ${XINE_LIBRARY} )

install(TARGETS videoplayer DESTINATION ${BIN_INSTALL_DIR})
