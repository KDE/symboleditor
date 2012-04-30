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
 * Implement the SymbolLibrary class
 */


/**
 * @page symbol_library Symbol Library
 * The symbols created are stored in symbol files. A number of different files can be created containing symbols
 * of different themes. When files are opened the symbol library will be populated with the contents of the file.
 *
 * The symbol library provides an icon view of all the existing symbols. Double clicking on one of the icons will
 * open this symbol in the editor and allow it to be modified. New symbols created can be saved to the library.
 * Note that symbols saved to the library are not immediately saved to disk. The user is required to save the
 * library with the File->Save command. The user may also use the File->Save As command to save the library to a
 * different file.
 *
 * @image html ui-main-library.png "The user interface showing the library tab"
 *
 * All changes to the library through adding new symbols, editing a symbol or importing a library can be undone
 * or redone with the undo and redo commands. A separate undo stack is managed for the library and for the editor
 * and the active one is dependent on which of the tabs is selected. Undoing commands in the editor tab does not
 * affect the library, similarly undoing commands in the library does not affect the editor.
 *
 * A context menu is available for the symbols in the list allowing individual symbols to be deleted. This can be
 * undone if required.
 *
 * Using the File->Import Library it is also possible to import symbols from another symbol file into the current
 * symbol library. These will then be appended to the current set of symbols.
 *
 * File->Close will close the current library leaving a new empty library that new symbols can be added to.
 */


#include "SymbolLibrary.h"

#include <QDataStream>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPainter>
#include <QtAlgorithms>

#include <KLocale>

#include "Exceptions.h"


/**
 * Construct a SymbolLibrary.
 * Set the url to Untitled and the index to 0.
 * The index will be set when a file is loaded and will be incremented when a new symbols
 * has been added. It will be saved with the file for the next time it is loaded.
 */
SymbolLibrary::SymbolLibrary(QListWidget *listWidget)
    :   m_listWidget(listWidget)
{
    clear();
}


/**
 * Destructor
 */
SymbolLibrary::~SymbolLibrary()
{
    clear();
}


/**
 * Clear the file of symbols.
 * Clears the undo stack, deletes all the QListWidgetItems and clears the symbol map.
 * The index is reset to 1.
 * The url is initialized to Untitled
 */
void SymbolLibrary::clear()
{
    m_undoStack.clear();
    m_symbols.clear();
    foreach (QListWidgetItem *item, m_listItems)
        delete item;
    m_listItems.clear();
    m_nextIndex = 1;
    m_url = KUrl(i18n("Untitled"));
}


/**
 * Get the path associated with an index.
 * If the index is not in the library it returns a default constructed Symbol.
 *
 * @param index a qint16 representing the index to find
 *
 * @return a Symbol
 */
Symbol SymbolLibrary::symbol(qint16 index)
{
    return (m_symbols.contains(index)?m_symbols[index]:Symbol());
}


/**
 * Take a symbol from the library.
 * Remove a symbol identified by it's index and return it.
 * The QListWidgetItem associated with the symbol is also removed and deleted.
 * If the index is not in the libary it returns a default constructed symbol.
 *
 * @param index the index of the Symbol to be removed
 *
 * @return a Symbol
 */
Symbol SymbolLibrary::takeSymbol(qint16 index)
{
    Symbol symbol;
    if (m_symbols.contains(index))
    {
        symbol = m_symbols.take(index);
        delete m_listItems.take(index);
    }
    return symbol;
}


/**
 * Update the Symbol for an index in the library.
 * If the index supplied is 0, a new index will be taken from the m_nextIndex value which is
 * the incremented. This value will be returned.
 * For new symbols a new QListWidgetItem is created. The icon is created for new symbols and
 * updated for existing ones.
 *
 * @param index a qint16 representing the index
 * @param symbol a const reference to a Symbol
 *
 * @return a qint16 representing the index, this is useful when the original index was 0
 */
qint16 SymbolLibrary::setSymbol(qint16 index, const Symbol &symbol)
{
    if (!index)
        index = m_nextIndex++;
    m_symbols.insert(index, symbol);

    if (m_listWidget)
    {
        QListWidgetItem *item = (m_listItems.contains(index))?m_listItems[index]:generateItem(index);
        item->setIcon(generateIcon(symbol));
    }

    return index;
}


/**
 * Get the QListWidgetItem associated with this symbol.
 *
 * @param index the index of the symbol
 *
 * @return a pointer to a QListWidgetItem
 */
QListWidgetItem *SymbolLibrary::item(qint16 index)
{
    QListWidgetItem *item = 0;
    if (m_listItems.contains(index))
        item = m_listItems[index];
    return item;
}


/**
 * Get the url for the file.
 *
 * @return a reference to a KUrl having the url of the file
 */
KUrl SymbolLibrary::url() const
{
    return m_url;
}


/**
 * Set the url for the file.
 *
 * @param url a const reference to a KUrl having the url of the file.
 */
void SymbolLibrary::setUrl(const KUrl &url)
{
    m_url = url;
}


/**
 * Get a sorted list of symbol indexes
 *
 * @return a QList<qint16> of sorted indexes
 */
