/********************************************************************************
 * Copyright (C) 2011-2014 by Stephen Allewell                                  *
 * steve.allewell@gmail.com                                                     *
 *                                                                              *
 * This program is free software; you can redistribute it and/or modify         *
 * it under the terms of the GNU General Public License as published by         *
 * the Free Software Foundation; either version 2 of the License, or            *
 * (at your option) any later version.                                          *
 ********************************************************************************/


/**
 * @file
 * Header file for the QUndoCommand derived classes.
 */


#ifndef Commands_H
#define Commands_H


#include <QUndoCommand>
#include <QWidget>

#include "SymbolLibrary.h"


class Editor;


/**
 * @brief Move to command class.
 *
 * Implement a move to operation on the QPainterPath for the current symbol.
 * If this is the first operation it updates the start position of the path, otherwise
 * it ends the current sub path and starts a new sub path at the specified point.
 *
 * The original path is stored for a possible undo.
 */
class MoveToCommand : public QUndoCommand
{
public:
    MoveToCommand(Editor *editor, const QPointF &to);
    virtual ~MoveToCommand();

    virtual void undo();
    virtual void redo();

private:
    Editor          *m_editor;  /**< pointer to the editor */
    QPointF         m_to;       /**< point to move to */
    QPainterPath    m_path;     /**< original path to be restored on undo */
};


/**
 * @brief Line to command class.
 *
 * Implement a line to operation on the QPainterPath for the current symbol.
 * The line is added to the current sub path.
 *
 * The original path is stored for a possible undo.
 */
class LineToCommand : public QUndoCommand
{
public:
    LineToCommand(Editor *editor, const QPointF &to);
    virtual ~LineToCommand();

    virtual void undo();
    virtual void redo();

private:
    Editor          *m_editor;  /**< pointer to the editor */
    QPointF         m_to;       /**< point to draw to */
    QPainterPath    m_path;     /**< original path to be restored on undo */
};


/**
 * @brief Cubic to command class.
 *
 * Implement a cubic to operation on the QPainterPath for the current symbol.
 * The cubic is added to the current sub path.
 *
 * The original path is stored for a possible undo.
 */
class CubicToCommand : public QUndoCommand
{
public:
    CubicToCommand(Editor *editor, const QPointF &control1, const QPointF &control2, const QPointF &to);
    virtual ~CubicToCommand();

    virtual void undo();
    virtual void redo();

private:
    Editor          *m_editor;  /**< pointer to the editor */
    QPointF         m_control1; /**< first control point */
    QPointF         m_control2; /**< second control point */
    QPointF         m_to;       /**< end point */
    QPainterPath    m_path;     /**< original path to be restored on undo */
};


/**
 * @brief Rectangle command class.
 *
 * Implement a rectangle operation on the QPainterPath for the current symbol.
 * The current sub path is closed and the rectangle is added as a new sub path as a
 * series of line to operations.
 *
 * The original path is stored for a possible undo.
 */
class RectangleCommand : public  QUndoCommand
{
public:
    RectangleCommand(Editor *editor, const QPointF &from, const QPointF &to);
    virtual ~RectangleCommand();

    virtual void undo();
    virtual void redo();

private:
    Editor          *m_editor;  /**< pointer to the editor */
    QPointF         m_from;     /**< first corner */
    QPointF         m_to;       /**< second corner */
    QPainterPath    m_path;     /**< original path to be restored on undo */
};


/**
 * @brief Ellipse command class.
 *
 * Implement an ellipse operation on the QPainterPath for the current symbol.
 * The current sub path is closed and the ellipse is added as a new sub path as a
 * series of cubic to operations.
 *
 * The original path is stored for a possible undo.
 */
class EllipseCommand : public QUndoCommand
{
public:
    EllipseCommand(Editor *editor, const QPointF &from, const QPointF &to);
    virtual ~EllipseCommand();

    virtual void undo();
    virtual void redo();

private:
    Editor          *m_editor;  /**< pointer to the editor */
    QPointF         m_from;     /**< first corner */
    QPointF         m_to;       /**< second corner */
    QPainterPath    m_path;     /**< original path to be restored on undo */
};


/**
 * @brief Move point command class.
 *
 * Implement moving a control point in the m_points list from one position to
 * another. The Editor QPainterPath needs to be reconstructed with the moved position.
 *
 * The original position is stored for a possible undo.
 */
class MovePointCommand : public QUndoCommand
{
public:
    MovePointCommand(Editor *editor, int index, const QPointF &from, const QPointF &to);
    virtual ~MovePointCommand();

    virtual void undo();
    virtual void redo();

private:
    Editor          *m_editor;  /**< pointer to the editor */
    int             m_index;    /**< index of the point in m_editor->m_points */
    QPointF         m_from;     /**< original position */
    QPointF         m_to;       /**< destination position */
};


