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

#include <QVector>
#include <QFile>
#include <QDir>
#include <QDebug>
#include "QtGui/private/qzipreader_p.h"
#include "zipfiles_manager.h"

ZipfilesManager* ZipfilesManager::_instance = NULL;

ZipfilesManager::ZipfilesManager() { }

ZipfilesManager* ZipfilesManager::Instance()
{
    if (_instance == NULL)
        _instance = new ZipfilesManager();
    return _instance;
}

bool ZipfilesManager::unzip(const QString& filePath, const QString& destinationDir)
{
    qInfo().nospace() << "Extracting " << filePath << " to " << destinationDir << ".";
    QDir baseDir(destinationDir);
    if (!baseDir.exists())
    {
        qWarning().nospace() << "Destination folder " << destinationDir << " does not exists.";
        return false;
    }

    QZipReader zipReader(filePath, QIODevice::ReadOnly);
    try
    {
        QVector<QZipReader::FileInfo> allFiles = zipReader.fileInfoList();
        foreach (QZipReader::FileInfo fi, allFiles) {
            if (fi.isDir)
            {
                const QString absPath = destinationDir + QDir::separator() + fi.filePath;
                if (baseDir.mkpath(fi.filePath)) {
                    if (!QFile::setPermissions(absPath, fi.permissions)) {
                        qWarning().nospace() << "Could not set permissions to folder " << absPath << ".";
                        return false;
                    }
                } else {
                    qWarning().nospace() << "Failed to create folder " << absPath << ".";
                    return false;
                }
            }
        }
        foreach (QZipReader::FileInfo fi, allFiles) {
            if (fi.isFile)
            {
                const QString absPath = destinationDir + QDir::separator() + fi.filePath;
                QFile file(absPath);
                if (file.open(QFile::WriteOnly)) {
                    if (file.write(zipReader.fileData(fi.filePath), zipReader.fileData(fi.filePath).size())) {
                        if (!file.setPermissions(fi.permissions)) {
                            qWarning().nospace() << "Could not set permissions to file " << absPath << ".";
                        }
                    } else {
                        qWarning().nospace() << "Failed to decompress data to file " << absPath << ".";
                    }
                    file.close();
                } else {
                    qWarning().nospace() << "Could not get write permissions to file " << absPath << ".";
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        qWarning().nospace() << "Exception caught while extracting archive. Exception: " << ex.what();
    }
    catch (...)
    {
        qWarning().nospace() << "Exception caught while extracting archive.";
    }

    zipReader.close();
    return true;
}
