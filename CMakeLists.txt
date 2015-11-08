cmake_minimum_required (VERSION 2.8.12)

if (POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif (POLICY CMP0063)

project (SymbolEditor)

find_package (ECM REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include (KDEInstallDirs)
include (KDECMakeSettings)
include (KDECompilerSettings)
include (ECMInstallIcons)
include (FeatureSummary)

kde_enable_exceptions ()

find_package (Qt5 CONFIG REQUIRED
    Core
    Widgets
)

find_package (KF5DocTools)

find_package (KF5 REQUIRED COMPONENTS
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
)

file (GLOB SymbolEditor_UI ${CMAKE_CURRENT_SOURCE_DIR}/ui/*.ui)
ki18n_wrap_ui (SymbolEditor_SRCS ${SymbolEditor_UI})

add_subdirectory (icons)
add_subdirectory (doc)

kconfig_add_kcfg_files (SymbolEditor_SRCS SymbolEditor.kcfgc)

qt5_wrap_ui (SymbolEditor_SRCS
    ui/EditorConfigPage.ui
)

ecm_install_icons (ICONS sc-apps-SymbolEditor.svgz DESTINATION ${ICON_INSTALL_DIR})

add_executable (SymbolEditor ${SymbolEditor_SRCS})

target_link_libraries (SymbolEditor
    Qt5::Core
    Qt5::Widgets
    KF5::ConfigGui
    KF5::I18n
    KF5::KIOFileWidgets
    KF5::WidgetsAddons
    KF5::XmlGui
)

add_definitions (
    -DQT_NO_COMPAT
    -DKDE_NO_COMPAT
)

add_definitions (-DTRANSLATION_DOMAIN="SymbolEditor")

if (SILENCE_DEPRECATED)
    add_definitions( -Wno-deprecated-declarations )
endif (SILENCE_DEPRECATED)

install (TARGETS SymbolEditor DESTINATION ${BIN_INSTALL_DIR})
install (FILES SymbolEditorui.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/SymbolEditor)
install (FILES SymbolEditor.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install (FILES org.kde.SymbolEditor.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

IF (SHARED_MIME_INFO_FOUND AND IS_DIRECTORY ${XDG_MIME_INSTALL_DIR})
    install (FILES SymbolEditor.xml DESTINATION ${XDG_MIME_INSTALL_DIR})
    update_xdg_mimetypes (${XDG_MIME_INSTALL_DIR})
ENDIF (SHARED_MIME_INFO_FOUND AND IS_DIRECTORY ${XDG_MIME_INSTALL_DIR})

feature_summary (WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
