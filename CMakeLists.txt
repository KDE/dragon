cmake_minimum_required(VERSION 3.16)

# KDE Application Version, managed by release script
set(RELEASE_SERVICE_VERSION_MAJOR "24")
set(RELEASE_SERVICE_VERSION_MINOR "01")
set(RELEASE_SERVICE_VERSION_MICRO "85")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(DragonPlayer VERSION ${RELEASE_SERVICE_VERSION})

set(PROJECT_VERSION ${RELEASE_SERVICE_VERSION})

set(QT_MIN_VERSION "6.5.0")
set(KF5_MIN_VERSION "5.240.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
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

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Widgets
)

find_package(KF6 ${KF5_MIN_VERSION} REQUIRED
    Config
    ConfigWidgets
    CoreAddons
    Crash
    DBusAddons
    DocTools
    I18n
    JobWidgets
    KIO
    Parts
    Solid
    WidgetsAddons
    WindowSystem
    XmlGui
)
find_package(Phonon4Qt6 4.6.60 REQUIRED NO_MODULE)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}/src/app
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    ${CMAKE_CURRENT_BINARY_DIR}
    )

include_directories(BEFORE SYSTEM ${PHONON_INCLUDES})

include(CheckIncludeFiles)
check_include_files(unistd.h HAVE_UNISTD_H)

configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

ecm_set_disabled_deprecation_versions(
    QT 6.5
    KF 5.100
)

add_definitions(-DTRANSLATION_DOMAIN="dragonplayer")

add_subdirectory(src/app)
add_subdirectory(misc)
add_subdirectory(doc)

ki18n_install(po)
kdoctools_install(po)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
