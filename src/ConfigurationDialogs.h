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
 * Header file for the Configuration dialog classes.
 */


#ifndef ConfigurationDialogs_H
#define ConfigurationDialogs_H


#include "ui_EditorConfigPage.h"


/**
 * @brief Implement configuration of the editor.
 */
class EditorConfigPage : public QWidget, public Ui::EditorConfigPage
{
    Q_OBJECT

public:
    explicit EditorConfigPage(QWidget *parent = 0, const char *name = 0);
};


#endif
