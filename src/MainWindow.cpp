/********************************************************************************
 * Copyright (C) 2011 by Stephen Allewell                                       *
 * stephen@mirramar.adsl24.co.uk                                                *
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
 * multiple symbols to be easily created based on the same design.
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
 * - @ref rotate_left
 * - @ref rotate_right
 * - @ref flip_horizontal
 * - @ref flip_vertical
 * - @ref snap_grid
 */


#include "MainWindow.h"

#include <QVBoxLayout>
#include <QListWidgetItem>
#include <QMenu>

#include <KAction>
#include <KActionCollection>
#include <KFileDialog>
#include <KGlobalSettings>
#include <KIO/NetAccess>
#include <KLocale>
#include <KMessageBox>
#include <KRecentFilesAction>
#include <KStatusBar>
#include <KTabWidget>
#include <KUrl>

#include "Editor.h"
#include "Exceptions.h"
#include "SymbolListWidget.h"
#include "SymbolLibrary.h"


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
    :   m_tabWidget(new KTabWidget(this)),
        m_editor(new Editor),
        m_listWidget(new SymbolListWidget(m_tabWidget)),
        m_symbolLibrary(new SymbolLibrary(m_listWidget)),
        m_item(0),
        m_menu(0)
{
    setObjectName("MainWindow#");

    KActionCollection *actions = actionCollection();

    QVBoxLayout *editorLayout = new QVBoxLayout;
    editorLayout->addWidget(m_editor, 0, Qt::AlignCenter);
    QWidget *layoutWidget = new QWidget;
    layoutWidget->setLayout(editorLayout);

    m_listWidget->setIconSize(48);
    m_listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    m_tabWidget->addTab(layoutWidget, "Editor");
    m_tabWidget->addTab(m_listWidget, "Symbol Library");

    setCentralWidget(m_tabWidget);

    setupActions();

    m_undoGroup.addStack(m_editor->undoStack());
    m_undoGroup.addStack(m_symbolLibrary->undoStack());
    connect(m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(currentChanged(int)));
    connect(m_editor, SIGNAL(message(QString)), statusBar(), SLOT(showMessage(QString)));
    connect(m_editor, SIGNAL(minLineWidth(bool)), actions->action("decreaseLineWidth"), SLOT(setDisabled(bool)));
    connect(m_editor, SIGNAL(maxLineWidth(bool)), actions->action("increaseLineWidth"), SLOT(setDisabled(bool)));
    connect(&m_undoGroup, SIGNAL(canUndoChanged(bool)), actions->action("edit_undo"), SLOT(setEnabled(bool)));
    connect(&m_undoGroup, SIGNAL(canRedoChanged(bool)), actions->action("edit_redo"), SLOT(setEnabled(bool)));
    connect(&m_undoGroup, SIGNAL(undoTextChanged(QString)), this, SLOT(undoTextChanged(QString)));
    connect(&m_undoGroup, SIGNAL(redoTextChanged(QString)), this, SLOT(redoTextChanged(QString)));
    connect(&m_undoGroup, SIGNAL(cleanChanged(bool)), this, SLOT(cleanChanged(bool)));
    connect(m_editor->undoStack(), SIGNAL(cleanChanged(bool)), actions->action("saveSymbol"), SLOT(setDisabled(bool)));
    connect(m_editor->undoStack(), SIGNAL(cleanChanged(bool)), actions->action("saveSymbolAsNew"), SLOT(setDisabled(bool)));
    connect(m_symbolLibrary->undoStack(), SIGNAL(cleanChanged(bool)), actions->action("file_save"), SLOT(setDisabled(bool)));
    connect(m_listWidget, SIGNAL(executed(QListWidgetItem*)), this, SLOT(itemSelected(QListWidgetItem*)));
    connect(m_listWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(listWidgetContextMenuRequested(QPoint)));

    setupGUI(KXmlGuiWindow::Default, "SymbolEditorui.rc");

    m_tabWidget->setCurrentIndex(0);                        // select the editor
    currentChanged(0);                                      // setting current index above doesn't trigger the signal
    actions->action("moveTo")->trigger();	                // select draw tool
    actions->action("enableSnap")->setChecked(true);        // enable snap
    actions->action("file_save")->setEnabled(false);        // nothing to save yet
    actions->action("saveSymbol")->setEnabled(false);       // nothing to save yet
    actions->action("saveSymbolAsNew")->setEnabled(false);  // nothing to save yet
    setActionsFromSymbol(m_editor->symbol().second);        // set the actions that depend on the current empty symbol, i.e. the defaults
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
    if (!clean)
    {
        int messageBoxResult = KMessageBox::warningYesNoCancel(this, i18n("Save changes to the symbol?\nSelecting No discards changes."));
        switch (messageBoxResult)
        {
            case KMessageBox::Yes:
                saveSymbol();
                save();
                clean = true;
                break;

            case KMessageBox::No:
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
    if (!clean)
    {
        int messageBoxResult = KMessageBox::warningYesNoCancel(this, i18n("Save changes to the library?\nSelecting No discards changes."));
        switch (messageBoxResult)
        {
            case KMessageBox::Yes:
                save();
                clean = true;
                break;

            case KMessageBox::No:
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
 * Test if it is ok to close the application.
 *
 * @return return true since queryClose has already been called
 */
bool MainWindow::queryExit()
{
    return true;
}


/**
 * Open a file.
 * Use the KFileDialog::getOpenUrl to get a KUrl to open which is then passed to filOpen(const KURl &).
 */
void MainWindow::fileOpen()
{
    KUrl url = KFileDialog::getOpenUrl(KUrl("kfiledialog:///"), i18n("*.sym|Cross Stitch Symbols"), this);
    if (!url.isEmpty())
        fileOpen(url);
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
void MainWindow::fileOpen(const KUrl &url)
{
    if (!editorClean() || !libraryClean())
        return;

    m_symbolLibrary->clear();
    m_editor->clear();

    if (url.isValid())
    {
        QString src;
        if (KIO::NetAccess::download(url, src, 0))
        {
            QFile file(src);
            if (file.open(QIODevice::ReadOnly))
            {
                QDataStream stream(&file);
                try
                {
                    stream >> *m_symbolLibrary;
                    m_symbolLibrary->setUrl(url);
                    KRecentFilesAction *action = static_cast<KRecentFilesAction *>(actionCollection()->action("file_open_recent"));
                    action->addUrl(url);
                    action->saveEntries(KConfigGroup(KGlobal::config(), "RecentFiles"));
                    m_tabWidget->setCurrentIndex(1);
                }
                catch (const InvalidFile &e)
                {
                    KMessageBox::sorry(0, i18n("This doesn't appear to be a valid symbol file"));
                }
                catch (const InvalidFileVersion &e)
                {
                    KMessageBox::sorry(0, i18n("Version %1 of the library file is not supported in this version of SymbolEditor", e.version));
                }
                catch (const InvalidSymbolVersion &e)
                {
                    KMessageBox::sorry(0, i18n("Version %1 of the symbol is not supported in this version of SymbolEditor", e.version));
                }
                catch (const FailedReadLibrary &e)
                {
                    KMessageBox::sorry(0, i18n("Failed to read the library\n%1", e.statusMessage()));
                    m_symbolLibrary->clear();
                }
                file.close();
            }
            else
                KMessageBox::sorry(0, i18n("Failed to open the file %1", url.fileName()));
        }
        else
            KMessageBox::sorry(0, i18n("Failed to download the file %1", url.fileName()));
    }
    else
        KMessageBox::sorry(0, i18n("The url %1 is invalid", url.fileName()));
}


/**
 * Save the library using its url, if this is Untitled than call saveAs to get a valid url.
 * Open the file and write to the stream. This is protected in a try-catch block to intercept
 * any exceptions thrown by the writing routines. If there were any exceptions thrown or the
 * file could not be opened a suitable error message is shown.
 */
void MainWindow::save()
{
    KUrl url = m_symbolLibrary->url();
    if (url == KUrl(i18n("Untitled")))
        saveAs();
    else
    {
        QFile file(url.path());
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            QDataStream stream(&file);
            stream.setVersion(QDataStream::Qt_4_0);
            try
            {
                stream << *m_symbolLibrary;
            }
            catch (const FailedWriteLibrary &e)
            {
                KMessageBox::sorry(0, i18n("Failed to write the library\n%1", e.statusMessage()));
            }
            file.close();
            m_symbolLibrary->undoStack()->setClean();
        }
        else
            KMessageBox::sorry(0, i18n("Failed to open the file %1\n%2", url.fileName(), file.errorString()));
    }
}


/**
 * Save the library using a different url.
 * This is also called from save when the assigned url is Untitled.
 * The new url is added to the recent files list.
 */
void MainWindow::saveAs()
{
    KUrl url = KFileDialog::getSaveUrl(QString("::%1").arg(KGlobalSettings::documentPath()), i18n("*.sym|Cross Stitch Symbols"), this, i18n("Save As"));
    if (url.isValid())
    {
        if (KIO::NetAccess::exists(url, false, 0))
        {
            if (KMessageBox::warningYesNo(this, i18n("This file already exists\nDo you want to overwrite it?")) == KMessageBox::No)
                return;
        }
        m_symbolLibrary->setUrl(url);
        save();
        KRecentFilesAction *action = static_cast<KRecentFilesAction *>(actionCollection()->action("file_open_recent"));
        action->addUrl(url);
        action->saveEntries(KConfigGroup(KGlobal::config(), "RecentFiles"));
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
    if (editorClean())
    {
        m_editor->clear();
        setActionsFromSymbol(m_editor->symbol().second);
        actionCollection()->action("moveTo")->trigger();   // Select move tool
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
    KUrl url = KFileDialog::getOpenUrl(KUrl("kfiledialog:///"), i18n("*.sym|Cross Stitch Symbols"), this);
    if (url.isEmpty())
        return;
    if (url.isValid())
    {
        QString src;
        if (KIO::NetAccess::download(url, src, 0))
        {
            QFile file(src);
            if (file.open(QIODevice::ReadOnly))
            {
                SymbolLibrary *lib = new SymbolLibrary;
                QDataStream stream(&file);
                try
                {
                    stream >> *lib;
                    m_symbolLibrary->undoStack()->push(new ImportLibraryCommand(m_symbolLibrary, lib));
                }
                catch (const InvalidFile &e)
                {
                    KMessageBox::sorry(0, i18n("This doesn't appear to be a valid symbol file"));
                }
                catch (const InvalidFileVersion &e)
                {
                    KMessageBox::sorry(0, i18n("Version %1 of the library file is not supported in this version of SymbolEditor", e.version));
                }
                file.close();
            }
            else
                KMessageBox::sorry(0, i18n("Failed to open the file %1", url.fileName()));
        }
        else
            KMessageBox::sorry(0, i18n("Failed to download the file %1", url.fileName()));
    }
    else
        KMessageBox::sorry(0, i18n("The url %1 is invalid", url.fileName()));
}


/**
 * Close the current library.
 * Check if the current symbol and the symbol library need to be saved and then clear
 * the library and the editor.
 */
void MainWindow::close()
{
    if (editorClean() && libraryClean())
    {
        m_editor->clear();
        m_symbolLibrary->clear();
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
    actionCollection()->action("edit_undo")->setText(QString(i18n("Undo %1", text)));
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
    actionCollection()->action("edit_redo")->setText(QString(i18n("Redo %1", text)));
}


/**
 * Update the caption based on the state of the undo stack.
 *
 * @param clean true if the symbol has not been changed, false otherwise
 */
void MainWindow::cleanChanged(bool clean)
{
    QString tab(i18nc("The Editor tab title", "Editor"));
    if (m_tabWidget->currentIndex() == 1)
        tab = QString(i18nc("The Library tab title", "Library"));
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
    if (index == 0) // Editor
        m_undoGroup.setActiveStack(m_editor->undoStack());
    else if (index == 1) // QListWidget
        m_undoGroup.setActiveStack(m_symbolLibrary->undoStack());
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
    if (editorClean())
    {
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
    if (m_item = m_listWidget->itemAt(pos))
    {
        if (!m_menu)
        {
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
 * Set up the applications actions.
 * Create standard actions.
 * Create other actions, setting the icon and data as required.
 * Several actions are added to groups which are set as exclusive.
 * All actions are added to the applications KActionCollection.
 */
void MainWindow::setupActions()
{
    KAction *action;
    QActionGroup *actionGroup;

    KActionCollection *actions = actionCollection();

    // File menu
    KStandardAction::open(this, SLOT(fileOpen()), actions);
    KStandardAction::openNew(this, SLOT(newSymbol()), actions);
    KStandardAction::openRecent(this, SLOT(fileOpen(KUrl)), actions)->loadEntries(KConfigGroup(KGlobal::config(), "RecentFiles"));
    KStandardAction::save(this, SLOT(save()), actions);
    KStandardAction::saveAs(this, SLOT(saveAs()), actions);

    action = new KAction(this);
    action->setText(i18n("Import Library"));
    connect(action, SIGNAL(triggered()), this, SLOT(importLibrary()));
    actions->addAction("importLibrary", action);

    action = new KAction(this);
    action->setText(i18n("Save Symbol"));
    action->setIcon(KIcon("save-symbol"));
    connect(action, SIGNAL(triggered()), this, SLOT(saveSymbol()));
    actions->addAction("saveSymbol", action);

    action = new KAction(this);
    action->setText(i18n("Save Symbol as New"));
    connect(action, SIGNAL(triggered()), this, SLOT(saveSymbolAsNew()));
    actions->addAction("saveSymbolAsNew", action);

    KStandardAction::close(this, SLOT(close()), actions);
    KStandardAction::quit(this, SLOT(quit()), actions);

    // Edit menu
    KStandardAction::undo(this, SLOT(undo()), actions);
    KStandardAction::redo(this, SLOT(redo()), actions);

    // Rendering menu
    action = new KAction(this);
    action->setText(i18n("Fill Path"));
    action->setIcon(KIcon("rating"));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered(bool)), m_editor, SLOT(selectFilled(bool)));
    actions->addAction("fillPath", action);

    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = new KAction(this);
    action->setText(i18n("Odd Even Fill"));
    action->setData(Qt::OddEvenFill);
    action->setIcon(KIcon("odd-even-fill"));
    action->setCheckable(true);
    actions->addAction("oddEvenFill", action);
    actionGroup->addAction(action);

    action = new KAction(this);
    action->setText(i18n("Winding Fill"));
    action->setData(Qt::WindingFill);
    action->setIcon(KIcon("winding-fill"));
    action->setCheckable(true);
    actions->addAction("windingFill", action);
    actionGroup->addAction(action);

    connect(actionGroup, SIGNAL(triggered(QAction*)), m_editor, SLOT(selectFillRule(QAction*)));

    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = new KAction(this);
    action->setText(i18n("Flat Cap"));
    action->setData(Qt::FlatCap);
    action->setIcon(KIcon("flat-cap"));
    action->setCheckable(true);
    actions->addAction("flatCap", action);
    actionGroup->addAction(action);

    action = new KAction(this);
    action->setText(i18n("Square Cap"));
    action->setData(Qt::SquareCap);
    action->setIcon(KIcon("square-cap"));
    action->setCheckable(true);
    actions->addAction("squareCap", action);
    actionGroup->addAction(action);

    action = new KAction(this);
    action->setText(i18n("Round Cap"));
    action->setData(Qt::RoundCap);
    action->setIcon(KIcon("round-cap"));
    action->setCheckable(true);
    actions->addAction("roundCap", action);
    actionGroup->addAction(action);

    connect(actionGroup, SIGNAL(triggered(QAction*)), m_editor, SLOT(selectCapStyle(QAction*)));

    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = new KAction(this);
    action->setText(i18n("Bevel Join"));
    action->setData(Qt::BevelJoin);
    action->setIcon(KIcon("bevel-join"));
    action->setCheckable(true);
    actions->addAction("bevelJoin", action);
    actionGroup->addAction(action);

    action = new KAction(this);
    action->setText(i18n("Miter Join"));
    action->setData(Qt::MiterJoin);
    action->setIcon(KIcon("miter-join"));
    action->setCheckable(true);
    actions->addAction("miterJoin", action);
    actionGroup->addAction(action);

    action = new KAction(this);
    action->setText(i18n("Round Join"));
    action->setData(Qt::RoundJoin);
    action->setIcon(KIcon("round-join"));
    action->setCheckable(true);
    actions->addAction("roundJoin", action);
    actionGroup->addAction(action);

    connect(actionGroup, SIGNAL(triggered(QAction*)), m_editor, SLOT(selectJoinStyle(QAction*)));

    action = new KAction(this);
    action->setText(i18n("Increase Line Width"));
    action->setIcon(KIcon("increase-line-width"));
    connect(action, SIGNAL(triggered()), m_editor, SLOT(increaseLineWidth()));
    actions->addAction("increaseLineWidth", action);

    action = new KAction(this);
    action->setText(i18n("Decrease Line Width"));
    action->setIcon(KIcon("decrease-line-width"));
    connect(action, SIGNAL(triggered()), m_editor, SLOT(decreaseLineWidth()));
    actions->addAction("decreaseLineWidth", action);

    // Tools Menu
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = new KAction(this);
    action->setText(i18n("Move To"));
    action->setData(Editor::MoveTo);
    action->setIcon(KIcon("go-jump"));
    action->setCheckable(true);
    actions->addAction("moveTo", action);
    actionGroup->addAction(action);

    action = new KAction(this);
    action->setText(i18n("Draw To"));
    action->setData(Editor::LineTo);
    action->setIcon(KIcon("draw-line"));
    action->setCheckable(true);
    actions->addAction("lineTo", action);
    actionGroup->addAction(action);

    action = new KAction(this);
    action->setText(i18n("Cubic To"));
    action->setData(Editor::CubicTo);
    action->setIcon(KIcon("draw-bezier-curves"));
    action->setCheckable(true);
    actions->addAction("cubicTo", action);
    actionGroup->addAction(action);

    action = new KAction(this);
    action->setText(i18n("Rectangle"));
    action->setData(Editor::Rectangle);
    action->setIcon(KIcon("draw-rectangle"));
    action->setCheckable(true);
    actions->addAction("rectangle", action);
    actionGroup->addAction(action);

    action = new KAction(this);
    action->setText(i18n("Ellipse"));
    action->setData(Editor::Ellipse);
    action->setIcon(KIcon("draw-ellipse"));
    action->setCheckable(true);
    actions->addAction("ellipse", action);
    actionGroup->addAction(action);

    connect(actionGroup, SIGNAL(triggered(QAction*)), m_editor, SLOT(selectTool(QAction*)));

    action = new KAction(this);
    action->setText(i18n("Rotate Left"));
    action->setIcon(KIcon("object-rotate-left"));
    connect(action, SIGNAL(triggered()), m_editor, SLOT(rotateLeft()));
    actions->addAction("rotateLeft", action);

    action = new KAction(this);
    action->setText(i18n("Rotate Right"));
    action->setIcon(KIcon("object-rotate-right"));
    connect(action, SIGNAL(triggered()), m_editor, SLOT(rotateRight()));
    actions->addAction("rotateRight", action);

    action = new KAction(this);
    action->setText(i18n("Flip Horizontal"));
    action->setIcon(KIcon("object-flip-horizontal"));
    connect(action, SIGNAL(triggered()), m_editor, SLOT(flipHorizontal()));
    actions->addAction("flipHorizontal", action);

    action = new KAction(this);
    action->setText(i18n("Flip Vertical"));
    action->setIcon(KIcon("object-flip-vertical"));
    connect(action, SIGNAL(triggered()), m_editor, SLOT(flipVertical()));
    actions->addAction("flipVertical", action);

    action = new KAction(this);
    action->setText(i18n("Enable Snap"));
    action->setIcon(KIcon("snap-to-grid"));
    action->setCheckable(true);
    connect(action, SIGNAL(toggled(bool)), m_editor, SLOT(enableSnap(bool)));
    actions->addAction("enableSnap", action);

    // Settings Menu
}


/**
 * Set the actions status based on the settings in the specified Symbol.
 *
 * @param symbol a const reference to a Symbol
 */
void MainWindow::setActionsFromSymbol(const Symbol &symbol)
{
    action("fillPath")->setChecked(symbol.filled());

    switch (symbol.path().fillRule())
    {
        case Qt::WindingFill:
            action("windingFill")->setChecked(true);
            break;

        case Qt::OddEvenFill:
            action("oddEvenFill")->setChecked(true);
            break;
    }

    switch (symbol.capStyle())
    {
        case Qt::FlatCap:
            action("flatCap")->setChecked(true);
            break;

        case Qt::SquareCap:
            action("squareCap")->setChecked(true);
            break;

        case Qt::RoundCap:
            action("roundCap")->setChecked(true);
            break;
    }

    switch (symbol.joinStyle())
    {
        case Qt::BevelJoin:
            action("bevelJoin")->setChecked(true);
            break;

        case Qt::MiterJoin:
            action("miterJoin")->setChecked(true);
            break;

        case Qt::RoundJoin:
            action("roundJoin")->setChecked(true);
            break;
    }

    action("increaseLineWidth")->setDisabled(symbol.lineWidth() == 1.00);
    action("decreaseLineWidth")->setDisabled(symbol.lineWidth() == 0.01);
}

