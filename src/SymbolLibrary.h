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
 * Header file for the SymbolLibrary class.
 */


#ifndef SymbolLibrary_H
#define SymbolLibrary_H


#include <QMap>
#include <QPainterPath>
#include <QUndoStack>

#include <KUrl>


class QDataStream;
class QListWidget;
class QListWidgetItem;


/**
 * @brief Manages the library of symbols.
 *
 * The symbol library holds all the symbols that have been added to it or loaded from a file.
 * The symbols are indexed and the index is incremented for each symbol added starting from 1.
 * For each of the symbols there is a QListWidgetItem which is assigned the QIcon that is
 * generated from the QPainterPath associated with the index.
 */
class SymbolLibrary
{
public:
    SymbolLibrary(QListWidget *listWidget = 0);
    ~SymbolLibrary();

    void clear();

    QPainterPath symbol(qint16 index);
    QPainterPath takeSymbol(qint16 index);
    qint16 setSymbol(qint16 index, const QPainterPath &path);
    QListWidgetItem *item(qint16 index);

    KUrl url() const;
    void setUrl(const KUrl &url);

    QList<qint16> indexes() const;

    QUndoStack *undoStack();

    friend QDataStream &operator<<(QDataStream &stream, const SymbolLibrary &library);
    friend QDataStream &operator>>(QDataStream &stream, SymbolLibrary &library);

private:
    void generateItems();
    QListWidgetItem *generateItem(qint16 index);
    QIcon generateIcon(const QPainterPath &path);

    static const qint32 version = 100;              /**< stream version of this file */

    QUndoStack m_undoStack;                         /**< holds the commands that have made changes to this library */

    KUrl    m_url;                                  /**< url of the file loaded, this may be Untitled */

    QListWidget *m_listWidget;                      /**< pointer to a QListWidget containing the QListWidgetItems for the QIcons, this may be null for an imported file */

    qint16                          m_nextIndex;    /**< index for the next symbol added */
    QMap<qint16, QPainterPath>      m_symbols;      /**< map of the QPainterPath to indexes */
    QMap<qint16, QListWidgetItem *> m_listItems;    /**< map of the QListWidgetItems to indexes */
};


QDataStream &operator<<(QDataStream &stream, const SymbolLibrary &library);
QDataStream &operator>>(QDataStream &stream, SymbolLibrary &library);


#endif

