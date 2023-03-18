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

#include <QDebug>
#include <QFile>
#include <QDir>
#include "utils.h"
#include "base64.h"

QString Utils::toUnixPath(const QString& path)
{
    QString result(path);
    result = result.replace("\\\\", "/");
    result = result.replace('\\', '/');
    result = result.replace("//", "/");
    return result;
}

QString Utils::capitalizeFirstLetter(const QString& str)
{
    if (str.length() > 0 && str[0] >= 'a' && str[0] <= 'z')
        return (str.length() > 1 ? str[0].toUpper() + str.mid(1) : str[0].toUpper());
    return str;
}

QString Utils::stripSpecialCharacters(const QString& str)
{
    if (str.isEmpty())
        return "";
    int len = str.length();
    QString result = "";
    if (len > 0)
        for (int i = 0; i < len; i++)
            if (str[i] == ' ' || str[i] == '#' || str[i] == ';' || str[i].isLetterOrNumber())
                result += str[i];
    return result;
}

QString Utils::toReadableFileSize(const qint64 size)
{
    if (size < 1024)
        return (size > 0 ? "1 Ko" : "0 Ko");
    QString digits = QString::number((size / 1024) + ((size % 1024) > 0 ? 1 : 0));
    int digitsLen = digits.length();
    if (digitsLen <= 3)
        return (digits + " Ko");
    QString padded = "";
    int rem = digitsLen % 3;
    if (rem > 0)
    {
        for (int i = 0; i < rem; i++)
            padded += digits[i];
        padded += " ";
    }
    int rounds = digitsLen / 3;
    for (int i = 0; i < rounds; i++)
    {
        padded += digits[rem + (i * 3)];
        padded += digits[rem + 1 + (i * 3)];
        padded += digits[rem + 2 + (i * 3)];
        padded += " ";
    }
    return (padded + "Ko");
}

QString Utils::base64Encode(const QString& str)
{
    QByteArray raw = str.toUtf8();
    const char *cstr = raw.constData();
    int cstrLen = raw.size();

    std::size_t buffLen = Base64::encodedSize(cstrLen) + 1;
    char* buff = new char[buffLen];
    memset(buff, 0, buffLen);

    Base64::b64Encode(buff, cstr, cstrLen);
    QString res(buff);
    delete[] buff;
    return res;
}

QString Utils::base64Decode(const QString& str)
{
    QByteArray raw = str.toUtf8();
    const char *cstr = raw.constData();
    int cstrLen = raw.size();

    std::size_t buffLen = Base64::decodedSize(cstrLen) + 1;
    char* buff = new char[buffLen];
    memset(buff, 0, buffLen);

    Base64::b64Decode(buff, cstr, cstrLen);
    QString res(buff);
    delete[] buff;
    return res;
}

bool Utils::copyRecursively(const QString& sourceFolder, const QString& destFolder, const bool overwrite)
{
    QDir sourceDir(sourceFolder);

    if (!sourceDir.exists())
        return false;

    QDir destDir(destFolder);
    if (!destDir.exists())
        destDir.mkdir(destFolder);

    QStringList files = sourceDir.entryList(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    for (int i = 0; i< files.count(); i++) {
        QString srcName = sourceFolder + QDir::separator() + files[i];
        QString destName = destFolder + QDir::separator() + files[i];
        if (overwrite)
        {
            QFile destFile(destName);
            if (destFile.exists())
                if (!destFile.remove())
                    qWarning().nospace() << "Unable to remove existing file at " << destName << ".";
        }
        if (!QFile::copy(srcName, destName))
            return false;
    }

    files.clear();
    files = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    for (int i = 0; i< files.count(); i++)
    {
        QString srcName = sourceFolder + QDir::separator() + files[i];
        QString destName = destFolder + QDir::separator() + files[i];
        if (!copyRecursively(srcName, destName))
            return false;
    }

    return true;
}

bool Utils::removeRecursively(const QString& folderPath, const QString& folderName)
{
    QString cleanFolderName = (folderName.isEmpty() ? "Folder" : folderName + " folder");
    QString unixFolderPath = Utils::toUnixPath(folderPath);
    QDir unixFolder(unixFolderPath);
    if (unixFolder.exists())
    {
        if (unixFolder.removeRecursively())
            qInfo().nospace() << Utils::capitalizeFirstLetter(cleanFolderName) << " at " << unixFolderPath << " has been removed.";
        else
        {
            qWarning().nospace() << "Failed to remove " << cleanFolderName << " at " << unixFolderPath << ".";
            return false;
        }
    }
    else
        qInfo().nospace() << Utils::capitalizeFirstLetter(cleanFolderName) << " was not found at " << unixFolderPath << ". Skipping removal.";
    return true;
}

bool Utils::removeFile(const QString& filePath, const QString& fileName)
{
    QString cleanFileName = (fileName.isEmpty() ? "File" : fileName + " file");
    QString unixFilePath = Utils::toUnixPath(filePath);
    QFile unixFile(unixFilePath);
    if (unixFile.exists())
    {
        if (unixFile.remove())
            qInfo().nospace() << cleanFileName << " at " << unixFilePath << " has been removed.";
        else
        {
            qWarning().nospace() << "Failed to remove " << cleanFileName << " at " << unixFilePath << " (Error message: " << unixFile.errorString() << ").";
            return false;
        }
    }
    else
        qInfo().nospace() << cleanFileName << " was not found at " << unixFilePath << ". Skipping removal.";
    return true;
}

bool Utils::removeFolderIfEmpty(const QString& folderPath)
{
    QString unixFolderPath = Utils::toUnixPath(folderPath);
    QDir unixFolder(unixFolderPath);
    if (unixFolder.exists() && unixFolder.isEmpty())
    {
        if (unixFolder.removeRecursively())
            qInfo().nospace() << "Empty folder at " << unixFolderPath << " has been removed.";
        else
        {
            qWarning().nospace() << "Unable to remove empty folder at " << unixFolderPath << ".";
            return false;
        }
    }
    else
        qInfo().nospace() << "Folder at " << unixFolderPath << " is not empty. Skipping removal.";
    return true;
}
