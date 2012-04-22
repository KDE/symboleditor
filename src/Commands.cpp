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
 * Implement the various QUndoCommand based commands that modify the editor
 * or the symbol library.
 */


/**
 * @page command Commands
 * A series of QUndoCommand derived commands are used to make modifications to the
 * editor and the symbol library.  These commands are pushed onto the relevant undo
 * stacks associated with the editor or the symbol library causing them to be
 * executed. This allows undo and redo functionality to be implemented.
 *
 * All functionality, including updating the views are encapsulated within the commands
 * undo and redo functions and the functions called by them.
 */


#include "Commands.h"

#include <QPainterPath>

#include <KLocale>

#include "Editor.h"
#include "SymbolLibrary.h"


/**
 * Constructor
 *
 * @param editor a pointer to the Editor
 * @param to a const reference to a QPointF representing the point to move to
 */
MoveToCommand::MoveToCommand(Editor *editor, const QPointF &to)
    :   QUndoCommand(i18n("Move to")),
        m_editor(editor),
        m_to(to)
{
}


/**
 * Destructor
 */
MoveToCommand::~MoveToCommand()
{
}


/**
 * Undo the move to command. Restores the path saved during redo.
 */
void MoveToCommand::undo()
{
    m_editor->removeLast(m_path);
}


/**
 * Redo the move command. Call the Editor moveTo function with the m_to point
 * returning the QPainterPath existing before the moveTo which is saved for undo.
 */
void MoveToCommand::redo()
{
    m_path = m_editor->moveTo(m_to);
}


/**
 * Constructor
 *
 * @param editor a pointer to the Editor
 * @param to a const reference to a QPointF representing the point to draw to
 */
LineToCommand::LineToCommand(Editor *editor, const QPointF &to)
    :   QUndoCommand(i18n("Line to")),
        m_editor(editor),
        m_to(to)
{
}


/**
 * Destructor
 */
LineToCommand::~LineToCommand()
{
}


/**
 * Undo the line command. Restore the path saved during redo.
 */
void LineToCommand::undo()
{
    m_editor->removeLast(m_path);
}


/**
 * Redo the line command. Call the Editor lineTo function with the m_to point
 * returning the QPainterPath existing before the lineTo which is saved for undo.
 */
void LineToCommand::redo()
{
    m_path = m_editor->lineTo(m_to);
}


/**
 * Constructor
 *
 * @param editor a pointer to the Editor
 * @param control1 a const reference to a QPointF representing the first control point
 * @param control2 a const reference to a QPointF representing the second control point
 * @param to a const reference to a QPointF representing the end point of the curve
 */
CubicToCommand::CubicToCommand(Editor *editor, const QPointF &control1, const QPointF &control2, const QPointF &to)
    :   QUndoCommand(i18n("Cubic to")),
        m_editor(editor),
        m_control1(control1),
        m_control2(control2),
        m_to(to)
{
}


/**
 * Destructor
 */
CubicToCommand::~CubicToCommand()
{
}


/**
 * Undo the cubic command. Restore the path saved during redo.
 */
void CubicToCommand::undo()
{
    m_editor->removeLast(m_path);
}


/**
 * Redo the cubic command. Call the Editor cubicTo function with the three points
 * returning the QPainterPath existing before the cubicTo which is saved for undo.
 */
void CubicToCommand::redo()
{
    m_path = m_editor->cubicTo(m_control1, m_control2, m_to);
}


/**
 * Constructor
 *
 * @param editor a pointer to the Editor
 * @param from a const reference to a QPointF representing the first corner
 * @param to a const reference to a QPointF representing the second corner
 */
RectangleCommand::RectangleCommand(Editor *editor, const QPointF &from, const QPointF &to)
    :   QUndoCommand(i18n("Add Rectangle")),
        m_editor(editor),
        m_from(from),
        m_to(to)
{
}


/**
 * Destructor
 */
RectangleCommand::~RectangleCommand()
{
}


/**
 * Undo the rectangle command. Restore the path saved during redo.
 */
