/********************************************************************************
 * Copyright (C) 2011-2015 by Stephen Allewell                                  *
 * steve.allewell@gmail.com                                                     *
 *                                                                              *
 * This program is free software; you can redistribute it and/or modify         *
 * it under the terms of the GNU General Public License as published by         *
 * the Free Software Foundation; either version 2 of the License, or            *
 * (at your option) any later version.                                          *
 ********************************************************************************/


/**
 * @file
 * Implement the MainWindow class. This supplies the main user interface which comprises of a tabbed widget
 * containing a symbol editor and a library of symbols loaded from a symbol file.  A standard menu bar, tool
 * bar and status bar are provided to access the various functions and provide any suitable feedback or status
 * messages.
 */


/**
 * @page main_window Main Window
 * The symbol editor main window comprises a tabbed widget containing the symbol editor and a list of the
 * current symbols in the loaded file. A standard menu bar, tool bar and status bar are provided to access
 * the various tools and functions and provide any necessary status messages or user feedback.
 *
 * @image html ui-main-editor.png "The user interface showing the editor tab"
 *
 * @section file_menu File Menu
 *
 * @subsection file_new New
 * Start a new symbol definition. The editor is cleared ready to define the new symbol. If there is an existing
 * symbol being edited that has not been saved, the user is prompted to save it or allow it to be overwritten.
 * Alternatively the user can cancel the creation of the new symbol leaving the current one intact.
 *
 * @subsection file_open Open
 * Open an existing symbol library. The current symbol and symbol library are cleared and the user is prompted to
 * select a file to be opened. If the current symbol of symbol file have not been saved the user is asked to
 * save them or allow them to be overwritten. Alternatively the user can cancel opening a file leaving the current
 * ones intact.
 *
 * @subsection file_open_recent Open Recent
 * Previously opened files are added to the recent files menu and can be opened by selecting the file specified.
 * The same rules relating to Open and the current symbol and library apply here.
 *
 * @subsection file_save Save
 * Save the current library to a file. If this is a new library the user will be prompted to enter a file name.
 *
 * @subsection file_save_as Save As
 * Save the current library using a different name. The user will be prompted to enter a file name.
 *
 * @subsection file_save_symbol Save Symbol
 * Save the current symbol being edited to the current library. This does not save the symbol library to disk, this
 * is done by the Save command.
 *
 * @subsection file_save_symbol_as_new Save Symbol as New
 * Save the symbol as if it was a new one. This would apply if the symbol was one from the library being edited. The
 * editor maintains a link between it and the library and would normally update it when saved. This will reset the
 * index and add the symbol to the library as a new one. The symbol being edited is now detached from the library
 * and subsequent saves will continue to add new symbols to the library. This can be used to create new symbols based
 * on an existing library symbol.
 *
 * @subsection file_import_library Import Library
 * Import an existing symbol library and append the symbols in it to the current library.
 *
 * @subsection file_close Close
 * Close the current library. The editor and the library are cleared leaving an empty library ready for new symbols
 * to be added. If the current symbol and library need to be saved the user is prompted to do so.
 *
 * @subsection file_quit Quit
 * Quit the application. If the current symbol and library need to be saved the user is prompted to do so.
 *
 * @section edit_menu Edit Menu
 *
 * @subsection edit_undo Undo
 * Actions that modify the symbol currently being edited or the current symbol library can be undone reverting to
 * the previous state. The editor and the library have independent undo stacks and changes to one do not affect the
 * other. The undo command affects the currently selected tab. The undo command in the menu shows a description of
 * the action that will be undone. For the undo command on the toolbar, the tooltip will show the desciption.
 *
 * @subsection edit_redo Redo
 * Actions that are undone can be redone. As with undo, the editor and the symbol library are independent of each
 * other and the redo command affects the currently selected tab. The redo command in the menu and the toolbar tooltip
 * shows a description of the action that will be redone.
 *
 * @subsection file_edit_toolbar File and Edit Toolbar
 * The file and edit menu toolbar allow quick access to these common functions.
 * @image html ui-main-toolbar.png
 * - @ref file_new
 * - @ref file_open
 * - @ref file_save
 * - @ref edit_undo
 * - @ref edit_redo
 * - @ref file_save_symbol
 *
 * @section rendering_menu Rendering Menu
 * The defined path can be rendered with various settings. Filled or unfilled, for which the fill method can be defined.
 * The path end cap can be defined as flat, square and round. The line join type can be defined as bevel, miter and round.
 * The line width can be increased or decreased.
 *
 * For full details of the rendering options, see the @ref path_rendering.
 *
 * @subsection rendering_toolbar Rendering Toolbar
 * The rendering toolbar allows quick access to these common functions.
 * @image html ui-rendering-toolbar.png
 * - @ref fill_mode
 * - @ref fill_rule
 * - @ref line_cap
 * - @ref line_join
 * - @ref line_width
 *
 * @section tools_menu Tools Menu
 * A number of tools are available to aid the design of the symbols. The symbols are composed of a series of sub paths
 * and each sub path is composed of a move to the start position (this defaults to 0,0 for new symbols) followed by
 * lines and curves. The curves are cubic splines having a start, an end and two control points defining the curve.
 * There are convenience tools to create rectangles and ellipses, but these will be broken down into lines and curves.
 *
 * All points created for the elements are moveable by dragging them to their new position. All points can be snapped
 * to the grid intersections if the snap option is turned on, otherwise they can be positioned anywhere.
 *
 * The symbol can be rotated clockwise and counter clockwise and also flipped vertically and horizontally. This allows
 * multiple symbols to be easily created based on the same design. Remember to use the save symbol as new option for this.
 *
 * For full details of the tools see the @ref editor_tools.
 *
 * @subsection tools_toolbar Tools Toolbar
 * The tools toolbar allows quick access to these common functions.
 * @image html ui-tools-toolbar.png
 * - @ref move_to
 * - @ref line_to
 * - @ref cubic_to
 * - @ref rectangle
 * - @ref ellipse
 * - @ref character
 * - @ref rotate_left
 * - @ref rotate_right
 * - @ref flip_horizontal
 * - @ref flip_vertical
 * - @ref scale_preferred
 * - @ref snap_grid
 * - @ref guide_lines
 */


