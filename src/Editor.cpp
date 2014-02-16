/********************************************************************************
 * Copyright (C) 2011 by Stephen Allewell                                       *
 * sallewell@users.sourceforge.net                                              *
 *                                                                              *
 * This program is free software; you can redistribute it and/or modify         *
 * it under the terms of the GNU General Public License as published by         *
 * the Free Software Foundation; either version 2 of the License, or            *
 * (at your option) any later version.                                          *
 ********************************************************************************/


/**
 * @file
 * Implement the Editor class.
 */


/**
 * @page editor_window Editor
 * The editor allows the user to create or modify the current symbol. It consists of a fixed size square grid
 * with several tools to create QPainterPath objects which will then be used to render the symbol. The QPainterPath
 * consists of one or more subpaths. Each subpath has a start point, which defaults to (0,0), but can be moved as
 * required. The various tools require one or more additional points to define the subpath. For each of the
 * tools used,the last point of the last tool becomes the first point of the next. Consequently all the individual
 * tools are joined together to create a single subpath. A new subpath can be started by using the move to option
 * to specify a new start point. Additionally the use of the rectangle and ellipse tools will create stand alone subpaths.
 *
 * The editor shows a green square which is the largest preferred size of the symbol. This allows some white space
 * around the symbol when it will be rendered in a pattern.
 *
 * When placing points, guide lines will be shown when the cursor is at a position that aligns with other
 * points either horizontally, vertically or at a 45 degree angle. The circle guides are created if the cursor
 * is at a position that aligns with another point on a circle where the center is at the center of the grid.
 *
 * @section editor_tools Editor Tools
 *
 * @subsection move_to Move To
 * Allows moving the start point of the current subpath. If the current subpath already has elements in
 * it, this effectively closes the current subpath and creates a new one.
 *
 * @subsection line_to Line To
 * Draws a line from the last point to the next selected point.
 *
 * @subsection cubic_to Cubic To
 * Draws a cubic spline consisting of a start point which will be the last point of the last command, two
 * control points and a finish point. The control points define the path of the curve between the start and end points.
 *
 * @subsection rectangle Rectangle
 * Draw a rectangle between two points. The points define the opposite corners of the rectangle. These can either
 * entered by clicking two points or clicking and dragging. When dragging, a rubber band rectangle will be shown
 * indicating the rectangle.
 *
 * @subsection ellipse Ellipse
 * Draw an ellipse between two points. The points define the opposite corners of a rectangle encompassing the
 * ellipse. As with the rectangle the points can be selected by clicking two points or clicking and dragging.
 * When dragging, a rubber band ellipse will be shown indicating the ellipse.
 *
 * @subsection rotate_left Rotate Left
 * Rotate the symbol counter clockwise 90 degrees.
 *
 * @subsection rotate_right Rotate Right
 * Rotate the symbol clockwise 90 degrees.
 *
 * @subsection flip_horizontal Flip Horizontally
 * Flip the symbol horizontally about an axis passing vertically through the center of the grid.
 *
 * @subsection flip_vertical Flip Vertically
 * Flip the symbol vertically about an axis passing horizontally through the center of the grid.
 *
 * @subsection snap_grid Snap to Grid
 * The selection of points can either be snapped to the grid or can be freely placed depending on if
 * the snap option is toggled on or off.
 *
 * @section path_rendering Path Rendering
 *
 * @subsection fill_mode Fill Mode
 * The defined path can either be set to filled or outline depending on if the fill mode is toggled
 * on or off.
 *
 * @subsection fill_rule Fill Rule
 * When filling is enabled the path will be filled. The rule defining the fill will be either Winding  of
 * Odd-Even.
 *
 * @par Winding
 * The winding rule will completely fill the objects boundary.
 *
 * @par Odd Even
 * The odd even rule will fill produce a checker board style fill where adjacent areas will be different
 * to each other.
 *
 * @subsection line_cap Line End Cap Style
 * The start and end of a path line can have different visual appearance depending on the following
 * attributes.
 *
 * @par Flat Cap
 * The line is stopped at the end point and the line end is cut square to the direction.
 *
 * @par Square Cap
 * The line is projected beyond the end point by half the line width and the line end is cut square to
 * the direction.
 *
 * @par Round Cap
 * The line is projected beyond the end point by half the line width and the line end is rounded.
 *
 * @subsection line_join Line Join Style
 * Where two lines join, the way the join is shown is determined by the join style as follows.
 *
 * @par Bevel Join
 * The lines are beveled at the intersection.
 *
 * @par Miter Join
 * The lines are extended and mitered to provide a corner.
 *
 * @par Round Join
 * The lines are joined with a rounded corner.
 *
 * @subsection line_width Line Width
 * The line width can be increased and decreased. The width is rendered equally about the line between
 * the two points. This attribute is mostly relevant to unfilled shapes.
 *
 * @section points Points
 * The points can either be snapped to the grid intersections or placed freely depending on the snap option.
 * All points can be moved by hovering over them until the cursor changes to a move cursor, then hold down
 * the left mouse button and drag the point to its new location.
 *
 * @section editing_symbols Editing Symbols
 * Editing a symbol in the library can be done by double clicking the symbol in the library view. This will
 * transfer the symbol into the editor allowing it to be changed and saved. In this scenario there is a link
 * between the symbol being edited and the library, so saving does update the correct symbol. If you wish to
 * use this symbol as a basis for other symbols using rotation and flipping, use the File->Save Symbol as New
 * command which will save the symbol to the library as a new one. Further changes to the symbol which are
 * saved will also become new symbols.
 */


