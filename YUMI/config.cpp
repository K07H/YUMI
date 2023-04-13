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

#include <QApplication>
#include <QtDebug>
#include <QFile>
#include <QRegularExpression>
#include "config.h"
#include "yumi.h"
#include "assets.h"

Config* Config::_instance = NULL;
QTranslator* Config::_frenchTranslator = NULL;
QTranslator* Config::_frenchBaseTranslator = NULL;
bool Config::_isFrenchTranslatorInstalled = false;
QTranslator* Config::_turkishTranslator = NULL;
QTranslator* Config::_turkishBaseTranslator = NULL;
bool Config::_isTurkishTranslatorInstalled = false;

Config::Config(void* yumiPtr)
{
    this->configChanged = false;
    this->checkedNoticesAndLicense = false;
    this->logScriptErrors = true;
    this->checkVersionAtStartup = true;
    this->downloadRequestTimeout = DEFAULT_DOWNLOAD_REQUEST_TIMEOUT;
    this->getRequestTimeout = DEFAULT_GET_REQUEST_TIMEOUT;
    this->language = DEFAULT_LANGUAGE;
    this->_frenchTranslator = NULL;
    this->_frenchBaseTranslator = NULL;
    this->_isFrenchTranslatorInstalled = false;
    this->_yumiPtr = yumiPtr;
    this->loadThemePresets();
}

Config* Config::Init(void* yumiPtr)
{
    if (_instance == NULL)
        _instance = new Config(yumiPtr);
    return _instance;
}

Config* Config::Instance()
{
    return _instance;
}

const QString Config::getConfigFilepath()
{
    qDebug().nospace() << "App path is " << yumi::appPath << ".";
    return QDir(yumi::appPath).absoluteFilePath(CONFIGURATION_FILENAME);
}

const QString Config::getThemesFilepath()
{
    return QDir(yumi::appPath).absoluteFilePath(THEMES_FILENAME);
}

QString Config::getExeTypeLabel(const int exeType)
{
    if (exeType == WIN_GAME_X64)
        return "WinX64";
    if (exeType == WIN_GAME_X86)
        return "WinX86";
    if (exeType == UNIX_GAME_X64)
        return "UnixX64";
    if (exeType == UNIX_GAME_X86)
        return "UnixX86";
    if (exeType == DARWIN_GAME)
        return "Darwin";
    if (exeType == WIN_GAME_IL2CPP_X64)
        return "WinX64_IL2CPP";
    if (exeType == WIN_GAME_IL2CPP_X86)
        return "WinX86_IL2CPP";
    return "Unknown";
}

QString Config::getBepInExNameFromExeType(int exeType)
{
    if (exeType == WIN_GAME_X64)
        return WINX64_BEPINEX_NAME;
    else if (exeType == WIN_GAME_X86)
        return WINX86_BEPINEX_NAME;
    else if (exeType == UNIX_GAME_X64)
        return UNIX_BEPINEX_NAME;
    else if (exeType == UNIX_GAME_X86)
        return UNIX_BEPINEX_NAME;
    else if (exeType == DARWIN_GAME)
        return UNIX_BEPINEX_NAME;
    else if (exeType == WIN_GAME_IL2CPP_X64)
        return IL2CPPX64_BEPINEX_NAME;
    else if (exeType == WIN_GAME_IL2CPP_X86)
        return IL2CPPX86_BEPINEX_NAME;
    else
        return "";
}