#include "MainWindow.h"

#include <QAction>
#include <QActionGroup>
#include <QFileDialog>
#include <QIcon>
#include <QVBoxLayout>
#include <QListWidgetItem>
#include <QMenu>
#include <QStatusBar>
#include <QTabWidget>
#include <QTemporaryFile>

#include <kwidgetsaddons_version.h>
#include <KActionCollection>
#include <KConfigDialog>
#include <KConfigGroup>
#include <KIO/FileCopyJob>
#include <KLocalizedString>
#include <KMessageBox>
#include <KRecentFilesAction>

#include "ConfigurationDialogs.h"
#include "Editor.h"
#include "Exceptions.h"
#include "SymbolListWidget.h"
#include "SymbolLibrary.h"

#include "ui_EditorConfigPage.h"

#include "SymbolEditor.h"


/**
 * Construct the MainWindow.
 * Create an instance of a symbol file.
 * Create the tab widget, editor, list widget and the symbol file. The tab widget is then set as
 * the central widget and will contain the editor and list widgets. The Editor is added to a
 * layout to allow it to be centralized in the main window.
 * Set up the actions, add the two undo stacks to the undo group and connect any signal slots required.
 * Set up the GUI from the applications rc file.
 * The editor page is selected in the tab widget which should also initialise the undo redo buttons.
 * The moveTo tool action is triggered to enable the moveTo tool as the initial one.
 * Other actions are initialised from the current Editor symbol.
 */
MainWindow::MainWindow()
    :   m_tabWidget(new QTabWidget(this)),
        m_editor(new Editor),
        m_listWidget(new SymbolListWidget(m_tabWidget)),
        m_symbolLibrary(new SymbolLibrary(m_listWidget)),
        m_item(nullptr),
        m_menu(nullptr)
{
    m_listWidget->loadFromLibrary(m_symbolLibrary);
    m_url = QUrl(i18n("Untitled"));

    setObjectName(QStringLiteral("MainWindow#"));

    KActionCollection *actions = actionCollection();

    m_listWidget->setIconSize(48);
    m_listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    m_tabWidget->addTab(m_editor, i18nc("The editor tab title", "Editor"));
    m_tabWidget->addTab(m_listWidget, i18nc("The library tab title", "Library"));

    setCentralWidget(m_tabWidget);

    setupActions();

    m_undoGroup.addStack(m_editor->undoStack());
    m_undoGroup.addStack(m_symbolLibrary->undoStack());
    connect(m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(currentChanged(int)));
    connect(m_editor, SIGNAL(message(QString)), statusBar(), SLOT(showMessage(QString)));
    connect(m_editor, SIGNAL(minLineWidth(bool)), actions->action(QStringLiteral("decreaseLineWidth")), SLOT(setDisabled(bool)));
    connect(m_editor, SIGNAL(maxLineWidth(bool)), actions->action(QStringLiteral("increaseLineWidth")), SLOT(setDisabled(bool)));
    connect(&m_undoGroup, SIGNAL(canUndoChanged(bool)), actions->action(QStringLiteral("edit_undo")), SLOT(setEnabled(bool)));
    connect(&m_undoGroup, SIGNAL(canRedoChanged(bool)), actions->action(QStringLiteral("edit_redo")), SLOT(setEnabled(bool)));
    connect(&m_undoGroup, SIGNAL(undoTextChanged(QString)), this, SLOT(undoTextChanged(QString)));
    connect(&m_undoGroup, SIGNAL(redoTextChanged(QString)), this, SLOT(redoTextChanged(QString)));
    connect(&m_undoGroup, SIGNAL(cleanChanged(bool)), this, SLOT(cleanChanged(bool)));
    connect(m_editor->undoStack(), SIGNAL(cleanChanged(bool)), actions->action(QStringLiteral("saveSymbol")), SLOT(setDisabled(bool)));
    connect(m_editor->undoStack(), SIGNAL(cleanChanged(bool)), actions->action(QStringLiteral("saveSymbolAsNew")), SLOT(setDisabled(bool)));
    connect(m_symbolLibrary->undoStack(), SIGNAL(cleanChanged(bool)), actions->action(QStringLiteral("file_save")), SLOT(setDisabled(bool)));
    connect(m_listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemSelected(QListWidgetItem*)));
    connect(m_listWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(listWidgetContextMenuRequested(QPoint)));

    setupGUI(KXmlGuiWindow::Default, QStringLiteral("SymbolEditorui.rc"));

    actions->action(QStringLiteral("moveTo"))->trigger();                   // select draw tool
    actions->action(QStringLiteral("enableSnap"))->setChecked(true);        // enable snap
    actions->action(QStringLiteral("enableGuides"))->setChecked(true);      // enable creation of guides
    actions->action(QStringLiteral("file_save"))->setEnabled(false);        // nothing to save yet
    actions->action(QStringLiteral("saveSymbol"))->setEnabled(false);       // nothing to save yet
    actions->action(QStringLiteral("saveSymbolAsNew"))->setEnabled(false);  // nothing to save yet
    setActionsFromSymbol(m_editor->symbol().second);        // set the actions that depend on the current empty symbol, i.e. the defaults

    currentChanged(m_tabWidget->currentIndex());              // this should be the editor
}