void RectangleCommand::undo()
{
    m_editor->removeLast(m_path);
}


/**
 * Redo the rectangle command. Call the Editor addRectangle function with the two points
 * returning the QPainterPath existing before the addRectangle which is saved for undo.
 */
void RectangleCommand::redo()
{
    m_path = m_editor->addRectangle(m_from, m_to);
}


/**
 * Constructor
 *
 * @param editor a pointer to the Editor
 * @param from a const reference to a QPointF representing the first corner
 * @param to a const reference to a QPointF representing the second corner
 */
EllipseCommand::EllipseCommand(Editor *editor, const QPointF &from, const QPointF &to)
    :   QUndoCommand(i18n("Add Ellipse")),
        m_editor(editor),
        m_from(from),
        m_to(to)
{
}


/**
 * Destructor
 */
EllipseCommand::~EllipseCommand()
{
}


/**
 * Undo the ellipse command. Restore the path saved during redo.
 */
void EllipseCommand::undo()
{
    m_editor->removeLast(m_path);
}


/**
 * Redo the ellipse command. Call the Editor addEllipse function with the two points
 * returning the QPainterPath existing before the addEllipse which is saved for undo.
 */
void EllipseCommand::redo()
{
    m_path = m_editor->addEllipse(m_from, m_to);
}


/**
 * Constructor
 *
 * @param editor a pointer to the Editor
 * @param index the index of the point in the Editor::m_points list
 * @param from a const reference to a QPointF representing the original position
 * @param to a const reference to a QPointF representing the new position
 */
MovePointCommand::MovePointCommand(Editor *editor, int index, const QPointF &from, const QPointF &to)
    :   QUndoCommand(i18n("Move point")),
        m_editor(editor),
        m_index(index),
        m_from(from),
        m_to(to)
{
}


/**
 * Destructor
 */
MovePointCommand::~MovePointCommand()
{
}


/**
 * Undo the move point command. Call the Editor::movePoint function to move the
 * indexed point to its original from position.
 */
void MovePointCommand::undo()
{
    m_editor->movePoint(m_index, m_from);
}


/**
 * Redo the move point command. Call the Editor::movePoint function to move the
 * indexed point to the to position.
 */
void MovePointCommand::redo()
{
    m_editor->movePoint(m_index, m_to);
}


/**
 * Constructor
 *
 * @param library a pointer to the SymbolLibrary
 * @param index the index of the symbol to be updated
 * @param path a const reference to a QPainterPath representing the updated symbol
 */
UpdateSymbolCommand::UpdateSymbolCommand(SymbolLibrary *library, qint16 index, const QPainterPath &path)
    :   QUndoCommand(),
        m_symbolLibrary(library),
        m_index(index),
        m_path(path)
{
}


/**
 * Destructor
 */
UpdateSymbolCommand::~UpdateSymbolCommand()
{
}


/**
 * Undo the update symbol command. If the original path was not empty it is restored
 * to the indexed symbol otherwise the indexes symbol is removed from the library.
 */
void UpdateSymbolCommand::undo()
{
    if (m_originalPath.isEmpty())
    {
        m_symbolLibrary->takeSymbol(m_index);
        m_index = 0;
    }
    else
        m_symbolLibrary->setSymbol(m_index, m_originalPath);
}


/**
 * Redo the update symbol command. The original QPainterPath is saved for undo. This may be empty.
 * The new QPainterPath is set in the library for the given index. If the index is 0 a new index
 * is generated, returned and saved for undo.
 */
void UpdateSymbolCommand::redo()
{
    m_originalPath = m_symbolLibrary->symbol(m_index);
    m_index = m_symbolLibrary->setSymbol(m_index, m_path);
}


/**
 * Constructor
 *
 * @param library a pointer to the SymbolLibrary
 * @param imported a pointer to the imported SymbolLibrary
 */
ImportLibraryCommand::ImportLibraryCommand(SymbolLibrary *library, SymbolLibrary *imported)
    :   QUndoCommand(i18n("Import Library")),
        m_symbolLibrary(library),
        m_imported(imported)
{
}