#include "Editor.h"

#include <QAction>
#include <QMouseEvent>
#include <QPainter>

#include <math.h>


const int gridElements = 32;                    /**< The number of elements in the grid */
const int elementSize = 16;                     /**< The size in pixels of an element */
const int elementGroup = 8;                     /**< The number of elements in a group */
const double pointSize = 9;                     /**< The size of a point */
const double threshold = 0.02;                  /**< The range to check for point closeness */
const int pointsRequired[] = {1, 1, 3, 2, 2};   /**< The number of points required for commands from Editor::ToolMode */


/**
 * Construct the Editor.
 * The editor is a sub class of a QWidget and is added to a layout widget.
 * It is resized to accommodate a gridElements number of cells of width elementSize. An additional
 * 1 is added to allow for a right and bottom edge to be drawn.
 * The boundary edges of the symbol editor are defined in the range 0..1. These are used to intersect
 * the guidelines which are shown at the m_angles relative to existing points
 *
 * @param parent a pointer to the parent widget, this is passed to the base class object
 */
Editor::Editor(QWidget *parent)
    :   QWidget(parent),
        m_size(gridElements*elementSize),
        m_index(0)
{
    resize(m_size + 1, m_size + 1);
    setMinimumSize(size());

    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_topEdge = QLineF(0.0, 0.0, 1.0, 0.0);
    m_bottomEdge = QLineF(0.0, 1.0, 1.0, 1.0);
    m_leftEdge = QLineF(0.0, 0.0, 0.0, 1.0);
    m_rightEdge = QLineF(1.0, 0, 1.0, 1.0);

    m_angles << 0 << 45 << 90 << 135 << 180 << 225 << 270 << 315;
}


/**
 * Destructor for the Editor.
 * No additional destruction is required here.
 */
Editor::~Editor()
{
}


/**
 * Get the index and Symbol.
 * Update the Symbol with the path being edited.
 *
 * @return the edited Symbol and it's index in a QPair
 */
QPair<qint16, Symbol> Editor::symbol()
{
    m_symbol.setPath(m_painterPath);
    return QPair<qint16, Symbol>(m_index, m_symbol);
}


/**
 * Set the symbol and index to be edited.
 * The undo stack will be cleared.
 *
 * @param pair a QPair consisting of the index and the Symbol
 */
void Editor::setSymbol(const QPair<qint16, Symbol> &pair)
{
    m_undoStack.clear();
    m_index = pair.first;
    m_symbol = pair.second;
    m_painterPath = m_symbol.path();
    deconstructPainterPath();
    update();
}


/**
 * Move the start of the sub path.
 * The point is in symbol coordinates.
 * Returns the current painterPath for later undo.
 *
 * @param to a const reference to a QPointF representing the new start position
 *
 * @return a QPainterPath representing the original path
 */
QPainterPath Editor::moveTo(const QPointF &to)
{
    QPainterPath path = m_painterPath;
    m_painterPath.moveTo(to);
    deconstructPainterPath();
    update();
    return path;
}


/**
 * Add a line to the sub path.
 * The point is in symbol coordinates.
 * Returns the current painterPath for later undo.
 *
 * @param to a const reference to a QPointF representing the end point of the line
 *
 * @return a QPainterPath representing the original path
 */
QPainterPath Editor::lineTo(const QPointF &to)
{
    QPainterPath path = m_painterPath;
    m_painterPath.lineTo(to);
    deconstructPainterPath();
    update();
    return path;
}


/**
 * Add a cubic spline to the sub path.
 * The points are in symbol coordinates.
 * Returns the current painterPath for later undo.
 *
 * @param control1 a const reference to a QPointF representing the first control point
 * @param control2 a const reference to a QPointF representing the second control point
 * @param to a const reference to a QPointF representing the end point of the curve
 *
 * @return a QPainterPath representing the original path
 */
QPainterPath Editor::cubicTo(const QPointF &control1, const QPointF &control2, const QPointF &to)
{
    QPainterPath path = m_painterPath;
    m_painterPath.cubicTo(control1, control2, to);
    deconstructPainterPath();
    update();
    return path;
}


/**
 * Add a rectangle as a new sub path.
 * The points are in symbol coordinates.
 * Returns the current painterPath for later undo.
 *
 * @param from a const reference to a QPointF representing the first corner
 * @param to a const reference to a QPointF representing the second corner
 *
 * @return a QPainterPath representing the original path
 */
QPainterPath Editor::addRectangle(const QPointF &from, const QPointF &to)
{
    QPainterPath path = m_painterPath;
    m_painterPath.addRect(QRectF(from, to));
    deconstructPainterPath();
    update();
    return path;
}


