/********************************************************************************
 * Copyright (C) 2012 by Stephen Allewell                                       *
 * stephen@mirramar.adsl24.co.uk                                                *
 *                                                                              *
 * This program is free software; you can redistribute it and/or modify         *
 * it under the terms of the GNU General Public License as published by         *
 * the Free Software Foundation; either version 2 of the License, or            *
 * (at your option) any later version.                                          *
 ********************************************************************************/


/**
 * @file
 * Implement the SymbolListWidget class
 */


/**
 * @page library_list_widget SymbolListWidget
 * This class implements an extension to the KListWidget class that provides population of the widget
 * with the contents of a SymbolLibrary. For each Symbol in the library a QListWidgetItem is created
 * with a data item representing the Symbol identifier in the library and an icon at a given size that
 * is generated from the Symbol path.
 *
 * The widget is intended to be used in a dialog or main window and allows selection of a symbol to be
 * used for some purpose in the application.
 *
 * @image html ui-main-library.png "The user interface showing the library tab"
 */


#include "SymbolListWidget.h"

#include <QPainter>
#include <QPen>

#include "Symbol.h"
#include "SymbolLibrary.h"


/**
 * Constructor.
 */
SymbolListWidget::SymbolListWidget(QWidget *parent)
    :   KListWidget(parent),
        m_library(0),
        m_lastIndex(0)
{
    setResizeMode(QListView::Adjust);
    setViewMode(QListView::IconMode);
    setIconSize(48);
}


/**
 * Destructor.
 */
SymbolListWidget::~SymbolListWidget()
{
}


/**
 * Set the size of the icons to be used.
 * The base QListWidget has the icon size and grid size set to this value.
 * If there are existing items in the KListWidget, they are updated with new icons.
 *
 * @param size the size in pixels
 */
void SymbolListWidget::setIconSize(int size)
{
    m_size = size;
    KListWidget::setIconSize(QSize(m_size, m_size));
    setGridSize(QSize(m_size, m_size));
    updateIcons();
}


/**
 * Populate the KListWidget with the QListWidgetItems for each Symbol in the SymbolLibrary.
 * An icon is created for each Symbol.
 *
 * @param library a pointer to the SymbolLibrary containing the Symbols
 */
void SymbolListWidget::loadFromLibrary(SymbolLibrary *library)
{
    if (!library)
        return;

    m_library = library;
//    QList<qint16> keys = library->indexes();
    foreach (qint16 index, library->indexes())
        addSymbol(index, library->symbol(index));
}


/**
 * Add an individual Symbol to the view.
 *
 * @param index the index of the Symbol
 * @param symbol a const reference to the Symbol to add
 */
void SymbolListWidget::addSymbol(qint16 index, const Symbol &symbol)
{
    QListWidgetItem *item = createItem(index);
    item->setIcon(createIcon(symbol, m_size));
}


/**
 * Remove a symbol item from the view.
 *
 * @param index the index of the item to remove
 */
void SymbolListWidget::removeSymbol(qint16 index)
{
    if (m_items.contains(index))
    {
        delete m_items.take(index);
    }
}


/**
 * If an item for the index currently exists return it otherwised create
 * an item to be inserted into the KListWidget.
 * The item created has a data entry added representing the index.
 * The items are inserted so that the Symbols are sorted by their index.
 *
 * @param index an index in the SymbolLibrary
 *
 * @return a pointer to the QListWidgetItem created
 */
QListWidgetItem *SymbolListWidget::createItem(qint16 index)
{
    if (m_items.contains(index))
        return m_items.value(index);
    QListWidgetItem *item = new QListWidgetItem;
    item->setData(Qt::UserRole, index);
    m_items.insert(index, item);
    int i = index;
    while (++i < m_lastIndex)
        if (m_items.contains(i))
            break;
    if (i >= m_lastIndex)
    {
        addItem(item);
        m_lastIndex = index;
    }
    else
        insertItem(row(m_items[i]), item);
    return item;
}


/**
 * Create a QIcon for the supplied Symbol.
 *
 * @param symbol a const reference to a Symbol
 * @param size a size for the icon
 *
 * @return a QIcon
 */
QIcon SymbolListWidget::createIcon(const Symbol &symbol, int size)
{
    QPixmap icon(size, size);
    icon.fill(Qt::white);
    QPainter p(&icon);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.scale(size, size);
    QBrush brush(symbol.filled()?Qt::SolidPattern:Qt::NoBrush);
    QPen pen;
    if (!symbol.filled())
    {
        pen.setWidthF(symbol.lineWidth());
        pen.setCapStyle(symbol.capStyle());
        pen.setJoinStyle(symbol.joinStyle());
    }
    p.setBrush(brush);
    p.setPen(pen);
    p.drawPath(symbol.path());
    p.end();
    return QIcon(icon);
}


/**
 * Generate the icons for all the QListWidgetItems stored in m_items.
 */
void SymbolListWidget::updateIcons()
{
    foreach (qint16 index, m_items.keys())
        m_items.value(index)->setIcon(createIcon(m_library->symbol(index), m_size));
}