void Config::parseGameLine(const QString& gameLine)
{
    if (gameLine.isEmpty() || !gameLine.contains("#;#"))
        return;
    QStringList elems = gameLine.split("#;#");
    if (elems.count() == 4)
    {
        if (elems[0].isEmpty())
        {
            qWarning().nospace() << "Found a game without valid name in configuration file. Skipping it.";
            return;
        }
        bool folderNotFound = true;
        if (!elems[1].isEmpty())
        {
            QDir gameFolder = QDir(elems[1]);
            if (gameFolder.exists())
                folderNotFound = false;
        }
        if (folderNotFound)
        {
            qWarning().nospace() << "Found a game without valid folder path in configuration file. Skipping it. GameName=" << elems[0] << " GamePath=" << elems[1] << ".";
            return;
        }
        if (elems[2].isEmpty())
        {
            qWarning().nospace() << "Found a game without valid executable path in configuration file. Skipping it. GameName=" << elems[0] << " GamePath=" << elems[1] << ".";
            return;
        }
        if (elems[3].isEmpty())
        {
            qWarning().nospace() << "Found a game without executable type in configuration file. Skipping it. GameName=" << elems[0] << " GamePath=" << elems[1] << " ExePath=" << elems[2] << ".";
            return;
        }
        bool isValidInt;
        int exeType = elems[3].toInt(&isValidInt);
        if (!isValidInt)
        {
            qWarning().nospace() << "Found a game without valid executable type in configuration file. Skipping it. GameName=" << elems[0] << " GamePath=" << elems[1] << " ExePath=" << elems[2] << " ExeType=" << elems[3] << ".";
            return;
        }
        ((yumi*)_yumiPtr)->gamesInfo.append(GameInfo(elems[0], elems[1], elems[2], exeType));
        qDebug().nospace() << "Successfully loaded game " << elems[0] << " from configuration file. GamePath=" << elems[1] << " ExeType=" << getExeTypeLabel(exeType) << " ExePath=" << elems[2] << ".";
    }
}

bool Config::SwapLanguage(const QString& lang, QApplication* app)
{
    if (lang.compare("English") == 0)
    {
        if (_isFrenchTranslatorInstalled && app != NULL)
        {
            if (_frenchBaseTranslator != NULL)
                app->removeTranslator(_frenchBaseTranslator);
            if (_frenchTranslator != NULL)
                app->removeTranslator(_frenchTranslator);
        }
        if (_isTurkishTranslatorInstalled && app != NULL)
        {
            if (_turkishBaseTranslator != NULL)
                app->removeTranslator(_turkishBaseTranslator);
            if (_turkishTranslator != NULL)
                app->removeTranslator(_turkishTranslator);
        }
        _isFrenchTranslatorInstalled = false;
        _isTurkishTranslatorInstalled = false;
        return true;
    }
    else if (lang.compare("Français") == 0)
    {
        if (!_isFrenchTranslatorInstalled)
        {
            if (_isTurkishTranslatorInstalled && app != NULL)
            {
                if (_turkishBaseTranslator != NULL)
                    app->removeTranslator(_turkishBaseTranslator);
                if (_turkishTranslator != NULL)
                    app->removeTranslator(_turkishTranslator);
            }
            _isTurkishTranslatorInstalled = false;

            if (_frenchTranslator == NULL)
            {
                _frenchTranslator = new QTranslator();
                _frenchTranslator->load(Assets::Instance()->frenchTranslation);
            }
            if (_frenchBaseTranslator == NULL)
            {
                _frenchBaseTranslator = new QTranslator();
                _frenchBaseTranslator->load(Assets::Instance()->frenchBaseTranslation);
            }
            if (_frenchTranslator != NULL && app != NULL)
                app->installTranslator(_frenchTranslator);
            if (_frenchBaseTranslator != NULL && app != NULL)
                app->installTranslator(_frenchBaseTranslator);
        }
        _isFrenchTranslatorInstalled = true;
        return true;
    }
    else if (lang.compare("Türk") == 0 || lang.compare("Türkçe") == 0)
    {
        if (!_isTurkishTranslatorInstalled)
        {
            if (_isFrenchTranslatorInstalled && app != NULL)
            {
                if (_frenchBaseTranslator != NULL)
                    app->removeTranslator(_frenchBaseTranslator);
                if (_frenchTranslator != NULL)
                    app->removeTranslator(_frenchTranslator);
            }
            _isFrenchTranslatorInstalled = false;

            if (_turkishTranslator == NULL)
            {
                _turkishTranslator = new QTranslator();
                _turkishTranslator->load(Assets::Instance()->turkishTranslation);
            }
            if (_turkishBaseTranslator == NULL)
            {
                _turkishBaseTranslator = new QTranslator();
                _turkishBaseTranslator->load(Assets::Instance()->turkishBaseTranslation);
            }
            if (_turkishTranslator != NULL && app != NULL)
                app->installTranslator(_turkishTranslator);
            if (_turkishBaseTranslator != NULL && app != NULL)
                app->installTranslator(_turkishBaseTranslator);
        }
        _isTurkishTranslatorInstalled = true;
        return true;
    }
    return false;
}

