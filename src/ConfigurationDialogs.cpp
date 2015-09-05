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
 * Implement the configuration dialog pages.
 */


/**
 * @page configuration Configuration Dialogs
 * Configuration of the SymbolEditor application is done through the configuration dialog.
 */


#include "ConfigurationDialogs.h"


/**
 * Constructor
 *
 * @param parent a pointer to the parent QWidget
 * @param name the name of this widget
 */
EditorConfigPage::EditorConfigPage(QWidget *parent, const char *name)
    :   QWidget(parent)
{
    setObjectName(name);
    setupUi(this);
}