/**
 * @brief Update symbol command class;
 *
 * Implement updating a symbol in the library using the index specified. The index may
 * be 0 for new symbols and a new index will be generated by the library.
 *
 * The original Symbol is stored for a possible undo.
 */
class UpdateSymbolCommand : public QUndoCommand
{
public:
    UpdateSymbolCommand(SymbolLibrary *library, qint16 index, const Symbol &symbol);
    virtual ~UpdateSymbolCommand();

    virtual void undo();
    virtual void redo();

private:
    SymbolLibrary   *m_symbolLibrary;   /**< pointer to the symbol library */
    qint16          m_index;            /**< index of the symbol in the library */
    Symbol          m_symbol;           /**< the updated symbol */
    Symbol          m_originalSymbol;   /**< original symbol to be restored on undo */
};


/**
 * @brief Import library command class.
 *
 * Implement importing the symbols from one library into the current library. The indexes
 * from the import library are ignored and new indexes are generated by the current library.
 *
 * The list of generated indexes is stored for a possible undo.
 */
class ImportLibraryCommand : public QUndoCommand
{
public:
    ImportLibraryCommand(SymbolLibrary *library, SymbolLibrary *imported);
    virtual ~ImportLibraryCommand();

    virtual void undo();
    virtual void redo();

private:
    SymbolLibrary   *m_symbolLibrary;   /**< pointer to the symbol library */
    SymbolLibrary   *m_imported;        /**< pointer to the imported library */
    QList<qint16>   m_addedIndexes;     /**< indexes of the symbols imported to be removed on undo */
};


/**
 * @brief Rotate left command class.
 *
 * Implement the rotation of the QPainterPath for the current symbol left (counter clockwise).
 *
 * Nothing is stored for undo as this is implemented as a rotate right.
 */
class RotateLeftCommand : public QUndoCommand
{
public:
    explicit RotateLeftCommand(Editor *editor);
    ~RotateLeftCommand();

    virtual void undo();
    virtual void redo();

private:
    Editor  *m_editor;                  /**< pointer to the editor */
};


/**
 * @brief Rotate right command class.
 *
 * Implement the rotation of the QPainterPath for the current symbol right (clockwise).
 *
 * Nothing is stored for undo as this is implemented as a rotate left.
 */
class RotateRightCommand : public QUndoCommand
{
public:
    explicit RotateRightCommand(Editor *editor);
    ~RotateRightCommand();

    virtual void undo();
    virtual void redo();

private:
    Editor  *m_editor;                  /**< pointer to the editor */
};


/**
 * @brief Flip horizontal command class.
 *
 * Implement the flipping of the QPainterPath for the current symbol horizontally about
 * the vertical axis passing through the center line of the symbol.
 *
 * Nothing is stored for undo as this implemented by another flip horizontal.
 */
class FlipHorizontalCommand : public QUndoCommand
{
public:
    explicit FlipHorizontalCommand(Editor *editor);
    ~FlipHorizontalCommand();

    virtual void undo();
    virtual void redo();

private:
    Editor  *m_editor;                  /**< pointer to the editor */
};


/**
 * @brief Flip vertical command class.
 *
 * Implement the flipping of the QPainterPath for the current symbol vertically about
 * the horizontal axis passing through the center line of the symbol.
 *
 * Nothing is stored for undo as this is implemented by another flip vertical.
 */
class FlipVerticalCommand : public QUndoCommand
{
public:
    explicit FlipVerticalCommand(Editor *editor);
    ~FlipVerticalCommand();

    virtual void undo();
    virtual void redo();

private:
    Editor  *m_editor;                  /**< pointer to the editor */
};


/**
 * @brief Scale to preferred size class.
 *
 * Implement scaling of the current symbol so that it fits within the preferred size rectangle.
 * No changes are made to symbols that are already inside it.
 */
class ScalePreferredCommand : public QUndoCommand
{
public:
    ScalePreferredCommand(Editor *editor, const QPainterPath &originlSymbol, int gridElements, int borderSize);
    ~ScalePreferredCommand();

    virtual void undo();
    virtual void redo();

private:
    Editor          *m_editor;          /**< pointer to the editor */
    QPainterPath    m_originalSymbol;   /**< original symbol path used for undo */
    int             m_gridElements;     /**< the number of elements in a grid side */
    int             m_borderSize;       /**< the number of elements in a border */
};


/**
 * @brief Change the fill state command class.
 *
 * Implement changing the fill state of the current symbol.
 *
 * The original state is stored for a possible undo.
 */
class ChangeFilledCommand : public QUndoCommand
{
public:
    ChangeFilledCommand(Editor *editor, bool from, bool to);
    virtual ~ChangeFilledCommand();

    virtual void undo();
    virtual void redo();

private:
    Editor  *m_editor;                  /**< pointer to the editor */
    bool    m_from;                     /**< original setting */
    bool    m_to;                       /**< new setting */
};


/**
 * @brief Change the fill rule command class.
 *
 * Implement changing the fill rule of the QPainterPath for the current symbol.
 *
 * The original fill rule is stored for a possible undo.
 */