int Config::loadConfig()
{
    QString configFilepath = getConfigFilepath();
    qDebug().nospace() << "Loading configuration file located at " << configFilepath << "...";

    QFile file(configFilepath);
    if (!file.exists())
    {
        qDebug().nospace() << "Configuration file not found. Skipping load.";
        return 0;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::ExistingOnly | QIODevice::Text))
    {
        qWarning().nospace() << "Failed to open configuration file located at " << configFilepath << ". Skipping load.";
        return -1;
    }

    QByteArray data;
    try
    {
        data = file.readAll();
        file.close();
    }
    catch (const std::exception& ex)
    {
        file.close();
        qWarning().nospace() << "Exception caught while reading configuration file located at " << configFilepath << ". Skipping load. Exception=[" << ex.what() << "]";
        return -1;
    }
    catch (...)
    {
        file.close();
        qWarning().nospace() << "Exception caught while reading configuration file located at " << configFilepath << ". Skipping load.";
        return -1;
    }

    QString fileTextUtf8 = QString::fromUtf8(data);
    if (fileTextUtf8.length() <= 0)
    {
        qDebug().nospace() << "Configuration file is empty. Skipping load.";
        return -1;
    }
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    QStringList lines = fileTextUtf8.replace("\r\n", "\n").split('\n', QString::SkipEmptyParts);
#else
    QStringList lines = fileTextUtf8.replace("\r\n", "\n").split('\n', Qt::SkipEmptyParts);
