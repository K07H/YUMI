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

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <QString>
#include <QList>
#include <QTranslator>
#define CONFIGURATION_FILENAME "YUMI_Config.txt"
#define THEMES_FILENAME "YUMI_Themes.txt"
#define DEFAULT_LANGUAGE "English"

struct ThemeInfo
{
    QString name;
    bool isValid;
    QString smallFontSizePx;
    QString regularFontSizePx;
    QString largeFontSizePx;
    QString extraLargeFontSizePx;
    QString primaryColorHex;
    QString primaryColorLightHex;
    QString primaryColorDeepHex;
    QString altPrimaryColorHex;
    QString altPrimaryColorLightHex;
    QString altPrimaryColorDeepHex;
    QString secondaryColorHex;
    QString secondaryColorLightHex;
    QString secondaryColorLighterHex;
    QString secondaryColorDeepHex;
    QString altSecondaryColorHex;
    QString altSecondaryColorLightHex;
    QString neutralColorHex;
    QString neutralColorLightHex;
    QString neutralColorLighterHex;
    QString alternateTextColorHex;
    QString primaryFontFamily;
};

class Config
{
private:
    Config() = delete;
    Config(const Config& other) = delete;
    Config& operator=(const Config&) = delete;
    Config(void* yumiPtr);
    void loadThemePresets();

    static Config* _instance;
    static QTranslator* _frenchTranslator;
    static QTranslator* _frenchBaseTranslator;
    static bool _isFrenchTranslatorInstalled;
    static QTranslator* _turkishTranslator;
    static QTranslator* _turkishBaseTranslator;
    static bool _isTurkishTranslatorInstalled;

    void* _yumiPtr;
    QList<ThemeInfo> _themePresets;

public:
    static Config* Init(void* yumiPtr);
    static Config* Instance();
    static bool SwapLanguage(const QString& lang, QApplication* app);

    const QString getConfigFilepath();
    const QString getThemesFilepath();
    QString getExeTypeLabel(const int exeType);
    QString getBepInExNameFromExeType(int exeType);
    void parseGameLine(const QString& gameLine);
    int loadConfig();
    bool saveConfig();
    void saveThemePresets();
    const QList<ThemeInfo>& getThemePresets();
    ThemeInfo getThemePreset(const QString& themeName);
    void addTheme(const ThemeInfo& theme);
    void removeTheme(const QString& themeName);
    QString serializeThemePreset(const ThemeInfo& theme);
    ThemeInfo deserializeThemePreset(const QString& theme);

    bool configChanged;
    bool checkedNoticesAndLicense;
    bool logScriptErrors;
    bool checkVersionAtStartup;
    int downloadRequestTimeout;
    int getRequestTimeout;
    QString language;
};

#endif