/**
 * Destructor
 * The imported library is deleted as this class has taken ownership of it.
 */
ImportLibraryCommand::~ImportLibraryCommand()
{
    delete m_imported;
}


/**
 * Undo the import library command. All symbols that were added are removed
 * from the library. The list of added indexes is cleared.
 */
void ImportLibraryCommand::undo()
{
    foreach (qint16 i, m_addedIndexes)
        m_symbolLibrary->takeSymbol(i);
    m_addedIndexes.clear();
}


/**
 * Redo the import library command. Each symbol in the imported library are added to the current
 * library creating new indexes which are added to the m_addedIndexes list for undo.
 */
void ImportLibraryCommand::redo()
{
    foreach (qint16 i, m_imported->indexes())
    {
        QPainterPath path = m_imported->symbol(i);
        m_addedIndexes.append(m_symbolLibrary->setSymbol(0, path)); // gets a new index
    }
}


/**
 * Constructor
 *
 * @param editor a pointer to the Editor
 */
RotateLeftCommand::RotateLeftCommand(Editor *editor)
    :   QUndoCommand(i18n("Rotate Left")),
        m_editor(editor)
{
}


/**
 * Destructor
 */
RotateLeftCommand::~RotateLeftCommand()
{
}


/**
 * Undo the rotate left command. Call the Editor::rotatePointRight function to reverse
 * the rotate left.
 */
void RotateLeftCommand::undo()
{
    m_editor->rotatePointsRight();
}


/**
 * Redo the rotate left command. Call the Editor::rotatePointsLeft function to rotate the points.
 */
void RotateLeftCommand::redo()
{
    m_editor->rotatePointsLeft();
}


/**
 * Constructor
 *
 * @param editor a pointer to the Editor
 */
RotateRightCommand::RotateRightCommand(Editor *editor)
    :   QUndoCommand(i18n("Rotate Right")),
        m_editor(editor)
{
}


/**
 * Destructor
 */
RotateRightCommand::~RotateRightCommand()
{
}


/**
 * Undo the rotate right command. Call the Editor::rotatePointsLeft function to reverse
 * the rotate right.
 */
void RotateRightCommand::undo()
{
    m_editor->rotatePointsLeft();
}


/**
 * Redo the rotate right command. Call the Editor::rotatePointsRight function to rotate the points.
 */
void RotateRightCommand::redo()
{
    m_editor->rotatePointsRight();
}


/**
 * Constructor
 *
 * @param editor a pointer to the Editor
 */
FlipHorizontalCommand::FlipHorizontalCommand(Editor *editor)
    :   QUndoCommand(i18n("Flip Horizontally")),
        m_editor(editor)
{
}


/**
 * Destructor
 */
FlipHorizontalCommand::~FlipHorizontalCommand()
{
}


/**
 * Undo the flip horizontal command. Call the Editor::flipPointsHorizontal to flip the points.
 */
void FlipHorizontalCommand::undo()
{
    m_editor->flipPointsHorizontal();
}


/**
 * Redo the flip horizontal command. Call the Editor::flipPointsHorizontal to flip the points.
 */
void FlipHorizontalCommand::redo()
{
    m_editor->flipPointsHorizontal();
}


/**
 * Constructor
 *
 * @param editor a pointer to the Editor
 */
FlipVerticalCommand::FlipVerticalCommand(Editor *editor)
    :   QUndoCommand(i18n("Flip Vertical")),
        m_editor(editor)
{
}


/**
 * Destructor
 */
FlipVerticalCommand::~FlipVerticalCommand()
{
}


/**
 * Undo the flip vertical command. Call the Editor::flipPointsVertical to flip the points.
 */
void FlipVerticalCommand::undo()
{
    m_editor->flipPointsVertical();
}


/**
 * Redo the flip vertical command. Call the Editor::flipPointsVertical to flip the points.
 */
void FlipVerticalCommand::redo()
{
    m_editor->flipPointsVertical();
}