/**
 * Descructor for the MainWindow
 * Delete the SymbolLibrary object. The other widgets that are created in the constructor are children of the
 * MainWindow and will be destroyed when this is.
 */
MainWindow::~MainWindow()
{
    delete m_symbolLibrary;
}


/**
 * Test if it is ok to close this window.
 * Check if the current symbol being edited has been changed and if the library has been changed.
 *
 * @return true if it is ok to close, false otherwise
 */
bool MainWindow::queryClose()
{
    return (editorClean() && libraryClean());
}


/**
 * Check if it ok to close the currently edited symbol.
 *
 * @return true if is ok to close the symbol, false otherwise
 */
bool MainWindow::editorClean()
{
    bool clean = m_editor->undoStack()->isClean();

    if (!clean) {
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
        int messageBoxResult = KMessageBox::warningTwoActionsCancel(this,
#else
        int messageBoxResult = KMessageBox::warningYesNoCancel(this,
#endif
                                                               i18n("Save changes to the symbol?"), QString(),
                                                               KStandardGuiItem::save(),
                                                               KStandardGuiItem::discard(),
                                                               KStandardGuiItem::cancel());

        switch (messageBoxResult) {
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
        case KMessageBox::PrimaryAction:
#else
        case KMessageBox::Yes:
#endif
            saveSymbol();
            save();
            clean = true;
            break;

#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
        case KMessageBox::SecondaryAction:
#else
        case KMessageBox::No:
#endif
            clean = true;
            break;

        case KMessageBox::Cancel:
            clean = false;
            break;
        }
    }

    return clean;
}


/**
 * Check if it is ok to close the library.
 *
 * @return true if it is ok to close the library, false otherwise
 */
bool MainWindow::libraryClean()
{
    bool clean = m_symbolLibrary->undoStack()->isClean();

    if (!clean) {
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
        int messageBoxResult = KMessageBox::warningTwoActionsCancel(this,
#else
        int messageBoxResult = KMessageBox::warningYesNoCancel(this,
#endif
                                                               i18n("Save changes to the library?"), QString(),
                                                               KStandardGuiItem::save(),
                                                               KStandardGuiItem::discard(),
                                                               KStandardGuiItem::cancel());

        switch (messageBoxResult) {
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
        case KMessageBox::PrimaryAction:
#else
        case KMessageBox::Yes:
#endif
            save();
            clean = true;
            break;

#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
        case KMessageBox::SecondaryAction:
#else
        case KMessageBox::No:
#endif
            clean = true;
            break;

        case KMessageBox::Cancel:
            clean = false;
            break;
        }
    }

    return clean;
}


/**
 * Open a file.
 * Use the QFileDialog::getOpenFileUrl to get a QUrl to open which is then passed to filOpen(const QUrl &).
 */
void MainWindow::fileOpen()
{
    QUrl url = QFileDialog::getOpenFileUrl(this, i18n("Open file"), QUrl::fromLocalFile(QDir::homePath()), i18n("Cross Stitch Symbols (*.sym)"));

    if (!url.isEmpty()) {
        fileOpen(url);
    }
}


/**
 * If a valid url is supplied, try and download the file (in case it comes from a remote source) and
 * then try and open it read only. Once opened create a QDataStream and try and read the contents.
 * This is protected in a try-catch block to intercept any exceptions that may be thrown by the loading
 * routines. If there were any errors, the symbol library will be cleared and a suitable error message
 * will be displayed.
 * The url of the file is set in the symbol file object only if there were no errors. This will avoid
 * writing to a corrupt file or to a file that isn't a symbol file. The url is added to the recent file
 * list.
 */
void MainWindow::fileOpen(const QUrl &url)
{
    if (!editorClean() || !libraryClean()) {
        return;
    }

    m_symbolLibrary->clear();
    m_editor->clear();

    if (url.isValid()) {
        QTemporaryFile tmpFile;

        if (tmpFile.open()) {
            tmpFile.close();

            KIO::FileCopyJob *job = KIO::file_copy(url, QUrl::fromLocalFile(tmpFile.fileName()), -1, KIO::Overwrite);

            if (job->exec()) {
                QFile reader(tmpFile.fileName());

                if (reader.open(QIODevice::ReadOnly)) {
                    QDataStream stream(&reader);

                    try {
                        stream >> *m_symbolLibrary;
                        m_url = url;
                        KRecentFilesAction *action = static_cast<KRecentFilesAction *>(actionCollection()->action(QStringLiteral("file_open_recent")));
                        action->addUrl(url);
                        action->saveEntries(KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("RecentFiles")));
                        m_tabWidget->setCurrentIndex(1);
                    } catch (const InvalidFile &e) {
                        KMessageBox::error(nullptr, i18n("This doesn't appear to be a valid symbol file"));
                    } catch (const InvalidFileVersion &e) {
                        KMessageBox::error(nullptr, i18n("Version %1 of the library file is not supported in this version of SymbolEditor", e.version));
                    } catch (const InvalidSymbolVersion &e) {
                        KMessageBox::error(nullptr, i18n("Version %1 of the symbol is not supported in this version of SymbolEditor", e.version));
                    } catch (const FailedReadLibrary &e) {
                        KMessageBox::error(nullptr, i18n("Failed to read the library\n%1", e.statusMessage()));
                        m_symbolLibrary->clear();
                    }
                } else {
                    KMessageBox::error(nullptr, reader.errorString());
                }
            } else {
                KMessageBox::error(nullptr, job->errorString());
            }
        } else {
            KMessageBox::error(nullptr, tmpFile.errorString());
        }
    } else {
        KMessageBox::error(nullptr, i18n("The url %1 is invalid", url.fileName()));
    }
}


/**
 * Save the library using its url, if this is Untitled than call saveAs to get a valid url.
 * Open the file and write to the stream. This is protected in a try-catch block to intercept
 * any exceptions thrown by the writing routines. If there were any exceptions thrown or the
 * file could not be opened a suitable error message is shown.
 */
void MainWindow::save()
{
    if (m_url == QUrl(i18n("Untitled"))) {
        saveAs();
    } else {
        QFile file(m_url.path());

        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            QDataStream stream(&file);
            stream.setVersion(QDataStream::Qt_4_0);

            try {
                stream << *m_symbolLibrary;
            } catch (const FailedWriteLibrary &e) {
                KMessageBox::error(nullptr, i18n("Failed to write the library\n%1", e.statusMessage()));
            }

            file.close();
            m_symbolLibrary->undoStack()->setClean();
        } else {
            KMessageBox::error(nullptr, i18n("Failed to open the file %1\n%2", m_url.fileName(), file.errorString()));
        }
    }
}


