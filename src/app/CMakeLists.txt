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
    KF5::Crash
    Qt::Widgets
    KF5::CoreAddons
    KF5::I18n
    KF5::KIOCore
    KF5::KIOWidgets
    KF5::WidgetsAddons
    KF5::ConfigCore
    KF5::Solid
    KF5::XmlGui
    KF5::DBusAddons
    KF5::JobWidgets
    KF5::WindowSystem
    KF5::Notifications
    ${PHONON_LIBRARY}
    )

install(TARGETS dragon ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

add_library(dragonpart MODULE)

target_sources(dragonpart PRIVATE
    ${videoplayer_common_SRCS}
    part.cpp
    partToolBar.cpp
)

kcoreaddons_desktop_to_json(dragonpart ${CMAKE_BINARY_DIR}/misc/dragonplayer_part.desktop)

target_link_libraries(dragonpart
    KF5::CoreAddons
    KF5::I18n
    KF5::KIOCore
    KF5::KIOWidgets
    KF5::WidgetsAddons
    KF5::ConfigCore
    KF5::Parts
    KF5::Solid
    KF5::XmlGui
    KF5::JobWidgets
    KF5::WindowSystem
    KF5::Notifications
    ${PHONON_LIBRARY}
    Qt::Core
    Qt::Widgets
)

install(TARGETS dragonpart DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/parts)