#endif
    if (lines.count() <= 0)
    {
        qDebug().nospace() << "No valid lines were found in configuration file. Skipping load.";
        return -1;
    }

    int i = 0;
    QString gameDelimiter("Game=");
    int gameDelimiterLen = gameDelimiter.length();
    QString licenseDelimiter("LicenseChecked=");
    int licenseDelimiterLen = licenseDelimiter.length();
    QString languageDelimiter("Language=");
    int languageDelimiterLen = languageDelimiter.length();
    QString themeDelimiter("Theme=");
    int themeDelimiterLen = themeDelimiter.length();
    QString logScriptErrorsDelimiter("LogScriptErrors=");
    int logScriptErrorsDelimiterLen = logScriptErrorsDelimiter.length();
    QString checkVersionDelimiter("CheckVersionAtStartup=");
    int checkVersionDelimiterLen = checkVersionDelimiter.length();
    QString downloadRequestTimeoutDelimiter("DownloadRequestTimeout=");
    int downloadRequestTimeoutDelimiterLen = downloadRequestTimeoutDelimiter.length();
    QString getRequestTimeoutDelimiter("GetRequestTimeout=");
    int getRequestTimeoutDelimiterLen = getRequestTimeoutDelimiter.length();
    for (const QString& line : lines)
    {
        i++;
        QString trimmedLine(line.trimmed());
        qDebug().nospace() << "Parsing line " << i << ": " << trimmedLine;

        int trimmedLineLen = trimmedLine.length();
        if (trimmedLineLen > gameDelimiterLen && trimmedLine.startsWith(gameDelimiter) && trimmedLine.contains(';'))
            parseGameLine(trimmedLine.mid(gameDelimiterLen));
        if (trimmedLineLen > licenseDelimiterLen && trimmedLine.startsWith(licenseDelimiter))
        {
            QString lineVal(trimmedLine.mid(licenseDelimiterLen));
            if (lineVal.compare("true", Qt::CaseInsensitive) == 0)
                checkedNoticesAndLicense = true;
        }
        if (trimmedLineLen > languageDelimiterLen && trimmedLine.startsWith(languageDelimiter))
        {
            QString lineVal(trimmedLine.mid(languageDelimiterLen));
            if (!lineVal.isEmpty())
            {
                if (lineVal.compare("Türk", Qt::CaseInsensitive) == 0)
                    lineVal = "Türkçe";
                if (lineVal.compare("English", Qt::CaseInsensitive) == 0 || lineVal.compare("Français", Qt::CaseInsensitive) == 0 || lineVal.compare("Türkçe", Qt::CaseInsensitive) == 0)
                    language = lineVal;
            }
        }
        if (trimmedLineLen > themeDelimiterLen && trimmedLine.startsWith(themeDelimiter))
        {
            QString lineVal(trimmedLine.mid(themeDelimiterLen));
            if (!lineVal.isEmpty())
            {
                ThemeInfo ti = this->getThemePreset(lineVal);
                if (ti.isValid)
                    ((yumi*)_yumiPtr)->theme = lineVal;
                else
                    qWarning() << "Wrong theme name found in configuration file. Default theme will be used.";
            }
        }
        if (trimmedLineLen > logScriptErrorsDelimiterLen && trimmedLine.startsWith(logScriptErrorsDelimiter))
        {
            QString lineVal(trimmedLine.mid(logScriptErrorsDelimiterLen));
            this->logScriptErrors = (lineVal.compare("true", Qt::CaseInsensitive) == 0);
        }
        if (trimmedLineLen > checkVersionDelimiterLen && trimmedLine.startsWith(checkVersionDelimiter))
        {
            QString lineVal(trimmedLine.mid(checkVersionDelimiterLen));
            this->checkVersionAtStartup = (lineVal.compare("true", Qt::CaseInsensitive) == 0);
        }
        if (trimmedLineLen > downloadRequestTimeoutDelimiterLen && trimmedLine.startsWith(downloadRequestTimeoutDelimiter))
        {
            QString lineVal(trimmedLine.mid(downloadRequestTimeoutDelimiterLen));
            bool ok;
            int timeoutVal = lineVal.toInt(&ok);
            if (ok)
            {
                if (timeoutVal >= 5 && timeoutVal <= 10800)
                    this->downloadRequestTimeout = timeoutVal;
                else
                    qWarning() << "Wrong download request timeout value in configuration file (it should be between 5 and 10800 seconds).";
            }
            else
                qWarning() << "Wrong download request timeout value in configuration file (value is not a valid number).";
        }
        if (trimmedLineLen > getRequestTimeoutDelimiterLen && trimmedLine.startsWith(getRequestTimeoutDelimiter))
        {
            QString lineVal(trimmedLine.mid(getRequestTimeoutDelimiterLen));
            bool ok;
            int timeoutVal = lineVal.toInt(&ok);
            if (ok)
            {
                if (timeoutVal >= 5 && timeoutVal <= 300)
                    this->getRequestTimeout = timeoutVal;
                else
                    qWarning() << "Wrong get request timeout value in configuration file (it should be between 5 and 300 seconds).";
            }
            else
                qWarning() << "Wrong get request timeout value in configuration file (value is not a valid number).";
        }
    }

    int nbGamesLoaded = ((yumi*)_yumiPtr)->gamesInfo.count();
    if (nbGamesLoaded > 0 && ((yumi*)_yumiPtr)->mainWidget != NULL)
        for (int j = 0; j < nbGamesLoaded; j++)
            ((yumi*)_yumiPtr)->mainWidget->sideMenu->addAction(((yumi*)_yumiPtr)->gamesInfo[j]);

    qDebug().nospace() << "Configuration file has been parsed. " << nbGamesLoaded << " games were added to games list.";

    if (language.compare(DEFAULT_LANGUAGE, Qt::CaseInsensitive) != 0)
    {
        Config::SwapLanguage(language, ((yumi*)_yumiPtr)->appPtr);
    }

    return nbGamesLoaded;
}

