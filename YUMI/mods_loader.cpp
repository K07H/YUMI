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
#include <QDebug>
#include <QProcess>
#include <QMessageBox>
#include <QTimer>
#include <QEventLoop>
#ifdef Q_OS_WIN
#include <windows.h>
#endif
#include "mods_loader.h"
#include "game_details.h"
#include "assets.h"
#include "utils.h"
#include "common.h"
#include "config.h"
#include "yumi.h"
#include "zipfiles_manager.h"
#include "select_game_window.h"

ModsLoader* ModsLoader::_instance = NULL;

ModsLoader::ModsLoader()
{
    installInProgress = false;
}

ModsLoader* ModsLoader::Instance()
{
    if (_instance == NULL)
        _instance = new ModsLoader();
    return _instance;
}

QString ModsLoader::getBepInExSubFolderPath(const QDir& gameFolder, const QString& subFolderName)
{
    if (subFolderName.isEmpty())
        return "";
    QDir bepInExFolder(gameFolder);
    if (!bepInExFolder.exists())
        return "";
    if (!bepInExFolder.cd("BepInEx"))
        return "";
    if (!bepInExFolder.cd(subFolderName))
        return "";
    return bepInExFolder.absolutePath();
}

QList<ModInfo>* ModsLoader::getModsList(const QString& modsFolderPath)
{
    if (modsFolderPath.isEmpty())
        return NULL;
    QDir pluginsFolder(modsFolderPath);
    if (!pluginsFolder.exists())
        return NULL;

    QStringList allMods = pluginsFolder.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    QStringList files = pluginsFolder.entryList(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    for (const QString& file : files)
    {
        if (file.endsWith(".dll", Qt::CaseInsensitive))
            allMods.push_back(file);
    }

    QList<ModInfo>* retval = NULL;
    int len = allMods.count();
    if (len > 0)
    {
        if (len > 1)
            allMods.sort();
        retval = new QList<ModInfo>();
        for (int i = 0; i < len; i++)
            if (pluginsFolder.exists(allMods[i]))
            {
                QString modName = allMods[i];
                if (modName.length() > 4 && modName.endsWith(".dll", Qt::CaseInsensitive))
                    modName = modName.left(modName.length() - 4);
                QString modPath(Utils::toUnixPath(pluginsFolder.absolutePath() + QDir::separator() + allMods[i]));
                retval->push_back(ModInfo(modName, modPath));
            }
    }

    return retval;
}

bool ModsLoader::isWindowsOS()
{
#ifdef Q_OS_WIN
    return true;
#else
    return false;
#endif
}

bool ModsLoader::isWineOS()
{
#ifdef Q_OS_WIN
    HMODULE hntdll = GetModuleHandle(L"ntdll.dll");
    bool isWine = (hntdll && GetProcAddress(hntdll, "wine_get_version"));
    if (isWine)
        qInfo() << "Wine OS detected.";
    return isWine;
#else
    return false;
#endif
}

QStringList ModsLoader::getDataFolders(const QString& gameFolderPath)
{
    QStringList result;
    if (gameFolderPath.isEmpty())
        return result;
    QDir gameFolder(gameFolderPath);
    if (!gameFolder.exists())
        return result;
    QStringList folders = gameFolder.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    if (folders.count() > 0)
        for (QString& folder : folders)
            if (!folder.isEmpty() && folder.endsWith("_Data"))
                result.push_back(folder);
    return result;
}

QStringList ModsLoader::getPossibleExeNames(const QString& gameFolderPath)
{
    QStringList result;
    QStringList dataFolders = getDataFolders(gameFolderPath);
    if (dataFolders.count() > 0)
        for (const QString& dataFolder : dataFolders)
            if (dataFolder.length() > 5 && dataFolder.endsWith("_Data"))
                result.push_back(QString(dataFolder).left(dataFolder.length() - 5));
    return result;
}

GameExeInfos ModsLoader::findGameExeInfos(const QString& gameFolderPath, const QString& gameName)
{
    GameExeInfos result;
    result.exeFilepath = "";
    result.exeType = UNKNOWN_GAME;
    QStringList possibleExeNames = getPossibleExeNames(gameFolderPath);
    if (possibleExeNames.count() > 0)
    {
        QString unix64Path = "";
        QString unix86Path = "";
        QString win64Path = "";
        QString win86Path = "";
        QString exePath;
        for (const QString& exeName : possibleExeNames)
        {
            exePath = Utils::toUnixPath(gameFolderPath + QDir::separator() + exeName);
            QFile darwinApp = QFile(exePath + ".app");
            if (darwinApp.exists())
            {
                result.exeFilepath = exePath + ".app";
                result.exeType = DARWIN_GAME;
                return result;
            }
            QFile unixApp64 = QFile(exePath + ".x86_64");
            if (unixApp64.exists())
                unix64Path = exePath + ".x86_64";
            QFile unixApp32 = QFile(exePath + ".x86");
            if (unixApp32.exists())
                unix86Path = exePath + ".x86";
            QFile winApp64 = QFile(exePath + ".exe");
            if (winApp64.exists())
                win64Path = exePath + ".exe";
            QFile winApp32 = QFile(exePath + "32.exe");
            if (winApp32.exists())
                win86Path = exePath + "32.exe";
        }

        bool isIL2CppGame = isIL2CPP(gameFolderPath);

        if (isWindowsOS())
        {
            if (!win64Path.isEmpty() && !win86Path.isEmpty())
            {
#ifdef Q_OS_WIN64
                result.exeFilepath = win64Path;
                result.exeType = (isIL2CppGame ? WIN_GAME_IL2CPP_X64 : WIN_GAME_X64);
#else
                result.exeFilepath = win86Path;
                result.exeType = (isIL2CppGame ? WIN_GAME_IL2CPP_X86 : WIN_GAME_X86);
#endif
                return result;
            }
            if (!win64Path.isEmpty())
            {
                result.exeFilepath = win64Path;
                result.exeType = (isIL2CppGame ? WIN_GAME_IL2CPP_X64 : WIN_GAME_X64);
                return result;
            }
            if (!win86Path.isEmpty())
            {
                result.exeFilepath = win86Path;
                result.exeType = (isIL2CppGame ? WIN_GAME_IL2CPP_X86 : WIN_GAME_X86);
                return result;
            }
            if (!unix64Path.isEmpty())
            {
                result.exeFilepath = unix64Path;
                result.exeType = UNIX_GAME_X64;
                return result;
            }
            if (!unix86Path.isEmpty())
            {
                result.exeFilepath = unix86Path;
                result.exeType = UNIX_GAME_X86;
                return result;
            }
        }
        else
        {
            if (!unix64Path.isEmpty() && !unix86Path.isEmpty())
            {
                QProcess isX64OS;
                isX64OS.start("uname", QStringList() << "-m");
                isX64OS.waitForFinished(QPROCESS_TIMEOUT);
                if (isX64OS.exitStatus() == QProcess::NormalExit && isX64OS.exitCode() == 0)
                {
                    QString isX64OSResult(isX64OS.readAllStandardOutput());
                    if (!isX64OSResult.isEmpty() && isX64OSResult.contains("x86_64"))
                    {
                        result.exeFilepath = unix64Path;
                        result.exeType = UNIX_GAME_X64;
                        return result;
                    }
                }
                result.exeFilepath = unix86Path;
                result.exeType = UNIX_GAME_X86;
                return result;
            }
            if (!unix64Path.isEmpty())
            {
                result.exeFilepath = unix64Path;
                result.exeType = UNIX_GAME_X64;
                return result;
            }
            if (!unix86Path.isEmpty())
            {
                result.exeFilepath = unix86Path;
                result.exeType = UNIX_GAME_X86;
                return result;
            }
            if (!win64Path.isEmpty())
            {
                result.exeFilepath = win64Path;
                result.exeType = (isIL2CppGame ? WIN_GAME_IL2CPP_X64 : WIN_GAME_X64);
                return result;
            }
            if (!win86Path.isEmpty())
            {
                result.exeFilepath = win86Path;
                result.exeType = (isIL2CppGame ? WIN_GAME_IL2CPP_X86 : WIN_GAME_X86);
                return result;
            }
        }
    }
    qCritical().nospace() << "Could not find executable filepath for " << gameName << " in " << gameFolderPath << ".";
    result.exeFilepath = "";
    result.exeType = UNKNOWN_GAME;
    return result;
}

int ModsLoader::getGameExeType(const QString& gameExeFilepath)
{
    if (!gameExeFilepath.isEmpty())
    {
        QFile gameExeFile(gameExeFilepath);
        if (gameExeFile.exists())
        {
            if (gameExeFilepath.endsWith(".app"))
                return DARWIN_GAME;
            if (gameExeFilepath.endsWith(".x86_64"))
                return UNIX_GAME_X64;
            if (gameExeFilepath.endsWith(".x86"))
                return UNIX_GAME_X86;
            if (gameExeFilepath.endsWith(".exe"))
            {
                bool isIL2CppGame = false;
                QString gameFolderPath = Utils::toUnixPath(gameExeFilepath);
                int lastIdx = gameExeFilepath.lastIndexOf('/');
                if (lastIdx > 0)
                {
                    gameFolderPath = gameFolderPath.left(lastIdx);
                    isIL2CppGame = isIL2CPP(gameFolderPath);
                }

                if (!gameExeFilepath.endsWith("32.exe"))
                    return (isIL2CppGame ? WIN_GAME_IL2CPP_X64 : WIN_GAME_X64);
                else
                    return (isIL2CppGame ? WIN_GAME_IL2CPP_X86 : WIN_GAME_X86);
            }
        }
    }
    return UNKNOWN_GAME;
}

QString ModsLoader::getUnixExePath(const QString& gameFolderPath)
{
    QStringList possibleExeNames = getPossibleExeNames(gameFolderPath);
    if (possibleExeNames.count() > 0)
    {
        QString unix64Path = "";
        QString unix86Path = "";
        QString exePath;
        for (const QString& exeName : possibleExeNames)
        {
            exePath = Utils::toUnixPath(gameFolderPath + QDir::separator() + exeName);
            QFile darwinApp = QFile(exePath + ".app");
            if (darwinApp.exists())
                return exePath + ".app";
            QFile unixApp64 = QFile(exePath + ".x86_64");
            if (unixApp64.exists())
                unix64Path = exePath + ".x86_64";
            QFile unixApp32 = QFile(exePath + ".x86");
            if (unixApp32.exists())
                unix86Path = exePath + ".x86";
        }
        if (!unix64Path.isEmpty() && !unix86Path.isEmpty())
        {
            QProcess isX64OS;
            isX64OS.start("uname", QStringList() << "-m");
            isX64OS.waitForFinished(QPROCESS_TIMEOUT);
            if (isX64OS.exitStatus() == QProcess::NormalExit && isX64OS.exitCode() == 0)
            {
                QString isX64OSResult(isX64OS.readAllStandardOutput());
                if (!isX64OSResult.isEmpty() && isX64OSResult.contains("x86_64"))
                    return unix64Path;
            }
            return unix86Path;
        }
        if (!unix64Path.isEmpty())
            return unix64Path;
        if (!unix86Path.isEmpty())
            return unix86Path;
    }
    qCritical().nospace() << "Could not find Unix executable in folder " << gameFolderPath << ".";
    return "";
}

int ModsLoader::getLauncherType(const QString& gameFolderPath)
{
    return ((gameFolderPath.contains("/steamapps/") || gameFolderPath.contains("\\steamapps\\")) ? LAUNCHER_STEAM : LAUNCHER_NATIVE);
}

int ModsLoader::is64bits(QFile& gameExe)
{
    if (gameExe.size() < NT_READ_MAXLEN)
        return -2;
    QByteArray data;
    if (!gameExe.open(QIODevice::ReadOnly | QIODevice::ExistingOnly))
        return -3;
    try { data = gameExe.read(NT_READ_MAXLEN); }
    catch (const std::exception& ex) { data = QByteArray(); qWarning().nospace() << "Exception caught while reading executable " << gameExe.fileName() << ". Exception=[" << ex.what() << "]"; }
    catch (...) { data = QByteArray(); qWarning() << "Exception caught while reading executable " << gameExe.fileName() << "."; }
    gameExe.close();
    qsizetype dataLen = data.length();
    if (dataLen < (LFANEW_OFFSET + 4) || (unsigned char)data.at(0) != 'M' || (unsigned char)data.at(1) != 'Z')
        return -4;
    unsigned long lfanew = (unsigned char)data.at(LFANEW_OFFSET) | (unsigned char)data.at(LFANEW_OFFSET + 1) << 8 | (unsigned char)data.at(LFANEW_OFFSET + 2) << 16 | (unsigned char)data.at(LFANEW_OFFSET + 3) << 24;
    if (lfanew >= (NT_READ_MAXLEN - (MAGIC_OFFSET + 2)))
        return -5;
    qsizetype typedLfanew = static_cast<qsizetype>(lfanew);
    if (dataLen < (typedLfanew + MAGIC_OFFSET + 1) || (unsigned char)data.at(typedLfanew) != 'P' || (unsigned char)data.at(typedLfanew + 1) != 'E')
        return -6;
    unsigned int magic = (unsigned char)data.at(typedLfanew + MAGIC_OFFSET) | (unsigned char)data.at(typedLfanew + MAGIC_OFFSET + 1) << 8;
    if (magic == X32_MAGIC)
        return 0;
    if (magic == X64_MAGIC)
        return 1;
    return -1;
}

bool ModsLoader::isIL2CPP(const QString& gameFolderPath)
{
    QFile gameAssemblyFile(Utils::toUnixPath(gameFolderPath + QDir::separator() + "GameAssembly.dll"));
    if (!gameAssemblyFile.exists())
        return false;
    QFile unityPlayerFile(Utils::toUnixPath(gameFolderPath + QDir::separator() + "UnityPlayer.dll"));
    if (!unityPlayerFile.exists())
        return false;
    return true;
}

QList<BepInExFile*>* ModsLoader::getBepInExFiles(const QString& exeFilepath, const int exeType)
{
    QList<BepInExFile*>* bepInExFiles = NULL;
    if (exeType == WIN_GAME_X64 || exeType == WIN_GAME_X86 || exeType == WIN_GAME_IL2CPP_X64 || exeType == WIN_GAME_IL2CPP_X86)
    {
        qDebug() << "Game's operating system: Windows/Wine";
        if (exeFilepath.isEmpty())
        {
            qCritical().nospace() << "Could not find game executable file at " << exeFilepath << ".";
            return NULL;
        }
        QFile exeFile(exeFilepath);
        if (!exeFile.exists())
        {
            qCritical().nospace() << "Game executable file does not exist at " << exeFilepath << ".";
            return NULL;
        }
        int is64 = is64bits(exeFile);
        if (is64 < 0 || is64 > 1)
        {
            qWarning().nospace() << "Could not determine if game executable at " << exeFilepath << " is x86 or x64 (code: " << QString::number(is64) << ").";
            qWarning() << "Using game executable architecture based on executable name.";
            is64 = ((exeType == WIN_GAME_X64 || exeType == WIN_GAME_IL2CPP_X64) ? 1 : 0);
        }
        if (exeType == WIN_GAME_IL2CPP_X64 || exeType == WIN_GAME_IL2CPP_X86)
        {
            if (is64 == 0)
                bepInExFiles = Assets::Instance()->getIL2CPPX86Files();
            else
                bepInExFiles = Assets::Instance()->getIL2CPPX64Files();
        }
        else
        {
            if (is64 == 0)
                bepInExFiles = Assets::Instance()->getX86Files();
            else
                bepInExFiles = Assets::Instance()->getX64Files();
        }
    }
    else if (exeType != UNKNOWN_GAME)
    {
        qDebug() << "Game's operating system: Unix";
        bepInExFiles = Assets::Instance()->getUnixFiles();
    }
    else
        qWarning() << "Could not determine game executable architecture" << (exeFilepath.isEmpty() ? "" : " for file at " + exeFilepath + "") << ".";
    if (bepInExFiles == NULL)
        qCritical() << "Could not load BepInEx files from resources.";
    return bepInExFiles;
}

bool ModsLoader::isBepInExInstalled(GameInfo* gameInfo, const QDir& gameFolder, void* gameDetails)
{
    if (!gameFolder.exists())
        return false;
    if (gameInfo->name.isEmpty())
        return false;
    QDir bepInExFolder(gameFolder);
    if (!bepInExFolder.cd("BepInEx"))
        return false;
    QString gameFolderPath(Utils::toUnixPath(gameFolder.absolutePath() + QDir::separator()));
    QList<BepInExFile*>* bepInExFiles = getBepInExFiles(gameInfo->exePath, gameInfo->exeType);
    if (bepInExFiles == NULL)
    {
        qWarning() << "Checking BepInEx installation failed: Could not retrieve BepInEx files from YUMI.";
        QMessageBox checkBepInExFailed(QMessageBox::Warning, QCoreApplication::translate("MainWindow", "Mods loader check failed", "Popup title"), QCoreApplication::translate("MainWindow", "Warning: Mods loader installation could not be verified (Unable to retrieve BepInEx files from YUMI). Please check your internet connection or increase download timeout in settings.", "Popup text"), QMessageBox::Ok, (GameDetails*)gameDetails);
        checkBepInExFailed.exec();
        return false;
    }
    int filesLen = bepInExFiles->count();
    for (int i = 0; i < filesLen; i++)
        if (!(*bepInExFiles)[i]->name.isEmpty() && (*bepInExFiles)[i]->file != NULL)
        {
            QString outputFolderPath(Utils::toUnixPath(gameFolderPath + QDir::separator() + (*bepInExFiles)[i]->path));
            if (!(*bepInExFiles)[i]->path.isEmpty())
            {
                QDir outputFolder(outputFolderPath);
                if (!outputFolder.exists())
                    return false;
            }
            QString outputFilePath(Utils::toUnixPath(outputFolderPath + QDir::separator() + (*bepInExFiles)[i]->name));
            QFile outputFile(outputFilePath);
            if (!outputFile.exists())
                return false;
        }
    return true;
}

bool ModsLoader::copyBepInExFiles(const QDir& gameFolder, const QString& gameFolderPath, const QList<BepInExFile*>& bepInExFiles)
{
    qDebug() << "Copying BepInEx files to game folder...";
    if (bepInExFiles.count() > 0)
        for (const BepInExFile* file : bepInExFiles)
            if (!file->name.isEmpty() && file->file != NULL)
            {
                QString outputFolderPath(Utils::toUnixPath(gameFolderPath + QDir::separator() + file->path));
                if (!file->path.isEmpty())
                {
                    QDir outputFolder(outputFolderPath);
                    if (!outputFolder.exists())
                        if (!gameFolder.mkpath(Utils::toUnixPath(file->path)))
                        {
                            qCritical().nospace() << "Unable to create folder " << outputFolderPath << ".";
                            return false;
                        }
                }
                QString outputFilePath(Utils::toUnixPath(outputFolderPath + QDir::separator() + file->name));
                QFile outputFile(outputFilePath);
                if (outputFile.exists())
                {
                    qWarning().nospace() << "BepInEx file " << file->name << " already exists in folder " << outputFolderPath << ". Overwriting it.";
                    if (!outputFile.isWritable())
                    {
                        if (!outputFile.open(QFile::ReadWrite))
                        {
                            if (!outputFile.setPermissions(outputFile.permissions() | QFile::WriteOwner | QFile::WriteUser))
                            {
                                qCritical().nospace() << "Unable to set write permission for file " << file->name << " from folder " << outputFolderPath << " (File permissions: " << QFile::permissions(outputFilePath) << ". Error message: " << outputFile.errorString() << ").";
                                return false;
                            }
                        }
                        if (!outputFile.isWritable())
                        {
                            if (!outputFile.open(QFile::ReadWrite))
                            {
                                qCritical().nospace() << "Unable to open file " << file->name << " from folder " << outputFolderPath << " with write permission (File permissions: " << QFile::permissions(outputFilePath) << ". Error message: " << outputFile.errorString() << ").";
                                return false;
                            }
                            if (!outputFile.isWritable())
                            {
                                qCritical().nospace() << "Unable to get write permission for file " << file->name << " from folder " << outputFolderPath << " (File permissions: " << QFile::permissions(outputFilePath) << ").";
                                return false;
                            }
                        }
                    }
                    if (!outputFile.remove())
                    {
                        qCritical().nospace() << "Unable to replace file " << file->name << " from folder " << outputFolderPath << " (File permissions: " << QFile::permissions(outputFilePath) << ". Error message: " << outputFile.errorString() << ").";
                        return false;
                    }
                    if (outputFile.isOpen())
                        outputFile.close();
                }
                if (!file->file->copy(outputFilePath))
                {
                    qCritical().nospace() << "Unable to copy file " << file->name << " to " << outputFilePath << " (Folder " << outputFolderPath << ". Error message: " << file->file->errorString() << ").";
                    return false;
                }
            }
    qDebug() << "Successfully copied BepInEx files to game folder.";
    return true;
}

bool ModsLoader::removeEmptyFolder(const QString& folderPath)
{
    QDir folder(folderPath);
    if (folder.exists() && folder.isEmpty())
    {
        if (folder.removeRecursively())
        {
            qInfo().nospace() << "Empty folder at " << folderPath << " has been removed.";
            return true;
        }
        else
            qWarning().nospace() << "Unable to remove empty folder at " << folderPath << ".";
    }
    return false;
}

bool ModsLoader::removeBepInExFiles(const QString& gameFolderPath, const QList<BepInExFile*>& bepInExFiles)
{
    qDebug() << "Removing BepInEx files from game folder...";
    bool success = true;
    if (bepInExFiles.count() > 0)
        for (const BepInExFile* file : bepInExFiles)
            if (!file->name.isEmpty())
            {
                QString outputFolderPath(Utils::toUnixPath(gameFolderPath + QDir::separator() + file->path));
                if (!file->path.isEmpty())
                {
                    QDir outputFolder(outputFolderPath);
                    if (!outputFolder.exists())
                    {
                        success = false;
                        qWarning().nospace() << "Unable to find folder " << outputFolderPath << ". Skipping " << file->name << " file removal.";
                        continue;
                    }
                }
                QString outputFilePath(Utils::toUnixPath(outputFolderPath + QDir::separator() + file->name));
                QFile outputFile(outputFilePath);
                if (!outputFile.exists())
                {
                    success = false;
                    qWarning().nospace() << "Unable to find file " << outputFilePath << ". Skipping file removal.";
                }
                else
                {
                    if (!outputFile.isWritable())
                    {
                        if (!outputFile.open(QFile::ReadWrite))
                        {
                            if (!outputFile.setPermissions(outputFile.permissions() | QFile::WriteOwner | QFile::WriteUser))
                                qWarning().nospace() << "Unable to set write permission for file " << outputFilePath << " (File permissions: " << QFile::permissions(outputFilePath) << ". Error message: " << outputFile.errorString() << ").";
                        }
                        if (!outputFile.isWritable())
                        {
                            if (!outputFile.open(QFile::ReadWrite))
                                qWarning().nospace() << "Unable to open file " << outputFilePath << " with write permission (File permissions: " << QFile::permissions(outputFilePath) << ". Error message: " << outputFile.errorString() << ").";
                            if (!outputFile.isWritable())
                                qWarning().nospace() << "Unable to get write permission for file " << outputFilePath << " (File permissions: " << QFile::permissions(outputFilePath) << ").";
                        }
                    }
                    if (!outputFile.remove())
                    {
                        success = false;
                        qWarning().nospace() << "Unable to remove file " << outputFilePath << " (Error message: " << outputFile.errorString() << ").";
                    }
                    else
                        qInfo().nospace() << "File " << outputFilePath << " has been removed.";
                    if (outputFile.isOpen())
                        outputFile.close();
                }
            }
    return success;
}

bool ModsLoader::isSteamRunning()
{
    bool isSteamRunning = false;
#ifdef Q_OS_WIN
    QProcess taskList;
    taskList.start("tasklist", QStringList() << "/NH" << "/FO" << "CSV" << "/FI" << QString("IMAGENAME eq Steam.exe"));
    taskList.waitForFinished(QPROCESS_TIMEOUT);
    if (taskList.exitStatus() == QProcess::NormalExit && taskList.exitCode() == 0)
    {
        QString taskListResult(taskList.readAllStandardOutput());
        isSteamRunning = (!taskListResult.isEmpty() && (taskListResult.startsWith("\"steam.exe") || taskListResult.startsWith("\"Steam.exe")));
    }
#else
    QProcess processList;
    processList.start("ps", QStringList() << "-C" << "steam" << "--no-headers" << "ww");
    processList.waitForFinished(QPROCESS_TIMEOUT);
    if (processList.exitStatus() == QProcess::NormalExit && processList.exitCode() == 0)
    {
        QString processListResult(processList.readAllStandardOutput());
        if (!processListResult.isEmpty() && (processListResult.contains("/steam\n") || processListResult.contains(" steam\n") || processListResult.contains("/Steam\n") || processListResult.contains(" Steam\n")))
            isSteamRunning = true;
    }
#endif
    return isSteamRunning;
}

bool ModsLoader::pluginsFolderExists(const QDir& gameFolder)
{
    if (!gameFolder.exists())
        return false;
    QDir pluginsFolder(gameFolder);
    if (!pluginsFolder.cd("BepInEx"))
        return false;
    if (!pluginsFolder.cd("plugins"))
        return false;
    return true;
}

QString ModsLoader::parseSteamConfigAttribute(const QString& line, const QString& prefix)
{
    qsizetype sttPos = line.indexOf(prefix);
    if (sttPos >= 0 && line.length() > sttPos + prefix.length() + 1 && line.endsWith('"'))
    {
        sttPos += prefix.length();
        qsizetype endPos = line.length() - 1;
        if (endPos > sttPos)
            return line.mid(sttPos, endPos - sttPos);
    }
    return "";
}

int ModsLoader::getSteamAppId(const QString& steamAppsFolderPath, const QDir& steamAppsFolder, const QString& gameName)
{
    qDebug() << "Retrieving app ID from Steam...";
    QStringList acfFiles = steamAppsFolder.entryList(QStringList() << "*.acf", QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    if (acfFiles.count() <= 0)
    {
        qWarning() << "Could not get app ID from Steam.";
        return -1;
    }
    QString searchAppId("\"appid\"\t\t\"");
    QString searchInstallDir("\"installdir\"\t\t\"");
    QString searchInstallDirWithName("\"installdir\"\t\t\"" + gameName + "\"");
    int appId;
    bool foundInstallDir;
    QString line;
    bool ok;
    for (const QString& acfFile : acfFiles)
    {
        appId = -1;
        foundInstallDir = false;
        QFile currentAcf(Utils::toUnixPath(steamAppsFolderPath + QDir::separator() + acfFile));
        if (currentAcf.exists())
        {
            if (currentAcf.open(QIODevice::ReadOnly | QIODevice::ExistingOnly | QIODevice::Text))
            {
                try
                {
                    QTextStream in(&currentAcf);
                    while (!in.atEnd() && !(foundInstallDir && appId != -1))
                    {
                        line = in.readLine();
                        if (!line.isEmpty())
                        {
                            if (appId == -1 && line.contains(searchAppId))
                            {
                                QString appIdStr = parseSteamConfigAttribute(line, searchAppId);
                                if (!appIdStr.isEmpty())
                                {
                                    ok = false;
                                    int parsedAppId = appIdStr.toInt(&ok);
                                    if (ok)
                                        appId = parsedAppId;
                                }
                            }
                            if (!foundInstallDir && line.contains(searchInstallDir) && line.contains(searchInstallDirWithName, Qt::CaseInsensitive))
                                foundInstallDir = true;
                        }
                    }
                }
                catch (const std::exception& ex)
                {
                    appId = -1;
                    qWarning().nospace() << "Exception caught while parsing .acf Steam file located at " << acfFile << ". Exception=[" << ex.what() << "]";
                }
                catch (...)
                {
                    appId = -1;
                    qWarning().nospace() << "Exception caught while parsing .acf Steam file located at " << acfFile << ".";
                }
                currentAcf.close();
                if (foundInstallDir && appId != -1)
                {
                    qDebug().nospace() << "Successfully retrieved app ID from Steam (" << appId << ").";
                    return appId;
                }
            }
            else
                qWarning().nospace() << "Could not open Steam file located at " << acfFile << ".";
        }
    }
    qWarning() << "Could not retrieve app ID from Steam.";
    return -1;
}

QString ModsLoader::getSteamAppsFolderPath(const QString& gameFolderPath)
{
    qsizetype pos = gameFolderPath.indexOf("/steamapps/");
    if (pos <= 0)
        pos = gameFolderPath.indexOf("\\steamapps\\");
    if (pos <= 0)
    {
        qWarning() << "Failed to find \"steamapps\" delimiter in game folder path.";
        return "";
    }
    return gameFolderPath.left(pos + 11);
}

QString ModsLoader::getSteamFolderPath(void* gameDetails, const QString& gameFolderPath)
{
    QString steamFolderPath("");
    bool inSteamLibraryFolder = (gameFolderPath.contains("/SteamLibrary/", Qt::CaseInsensitive) || gameFolderPath.contains("\\SteamLibrary\\", Qt::CaseInsensitive));
    if (!inSteamLibraryFolder && (gameFolderPath.contains("/Steam/", Qt::CaseInsensitive) || gameFolderPath.contains("\\Steam\\", Qt::CaseInsensitive)))
    {
        qsizetype pos = gameFolderPath.indexOf("/Steam/", 0, Qt::CaseInsensitive);
        if (pos <= 0)
            pos = gameFolderPath.indexOf("\\Steam\\", 0, Qt::CaseInsensitive);
        if (pos > 0)
        {
            steamFolderPath = gameFolderPath.left(pos + 7);
            QDir testSteamDir(Utils::toUnixPath(steamFolderPath));
            if (!testSteamDir.exists())
                steamFolderPath = "";
        }
    }
    if (steamFolderPath.isEmpty() && inSteamLibraryFolder)
    {
        qDebug() << "Searching for Steam folder from SteamLibrary...";
        qsizetype slPos = gameFolderPath.indexOf("/SteamLibrary/", 0, Qt::CaseInsensitive);
        if (slPos <= 0)
            slPos = gameFolderPath.indexOf("\\SteamLibrary\\", 0, Qt::CaseInsensitive);
        if (slPos > 0)
        {
            QString steamLibraryFolderPath = gameFolderPath.left(slPos + 14);
            QDir steamLibraryFolder(Utils::toUnixPath(steamLibraryFolderPath));
            if (steamLibraryFolder.exists() && steamLibraryFolder.exists("libraryfolder.vdf"))
            {
                QFile libraryVdf(Utils::toUnixPath(steamLibraryFolderPath + "libraryfolder.vdf"));
                if (libraryVdf.exists())
                {
                    if (libraryVdf.open(QIODevice::ReadOnly | QIODevice::ExistingOnly | QIODevice::Text))
                    {
                        QString line;
                        QString searchLauncher("\"launcher\"\t\t\"");
                        try
                        {
                            QTextStream in(&libraryVdf);
                            while (!in.atEnd() && steamFolderPath.isEmpty())
                            {
                                line = in.readLine();
                                if (!line.isEmpty() && line.contains(searchLauncher))
                                {
                                    QString launcherPath = parseSteamConfigAttribute(line, searchLauncher);
                                    if (!launcherPath.isEmpty() && launcherPath.endsWith("steam.exe", Qt::CaseInsensitive) && launcherPath.length() > 9)
                                    {
                                        QString launcherFolderPath(Utils::toUnixPath(launcherPath.left(launcherPath.length() - 9)));
                                        if (QDir(launcherFolderPath).exists())
                                            steamFolderPath = launcherFolderPath;
                                    }
                                }
                            }
                            if (steamFolderPath.isEmpty())
                                qDebug() << "Steam folder path not found in libraryfolder vdf file.";
                        }
                        catch (const std::exception& ex) { qDebug() << "Exception caught while parsing libraryfolder vdf file. Exception=[" << ex.what() << "]"; }
                        catch (...) { qDebug() << "Exception caught while parsing libraryfolder vdf file."; }
                        libraryVdf.close();
                    }
                    else
                        qDebug() << "Could not open libraryfolder vdf file for reading.";
                }
                else
                    qDebug() << "libraryfolder vdf file does not exists in SteamLibrary.";
            }
            else
                qDebug() << "Could not find libraryfolder vdf file in SteamLibrary.";
        }
        else
            qDebug() << "Could not find Steam folder from SteamLibrary.";
    }
    if (steamFolderPath.isEmpty())
    {
        qDebug() << "Unable to find Steam folder location. Asking its location to user...";
        steamFolderPath = ((GameDetails*)gameDetails)->askSteamFolderLocation();
        if (steamFolderPath.isEmpty())
            qWarning() << "The Steam folder path provided by user is incorrect, installation was canceled by user.";
        else if (!QDir(steamFolderPath).exists())
        {
            qWarning() << "The Steam folder provided by user does not exists.";
            return "";
        }
    }
    return steamFolderPath;
}

QStringList ModsLoader::getSteamLocalConfigs(const QString& userDataFolderPath, const QDir& userDataFolder)
{
    QStringList result;
    QStringList usersFolders = userDataFolder.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    if (usersFolders.count() <= 0)
        return result;
    for (const QString& userFolder : usersFolders)
    {
        QDir currentUserFolder(Utils::toUnixPath(userDataFolderPath + QDir::separator() + userFolder));
        if (currentUserFolder.exists())
            if (currentUserFolder.cd("config"))
                if (currentUserFolder.exists("localconfig.vdf"))
                {
                    QString localConfigPath;
                    if (userFolder.endsWith('/') || userFolder.endsWith('\\'))
                        localConfigPath = Utils::toUnixPath(userFolder + "config/localconfig.vdf");
                    else
                        localConfigPath = Utils::toUnixPath(userFolder + QDir::separator() + "config/localconfig.vdf");
                    result.push_back(localConfigPath);
                }
    }
    return result;
}

QString ModsLoader::stripERROutputCommand(const QString& cmd)
{
    if (cmd.trimmed().startsWith("2>"))
        return "";
    int errOutCmdStt = cmd.indexOf(" 2>");
    if (errOutCmdStt > 0)
        return cmd.left(errOutCmdStt);
    return cmd;
}

bool ModsLoader::updateSteamLocalConfigFile(const QString& localConfigPath, const int appId, const QString& preArguments, const QString& postArguments)
{
    qDebug().nospace() << "Searching if modifications are needed for localconfig vdf file at " << localConfigPath << ".";
    QFile localConfig(localConfigPath);
    if (!localConfig.exists())
    {
        qWarning().nospace() << "Failed to find localconfig file at " << localConfigPath << ".";
        return false;
    }
    if (!localConfig.open(QIODevice::ReadOnly | QIODevice::ExistingOnly | QIODevice::Text))
    {
        qWarning().nospace() << "Failed to open localconfig file at " << localConfigPath << " for reading.";
        return false;
    }
    QStringList newFileContent;
    QString line;
    QString lineBkp;
    QString launchOptionsPrefix = "\"LaunchOptions\"\t\t\"";
    QString toSearch = "\"" + QString::number(appId) + "\"";
    bool foundLaunchOptions = false;
    int openedBraces = 1;
    int tabulationsAmount = -1;
    int step = 0;
    try
    {
        QTextStream in(&localConfig);
        while (!in.atEnd())
        {
            line = in.readLine();
            if (line.isEmpty())
                newFileContent.push_back("\n");
            else
            {
                if (step != 7)
                    newFileContent.push_back(line + "\n");
                else
                    lineBkp = line;
                line = line.trimmed();
                if (step == 0 && line.compare("\"UserLocalConfigStore\"") == 0)
                    ++step;
                else if (step == 1 && line.compare("\"Software\"") == 0)
                    ++step;
                else if (step == 2 && line.compare("\"Valve\"") == 0)
                    ++step;
                else if (step == 3 && line.compare("\"Steam\"") == 0)
                    ++step;
                else if (step == 4 && line.compare("\"apps\"") == 0)
                    ++step;
                else if (step == 5 && line.compare(toSearch) == 0)
                    ++step;
                else if (step == 6 && line.compare("{") == 0)
                    ++step;
                else if (step == 7)
                {
                    if (tabulationsAmount < 0 && !lineBkp.isEmpty())
                    {
                        bool run = true;
                        tabulationsAmount = 0;
                        for (int i = 0; run && (i < lineBkp.length()); i++)
                        {
                            if (lineBkp[i] == '\t')
                                ++tabulationsAmount;
                            else
                                run = false;
                        }
                    }
                    if (line.startsWith(launchOptionsPrefix) && line.endsWith('"'))
                    {
                        foundLaunchOptions = true;
                        QString newLaunchOptions = launchOptionsPrefix + preArguments;
                        QString currentLaunchOptions = parseSteamConfigAttribute(line, launchOptionsPrefix);
                        QString existingArguments = currentLaunchOptions;
                        qsizetype commandPos = currentLaunchOptions.indexOf("%command% ");
                        if (commandPos >= 0 && currentLaunchOptions.length() > (commandPos + 10))
                            existingArguments = currentLaunchOptions.right(currentLaunchOptions.length() - (commandPos + 10));
                        else
                        {
                            commandPos = currentLaunchOptions.indexOf("%command%");
                            if (commandPos >= 0 && currentLaunchOptions.length() > (commandPos + 9))
                                existingArguments = currentLaunchOptions.right(currentLaunchOptions.length() - (commandPos + 9));
                        }
                        existingArguments = stripERROutputCommand(existingArguments);
                        if (!existingArguments.isEmpty())
                            newLaunchOptions += (" " + existingArguments);
                        if (Config::Instance()->logScriptErrors)
                            newLaunchOptions += (" " + postArguments);
                        QString origLaunchOptions = launchOptionsPrefix + currentLaunchOptions + "\"";
                        QString oldLine(lineBkp);
                        QString modifiedLine = lineBkp.replace(origLaunchOptions, newLaunchOptions);
                        qDebug().nospace() << "Modifying Steam launch options line from " << oldLine << " to " << modifiedLine << ".";
                        newFileContent.push_back(modifiedLine + "\n");
                    }
                    else
                    {
                        if (line.compare("{") == 0)
                            ++openedBraces;
                        else if (line.compare("}") == 0)
                            --openedBraces;
                        if (openedBraces <= 0)
                        {
                            ++step;
                            if (!foundLaunchOptions)
                            {
                                QString toInsert = "";
                                for (int j = 0; j < tabulationsAmount; j++)
                                    toInsert += "\t";
                                if (Config::Instance()->logScriptErrors)
                                    toInsert += (launchOptionsPrefix + preArguments + " " + postArguments + "\"");
                                else
                                    toInsert += (launchOptionsPrefix + preArguments + "\"");
                                newFileContent.push_back(toInsert + "\n");
                            }
                        }
                        newFileContent.push_back(lineBkp + "\n");
                    }
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        localConfig.close();
        qCritical().nospace() << "Exception caught while updating localconfig vdf file located at " << localConfigPath << ". Exception=[" << ex.what() << "]";
        return false;
    }
    catch (...)
    {
        localConfig.close();
        qCritical().nospace() << "Exception caught while updating localconfig vdf file located at " << localConfigPath << ".";
        return false;
    }
    localConfig.close();
    if (newFileContent.count() <= 0)
    {
        qDebug().nospace() << "No modifications found to be made. Skipping modification of localconfig vdf file at " << localConfigPath << ".";
        return false;
    }
    QString edited = "";
    for (const QString& str : newFileContent)
        edited += str;
    if (!localConfig.open(QIODevice::WriteOnly | QIODevice::ExistingOnly | QIODevice::Truncate | QIODevice::Text))
    {
        if (!localConfig.setPermissions(localConfig.permissions() | QFileDevice::WriteOwner | QFileDevice::WriteUser))
        {
            qCritical().nospace() << "Unable to get write permissions to Steam config file at " << localConfigPath << " for writing (permissions: " << localConfig.permissions() << ").";
            return false;
        }
        if (!localConfig.open(QIODevice::WriteOnly | QIODevice::ExistingOnly | QIODevice::Truncate | QIODevice::Text))
        {
            qCritical().nospace() << "Unable to open localconfig file at " << localConfigPath << " for writing (permissions: " << localConfig.permissions() << ").";
            return false;
        }
    }
    if (!localConfig.isWritable())
    {
        localConfig.close();
        qCritical().nospace() << "Failed to get write permissions for Steam config file at " << localConfigPath << " (permissions: " << localConfig.permissions() << ").";
        return false;
    }
    try
    {
        QTextStream ts(&localConfig);
        ts << edited;
        ts.flush();
        localConfig.close();
        qDebug().nospace() << "Successfully edited localconfig vdf file at " << localConfigPath << ".";
        return true;
    }
    catch (const std::exception& ex)
    {
        localConfig.close();
        qCritical().nospace() << "Exception caught while writing executable name inside BepInEx script. Exception=[" << ex.what() << "]";
        return false;
    }
    catch (...)
    {
        localConfig.close();
        qCritical() << "Exception caught while writing executable name inside BepInEx script.";
        return false;
    }
}

bool ModsLoader::cleanSteamLocalConfigFile(const QString& localConfigPath, const int appId)
{
    qDebug().nospace() << "Searching for modifications in localconfig vdf file at " << localConfigPath << ".";
    QFile localConfig(localConfigPath);
    if (!localConfig.exists())
    {
        qWarning().nospace() << "Failed to find localconfig file at " << localConfigPath << ".";
        return false;
    }
    if (!localConfig.open(QIODevice::ReadOnly | QIODevice::ExistingOnly | QIODevice::Text))
    {
        qWarning().nospace() << "Failed to open localconfig file at " << localConfigPath << " for reading.";
        return false;
    }
    QStringList newFileContent;
    QString line;
    QString lineBkp;
    QString launchOptionsPrefix = "\"LaunchOptions\"\t\t\"";
    QString toSearch = "\"" + QString::number(appId) + "\"";
    bool foundLaunchOptions = false;
    int tabulationsAmount = -1;
    int step = 0;
    try
    {
        QTextStream in(&localConfig);
        while (!in.atEnd())
        {
            line = in.readLine();
            if (line.isEmpty())
                newFileContent.push_back("\n");
            else
            {
                if (step != 7)
                    newFileContent.push_back(line + "\n");
                else
                    lineBkp = line;
                line = line.trimmed();
                if (step == 0 && line.compare("\"UserLocalConfigStore\"") == 0)
                    ++step;
                else if (step == 1 && line.compare("\"Software\"") == 0)
                    ++step;
                else if (step == 2 && line.compare("\"Valve\"") == 0)
                    ++step;
                else if (step == 3 && line.compare("\"Steam\"") == 0)
                    ++step;
                else if (step == 4 && line.compare("\"apps\"") == 0)
                    ++step;
                else if (step == 5 && line.compare(toSearch) == 0)
                    ++step;
                else if (step == 6 && line.compare("{") == 0)
                    ++step;
                else if (step == 7)
                {
                    if (tabulationsAmount < 0 && !lineBkp.isEmpty())
                    {
                        bool run = true;
                        tabulationsAmount = 0;
                        for (int i = 0; run && (i < lineBkp.length()); i++)
                        {
                            if (lineBkp[i] == '\t')
                                ++tabulationsAmount;
                            else
                                run = false;
                        }
                    }
                    if (line.startsWith(launchOptionsPrefix) && line.endsWith('"'))
                    {
                        foundLaunchOptions = true;
                        QString toInsert = "";
                        for (int j = 0; j < tabulationsAmount; j++)
                            toInsert += "\t";
                        toInsert += (launchOptionsPrefix + "\"");
                        QString oldLine(lineBkp);
                        qDebug().nospace() << "Modifying Steam launch options line from " << oldLine << " to " << toInsert << ".";
                        newFileContent.push_back(toInsert + "\n");
                    }
                    else
                        newFileContent.push_back(lineBkp + "\n");
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        localConfig.close();
        qCritical().nospace() << "Exception caught while updating localconfig vdf file located at " << localConfigPath << ". Exception=[" << ex.what() << "]";
        return false;
    }
    catch (...)
    {
        localConfig.close();
        qCritical().nospace() << "Exception caught while updating localconfig vdf file located at " << localConfigPath << ".";
        return false;
    }
    localConfig.close();
    if (newFileContent.count() <= 0)
    {
        qDebug().nospace() << "No modifications found to be made. Skipping modification of localconfig vdf file at " << localConfigPath << ".";
        return false;
    }
    if (!foundLaunchOptions)
    {
        qDebug().nospace() << "No existing launch options were found. Skipping modification of localconfig vdf file at " << localConfigPath << ".";
        return true;
    }
    QString edited = "";
    for (const QString& str : newFileContent)
        edited += str;
    if (!localConfig.open(QIODevice::WriteOnly | QIODevice::ExistingOnly | QIODevice::Truncate | QIODevice::Text))
    {
        if (!localConfig.setPermissions(localConfig.permissions() | QFileDevice::WriteOwner | QFileDevice::WriteUser))
        {
            qCritical().nospace() << "Unable to get write permissions to Steam config file at " << localConfigPath << " for writing (permissions: " << localConfig.permissions() << ").";
            return false;
        }
        if (!localConfig.open(QIODevice::WriteOnly | QIODevice::ExistingOnly | QIODevice::Truncate | QIODevice::Text))
        {
            qCritical().nospace() << "Unable to open localconfig file at " << localConfigPath << " for writing (permissions: " << localConfig.permissions() << ").";
            return false;
        }
    }
    if (!localConfig.isWritable())
    {
        localConfig.close();
        qCritical().nospace() << "Failed to get write permissions for Steam config file at " << localConfigPath << " (permissions: " << localConfig.permissions() << ").";
        return false;
    }
    try
    {
        QTextStream ts(&localConfig);
        ts << edited;
        ts.flush();
        localConfig.close();
        qDebug().nospace() << "Successfully edited localconfig vdf file at " << localConfigPath << ".";
        return true;
    }
    catch (const std::exception& ex)
    {
        localConfig.close();
        qCritical().nospace() << "Exception caught while writing executable name inside BepInEx script. Exception=[" << ex.what() << "]";
        return false;
    }
    catch (...)
    {
        localConfig.close();
        qCritical() << "Exception caught while writing executable name inside BepInEx script.";
        return false;
    }
}

bool ModsLoader::configureSteamCommandLine(BepInExConfig& config, const QString& preArguments, const QString& postArguments)
{
    qDebug() << "Setting up Steam command line arguments...";
    QString steamAppsFolderPath = getSteamAppsFolderPath(config.game->path);
    if (steamAppsFolderPath.isEmpty())
        return false;
    QDir steamAppsFolder(Utils::toUnixPath(steamAppsFolderPath));
    if (!steamAppsFolder.exists())
    {
        qWarning() << "Failed to find \"steamapps\" folder.";
        return false;
    }
    int steamAppId = getSteamAppId(steamAppsFolderPath, steamAppsFolder, config.game->name);
    if (steamAppId == -1)
        return false;
    QString steamFolderPath = getSteamFolderPath(config.gameDetails, config.game->path);
    if (steamFolderPath.isEmpty())
        return false;
    QDir steamFolder(Utils::toUnixPath(steamFolderPath));
    if (!steamFolder.exists())
    {
        qWarning() << "Failed to find \"Steam\" folder.";
        return false;
    }
    QDir userDataFolder(steamFolder);
    if (!userDataFolder.cd("userdata"))
    {
        qWarning() << "Failed to find \"Steam/userdata\" folder.";
        return false;
    }
    QString userdataFolderPath(Utils::toUnixPath(steamFolderPath + QDir::separator() + "userdata"));
    QStringList localConfigsPaths = getSteamLocalConfigs(userdataFolderPath, userDataFolder);
    if (localConfigsPaths.count() <= 0)
    {
        qWarning() << "Failed to list localconfig vdf files.";
        return false;
    }

    if (isSteamRunning())
    {
        QMessageBox pleaseCloseSteam(QMessageBox::Information, QCoreApplication::translate("MainWindow", "Please close Steam", "Popup title"), QCoreApplication::translate("MainWindow", "Please close Steam then click on \"OK\" to continue installation.", "Popup text"), QMessageBox::Ok, (GameDetails*)config.gameDetails);
        pleaseCloseSteam.exec();
    }
    else
        config.steamRestarted = true;
    bool checkIfSteamRuns = true;
    while (checkIfSteamRuns)
    {
        if (isSteamRunning())
        {
            QMessageBox continueInstall(QMessageBox::Information, QCoreApplication::translate("MainWindow", "Please close Steam", "Popup title"), QCoreApplication::translate("MainWindow", "Steam is still running! You need to close Steam now to continue installation. Click on \"OK\" when Steam is closed (recommended), or click on \"Cancel\" to cancel mods loader configuration (not recommended).", "Popup text"), QMessageBox::Cancel | QMessageBox::Ok, (GameDetails*)config.gameDetails);
            int clickedBtn = continueInstall.exec();
            if (clickedBtn != QMessageBox::Ok)
            {
                config.configurationCanceled = true;
                qWarning() << "Mods loader configuration has been canceled.";
                QMessageBox configCanceled(QMessageBox::Information, QCoreApplication::translate("MainWindow", "Configuration canceled", "Popup title"), QCoreApplication::translate("MainWindow", "Mods loader has been installed but its configuration has been canceled. Please refer to YUMI's wiki to manually configure mods loader.", "Popup text"), QMessageBox::Ok, (GameDetails*)config.gameDetails);
                configCanceled.exec();
                return false;
            }
        }
        else
        {
            checkIfSteamRuns = false;
            config.steamRestarted = true;
        }
    }

    int updated = 0;
    for (const QString& localConfigPath : localConfigsPaths)
        if (updateSteamLocalConfigFile(Utils::toUnixPath(userdataFolderPath + QDir::separator() + localConfigPath), steamAppId, preArguments, postArguments))
            ++updated;

    return (updated > 0);
}

bool ModsLoader::cleanSteamCommandLine(BepInExConfig& config)
{
    qDebug() << "Cleaning Steam command line arguments...";
    QString steamAppsFolderPath = getSteamAppsFolderPath(config.game->path);
    if (steamAppsFolderPath.isEmpty())
        return false;
    QDir steamAppsFolder(Utils::toUnixPath(steamAppsFolderPath));
    if (!steamAppsFolder.exists())
    {
        qWarning() << "Failed to find \"steamapps\" folder.";
        return false;
    }
    int steamAppId = getSteamAppId(steamAppsFolderPath, steamAppsFolder, config.game->name);
    if (steamAppId == -1)
        return false;
    QString steamFolderPath = getSteamFolderPath(config.gameDetails, config.game->path);
    if (steamFolderPath.isEmpty())
        return false;
    QDir steamFolder(Utils::toUnixPath(steamFolderPath));
    if (!steamFolder.exists())
    {
        qWarning() << "Failed to find \"Steam\" folder.";
        return false;
    }
    QDir userDataFolder(steamFolder);
    if (!userDataFolder.cd("userdata"))
    {
        qWarning() << "Failed to find \"Steam/userdata\" folder.";
        return false;
    }
    QString userdataFolderPath(Utils::toUnixPath(steamFolderPath + QDir::separator() + "userdata"));
    QStringList localConfigsPaths = getSteamLocalConfigs(userdataFolderPath, userDataFolder);
    if (localConfigsPaths.count() <= 0)
    {
        qWarning() << "Failed to list localconfig vdf files.";
        return false;
    }

    if (isSteamRunning())
    {
        QMessageBox pleaseCloseSteam(QMessageBox::Information, QCoreApplication::translate("MainWindow", "Please close Steam", "Popup title"), QCoreApplication::translate("MainWindow", "Please close Steam then click on OK to continue uninstallation.", "Popup text"), QMessageBox::Ok, (GameDetails*)config.gameDetails);
        pleaseCloseSteam.exec();
    }
    else
        config.steamRestarted = true;
    bool checkIfSteamRuns = true;
    while (checkIfSteamRuns)
    {
        if (isSteamRunning())
        {
            QMessageBox continueUninstall(QMessageBox::Information, QCoreApplication::translate("MainWindow", "Please close Steam", "Popup title"), QCoreApplication::translate("MainWindow", "Steam is still running! You need to close Steam now to continue uninstallation. Click on \"OK\" when Steam is closed (recommended), or click on \"Cancel\" to cancel Steam configuration cleaning (not recommended).", "Popup text"), QMessageBox::Cancel | QMessageBox::Ok, (GameDetails*)config.gameDetails);
            int clickedBtn = continueUninstall.exec();
            if (clickedBtn != QMessageBox::Ok)
            {
                config.configurationCanceled = true;
                qWarning() << "Steam configuration cleaning has been canceled.";
                QMessageBox configCanceled(QMessageBox::Information, QCoreApplication::translate("MainWindow", "Steam config clearing canceled", "Popup title"), QCoreApplication::translate("MainWindow", "Mods loader has been uninstalled but Steam configuration has not been cleared. Please remove the game's additional arguments: You can do so by opening the game properties in Steam, then search for the field \"Additional arguments\" and remove its content. You can join the Discord for additional help.", "Popup text"), QMessageBox::Ok, (GameDetails*)config.gameDetails);
                configCanceled.exec();
                return false;
            }
        }
        else
        {
            checkIfSteamRuns = false;
            config.steamRestarted = true;
        }
    }

    int updated = 0;
    for (const QString& localConfigPath : localConfigsPaths)
        if (cleanSteamLocalConfigFile(Utils::toUnixPath(userdataFolderPath + QDir::separator() + localConfigPath), steamAppId))
            ++updated;

    return (updated > 0);
}

bool ModsLoader::setupWineDllOverride(BepInExConfig& config, const QDir& gameFolder)
{
    if (config.launcherType == LAUNCHER_NATIVE)
    {
        qDebug() << "Configuring Wine for native game...";
        QString newWineDllOverrides("winhttp=n,b");
        QString wineDllOverrides = qEnvironmentVariable("WINEDLLOVERRIDES");
        if (!wineDllOverrides.isEmpty())
        {
            qDebug().nospace() << "Current Wine DLL overrides is set to " << wineDllOverrides << ".";
            QStringList parts = wineDllOverrides.split(";");
            QStringList newParts;
            if (parts.count() > 0)
                for (QString& part : parts)
                {
                    QString current(part.replace(",winhttp,", ",").replace(",winhttp=", "="));
                    if (!current.startsWith("winhttp="))
                        newParts.push_back(current);
                }
            if (newParts.count() > 0)
                for (const QString& newPart : newParts)
                    newWineDllOverrides += (";" + newPart);
        }
        if (!qputenv("WINEDLLOVERRIDES", newWineDllOverrides.toLocal8Bit()))
        {
            qCritical().nospace() << "Failed to set Wine DLL overrides to " << newWineDllOverrides << ".";
            return false;
        }
        else
            qDebug().nospace() << "Successfully set Wine DLL overrides to " << newWineDllOverrides << ".";
        qDebug() << "Successfully configured Wine for native game.";
        return true;
    }
    else if (config.launcherType == LAUNCHER_STEAM)
    {
        qDebug() << "Configuring Wine for Steam game...";
        QString errorLogFilePath(Utils::toUnixPath(gameFolder.absolutePath() + QDir::separator() + RUN_YUMI_SCRIPT_NAME + ".err"));
        if (!configureSteamCommandLine(config, "WINEDLLOVERRIDES=\\\"winhttp=n,b\\\" %command%", "2>\\\"" + errorLogFilePath + "\\\""))
        {
            if (!config.configurationCanceled)
                qCritical() << "Failed to add Wine DLL override to additional arguments for Steam game.";
            return false;
        }
        qDebug() << "Successfully configured Wine for Steam game.";
        return true;
    }
    qCritical() << "Failed to add Wine DLL override (unknown game launcher).";
    return false;
}

bool ModsLoader::cleanWineDllOverride(BepInExConfig& config)
{
    if (config.launcherType == LAUNCHER_NATIVE)
    {
        qDebug() << "Cleaning Wine for native game...";
        QString newWineDllOverrides("");
        QString wineDllOverrides = qEnvironmentVariable("WINEDLLOVERRIDES");
        if (!wineDllOverrides.isEmpty())
        {
            qDebug().nospace() << "Current Wine DLL overrides is set to " << wineDllOverrides << ".";
            QStringList parts = wineDllOverrides.split(";");
            QStringList newParts;
            if (parts.count() > 0)
                for (QString& part : parts)
                {
                    QString current(part.replace(",winhttp,", ",").replace(",winhttp=", "="));
                    if (!current.startsWith("winhttp="))
                        newParts.push_back(current);
                }
            if (newParts.count() > 0)
                for (const QString& newPart : newParts)
                    newWineDllOverrides += (";" + newPart);
            if (!qputenv("WINEDLLOVERRIDES", newWineDllOverrides.toLocal8Bit()))
            {
                qCritical().nospace() << "Failed to set Wine DLL overrides to " << newWineDllOverrides << ".";
                return false;
            }
            else
                qDebug().nospace() << "Successfully set Wine DLL overrides to " << newWineDllOverrides << ".";
        }
        qDebug() << "Successfully cleaned Wine for native game.";
        return true;
    }
    else if (config.launcherType == LAUNCHER_STEAM)
    {
        qDebug() << "Cleaning Wine for Steam game...";
        if (!cleanSteamCommandLine(config))
        {
            if (!config.configurationCanceled)
                qCritical() << "Failed to clean Wine DLL additional arguments for Steam game.";
            return false;
        }
        qDebug() << "Successfully cleaned Wine for Steam game.";
        return true;
    }
    qCritical() << "Failed to clean Wine DLL override (unknown game launcher).";
    return false;
}

bool ModsLoader::setupBepInExScript(BepInExConfig& config, const QDir& gameFolder)
{
    qDebug() << "Configuring BepInEx script...";
    QString scriptPath(Utils::toUnixPath(config.game->path + QDir::separator() + RUN_YUMI_SCRIPT_NAME + ".sh"));
    QFile scriptFile(scriptPath);
    if (!scriptFile.exists())
    {
        qCritical().nospace() << "Unable to find script file at " << scriptPath << ".";
        return false;
    }
    if (!scriptFile.open(QIODevice::ReadOnly | QIODevice::ExistingOnly | QIODevice::Text))
    {
        qCritical().nospace() << "Unable to open script file at " << scriptPath << " for reading.";
        return false;
    }
    QString data;
    try { data = scriptFile.readAll(); }
    catch (const std::exception& ex) { scriptFile.close(); qCritical().nospace() << "Exception caught while reading BepInEx script. Exception=[" << ex.what() << "]"; return false; }
    catch (...) { scriptFile.close(); qCritical() << "Exception caught while reading BepInEx script."; return false; }
    scriptFile.close();
    if (data.isEmpty() || !data.contains("executable_name=\"\""))
    {
        qCritical().nospace() << "Unable to find \"executable_name\" variable in script file at " << scriptPath << ".";
        return false;
    }
    QString unixExePath(getUnixExePath(config.game->path));
    int lastSeparatorPos = unixExePath.lastIndexOf('/');
    QString unixExeName((lastSeparatorPos >= 0 && unixExePath.length() > lastSeparatorPos + 1) ? unixExePath.mid(lastSeparatorPos + 1) : unixExePath);

    QString edited(data.replace("executable_name=\"\"", "executable_name=\"" + unixExeName + "\""));
    if (!scriptFile.open(QIODevice::WriteOnly | QIODevice::ExistingOnly | QIODevice::Truncate | QIODevice::Text))
    {
        if (!scriptFile.setPermissions(scriptFile.permissions() | QFileDevice::WriteOwner | QFileDevice::WriteUser))
        {
            qCritical().nospace() << "Unable to get write permissions to script file at " << scriptPath << " for writing (permissions: " << scriptFile.permissions() << ").";
            return false;
        }
        if (!scriptFile.open(QIODevice::WriteOnly | QIODevice::ExistingOnly | QIODevice::Truncate | QIODevice::Text))
        {
            qCritical().nospace() << "Unable to open script file at " << scriptPath << " for writing (permissions: " << scriptFile.permissions() << ").";
            return false;
        }
    }
    if (!scriptFile.isWritable())
    {
        scriptFile.close();
        qCritical().nospace() << "Script file at " << scriptPath << " is not writable.";
        return false;
    }
    try
    {
        QTextStream ts(&scriptFile);
        ts << edited;
        ts.flush();
        scriptFile.close();
    }
    catch (const std::exception& ex)
    {
        scriptFile.close();
        qCritical().nospace() << "Exception caught while writing executable name inside BepInEx script. Exception=[" << ex.what() << "]";
        return false;
    }
    catch (...)
    {
        scriptFile.close();
        qCritical() << "Exception caught while writing executable name inside BepInEx script.";
        return false;
    }
    try
    {
        if (!scriptFile.setPermissions(scriptFile.permissions() | QFileDevice::ExeUser))
            qWarning().nospace() << "Unable to add user execution permissions to script file at " << scriptPath << ".";
    }
    catch (const std::exception& ex)
    {
        qWarning().nospace() << "Exception caught while making BepInEx script executable. Exception=[" << ex.what() << "]";
    }
    catch (...)
    {
        qWarning() << "Exception caught while making BepInEx script executable.";
    }
    if (config.launcherType == LAUNCHER_NATIVE)
    {
        qDebug() << "Successfully configured BepInEx script (simply run \"" + QString(RUN_YUMI_SCRIPT_NAME) + ".sh\" to start playing).";
        return true;
    }
    else if (config.launcherType == LAUNCHER_STEAM)
    {
        QString errorLogFilePath(Utils::toUnixPath(gameFolder.absolutePath() + QDir::separator() + RUN_YUMI_SCRIPT_NAME + ".err"));
        if (!configureSteamCommandLine(config, "\\\"" + scriptPath + "\\\" %command%", "2>\\\"" + errorLogFilePath + "\\\""))
        {
            if (!config.configurationCanceled)
                qCritical() << "Failed to add BepInEx script to additional arguments for Steam launcher.";
            return false;
        }
        qDebug() << "Successfully configured BepInEx script (just launch the game from Steam to start playing).";
        return true;
    }
    qCritical() << "Failed to configured BepInEx script (unknown game launcher).";
    return false;
}

bool ModsLoader::cleanBepInExScript(BepInExConfig& config)
{
    if (config.launcherType == LAUNCHER_NATIVE)
    {
        qDebug() << "Successfully cleaned BepInEx script.";
        return true;
    }
    else if (config.launcherType == LAUNCHER_STEAM)
    {
        if (!cleanSteamCommandLine(config))
        {
            if (!config.configurationCanceled)
                qCritical() << "Failed to clean BepInEx script from additional arguments for Steam launcher.";
            return false;
        }
        qDebug() << "Successfully cleaned BepInEx script.";
        return true;
    }
    qCritical() << "Failed to clean BepInEx script (unknown game launcher).";
    return false;
}

bool ModsLoader::configureBepInEx(BepInExConfig& config, const QDir& gameFolder)
{
    if (isWindowsOS())
    {
        if (isWineOS())
        {
            if (config.game->exeType == WIN_GAME_X64 || config.game->exeType == WIN_GAME_X86 || config.game->exeType == WIN_GAME_IL2CPP_X64 || config.game->exeType == WIN_GAME_IL2CPP_X86)
            {
                if (!setupWineDllOverride(config, gameFolder))
                {
                    if (!config.configurationCanceled)
                        qCritical() << "BepInEx installation error: Could not setup Wine DLL override.";
                    return false;
                }
            }
            else
            {
                qCritical() << "BepInEx installation error: You are trying to install BepInEx for a Unix game from a Windows operating system. Please run YUMI in your Unix emulator directly.";
                QMessageBox installError(QMessageBox::Critical, QCoreApplication::translate("MainWindow", "Installation error", "Popup title"), QCoreApplication::translate("MainWindow", "You are trying to install the mods loader for a Unix game from a Windows operating system. Please run YUMI in your Unix emulator directly.", "Popup text"), QMessageBox::Ok, (GameDetails*)config.gameDetails);
                installError.exec();
                return false;
            }
        }
        qDebug() << "Successfully configured BepInEx for Windows game.";
    }
    else
    {
        if (config.game->exeType == WIN_GAME_X64 || config.game->exeType == WIN_GAME_X86 || config.game->exeType == WIN_GAME_IL2CPP_X64 || config.game->exeType == WIN_GAME_IL2CPP_X86)
        {
            qCritical() << "BepInEx installation error: You are trying to install BepInEx for a Windows game from a Unix operating system. Please run YUMI in your Windows emulator directly.";
            QMessageBox installError(QMessageBox::Critical, QCoreApplication::translate("MainWindow", "Installation error", "Popup title"), QCoreApplication::translate("MainWindow", "You are trying to install the mods loader for a Windows game from a Unix operating system. Please run YUMI in your Windows emulator directly.", "Popup text"), QMessageBox::Ok, (GameDetails*)config.gameDetails);
            installError.exec();
            return false;
        }
        else
        {
            if (!setupBepInExScript(config, gameFolder))
            {
                if (!config.configurationCanceled)
                    qCritical() << "BepInEx installation error: Could not setup BepInEx script.";
                return false;
            }
        }
        qDebug() << "Successfully configured BepInEx for Unix game.";
    }
    return true;
}

bool ModsLoader::cleanBepInEx(BepInExConfig& config)
{
    if (isWindowsOS())
    {
        if (isWineOS())
        {
            if (config.game->exeType == WIN_GAME_X64 || config.game->exeType == WIN_GAME_X86 || config.game->exeType == WIN_GAME_IL2CPP_X64 || config.game->exeType == WIN_GAME_IL2CPP_X86)
            {
                if (!cleanWineDllOverride(config))
                {
                    if (!config.configurationCanceled)
                        qCritical() << "BepInEx cleaning error: Could not remove Wine DLL override.";
                    return false;
                }
            }
        }
        qDebug() << "Successfully cleaned BepInEx for Windows game.";
    }
    else
    {
        if (!(config.game->exeType == WIN_GAME_X64 || config.game->exeType == WIN_GAME_X86 || config.game->exeType == WIN_GAME_IL2CPP_X64 || config.game->exeType == WIN_GAME_IL2CPP_X86))
        {
            if (!cleanBepInExScript(config))
            {
                if (!config.configurationCanceled)
                    qCritical() << "BepInEx cleaning error: Could not remove BepInEx script.";
                return false;
            }
        }
        qDebug() << "Successfully cleaned BepInEx for Unix game.";
    }
    return true;
}

bool ModsLoader::copyRecursively(QString sourceFolder, QString destFolder, bool overwrite)
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

QString ModsLoader::extractModToTempFolder(QDir& yumiDir, const QString& modsFolderPath, const QString& modFileName)
{
    QString tempModsDirPath = Utils::toUnixPath(yumi::appPath + QDir::separator() + "temp_mods");
    QDir tempModsDir(tempModsDirPath);
    if (!tempModsDir.exists())
        if (!yumiDir.mkdir("temp_mods"))
        {
            qWarning().nospace() << "Could not create temporary mods folder at " << tempModsDirPath << ".";
            return "";
        }
    QString modFileNameNoExt = modFileName.left(modFileName.length() - 4);
    qInfo().nospace() << "Extracting mod " << modFileNameNoExt << "...";
    QString tempModDirPath(Utils::toUnixPath(tempModsDirPath + QDir::separator() + modFileNameNoExt));
    QDir tempModDir(tempModDirPath);
    try
    {
        if (!tempModDir.exists())
            if (!tempModsDir.mkdir(modFileNameNoExt))
            {
                qWarning().nospace() << "Could not create temporary mod folder at " << tempModDirPath << ".";
                return "";
            }

        QString modFilePath = Utils::toUnixPath(modsFolderPath + QDir::separator() + modFileName);
        if (!ZipfilesManager::Instance()->unzip(modFilePath, tempModDirPath))
        {
            qWarning().nospace() << "Failed to unzip mod " << modFilePath << ".";
            if (tempModDir.exists())
                if (!tempModDir.removeRecursively())
                    qWarning().nospace() << "Failed to remove temporary mod folder at " << tempModDirPath << ".";
            return "";
        }

        qInfo().nospace() << "Mod " << modFileNameNoExt << " has been extracted to temporary folder at " << tempModDirPath << ".";
        return tempModDirPath;
    }
    catch (const std::exception& ex)
    {
        qWarning().nospace() << "Exception caught while extracting mod " << modFileNameNoExt << " to temporary folder at " << tempModDirPath << " (Exception: " << ex.what() << ").";
    }
    catch (...)
    {
        qWarning().nospace() << "Exception caught while extracting mod" << modFileNameNoExt << " to temporary folder at " << tempModDirPath << ".";
    }
    if (tempModDir.exists())
        if (!tempModDir.removeRecursively())
            qWarning().nospace() << "Could not remove mod temporary folder " << tempModDirPath << ".";
    return "";
}

QString ModsLoader::moveModFolderToTempFolder(QDir& yumiDir, const QString& modsFolderPath, const QString& modFolderName)
{
    QString tempModsDirPath = Utils::toUnixPath(yumi::appPath + QDir::separator() + "temp_mods");
    QDir tempModsDir(tempModsDirPath);
    if (!tempModsDir.exists())
        if (!yumiDir.mkdir("temp_mods"))
        {
            qWarning().nospace() << "Could not create temporary mods folder at " << tempModsDirPath << ".";
            return "";
        }
    qInfo().nospace() << "Moving mod " << modFolderName << "...";
    QString tempModDirPath(Utils::toUnixPath(tempModsDirPath + QDir::separator() + modFolderName));
    QDir tempModDir(tempModDirPath);
    try
    {
        if (!tempModDir.exists())
            if (!tempModsDir.mkdir(modFolderName))
            {
                qWarning().nospace() << "Could not create temporary mod folder at " << tempModDirPath << ".";
                return "";
            }

        QString modFolderPathToMove = Utils::toUnixPath(modsFolderPath + QDir::separator() + modFolderName);
        if (!copyRecursively(modFolderPathToMove, tempModDirPath))
        {
            qWarning().nospace() << "Failed to move mod " << modFolderName << " from " << modFolderPathToMove << " to " << tempModDirPath << ".";
            return "";
        }

        qInfo().nospace() << "Mod " << modFolderName << " has been moved to temporary folder at " << tempModDirPath << ".";
        return tempModDirPath;
    }
    catch (const std::exception& ex)
    {
        qWarning().nospace() << "Exception caught while moving mod " << modFolderName << " to temporary folder at " << tempModDirPath << " (Exception: " << ex.what() << ").";
    }
    catch (...)
    {
        qWarning().nospace() << "Exception caught while moving mod " << modFolderName << " to temporary folder at " << tempModDirPath << ".";
    }
    return "";
}

QString ModsLoader::getActualTempModFolderPath(const QString& tempModDirPath, const QString& modName)
{
    QString actualModFolderPath(tempModDirPath);
    QDir tempModDir(tempModDirPath);
    QStringList modDirectories = tempModDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    if (modDirectories.count() == 1 && modDirectories[0].compare("plugins") != 0 && modDirectories[0].compare("patchers") != 0 && modDirectories[0].compare("config") != 0)
        actualModFolderPath = Utils::toUnixPath(tempModDirPath + QDir::separator() + modDirectories[0]);
    QDir actualModFolder(actualModFolderPath);
    if (actualModFolder.exists())
    {
        QStringList actualModDirectories = actualModFolder.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
        if (actualModDirectories.count() > 0 && (actualModDirectories.contains("plugins") || actualModDirectories.contains("patchers") || actualModDirectories.contains("config")))
            return actualModFolderPath;
        else
        {
            qWarning().nospace() << "Incorrect archive organisation for mod " << modName << ".";
            return "";
        }
    }
    else
    {
        qWarning().nospace() << "Wrong archive organisation for mod " << modName << ".";
        return "";
    }
}

void ModsLoader::removeTemporaryExtractionFolder()
{
    QString tempModsDirPath = Utils::toUnixPath(yumi::appPath + QDir::separator() + "temp_mods");
    QDir tempModsDir(tempModsDirPath);
    if (tempModsDir.exists())
        if (!tempModsDir.removeRecursively())
            qWarning().nospace() << "Unable to remove temporary mods extraction folder at " << tempModsDirPath << ".";
}

bool ModsLoader::installExtractedMod(const QString& extractedModFolderPath, const QString& modName, const QString& modsFolderPath, void* yumiPtr, bool isArchive)
{
    bool installed = false;
    QDir extractedModFolder(extractedModFolderPath);
    try
    {
        QString actualTempModFolderPath = getActualTempModFolderPath(extractedModFolderPath, modName);
        if (!actualTempModFolderPath.isEmpty())
        {
            QString modForGameName = "";
            QString modInfoFilePath = Utils::toUnixPath(actualTempModFolderPath + QDir::separator() + "mod_info.txt");
            QFile modInfoFile(modInfoFilePath);
            if (modInfoFile.exists())
                if (modInfoFile.open(QIODevice::ReadOnly))
                {
                    try { modForGameName = modInfoFile.readAll(); }
                    catch (const std::exception& ex) { modForGameName = ""; qWarning().nospace() << "Exception caught while reading mod info file (Exception: " << ex.what() << ")."; }
                    catch (...) { modForGameName = ""; qWarning() << "Exception caught while reading mod info file."; }
                    modInfoFile.close();
                }
            if (modForGameName.isEmpty())
            {
                QTimer timer;
                timer.setSingleShot(true);
                QEventLoop loop;
                SelectGameWindow* selectGame = new SelectGameWindow(yumiPtr, modName, NULL);
                selectGame->doShowAt(((yumi*)yumiPtr)->getCenter());
                QObject::connect(selectGame, SIGNAL(doClose()), &loop, SLOT(quit()));
                QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
                timer.start(900000);
                loop.exec();
                modForGameName = selectGame->selectedGame;
                if (selectGame->isVisible())
                    selectGame->close();
                delete selectGame;
            }
            if (modForGameName.isEmpty())
                qWarning().nospace() << "Could not identify for which game the mod " << modName << " is made. Skipping mod installation.";
            else
            {
                GameInfo* gi = NULL;
                int nbGames = ((yumi*)yumiPtr)->gamesInfo.count();
                for (int i = 0; i < nbGames; i++)
                    if (modForGameName.compare((((yumi*)yumiPtr)->gamesInfo)[i].name) == 0)
                    {
                        gi = &((((yumi*)yumiPtr)->gamesInfo)[i]);
                        break;
                    }
                if (gi == NULL)
                {
                    qWarning().nospace() << "Could not find the game for which the mod " << modName << " is made. Make sure to add the game " << modForGameName << " in YUMI, then restart YUMI to complete mod installation.";
                    if (extractedModFolder.exists())
                        if (!extractedModFolder.removeRecursively())
                            qWarning().nospace() << "Unable to remove temporary mod folder at " << extractedModFolderPath << ".";
                }
                else
                {
                    QDir actualTempModFolder(actualTempModFolderPath);
                    if (!actualTempModFolder.exists())
                        qWarning().nospace() << "Could not find the extracted mod folder for mod " << modName << " at " << actualTempModFolderPath << ".";
                    else
                    {
                        QStringList modDirectories = actualTempModFolder.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
                        int nbModDirectories = modDirectories.count();
                        if (nbModDirectories <= 0)
                            qWarning().nospace() << "The archive organisation is not correct for mod file " << modName << ".";
                        else
                        {
                            bool errorHappened = false;
                            for (int i = 0; i < nbModDirectories; i++)
                            {
                                if (modDirectories[i].compare("config") == 0)
                                {
                                    QString modConfigFolderPath = Utils::toUnixPath(actualTempModFolderPath + QDir::separator() + "config");
                                    QString gameBepInExConfigDirPath = Utils::toUnixPath(gi->path + "/BepInEx/config");
                                    if (!copyRecursively(modConfigFolderPath, gameBepInExConfigDirPath))
                                    {
                                        errorHappened = true;
                                        qWarning().nospace() << "Failed to copy some config files for mod " << modName << " from " << modConfigFolderPath << " to " << gameBepInExConfigDirPath << ".";
                                    }
                                }
                                else if (modDirectories[i].compare("patchers") == 0)
                                {
                                    QString modPatchersFolderPath = Utils::toUnixPath(actualTempModFolderPath + QDir::separator() + "patchers");
                                    QString gameBepInExPatchersDirPath = Utils::toUnixPath(gi->path + "/BepInEx/patchers");
                                    if (!copyRecursively(modPatchersFolderPath, gameBepInExPatchersDirPath))
                                    {
                                        errorHappened = true;
                                        qWarning().nospace() << "Failed to copy some patchers files for mod " << modName << " from " << modPatchersFolderPath << " to " << gameBepInExPatchersDirPath << ".";
                                    }
                                }
                                else if (modDirectories[i].compare("plugins") == 0)
                                {
                                    QString modPluginsFolderPath = Utils::toUnixPath(actualTempModFolderPath + QDir::separator() + "plugins");
                                    QString gameBepInExPluginsDirPath = Utils::toUnixPath(gi->path + "/BepInEx/plugins");
                                    if (!copyRecursively(modPluginsFolderPath, gameBepInExPluginsDirPath))
                                    {
                                        errorHappened = true;
                                        qWarning().nospace() << "Failed to copy some plugins files for mod " << modName << " from " << modPluginsFolderPath << " to " << gameBepInExPluginsDirPath << ".";
                                    }
                                }
                            }
                            if (!errorHappened)
                            {
                                qInfo().nospace() << "Mod " << modName << " has been installed successfully.";
                                installed = true;
                                if (isArchive)
                                {
                                    QFile modArchive = Utils::toUnixPath(modsFolderPath + QDir::separator() + modName + ".zip");
                                    if (modArchive.exists())
                                    {
                                        if (modArchive.remove())
                                            qInfo().nospace() << "Mod archive file " << (modName + ".zip") << " has been removed.";
                                        else
                                            qWarning().nospace() << "Unable to remove mod archive file " << (modName + ".zip") << ".";
                                    }
                                }
                                else
                                {
                                    QDir modFolder = Utils::toUnixPath(modsFolderPath + QDir::separator() + modName);
                                    if (modFolder.exists())
                                    {
                                        if (modFolder.removeRecursively())
                                            qInfo().nospace() << "Mod folder " << modName << " has been removed.";
                                        else
                                            qWarning().nospace() << "Unable to remove mod folder " << modName << ".";
                                    }
                                }
                            }
                            else
                                qWarning().nospace() << "An error happened during installation of " << modName << " mod.";
                        }
                    }
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        qWarning().nospace() << "Exception caught while installing mod " << modName << " (Exception: " << ex.what() << ").";
    }
    catch (...)
    {
        qWarning().nospace() << "Exception caught while installing mod " << modName << ".";
    }
    if (extractedModFolder.exists())
        if (!extractedModFolder.removeRecursively())
            qWarning().nospace() << "Unable to remove temporary mod folder at " << extractedModFolderPath << ".";
    return installed;
}

void ModsLoader::installMods(void* yumiPtr)
{
    installInProgress = true;

    QString modsFolderPath = Utils::toUnixPath(yumi::appPath + QDir::separator() + "mods");
    QDir modsFolder(modsFolderPath);
    if (!modsFolder.exists())
    {
        qInfo().nospace() << "No mods requiring installation were found.";
        return;
    }
    QDir yumiDir(yumi::appPath);
    if (!yumiDir.exists())
    {
        qCritical().nospace() << "YUMI folder not found.";
        return;
    }

    int nbModsInstalled = 0;

    QStringList modsList = modsFolder.entryList(QStringList() << "*.zip", QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    int nbCompressedModsToInstall = modsList.count();
    if (nbCompressedModsToInstall > 0)
    {
        qInfo().nospace() << "Found " << nbCompressedModsToInstall << " compressed mods to install.";
        foreach (const QString& modFileName, modsList)
        {
            QString modName = modFileName.left(modFileName.length() - 4);
            qInfo().nospace() << "Installing mod " << modName << "...";

            QString extractedModFolderPath;
            try { extractedModFolderPath = extractModToTempFolder(yumiDir, modsFolderPath, modFileName); }
            catch (const std::exception& ex)
            {
                extractedModFolderPath = "";
                qWarning().nospace() << "Exception caught while extracting mod " << modName << " (Exception: " << ex.what() << ").";
            }
            catch (...)
            {
                extractedModFolderPath = "";
                qWarning().nospace() << "Exception caught while extracting mod " << modName << ".";
            }
            if (!extractedModFolderPath.isEmpty())
                if (installExtractedMod(extractedModFolderPath, modName, modsFolderPath, yumiPtr, true))
                    nbModsInstalled++;
        }
    }
    else
        qInfo().nospace() << "No mods requiring extraction and installation were found in the \"mods\" folder.";

    modsList = modsFolder.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    QStringList validModFolders;
    foreach (const QString& modFolderName, modsList)
    {
        QDir currentModFolder(Utils::toUnixPath(modsFolderPath + QDir::separator() + modFolderName));
        if (currentModFolder.exists())
        {
            QStringList innerFolders = currentModFolder.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
            if (innerFolders.count() > 0 && (innerFolders.contains("config") || innerFolders.contains("patchers") || innerFolders.contains("plugins")))
                validModFolders.push_back(modFolderName);
        }
    }
    int nbModFoldersToInstall = validModFolders.count();
    if (nbModFoldersToInstall > 0)
    {
        qInfo().nospace() << "Found " << nbModFoldersToInstall << " mod folders to install.";
        foreach (const QString& modFolderName, validModFolders)
        {
            QString modName(modFolderName);
            qInfo().nospace() << "Installing mod " << modName << "...";

            QString movedModFolderPath;
            try { movedModFolderPath = moveModFolderToTempFolder(yumiDir, modsFolderPath, modFolderName); }
            catch (const std::exception& ex)
            {
                movedModFolderPath = "";
                qWarning().nospace() << "Exception caught while moving mod folder " << modName << " (Exception: " << ex.what() << ").";
            }
            catch (...)
            {
                movedModFolderPath = "";
                qWarning().nospace() << "Exception caught while moving mod folder " << modName << ".";
            }
            if (!movedModFolderPath.isEmpty())
                if (installExtractedMod(movedModFolderPath, modName, modsFolderPath, yumiPtr, false))
                    nbModsInstalled++;
        }
    }
    else
        qInfo().nospace() << "No mod folders requiring installation were found in the \"mods\" folder.";

    qInfo().nospace() << nbModsInstalled << " mods have been installed and " << ((nbCompressedModsToInstall + nbModFoldersToInstall) - nbModsInstalled) << " mods have been skipped.";

    removeTemporaryExtractionFolder();

    installInProgress = false;
}

bool ModsLoader::installBepInEx(void* yumiPtr, void* gameDetails, GameInfo* game, const bool forEveryone)
{
    if (yumiPtr == NULL || gameDetails == NULL || game == NULL)
    {
        qCritical() << "BepInEx installation error: Arguments are incorrect.";
        if (yumiPtr != NULL)
            ((yumi*)yumiPtr)->showStatusBarMsg(QCoreApplication::translate("MainWindow", "Mods loader was not installed.", "Tooltip text"));
        QMessageBox installFailed(QMessageBox::Critical, QCoreApplication::translate("MainWindow", "Install failed", "Popup title"), QCoreApplication::translate("MainWindow", "Mods loader installation failed: Provided arguments are incorrect. Please restart YUMI then try again. You can also get help on the Discord server.", "Popup text"), QMessageBox::Ok, (GameDetails*)gameDetails);
        installFailed.exec();
        return false;
    }
    if (game->name.isEmpty() || game->path.isEmpty())
    {
        qCritical() << "BepInEx installation error: Game name or folder path is empty.";
        ((yumi*)yumiPtr)->showStatusBarMsg(QCoreApplication::translate("MainWindow", "Mods loader was not installed.", "Tooltip text"));
        QMessageBox installFailed(QMessageBox::Critical, QCoreApplication::translate("MainWindow", "Install failed", "Popup title"), QCoreApplication::translate("MainWindow", "Mods loader installation failed: Game name or folder path is empty. Please remove the game from YUMI then add it again. You can also get help on the Discord server.", "Popup text"), QMessageBox::Ok, (GameDetails*)gameDetails);
        installFailed.exec();
        return false;
    }
    QDir gameFolder(game->path);
    if (!gameFolder.exists())
    {
        qCritical() << "BepInEx installation error: Could not find game folder.";
        ((yumi*)yumiPtr)->showStatusBarMsg(QCoreApplication::translate("MainWindow", "Mods loader was not installed.", "Tooltip text"));
        QMessageBox installFailed(QMessageBox::Critical, QCoreApplication::translate("MainWindow", "Install failed", "Popup title"), QCoreApplication::translate("MainWindow", "Mods loader installation failed for \"%1\": Unable to find game folder at \"%2\". Please try again, or remove the game from YUMI then add it again. You can also get help on the Discord server.", "Popup text").arg(game->name, game->path), QMessageBox::Ok, (GameDetails*)gameDetails);
        installFailed.exec();
        return false;
    }
    QList<BepInExFile*>* bepInExFiles = getBepInExFiles(game->exePath, game->exeType);
    if (bepInExFiles == NULL)
    {
        qCritical() << "BepInEx installation error: Could not retrieve BepInEx files from YUMI.";
        ((yumi*)yumiPtr)->showStatusBarMsg(QCoreApplication::translate("MainWindow", "Mods loader was not installed.", "Tooltip text"));
        QMessageBox installFailed(QMessageBox::Critical, QCoreApplication::translate("MainWindow", "Install failed", "Popup title"), QCoreApplication::translate("MainWindow", "Mods loader installation failed for \"%1\": Unable to retrieve mods loader files for executable \"%2\" (executable type: %3). You can get help on the Discord server.", "Popup text").arg(game->name, game->exePath, Config::Instance()->getExeTypeLabel(game->exeType)), QMessageBox::Ok, (GameDetails*)gameDetails);
        installFailed.exec();
        return false;
    }
    if (!copyBepInExFiles(gameFolder, game->path, *bepInExFiles))
    {
        qCritical().nospace() << "BepInEx installation error: Could not copy BepInEx files to game folder at " << game->path << ".";
        ((yumi*)yumiPtr)->showStatusBarMsg(QCoreApplication::translate("MainWindow", "Mods loader was not installed.", "Tooltip text"));
        QMessageBox installFailed(QMessageBox::Critical, QCoreApplication::translate("MainWindow", "Install failed", "Popup title"), QCoreApplication::translate("MainWindow", "Mods loader installation failed for \"%1\": Error when copying files to folder \"%2\". Please make sure YUMI has write permissions on the game folder then try again. You can also get help on the Discord server.", "Popup text").arg(game->name, game->path), QMessageBox::Ok, (GameDetails*)gameDetails);
        installFailed.exec();
        return false;
    }

    bool steamIsRunning = false;
    bool checkIfSteamRuns = true;
    if (!isSteamRunning())
    {
        QMessageBox pleaseStartSteam(QMessageBox::Information, QCoreApplication::translate("MainWindow", "Please launch Steam", "Popup title"), QCoreApplication::translate("MainWindow", "Please launch Steam then click on \"OK\" to continue installation.", "Popup text"), QMessageBox::Ok, (GameDetails*)gameDetails);
        pleaseStartSteam.exec();
    }
    else
    {
        steamIsRunning = true;
        checkIfSteamRuns = false;
    }
    while (checkIfSteamRuns)
    {
        if (!isSteamRunning())
        {
            QMessageBox continueInstall(QMessageBox::Information, QCoreApplication::translate("MainWindow", "Please launch Steam", "Popup title"), QCoreApplication::translate("MainWindow", "Steam is still not running! You need to launch Steam now to continue installation. Click on \"OK\" when Steam is launched (recommended), or click on \"Cancel\" to cancel mods loader configuration (not recommended).", "Popup text"), QMessageBox::Cancel | QMessageBox::Ok, (GameDetails*)gameDetails);
            int clickedBtn = continueInstall.exec();
            if (clickedBtn != QMessageBox::Ok)
            {
                qWarning() << "Mods loader configuration has been canceled.";
                QMessageBox configCanceled(QMessageBox::Information, QCoreApplication::translate("MainWindow", "Configuration canceled", "Popup title"), QCoreApplication::translate("MainWindow", "Mods loader will get installed but its configuration has been canceled. Please refer to YUMI's wiki to manually configure mods loader.", "Popup text"), QMessageBox::Ok, (GameDetails*)gameDetails);
                configCanceled.exec();
                checkIfSteamRuns = false;
            }
        }
        else
        {
            steamIsRunning = true;
            checkIfSteamRuns = false;
        }
    }
    int launcherType = getLauncherType(game->path);
    BepInExConfig config;
    config.gameDetails = gameDetails;
    config.game = game;
    config.forEveryone = forEveryone;
    config.launcherType = launcherType;
    config.steamRestarted = false;
    config.configurationCanceled = false;
    if (!configureBepInEx(config, gameFolder))
    {
        if (config.configurationCanceled)
            qInfo() << "BepInEx configuration has been canceled by user.";
        else
            qCritical() << "BepInEx configuration failed.";
        ((yumi*)yumiPtr)->showStatusBarMsg(QCoreApplication::translate("MainWindow", "Mods loader was not installed.", "Tooltip text"));
        QMessageBox installFailed(QMessageBox::Critical, QCoreApplication::translate("MainWindow", "Install failed", "Popup title"), QCoreApplication::translate("MainWindow", "Mods loader was not fully installed for \"%1\": Configuration failed. Please try again or refer to the wiki for manual configuration of the mods loader. You can also get help on the Discord server.", "Popup text").arg(game->name), QMessageBox::Ok, (GameDetails*)gameDetails);
        installFailed.exec();
        return false;
    }
    QMessageBox startGameToFinishInstall(QMessageBox::Information, QCoreApplication::translate("MainWindow", "Launch game now", "Popup title"), QCoreApplication::translate("MainWindow", "Please launch %1 to finish installation (if it's a Steam game, launch it from Steam).", "Popup text").arg(game->name), QMessageBox::Ok, (GameDetails*)gameDetails);
    startGameToFinishInstall.exec();
    bool checkPluginsFolderExists = true;
    while (checkPluginsFolderExists)
    {
        if (!pluginsFolderExists(gameFolder))
        {
            QMessageBox gameStillNotStarted(QMessageBox::Information, QCoreApplication::translate("MainWindow", "Launch game now", "Popup title"), QCoreApplication::translate("MainWindow", "It seems that you did not launched %1 yet. Please launch the game now to finish installation (if it's a Steam game, launch it from Steam) then click on \"OK\", or click on \"Cancel\" if you want to launch the game later.", "Popup text").arg(game->name), QMessageBox::Cancel | QMessageBox::Ok, (GameDetails*)gameDetails);
            int clickedBtn = gameStillNotStarted.exec();
            if (clickedBtn != QMessageBox::Ok)
            {
                checkPluginsFolderExists = false;
                qWarning() << "BepInEx installation is not completely finished (user asked to launch the game later).";
                QDir configFolder(Utils::toUnixPath(game->path + QDir::separator() + "BepInEx" + QDir::separator() + "config"));
                if (!configFolder.exists())
                    gameFolder.mkpath("BepInEx/config");
                QDir patchersFolder(Utils::toUnixPath(game->path + QDir::separator() + "BepInEx" + QDir::separator() + "patchers"));
                if (!patchersFolder.exists())
                    gameFolder.mkpath("BepInEx/patchers");
                QDir pluginsFolder(Utils::toUnixPath(game->path + QDir::separator() + "BepInEx" + QDir::separator() + "plugins"));
                if (!pluginsFolder.exists())
                    gameFolder.mkpath("BepInEx/plugins");
            }
        }
        else
        {
            checkPluginsFolderExists = false;
            qInfo() << "BepInEx installation has been completed successfully.";
        }
    }
    ((GameDetails*)gameDetails)->updateGameDetails(game);
    ((yumi*)yumiPtr)->showStatusBarMsg(QCoreApplication::translate("MainWindow", "Mods loader has been installed for \"%1\".", "Tooltip text").arg(game->name));
    QMessageBox modsLoaderInstalled(QMessageBox::Information, QCoreApplication::translate("MainWindow", "Mods loader installed", "Popup title"), QCoreApplication::translate("MainWindow", "Mods loader has been installed. You can now install mods (plugins/patchers) for \"%1\".", "Popup text").arg(game->name), QMessageBox::Ok, (GameDetails*)gameDetails);
    modsLoaderInstalled.exec();
    return true;
}

bool ModsLoader::uninstallBepInEx(void* yumiPtr, void* gameDetails, GameInfo* game, const bool includingBepInExConfigFile, const bool includingAllModsAndSavedData)
{
    if (yumiPtr == NULL || gameDetails == NULL || game == NULL)
    {
        qCritical() << "BepInEx uninstallation error: Arguments provided are incorrect.";
        if (yumiPtr != NULL)
            ((yumi*)yumiPtr)->showStatusBarMsg(QCoreApplication::translate("MainWindow", "Mods loader was not uninstalled.", "Tooltip text"));
        QMessageBox uninstallFailed(QMessageBox::Critical, QCoreApplication::translate("MainWindow", "Uninstall failed", "Popup title"), QCoreApplication::translate("MainWindow", "Mods loader was not uninstalled because provided arguments are not correct.", "Popup text"), QMessageBox::Ok, (GameDetails*)gameDetails);
        uninstallFailed.exec();
        return false;
    }
    if (game->name.isEmpty() || game->path.isEmpty())
    {
        qCritical() << "BepInEx uninstallation error: Game name or path is incorrect.";
        ((yumi*)yumiPtr)->showStatusBarMsg(QCoreApplication::translate("MainWindow", "Mods loader was not uninstalled.", "Tooltip text"));
        QMessageBox uninstallFailed(QMessageBox::Critical, QCoreApplication::translate("MainWindow", "Uninstall failed", "Popup title"), QCoreApplication::translate("MainWindow", "Mods loader was not uninstalled for \"%1\" because provided game name or path is incorrect.", "Popup text").arg(game->name), QMessageBox::Ok, (GameDetails*)gameDetails);
        uninstallFailed.exec();
        return false;
    }
    QDir gameFolder(game->path);
    if (!gameFolder.exists())
    {
        qCritical() << "BepInEx uninstallation error: Could not find game folder.";
        ((yumi*)yumiPtr)->showStatusBarMsg(QCoreApplication::translate("MainWindow", "Mods loader was not uninstalled.", "Tooltip text"));
        QMessageBox uninstallFailed(QMessageBox::Critical, QCoreApplication::translate("MainWindow", "Uninstall failed", "Popup title"), QCoreApplication::translate("MainWindow", "Mods loader was not uninstalled for \"%1\" because game folder was not found at \"%2\".", "Popup text").arg(game->name, game->path), QMessageBox::Ok, (GameDetails*)gameDetails);
        uninstallFailed.exec();
        return false;
    }
    int launcherType = getLauncherType(game->path);
    BepInExConfig config;
    config.gameDetails = gameDetails;
    config.game = game;
    config.forEveryone = true;
    config.launcherType = launcherType;
    config.steamRestarted = false;
    config.configurationCanceled = false;
    if (!cleanBepInEx(config))
    {
        if (config.configurationCanceled)
            qInfo() << "BepInEx cleaning has been canceled by user.";
        else
            qCritical() << "BepInEx cleaning failed.";
        ((yumi*)yumiPtr)->showStatusBarMsg(QCoreApplication::translate("MainWindow", "Steam cleaning failed.", "Tooltip text"));
        QMessageBox cleaningFailed(QMessageBox::Warning, QCoreApplication::translate("MainWindow", "Steam cleaning failed", "Popup title"), QCoreApplication::translate("MainWindow", "Removing \"%1\" additional arguments from Steam failed. Please verify that additional arguments have been removed in Steam. To do so, open \"%1\" properties in Steam and search the field \"Additional arguments\", then remove the content of this field. You can get additional help on the Discord server.", "Popup text").arg(game->name), QMessageBox::Ok, (GameDetails*)gameDetails);
        cleaningFailed.exec();
    }
    QList<BepInExFile*>* bepInExFiles = getBepInExFiles(game->exePath, game->exeType);
    if (bepInExFiles == NULL)
    {
        qCritical() << "BepInEx uninstallation error: Could not retrieve BepInEx files from YUMI.";
        ((yumi*)yumiPtr)->showStatusBarMsg(QCoreApplication::translate("MainWindow", "Mods loader was not uninstalled.", "Tooltip text"));
        QMessageBox uninstallFailed(QMessageBox::Critical, QCoreApplication::translate("MainWindow", "Uninstall failed", "Popup title"), QCoreApplication::translate("MainWindow", "Mods loader was not uninstalled for \"%1\" because it was not possible to identify mods loader files associated to the executable at \"%2\" (Executable type: %3).", "Popup text").arg(game->name, game->exePath, Config::Instance()->getExeTypeLabel(game->exeType)), QMessageBox::Ok, (GameDetails*)gameDetails);
        uninstallFailed.exec();
        return false;
    }
    if (!removeBepInExFiles(game->path, *bepInExFiles))
        qWarning().nospace() << "Some BepInEx files could not be removed from game folder at " << game->path << ".";
    QString bepInExCachePath(Utils::toUnixPath(game->path + QDir::separator() + "BepInEx" + QDir::separator() + "cache"));
    QDir bepInExCache(bepInExCachePath);
    if (!bepInExCache.exists())
        qWarning().nospace() << "BepInEx cache folder was not found at " << bepInExCachePath << ". Skipping BepInEx cache removal.";
    else
    {
        if (!bepInExCache.removeRecursively())
            qWarning().nospace() << "Failed to remove BepInEx cache folder at " << bepInExCachePath << ".";
        else
            qInfo().nospace() << "BepInEx cache folder at " << bepInExCachePath << " has been removed.";
    }
    if (includingBepInExConfigFile)
    {
        QString configFilePath(Utils::toUnixPath(game->path + QDir::separator() + "BepInEx" + QDir::separator() + "config" + QDir::separator() + "BepInEx.cfg"));
        QFile configFile(configFilePath);
        if (!configFile.exists())
            qWarning().nospace() << "BepInEx config file was not found at " << configFilePath << ". Skipping BepInEx config file removal.";
        else
        {
            if (!configFile.remove())
                qWarning().nospace() << "Failed to remove BepInEx config file at " << configFilePath << " (Error message: " << configFile.errorString() << ").";
            else
                qInfo().nospace() << "BepInEx config file at " << configFilePath << " has been removed.";
        }
    }
    if (includingAllModsAndSavedData)
    {
        QString configFolderPath(Utils::toUnixPath(game->path + QDir::separator() + "BepInEx" + QDir::separator() + "config"));
        QDir configFolder(configFolderPath);
        if (!configFolder.exists())
            qWarning().nospace() << "BepInEx config folder was not found at " << configFolderPath << ". Skipping removal.";
        else
        {
            if (!configFolder.removeRecursively())
                qWarning().nospace() << "Failed to remove BepInEx config folder at " << configFolderPath << ".";
            else
                qInfo().nospace() << "BepInEx config folder at " << configFolderPath << " has been removed.";
        }
        QString patchersFolderPath(Utils::toUnixPath(game->path + QDir::separator() + "BepInEx" + QDir::separator() + "patchers"));
        QDir patchersFolder(patchersFolderPath);
        if (!patchersFolder.exists())
            qWarning().nospace() << "BepInEx patchers folder was not found at " << patchersFolderPath << ". Skipping removal.";
        else
        {
            if (!patchersFolder.removeRecursively())
                qWarning().nospace() << "Failed to remove BepInEx patchers folder at " << patchersFolderPath << ".";
            else
                qInfo().nospace() << "BepInEx patchers folder at " << patchersFolderPath << " has been removed.";
        }
        QString pluginsFolderPath(Utils::toUnixPath(game->path + QDir::separator() + "BepInEx" + QDir::separator() + "plugins"));
        QDir pluginsFolder(pluginsFolderPath);
        if (!pluginsFolder.exists())
            qWarning().nospace() << "BepInEx plugins folder was not found at " << pluginsFolderPath << ". Skipping removal.";
        else
        {
            if (!pluginsFolder.removeRecursively())
                qWarning().nospace() << "Failed to remove BepInEx plugins folder at " << pluginsFolderPath << ".";
            else
                qInfo().nospace() << "BepInEx plugins folder at " << pluginsFolderPath << " has been removed.";
        }
        QString disabledPatchersFolderPath(Utils::toUnixPath(game->path + QDir::separator() + "BepInEx" + QDir::separator() + "patchers"));
        QDir disabledPatchersFolder(disabledPatchersFolderPath);
        if (!disabledPatchersFolder.exists())
            qInfo().nospace() << "Disabled patchers folder was not found at " << disabledPatchersFolderPath << ". Skipping removal.";
        else
        {
            if (!disabledPatchersFolder.removeRecursively())
                qWarning().nospace() << "Failed to remove disabled patchers folder at " << disabledPatchersFolderPath << ".";
            else
                qInfo().nospace() << "Disabled patchers folder at " << disabledPatchersFolderPath << " has been removed.";
        }
        QString disabledPluginsFolderPath(Utils::toUnixPath(game->path + QDir::separator() + "BepInEx" + QDir::separator() + "plugins"));
        QDir disabledPluginsFolder(disabledPatchersFolderPath);
        if (!disabledPluginsFolder.exists())
            qInfo().nospace() << "Disabled plugins folder was not found at " << disabledPluginsFolderPath << ". Skipping removal.";
        else
        {
            if (!disabledPluginsFolder.removeRecursively())
                qWarning().nospace() << "Failed to remove disabled plugins folder at " << disabledPluginsFolderPath << ".";
            else
                qInfo().nospace() << "Disabled plugins folder at " << disabledPluginsFolderPath << " has been removed.";
        }
    }
    removeEmptyFolder(Utils::toUnixPath(game->path + "/BepInEx/config"));
    removeEmptyFolder(Utils::toUnixPath(game->path + "/BepInEx/core"));
    removeEmptyFolder(Utils::toUnixPath(game->path + "/BepInEx/patchers"));
    removeEmptyFolder(Utils::toUnixPath(game->path + "/BepInEx/plugins"));
    removeEmptyFolder(Utils::toUnixPath(game->path + "/BepInEx/disabled_patchers"));
    removeEmptyFolder(Utils::toUnixPath(game->path + "/BepInEx/disabled_plugins"));
    removeEmptyFolder(Utils::toUnixPath(game->path + "/BepInEx"));
    QString gameName(game->name);
    ((GameDetails*)gameDetails)->updateGameDetails(game);
    qInfo().nospace() << "BepInEx has been uninstalled for " << gameName << ".";
    ((yumi*)yumiPtr)->showStatusBarMsg(QCoreApplication::translate("MainWindow", "Mods loader has been uninstalled for \"%1\".", "Tooltip text").arg(gameName));
    QMessageBox modsLoaderUninstalled(QMessageBox::Information, QCoreApplication::translate("MainWindow", "Mods loader uninstalled", "Popup title"), QCoreApplication::translate("MainWindow", "Mods loader has been uninstalled for \"%1\".", "Popup text").arg(gameName), QMessageBox::Ok, (GameDetails*)gameDetails);
    modsLoaderUninstalled.exec();
    return true;
}
