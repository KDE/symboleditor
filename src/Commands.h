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
 */
class UpdateSymbolCommand : public QUndoCommand
{
public:
    UpdateSymbolCommand(SymbolLibrary *library, qint16 index, const QPainterPath &path);
    virtual ~UpdateSymbolCommand();

    virtual void undo();
    virtual void redo();

private:
    SymbolLibrary   *m_symbolLibrary;   /**< pointer to the symbol library */
    qint16          m_index;            /**< index of the symbol in the library */
    QPainterPath    m_path;             /**< path of the updated symbol */
    QPainterPath    m_originalPath;     /**< original path to be restored on undo */
};


/**
 * @brief Import library command class.
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
 */
class RotateLeftCommand : public QUndoCommand
{
public:
    RotateLeftCommand(Editor *editor);
    ~RotateLeftCommand();

    virtual void undo();
    virtual void redo();

private:
    Editor  *m_editor;                  /**< pointer to the editor */
};


/**
 * @brief Rotate right command class.
 */
class RotateRightCommand : public QUndoCommand
{
public:
    RotateRightCommand(Editor *editor);
    ~RotateRightCommand();

    virtual void undo();
    virtual void redo();

private:
    Editor  *m_editor;                  /**< pointer to the editor */
};


/**
 * @brief Flip horizontal command class.
 */
class FlipHorizontalCommand : public QUndoCommand
{
public:
    FlipHorizontalCommand(Editor *editor);
    ~FlipHorizontalCommand();

    virtual void undo();
    virtual void redo();

private:
    Editor  *m_editor;                  /**< pointer to the editor */
};


/**
 * @brief Flip vertical command class.
 */
class FlipVerticalCommand : public QUndoCommand
{
public:
    FlipVerticalCommand(Editor *editor);
    ~FlipVerticalCommand();

    virtual void undo();
    virtual void redo();

private:
    Editor  *m_editor;                  /**< pointer to the editor */
};


#endif