/**
 * Save the library using a different url.
 * This is also called from save when the assigned url is Untitled.
 * The new url is added to the recent files list.
 */
void MainWindow::saveAs()
{
    QUrl url = QFileDialog::getSaveFileUrl(this, i18n("Save As..."), QUrl::fromLocalFile(QDir::homePath()), i18n("Cross Stitch Symbols (*.sym)"));

    if (url.isValid()) {
        m_url = url;
        save();
        KRecentFilesAction *action = static_cast<KRecentFilesAction *>(actionCollection()->action(QStringLiteral("file_open_recent")));
        action->addUrl(url);
        action->saveEntries(KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("RecentFiles")));
    }
}


/**
 * Initialise a new symbol.
 * Check if the current symbol has been saved or can be overwritten and then call clear
 * on the editor which initializes the editor with an empty symbol.
 * The actions are reset to those relevant to the new empty symbol.
 * The moveTo action is triggered as this is the most likely to be used next.
 */
void MainWindow::newSymbol()
{
    if (editorClean()) {
        m_editor->clear();
        setActionsFromSymbol(m_editor->symbol().second);
        actionCollection()->action(QStringLiteral("moveTo"))->trigger();   // Select move tool
    }
}


/**
 * Save the current symbol.
 * Store the symbol currently in the editor into the symbol library object. If it is a
 * new symbol the index will be 0, a new index will then be created. Otherwise the index
 * will be the one from the library and storing it will overwrite the existing symbol.
 */
void MainWindow::saveSymbol()
{
    QPair<qint16, Symbol> pair = m_editor->symbol();
    m_symbolLibrary->undoStack()->push(new UpdateSymbolCommand(m_symbolLibrary, pair.first, pair.second));
    m_editor->undoStack()->setClean();
}


/**
 * Save the current symbol using a new index.
 * Store the symbol currently in the editor into the symbol library object.  Ignore the
 * editors index, resetting it to 0 and a new index will then be created.
 * Reassign this symbol back to the editor with a 0 index effectively creating a new symbol in
 * the editor.
 */
void MainWindow::saveSymbolAsNew()
{
    QPair<qint16, Symbol> pair = m_editor->symbol();
    pair.first = 0;
    m_symbolLibrary->undoStack()->push(new UpdateSymbolCommand(m_symbolLibrary, pair.first, pair.second));
    m_editor->setSymbol(pair);
}


/**
 * Import a library of symbols into the current library.
 * Get a url for the library file and try and download it in case it comes from a remote source.
 * Open the file and read the contents into a new SymbolLibrary object. The read is protected by
 * a try-catch block to intercept any exceptions thrown by the read routines. If there were no
 * errors an ImportLibraryCommand is created and pushed onto the symbol library undo stack.
 * This will copy all the symbols from the imported library into the current library.
 * Any errors will display a suitable error message.
 */