/**
 * Add an ellipse as a new sub path.
 * The points are in symbol coordinates.
 * Returns the current painterPath for later undo.
 *
 * @param from a const reference to a QPointF representing the first corner
 * @param to a const reference to a QPointF representing the second corner
 *
 * @return a QPainterPath representing the original path
 */
QPainterPath Editor::addEllipse(const QPointF &from, const QPointF &to)
{
    QPainterPath path = m_painterPath;
    m_painterPath.addEllipse(QRectF(from, to));
    deconstructPainterPath();
    update();
    return path;
}


/**
 * Remove the last added element by restoring the path that existed before the last command.
 *
 * @param path a const reference to a QPainterPath being restored
 */
void Editor::removeLast(const QPainterPath& path)
{
    m_painterPath = path;
    deconstructPainterPath();
    update();
}


/**
 * Move a point node to a new position.
 *
 * @param index the index of the point to move
 * @param to a const reference to a QPointF representing the new position
 */
void Editor::movePoint(int index, const QPointF &to)
{
    m_points[index] = to;
    constructPainterPath();
    update();
}


/**
 * Rotate all the points around the center of the symbol 90 degrees counter clockwise.
 */
void Editor::rotatePointsLeft()
{
    for (int i = 0 ; i < m_points.count() ; ++i) {
        QPointF p = m_points[i];
        m_points[i] = QPointF(p.y(), 1.0 - p.x());
    }

    for (int i = 0 ; i < m_activePoints.count() ; ++i) {
        QPointF p = m_activePoints[i];
        m_activePoints[i] = QPointF(p.y(), 1.0 - p.x());
    }

    constructPainterPath();
    update();
}


/**
 * Rotate all the points around the center of the symbol 90 degrees clockwise
 */
void Editor::rotatePointsRight()
{
    for (int i = 0 ; i < m_points.count() ; ++i) {
        QPointF p = m_points[i];
        m_points[i] = QPointF(1.0 - p.y(), p.x());
    }

    for (int i = 0 ; i < m_activePoints.count() ; ++i) {
        QPointF p = m_activePoints[i];
        m_activePoints[i] = QPointF(1.0 - p.y(), p.x());
    }

    constructPainterPath();
    update();
}


/**
 * Flip all the points about a vertical axis passing through the symbol center.
 */
void Editor::flipPointsHorizontal()
{
    for (int i = 0 ; i < m_points.count() ; ++i) {
        QPointF p = m_points[i];
        p.setX(1.0 - p.x());
        m_points[i] = p;
    }

    for (int i = 0 ; i < m_activePoints.count() ; ++i) {
        QPointF p = m_activePoints[i];
        p.setX(1.0 - p.x());
        m_activePoints[i] = p;
    }

    constructPainterPath();
    update();
}


/**
 * Flip all the points about a horizontal axis passing through the symbol center.
 */
void Editor::flipPointsVertical()
{
    for (int i = 0 ; i < m_points.count() ; ++i) {
        QPointF p = m_points[i];
        p.setY(1.0 - p.y());
        m_points[i] = p;
    }

    for (int i = 0 ; i < m_activePoints.count() ; ++i) {
        QPointF p = m_activePoints[i];
        p.setY(1.0 - p.y());
        m_activePoints[i] = p;
    }

    constructPainterPath();
    update();
}


/**
 * Set the fill state of the symbols path.
 *
 * @param filled @c true if the path is to be filled, @c false for outline paths
 */
void Editor::setFilled(bool filled)
{
    m_symbol.setFilled(filled);
    update();
}


/**
 * Set the fill rule of the symbols path.
 *
 * @param rule a Qt::FillRule value, see the QPainterPath documentation for details
 */
void Editor::setFillRule(Qt::FillRule rule)
{
    m_painterPath.setFillRule(rule);
    update();
}


/**
 * Set the pen cap style, see the QPen documentation for details on the styles.
 *
 * @param capStyle a Qt::PenCapStyle value
 */
void Editor::setCapStyle(Qt::PenCapStyle capStyle)
{
    m_symbol.setCapStyle(capStyle);
    update();
}


/**
 * Set the pen join style, see the QPen documentation for details on the styles.
 *
 * @param joinStyle a Qt::PenJoinStyle value
 */
void Editor::setJoinStyle(Qt::PenJoinStyle joinStyle)
{
    m_symbol.setJoinStyle(joinStyle);
    update();
}


/**
 * Set the line width.
 * The width parameter is rounded to 2 decimal places to avoid errors in the comparisons.
 * Signals are emitted to enable or disable the relevant actions depending on the values.
 *
 * @param width the new line width
 */
void Editor::setLineWidth(double width)
{
    width = round(width * 100) / 100;
    m_symbol.setLineWidth(width);
    emit minLineWidth(width == 0.01);
    emit maxLineWidth(width == 1.00);
    update();
}


/**
 * Clear the editor to represent a blank symbol.
 * Delete the existing data and update the display.
 */
