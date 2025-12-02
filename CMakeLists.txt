cmake_minimum_required(VERSION 3.28)

# KDE Application Version, managed by release script
set(RELEASE_SERVICE_VERSION_MAJOR "26")
set(RELEASE_SERVICE_VERSION_MINOR "03")
set(RELEASE_SERVICE_VERSION_MICRO "70")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(DragonPlayer VERSION ${RELEASE_SERVICE_VERSION})

set(PROJECT_VERSION ${RELEASE_SERVICE_VERSION})

set(QT_MIN_VERSION "6.8.0")
set(KF_MIN_VERSION "6.12.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)
include(KDEGitCommitHooks)

include(ECMInstallIcons)
include(ECMSetupVersion)
include(ECMDeprecationSettings)
include(FeatureSummary)
include(ECMQmlModule)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Quick
    Multimedia
)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED
    CoreAddons
    Crash
    I18n
    KIO
    WindowSystem
    # Used by qml (needed for linting)
    Kirigami
    Config
)

set(WITH_KGUIADDONS_BACKPORT OFF)
if(KF_MIN_VERSION VERSION_GREATER_EQUAL "6.22.0")
    message(FATAL_ERROR "Drop kguiaddons-backport!")
else()
    find_package(KF6GuiAddons CONFIG)
    if(KF6GuiAddons_FOUND AND KF6GuiAddons_VERSION VERSION_LESS "6.22.0")
        set(WITH_KGUIADDONS_BACKPORT ON)
        if(Qt6Gui_VERSION VERSION_GREATER_EQUAL "6.10.0")
            find_package(Qt6GuiPrivate ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE)
        else()
            find_package(Qt6Gui ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE COMPONENTS Private)
        endif()
    endif()
endif()

find_package(PkgConfig)
pkg_check_modules(libavcodec REQUIRED IMPORTED_TARGET libavcodec)

configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

ecm_set_disabled_deprecation_versions(
    QT 6.8
    KF 6.12
)

add_definitions(-DTRANSLATION_DOMAIN="dragonplayer")

add_subdirectory(src)
add_subdirectory(misc)

ki18n_install(po)


file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
