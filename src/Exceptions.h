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
 * Header file for the exception classes.
 */


#ifndef Exceptions_H
#define Exceptions_H


#include <QDataStream>
#include <QtGlobal>


/**
 * @brief Invalid file exception class.
 *
 * This is thrown when the file being opened is not a symbol library.
 */
class InvalidFile
{
public:
    InvalidFile();
    ~InvalidFile();

private:
};


/**
 * @brief Invalid file version exception class.
 *
 * This is thrown when the library file opened is a version that is not known.
 */
class InvalidFileVersion
{
public:
    InvalidFileVersion(qint32 v);
    ~InvalidFileVersion();

    qint32 version;                 /**< the version of the file read */
};


/**
 * @brief Failed to read the library exception class.
 *
 * This is thrown when there was an error reading the QDataStream.
 */
class FailedReadLibrary
{
public:
    FailedReadLibrary(QDataStream::Status status);
    ~FailedReadLibrary();

    QString statusMessage() const;

private:
    QDataStream::Status m_status;   /**< the status of the error */
};


/**
 * @brief Failed to write the library exception class.
 *
 * This is thrown when there was an error reading the QDataStream.
 */
class FailedWriteLibrary
{
public:
    FailedWriteLibrary(QDataStream::Status status);
    ~FailedWriteLibrary();

    QString statusMessage() const;

private:
    QDataStream::Status m_status;   /**< the status of the error */
};


/**
 * @brief Found an invalid symbol version.
 *
 * This is thrown when the symbol being read was not a known version.
 */
class InvalidSymbolVersion
{
public:
    InvalidSymbolVersion(qint32 v);
    ~InvalidSymbolVersion();

    qint32  version;                /** the version of the symbol read */
};


#endif