bool Config::saveConfig()
{
    QString configFilepath = getConfigFilepath();
    qDebug().nospace() << "Saving configuration to " << configFilepath << "...";

    QString toSave = ("Language=" + this->language + "\nTheme=" + ((yumi*)_yumiPtr)->theme + "\n");

    if (this->logScriptErrors)
        toSave += "LogScriptErrors=true\n";
    else
        toSave += "LogScriptErrors=false\n";

    if (this->checkVersionAtStartup)
        toSave += "CheckVersionAtStartup=true\n";
    else
        toSave += "CheckVersionAtStartup=false\n";

    if (this->checkedNoticesAndLicense)
        toSave += "LicenseChecked=true\n";

    toSave += "DownloadRequestTimeout=" + QString::number(this->downloadRequestTimeout) + "\n";

    toSave += "GetRequestTimeout=" + QString::number(this->getRequestTimeout) + "\n";

    int nbGames = ((yumi*)_yumiPtr)->gamesInfo.count();
    if (nbGames <= 0)
        qDebug().nospace() << "No valid games were found in the list.";

    int nbGamesSaved = 0;
    if (nbGames > 0)
    {
        QList<GameInfo> games(((yumi*)_yumiPtr)->gamesInfo);
        for (const GameInfo& gi : games)
        {
            nbGamesSaved++;
            toSave += "Game=" + gi.name + "#;#" + gi.path + "#;#" + gi.exePath + "#;#" + QString::number(gi.exeType) + "\n";
        }
    }

    QFile file(configFilepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qWarning().nospace() << "Failed to open configuration file located at " << configFilepath << " for writing. Skipping save.";
        return false;
    }
    if (!file.isWritable())
    {
        file.close();
        qWarning().nospace() << "Configuration file located at " << configFilepath << " is not writable. Skipping save.";
        return false;
    }
    try
    {
        file.write(toSave.toUtf8());
        file.close();
    }
    catch (const std::exception& ex)
    {
        file.close();
        qWarning().nospace() << "Exception caught while writing to configuration file at " << configFilepath << ". Skipping save. Exception=[" << ex.what() << "]";
        return false;
    }
    catch (...)
    {
        file.close();
        qWarning().nospace() << "Exception caught while writing to configuration file at " << configFilepath << ". Skipping save.";
        return false;
    }
    qDebug().nospace() << "Configuration file has been saved to " << configFilepath << ". " << nbGamesSaved << " games info were saved.";
    return true;
}

void Config::loadThemePresets()
{
    if (_themePresets.count() > 0)
        return;

    ThemeInfo defaultTheme;
    defaultTheme.name = "Default";
    defaultTheme.isValid = true;
    defaultTheme.primaryFontFamily = Assets::primaryFontFamily;
    defaultTheme.smallFontSizePx = "14";
    defaultTheme.regularFontSizePx = "16";
    defaultTheme.largeFontSizePx = "20";
    defaultTheme.extraLargeFontSizePx = "24";
    defaultTheme.primaryColorHex = "#FFC65A";
    defaultTheme.primaryColorLightHex = "#FFE4B0";
    defaultTheme.primaryColorDeepHex = "#FFAC0E";
    defaultTheme.altPrimaryColorHex = "#E67200";
    defaultTheme.altPrimaryColorLightHex = "#FFA940";
    defaultTheme.altPrimaryColorDeepHex = "#BB4600";
    defaultTheme.secondaryColorHex = "#152340";
    defaultTheme.secondaryColorLightHex = "#28457D";
    defaultTheme.secondaryColorLighterHex = "#285AA0";
    defaultTheme.secondaryColorDeepHex = "#022C8C";
    defaultTheme.altSecondaryColorHex = "#24425C";
    defaultTheme.altSecondaryColorLightHex = "#3393BA";
    defaultTheme.neutralColorHex = "#373737";
    defaultTheme.neutralColorLightHex = "#565656";
    defaultTheme.neutralColorLighterHex = "#6C6C6C";
    defaultTheme.alternateTextColorHex = "#FFFFFF";
    _themePresets.push_back(defaultTheme);

    QString themesFilepath = getThemesFilepath();
    qDebug().nospace() << "Loading themes file located at " << themesFilepath << "...";
    QFile file(themesFilepath);
    if (!file.exists())
    {
        qDebug().nospace() << "Themes file not found. Skipping load.";
        return;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::ExistingOnly | QIODevice::Text))
    {
        qWarning().nospace() << "Failed to open themes file located at " << themesFilepath << ". Skipping load.";
        return;
    }

    QByteArray data;
    try
    {
        data = file.readAll();
        file.close();
    }
    catch (const std::exception& ex)
    {
        file.close();
        qWarning().nospace() << "Exception caught while reading themes file located at " << themesFilepath << ". Skipping load. Exception=[" << ex.what() << "]";
        return;
    }
    catch (...)
    {
        file.close();
        qWarning().nospace() << "Exception caught while reading themes file located at " << themesFilepath << ". Skipping load.";
        return;
    }

    QString fileTextUtf8 = QString::fromUtf8(data);
    if (fileTextUtf8.length() <= 0)
    {
        qDebug().nospace() << "Themes file is empty. Skipping load.";
        return;
    }
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    QStringList lines = fileTextUtf8.replace("\r\n", "\n").split('\n', QString::SkipEmptyParts);
#else
    QStringList lines = fileTextUtf8.replace("\r\n", "\n").split('\n', Qt::SkipEmptyParts);
