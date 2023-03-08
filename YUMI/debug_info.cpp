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

#include <QDir>
#include <QDateTime>
#include "debug_info.h"
#include "utils.h"
#include "yumi.h"
#include "config.h"
#include "logger.h"

DebugInfo* DebugInfo::_instance = NULL;

DebugInfo::DebugInfo(QObject* parent) : QObject(parent) { }

DebugInfo* DebugInfo::Init(QObject* parent)
{
    if (_instance == NULL)
        _instance = new DebugInfo(parent);
    return _instance;
}

DebugInfo* DebugInfo::Instance()
{
    return _instance;
}

QString DebugInfo::GetFileTree(const QString& folderPath, QVector<bool> tabs, bool isLastFolder, bool filesPresent)
{
    if (folderPath.isEmpty())
        return "";
    QDir folder(folderPath);
    if (!folder.exists())
        return "";

    QString folderTabs = "";
    QString fileTabs = "";
    int nbTabs = tabs.count();
    if (nbTabs > 0)
        for (int i = 0; i < nbTabs; i++)
        {
            bool notLastTab = (i < (nbTabs - 1));
            folderTabs += (notLastTab ? (tabs[i] ? "   " : "│  ") : (isLastFolder && !filesPresent ? "└──" : "├──"));
            fileTabs += ((!isLastFolder || filesPresent || notLastTab) ? (tabs[i] ? "   " : "│  ") : "   ");
        }

    QFileInfo fi(folderPath);
    QString res = folderTabs + (nbTabs <= 0 ? "~ " : "⊞ ") + folder.dirName() + " (" + fi.lastModified().toString("yyyy-MM-dd hh:mm:ss") + ")\n";
    QStringList folders = folder.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDir::SortFlag::Name | QDir::SortFlag::IgnoreCase);
    QStringList files = folder.entryList(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDir::SortFlag::Name | QDir::SortFlag::IgnoreCase);
    int nbFolders = folders.count();
    int nbFiles = files.count();

    for (int i = 0; i < nbFolders; i++)
    {
        QVector<bool> newTabs(tabs);
        newTabs.push_back((i == (nbFolders - 1)) && (nbFiles <= 0));
        res += GetFileTree(Utils::toUnixPath(folderPath + QDir::separator() + folders[i]), newTabs, i == (nbFolders - 1), nbFiles > 0);
    }
    for (int i = 0; i < nbFiles; i++)
    {
        QFileInfo currFile(Utils::toUnixPath(folderPath + QDir::separator() + files[i]));
        QString fileSize;
        if (currFile.exists())
            fileSize = Utils::toReadableFileSize(currFile.size()) + " - " + currFile.lastModified().toString("yyyy-MM-dd hh:mm:ss");
        else
            fileSize = "deleted - unknown";
        res += (fileTabs + (i == (nbFiles - 1) ? "└╌╌ " : "├╌╌ ") + files[i] + " (" + fileSize + ")\n");
    }

    return res;
}

QString DebugInfo::GetFileContent(const QString& filePath)
{
    QString res = filePath;
    QFile yumiLogFile(filePath);
    if (yumiLogFile.exists())
    {
        QFileInfo fi(filePath);
        res += " (" + fi.lastModified().toString("yyyy-MM-dd hh:mm:ss") + "):\n";
        if (yumiLogFile.open(QIODevice::ReadOnly))
        {
            res += QString::fromUtf8(yumiLogFile.readAll());
            yumiLogFile.close();
        }
    }
    else
        res += ":\n";
    return res;
}

QString DebugInfo::GetSteamID(const QString& str)
{
    if (str.isEmpty())
        return "";

    QString delim = "Caching Steam ID:";
    int sidPos = str.indexOf(delim, 0, Qt::CaseInsensitive);
    if (sidPos > 0 && str.length() > (sidPos + delim.length()))
    {
        int endPos = -1;
        QString sid = "";
        sidPos += delim.length();
        for (int i = sidPos; i < str.length() && endPos < 0; i++)
        {
            if (str[i] >= '0' && str[i] <= '9')
                sid += str[i];
            else
                if (str[i] != ' ' && str[i] != '\t')
                    endPos = i;
        }
        if (endPos > sidPos)
            return sid;
    }
    return "";
}

QString DebugInfo::GetOSUserName(const QString& str)
{
#ifndef Q_OS_WINDOWS
    QString delim = "\"/home/";
#else
    QString delim = ":/Users/";
#endif
    int sttPos = str.indexOf(delim);
    if (sttPos < 0)
    {
#ifndef Q_OS_WINDOWS
        delim = "/home/";
#else
        delim = ":\\Users\\";
#endif
        sttPos = str.indexOf(delim);
    }
    if (sttPos < 0)
        return "";
    sttPos += delim.length();
    int strLen = str.length();
    if (strLen <= sttPos)
        return "";
    QString userName = "";
    for (int i = sttPos; i < strLen; i++)
    {
        if (str[i] == '/' || str[i] == '\\' || str[i] == '\n')
            break;
        else
            userName += str[i];
    }
    return userName;
}

