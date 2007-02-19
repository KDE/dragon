set(videoplayer_SRCS  
    xineEngine.cpp
    xineConfig.cpp
    xineScope.c
    theStream.cpp
    videoWindow.cpp
    videoSettings.cpp
    captureFrame.cpp
    actions.cpp
    stateChange.cpp
    slider.cpp
    analyzer.cpp
    playDialog.cpp
    listView.cpp
    adjustSizeButton.cpp
    fullScreenAction.cpp
    insertAspectRatioMenuItems.cpp
    playlistFile.cpp
    volumeAction.cpp
    ../mxcl.library.cpp
    main.cpp
    mainWindow.cpp )

kde4_automoc(${videoplayer_SRCS})

kde4_add_executable(videoplayer ${videoplayer_SRCS})

target_link_libraries(videoplayer  ${KDE4_KDECORE_LIBS} amarok )

install(TARGETS videoplayer DESTINATION ${BIN_INSTALL_DIR})