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
#include <QApplication>
#include <QDir>
#include <QTimer>
#include <QEventLoop>
#include <QMessageBox>
#include "assets.h"
#include "common.h"
#include "yumi.h"
#include "utils.h"
#include "zipfiles_manager.h"
#include "downloading_window.h"

Assets* Assets::_instance = NULL;
QString Assets::primaryFontFamily = "Gill Sans MT";

Assets::Assets(QObject* parent) : QObject(parent)
{
    this->_yumiPtr = (void*)parent;
    this->_unixBepInEx = NULL;
    this->_x64BepInEx = NULL;
    this->_x86BepInEx = NULL;
    this->_il2cppx64BepInEx = NULL;
    this->_il2cppx86BepInEx = NULL;
    this->_fileDownloader = NULL;
}

Assets::~Assets()
{
    if (this->_unixBepInEx != NULL)
    {
        this->_unixBepInEx->clear();
        delete this->_unixBepInEx;
        this->_unixBepInEx = NULL;
    }
    if (this->_x64BepInEx != NULL)
    {
        this->_x64BepInEx->clear();
        delete this->_x64BepInEx;
        this->_x64BepInEx = NULL;
    }
    if (this->_x86BepInEx != NULL)
    {
        this->_x86BepInEx->clear();
        delete this->_x86BepInEx;
        this->_x86BepInEx = NULL;
    }
    if (this->_il2cppx64BepInEx != NULL)
    {
        this->_il2cppx64BepInEx->clear();
        delete this->_il2cppx64BepInEx;
        this->_il2cppx64BepInEx = NULL;
    }
    if (this->_il2cppx86BepInEx != NULL)
    {
        this->_il2cppx86BepInEx->clear();
        delete this->_il2cppx86BepInEx;
        this->_il2cppx86BepInEx = NULL;
    }
}

Assets* Assets::Init(QObject* parent)
{
    if (_instance == NULL)
        _instance = new Assets(parent);
    return _instance;
}

Assets* Assets::Instance()
{
    return _instance;
}

bool Assets::extractBepInExArchive(const QString& bepInExName, const QString& downloadsFolderPath)
{
    QDir downloadsFolder(downloadsFolderPath);
    if (!downloadsFolder.exists())
    {
        QDir yumiFolder(yumi::appPath);
        if (!yumiFolder.mkdir("downloads"))
            qWarning().nospace() << "Unable to create folder " << downloadsFolderPath << ".";
    }
    if (!downloadsFolder.exists())
        return false;
    QString outputFilePath(Utils::toUnixPath(downloadsFolderPath + QDir::separator() + bepInExName + ".zip"));
    QFile outputFile(outputFilePath);
    if (!outputFile.open(QIODevice::WriteOnly))
    {
        qWarning().nospace() << "Could not open file " << outputFilePath << " for writing (Error message: " << outputFile.errorString() << ").";
        return false;
    }
    bool zipFileClosed = false;
    bool extracted = false;
    try
    {
        outputFile.write(_fileDownloader->downloadedData());
        QFile bepInExArchive(outputFilePath);
        if (bepInExArchive.exists())
        {
            QString oldBepInExFolderPath(Utils::toUnixPath(downloadsFolderPath + QDir::separator() + bepInExName));
            QDir oldBepInExFolder(oldBepInExFolderPath);
            if (oldBepInExFolder.exists())
                if (!oldBepInExFolder.removeRecursively())
                    qWarning() << "Failed to delete old BepInEx folder at " << oldBepInExFolderPath << ".";
            if (ZipfilesManager::Instance()->unzip(outputFilePath, downloadsFolderPath))
            {
                extracted = true;
                qInfo().nospace() << "Successfully extracted BepInEx folder " << bepInExName << " to " << downloadsFolderPath << ".";
                outputFile.close();
                zipFileClosed = true;
            }
            else
                qWarning().nospace() << "Failed to extract archive " << outputFilePath << " in folder " << downloadsFolderPath << ".";
            if (!bepInExArchive.remove())
                qWarning().nospace() << "Failed to remove downloaded BepInEx archive at " << outputFilePath << " to save disk space.";
        }
        else
            qWarning().nospace() << "Failed to write BepInEx archive to " << outputFilePath << ".";
    }
    catch (const std::exception& ex)
    {
        qWarning().nospace() << "Exception caught while extracting BepInEx archive (Exception: " << ex.what() << ").";
    }
    catch (...)
    {
        qWarning() << "Exception caught while extracting BepInEx archive.";
    }
    if (!zipFileClosed)
        outputFile.close();
    return extracted;
}

bool Assets::downloadBepInExArchive(const QString& bepInExName, const QString& downloadsFolderPath)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    DownloadingWindow* downloading = new DownloadingWindow(NULL);
    downloading->doShowAt(((yumi*)_yumiPtr)->getCenter());

    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
#if IS_DEVELOPMENT
    QString downloadBepInExUrl(QString(DOWNLOAD_BEPINEX_URL) + bepInExName + "_dev.zip");
#else
    QString downloadBepInExUrl(QString(DOWNLOAD_BEPINEX_URL) + bepInExName + ".zip");
#endif
    _fileDownloader = new YumiFileDownloader(downloadBepInExUrl, this);
    connect(_fileDownloader, SIGNAL(downloaded()), &loop, SLOT(quit()));
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    timer.start(1000 * Config::Instance()->downloadRequestTimeout);
    loop.exec();
    if (timer.isActive())
    {
        if (_fileDownloader->errorMsg().isEmpty())
        {
            bool res = extractBepInExArchive(bepInExName, downloadsFolderPath);
            QApplication::restoreOverrideCursor();
            if (downloading->isVisible())
                downloading->close();
            delete downloading;
            delete _fileDownloader;
            _fileDownloader = NULL;
            return res;
        }
        else
        {
            QApplication::restoreOverrideCursor();
            if (downloading->isVisible())
                downloading->close();
            delete downloading;
            qWarning().nospace() << "Unable to download BepInEx from " << downloadBepInExUrl << ". Error message: " << _fileDownloader->errorMsg();
            QMessageBox downloadFailed = QMessageBox(QMessageBox::Warning, QCoreApplication::translate("Assets", "Download failed", "Popup title"), QCoreApplication::translate("Assets", "Unable to download mods loader from \"%1\" (Error message: \"%2\").", "Popup text").arg(downloadBepInExUrl, _fileDownloader->errorMsg()), QMessageBox::Ok);
            downloadFailed.exec();
        }
    }
    else
    {
        QApplication::restoreOverrideCursor();
        if (downloading->isVisible())
            downloading->close();
        delete downloading;
        qWarning().nospace() << "Unable to download BepInEx from " << downloadBepInExUrl << ". Check internet connection or increase download timeout in settings.";
        QMessageBox downloadFailed = QMessageBox(QMessageBox::Warning, QCoreApplication::translate("Assets", "Download failed", "Popup title"), QCoreApplication::translate("Assets", "Unable to download mods loader from \"%1\". Please check your internet connection or try to increase download timeout in settings.", "Popup text").arg(downloadBepInExUrl), QMessageBox::Ok);
        downloadFailed.exec();
    }
    delete _fileDownloader;
    _fileDownloader = NULL;
    return false;
}

bool Assets::isBepInExUpToDate(const QString& bepInExName, const QString& bepInExFolderPath)
{
    QDir bepInExFolder(bepInExFolderPath);
    if (!bepInExFolder.exists())
    {
        qWarning().nospace() << "Unable to find BepInEx folder at " << bepInExFolderPath << ".";
        return false;
    }

    QString bepInExVersionFilePath(Utils::toUnixPath(bepInExFolderPath + "/.yumi_bepinex_version"));
    QFile bepInExVersionFile(bepInExVersionFilePath);
    if (bepInExVersionFile.exists())
    {
        if (bepInExVersionFile.open(QIODevice::ReadOnly))
        {
            QString data;
            try { data = bepInExVersionFile.readAll(); }
            catch (const std::exception& ex) { data = ""; qWarning().nospace() << "Exception caught while reading BepInEx version file (Exception: " << ex.what() << ")."; }
            catch (...) { data = ""; qWarning() << "Exception caught while reading BepInEx version file."; }
            bepInExVersionFile.close();

            if (data.isEmpty())
            {
                qWarning().nospace() << "Unable to get current BepInEx version from file at " << bepInExVersionFilePath << ".";
                return false;
            }

            QString latestVersion = YumiNetwork::Instance()->sendGetRequestSync(QString(YUMI_BEPINEX_VERSION_CHECK_URL) + "?bepinex=" + bepInExName);
            if (latestVersion.startsWith("ERROR: "))
            {
                qWarning().nospace() << "Failed to check latest BepInEx version (" << latestVersion << ").";
                QMessageBox checkLatestVersionFailed = QMessageBox(QMessageBox::Warning, QCoreApplication::translate("Assets", "Mods loader version check failed", "Popup title"), QCoreApplication::translate("Assets", "Unable to check if your mods loader version is up to date. Please check your internet connection. YUMI will now try to download latest mods loader version.", "Popup text"), QMessageBox::Ok);
                checkLatestVersionFailed.exec();
                return false;
            }
            if (latestVersion.isEmpty())
            {
                qWarning() << "Unable to get latest BepInEx version ID (received empty string from web server).";
                return false;
            }
            if (data.compare(latestVersion) == 0)
            {
                qInfo() << "BepInEx version is up-to-date.";
                return true;
            }
        }
        else
            qWarning().nospace() << "Unable to open file " << bepInExVersionFilePath << " for reading (Error message: " << bepInExVersionFile.errorString() << ").";
    }
    else
        qWarning().nospace() << "BepInEx version file not found at " << bepInExVersionFilePath << ".";
    return false;
}