#endif
    if (lines.count() <= 0)
    {
        qDebug().nospace() << "No valid lines were found in themes file. Skipping load.";
        return;
    }

    int i = 0;
    QString themeDelimiter("Preset=");
    int themeDelimiterLen = themeDelimiter.length();
    for (const QString& line : lines)
    {
        i++;
        QString trimmedLine(line.trimmed());
        qDebug().nospace() << "Parsing line " << i << ": " << trimmedLine;

        int trimmedLineLen = trimmedLine.length();
        if (trimmedLineLen > themeDelimiterLen && trimmedLine.startsWith(themeDelimiter) && trimmedLine.contains("#;#"))
        {
            ThemeInfo current = deserializeThemePreset(trimmedLine.mid(themeDelimiterLen));
            if (current.isValid)
            {
                bool nameAlreadyPresent = false;
                int tmpLen = this->_themePresets.count();
                for (int j = 0; j < tmpLen; j++)
                    if (this->_themePresets[j].name.compare(current.name) == 0)
                    {
                        nameAlreadyPresent = true;
                        break;
                    }
                if (!nameAlreadyPresent)
                    this->_themePresets.push_back(current);
                else
                    qWarning().nospace() << "Skipping theme preset line (name already exists): " << trimmedLine;
            }
            else
                qWarning().nospace() << "Skipping incorrect theme preset line: " << trimmedLine;
        }
    }

    int nbThemesLoaded = this->_themePresets.count() - 1;
    qDebug().nospace() << "Themes file has been parsed. " << nbThemesLoaded << " themes were added to themes list.";
}

void Config::saveThemePresets()
{
    QString themesFilepath = getThemesFilepath();
    qDebug().nospace() << "Saving themes to " << themesFilepath << "...";

    QString toSave = "";
    int themesLen = this->_themePresets.count();
    if (themesLen <= 1)
    {
        qDebug().nospace() << "No additional themes were found in the list. Skipping save.";
        return;
    }

    int nbThemesSaved = 0;
    for (int i = 0; i < themesLen; i++)
        if (this->_themePresets[i].name.compare("Default") != 0 && this->_themePresets[i].isValid)
        {
            nbThemesSaved++;
            toSave += ("Preset=" + serializeThemePreset(this->_themePresets[i]) + "\n");
        }

    QFile file(themesFilepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qWarning().nospace() << "Failed to open themes file located at " << themesFilepath << " for writing. Skipping save.";
        return;
    }
    if (!file.isWritable())
    {
        file.close();
        qWarning().nospace() << "Themes file located at " << themesFilepath << " is not writable. Skipping save.";
        return;
    }
    try
    {
        file.write(toSave.toUtf8());
        file.close();
    }
    catch (const std::exception& ex)
    {
        file.close();
        qWarning().nospace() << "Exception caught while writing to themes file at " << themesFilepath << ". Skipping save. Exception=[" << ex.what() << "]";
        return;
    }
    catch (...)
    {
        file.close();
        qWarning().nospace() << "Exception caught while writing to themes file at " << themesFilepath << ". Skipping save.";
        return;
    }
    qDebug().nospace() << "Themes file has been saved to " << themesFilepath << ". " << nbThemesSaved << " themes were saved.";
}

const QList<ThemeInfo>& Config::getThemePresets()
{
    return this->_themePresets;
}

