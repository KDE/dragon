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

ki18n_wrap_ui(videoplayer_app_SRCS videoSettingsWidget.ui loadView.ui audioView2.ui)

qt5_add_resources(videoplayer_app_SRCS
    dragonplayer.qrc
)

add_executable(dragon ${videoplayer_app_SRCS} )

target_link_libraries(dragon
    Qt5::Core
    KF5::Crash
    Qt5::Widgets
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

install(TARGETS dragon ${INSTALL_TARGETS_DEFAULT_ARGS})

set(videoplayer_part_SRCS
    ${videoplayer_common_SRCS}
    part.cpp
    partToolBar.cpp )

add_library(dragonpart MODULE ${videoplayer_part_SRCS})
kcoreaddons_desktop_to_json(dragonpart ${CMAKE_SOURCE_DIR}/misc/dragonplayer_part.desktop)

target_link_libraries(dragonpart
    Qt5::Core
    Qt5::Widgets
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
    )

install(TARGETS dragonpart DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/parts )
