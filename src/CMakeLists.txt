# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>

include(ECMSetupVersion)
ecm_setup_version(${PROJECT_VERSION}
                  VARIABLE_PREFIX DRAGON
                  VERSION_HEADER version.h)

ecm_add_qml_module(dragonmodule URI org.kde.dragon VERSION 1.0 GENERATE_PLUGIN_SOURCE)
ecm_target_qml_sources(dragonmodule
    SOURCES
        qml/AboutPage.qml
        qml/ControlsBar.qml
        qml/IconToolButton.qml
        qml/Main.qml
        qml/OverlayPopup.qml
        qml/PlayerPage.qml
        qml/VolumeButton.qml
        qml/WelcomeView.qml
)
target_sources(dragonmodule
    PRIVATE
        mpris2/mediaplayer2.cpp
        mpris2/mediaplayer2player.cpp
        mpris2/mpris2.cpp
        backend.cpp
        fileopen.cpp
        sandbox.cpp)
target_link_libraries(dragonmodule
    PRIVATE
        Qt6::Core
        KF6::KIOCore
        KF6::I18n
        KF6::Service
        KF6::WindowSystem
        Qt6::DBus
        Qt6::Qml
        Qt6::Quick
        Qt6::Multimedia
        PkgConfig::libavcodec)
# Set mpris2 as include dir so the qml plugin source can find it
target_include_directories(dragonmodule PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/mpris2>")
ecm_finalize_qml_module(dragonmodule DESTINATION ${KDE_INSTALL_QMLDIR})

add_executable(dragon main.cpp)
target_link_libraries(dragon
    Qt6::Core
    Qt6::Gui
    KF6::CoreAddons
    KF6::I18n
    KF6::I18nQml
    KF6::Crash
    Qt6::Qml
    Qt6::Widgets
)
install(TARGETS dragon DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

if(WITH_KGUIADDONS_BACKPORT)
    target_compile_definitions(dragon PRIVATE WITH_DBUS)
    target_include_directories(dragon PRIVATE kguiaddons-backport)
    target_sources(dragon PRIVATE kguiaddons-backport/ksysteminhibitor.cpp kguiaddons-backport/systeminhibitor.cpp)
    target_link_libraries(dragon KF6::GuiAddons Qt6::GuiPrivate Qt6::DBus)
endif()
