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
    KF6::Crash
    Qt::Widgets
    KF6::CoreAddons
    KF6::I18n
    KF6::KIOCore
    KF6::KIOWidgets
    KF6::WidgetsAddons
    KF6::ConfigCore
    KF6::Solid
    KF6::XmlGui
    KF6::DBusAddons
    KF6::JobWidgets
    KF6::WindowSystem
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
    KF6::CoreAddons
    KF6::I18n
    KF6::KIOCore
    KF6::KIOWidgets
    KF6::WidgetsAddons
    KF6::ConfigCore
    KF6::Parts
    KF6::Solid
    KF6::XmlGui
    KF6::JobWidgets
    KF6::WindowSystem
    ${PHONON_LIBRARY}
    Qt::Core
    Qt::Widgets
)

install(TARGETS dragonpart DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf6/parts)