void Editor::clear()
{
    m_undoStack.clear();
    m_points.clear();
    m_activePoints.clear();
    m_elements.clear();
    m_index = 0;
    m_symbol = Symbol();
    m_painterPath = m_symbol.path();
    update();
}


/**
 * Get a pointer to the undo stack.
 *
 * @return a pointer to the QUndoStack
 */
QUndoStack *Editor::undoStack()
{
    return &m_undoStack;
}


/**
 * Select the next tool.
 * Remove any points that may have been used in the last tool command but not yet committed.
 * Set the tool mode from the sending actions data.
 *
 * @param action a pointer to the action triggering the selection
 */
void Editor::selectTool(QAction *action)
{
    m_activePoints.clear();
    m_toolMode = static_cast<Editor::ToolMode>(action->data().toInt());
}


/**
 * Switch the snap mode on or off.
 *
 * @param enabled true if on, false otherwise
 */
void Editor::enableSnap(bool enabled)
{
    m_snap = enabled;
}


/**
 * Switch the fill mode on or off.
 *
 * @param filled true if on, false otherwise
 */
void Editor::selectFilled(bool filled)
{
    m_undoStack.push(new ChangeFilledCommand(this, m_symbol.filled(), filled));
}


/**
 * Select the fill rule to be used when filling is enabled.
 * The specified rule is taken from the sending actions data.
 *
 * @param action a pointer to the action triggering the selection
 */
void Editor::selectFillRule(QAction *action)
{
    Qt::FillRule fillRule = static_cast<Qt::FillRule>(action->data().toInt());
    m_undoStack.push(new ChangeFillRuleCommand(this, m_symbol.path().fillRule(), fillRule));
}


/**
 * Select the pen cap style.
 * The specified value is taken from the sending actions data.
 *
 * @param action a pointer to the action that triggered the selection
 */
void Editor::selectCapStyle(QAction *action)
{
    Qt::PenCapStyle capStyle = static_cast<Qt::PenCapStyle>(action->data().toInt());
    m_undoStack.push(new ChangeCapStyleCommand(this, m_symbol.capStyle(), capStyle));
}


/**
 * Select the pen join style.
 * The specified value is taken from the sending actions data.
 *
 * @param action a pointer to the action that triggered the selection
 */
void Editor::selectJoinStyle(QAction *action)
{
    Qt::PenJoinStyle joinStyle = static_cast<Qt::PenJoinStyle>(action->data().toInt());
    m_undoStack.push(new ChangeJoinStyleCommand(this, m_symbol.joinStyle(), joinStyle));
}


/**
 * Increase the thickness of the path.
 */
void Editor::increaseLineWidth()
{
    m_undoStack.push(new IncreaseLineWidthCommand(this, m_symbol.lineWidth(), m_symbol.lineWidth() + 0.01));
}


/**
 * Decrease the thickness of the path.
 */
void Editor::decreaseLineWidth()
{
    m_undoStack.push(new DecreaseLineWidthCommand(this, m_symbol.lineWidth(), m_symbol.lineWidth() - 0.01));
}


/**
 * Rotate the symbol left (counter clock wise)
 */
void Editor::rotateLeft()
{
    m_undoStack.push(new RotateLeftCommand(this));
}


/**
 * Rotate the symbol right (clock wise)
 */
void Editor::rotateRight()
{
    m_undoStack.push(new RotateRightCommand(this));
}


/**
 * Flip the symbol horizontally about the vertical center line.
 */
void Editor::flipHorizontal()
{
    m_undoStack.push(new FlipHorizontalCommand(this));
}


/**
 * Flip the symbol vertically about the horizontal center line.
 */
void Editor::flipVertical()
{
    m_undoStack.push(new FlipVerticalCommand(this));
}


/**
 * Process the mouse press event. Effective for the left mouse button pressed.
 * Test if the cursor is over an existing point in which case dragging of the
 * point is enabled. Otherwise a point is added to the active points.
 *
 * @param event a pointer to a QMouseEvent
 */
void Editor::mousePressEvent(QMouseEvent *event)
{
    QPointF p = snapPoint(event->pos());

    if (event->buttons() & Qt::LeftButton) {
        m_start = m_tracking = p;
        m_rubberBand = QRectF();

        if (node(p)) {
            m_dragging = true;
            m_dragPointIndex = nodeUnderCursor(p);
        } else {
            addPoint(p);
        }
    }

    update();
}


/**
 * Process the mouse move event. Effective for the left mouse button pressed.
 * Clear any currently defined guides.
 * If dragging a point, update the points position and update the view, otherwise
 * if the toolmode is for a rectangle or an ellipse initialise the rubber band
 * rectange and update the view.
 * If the left button is not pressed, test if the cursor is over an existing point
 * and if it is change the cursor to a SizeAllCursor to indicate it can be moved,
 * otherwise reset it to an ArrowCursor.
 * Guide lines are constructed for each of the existing points to show if the new
 * position lies in line with or at a specific angle to or on a circular path to
 * that point.
 *
 * @param event a pointer to a QMouseEvent
 */