bool Assets::getLocalBepInEx(const QString& bepInExName, const QString& bepInExFolderPath, const QString& downloadsFolderPath)
{
    if (isBepInExUpToDate(bepInExName, bepInExFolderPath))
        return true;
    if (downloadBepInExArchive(bepInExName, downloadsFolderPath))
        return true;
    else
    {
        QString bepInExArchiveName(bepInExName + ".zip");
        qWarning().nospace() << "Unable to download and extract latest BepInEx archive " << bepInExArchiveName << " to folder " << downloadsFolderPath << ".";
        QMessageBox downloadAndExtractFailed = QMessageBox(QMessageBox::Warning, QCoreApplication::translate("Assets", "Download failed", "Popup title"), QCoreApplication::translate("Assets", "Unable to download and extract BepInEx archive \"%1\" to folder \"%2\". Check \"YUMI_Output\" log file for more info.", "Popup text").arg(bepInExArchiveName, downloadsFolderPath), QMessageBox::Ok);
        downloadAndExtractFailed.exec();
        return false;
    }
}

QList<BepInExFile*>* Assets::getUnixFiles()
{
    if (_unixBepInEx == NULL)
    {
        QString downloadsFolderPath(Utils::toUnixPath(yumi::appPath + "/downloads"));
        QString bepInExFolderPath(Utils::toUnixPath(downloadsFolderPath + QDir::separator() + UNIX_BEPINEX_NAME));

        if (getLocalBepInEx(UNIX_BEPINEX_NAME, bepInExFolderPath, downloadsFolderPath))
        {
            _unixBepInEx = new QList<BepInExFile*>();

            _unixBepInEx->append(new BepInExFile("0Harmony.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/0Harmony.dll")));
            _unixBepInEx->append(new BepInExFile("0Harmony.xml", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/0Harmony.xml")));
            _unixBepInEx->append(new BepInExFile("0Harmony20.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/0Harmony20.dll")));
            _unixBepInEx->append(new BepInExFile("BepInEx.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.dll")));
            _unixBepInEx->append(new BepInExFile("BepInEx.Harmony.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.Harmony.dll")));
            _unixBepInEx->append(new BepInExFile("BepInEx.Harmony.xml", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.Harmony.xml")));
            _unixBepInEx->append(new BepInExFile("BepInEx.Preloader.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.Preloader.dll")));
            _unixBepInEx->append(new BepInExFile("BepInEx.Preloader.xml", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.Preloader.xml")));
            _unixBepInEx->append(new BepInExFile("BepInEx.xml", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.xml")));
            _unixBepInEx->append(new BepInExFile("HarmonyXInterop.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/HarmonyXInterop.dll")));
            _unixBepInEx->append(new BepInExFile("Mono.Cecil.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Mono.Cecil.dll")));
            _unixBepInEx->append(new BepInExFile("Mono.Cecil.Mdb.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Mono.Cecil.Mdb.dll")));
            _unixBepInEx->append(new BepInExFile("Mono.Cecil.Pdb.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Mono.Cecil.Pdb.dll")));
            _unixBepInEx->append(new BepInExFile("Mono.Cecil.Rocks.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Mono.Cecil.Rocks.dll")));
            _unixBepInEx->append(new BepInExFile("MonoMod.RuntimeDetour.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/MonoMod.RuntimeDetour.dll")));
            _unixBepInEx->append(new BepInExFile("MonoMod.RuntimeDetour.xml", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/MonoMod.RuntimeDetour.xml")));
            _unixBepInEx->append(new BepInExFile("MonoMod.Utils.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/MonoMod.Utils.dll")));
            _unixBepInEx->append(new BepInExFile("MonoMod.Utils.xml", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/MonoMod.Utils.xml")));
            _unixBepInEx->append(new BepInExFile("libdoorstop_x64.dylib", "doorstop_libs", new QFile(bepInExFolderPath + "/doorstop_libs/libdoorstop_x64.dylib")));
            _unixBepInEx->append(new BepInExFile("libdoorstop_x64.so", "doorstop_libs", new QFile(bepInExFolderPath + "/doorstop_libs/libdoorstop_x64.so")));
            _unixBepInEx->append(new BepInExFile("libdoorstop_x86.dylib", "doorstop_libs", new QFile(bepInExFolderPath + "/doorstop_libs/libdoorstop_x86.dylib")));
            _unixBepInEx->append(new BepInExFile("libdoorstop_x86.so", "doorstop_libs", new QFile(bepInExFolderPath + "/doorstop_libs/libdoorstop_x86.so")));
            _unixBepInEx->append(new BepInExFile("run_bepinex.sh", "", new QFile(bepInExFolderPath + "/run_bepinex.sh")));
            _unixBepInEx->append(new BepInExFile("run_yumi_bepinex.sh", "", new QFile(bepInExFolderPath + "/run_yumi_bepinex.sh")));
            _unixBepInEx->append(new BepInExFile(".yumi_bepinex_version", "", new QFile(bepInExFolderPath + "/.yumi_bepinex_version")));
        }
    }
    return _unixBepInEx;
}

QList<BepInExFile*>* Assets::getX64Files()
{
    if (_x64BepInEx == NULL)
    {
        QString downloadsFolderPath(Utils::toUnixPath(yumi::appPath + "/downloads"));
        QString bepInExFolderPath(Utils::toUnixPath(downloadsFolderPath + QDir::separator() + WINX64_BEPINEX_NAME));

        if (getLocalBepInEx(WINX64_BEPINEX_NAME, bepInExFolderPath, downloadsFolderPath))
        {
            _x64BepInEx = new QList<BepInExFile*>();

            _x64BepInEx->append(new BepInExFile("0Harmony.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/0Harmony.dll")));
            _x64BepInEx->append(new BepInExFile("0Harmony.xml", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/0Harmony.xml")));
            _x64BepInEx->append(new BepInExFile("0Harmony20.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/0Harmony20.dll")));
            _x64BepInEx->append(new BepInExFile("BepInEx.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.dll")));
            _x64BepInEx->append(new BepInExFile("BepInEx.Harmony.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.Harmony.dll")));
            _x64BepInEx->append(new BepInExFile("BepInEx.Harmony.xml", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.Harmony.xml")));
            _x64BepInEx->append(new BepInExFile("BepInEx.Preloader.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.Preloader.dll")));
            _x64BepInEx->append(new BepInExFile("BepInEx.Preloader.xml", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.Preloader.xml")));
            _x64BepInEx->append(new BepInExFile("BepInEx.xml", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.xml")));
            _x64BepInEx->append(new BepInExFile("HarmonyXInterop.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/HarmonyXInterop.dll")));
            _x64BepInEx->append(new BepInExFile("Mono.Cecil.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Mono.Cecil.dll")));
            _x64BepInEx->append(new BepInExFile("Mono.Cecil.Mdb.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Mono.Cecil.Mdb.dll")));
            _x64BepInEx->append(new BepInExFile("Mono.Cecil.Pdb.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Mono.Cecil.Pdb.dll")));
            _x64BepInEx->append(new BepInExFile("Mono.Cecil.Rocks.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Mono.Cecil.Rocks.dll")));
            _x64BepInEx->append(new BepInExFile("MonoMod.RuntimeDetour.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/MonoMod.RuntimeDetour.dll")));
            _x64BepInEx->append(new BepInExFile("MonoMod.RuntimeDetour.xml", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/MonoMod.RuntimeDetour.xml")));
            _x64BepInEx->append(new BepInExFile("MonoMod.Utils.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/MonoMod.Utils.dll")));
            _x64BepInEx->append(new BepInExFile("MonoMod.Utils.xml", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/MonoMod.Utils.xml")));
            _x64BepInEx->append(new BepInExFile("doorstop_config.ini", "", new QFile(bepInExFolderPath + "/doorstop_config.ini")));
            _x64BepInEx->append(new BepInExFile("winhttp.dll", "", new QFile(bepInExFolderPath + "/winhttp.dll")));
            _x64BepInEx->append(new BepInExFile(".yumi_bepinex_version", "", new QFile(bepInExFolderPath + "/.yumi_bepinex_version")));
        }
    }
    return _x64BepInEx;
}

QList<BepInExFile*>* Assets::getX86Files()
{
    if (_x86BepInEx == NULL)
    {
        QString downloadsFolderPath(Utils::toUnixPath(yumi::appPath + "/downloads"));
        QString bepInExFolderPath(Utils::toUnixPath(downloadsFolderPath + QDir::separator() + WINX86_BEPINEX_NAME));

        if (getLocalBepInEx(WINX86_BEPINEX_NAME, bepInExFolderPath, downloadsFolderPath))
        {
            _x86BepInEx = new QList<BepInExFile*>();

            _x86BepInEx->append(new BepInExFile("0Harmony.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/0Harmony.dll")));
            _x86BepInEx->append(new BepInExFile("0Harmony.xml", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/0Harmony.xml")));
            _x86BepInEx->append(new BepInExFile("0Harmony20.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/0Harmony20.dll")));
            _x86BepInEx->append(new BepInExFile("BepInEx.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.dll")));
            _x86BepInEx->append(new BepInExFile("BepInEx.Harmony.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.Harmony.dll")));
            _x86BepInEx->append(new BepInExFile("BepInEx.Harmony.xml", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.Harmony.xml")));
            _x86BepInEx->append(new BepInExFile("BepInEx.Preloader.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.Preloader.dll")));
            _x86BepInEx->append(new BepInExFile("BepInEx.Preloader.xml", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.Preloader.xml")));
            _x86BepInEx->append(new BepInExFile("BepInEx.xml", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.xml")));
            _x86BepInEx->append(new BepInExFile("HarmonyXInterop.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/HarmonyXInterop.dll")));
            _x86BepInEx->append(new BepInExFile("Mono.Cecil.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Mono.Cecil.dll")));
            _x86BepInEx->append(new BepInExFile("Mono.Cecil.Mdb.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Mono.Cecil.Mdb.dll")));
            _x86BepInEx->append(new BepInExFile("Mono.Cecil.Pdb.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Mono.Cecil.Pdb.dll")));
            _x86BepInEx->append(new BepInExFile("Mono.Cecil.Rocks.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Mono.Cecil.Rocks.dll")));
            _x86BepInEx->append(new BepInExFile("MonoMod.RuntimeDetour.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/MonoMod.RuntimeDetour.dll")));
            _x86BepInEx->append(new BepInExFile("MonoMod.RuntimeDetour.xml", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/MonoMod.RuntimeDetour.xml")));
            _x86BepInEx->append(new BepInExFile("MonoMod.Utils.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/MonoMod.Utils.dll")));
            _x86BepInEx->append(new BepInExFile("MonoMod.Utils.xml", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/MonoMod.Utils.xml")));
            _x86BepInEx->append(new BepInExFile("doorstop_config.ini", "", new QFile(bepInExFolderPath + "/doorstop_config.ini")));
            _x86BepInEx->append(new BepInExFile("winhttp.dll", "", new QFile(bepInExFolderPath + "/winhttp.dll")));
            _x86BepInEx->append(new BepInExFile(".yumi_bepinex_version", "", new QFile(bepInExFolderPath + "/.yumi_bepinex_version")));
        }
        else
        {
            QString bepInExArchiveName(QString(WINX86_BEPINEX_NAME) + ".zip");
            qWarning().nospace() << "Unable to download and extract BepInEx archive " << bepInExArchiveName << " to folder " << downloadsFolderPath << ".";
            QMessageBox downloadAndExtractFailed = QMessageBox(QMessageBox::Warning, QCoreApplication::translate("Assets", "Download failed", "Popup title"), QCoreApplication::translate("Assets", "Unable to download and extract BepInEx archive \"%1\" to folder \"%2\". Check \"YUMI_Output\" log file for more info.", "Popup text").arg(bepInExArchiveName, downloadsFolderPath), QMessageBox::Ok);
            downloadAndExtractFailed.exec();
        }
    }
    return _x86BepInEx;
}

