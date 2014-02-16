/********************************************************************************
 * Copyright (C) 2012 by Stephen Allewell                                       *
 * sallewell@users.sourceforge.net                                              *
 *                                                                              *
 * This program is free software; you can redistribute it and/or modify         *
 * it under the terms of the GNU General Public License as published by         *
 * the Free Software Foundation; either version 2 of the License, or            *
 * (at your option) any later version.                                          *
 ********************************************************************************/


/**
 * @file
 * Header file for the SymbolListWidget class.
 */


#ifndef SymbolListWidget_H
#define SymbolListWidget_H


#include <KListWidget>


class QMimeData;

class SymbolLibrary;
class Symbol;


/**
 * @brief An extension to the KListWidget to view and select Symbols.
 *
 * This widget is an extension to the KListWidget that can be populated from
 * a SymbolLibrary to display the contained Symbols and allow selection of one
 * of the Symbols for further processing.
 *
 * Additional Symbols can be added individually and all Symbols will be sorted
 * in the view by their index value.
 *
 * Symbols can be removed by their index value.
 */
class SymbolListWidget : public KListWidget
{
public:
    explicit SymbolListWidget(QWidget *parent);
    ~SymbolListWidget();

    void setIconSize(int size);
    void loadFromLibrary(SymbolLibrary *library);
    void addSymbol(qint16 index, const Symbol &symbol);
    void removeSymbol(qint16 index);

    static QIcon createIcon(const Symbol &symbol, int size);

protected:
    virtual QStringList mimeTypes() const;
    virtual Qt::DropActions supportedDropActions() const;
    virtual QMimeData *mimeData(const QList<QListWidgetItem *> items) const;
    virtual bool dropMimeData(int index, const QMimeData *mimeData, Qt::DropAction action);

private:
    QListWidgetItem *createItem(qint16 index);
    void updateIcons();

    int             m_size;                     /**< size of icons generated in the view */
    SymbolLibrary   *m_library;                 /**< pointer to the library the items belong to */

    qint16          m_lastIndex;                /**< the last index in the list */

    QMap<qint16, QListWidgetItem*>  m_items;    /**< map of index to QListWidgetItem */
};


#endif