class ChangeFillRuleCommand : public QUndoCommand
{
public:
    ChangeFillRuleCommand(Editor *editor, Qt::FillRule from, Qt::FillRule to);
    virtual ~ChangeFillRuleCommand();

    virtual void undo();
    virtual void redo();

private:
    Editor          *m_editor;          /**< pointer to the Editor */
    Qt::FillRule    m_from;             /**< original setting */
    Qt::FillRule    m_to;               /**< new setting */
};


/**
 * @brief Change the pen cap style command.
 *
 * Implement changing the line cap style for the current symbol.
 *
 * The original style is stored for a possible undo.
 */
class ChangeCapStyleCommand : public QUndoCommand
{
public:
    ChangeCapStyleCommand(Editor *editor, Qt::PenCapStyle from, Qt::PenCapStyle to);
    virtual ~ChangeCapStyleCommand();

    virtual void undo();
    virtual void redo();

private:
    Editor          *m_editor;          /**< pointer to the Editor */
    Qt::PenCapStyle m_from;             /**< original setting */
    Qt::PenCapStyle m_to;               /**< new setting */
};


/**
 * @brief Change the pen join style command.
 *
 * Implement changing the line join style for the current symbol.
 *
 * The original style is stored for a possible undo.
 */
class ChangeJoinStyleCommand : public QUndoCommand
{
public:
    ChangeJoinStyleCommand(Editor *editor, Qt::PenJoinStyle from, Qt::PenJoinStyle to);
    virtual ~ChangeJoinStyleCommand();

    virtual void undo();
    virtual void redo();

private:
    Editor              *m_editor;      /**< pointer to the Editor */
    Qt::PenJoinStyle    m_from;         /**< original setting */
    Qt::PenJoinStyle    m_to;           /**< new setting */
};


/**
 * @brief Delete a symbol from the library command.
 *
 * Implement deleting a symbol from the library using the index specified.
 *
 * The index and the symbol removed are stored for a possible undo.
 */
class DeleteSymbolCommand : public QUndoCommand
{
public:
    DeleteSymbolCommand(SymbolLibrary *library, qint16 index);
    virtual ~DeleteSymbolCommand();

    virtual void undo();
    virtual void redo();

private:
    SymbolLibrary   *m_symbolLibrary;   /**< pointer to the SymbolLibrary */
    qint16          m_index;            /**< index of the symbol */
    Symbol          m_symbol;           /**< the symbol deleted, stored for undo */
};


/**
 * @brief Increase the line width of the path.
 *
 * Implement increasing the line width for the current symbol. Multiple increases
 * of the width are merged together to allow for a single undo or redo operation.
 *
 * The original width is stored for a possible undo.
 */
class IncreaseLineWidthCommand : public QUndoCommand
{
public:
    IncreaseLineWidthCommand(Editor *editor, qreal from, qreal to);
    virtual ~IncreaseLineWidthCommand();

    virtual void redo();
    virtual void undo();

    virtual int id() const;
    virtual bool mergeWith(const QUndoCommand *command);

private:
    Editor      *m_editor;              /**< pointer to the Editor */
    qreal       m_from;                 /**< the original setting */
    qreal       m_to;                   /**< the new setting, this may be changed by merging commands */
};


/**
 * @brief Decrease the line width of the path.
 *
 * Implement decreasing the line width for the current symbol. Multiple decreases
 * of the width are merged together to allow for a single undo or redo operation.
 *
 * The original width is stored for a possible undo.
 */
class DecreaseLineWidthCommand : public QUndoCommand
{
public:
    DecreaseLineWidthCommand(Editor *editor, qreal from, qreal to);
    virtual ~DecreaseLineWidthCommand();

    virtual void redo();
    virtual void undo();

    virtual int id() const;
    virtual bool mergeWith(const QUndoCommand *command);

private:
    Editor      *m_editor;              /**< pointer to the Editor */
    qreal       m_from;                 /**< the original setting */
    qreal       m_to;                   /**< the new setting, this may be changed by merging commands */
};


/**
 * @brief Add new symbols dragged from another instance of the SymbolEditor.
 *
 * Allows copying of symbols from one symbol library to another using drag and drop.
 */
class DragAndDropCommand : public QUndoCommand
{
public:
    DragAndDropCommand(SymbolLibrary *library, const QMimeData *mimeData);

    virtual void redo();
    virtual void undo();

private:
    SymbolLibrary   *m_library;
    QList<Symbol>   m_symbols;
    QList<qint16>   m_indexes;
};


/**
 * @brief Add a fonted character as a symbol.
 *
 * Allows a character to be added after selection from the KCharSelect dialog.
 */
class AddCharacterCommand : public QUndoCommand
{
public:
    AddCharacterCommand(Editor *editor, const QPainterPath &path);

    virtual void redo();
    virtual void undo();

private:
    Editor          *m_editor;
    QPainterPath    m_path;
};


#endif