void Editor::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_guideLines.isEmpty()) {
        m_guideLines.clear();
        m_guideCircles.clear();
        update();
    }

    QPointF p = snapPoint(event->pos());

    if (event->buttons() & Qt::LeftButton) {
        if (m_tracking != p) {
            m_tracking = p;

            if (m_dragging) {
                if (m_dragPointIndex.first) {
                    m_points[m_dragPointIndex.second] = p;
                    constructPainterPath();
                } else {
                    m_activePoints[m_dragPointIndex.second] = p;
                }
            } else if (m_toolMode == Rectangle || m_toolMode == Ellipse) {
                m_rubberBand = QRectF(m_start, m_tracking).normalized();
            }

            update();
        }
    } else {
        if (node(p)) {
            setCursor(Qt::SizeAllCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
    }

    if (constructGuides(p)) {
        update();
    }
}


/**
 * Process the mouse release event.
 * If dragging a point that is in the m_points list and it hasn't been dragged back to
 * its start position create a MovePointCommand and push it onto the editor undo stack.
 * Pushing commands onto the stack will execute it, although the point is already in its
 * new position, this will have no adverse effects, but will record the move for possible
 * undo/redo later.
 * If not dragging a point, clear the rubber band rectangle and add a point if the new
 * point is not the same as the starting point.
 *
 * @param event a pointer to a QMouseEvent
 */
void Editor::mouseReleaseEvent(QMouseEvent *event)
{
    QPointF p = snapPoint(event->pos());

    if (m_dragging) {
        if ((p != m_start) && (m_dragPointIndex.first)) {
            m_undoStack.push(new MovePointCommand(this, m_dragPointIndex.second, m_start, p));
        }

        m_dragging = false;
    } else if (m_toolMode == Rectangle || m_toolMode == Ellipse) {
        m_rubberBand = QRectF();

        if (p != m_start) {
            addPoint(p);
        }
    }

    update();
}


/**
 * Add a point for the current tool.
 * Each tool requires a number of points to be defined to fully describe the shape the
 * tool represents. This function adds the points and keeps track of the number of points
 * required.  When the number of points required have been selected, the tool command can
 * be implemented and added to the symbol undo stack.
 * The points are in symbol coordinates in the range 0..1
 *
 * @param point a const reference to a QPointF
 */
void Editor::addPoint(const QPointF &point)
{
    m_activePoints.append(point);

    if (m_activePoints.count() == pointsRequired[m_toolMode]) {
        switch (m_toolMode) {
        case MoveTo:
            m_undoStack.push(new MoveToCommand(this, m_activePoints.at(0)));
            break;

        case LineTo:
            m_undoStack.push(new LineToCommand(this, m_activePoints.at(0)));
            break;

        case CubicTo:
            m_undoStack.push(new CubicToCommand(this, m_activePoints.at(0), m_activePoints.at(1), m_activePoints.at(2)));
            break;

        case Rectangle:
            m_undoStack.push(new RectangleCommand(this, m_activePoints.at(0), m_activePoints.at(1)));
            break;

        case Ellipse:
            m_undoStack.push(new EllipseCommand(this, m_activePoints.at(0), m_activePoints.at(1)));
            break;
        }

        m_activePoints.clear();
    }

    update();
}


/**
 * Paint the contents of the editor.
 * This will fill the background and draw a grid based on the elementSize and elementGroup.
 * For each element of the current path the control points are drawn with suitable lines
 * joining them, for example for a cubic curve, a curve is drawn, but the control points
 * are joined with dashed lines.
 * A complete path is constructed and painted in a light colour with transparency to show
 * the current symbol shape.
 * TODO scale the painter earlier to avoid the use of toScreen.
 *
 * @param event a pointer to a QPaintEvent
 */
void Editor::paintEvent(QPaintEvent *event)
{
    // initialise the painter
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.fillRect(event->rect(), Qt::white);

    // draw vertical grid
    for (int x = 0 ; x < gridElements + 1 ; ++x) {
        if (x % elementGroup) {
            p.setPen(Qt::lightGray);
        } else {
            p.setPen(Qt::darkGray);
        }

        p.drawLine(x * elementSize, 0, x * elementSize, m_size + 1);
    }

    // draw horizontal grid
    for (int y = 0 ; y < gridElements + 1 ; ++y) {
        if (y % elementGroup) {
            p.setPen(Qt::lightGray);
        } else {
            p.setPen(Qt::darkGray);
        }

        p.drawLine(0, y * elementSize, m_size + 1, y * elementSize);
    }

    // define a rectangle for the points
    QRect dot(0, 0, pointSize, pointSize);

    // create a dashed pen for use with curve references and a wide pen for the shape
    QPen dashedPen(Qt::DashLine);

    // draw all the points as a circle
    p.setBrush(Qt::SolidPattern);

    for (int i = 0 ; i < m_points.count() ; ++i) {
        QPoint s = toScreen(m_points.at(i));
        dot.moveCenter(s);
        p.drawEllipse(dot);
    }

    for (int i = 0 ; i < m_activePoints.count() ; ++i) {
        QPoint s = toScreen(m_activePoints.at(i));
        dot.moveCenter(s);
        p.drawEllipse(dot);
    }

    // iterate through the elements and for each curve element draw the reference lines with a dashed pen
    for (int i = 0, j = 0 ; i < m_elements.count() ; ++i) {
        QPainterPath::ElementType element = m_elements[i];
        QPoint s;
        QPoint e;
        QPoint c1;
        QPoint c2;

        switch (element) {
        case QPainterPath::MoveToElement:
            // increment to next pointer
            ++j;
            break;

        case QPainterPath::LineToElement:
            // increment to next pointer
            j++;
            break;

        case QPainterPath::CurveToElement:
            p.setPen(dashedPen);
            s = toScreen(m_points.at(j - 1));
            c1 = toScreen(m_points.at(j++));
            c2 = toScreen(m_points.at(j++));
            e = toScreen(m_points.at(j++));
            p.drawLine(s, c1);
            p.drawLine(c1, c2);
            p.drawLine(c2, e);
            break;
        }
    }

    // draw the rubber band rectangle or the active points for the current command
    if (m_rubberBand.isValid()) {
        p.setBrush(Qt::NoBrush);

        if (m_toolMode == Rectangle) {
            p.drawRect(QRect(toScreen(m_rubberBand.topLeft()), toScreen(m_rubberBand.bottomRight())));
        } else {
            p.drawEllipse(QRect(toScreen(m_rubberBand.topLeft()), toScreen(m_rubberBand.bottomRight())));
        }
    } else if (m_activePoints.count() && m_toolMode != Rectangle && m_toolMode != Ellipse) {
        p.setPen(dashedPen);
        QPoint s;

        if (m_points.count()) {
            s = toScreen(m_points.last());
        }

        for (int i = 0 ; i < m_activePoints.count() ; ++i) {
            QPoint e = toScreen(m_activePoints[i]);
            p.drawLine(s, e);
            s = e;
        }
    }

    // draw a rectangle representing the preferred symbol size allowing for some white space
    int border = elementSize * 2;
    QRect preferredSizeRect = QRect(0, 0, m_size, m_size).adjusted(border, border, -border, -border);
    QColor preferredSizeColor(Qt::green);
    preferredSizeColor.setAlpha(128);
    p.setPen(preferredSizeColor);
    p.setBrush(Qt::NoBrush);
    p.drawRect(preferredSizeRect);

    // scale the painter and draw the path
    p.scale(m_size, m_size);
    QColor c(Qt::black);
    c.setAlpha(128);
    QPen pathPen(c);
    pathPen.setWidthF(m_symbol.filled() ? 0.0 : m_symbol.lineWidth());
    pathPen.setCapStyle(m_symbol.capStyle());
    pathPen.setJoinStyle(m_symbol.joinStyle());
    p.setPen(pathPen);
    QBrush pathFill(m_symbol.filled() ? Qt::SolidPattern : Qt::NoBrush);
    pathFill.setColor(c);
    p.setBrush(pathFill);
    p.drawPath(m_painterPath);

    // draw the guidelines
    QColor red(Qt::red);
    red.setAlpha(128);
    QPen guidePen(red);
    p.setPen(guidePen);
    p.setBrush(Qt::NoBrush);
    QRectF snapRect(0, 0, 0.03, 0.03);

    foreach (const QPointF & snapPoint, m_snapPoints) {
        snapRect.moveCenter(snapPoint);
        p.drawRect(snapRect);
    }

    foreach (const QLineF & guideLine, m_guideLines) {
        p.drawLine(guideLine);
    }

    foreach (qreal guideCircle, m_guideCircles) {
        p.drawEllipse(QPointF(0.5, 0.5), guideCircle, guideCircle);
    }
}


/**
 * Process key presses to check for Escape to clear the current points being entered.
 *
 * @param event a pointer to a QKeyEvent
 */
void Editor::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
        m_activePoints.clear();
        update();
        break;

    default:
        QWidget::keyPressEvent(event);
        break;
    }
}