void MainWindow::importLibrary()
{
    QUrl url = QFileDialog::getOpenFileUrl(this, i18n("Import library"), QUrl::fromLocalFile(QDir::homePath()), i18n("Cross Stitch Symbols (*.sym)"));

    if (url.isEmpty()) {
        return;
    }

    if (url.isValid()) {
        QTemporaryFile tmpFile;

        if (tmpFile.open()) {
            KIO::FileCopyJob *job = KIO::file_copy(url, QUrl::fromLocalFile(tmpFile.fileName()), -1, KIO::Overwrite);

            if (job->exec()) {
                SymbolLibrary *lib = new SymbolLibrary;
                QDataStream stream(&tmpFile);

                try {
                    stream >> *lib;
                    m_symbolLibrary->undoStack()->push(new ImportLibraryCommand(m_symbolLibrary, lib));
                } catch (const InvalidFile &e) {
                    KMessageBox::error(nullptr, i18n("This doesn't appear to be a valid symbol file"));
                } catch (const InvalidFileVersion &e) {
                    KMessageBox::error(nullptr, i18n("Version %1 of the library file is not supported in this version of SymbolEditor", e.version));
                }
            } else {
                KMessageBox::error(nullptr, job->errorString());
            }
        } else {
            KMessageBox::error(nullptr, tmpFile.errorString());
        }
    } else {
        KMessageBox::error(nullptr, i18n("The url %1 is invalid", url.fileName()));
    }
}


/**
 * Close the current library.
 * Check if the current symbol and the symbol library need to be saved and then clear
 * the library and the editor.
 */
void MainWindow::close()
{
    if (editorClean() && libraryClean()) {
        m_editor->clear();
        m_symbolLibrary->clear();
        m_url = QUrl(i18n("Untitled"));
    }
}


/**
 * Quit the application.
 * Closes this MainWindow.
 */
void MainWindow::quit()
{
    KXmlGuiWindow::close();
}


/**
 * Undo the last operation.
 * Several undo stacks will be available. One for the library object and one for the editor.
 * Changing the tab will update the actions depending on the contents of the relevant undo stack.
 */
void MainWindow::undo()
{
    m_undoGroup.undo();
}


/**
 * Redo the last operation undone.
 * Several undo stacks will be available. One for the library object and one for the editor.
 * Changing the tab will update the actions depending on the contents of the relevant undo stack.
 */
void MainWindow::redo()
{
    m_undoGroup.redo();
}


/**
 * Update the undo action text to reflect the last operation available to undo.
 * Several undo stacks will be available. One for the library object and one for the editor.
 * Changing the tab will update the actions depending on the contents of the relevant undo stack.
 *
 * @param text the text string to describe the operation
 */
void MainWindow::undoTextChanged(const QString &text)
{
    actionCollection()->action(QStringLiteral("edit_undo"))->setText(QString(i18n("Undo %1", text)));
}


/**
 * Update the redo action text to reflect the last operation available to redo.
 * Several undo stacks will be available. One for the library object and one for the editor.
 * Changing the tab will update the actions depending on the contents of the relevant undo stack.
 *
 * @param text the text string to describe the operation
 */
void MainWindow::redoTextChanged(const QString &text)
{
    actionCollection()->action(QStringLiteral("edit_redo"))->setText(QString(i18n("Redo %1", text)));
}


/**
 * Update the caption based on the state of the undo stack.
 *
 * @param clean true if the symbol has not been changed, false otherwise
 */
void MainWindow::cleanChanged(bool clean)
{
    QString tab;

    if (m_tabWidget->currentIndex() == 1) {
        tab = QString(i18nc("The library tab title %1 is the file name", "%1 Library", m_url.fileName()));
    } else {
        tab = QString(i18nc("The editor tab title %1 is the file name", "%1 Editor", m_url.fileName()));
    }

    setCaption(tab, !clean);
}


/**
 * Change the tab selected.
 * This is connected to the QTabWidget::currentChanged() slot and indicates that the current tab
 * has changed. This allows the undo stack connections to be modified.
 *
 * @param index the index of the page
 */
void MainWindow::currentChanged(int index)
{
    if (index == 0) { // Editor
        m_undoGroup.setActiveStack(m_editor->undoStack());
        m_editor->updateStatusMessage();
    } else if (index == 1) { // QListWidget
        m_undoGroup.setActiveStack(m_symbolLibrary->undoStack());
        statusBar()->clearMessage();
    }
}


/**
 * Edit an existing symbol from the symbol library.
 * Check if the current symbol being edited has been changed. If yes, ask if it should be
 * saved or discarded.
 * Clear the contents of the editor and assign a copy of the item symbol to it to edit.
 * The actions are updated to reflect the settings of the symbol being edited.
 *
 * @param item a pointer to a QListWidgetItem that was double clicked.
 */
void MainWindow::itemSelected(QListWidgetItem *item)
{
    QPair<qint16, Symbol> pair;

    if (editorClean()) {
        m_editor->clear();
        pair.first = static_cast<qint16>(item->data(Qt::UserRole).toInt());
        pair.second = m_symbolLibrary->symbol(pair.first);
        m_editor->setSymbol(pair);
        setActionsFromSymbol(pair.second);
        m_tabWidget->setCurrentIndex(0);
    }
}


/**
 * Display a context menu for the list widget.
 * Options:
 *  Delete Symbol
 *
 * @param pos a const reference to a QPoint representing the cursor position
 */
void MainWindow::listWidgetContextMenuRequested(const QPoint &pos)
{
    if ((m_item = m_listWidget->itemAt(pos))) {
        if (!m_menu) {
            m_menu = new QMenu;
            m_menu->addAction(i18n("Delete Symbol"), this, SLOT(deleteSymbol()));
        }

        m_menu->popup(QCursor::pos());
    }
}


/**
 * Delete the symbol pointed to by m_item.
 */