ThemeInfo Config::getThemePreset(const QString& themeName)
{
    if (themeName.isEmpty())
    {
        ThemeInfo error;
        error.isValid = false;
        error.name = "";
        return error;
    }

    int nbThemes = this->_themePresets.count();
    if (nbThemes > 0)
        for (int i = 0; i < nbThemes; i++)
            if (themeName.compare(this->_themePresets[i].name) == 0)
                return this->_themePresets[i];

    ThemeInfo error;
    error.isValid = false;
    error.name = "";
    return error;
}

void Config::addTheme(const ThemeInfo& theme)
{
    if (theme.isValid && !theme.name.isEmpty())
    {
        this->_themePresets.push_back(theme);
        this->saveThemePresets();
    }
}

void Config::removeTheme(const QString& themeName)
{
    if (themeName.isEmpty())
        return;
    int toDelete = -1;
    int nbThemes = this->_themePresets.count();
    if (nbThemes > 0)
        for (int i = 0; i < nbThemes; i++)
            if (themeName.compare(this->_themePresets[i].name) == 0)
            {
                toDelete = i;
                break;
            }
    if (toDelete >= 0)
        this->_themePresets.removeAt(toDelete);
}

QString Config::serializeThemePreset(const ThemeInfo& theme)
{
    return theme.smallFontSizePx + "#;#" +
           theme.regularFontSizePx + "#;#" +
           theme.largeFontSizePx + "#;#" +
           theme.extraLargeFontSizePx + "#;#" +
           theme.primaryColorHex + "#;#" +
           theme.primaryColorLightHex + "#;#" +
           theme.primaryColorDeepHex + "#;#" +
           theme.altPrimaryColorHex + "#;#" +
           theme.altPrimaryColorLightHex + "#;#" +
           theme.altPrimaryColorDeepHex + "#;#" +
           theme.secondaryColorHex + "#;#" +
           theme.secondaryColorLightHex + "#;#" +
           theme.secondaryColorLighterHex + "#;#" +
           theme.secondaryColorDeepHex + "#;#" +
           theme.altSecondaryColorHex + "#;#" +
           theme.altSecondaryColorLightHex + "#;#" +
           theme.neutralColorHex + "#;#" +
           theme.neutralColorLightHex + "#;#" +
           theme.neutralColorLighterHex + "#;#" +
           theme.alternateTextColorHex + "#;#" +
           theme.primaryFontFamily + "#;#" +
           theme.name;
}

ThemeInfo Config::deserializeThemePreset(const QString& theme)
{
    ThemeInfo result;
    result.isValid = false;
    if (theme.isEmpty())
        return result;
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    QStringList splitted = theme.split("#;#", QString::SkipEmptyParts);
#else
    QStringList splitted = theme.split("#;#", Qt::SkipEmptyParts);
#endif
    if (splitted.count() != 22)
        return result;
    for (int i = 0; i < 22; i++)
        if (splitted[i].isEmpty())
            return result;
    result.smallFontSizePx = splitted[0];
    result.regularFontSizePx = splitted[1];
    result.largeFontSizePx = splitted[2];
    result.extraLargeFontSizePx = splitted[3];
    result.primaryColorHex = splitted[4];
    result.primaryColorLightHex = splitted[5];
    result.primaryColorDeepHex = splitted[6];
    result.altPrimaryColorHex = splitted[7];
    result.altPrimaryColorLightHex = splitted[8];
    result.altPrimaryColorDeepHex = splitted[9];
    result.secondaryColorHex = splitted[10];
    result.secondaryColorLightHex = splitted[11];
    result.secondaryColorLighterHex = splitted[12];
    result.secondaryColorDeepHex = splitted[13];
    result.altSecondaryColorHex = splitted[14];
    result.altSecondaryColorLightHex = splitted[15];
    result.neutralColorHex = splitted[16];
    result.neutralColorLightHex = splitted[17];
    result.neutralColorLighterHex = splitted[18];
    result.alternateTextColorHex = splitted[19];
    result.primaryFontFamily = splitted[20];
    result.name = splitted[21];
    result.isValid = true;
    return result;
}