/**
 * Convert a point to a symbol point.
 *
 * The point will have come from the mouse events pos() value and will be converted to
 * a value representing a point in the symbol, not necessarily at a snap position.
 *
 * @param point a const reference to a QPoint
 *
 * @return a QPointF representing the point in symbol coordinates between 0..1
 */
QPointF Editor::toSymbol(const QPoint &point) const
{
    double sx = static_cast<double>(point.x()) / m_size;
    double sy = static_cast<double>(point.y()) / m_size;
    return QPointF(sx, sy);
}


/**
 * Convert a point to a snap point.
 *
 * The point comes from the mouse events pos() value and will be converted to a value
 * in symbol coordinates. If snapping is enabled, the point will be snapped to a guide
 * or to a grid intersection. If snapping is disabled, the point is returned converted
 * to the symbol coordinates.
 *
 * @param point a const reference to a QPoint
 *
 * @return a QPointF representing the symbol coordinates either snapped or not.
 */
QPointF Editor::snapPoint(const QPoint &point) const
{
    QPair<bool, QPointF> snap = snapToGuide(toSymbol(point));

    if (!snap.first) {
        snap = snapToGrid(point);
    }

    return snap.second;
}


/**
 * Convert a point to a symbol snap point.
 * The point will have come from the mouse events pos() value and will be converted to
 * a value that represents an intersection of the grid lines. This will happen only if
 * snapping is enabled.
 *
 * @param point a const reference to a QPoint
 *
 * @return a QPair<bool, QPointF> the bool element determines if a snap point was found represented by the QPointF element
 */
