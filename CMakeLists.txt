cmake_minimum_required(VERSION 2.6)

project (SymbolEditor)

find_package (KDE4 REQUIRED)
find_package (Doxygen)
find_package (SharedMimeInfo)

if (DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target (doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif (DOXYGEN_FOUND)

include_directories (${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

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

add_subdirectory(icons)
add_subdirectory(doc)

kde4_add_kcfg_files(SymbolEditor_SRCS SymbolEditor.kcfgc)

kde4_add_ui_files(SymbolEditor_SRCS
    ui/EditorConfigPage.ui
)

kde4_install_icons(${ICON_INSTALL_DIR})
kde4_update_iconcache()

kde4_add_executable (SymbolEditor ${SymbolEditor_SRCS})

target_link_libraries (SymbolEditor ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS})

set (CMAKE_CXX_FLAGS "-DQT_NO_COMPAT -DKDE_NO_COMPAT")

install (TARGETS SymbolEditor DESTINATION ${BIN_INSTALL_DIR})
install (FILES SymbolEditorui.rc DESTINATION ${DATA_INSTALL_DIR}/SymbolEditor)
install (FILES SymbolEditor.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install (FILES SymbolEditor.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

IF (SHARED_MIME_INFO_FOUND AND IS_DIRECTORY ${XDG_MIME_INSTALL_DIR})
    install (FILES SymbolEditor.xml DESTINATION ${XDG_MIME_INSTALL_DIR})
    update_xdg_mimetypes (${XDG_MIME_INSTALL_DIR})
ENDIF(SHARED_MIME_INFO_FOUND AND IS_DIRECTORY ${XDG_MIME_INSTALL_DIR})
