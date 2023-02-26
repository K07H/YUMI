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
#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include "game_loader.h"
#include "yumi.h"
#include "utils.h"
#include "mods_loader.h"

GameLoader* GameLoader::_instance = NULL;

GameLoader::GameLoader(void* yumi)
{
    this->_yumiPtr = yumi;
}

GameLoader* GameLoader::Init(void* yumi)
{
    if (_instance == NULL)
        _instance = new GameLoader(yumi);
    return _instance;
}

GameLoader* GameLoader::Instance()
{
    return _instance;
}

bool GameLoader::isDataFolderPresent(const QDir& gameFolder)
{
    QStringList folders = gameFolder.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    if (folders.count() > 0)
        for (const QString& folder : folders)
            if (!folder.isEmpty() && folder.endsWith("_Data"))
                return true;
    return false;
}

bool GameLoader::isUnityGame(const QDir& gameFolder, const QString& gameName)
{
    if (!gameFolder.exists())
        return false;
    QDir gameDataFolder(gameFolder);
    if (!gameDataFolder.cd(gameName + "_Data"))
        if (!isDataFolderPresent(gameFolder))
            return false;

    return true;
}

int GameLoader::loadGameFolder(const QString& folderPath)
{
    QDir gameFolder(folderPath);

    if (!gameFolder.exists())
        return -1;
    if (!gameFolder.makeAbsolute())
        return -1;

    QString gameName = gameFolder.dirName();
    QString gamePath = gameFolder.absolutePath();
    if (gameName.isEmpty() || gamePath.isEmpty())
        return -1;

    if (gameName.endsWith(".app"))
    {
        if (gameName.length() <= 4)
            return -1;
        gameName = gameName.left(gameName.length() - 4);
    }

    if (((yumi*)_yumiPtr)->gamesInfo.count() > 0)
    {
        QList<GameInfo> copy(((yumi*)_yumiPtr)->gamesInfo);
        for (const GameInfo& gi : copy)
            if (gi.path.compare(gamePath, Qt::CaseInsensitive) == 0)
            {
                qWarning().nospace() << "This game is already present in games list (with name " << gi.name << " and path " << gi.path << ").";
                return -2;
            }
    }

    if (!isUnityGame(gameFolder, gameName))
    {
        qWarning().nospace() << "This game is not a Unity game.";
        return -3;
    }

    GameExeInfos gameExeInfos = ModsLoader::Instance()->findGameExeInfos(gamePath, gameName);
    GameInfo newGame(gameName, gamePath, gameExeInfos.exeFilepath, gameExeInfos.exeType);
    ((yumi*)_yumiPtr)->gamesInfo.append(newGame);
    ((yumi*)_yumiPtr)->configuration->configChanged = true;
    qDebug().nospace() << "Successfully added game " << gameName << " to games list. GamePath=" << gamePath;

    ((yumi*)_yumiPtr)->mainWidget->sideMenu->addAction(newGame);
    ((yumi*)_yumiPtr)->setSelectedGame(&newGame);
    ((yumi*)_yumiPtr)->mainWidget->sideMenu->setSelectedButton(newGame.path);

    return 0;
}

void GameLoader::showLoadGameFolderResult(const int retval, const QString& folderPath)
{
    QString dirPath(Utils::toUnixPath(folderPath));
    if (retval == 0)
        ((yumi*)_yumiPtr)->showStatusBarMsg(QCoreApplication::translate("GameLoader", "Successfully added game from folder \"%1\".", "Tooltip text").arg(dirPath));
    else if (retval == -2)
        ((yumi*)_yumiPtr)->showStatusBarMsg(QCoreApplication::translate("GameLoader", "Error: Game from folder \"%1\" already exists in games list.", "Tooltip text").arg(dirPath));
    else if (retval == -3)
        ((yumi*)_yumiPtr)->showStatusBarMsg(QCoreApplication::translate("GameLoader", "Error: Game from folder \"%1\" is not a Unity game.", "Tooltip text").arg(dirPath));
    else
        ((yumi*)_yumiPtr)->showStatusBarMsg(QCoreApplication::translate("GameLoader", "Error: Could not load game from folder \"%1\" (selection problem).", "Tooltip text").arg(dirPath));
}

int GameLoader::addGameFolder(const QString& folderPath)
{
    const int retval = loadGameFolder(folderPath);
    showLoadGameFolderResult(retval, folderPath);
    return retval;
}

int GameLoader::openGameFolder()
{
    QFileDialog fileDialog((yumi*)_yumiPtr, QCoreApplication::translate("GameLoader", "Select game folder...", "Folder selection dialog title"));
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::Directory);
    if (fileDialog.exec() != QDialog::Accepted)
        return 1;
    const QString folderPath = fileDialog.selectedFiles().constFirst();
    return addGameFolder(folderPath);
}