QPair<bool, QPointF> Editor::snapToGrid(const QPoint &point) const
{
    QPair<bool, QPointF> snap(false, point);

    if (m_snap) {
        double sx = round(static_cast<double>(point.x()) * gridElements / (m_size)) / gridElements;
        double sy = round(static_cast<double>(point.y()) * gridElements / (m_size)) / gridElements;
        snap.first = true;
        snap.second = QPointF(sx, sy);
    }

    return snap;
}


/**
 * Convert a point to a guide snap point.
 * The point will have come from the mouse events pos() value and will be converted to
 * a value that is within the threshold of a calculated guide intersection. This will
 * happen only if the snapping is enabled.
 *
 * @param point a const reference to a QPointF
 *
 * @return a QPair<bool, QPointF> the bool element determines if a snap point was found represented by the QPointF element
 */
QPair<bool, QPointF> Editor::snapToGuide(const QPointF &point) const
{
    QPair<bool, QPointF> snap(false, point);

    if (m_snap) {
        foreach (const QPointF & p, m_snapPoints) {
            if ((point - p).manhattanLength() < threshold) {
                snap.first = true;
                snap.second = p;
                break;
            }
        }
    }

    return snap;
}


/**
 * Convert a symbol point to a screen point.
 * The symbol point will be a QPointF in the range 0..1 and will be converted to a value
 * representing the position in the editor.
 *
 * @param point a const reference to a QPointF
 *
 * @return a QPoint representing the screen position in the editor
 */
QPoint Editor::toScreen(const QPointF &point) const
{
    int sx = floor(point.x() * m_size);
    int sy = floor(point.y() * m_size);
    return QPoint(sx, sy);
}


/**
 * Test if a control point is at the point specified.
 * This will test both the committed m_points and the uncommitted m_activePoints
 *
 * @param point a const reference to a QPointF representing the symbol coordinate
 *
 * @return true if a node exists here, false otherwise
 */
bool Editor::node(const QPointF &point) const
{
    bool found = false;

    for (int i = 0 ; i < m_points.count() ; ++i) {
        QPointF distance = point - m_points[i];

        if (distance.manhattanLength() < threshold) {
            found = true;
        }
    }

    for (int i = 0 ; i < m_activePoints.count() ; ++i) {
        QPointF distance = point - m_activePoints[i];

        if (distance.manhattanLength() < threshold) {
            found = true;
        }
    }

    return found;
}


/**
 * Find the index of the node that is at the point specified.
 * This will test both the committed m_points and the uncommitted m_activePoints
 *
 * @param point a const reference to a QPointF representing the point to find
 *
 * @return a QPair<bool, int> representing the list and the index of the node, true if committed, false otherwise
 * -1 is returned as the index if the node was not found
 */
QPair<bool, int> Editor::nodeUnderCursor(const QPointF &point) const
{
    for (int i = 0 ; i < m_points.count() ; ++i) {
        QPointF distance = point - m_points[i];

        if (distance.manhattanLength() < threshold) {
            return QPair<bool, int>(true, i);
        }
    }

    for (int i = 0 ; i < m_activePoints.count() ; ++i) {
        QPointF distance = point - m_activePoints[i];

        if (distance.manhattanLength() < threshold) {
            return QPair<bool, int>(false, i);
        }
    }

    return QPair<bool, int>(false, -1);
}


/**
 * Convert a QPainterPath to commands and points.
 * Clear the current lists of points.
 * Iterate all the elements of the QPainterPath adding commands and points as necessary.
 */
void Editor::deconstructPainterPath()
{
    m_points.clear();
    m_activePoints.clear();
    m_elements.clear();

    for (int i = 0 ; i < m_painterPath.elementCount() ; ++i) {
        QPainterPath::Element e = m_painterPath.elementAt(i);

        switch (e.type) {
        case QPainterPath::MoveToElement:
            m_elements.append(e.type);
            m_points.append(QPointF(e));
            break;

        case QPainterPath::LineToElement:
            m_elements.append(e.type);
            m_points.append(QPointF(e));
            break;

        case QPainterPath::CurveToElement:
            m_elements.append(e.type);
            m_points.append(QPointF(e));
            break;

        case QPainterPath::CurveToDataElement:
            m_points.append(QPointF(e));
            break;
        }
    }
}


/**
 * Construct a QPainterPath from the commands and points.
 * Initialise an empty QPainterPath.
 * Iterate all elements stored in m_elements and add them to the path.
 */
