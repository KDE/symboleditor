cmake_minimum_required (VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project (SymbolEditor VERSION "3.0.0")

find_package (ECM REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include (KDEInstallDirs)
include (KDECMakeSettings)
include (KDECompilerSettings NO_POLICY_SCOPE)
include (ECMSetupVersion)
include (ECMInstallIcons)
include (FeatureSummary)

kde_enable_exceptions ()

find_package (Qt6 CONFIG REQUIRED
    Core
    Widgets
)

find_package (KF6DocTools)

find_package (KF6 REQUIRED COMPONENTS
    Config
    I18n
    KIO
    WidgetsAddons
    XmlGui
)

find_package (Doxygen)
find_package (SharedMimeInfo)

if (DOXYGEN_FOUND)
    configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target (doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                       WORKING_DIRECTORY ${CMAKE_CURENT_BINARY_DIR}
                       COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif (DOXYGEN_FOUND)

include_directories (BEFORE ${CMAKE_CURENT_BINARY_DIR})

ecm_setup_version(${SymbolEditor_VERSION}
    VARIABLE_PREFIX SYMBOLEDITOR
    VERSION_HEADER Version.h
)

set (SymbolEditor_SRCS
    src/Commands.cpp
    src/ConfigurationDialogs.cpp
    src/Editor.cpp
    src/Exceptions.cpp
    src/Main.cpp
    src/MainWindow.cpp
    src/Symbol.cpp
    src/SymbolLibrary.cpp
    src/SymbolListWidget.cpp

    SymbolEditor.qrc
)

file (GLOB SymbolEditor_UI ${CMAKE_CURRENT_SOURCE_DIR}/ui/*.ui)
ki18n_wrap_ui (SymbolEditor_SRCS ${SymbolEditor_UI})

add_subdirectory (icons)

ki18n_install(po)
if (KF6DocTools_FOUND)
    add_subdirectory (doc)
    kdoctools_install(po)
endif (KF6DocTools_FOUND)

kconfig_add_kcfg_files (SymbolEditor_SRCS SymbolEditor.kcfgc)

ecm_install_icons (ICONS sc-apps-symboleditor.svgz DESTINATION ${KDE_INSTALL_ICONDIR})

add_executable (SymbolEditor ${SymbolEditor_SRCS})

target_link_libraries (SymbolEditor
    Qt6::Core
    Qt6::Widgets
    KF6::ConfigGui
    KF6::I18n
    KF6::KIOFileWidgets
    KF6::WidgetsAddons
    KF6::XmlGui
)

add_definitions (
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_TO_ASCII
)

if (SILENCE_DEPRECATED)
    add_definitions( -Wno-deprecated-declarations )
endif (SILENCE_DEPRECATED)

install (TARGETS SymbolEditor DESTINATION ${KDE_INSTALL_BINDIR})
install (FILES SymbolEditor.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR})
install (FILES org.kde.SymbolEditor.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install (FILES org.kde.SymbolEditor.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

IF (SHARED_MIME_INFO_FOUND AND IS_DIRECTORY ${KDE_INSTALL_MIMEDIR})
    install (FILES SymbolEditor.xml DESTINATION ${KDE_INSTALL_MIMEDIR})
    update_xdg_mimetypes (${KDE_INSTALL_MIMEDIR})
ENDIF (SHARED_MIME_INFO_FOUND AND IS_DIRECTORY ${KDE_INSTALL_MIMEDIR})

feature_summary (WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
