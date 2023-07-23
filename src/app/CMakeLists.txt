set(videoplayer_common_SRCS
    theStream.cpp
    actions.cpp
    videoWindow.cpp )

add_executable(dragon)

target_sources(dragon PRIVATE
    ${videoplayer_common_SRCS}
    analyzer/analyzerBase.cpp
    analyzer/blockAnalyzer.cpp
    analyzer/fht.cpp
    audioView2.cpp
    stateChange.cpp
    discSelectionDialog.cpp
    adjustSizeButton.cpp
    fullScreenToolBarHandler.cpp
    playlistFile.cpp
    main.cpp
    playerApplication.cpp
    timeLabel.cpp
    mainWindow.cpp
    loadView.cpp
    ../mpris2/mpris2.cpp
    ../mpris2/mediaplayer2.cpp
    ../mpris2/mediaplayer2player.cpp

    dragonplayer.qrc
)

ki18n_wrap_ui(dragon
    videoSettingsWidget.ui
    loadView.ui
    audioView2.ui
)

target_link_libraries(dragon
    Qt::Core
    KF${KF_MAJOR_VERSION}::Crash
    Qt::Widgets
    KF${KF_MAJOR_VERSION}::CoreAddons
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::KIOCore
    KF${KF_MAJOR_VERSION}::KIOWidgets
    KF${KF_MAJOR_VERSION}::WidgetsAddons
    KF${KF_MAJOR_VERSION}::ConfigCore
    KF${KF_MAJOR_VERSION}::Solid
    KF${KF_MAJOR_VERSION}::XmlGui
    KF${KF_MAJOR_VERSION}::DBusAddons
    KF${KF_MAJOR_VERSION}::JobWidgets
    KF${KF_MAJOR_VERSION}::WindowSystem
    ${PHONON_LIBRARY}
    )

install(TARGETS dragon ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

add_library(dragonpart MODULE)

target_sources(dragonpart PRIVATE
    ${videoplayer_common_SRCS}
    part.cpp
    partToolBar.cpp
)

target_link_libraries(dragonpart
    KF${KF_MAJOR_VERSION}::CoreAddons
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::KIOCore
    KF${KF_MAJOR_VERSION}::KIOWidgets
    KF${KF_MAJOR_VERSION}::WidgetsAddons
    KF${KF_MAJOR_VERSION}::ConfigCore
    KF${KF_MAJOR_VERSION}::Parts
    KF${KF_MAJOR_VERSION}::Solid
    KF${KF_MAJOR_VERSION}::XmlGui
    KF${KF_MAJOR_VERSION}::JobWidgets
    KF${KF_MAJOR_VERSION}::WindowSystem
    ${PHONON_LIBRARY}
    Qt::Core
    Qt::Widgets
)

install(TARGETS dragonpart DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf${QT_MAJOR_VERSION}/parts)
