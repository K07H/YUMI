/*
** YUMI (Your Unity Mods Installer) is a LogiCode TM software product (program,
** object code and source code) of the LC-IT Solutions company (SIREN:
** 882877525; Email: <contact@lc-it-solutions.com>) and its purpose is to
** facilitate the installation of video game plugins for Unity-based games.
** TM Copyright 2022-2023 LogiCode. All rights reserved.
** Copyright 2022-2023 LC-IT Solutions. All rights reserved.
**
** This file is part of YUMI. YUMI is licensed under the GNU General Public
** License, Version 2. No trademark or publicity rights are granted by this
** license. This license does not grant you any right, title or interest in any
** LC-IT Solutions name, logo or trademarks. You may not claim any affiliation
** or association with LC-IT Solutions or its employees. You must reproduce
** faithfully all trademarks, copyrights, author attributions and other
** proprietary and legal notices on copies of the Program, copies of the Program
** object code, copies of the Program source code, modified versions of the
** Program, modified versions of the Program object code and modified versions
** of the Program source code. Neither the name of the licensor and copyright
** holder (i.e. LC-IT Solutions), or its affiliates, may be used to endorse or
** promote derived products without specific prior written permission from LC-IT
** Solutions.
**
**
**                      TRADEMARK AND COPYRIGHT NOTICES
**
** TM Copyright 2022-2023 LogiCode. All rights reserved.
** Copyright 2022-2023 LC-IT Solutions. All rights reserved.
**
**
**                 REDISTRIBUTION AND MODIFICATION CONDITIONS
**
** YUMI is free software: you can redistribute it and/or modify it under the
** terms of the GNU General Public License, Version 2, as published by the Free
** Software Foundation, provided that the following conditions are met:
**
**     1. Redistributions of source code must retain the above trademark and
**        copyright notices, this list of conditions and the following
**        disclaimer.
**
**     2. Redistributions in binary form must reproduce the above trademark and
**        copyright notices, this list of conditions and the following
**        disclaimer in the documentation and/or other materials provided with
**        the distribution.
**
**
**                                 DISCLAIMER
**
** THE PROGRAM IS PROVIDED IN THE HOPE THAT IT WILL BE USEFUL, BUT WITHOUT ANY
** WARRANTIES, WHETHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,
** IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT,
** TITLE AND MERCHANTABILITY. THE PROGRAM IS BEING DELIVERED OR MADE AVAILABLE
** "AS IS", "WITH ALL FAULTS" AND WITHOUT WARRANTY OR REPRESENTATION. THE ENTIRE
** RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU. SHOULD THE
** PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARRY SERVICING,
** REPAIR OR CORRECTION.
**
** UNDER NO CIRCUMSTANCES SHALL ANY COPYRIGHT HOLDER OR ITS AFFILIATES, OR ANY
** OTHER PARTY WHO MODIFIES AND/OR CONVEYS THE PROGRAM AS PERMITTED BY THE GNU
** GENERAL PUBLIC LICENSE, VERSION 2, BE LIABLE TO YOU, WHETHER IN AN ACTION OF
** CONTRACT, TORT OR OTHERWISE, FOR ANY DAMAGES OR OTHER LIABILITY, INCLUDING
** ANY GENERAL, DIRECT, INDIRECT, SPECIAL, INCIDENTAL, CONSEQUENTIAL OR PUNITIVE
** DAMAGES ARISING FROM, OUT OF OR IN CONNECTION WITH THE USE OR INABILITY TO
** USE THE PROGRAM, OR OTHER DEALINGS WITH THE PROGRAM (INCLUDING, BUT NOT
** LIMITED TO, LOSS OF DATA, DATA BEING RENDERED INACCURATE, LOSSES SUSTAINED BY
** YOU OR THIRD PARTIES, OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER
** PROGRAMS), WHETHER OR NOT ANY COPYRIGHT HOLDER OR SUCH OTHER PARTY RECEIVES
** NOTICE OF ANY SUCH DAMAGES AND WHETHER OR NOT SUCH DAMAGES COULD HAVE BEEN
** FORESEEN.
**
** See the GNU General Public License, Version 2, for more details. You should
** have received a copy of it along with the Program. If not, see
** <https://www.gnu.org/licenses/>, or write to the Free Software Foundation, 51
** Franklin Street, Fifth Floor, Boston, MA 02110 USA.
*/

#include <QCoreApplication>
#include <QDateTime>
#include <QTextStream>
#include "logger.h"

QFile Logger::outputLogFile = QFile(LOG_FILENAME);
QtMessageHandler Logger::qtDefaultMsgHandler = qInstallMessageHandler(0);

void Logger::writeToLogFile(const QString& toLog)
{
    if (!outputLogFile.isOpen())
        outputLogFile.open(QIODevice::WriteOnly | QIODevice::Append);

    if (outputLogFile.isOpen() && outputLogFile.isWritable())
    {
        QTextStream ts(&outputLogFile);
        ts << toLog << '\n';
        ts.flush();
    }
}

void Logger::myCustomMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    QDateTime localDatetime(QDateTime::currentDateTime());
    QString formattedDatetime = localDatetime.toString(QCoreApplication::translate("Logger", "yyyy-MM-dd hh:mm:ss", "Date and time format"));

    switch (type)
    {
    case QtDebugMsg:
        writeToLogFile(QString("%1: " + QCoreApplication::translate("Logger", "Debug", "Label text") + ": %2").arg(formattedDatetime, msg));
        break;
    case QtInfoMsg:
        writeToLogFile(QString("%1: " + QCoreApplication::translate("Logger", "Info", "Label text") + ": %2").arg(formattedDatetime, msg));
        break;
    case QtWarningMsg:
        writeToLogFile(QString("%1: " + QCoreApplication::translate("Logger", "Warning", "Label text") + ": %2").arg(formattedDatetime, msg));
        break;
    case QtCriticalMsg:
        writeToLogFile(QString("%1: " + QCoreApplication::translate("Logger", "Critical", "Label text") + ": %2").arg(formattedDatetime, msg));
        break;
    case QtFatalMsg:
        writeToLogFile(QString("%1: " + QCoreApplication::translate("Logger", "Fatal", "Label text") + ": %2").arg(formattedDatetime, msg));
        break;
    default:
        break;
    }

    (*qtDefaultMsgHandler)(type, context, msg);
}

void Logger::initialize()
{
    if (outputLogFile.exists())
    {
        try
        {
            QFile backupLogFileC = QFile(BACKUP_LOG_FILENAME_C);
            if (backupLogFileC.exists())
                backupLogFileC.remove();

            QFile backupLogFileB = QFile(BACKUP_LOG_FILENAME_B);
            if (backupLogFileB.exists())
                backupLogFileB.rename(BACKUP_LOG_FILENAME_C);

            QFile backupLogFileA = QFile(BACKUP_LOG_FILENAME_A);
            if (backupLogFileA.exists())
                backupLogFileA.rename(BACKUP_LOG_FILENAME_B);
        }
        catch (...) { }

        QFile toRemove = QFile(BACKUP_LOG_FILENAME_A);
        if (toRemove.exists())
            toRemove.remove();

        if (!toRemove.exists())
            QFile(LOG_FILENAME).rename(BACKUP_LOG_FILENAME_A);
    }

    qInstallMessageHandler(myCustomMessageHandler);
}

Logger::Logger()
{
    Logger::initialize();
}

Logger::~Logger()
{
    if (outputLogFile.isOpen())
        outputLogFile.close();
}