void Editor::constructPainterPath()
{
    QPainterPath path;;

    for (int i = 0, j = 0 ; i < m_elements.count() ; ++i) {
        QPainterPath::ElementType e = m_elements[i];

        switch (e) {
        case QPainterPath::MoveToElement:
            path.moveTo(m_points[j++]);
            break;

        case QPainterPath::LineToElement:
            path.lineTo(m_points[j++]);
            break;

        case QPainterPath::CurveToElement:
            path.cubicTo(m_points[j], m_points[j + 1], m_points[j + 2]);
            j += 3;
            break;
        }
    }

    path.setFillRule(m_painterPath.fillRule());
    m_painterPath = path;
}


/**
 * Construct guides for the point relative to the other points.
 * Iterate through points in m_points and m_active points passing them to the
 * constructLineGuides and constructCircleGuides with the new point to create
 * guide lines conforming with the requirements for each.
 *
 * @param to a const reference to a QPointF representing the new point position
 *
 * @return true if guides created, false otherwise.  Guides will be in m_guideLines and m_guideCircles
 */
bool Editor::constructGuides(const QPointF &to)
{
    foreach (const QPointF & from, m_points) {
        constructLineGuides(from, to);
        constructCircleGuides(from, to);
    }

    foreach (const QPointF & from, m_activePoints) {
        constructLineGuides(from, to);
        constructCircleGuides(from, to);
    }

    return !m_guideLines.isEmpty();
}


/**
 * Construct line guides for the given point.
 * The angle formed by the line from-to is tested against the allowed angles.
 * If the angle is acceptable a new QLineF is created projected to the edges of
 * the grid.
 *
 * @param from a const reference to a QPointF representing the point to test, from m_points or m_activePoints
 * @param to a const reference to a QPointF representing the cursor position
 */
void Editor::constructLineGuides(const QPointF &from, const QPointF &to)
{
    if (from != to) {
        QLineF line(from, to);

        if (m_angles.contains(line.angle())) {
            m_guideLines.append(projected(line));
        }
    }
}


/**
 * Construct circular guides for the given point.
 * Calculate the possible intersection points where point p would be if it was mirrored
 * across the vertical and horizontal center lines, this would be comparable to it being
 * on a circle whose center is at the grid center.
 * There are special cases where the point is on the center line. These mirrored points will
 * not be included as they are the same as the new one.
 *
 * @param from a const reference to a QPointF representing the point to test, from m_points or m_activePoints
 * @param to a const reference to a QPointF representing the cursor position
 */
void Editor::constructCircleGuides(const QPointF &from, const QPointF &to)
{
    QVector<QPointF> intersections;
    intersections << QPointF(1.0 - from.x(), from.y()) << QPointF(from.x(), 1.0 - from.y()) << QPointF(1.0 - from.x(), 1.0 - from.y());
    bool circles = false;

    foreach (const QPointF & i, intersections) {
        if (i == from) { // point is on center line
            break;
        }

        if (i == to) {
            m_guideLines.append(projected(QLineF(i, to)));
            circles = true;
        }
    }

    if (circles) {
        m_guideCircles.append(sqrt(pow(0.5 - to.x(), 2) + pow(0.5 - to.y(), 2)));
    }
}


/**
 * Project the line to the edges of the grid.
 * Calculate the points where the projected line would intersect the edges.
 * Check which edges get intersected within the coordinate 0..1 which will determine
 * which points are required to construct the projected line.
 * A special case occurs when the line is a diagonal passing through the corners as it
 * intersects all sides within the coordinate range 0..1
 *
 * @param line a const reference to a QLineF
 *
 * @return a QLineF representing the projected line
 */
QLineF Editor::projected(const QLineF &line) const
{
    QPointF intersectTop;
    QPointF intersectBottom;
    QPointF intersectLeft;
    QPointF intersectRight;

    QLineF::IntersectType t;
    QLineF::IntersectType b;
    QLineF::IntersectType l;
    QLineF::IntersectType r;

    t = line.intersect(m_topEdge, &intersectTop);
    b = line.intersect(m_bottomEdge, &intersectBottom);
    l = line.intersect(m_leftEdge, &intersectLeft);
    r = line.intersect(m_rightEdge, &intersectRight);

    if (t == 0) {      // horizontal line
        return QLineF(intersectLeft, intersectRight);
    }

    if (l == 0) {      // vertical line
        return QLineF(intersectTop, intersectBottom);
    }

    if (intersectTop == QPointF(0.0, 0.0) || intersectTop == QPointF(1.0, 0.0)) { // diagonal line at the corners
        return QLineF(intersectTop, intersectBottom);
    }

    if (intersectTop.x() > 0.0 && intersectTop.x() < 1.0) {
        if (intersectLeft.y() > 0.0 && intersectLeft.y() < 1.0) {
            return QLineF(intersectTop, intersectLeft);
        } else {
            return QLineF(intersectTop, intersectRight);
        }
    } else {
        if (intersectLeft.y() > 0.0 && intersectLeft.y() < 1.0) {
            return QLineF(intersectBottom, intersectLeft);
        } else {
            return QLineF(intersectBottom, intersectRight);
        }
    }
}