void DebugInfo::GetDriveLettersForDelim(QVector<QChar>& driveLetters, const QString& str, const QString& delimiter)
{
    int stt = str.indexOf(delimiter);
    if (stt > 0)
    {
        int delimiterLen = delimiter.length();
        int strLen = str.length();
        while (stt > 0 && stt < strLen)
        {
            if (stt > 1 && str[stt - 1] >= 'A' && str[stt - 1] <= 'Z' && (str[stt - 2] == '"' || str[stt - 2] == '\r' || str[stt - 2] == '\n'))
                if (!driveLetters.contains(str[stt - 1]))
                    driveLetters.push_back(str[stt - 1]);
            stt = str.indexOf(delimiter, stt + delimiterLen);
        }
    }
}

#ifdef Q_OS_WINDOWS
QVector<QChar> DebugInfo::GetDriveLetters(const QString& str)
#else
QVector<QChar> DebugInfo::GetDriveLetters(const QString&)
#endif
{
    QVector<QChar> driveLetters = QVector<QChar>();
#ifdef Q_OS_WINDOWS
    GetDriveLettersForDelim(driveLetters, str, ":/");
    GetDriveLettersForDelim(driveLetters, str, ":\\");
#endif
    return driveLetters;
}

QString DebugInfo::AnonymizeLogs(QString& str)
{
    if (str.isEmpty())
        return "";
    QString sid = GetSteamID(str);
    if (!sid.isEmpty())
        str = str.replace(sid, "STEAMID");
    QString osu = GetOSUserName(str);
    if (!osu.isEmpty())
        str = str.replace(osu, "USERNAME");
    QVector<QChar> driveLetters = GetDriveLetters(str);
    int driveLettersLen = driveLetters.count();
    for (int i = 0; i < driveLettersLen; i++)
        str = str.replace(QString(driveLetters[i]) + ":/", "X:/").replace(QString(driveLetters[i]) + ":\\", "X:\\");
    return str;
}

QString DebugInfo::GetYUMILogs(bool anon)
{
    QString res = GetFileContent(Utils::toUnixPath(yumi::appPath + QDir::separator() + LOG_FILENAME)) + "\n\n" +
            GetFileContent(Utils::toUnixPath(yumi::appPath + QDir::separator() + BACKUP_LOG_FILENAME_A)) + "\n\n" +
            GetFileContent(Utils::toUnixPath(yumi::appPath + QDir::separator() + BACKUP_LOG_FILENAME_B)) + "\n\n" +
            GetFileContent(Utils::toUnixPath(yumi::appPath + QDir::separator() + BACKUP_LOG_FILENAME_C)) + "\n\n" +
            GetFileContent(Utils::toUnixPath(yumi::appPath + QDir::separator() + CONFIGURATION_FILENAME));
    return (anon ? AnonymizeLogs(res) : res);
}

QString DebugInfo::GetYUMIFileTree(bool anon)
{
    QVector<bool> empty = QVector<bool>();
    QString res = GetFileTree(yumi::appPath, empty);
    return (anon ? AnonymizeLogs(res) : res);
}

QString DebugInfo::GetGameLogs(GameInfo* gameInfo, bool anon)
{
#ifdef Q_OS_WINDOWS
    QString res = (gameInfo == NULL ? "" : GetFileContent(Utils::toUnixPath(gameInfo->path + "/BepInEx/LogOutput.log")) + "\n\n" + GetFileContent(Utils::toUnixPath(gameInfo->path + "/BepInEx/ErrorLog.log")));
#else
    QString runYumiLogsFilePath = gameInfo == NULL ? "" : Utils::toUnixPath(gameInfo->path + "/run_yumi_bepinex.log");
    bool runYumiLogsExists = gameInfo == NULL ? false : QFileInfo::exists(runYumiLogsFilePath);
    QString runYumiErrorLogsFilePath = gameInfo == NULL ? "" : Utils::toUnixPath(gameInfo->path + "/run_yumi_bepinex.err");
    bool runYumiErrorLogsExists = gameInfo == NULL ? false : QFileInfo::exists(runYumiErrorLogsFilePath);
    QString res = (gameInfo == NULL ? "" : GetFileContent(Utils::toUnixPath(gameInfo->path + "/BepInEx/LogOutput.log")) +
                                           "\n\n" + GetFileContent(Utils::toUnixPath(gameInfo->path + "/BepInEx/ErrorLog.log")) +
                                           (runYumiLogsExists ? "\n\n" + GetFileContent(runYumiLogsFilePath) : "") +
                                           (runYumiErrorLogsExists ? "\n\n" + GetFileContent(runYumiErrorLogsFilePath) : ""));
#endif
    return (anon ? AnonymizeLogs(res) : res);
}

QString DebugInfo::GetGameFileTree(GameInfo* gameInfo, bool anon)
{
    QVector<bool> empty = QVector<bool>();
    QString res = (gameInfo == NULL ? "" : GetFileTree(gameInfo->path, empty));
    return (anon ? AnonymizeLogs(res) : res);
}

QString DebugInfo::GetFullDebugLogs(GameInfo* gameInfo, bool anon)
{
    QString toLog = GetYUMILogs(false);
    toLog += ("\n\n" + GetYUMIFileTree(false));
    if (gameInfo != NULL)
    {
        toLog += ("\n\n" + GetGameLogs(gameInfo, false));
        toLog += ("\n\n" + GetGameFileTree(gameInfo, false));
    }
    return (anon ? AnonymizeLogs(toLog) : toLog);
}