QList<qint16> SymbolLibrary::indexes() const
{
    QList<qint16> keys = m_symbols.keys();
    qSort(keys.begin(), keys.end());
    return keys;
}


/**
 * Get a pointer to the symbol library undo stack.
 *
 * @return a pointer to a QUndoStack
 */
QUndoStack *SymbolLibrary::undoStack()
{
    return &m_undoStack;
}


/**
 * Generate all the items in the library.
 * This will be called when a library file is loaded to generate all the new
 * QListWidgetItems for the symbols in the library and generate an icon for it.
 * A list of sorted QMap keys is retrieved to allow adding items in the correct
 * order.
 */
void SymbolLibrary::generateItems()
{
    if (!m_listWidget)
        return;
    QList<qint16> keys = indexes();
    foreach (qint16 index, keys)
    {
        QListWidgetItem *item = generateItem(index);
        item->setIcon(generateIcon(m_symbols[index]));
    }
}


/**
 * Generate the QListWidgetItem for the specified symbol.
 * The new item is assigned the index and is added to or inserted into the list of
 * items and is also added to the QListWidget.
 *
 * @param index the index of the item
 *
 * @return a pointer to a QListWidgetItem
 */
QListWidgetItem *SymbolLibrary::generateItem(qint16 index)
{
    QListWidgetItem *item = new QListWidgetItem;
    item->setData(Qt::UserRole, index);
    m_listItems.insert(index, item);
    int i = index;
    while (++i < m_nextIndex)
        if (m_listItems.contains(i))
            break;
    if (i == m_nextIndex)
        m_listWidget->addItem(item);
    else
        m_listWidget->insertItem(m_listWidget->row(m_listItems[i]), item);
    return item;
}


/**
 * Generate a QIcon for the supplied Symbol.
 *
 * @param symbol a const reference to a Symbol
 *
 * @return a QIcon
 */
QIcon SymbolLibrary::generateIcon(const Symbol &symbol)
{
    QPixmap icon(48, 48);
    icon.fill(Qt::white);
    QPainter p(&icon);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.scale(48, 48);
    QBrush brush(symbol.filled()?Qt::SolidPattern:Qt::NoBrush);
    QPen pen;
    pen.setWidthF(symbol.lineWidth());
    pen.setCapStyle(symbol.capStyle());
    pen.setJoinStyle(symbol.joinStyle());
    p.setBrush(brush);
    p.setPen(pen);
    p.drawPath(symbol.path());
    p.end();
    return QIcon(icon);
}


/**
 * Stream out the file.
 * Symbol files are indicated with a magic string of KXStitchSymbols. The stream version is set
 * to maintain consistency with the streamed objects.
 * Write the version, current index and the map of symbols.
 *
 * @param stream a reference to a QDataStream
 * @param library a const reference to a SymbolLibrary
 *
 * @return a reference to a QDataStream
 */
QDataStream &operator<<(QDataStream &stream, const SymbolLibrary &library)
{
    stream.writeRawData("KXStitchSymbols", 15);
    stream.setVersion(QDataStream::Qt_4_0);
    stream << library.version;
    stream << library.m_nextIndex;
    if (stream.status() != QDataStream::Ok)
        throw FailedWriteLibrary(stream.status());
    stream << library.m_symbols;
    return stream;
}


/**
 * Stream in the file.
 * Initially clear the current contents and reset the url.
 * Symbol files are indicated with a magic string of KXStitchSymbols. The stream version is set
 * to maintain consistency with the streamed objects. Read and check the magic string. If this
 * is not a symbol file throw an exception.
 * Read in the version.
 * Read the data for the specified version. The stream is checked for errors and an exception is
 * thrown if there was an error.
 *
 * @param stream a reference to a QDataStream
 * @param library a reference to a SymbolLibrary
 *
 * @return a reference to a QDataStream
 */
QDataStream &operator>>(QDataStream &stream, SymbolLibrary &library)
{
    library.clear();

    char magic[15];
    stream.readRawData(magic, 15);
    if (strncmp(magic, "KXStitchSymbols", 15) == 0)
    {
        stream.setVersion(QDataStream::Qt_4_0);
        qint32 version;
        qint16 nextIndex;
        QMap<qint16, QPainterPath> paths_v100;
        QList<qint16> paths_v100_keys;
        stream >> version;
        switch (version)
        {
            case 101:
                stream >> library.m_nextIndex;
                if (stream.status() != QDataStream::Ok)
                    throw FailedReadLibrary(stream.status());
                stream >> library.m_symbols;
                library.generateItems();
                break;

            case 100:
                stream >> library.m_nextIndex;
                library.m_nextIndex++;
                stream >> paths_v100;
                paths_v100_keys = paths_v100.keys();
                foreach (qint16 index, paths_v100_keys)
                {
                    Symbol symbol;
                    symbol.setPath(paths_v100[index]);
                    library.setSymbol(index, symbol);
                }
                break;

            default:
                throw InvalidFileVersion(version);
                break;
        }
    }
    else
        throw InvalidFile();

    return stream;
}