QList<BepInExFile*>* Assets::getIL2CPPX64Files()
{
    if (_il2cppx64BepInEx == NULL)
    {
        QString downloadsFolderPath(Utils::toUnixPath(yumi::appPath + "/downloads"));
        QString bepInExFolderPath(Utils::toUnixPath(downloadsFolderPath + QDir::separator() + IL2CPPX64_BEPINEX_NAME));

        if (getLocalBepInEx(IL2CPPX64_BEPINEX_NAME, bepInExFolderPath, downloadsFolderPath))
        {
            _il2cppx64BepInEx = new QList<BepInExFile*>();

            _il2cppx64BepInEx->append(new BepInExFile("0Harmony.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/0Harmony.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("AsmResolver.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/AsmResolver.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("AsmResolver.DotNet.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/AsmResolver.DotNet.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("AsmResolver.PE.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/AsmResolver.PE.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("AsmResolver.PE.File.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/AsmResolver.PE.File.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("AssetRipper.VersionUtilities.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/AssetRipper.VersionUtilities.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("BepInEx.Core.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.Core.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("BepInEx.Preloader.Core.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.Preloader.Core.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("BepInEx.Unity.Common.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.Unity.Common.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("BepInEx.Unity.IL2CPP.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.Unity.IL2CPP.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("BepInEx.Unity.IL2CPP.dll.config", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.Unity.IL2CPP.dll.config")));
            _il2cppx64BepInEx->append(new BepInExFile("Cpp2IL.Core.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Cpp2IL.Core.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Disarm.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Disarm.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("dobby.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/dobby.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Gee.External.Capstone.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Gee.External.Capstone.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Iced.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Iced.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Il2CppInterop.Common.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Il2CppInterop.Common.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Il2CppInterop.Generator.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Il2CppInterop.Generator.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Il2CppInterop.HarmonySupport.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Il2CppInterop.HarmonySupport.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Il2CppInterop.Runtime.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Il2CppInterop.Runtime.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("LibCpp2IL.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/LibCpp2IL.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Mono.Cecil.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Mono.Cecil.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Mono.Cecil.Mdb.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Mono.Cecil.Mdb.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Mono.Cecil.Pdb.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Mono.Cecil.Pdb.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Mono.Cecil.Rocks.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Mono.Cecil.Rocks.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("MonoMod.RuntimeDetour.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/MonoMod.RuntimeDetour.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("MonoMod.Utils.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/MonoMod.Utils.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("SemanticVersioning.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/SemanticVersioning.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("StableNameDotNet.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/StableNameDotNet.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("WasmDisassembler.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/WasmDisassembler.dll")));
            _il2cppx64BepInEx->append(new BepInExFile(".version", "dotnet", new QFile(bepInExFolderPath + "/dotnet/.version")));
            _il2cppx64BepInEx->append(new BepInExFile("clretwrc.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/clretwrc.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("clrjit.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/clrjit.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("coreclr.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/coreclr.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("dbgshim.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/dbgshim.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("hostpolicy.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/hostpolicy.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Microsoft.Bcl.AsyncInterfaces.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.Bcl.AsyncInterfaces.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Microsoft.CSharp.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.CSharp.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Microsoft.DiaSymReader.Native.amd64.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.DiaSymReader.Native.amd64.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Microsoft.Extensions.DependencyInjection.Abstractions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.Extensions.DependencyInjection.Abstractions.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Microsoft.Extensions.DependencyInjection.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.Extensions.DependencyInjection.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Microsoft.Extensions.Logging.Abstractions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.Extensions.Logging.Abstractions.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Microsoft.Extensions.Logging.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.Extensions.Logging.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Microsoft.Extensions.Options.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.Extensions.Options.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Microsoft.Extensions.Primitives.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.Extensions.Primitives.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Microsoft.NETCore.App.deps.json", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.NETCore.App.deps.json")));
            _il2cppx64BepInEx->append(new BepInExFile("Microsoft.NETCore.App.runtimeconfig.json", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.NETCore.App.runtimeconfig.json")));
            _il2cppx64BepInEx->append(new BepInExFile("Microsoft.VisualBasic.Core.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.VisualBasic.Core.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Microsoft.VisualBasic.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.VisualBasic.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Microsoft.Win32.Primitives.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.Win32.Primitives.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("Microsoft.Win32.Registry.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.Win32.Registry.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("mscordaccore.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/mscordaccore.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("mscordaccore_amd64_amd64_6.0.722.32202.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/mscordaccore_amd64_amd64_6.0.722.32202.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("mscordbi.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/mscordbi.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("mscorlib.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/mscorlib.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("mscorrc.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/mscorrc.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("msquic.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/msquic.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("netstandard.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/netstandard.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.AppContext.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.AppContext.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Buffers.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Buffers.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Collections.Concurrent.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Collections.Concurrent.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Collections.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Collections.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Collections.Immutable.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Collections.Immutable.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Collections.NonGeneric.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Collections.NonGeneric.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Collections.Specialized.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Collections.Specialized.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.ComponentModel.Annotations.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.ComponentModel.Annotations.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.ComponentModel.DataAnnotations.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.ComponentModel.DataAnnotations.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.ComponentModel.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.ComponentModel.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.ComponentModel.EventBasedAsync.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.ComponentModel.EventBasedAsync.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.ComponentModel.Primitives.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.ComponentModel.Primitives.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.ComponentModel.TypeConverter.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.ComponentModel.TypeConverter.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Configuration.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Configuration.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Console.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Console.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Core.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Core.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Data.Common.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Data.Common.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Data.DataSetExtensions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Data.DataSetExtensions.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Data.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Data.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Diagnostics.Contracts.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Diagnostics.Contracts.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Diagnostics.Debug.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Diagnostics.Debug.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Diagnostics.DiagnosticSource.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Diagnostics.DiagnosticSource.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Diagnostics.FileVersionInfo.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Diagnostics.FileVersionInfo.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Diagnostics.Process.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Diagnostics.Process.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Diagnostics.StackTrace.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Diagnostics.StackTrace.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Diagnostics.TextWriterTraceListener.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Diagnostics.TextWriterTraceListener.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Diagnostics.Tools.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Diagnostics.Tools.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Diagnostics.TraceSource.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Diagnostics.TraceSource.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Diagnostics.Tracing.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Diagnostics.Tracing.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Drawing.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Drawing.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Drawing.Primitives.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Drawing.Primitives.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Dynamic.Runtime.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Dynamic.Runtime.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Formats.Asn1.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Formats.Asn1.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Globalization.Calendars.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Globalization.Calendars.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Globalization.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Globalization.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Globalization.Extensions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Globalization.Extensions.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.IO.Compression.Brotli.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.Compression.Brotli.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.IO.Compression.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.Compression.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.IO.Compression.FileSystem.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.Compression.FileSystem.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.IO.Compression.Native.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.Compression.Native.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.IO.Compression.ZipFile.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.Compression.ZipFile.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.IO.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.IO.FileSystem.AccessControl.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.FileSystem.AccessControl.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.IO.FileSystem.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.FileSystem.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.IO.FileSystem.DriveInfo.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.FileSystem.DriveInfo.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.IO.FileSystem.Primitives.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.FileSystem.Primitives.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.IO.FileSystem.Watcher.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.FileSystem.Watcher.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.IO.IsolatedStorage.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.IsolatedStorage.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.IO.MemoryMappedFiles.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.MemoryMappedFiles.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.IO.Pipes.AccessControl.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.Pipes.AccessControl.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.IO.Pipes.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.Pipes.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.IO.UnmanagedMemoryStream.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.UnmanagedMemoryStream.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Linq.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Linq.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Linq.Expressions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Linq.Expressions.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Linq.Parallel.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Linq.Parallel.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Linq.Queryable.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Linq.Queryable.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Memory.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Memory.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Net.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Net.Http.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.Http.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Net.Http.Json.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.Http.Json.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Net.HttpListener.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.HttpListener.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Net.Mail.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.Mail.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Net.NameResolution.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.NameResolution.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Net.NetworkInformation.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.NetworkInformation.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Net.Ping.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.Ping.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Net.Primitives.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.Primitives.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Net.Quic.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.Quic.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Net.Requests.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.Requests.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Net.Security.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.Security.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Net.ServicePoint.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.ServicePoint.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Net.Sockets.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.Sockets.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Net.WebClient.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.WebClient.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Net.WebHeaderCollection.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.WebHeaderCollection.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Net.WebProxy.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.WebProxy.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Net.WebSockets.Client.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.WebSockets.Client.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Net.WebSockets.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.WebSockets.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Numerics.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Numerics.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Numerics.Vectors.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Numerics.Vectors.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.ObjectModel.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.ObjectModel.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Private.CoreLib.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Private.CoreLib.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Private.DataContractSerialization.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Private.DataContractSerialization.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Private.Uri.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Private.Uri.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Private.Xml.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Private.Xml.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Private.Xml.Linq.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Private.Xml.Linq.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Reflection.DispatchProxy.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Reflection.DispatchProxy.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Reflection.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Reflection.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Reflection.Emit.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Reflection.Emit.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Reflection.Emit.ILGeneration.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Reflection.Emit.ILGeneration.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Reflection.Emit.Lightweight.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Reflection.Emit.Lightweight.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Reflection.Extensions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Reflection.Extensions.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Reflection.Metadata.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Reflection.Metadata.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Reflection.Primitives.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Reflection.Primitives.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Reflection.TypeExtensions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Reflection.TypeExtensions.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Resources.Reader.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Resources.Reader.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Resources.ResourceManager.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Resources.ResourceManager.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Resources.Writer.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Resources.Writer.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Runtime.CompilerServices.Unsafe.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.CompilerServices.Unsafe.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Runtime.CompilerServices.VisualC.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.CompilerServices.VisualC.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Runtime.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Runtime.Extensions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.Extensions.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Runtime.Handles.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.Handles.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Runtime.InteropServices.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.InteropServices.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Runtime.InteropServices.RuntimeInformation.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.InteropServices.RuntimeInformation.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Runtime.Intrinsics.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.Intrinsics.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Runtime.Loader.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.Loader.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Runtime.Numerics.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.Numerics.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Runtime.Serialization.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.Serialization.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Runtime.Serialization.Formatters.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.Serialization.Formatters.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Runtime.Serialization.Json.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.Serialization.Json.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Runtime.Serialization.Primitives.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.Serialization.Primitives.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Runtime.Serialization.Xml.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.Serialization.Xml.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Security.AccessControl.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.AccessControl.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Security.Claims.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.Claims.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Security.Cryptography.Algorithms.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.Cryptography.Algorithms.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Security.Cryptography.Cng.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.Cryptography.Cng.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Security.Cryptography.Csp.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.Cryptography.Csp.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Security.Cryptography.Encoding.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.Cryptography.Encoding.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Security.Cryptography.OpenSsl.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.Cryptography.OpenSsl.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Security.Cryptography.Primitives.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.Cryptography.Primitives.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Security.Cryptography.X509Certificates.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.Cryptography.X509Certificates.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Security.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Security.Principal.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.Principal.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Security.Principal.Windows.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.Principal.Windows.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Security.SecureString.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.SecureString.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.ServiceModel.Web.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.ServiceModel.Web.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.ServiceProcess.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.ServiceProcess.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Text.Encoding.CodePages.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Text.Encoding.CodePages.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Text.Encoding.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Text.Encoding.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Text.Encoding.Extensions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Text.Encoding.Extensions.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Text.Encodings.Web.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Text.Encodings.Web.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Text.Json.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Text.Json.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Text.RegularExpressions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Text.RegularExpressions.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Threading.Channels.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Threading.Channels.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Threading.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Threading.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Threading.Overlapped.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Threading.Overlapped.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Threading.Tasks.Dataflow.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Threading.Tasks.Dataflow.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Threading.Tasks.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Threading.Tasks.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Threading.Tasks.Extensions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Threading.Tasks.Extensions.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Threading.Tasks.Parallel.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Threading.Tasks.Parallel.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Threading.Thread.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Threading.Thread.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Threading.ThreadPool.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Threading.ThreadPool.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Threading.Timer.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Threading.Timer.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Transactions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Transactions.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Transactions.Local.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Transactions.Local.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.ValueTuple.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.ValueTuple.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Web.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Web.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Web.HttpUtility.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Web.HttpUtility.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Windows.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Windows.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Xml.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Xml.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Xml.Linq.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Xml.Linq.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Xml.ReaderWriter.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Xml.ReaderWriter.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Xml.Serialization.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Xml.Serialization.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Xml.XDocument.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Xml.XDocument.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Xml.XmlDocument.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Xml.XmlDocument.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Xml.XmlSerializer.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Xml.XmlSerializer.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Xml.XPath.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Xml.XPath.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("System.Xml.XPath.XDocument.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Xml.XPath.XDocument.dll")));
            _il2cppx64BepInEx->append(new BepInExFile("WindowsBase.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/WindowsBase.dll")));
            _il2cppx64BepInEx->append(new BepInExFile(".doorstop_version", "", new QFile(bepInExFolderPath + "/.doorstop_version")));
            _il2cppx64BepInEx->append(new BepInExFile("doorstop_config.ini", "", new QFile(bepInExFolderPath + "/doorstop_config.ini")));
            _il2cppx64BepInEx->append(new BepInExFile("winhttp.dll", "", new QFile(bepInExFolderPath + "/winhttp.dll")));
            _il2cppx64BepInEx->append(new BepInExFile(".yumi_bepinex_version", "", new QFile(bepInExFolderPath + "/.yumi_bepinex_version")));
        }
    }
    return _il2cppx64BepInEx;
}

QList<BepInExFile*>* Assets::getIL2CPPX86Files()
{
    if (_il2cppx86BepInEx == NULL)
    {
        QString downloadsFolderPath(Utils::toUnixPath(yumi::appPath + "/downloads"));
        QString bepInExFolderPath(Utils::toUnixPath(downloadsFolderPath + QDir::separator() + IL2CPPX86_BEPINEX_NAME));

        if (getLocalBepInEx(IL2CPPX86_BEPINEX_NAME, bepInExFolderPath, downloadsFolderPath))
        {
            _il2cppx86BepInEx = new QList<BepInExFile*>();

            _il2cppx86BepInEx->append(new BepInExFile("0Harmony.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/0Harmony.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("AsmResolver.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/AsmResolver.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("AsmResolver.DotNet.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/AsmResolver.DotNet.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("AsmResolver.PE.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/AsmResolver.PE.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("AsmResolver.PE.File.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/AsmResolver.PE.File.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("AssetRipper.VersionUtilities.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/AssetRipper.VersionUtilities.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("BepInEx.Core.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.Core.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("BepInEx.Preloader.Core.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.Preloader.Core.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("BepInEx.Unity.Common.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.Unity.Common.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("BepInEx.Unity.IL2CPP.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.Unity.IL2CPP.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("BepInEx.Unity.IL2CPP.dll.config", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/BepInEx.Unity.IL2CPP.dll.config")));
            _il2cppx86BepInEx->append(new BepInExFile("Cpp2IL.Core.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Cpp2IL.Core.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Disarm.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Disarm.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("dobby.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/dobby.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Gee.External.Capstone.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Gee.External.Capstone.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Iced.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Iced.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Il2CppInterop.Common.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Il2CppInterop.Common.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Il2CppInterop.Generator.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Il2CppInterop.Generator.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Il2CppInterop.HarmonySupport.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Il2CppInterop.HarmonySupport.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Il2CppInterop.Runtime.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Il2CppInterop.Runtime.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("LibCpp2IL.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/LibCpp2IL.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Mono.Cecil.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Mono.Cecil.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Mono.Cecil.Mdb.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Mono.Cecil.Mdb.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Mono.Cecil.Pdb.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Mono.Cecil.Pdb.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Mono.Cecil.Rocks.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/Mono.Cecil.Rocks.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("MonoMod.RuntimeDetour.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/MonoMod.RuntimeDetour.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("MonoMod.Utils.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/MonoMod.Utils.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("SemanticVersioning.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/SemanticVersioning.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("StableNameDotNet.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/StableNameDotNet.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("WasmDisassembler.dll", "BepInEx/core", new QFile(bepInExFolderPath + "/BepInEx/core/WasmDisassembler.dll")));
            _il2cppx86BepInEx->append(new BepInExFile(".version", "dotnet", new QFile(bepInExFolderPath + "/dotnet/.version")));
            _il2cppx86BepInEx->append(new BepInExFile("clretwrc.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/clretwrc.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("clrjit.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/clrjit.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("coreclr.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/coreclr.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("dbgshim.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/dbgshim.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("hostpolicy.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/hostpolicy.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Microsoft.Bcl.AsyncInterfaces.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.Bcl.AsyncInterfaces.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Microsoft.CSharp.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.CSharp.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Microsoft.DiaSymReader.Native.x86.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.DiaSymReader.Native.x86.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Microsoft.Extensions.DependencyInjection.Abstractions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.Extensions.DependencyInjection.Abstractions.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Microsoft.Extensions.DependencyInjection.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.Extensions.DependencyInjection.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Microsoft.Extensions.Logging.Abstractions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.Extensions.Logging.Abstractions.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Microsoft.Extensions.Logging.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.Extensions.Logging.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Microsoft.Extensions.Options.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.Extensions.Options.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Microsoft.Extensions.Primitives.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.Extensions.Primitives.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Microsoft.NETCore.App.deps.json", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.NETCore.App.deps.json")));
            _il2cppx86BepInEx->append(new BepInExFile("Microsoft.NETCore.App.runtimeconfig.json", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.NETCore.App.runtimeconfig.json")));
            _il2cppx86BepInEx->append(new BepInExFile("Microsoft.VisualBasic.Core.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.VisualBasic.Core.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Microsoft.VisualBasic.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.VisualBasic.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Microsoft.Win32.Primitives.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.Win32.Primitives.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("Microsoft.Win32.Registry.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/Microsoft.Win32.Registry.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("mscordaccore.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/mscordaccore.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("mscordaccore_x86_x86_6.0.722.32202.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/mscordaccore_x86_x86_6.0.722.32202.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("mscordbi.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/mscordbi.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("mscorlib.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/mscorlib.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("mscorrc.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/mscorrc.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("msquic.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/msquic.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("netstandard.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/netstandard.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.AppContext.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.AppContext.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Buffers.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Buffers.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Collections.Concurrent.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Collections.Concurrent.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Collections.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Collections.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Collections.Immutable.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Collections.Immutable.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Collections.NonGeneric.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Collections.NonGeneric.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Collections.Specialized.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Collections.Specialized.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.ComponentModel.Annotations.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.ComponentModel.Annotations.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.ComponentModel.DataAnnotations.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.ComponentModel.DataAnnotations.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.ComponentModel.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.ComponentModel.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.ComponentModel.EventBasedAsync.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.ComponentModel.EventBasedAsync.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.ComponentModel.Primitives.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.ComponentModel.Primitives.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.ComponentModel.TypeConverter.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.ComponentModel.TypeConverter.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Configuration.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Configuration.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Console.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Console.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Core.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Core.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Data.Common.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Data.Common.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Data.DataSetExtensions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Data.DataSetExtensions.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Data.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Data.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Diagnostics.Contracts.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Diagnostics.Contracts.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Diagnostics.Debug.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Diagnostics.Debug.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Diagnostics.DiagnosticSource.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Diagnostics.DiagnosticSource.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Diagnostics.FileVersionInfo.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Diagnostics.FileVersionInfo.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Diagnostics.Process.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Diagnostics.Process.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Diagnostics.StackTrace.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Diagnostics.StackTrace.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Diagnostics.TextWriterTraceListener.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Diagnostics.TextWriterTraceListener.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Diagnostics.Tools.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Diagnostics.Tools.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Diagnostics.TraceSource.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Diagnostics.TraceSource.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Diagnostics.Tracing.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Diagnostics.Tracing.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Drawing.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Drawing.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Drawing.Primitives.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Drawing.Primitives.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Dynamic.Runtime.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Dynamic.Runtime.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Formats.Asn1.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Formats.Asn1.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Globalization.Calendars.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Globalization.Calendars.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Globalization.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Globalization.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Globalization.Extensions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Globalization.Extensions.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.IO.Compression.Brotli.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.Compression.Brotli.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.IO.Compression.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.Compression.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.IO.Compression.FileSystem.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.Compression.FileSystem.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.IO.Compression.Native.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.Compression.Native.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.IO.Compression.ZipFile.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.Compression.ZipFile.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.IO.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.IO.FileSystem.AccessControl.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.FileSystem.AccessControl.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.IO.FileSystem.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.FileSystem.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.IO.FileSystem.DriveInfo.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.FileSystem.DriveInfo.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.IO.FileSystem.Primitives.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.FileSystem.Primitives.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.IO.FileSystem.Watcher.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.FileSystem.Watcher.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.IO.IsolatedStorage.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.IsolatedStorage.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.IO.MemoryMappedFiles.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.MemoryMappedFiles.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.IO.Pipes.AccessControl.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.Pipes.AccessControl.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.IO.Pipes.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.Pipes.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.IO.UnmanagedMemoryStream.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.IO.UnmanagedMemoryStream.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Linq.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Linq.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Linq.Expressions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Linq.Expressions.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Linq.Parallel.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Linq.Parallel.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Linq.Queryable.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Linq.Queryable.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Memory.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Memory.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Net.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Net.Http.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.Http.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Net.Http.Json.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.Http.Json.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Net.HttpListener.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.HttpListener.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Net.Mail.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.Mail.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Net.NameResolution.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.NameResolution.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Net.NetworkInformation.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.NetworkInformation.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Net.Ping.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.Ping.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Net.Primitives.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.Primitives.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Net.Quic.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.Quic.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Net.Requests.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.Requests.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Net.Security.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.Security.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Net.ServicePoint.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.ServicePoint.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Net.Sockets.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.Sockets.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Net.WebClient.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.WebClient.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Net.WebHeaderCollection.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.WebHeaderCollection.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Net.WebProxy.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.WebProxy.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Net.WebSockets.Client.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.WebSockets.Client.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Net.WebSockets.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Net.WebSockets.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Numerics.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Numerics.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Numerics.Vectors.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Numerics.Vectors.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.ObjectModel.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.ObjectModel.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Private.CoreLib.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Private.CoreLib.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Private.DataContractSerialization.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Private.DataContractSerialization.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Private.Uri.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Private.Uri.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Private.Xml.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Private.Xml.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Private.Xml.Linq.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Private.Xml.Linq.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Reflection.DispatchProxy.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Reflection.DispatchProxy.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Reflection.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Reflection.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Reflection.Emit.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Reflection.Emit.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Reflection.Emit.ILGeneration.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Reflection.Emit.ILGeneration.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Reflection.Emit.Lightweight.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Reflection.Emit.Lightweight.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Reflection.Extensions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Reflection.Extensions.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Reflection.Metadata.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Reflection.Metadata.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Reflection.Primitives.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Reflection.Primitives.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Reflection.TypeExtensions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Reflection.TypeExtensions.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Resources.Reader.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Resources.Reader.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Resources.ResourceManager.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Resources.ResourceManager.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Resources.Writer.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Resources.Writer.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Runtime.CompilerServices.Unsafe.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.CompilerServices.Unsafe.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Runtime.CompilerServices.VisualC.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.CompilerServices.VisualC.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Runtime.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Runtime.Extensions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.Extensions.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Runtime.Handles.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.Handles.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Runtime.InteropServices.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.InteropServices.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Runtime.InteropServices.RuntimeInformation.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.InteropServices.RuntimeInformation.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Runtime.Intrinsics.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.Intrinsics.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Runtime.Loader.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.Loader.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Runtime.Numerics.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.Numerics.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Runtime.Serialization.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.Serialization.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Runtime.Serialization.Formatters.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.Serialization.Formatters.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Runtime.Serialization.Json.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.Serialization.Json.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Runtime.Serialization.Primitives.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.Serialization.Primitives.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Runtime.Serialization.Xml.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Runtime.Serialization.Xml.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Security.AccessControl.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.AccessControl.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Security.Claims.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.Claims.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Security.Cryptography.Algorithms.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.Cryptography.Algorithms.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Security.Cryptography.Cng.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.Cryptography.Cng.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Security.Cryptography.Csp.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.Cryptography.Csp.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Security.Cryptography.Encoding.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.Cryptography.Encoding.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Security.Cryptography.OpenSsl.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.Cryptography.OpenSsl.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Security.Cryptography.Primitives.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.Cryptography.Primitives.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Security.Cryptography.X509Certificates.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.Cryptography.X509Certificates.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Security.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Security.Principal.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.Principal.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Security.Principal.Windows.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.Principal.Windows.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Security.SecureString.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Security.SecureString.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.ServiceModel.Web.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.ServiceModel.Web.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.ServiceProcess.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.ServiceProcess.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Text.Encoding.CodePages.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Text.Encoding.CodePages.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Text.Encoding.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Text.Encoding.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Text.Encoding.Extensions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Text.Encoding.Extensions.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Text.Encodings.Web.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Text.Encodings.Web.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Text.Json.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Text.Json.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Text.RegularExpressions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Text.RegularExpressions.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Threading.Channels.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Threading.Channels.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Threading.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Threading.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Threading.Overlapped.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Threading.Overlapped.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Threading.Tasks.Dataflow.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Threading.Tasks.Dataflow.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Threading.Tasks.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Threading.Tasks.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Threading.Tasks.Extensions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Threading.Tasks.Extensions.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Threading.Tasks.Parallel.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Threading.Tasks.Parallel.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Threading.Thread.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Threading.Thread.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Threading.ThreadPool.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Threading.ThreadPool.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Threading.Timer.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Threading.Timer.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Transactions.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Transactions.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Transactions.Local.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Transactions.Local.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.ValueTuple.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.ValueTuple.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Web.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Web.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Web.HttpUtility.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Web.HttpUtility.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Windows.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Windows.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Xml.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Xml.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Xml.Linq.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Xml.Linq.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Xml.ReaderWriter.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Xml.ReaderWriter.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Xml.Serialization.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Xml.Serialization.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Xml.XDocument.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Xml.XDocument.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Xml.XmlDocument.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Xml.XmlDocument.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Xml.XmlSerializer.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Xml.XmlSerializer.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Xml.XPath.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Xml.XPath.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("System.Xml.XPath.XDocument.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/System.Xml.XPath.XDocument.dll")));
            _il2cppx86BepInEx->append(new BepInExFile("WindowsBase.dll", "dotnet", new QFile(bepInExFolderPath + "/dotnet/WindowsBase.dll")));
            _il2cppx86BepInEx->append(new BepInExFile(".doorstop_version", "", new QFile(bepInExFolderPath + "/.doorstop_version")));
            _il2cppx86BepInEx->append(new BepInExFile("doorstop_config.ini", "", new QFile(bepInExFolderPath + "/doorstop_config.ini")));
            _il2cppx86BepInEx->append(new BepInExFile("winhttp.dll", "", new QFile(bepInExFolderPath + "/winhttp.dll")));
            _il2cppx86BepInEx->append(new BepInExFile(".yumi_bepinex_version", "", new QFile(bepInExFolderPath + "/.yumi_bepinex_version")));
        }
    }
    return _il2cppx86BepInEx;
}

QStringList Assets::getAllBepInExFiles()
{
    QStringList res = QStringList();
    res << "/.doorstop_version";
    res << "/.yumi_bepinex_version";
    res << "/BepInEx/core/0Harmony.dll";
    res << "/BepInEx/core/0Harmony.xml";
    res << "/BepInEx/core/0Harmony20.dll";
    res << "/BepInEx/core/AsmResolver.DotNet.dll";
    res << "/BepInEx/core/AsmResolver.PE.File.dll";
    res << "/BepInEx/core/AsmResolver.PE.dll";
    res << "/BepInEx/core/AsmResolver.dll";
    res << "/BepInEx/core/AssetRipper.VersionUtilities.dll";
    res << "/BepInEx/core/BepInEx.Core.dll";
    res << "/BepInEx/core/BepInEx.Harmony.dll";
    res << "/BepInEx/core/BepInEx.Harmony.xml";
    res << "/BepInEx/core/BepInEx.Preloader.Core.dll";
    res << "/BepInEx/core/BepInEx.Preloader.dll";
    res << "/BepInEx/core/BepInEx.Preloader.xml";
    res << "/BepInEx/core/BepInEx.Unity.Common.dll";
    res << "/BepInEx/core/BepInEx.Unity.IL2CPP.dll";
    res << "/BepInEx/core/BepInEx.Unity.IL2CPP.dll.config";
    res << "/BepInEx/core/BepInEx.dll";
    res << "/BepInEx/core/BepInEx.xml";
    res << "/BepInEx/core/Cpp2IL.Core.dll";
    res << "/BepInEx/core/Disarm.dll";
    res << "/BepInEx/core/Gee.External.Capstone.dll";
    res << "/BepInEx/core/HarmonyXInterop.dll";
    res << "/BepInEx/core/Iced.dll";
    res << "/BepInEx/core/Il2CppInterop.Common.dll";
    res << "/BepInEx/core/Il2CppInterop.Generator.dll";
    res << "/BepInEx/core/Il2CppInterop.HarmonySupport.dll";
    res << "/BepInEx/core/Il2CppInterop.Runtime.dll";
    res << "/BepInEx/core/LibCpp2IL.dll";
    res << "/BepInEx/core/Mono.Cecil.Mdb.dll";
    res << "/BepInEx/core/Mono.Cecil.Pdb.dll";
    res << "/BepInEx/core/Mono.Cecil.Rocks.dll";
    res << "/BepInEx/core/Mono.Cecil.dll";
    res << "/BepInEx/core/MonoMod.RuntimeDetour.dll";
    res << "/BepInEx/core/MonoMod.RuntimeDetour.xml";
    res << "/BepInEx/core/MonoMod.Utils.dll";
    res << "/BepInEx/core/MonoMod.Utils.xml";
    res << "/BepInEx/core/SemanticVersioning.dll";
    res << "/BepInEx/core/StableNameDotNet.dll";
    res << "/BepInEx/core/WasmDisassembler.dll";
    res << "/BepInEx/core/dobby.dll";
    res << "/doorstop_config.ini";
    res << "/doorstop_libs/libdoorstop_x64.dylib";
    res << "/doorstop_libs/libdoorstop_x64.so";
    res << "/doorstop_libs/libdoorstop_x86.dylib";
    res << "/doorstop_libs/libdoorstop_x86.so";
    res << "/dotnet/.version";
    res << "/dotnet/Microsoft.Bcl.AsyncInterfaces.dll";
    res << "/dotnet/Microsoft.CSharp.dll";
    res << "/dotnet/Microsoft.DiaSymReader.Native.amd64.dll";
    res << "/dotnet/Microsoft.DiaSymReader.Native.x86.dll";
    res << "/dotnet/Microsoft.Extensions.DependencyInjection.Abstractions.dll";
    res << "/dotnet/Microsoft.Extensions.DependencyInjection.dll";
    res << "/dotnet/Microsoft.Extensions.Logging.Abstractions.dll";
    res << "/dotnet/Microsoft.Extensions.Logging.dll";
    res << "/dotnet/Microsoft.Extensions.Options.dll";
    res << "/dotnet/Microsoft.Extensions.Primitives.dll";
    res << "/dotnet/Microsoft.NETCore.App.deps.json";
    res << "/dotnet/Microsoft.NETCore.App.runtimeconfig.json";
    res << "/dotnet/Microsoft.VisualBasic.Core.dll";
    res << "/dotnet/Microsoft.VisualBasic.dll";
    res << "/dotnet/Microsoft.Win32.Primitives.dll";
    res << "/dotnet/Microsoft.Win32.Registry.dll";
    res << "/dotnet/System.AppContext.dll";
    res << "/dotnet/System.Buffers.dll";
    res << "/dotnet/System.Collections.Concurrent.dll";
    res << "/dotnet/System.Collections.Immutable.dll";
    res << "/dotnet/System.Collections.NonGeneric.dll";
    res << "/dotnet/System.Collections.Specialized.dll";
    res << "/dotnet/System.Collections.dll";
    res << "/dotnet/System.ComponentModel.Annotations.dll";
    res << "/dotnet/System.ComponentModel.DataAnnotations.dll";
    res << "/dotnet/System.ComponentModel.EventBasedAsync.dll";
    res << "/dotnet/System.ComponentModel.Primitives.dll";
    res << "/dotnet/System.ComponentModel.TypeConverter.dll";
    res << "/dotnet/System.ComponentModel.dll";
    res << "/dotnet/System.Configuration.dll";
    res << "/dotnet/System.Console.dll";
    res << "/dotnet/System.Core.dll";
    res << "/dotnet/System.Data.Common.dll";
    res << "/dotnet/System.Data.DataSetExtensions.dll";
    res << "/dotnet/System.Data.dll";
    res << "/dotnet/System.Diagnostics.Contracts.dll";
    res << "/dotnet/System.Diagnostics.Debug.dll";
    res << "/dotnet/System.Diagnostics.DiagnosticSource.dll";
    res << "/dotnet/System.Diagnostics.FileVersionInfo.dll";
    res << "/dotnet/System.Diagnostics.Process.dll";
    res << "/dotnet/System.Diagnostics.StackTrace.dll";
    res << "/dotnet/System.Diagnostics.TextWriterTraceListener.dll";
    res << "/dotnet/System.Diagnostics.Tools.dll";
    res << "/dotnet/System.Diagnostics.TraceSource.dll";
    res << "/dotnet/System.Diagnostics.Tracing.dll";
    res << "/dotnet/System.Drawing.Primitives.dll";
    res << "/dotnet/System.Drawing.dll";
    res << "/dotnet/System.Dynamic.Runtime.dll";
    res << "/dotnet/System.Formats.Asn1.dll";
    res << "/dotnet/System.Globalization.Calendars.dll";
    res << "/dotnet/System.Globalization.Extensions.dll";
    res << "/dotnet/System.Globalization.dll";
    res << "/dotnet/System.IO.Compression.Brotli.dll";
    res << "/dotnet/System.IO.Compression.FileSystem.dll";
    res << "/dotnet/System.IO.Compression.Native.dll";
    res << "/dotnet/System.IO.Compression.ZipFile.dll";
    res << "/dotnet/System.IO.Compression.dll";
    res << "/dotnet/System.IO.FileSystem.AccessControl.dll";
    res << "/dotnet/System.IO.FileSystem.DriveInfo.dll";
    res << "/dotnet/System.IO.FileSystem.Primitives.dll";
    res << "/dotnet/System.IO.FileSystem.Watcher.dll";
    res << "/dotnet/System.IO.FileSystem.dll";
    res << "/dotnet/System.IO.IsolatedStorage.dll";
    res << "/dotnet/System.IO.MemoryMappedFiles.dll";
    res << "/dotnet/System.IO.Pipes.AccessControl.dll";
    res << "/dotnet/System.IO.Pipes.dll";
    res << "/dotnet/System.IO.UnmanagedMemoryStream.dll";
    res << "/dotnet/System.IO.dll";
    res << "/dotnet/System.Linq.Expressions.dll";
    res << "/dotnet/System.Linq.Parallel.dll";
    res << "/dotnet/System.Linq.Queryable.dll";
    res << "/dotnet/System.Linq.dll";
    res << "/dotnet/System.Memory.dll";
    res << "/dotnet/System.Net.Http.Json.dll";
    res << "/dotnet/System.Net.Http.dll";
    res << "/dotnet/System.Net.HttpListener.dll";
    res << "/dotnet/System.Net.Mail.dll";
    res << "/dotnet/System.Net.NameResolution.dll";
    res << "/dotnet/System.Net.NetworkInformation.dll";
    res << "/dotnet/System.Net.Ping.dll";
    res << "/dotnet/System.Net.Primitives.dll";
    res << "/dotnet/System.Net.Quic.dll";
    res << "/dotnet/System.Net.Requests.dll";
    res << "/dotnet/System.Net.Security.dll";
    res << "/dotnet/System.Net.ServicePoint.dll";
    res << "/dotnet/System.Net.Sockets.dll";
    res << "/dotnet/System.Net.WebClient.dll";
    res << "/dotnet/System.Net.WebHeaderCollection.dll";
    res << "/dotnet/System.Net.WebProxy.dll";
    res << "/dotnet/System.Net.WebSockets.Client.dll";
    res << "/dotnet/System.Net.WebSockets.dll";
    res << "/dotnet/System.Net.dll";
    res << "/dotnet/System.Numerics.Vectors.dll";
    res << "/dotnet/System.Numerics.dll";
    res << "/dotnet/System.ObjectModel.dll";
    res << "/dotnet/System.Private.CoreLib.dll";
    res << "/dotnet/System.Private.DataContractSerialization.dll";
    res << "/dotnet/System.Private.Uri.dll";
    res << "/dotnet/System.Private.Xml.Linq.dll";
    res << "/dotnet/System.Private.Xml.dll";
    res << "/dotnet/System.Reflection.DispatchProxy.dll";
    res << "/dotnet/System.Reflection.Emit.ILGeneration.dll";
    res << "/dotnet/System.Reflection.Emit.Lightweight.dll";
    res << "/dotnet/System.Reflection.Emit.dll";
    res << "/dotnet/System.Reflection.Extensions.dll";
    res << "/dotnet/System.Reflection.Metadata.dll";
    res << "/dotnet/System.Reflection.Primitives.dll";
    res << "/dotnet/System.Reflection.TypeExtensions.dll";
    res << "/dotnet/System.Reflection.dll";
    res << "/dotnet/System.Resources.Reader.dll";
    res << "/dotnet/System.Resources.ResourceManager.dll";
    res << "/dotnet/System.Resources.Writer.dll";
    res << "/dotnet/System.Runtime.CompilerServices.Unsafe.dll";
    res << "/dotnet/System.Runtime.CompilerServices.VisualC.dll";
    res << "/dotnet/System.Runtime.Extensions.dll";
    res << "/dotnet/System.Runtime.Handles.dll";
    res << "/dotnet/System.Runtime.InteropServices.RuntimeInformation.dll";
    res << "/dotnet/System.Runtime.InteropServices.dll";
    res << "/dotnet/System.Runtime.Intrinsics.dll";
    res << "/dotnet/System.Runtime.Loader.dll";
    res << "/dotnet/System.Runtime.Numerics.dll";
    res << "/dotnet/System.Runtime.Serialization.Formatters.dll";
    res << "/dotnet/System.Runtime.Serialization.Json.dll";
    res << "/dotnet/System.Runtime.Serialization.Primitives.dll";
    res << "/dotnet/System.Runtime.Serialization.Xml.dll";
    res << "/dotnet/System.Runtime.Serialization.dll";
    res << "/dotnet/System.Runtime.dll";
    res << "/dotnet/System.Security.AccessControl.dll";
    res << "/dotnet/System.Security.Claims.dll";
    res << "/dotnet/System.Security.Cryptography.Algorithms.dll";
    res << "/dotnet/System.Security.Cryptography.Cng.dll";
    res << "/dotnet/System.Security.Cryptography.Csp.dll";
    res << "/dotnet/System.Security.Cryptography.Encoding.dll";
    res << "/dotnet/System.Security.Cryptography.OpenSsl.dll";
    res << "/dotnet/System.Security.Cryptography.Primitives.dll";
    res << "/dotnet/System.Security.Cryptography.X509Certificates.dll";
    res << "/dotnet/System.Security.Principal.Windows.dll";
    res << "/dotnet/System.Security.Principal.dll";
    res << "/dotnet/System.Security.SecureString.dll";
    res << "/dotnet/System.Security.dll";
    res << "/dotnet/System.ServiceModel.Web.dll";
    res << "/dotnet/System.ServiceProcess.dll";
    res << "/dotnet/System.Text.Encoding.CodePages.dll";
    res << "/dotnet/System.Text.Encoding.Extensions.dll";
    res << "/dotnet/System.Text.Encoding.dll";
    res << "/dotnet/System.Text.Encodings.Web.dll";
    res << "/dotnet/System.Text.Json.dll";
    res << "/dotnet/System.Text.RegularExpressions.dll";
    res << "/dotnet/System.Threading.Channels.dll";
    res << "/dotnet/System.Threading.Overlapped.dll";
    res << "/dotnet/System.Threading.Tasks.Dataflow.dll";
    res << "/dotnet/System.Threading.Tasks.Extensions.dll";
    res << "/dotnet/System.Threading.Tasks.Parallel.dll";
    res << "/dotnet/System.Threading.Tasks.dll";
    res << "/dotnet/System.Threading.Thread.dll";
    res << "/dotnet/System.Threading.ThreadPool.dll";
    res << "/dotnet/System.Threading.Timer.dll";
    res << "/dotnet/System.Threading.dll";
    res << "/dotnet/System.Transactions.Local.dll";
    res << "/dotnet/System.Transactions.dll";
    res << "/dotnet/System.ValueTuple.dll";
    res << "/dotnet/System.Web.HttpUtility.dll";
    res << "/dotnet/System.Web.dll";
    res << "/dotnet/System.Windows.dll";
    res << "/dotnet/System.Xml.Linq.dll";
    res << "/dotnet/System.Xml.ReaderWriter.dll";
    res << "/dotnet/System.Xml.Serialization.dll";
    res << "/dotnet/System.Xml.XDocument.dll";
    res << "/dotnet/System.Xml.XPath.XDocument.dll";
    res << "/dotnet/System.Xml.XPath.dll";
    res << "/dotnet/System.Xml.XmlDocument.dll";
    res << "/dotnet/System.Xml.XmlSerializer.dll";
    res << "/dotnet/System.Xml.dll";
    res << "/dotnet/System.dll";
    res << "/dotnet/WindowsBase.dll";
    res << "/dotnet/clretwrc.dll";
    res << "/dotnet/clrjit.dll";
    res << "/dotnet/coreclr.dll";
    res << "/dotnet/dbgshim.dll";
    res << "/dotnet/hostpolicy.dll";
    res << "/dotnet/mscordaccore.dll";
    res << "/dotnet/mscordaccore_amd64_amd64_6.0.722.32202.dll";
    res << "/dotnet/mscordaccore_x86_x86_6.0.722.32202.dll";
    res << "/dotnet/mscordbi.dll";
    res << "/dotnet/mscorlib.dll";
    res << "/dotnet/mscorrc.dll";
    res << "/dotnet/msquic.dll";
    res << "/dotnet/netstandard.dll";
    res << "/run_bepinex.sh";
    res << "/run_yumi_bepinex.sh";
    res << "/winhttp.dll";
    return res;
}

QString Assets::getColorBtnStyle(const QString& color)
{
    return QString("QPushButton { min-height: 25px; font-size: 14px; color: black; border: 1px solid black; border-radius: 6px; background-color: " + color + "; } "
        "QPushButton:hover { background-color: " + color + "; border: 2px solid white; } "
        "QPushButton:pressed { background-color: " + color + "; } "
        "QPushButton:flat { border: none; } "
        "QPushButton:default { border-color: navy; }");
}

void Assets::updateStyles()
{
    primaryColor = QColor(PRIMARY_COLOR_HEX);
    secondaryColor = QColor(SECONDARY_COLOR_HEX);
    secondaryColorLight = QColor(SECONDARY_COLOR_LIGHT_HEX);
    secondaryColorDeep = QColor(SECONDARY_COLOR_DEEP_HEX);
    lightTextColor = QColor(LIGHT_TEXT_COLOR_HEX);

    DEFAULT_WINDOW_STYLE = QString("background-color: " + PRIMARY_COLOR_HEX + "; border: 1px solid " + SECONDARY_COLOR_HEX + "; border-radius: 10px; margin: 0; padding: 0;");
    DEFAULT_WINDOW_TITLE_STYLE = QString("background-color: " + PRIMARY_COLOR_LIGHT_HEX + "; border-top: 1px solid " + SECONDARY_COLOR_HEX + "; border-left: 1px solid " + SECONDARY_COLOR_HEX + "; border-right: 1px solid " + SECONDARY_COLOR_HEX + "; border-bottom: 3px solid " + SECONDARY_COLOR_HEX + "; border-top-left-radius: 10px; border-top-right-radius: 10px;");
    REGULAR_SCROLL_AREA_STYLE = QString("background-color: " + SECONDARY_COLOR_LIGHT_HEX + "; border: 1px solid " + SECONDARY_COLOR_HEX + "; border-radius: 6px;");
    SIDE_MENU_SCROLL_AREA_STYLE = QString("background-color: " + PRIMARY_COLOR_LIGHT_HEX + "; border-radius: 10px;");

    TITLE_LABEL_STYLE = QString("color: " + SECONDARY_COLOR_HEX + "; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; font-size: " + XXL_FONT_SIZE_PX + "px; font-weight: 500;");
    SMALLER_TITLE_LABEL_STYLE = QString("color: " + SECONDARY_COLOR_HEX + "; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; font-size: " + XL_FONT_SIZE_PX + "px; font-weight: 500;");
    REGULAR_LABEL_STYLE = QString("color: " + SECONDARY_COLOR_HEX + "; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; font-size: " + DEFAULT_FONT_SIZE_PX + "px;");
    ALTERNATE_LABEL_STYLE = QString("color: " + LIGHT_TEXT_COLOR_HEX + "; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; font-size: " + DEFAULT_FONT_SIZE_PX + "px;");
    ITALIC_LABEL_STYLE = QString("color: " + SECONDARY_COLOR_HEX + "; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; font-size: " + DEFAULT_FONT_SIZE_PX + "px; font-style: italic;");
    BOLD_LABEL_STYLE = QString("color: " + SECONDARY_COLOR_HEX + "; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; font-size: " + DEFAULT_FONT_SIZE_PX + "px; font-weight: bold;");
    ABOUT_LABEL_STYLE = QString("color: " + SECONDARY_COLOR_HEX + "; font-family: \"" + SECONDARY_FONT_FAMILY + "\", sans-serif; font-size: " + SM_FONT_SIZE_PX + "px;");
    LICENSE_FONT_STYLE = QString("color: " + LIGHT_TEXT_COLOR_HEX + "; font-family: \"" + SECONDARY_FONT_FAMILY + "\", sans-serif; font-size: 12px;");
    COMBOBOX_LABEL_STYLE = QString("color: black; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; font-size: " + DEFAULT_FONT_SIZE_PX + "px;");

    MAIN_BUTTON_STYLE = QString("min-height: 40px; max-width: 350px; margin: 5px; padding-left: 15px; padding-right: 15px; color: " + LIGHT_TEXT_COLOR_HEX + "; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; font-size: " + XL_FONT_SIZE_PX + "px; font-weight: 500; border: 2px solid " + SECONDARY_COLOR_HEX + "; border-radius: 6px; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 " + SECONDARY_COLOR_LIGHTER_HEX + ", stop: 1 " + SECONDARY_COLOR_HEX + ");");
    MAIN_BUTTON_STYLE_FULLWIDTH = QString("min-height: 40px; color: " + LIGHT_TEXT_COLOR_HEX + "; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; font-size: " + XL_FONT_SIZE_PX + "px; font-weight: 500; border: 2px solid " + SECONDARY_COLOR_HEX + "; border-radius: 6px; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 " + SECONDARY_COLOR_LIGHTER_HEX + ", stop: 1 " + SECONDARY_COLOR_HEX + ");");
    MAIN_BUTTON_STYLE_HOVER = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 " + ALT_SECONDARY_COLOR_LIGHT_HEX + ", stop: 1 " + ALT_SECONDARY_COLOR_HEX + ");");
    MAIN_BUTTON_STYLE_PRESSED = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 " + ALT_PRIMARY_COLOR_LIGHT_HEX + ", stop: 1 " + ALT_PRIMARY_COLOR_HEX + ");");

    SECONDARY_BUTTON_STYLE = QString("min-height: 25px; padding-left: 5px; padding-right: 5px; font-size: " + SM_FONT_SIZE_PX + "px; color: " + LIGHT_TEXT_COLOR_HEX + "; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; border: 1px solid " + SECONDARY_COLOR_HEX + "; border-radius: 6px; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 " + SECONDARY_COLOR_LIGHTER_HEX + ", stop: 1 " + SECONDARY_COLOR_HEX + ");");
    SECONDARY_BUTTON_STYLE_DISABLED = QString("min-height: 25px; padding-left: 5px; padding-right: 5px; font-size: " + SM_FONT_SIZE_PX + "px; color: " + LIGHT_TEXT_COLOR_HEX + "; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; border: 1px solid " + SECONDARY_COLOR_HEX + "; border-radius: 6px; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 " + NEUTRAL_COLOR_LIGHTER_HEX + ", stop: 1 " + NEUTRAL_COLOR_HEX + ");");
    SECONDARY_BUTTON_STYLE_SELECTED = QString("min-height: 30px; padding-left: 5px; padding-right: 5px; font-size: " + SM_FONT_SIZE_PX + "px; font-weight: 500; color: " + LIGHT_TEXT_COLOR_HEX + "; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; border: 3px solid " + SECONDARY_COLOR_HEX + "; border-radius: 6px; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 " + ALT_PRIMARY_COLOR_HEX + ", stop: 1 " + ALT_PRIMARY_COLOR_DEEP_HEX + ");");
    SECONDARY_BUTTON_STYLE_HOVER = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 " + ALT_SECONDARY_COLOR_LIGHT_HEX + ", stop: 1 " + ALT_SECONDARY_COLOR_HEX + ");");
    SECONDARY_BUTTON_STYLE_SELECTED_HOVER = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 " + ALT_PRIMARY_COLOR_HEX + ", stop: 1 " + ALT_PRIMARY_COLOR_DEEP_HEX + ");");
    SECONDARY_BUTTON_STYLE_PRESSED = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 " + ALT_PRIMARY_COLOR_LIGHT_HEX + ", stop: 1 " + ALT_PRIMARY_COLOR_HEX + ");");

    mainBtnStyle = "QPushButton { " + MAIN_BUTTON_STYLE +  " } "
        "QPushButton:hover { " + MAIN_BUTTON_STYLE_HOVER + " } "
        "QPushButton:pressed { " + MAIN_BUTTON_STYLE_PRESSED + " } "
        "QPushButton:flat { " + MAIN_BUTTON_STYLE_FLAT + " } "
        "QPushButton:default { " + MAIN_BUTTON_STYLE_DEFAULT + " }";

    mainBtnStyleFullWidth = "QPushButton { " + MAIN_BUTTON_STYLE_FULLWIDTH +  " } "
        "QPushButton:hover { " + MAIN_BUTTON_STYLE_HOVER + " } "
        "QPushButton:pressed { " + MAIN_BUTTON_STYLE_PRESSED + " } "
        "QPushButton:flat { " + MAIN_BUTTON_STYLE_FLAT + " } "
        "QPushButton:default { " + MAIN_BUTTON_STYLE_DEFAULT + " }";

    secondaryBtnStyle = "QPushButton { " + SECONDARY_BUTTON_STYLE +  " } "
        "QPushButton:hover { " + SECONDARY_BUTTON_STYLE_HOVER + " } "
        "QPushButton:pressed { " + SECONDARY_BUTTON_STYLE_PRESSED + " } "
        "QPushButton:flat { " + SECONDARY_BUTTON_STYLE_FLAT + " } "
        "QPushButton:default { " + SECONDARY_BUTTON_STYLE_DEFAULT + " }";

    secondaryBtnStyleDisabled = "QPushButton { " + SECONDARY_BUTTON_STYLE_DISABLED +  " } "
        "QPushButton:hover { " + SECONDARY_BUTTON_STYLE_HOVER + " } "
        "QPushButton:pressed { " + SECONDARY_BUTTON_STYLE_PRESSED + " } "
        "QPushButton:flat { " + SECONDARY_BUTTON_STYLE_FLAT + " } "
        "QPushButton:default { " + SECONDARY_BUTTON_STYLE_DEFAULT + " }";

    secondaryBtnStyleSelected = "QPushButton { " + SECONDARY_BUTTON_STYLE_SELECTED +  " } "
        "QPushButton:hover { " + SECONDARY_BUTTON_STYLE_SELECTED_HOVER + " } "
        "QPushButton:pressed { " + SECONDARY_BUTTON_STYLE_PRESSED + " } "
        "QPushButton:flat { " + SECONDARY_BUTTON_STYLE_FLAT + " } "
        "QPushButton:default { " + SECONDARY_BUTTON_STYLE_DEFAULT + " }";

    secondaryBtnStyleSelectedDisabled = "QPushButton { " + SECONDARY_BUTTON_STYLE_DISABLED +  " } "
        "QPushButton:hover { " + SECONDARY_BUTTON_STYLE_SELECTED_HOVER + " } "
        "QPushButton:pressed { " + SECONDARY_BUTTON_STYLE_PRESSED + " } "
        "QPushButton:flat { " + SECONDARY_BUTTON_STYLE_FLAT + " } "
        "QPushButton:default { " + SECONDARY_BUTTON_STYLE_DEFAULT + " }";

    checkboxStyle = "QCheckBox { margin-left: 10px; margin-right: 10px; padding: 5px; font-size: " + DEFAULT_FONT_SIZE_PX + "px; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; color: " + SECONDARY_COLOR_HEX + "; } "
        "QCheckBox:hover { color: " + SECONDARY_COLOR_LIGHTER_HEX + "; border: 2px solid transparent; border-radius: 6px; } "
        "QCheckBox::indicator { width: 25px; height: 25px; } "
        "QCheckBox::indicator:unchecked { image: url(" + imgRsrcPath + "/checkbox_unchecked.png); } "
        "QCheckBox::indicator:unchecked:hover { image: url(" + imgRsrcPath + "/checkbox_unchecked_hover.png); } "
        "QCheckBox::indicator:unchecked:pressed { image: url(" + imgRsrcPath + "/checkbox_unchecked_pressed.png); } "
        "QCheckBox::indicator:checked { image: url(" + imgRsrcPath + "/checkbox_checked.png); } "
        "QCheckBox::indicator:checked:hover { image: url(" + imgRsrcPath + "/checkbox_checked_hover.png); } "
        "QCheckBox::indicator:checked:pressed { image: url(" + imgRsrcPath + "/checkbox_checked_pressed.png); } "
        "QCheckBox::indicator:indeterminate:hover { image: url(" + imgRsrcPath + "/checkbox_indeterminated_hover.png); } "
        "QCheckBox::indicator:indeterminate:pressed { image: url(" + imgRsrcPath + "/checkbox_indeterminated_pressed.png); }";

    menuBarStyle = "QMenuBar { font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; font-weight: bold; font-size: " + DEFAULT_FONT_SIZE_PX + "px; color: " + SECONDARY_COLOR_HEX + "; background-color: " + PRIMARY_COLOR_LIGHT_HEX + "; border-top: 1px solid " + SECONDARY_COLOR_HEX + "; border-left: 1px solid " + SECONDARY_COLOR_HEX + "; border-right: 1px solid " + SECONDARY_COLOR_HEX + "; border-bottom: 2px solid " + SECONDARY_COLOR_HEX + "; border-top-left-radius: 10px; border-top-right-radius: 10px; padding: 0; margin: 0; } "
#ifdef Q_OS_WINDOWS
            "QMenuBar::item { padding: 9px 14px 8px 14px; margin-top: 0px; margin-bottom: 0px; border: 0px solid transparent; } "
#else
            "QMenuBar::item { padding: 10px 14px 10px 14px; margin-top: 0px; margin-bottom: 0px; border: 0px solid transparent; } "
#endif
            "QMenuBar::item:selected { background-color: " + PRIMARY_COLOR_HEX + "; } "
            "QMenuBar::item:pressed { background-color: " + PRIMARY_COLOR_DEEP_HEX + "; } "
            "QMenu { background-color: " + PRIMARY_COLOR_DEEP_HEX + "; color: " + SECONDARY_COLOR_HEX + "; padding: 5px; } "
            "QMenu::icon { padding-left: 5px; } "
            "QMenu::item { color: " + SECONDARY_COLOR_HEX + "; min-width: 100px; font-weight: 600; font-size: " + SM_FONT_SIZE_PX + "px; padding: 7px; border: 0px solid transparent; border-radius: 6px; } "
            "QMenu::item:selected { background-color: " + PRIMARY_COLOR_LIGHT_HEX + "; }";
    statusBarStyle = "QStatusBar { background-color: " + PRIMARY_COLOR_LIGHT_HEX + "; border: 1px solid " + SECONDARY_COLOR_HEX + "; border-bottom-left-radius: 10px; border-bottom-right-radius: 10px; font-style: italic; color: " + NEUTRAL_COLOR_LIGHT_HEX + "; } "
        "QStatusBar::item { font-style: italic; color: " + NEUTRAL_COLOR_LIGHT_HEX + "; } "
        "QSizeGrip { padding-right: 5px; padding-bottom: 5px; }";
}

void Assets::resetStyles()
{
    PRIMARY_FONT_FAMILY = Assets::primaryFontFamily;

    SM_FONT_SIZE_PX = QString("14");
    DEFAULT_FONT_SIZE_PX = QString("16");
    XL_FONT_SIZE_PX = QString("20");
    XXL_FONT_SIZE_PX = QString("24");

    PRIMARY_COLOR_HEX = QString("#FFC65A");
    PRIMARY_COLOR_LIGHT_HEX = QString("#FFE4B0");
    PRIMARY_COLOR_DEEP_HEX = QString("#FFAC0E");

    ALT_PRIMARY_COLOR_HEX = QString("#E67200");
    ALT_PRIMARY_COLOR_LIGHT_HEX = QString("#FFA940");
    ALT_PRIMARY_COLOR_DEEP_HEX = QString("#BB4600");

    SECONDARY_COLOR_HEX = QString("#152340");
    SECONDARY_COLOR_LIGHT_HEX = QString("#28457D");
    SECONDARY_COLOR_LIGHTER_HEX = QString("#285AA0");
    SECONDARY_COLOR_DEEP_HEX = QString("#022C8C");

    ALT_SECONDARY_COLOR_HEX = QString("#24425C");
    ALT_SECONDARY_COLOR_LIGHT_HEX = QString("#3393BA");

    NEUTRAL_COLOR_HEX = QString("#373737");
    NEUTRAL_COLOR_LIGHT_HEX = QString("#565656");
    NEUTRAL_COLOR_LIGHTER_HEX = QString("#6C6C6C");

    LIGHT_TEXT_COLOR_HEX = QString("#FFFFFF");

    updateStyles();
}
