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

#ifndef __MODS_LOADER_H__
#define __MODS_LOADER_H__

#include <QString>
#include <QList>
#include <QDir>
#include "mod_info.h"
#include "game_info.h"
#include "bepinexfile.h"

#define NT_READ_MAXLEN 1024
#define LFANEW_OFFSET 60
#define MAGIC_OFFSET 24
#define X32_MAGIC 267
#define X64_MAGIC 523

#define LAUNCHER_NATIVE 0
#define LAUNCHER_STEAM 1
#define LAUNCHER_EPIC 2

#define QPROCESS_TIMEOUT 20000

struct GameExeInfos
{
    QString exeFilepath;
    int exeType;
};

struct BepInExConfig
{
    void* gameDetails;
    GameInfo* game;
    bool forEveryone;
    int launcherType;
    bool steamRestarted;
    bool configurationCanceled;
};

class ModsLoader
{
private:
    static ModsLoader* _instance;

    ModsLoader();
    ModsLoader(const ModsLoader& other) = delete;
    ModsLoader& operator=(const ModsLoader&) = delete;

    bool isWindowsOS();
    bool isWineOS();
    bool removeEmptyFolder(const QString& folderPath);
    int getLauncherType(const QString& gameFolderPath);
    QStringList getDataFolders(const QString& gameFolderPath);
    QStringList getPossibleExeNames(const QString& gameFolderPath);
    QString getUnixExePath(const QString& gameFolderPath);
    int is64bits(QFile& gameExe);
    bool isIL2CPP(const QString& gameFolderPath);
    QList<BepInExFile*>* getBepInExFiles(const QString& exeFilepath, const int exeType);
    bool copyBepInExFiles(const QDir& gameFolder, const QString& gameFolderPath, const QList<BepInExFile*>& bepInExFiles);
    bool removeBepInExFiles(const QString& gameFolderPath, const QList<BepInExFile*>& bepInExFiles);
    bool isSteamRunning();
    bool pluginsFolderExists(const QDir& gameFolder);
    QString parseSteamConfigAttribute(const QString& line, const QString& prefix);
    QString getSteamAppsFolderPath(const QString& gameFolderPath);
    QString getSteamFolderPath(void* gameDetails, const QString& gameFolderPath);
    QStringList getSteamLocalConfigs(const QString& userDataFolderPath, const QDir& userDataFolder);
    int getSteamAppId(const QString& steamAppsFolderPath, const QDir& steamAppsFolder, const QString& gameName);
    QString stripERROutputCommand(const QString& cmd);
    bool updateSteamLocalConfigFile(const QString& localConfigPath, const int appId, const QString& preArguments, const QString& postArguments);
    bool cleanSteamLocalConfigFile(const QString& localConfigPath, const int appId);
    bool configureSteamCommandLine(BepInExConfig& config, const QString& preArguments, const QString& postArguments);
    bool cleanSteamCommandLine(BepInExConfig& config);
    bool setupWineDllOverride(BepInExConfig& config, const QDir& gameFolder);
    bool cleanWineDllOverride(BepInExConfig& config);
    bool setupBepInExScript(BepInExConfig& config, const QDir& gameFolder);
    bool cleanBepInExScript(BepInExConfig& config);
    bool configureBepInEx(BepInExConfig& config, const QDir& gameFolder);
    bool cleanBepInEx(BepInExConfig& config);
    bool createBepInExFoldersForMod(QDir& tempBepInExDir, const QString& tempBepInExDirPath, const QString& gameFolderName);
    QString extractModToTempFolder(QDir& yumiDir, const QString& modsFolderPath, const QString& modFileName);
    QString moveModFolderToTempFolder(QDir& yumiDir, const QString& modsFolderPath, const QString& modFolderName);
    QString getActualTempModFolderPath(const QString& tempModDirPath, const QString& modName);
    void removeTemporaryExtractionFolder();
    bool installExtractedMod(const QString& extractedModFolderPath, const QString& modName, const QString& modsFolderPath, void* yumiPtr, bool isArchive);

public:
    static ModsLoader* Instance();

    bool installInProgress;

    int getGameExeType(const QString& gameExeFilepath);
    GameExeInfos findGameExeInfos(const QString& gameFolderPath, const QString& gameName);
    bool isBepInExInstalled(GameInfo* gameInfo, const QDir& gameFolder, void* gameDetails);
    QString getBepInExSubFolderPath(const QDir& gameFolder, const QString& subFolderName);
    QList<ModInfo>* getModsList(const QString& modsFolderPath);
    bool installBepInEx(void* yumiPtr, void* gameDetails, GameInfo* game, const bool forEveryone);
    bool uninstallBepInEx(void* yumiPtr, void* gameDetails, GameInfo* game, const bool includingBepInExConfigFile, const bool includingAllModsAndSavedData);
    void installMods(void* yumiPtr);
    bool copyRecursively(QString sourceFolder, QString destFolder, bool overwrite = true);
};

#endif