void MainWindow::deleteSymbol()
{
    m_symbolLibrary->undoStack()->push(new DeleteSymbolCommand(m_symbolLibrary, static_cast<qint16>(m_item->data(Qt::UserRole).toInt())));
}


/**
 * Configure the application.
 * Display the configuration dialog, creating it if necessary.
 */
void MainWindow::preferences()
{
    if (KConfigDialog::showDialog(QStringLiteral("preferences"))) {
        return;
    }

    KConfigDialog *dialog = new KConfigDialog(this, QStringLiteral("preferences"), Configuration::self());
    dialog->setFaceType(KPageDialog::List);

    dialog->addPage(new EditorConfigPage(nullptr, QStringLiteral("EditorConfigPage")), i18nc("The Editor configuration page", "Editor"), QStringLiteral("preferences-desktop"));
//    dialog->setHelp("ConfigurationDialog");

    connect(dialog, SIGNAL(settingsChanged(QString)), m_editor, SLOT(readSettings()));

    dialog->show();
}


/**
 * Set up the applications actions.
 * Create standard actions.
 * Create other actions, setting the icon and data as required.
 * Several actions are added to groups which are set as exclusive.
 * All actions are added to the applications QActionCollection.
 */
void MainWindow::setupActions()
{
    QAction *action;
    QActionGroup *actionGroup;

    KActionCollection *actions = actionCollection();

    // File menu
    KStandardAction::open(this, SLOT(fileOpen()), actions);
    KStandardAction::openNew(this, SLOT(newSymbol()), actions);
    KStandardAction::openRecent(this, SLOT(fileOpen(QUrl)), actions)->loadEntries(KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("RecentFiles")));
    KStandardAction::save(this, SLOT(save()), actions);
    KStandardAction::saveAs(this, SLOT(saveAs()), actions);

    action = new QAction(this);
    action->setText(i18n("Import Library"));
    action->setWhatsThis(i18n("Imports another library appending the symbols it contains to the current library."));
    connect(action, SIGNAL(triggered()), this, SLOT(importLibrary()));
    actions->addAction(QStringLiteral("importLibrary"), action);

    action = new QAction(this);
    action->setText(i18n("Save Symbol"));
    action->setWhatsThis(i18n("Save the symbol to the library. If this is a new symbol, subsequent saves will create additional symbols in the library. If the symbol was selected from the library to edit then saving will update that symbol in the library."));
    action->setIcon(QIcon::fromTheme(QStringLiteral("symboleditor-save-symbol")));
    connect(action, SIGNAL(triggered()), this, SLOT(saveSymbol()));
    actions->addAction(QStringLiteral("saveSymbol"), action);

    action = new QAction(this);
    action->setText(i18n("Save Symbol as New"));
    action->setWhatsThis(i18n("Save the current symbol as a new one in the library. Subsequent saves will update the new symbol."));
    connect(action, SIGNAL(triggered()), this, SLOT(saveSymbolAsNew()));
    actions->addAction(QStringLiteral("saveSymbolAsNew"), action);

    KStandardAction::close(this, SLOT(close()), actions);
    KStandardAction::quit(this, SLOT(quit()), actions);

    // Edit menu
    KStandardAction::undo(this, SLOT(undo()), actions);
    KStandardAction::redo(this, SLOT(redo()), actions);

    // Rendering menu
    action = new QAction(this);
    action->setText(i18n("Fill Path"));
    action->setWhatsThis(i18n("Enable path filling. The path defines the closed boundary of the shape and the path is filled with the selected fill method."));
    action->setIcon(QIcon::fromTheme(QStringLiteral("format-fill-color")));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered(bool)), m_editor, SLOT(selectFilled(bool)));
    actions->addAction(QStringLiteral("fillPath"), action);

    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = new QAction(this);
    action->setText(i18n("Odd Even Fill"));
    action->setWhatsThis(i18n("The Odd Even fill method will fill alternate areas of the symbol."));
    action->setData(Qt::OddEvenFill);
    action->setIcon(QIcon::fromTheme(QStringLiteral("symboleditor-odd-even-fill")));
    action->setCheckable(true);
    actions->addAction(QStringLiteral("oddEvenFill"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Winding Fill"));
    action->setWhatsThis(i18n("The Winding fill method will fill the complete interior of the path."));
    action->setData(Qt::WindingFill);
    action->setIcon(QIcon::fromTheme(QStringLiteral("symboleditor-winding-fill")));
    action->setCheckable(true);
    actions->addAction(QStringLiteral("windingFill"), action);
    actionGroup->addAction(action);

    connect(actionGroup, SIGNAL(triggered(QAction*)), m_editor, SLOT(selectFillRule(QAction*)));

    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = new QAction(this);
    action->setText(i18n("Flat Cap"));
    action->setWhatsThis(i18n("The Flat Cap end type provides a square end that stops at the end point of the line.\n\nThis is only applicable to non-filled paths."));
    action->setData(Qt::FlatCap);
    action->setIcon(QIcon::fromTheme(QStringLiteral("stroke-cap-butt")));
    action->setCheckable(true);
    actions->addAction(QStringLiteral("flatCap"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Square Cap"));
    action->setWhatsThis(i18n("The Square Cap end type provides a square end that projects beyond the end point of the line by half the line width.\n\nThis is only applicable to non-filled paths."));
    action->setData(Qt::SquareCap);
    action->setIcon(QIcon::fromTheme(QStringLiteral("stroke-cap-square")));
    action->setCheckable(true);
    actions->addAction(QStringLiteral("squareCap"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Round Cap"));
    action->setWhatsThis(i18n("The Round Cap end type provides a round end that projects beyond the end point of the line with a radius of half the line width.\n\nThis is only applicable to non-filled paths."));
    action->setData(Qt::RoundCap);
    action->setIcon(QIcon::fromTheme(QStringLiteral("stroke-cap-round")));
    action->setCheckable(true);
    actions->addAction(QStringLiteral("roundCap"), action);
    actionGroup->addAction(action);

    connect(actionGroup, SIGNAL(triggered(QAction*)), m_editor, SLOT(selectCapStyle(QAction*)));

    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = new QAction(this);
    action->setText(i18n("Bevel Join"));
    action->setWhatsThis(i18n("The Bevel Join provides a beveled corner between two lines.\n\nThis is only applicable to non-filled paths."));
    action->setData(Qt::BevelJoin);
    action->setIcon(QIcon::fromTheme(QStringLiteral("stroke-join-bevel")));
    action->setCheckable(true);
    actions->addAction(QStringLiteral("bevelJoin"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Miter Join"));
    action->setWhatsThis(i18n("The Miter Join provides a mitered corner between two lines.\n\nThis is only applicable to non-filled paths."));
    action->setData(Qt::MiterJoin);
    action->setIcon(QIcon::fromTheme(QStringLiteral("stroke-join-miter")));
    action->setCheckable(true);
    actions->addAction(QStringLiteral("miterJoin"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Round Join"));
    action->setWhatsThis(i18n("The Round Join provides a rounded corner between two lines using a radius of half the line width.\n\nThis is only applicable to non-filled paths."));
    action->setData(Qt::RoundJoin);
    action->setIcon(QIcon::fromTheme(QStringLiteral("stroke-join-round")));
    action->setCheckable(true);
    actions->addAction(QStringLiteral("roundJoin"), action);
    actionGroup->addAction(action);

    connect(actionGroup, SIGNAL(triggered(QAction*)), m_editor, SLOT(selectJoinStyle(QAction*)));

    action = new QAction(this);
    action->setText(i18n("Increase Line Width"));
    action->setWhatsThis(i18n("Increases the line width.\n\nThis is only applicable to non-filled paths."));
    action->setIcon(QIcon::fromTheme(QStringLiteral("symboleditor-increase-line-width")));
    connect(action, SIGNAL(triggered()), m_editor, SLOT(increaseLineWidth()));
    actions->addAction(QStringLiteral("increaseLineWidth"), action);

    action = new QAction(this);
    action->setText(i18n("Decrease Line Width"));
    action->setWhatsThis(i18n("Decreases the line width.\n\nThis is only applicable to non-filled paths."));
    action->setIcon(QIcon::fromTheme(QStringLiteral("symboleditor-decrease-line-width")));
    connect(action, SIGNAL(triggered()), m_editor, SLOT(decreaseLineWidth()));
    actions->addAction(QStringLiteral("decreaseLineWidth"), action);

    // Tools Menu
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = new QAction(this);
    action->setText(i18n("Move To"));
    action->setWhatsThis(i18n("Move the current point to a new position. This implicitly closes any existing sub path, starting a new one."));
    action->setData(Editor::MoveTo);
    action->setIcon(QIcon::fromTheme(QStringLiteral("go-jump")));
    action->setCheckable(true);
    actions->addAction(QStringLiteral("moveTo"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Draw To"));
    action->setWhatsThis(i18n("Add a straight line from the current position to a defined end point. The end point becomes the new current position."));
    action->setData(Editor::LineTo);
    action->setIcon(QIcon::fromTheme(QStringLiteral("draw-line")));
    action->setCheckable(true);
    actions->addAction(QStringLiteral("lineTo"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Cubic To"));
    action->setWhatsThis(i18n("Add a cubic bezier curve from the current position using two control points and an end point. The end point becomes the new current position."));
    action->setData(Editor::CubicTo);
    action->setIcon(QIcon::fromTheme(QStringLiteral("draw-bezier-curves")));
    action->setCheckable(true);
    actions->addAction(QStringLiteral("cubicTo"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Rectangle"));
    action->setWhatsThis(i18n("Add a rectangle as a separate sub path defined by two points representing the opposite corners."));
    action->setData(Editor::Rectangle);
    action->setIcon(QIcon::fromTheme(QStringLiteral("draw-rectangle")));
    action->setCheckable(true);
    actions->addAction(QStringLiteral("rectangle"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Ellipse"));
    action->setWhatsThis(i18n("Add an ellipse as a separate sub path defined by a bounding rectangle represented by two opposite corners."));
    action->setData(Editor::Ellipse);
    action->setIcon(QIcon::fromTheme(QStringLiteral("draw-ellipse")));
    action->setCheckable(true);
    actions->addAction(QStringLiteral("ellipse"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Insert Character"));
    action->setWhatsThis(i18n("Allows selection of a character from any font to be inserted as a closed sub path. The inserted character will overwrite any existing path, but additional sub paths may be added to the character."));
    action->setData(Editor::Character);
    action->setIcon(QIcon::fromTheme(QStringLiteral("insert-text")));
    action->setCheckable(true);
    actions->addAction(QStringLiteral("character"), action);
    actionGroup->addAction(action);

    connect(actionGroup, SIGNAL(triggered(QAction*)), m_editor, SLOT(selectTool(QAction*)));

    action = new QAction(this);
    action->setText(i18n("Rotate Left"));
    action->setWhatsThis(i18n("Rotate all the points of a path counter-clockwise 90 degrees around the center of the editor."));
    action->setIcon(QIcon::fromTheme(QStringLiteral("object-rotate-left")));
    connect(action, SIGNAL(triggered()), m_editor, SLOT(rotateLeft()));
    actions->addAction(QStringLiteral("rotateLeft"), action);

    action = new QAction(this);
    action->setText(i18n("Rotate Right"));
    action->setWhatsThis(i18n("Rotate all the points of a path clockwise 90 degrees around the center point of the editor."));
    action->setIcon(QIcon::fromTheme(QStringLiteral("object-rotate-right")));
    connect(action, SIGNAL(triggered()), m_editor, SLOT(rotateRight()));
    actions->addAction(QStringLiteral("rotateRight"), action);

    action = new QAction(this);
    action->setText(i18n("Flip Horizontal"));
    action->setWhatsThis(i18n("Flip all the points of the path horizontally about the vertical center of the editor."));
    action->setIcon(QIcon::fromTheme(QStringLiteral("object-flip-horizontal")));
    connect(action, SIGNAL(triggered()), m_editor, SLOT(flipHorizontal()));
    actions->addAction(QStringLiteral("flipHorizontal"), action);

    action = new QAction(this);
    action->setText(i18n("Flip Vertical"));
    action->setWhatsThis(i18n("Flip all the points of the path vertically about the horizontal center of the editor."));
    action->setIcon(QIcon::fromTheme(QStringLiteral("object-flip-vertical")));
    connect(action, SIGNAL(triggered()), m_editor, SLOT(flipVertical()));
    actions->addAction(QStringLiteral("flipVertical"), action);

    action = new QAction(this);
    action->setText(i18n("Scale to Preferred Size"));
    action->setWhatsThis(i18n("Scale the current symbol so that it fits within the preferred size of a symbol."));
    action->setIcon(QIcon::fromTheme(QStringLiteral("symboleditor-scale-preferred")));
    connect(action, SIGNAL(triggered()), m_editor, SLOT(scalePreferred()));
    actions->addAction(QStringLiteral("scalePreferred"), action);

    action = new QAction(this);
    action->setText(i18n("Enable Snap"));
    action->setWhatsThis(i18n("Enable snapping of points to guide intersections or to the grid."));
    action->setIcon(QIcon::fromTheme(QStringLiteral("snap-orthogonal")));
    action->setCheckable(true);
    connect(action, SIGNAL(toggled(bool)), m_editor, SLOT(enableSnap(bool)));
    actions->addAction(QStringLiteral("enableSnap"), action);

    action = new QAction(this);
    action->setText(i18n("Enable Guides"));
    action->setWhatsThis(i18n("Enable the generation of guide intersections."));
    action->setIcon(QIcon::fromTheme(QStringLiteral("snap-intersection")));
    action->setCheckable(true);
    connect(action, SIGNAL(toggled(bool)), m_editor, SLOT(enableGuides(bool)));
    actions->addAction(QStringLiteral("enableGuides"), action);

    // Settings Menu
    KStandardAction::preferences(this, SLOT(preferences()), actions);
}


/**
 * Set the actions status based on the settings in the specified Symbol.
 *
 * @param symbol a const reference to a Symbol
 */
void MainWindow::setActionsFromSymbol(const Symbol &symbol)
{
    action(QStringLiteral("fillPath"))->setChecked(symbol.filled());

    switch (symbol.path().fillRule()) {
    case Qt::WindingFill:
        action(QStringLiteral("windingFill"))->setChecked(true);
        break;

    case Qt::OddEvenFill:
        action(QStringLiteral("oddEvenFill"))->setChecked(true);
        break;
    }

    switch (symbol.capStyle()) {
    case Qt::FlatCap:
        action(QStringLiteral("flatCap"))->setChecked(true);
        break;

    case Qt::SquareCap:
        action(QStringLiteral("squareCap"))->setChecked(true);
        break;

    case Qt::RoundCap:
        action(QStringLiteral("roundCap"))->setChecked(true);
        break;

    case Qt::MPenCapStyle: // this is a combination of the Qt::FlatCap, Qt::SquareCap and Qt::RoundCap
        break;
    }

    switch (symbol.joinStyle()) {
    case Qt::BevelJoin:
        action(QStringLiteral("bevelJoin"))->setChecked(true);
        break;

    case Qt::MiterJoin:
    case Qt::SvgMiterJoin:
        action(QStringLiteral("miterJoin"))->setChecked(true);
        break;

    case Qt::RoundJoin:
        action(QStringLiteral("roundJoin"))->setChecked(true);
        break;

    case Qt::MPenJoinStyle: // this is a combination of Qt::BevelJoin, Qt::MiterJoin, Qt::SvgMiterJoin and Qt::RoundJoin
        break;
    }

    action(QStringLiteral("increaseLineWidth"))->setDisabled(symbol.lineWidth() == 1.00);
    action(QStringLiteral("decreaseLineWidth"))->setDisabled(symbol.lineWidth() == 0.01);
}

#include "moc_MainWindow.cpp"
