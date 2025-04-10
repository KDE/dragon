cmake_minimum_required(VERSION 3.16)

# KDE Application Version, managed by release script
set(RELEASE_SERVICE_VERSION_MAJOR "25")
set(RELEASE_SERVICE_VERSION_MINOR "07")
set(RELEASE_SERVICE_VERSION_MICRO "70")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(DragonPlayer VERSION ${RELEASE_SERVICE_VERSION})

set(PROJECT_VERSION ${RELEASE_SERVICE_VERSION})

set(QT_MIN_VERSION "6.5.0")
set(KF_MIN_VERSION "6.3.0")

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

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Widgets
)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED
    Config
    ConfigWidgets
    CoreAddons
    Crash
    DBusAddons
    I18n
    JobWidgets
    KIO
    Parts
    Solid
    WidgetsAddons
    WindowSystem
    XmlGui
)

find_package(KF6DocTools ${KF_MIN_VERSION})
set_package_properties(KF6DocTools PROPERTIES DESCRIPTION
    "Tools to generate documentation"
    TYPE OPTIONAL
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
    QT 6.7
    KF 6.4
)

add_definitions(-DTRANSLATION_DOMAIN="dragonplayer")

add_subdirectory(src/app)
add_subdirectory(misc)

ki18n_install(po)
if(KF6DocTools_FOUND)
    kdoctools_install(po)
    add_subdirectory(doc)
endif()


file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
