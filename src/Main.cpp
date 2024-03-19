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
 * Implement the main function that is the first function to be called when the application starts.
 * Initialises the application and creates the MainWindow. Calls exec on the KApplication to begin
 * the event loop.
 */


/**
 * @mainpage SymbolEditor Documentation
 *
 * @section intro_section Introduction
 * The symbol editor is used to create cross stitch symbols for the KXStitch application.  Originally KXStitch
 * relied on standard character fonts to supply these symbols, but differences in user languages and the quality
 * of the fonts available along with the alignment of the characters and the limited number available has driven
 * the need to create a dedicated symbol set.
 *
 * The symbol set will be stored in a file and several files can be created depending on the users needs, for
 * example having a halloween symbol set for halloween themed patterns.
 *
 * Each of the files will contain a series of symbols which will be displayed in the symbol library tab. Editing
 * of existing symbols can be done by double clicking the entry in the library which will populate the editor
 * with this symbol. Alternatively new symbols can be created and added to the library.
 *
 * @section user_interface The User Interface
 *
 * - @ref main_window
 *  - @ref file_menu
 *  - @ref edit_menu
 *   - @ref file_edit_toolbar
 *  - @ref rendering_menu
 *  - @ref tools_menu
 *   - @ref tools_toolbar
 * - @ref editor_window
 *  - @ref editor_tools
 *  - @ref path_rendering
 *  - @ref points
 *  - @ref editing_symbols
 * - @ref symbol_library
 *
 * @section license_section License
 * SymbolEditor is provided as free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <QApplication>
#include <QCommandLineParser>
#include <QUrl>

#include <KAboutData>
#include <KLocalizedString>

#include "MainWindow.h"


/**
 * The main function creates an instance of a KAboutData object and populates it with any information necessary
 * for the application.
 *
 * A QCommandLineParser object is created to manage any arguments passed on the command line. At this time these are not
 * used.
 *
 * A QApplication object is created to manage the application and a new MainWindow is created and shown on the desktop.
 *
 * The QApplication instance is then executed which begins the event loop allowing user interaction.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("SymbolEditor");

    KAboutData aboutData(QStringLiteral("SymbolEditor"),                    // component name
                         QString(i18n("SymbolEditor")),                     // display name
                         QStringLiteral("2.1.0"),                          // version
                         i18n("A cross stitch symbol editor."),             // short description
                         KAboutLicense::GPL_V2,                             // license
                         i18n("(c)2011-2019 Stephen Allewell"),             // copyright
                         QString(),                                         // other text
                         QStringLiteral("http://userbase.kde.org/SymbolEditor")    // home page
                         // bug address defaults to submit@bugs.kde.org
               );

    aboutData.addAuthor(QStringLiteral("Stephen Allewell"), i18n("Project Lead"), QStringLiteral("steve.allewell@gmail.com"));
    aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));

    KAboutData::setApplicationData(aboutData);
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("SymbolEditor")));

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);

    parser.process(app);

    MainWindow *mainWindow = new MainWindow();
    mainWindow->show();

    return app.exec();
}
