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
#include <QDebug>
#include <QStyleOption>
#include <QPainter>
#include <QPainterPath>
#include <QStringList>
#include <QFontDatabase>
#include <QMessageBox>
#include "yumi_settings.h"
#include "common.h"
#include "assets.h"
#include "yumi.h"
#include "utils.h"

YumiSettings::YumiSettings(void* yumiPtr, QWidget* parent) : QWidget(parent)
{
    this->_yumiPtr = yumiPtr;
    this->_themeChanged = false;
    this->_windowMoving = false;
    this->_saveThemePopup = NULL;

    QString windowTitle(QCoreApplication::translate("Settings", "YUMI  Settings", "Window title"));

    this->setWindowTitle(windowTitle);
    this->setWindowIcon(Assets::Instance()->appIcon);
    this->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_NoSystemBackground, true);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setContentsMargins(0, 0, 0, 0);

    this->_title = new WindowTitle(windowTitle, this);

    this->_generalSettingsLabel = new QLabel(QCoreApplication::translate("Settings", "General settings", "Label text"));
    this->_generalSettingsLabel->setStyleSheet("QLabel { margin-left: 5px; " + Assets::Instance()->SMALLER_TITLE_LABEL_STYLE + " }");

    this->_languageLabel = new QLabel(QCoreApplication::translate("Settings", "Language:", "Label text"));
    this->_languageLabel->setStyleSheet("QLabel { " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");

    this->_languageComboBox = new QComboBox();
    this->_languageComboBox->addItem(Assets::Instance()->englishFlagIcon, "English");
    this->_languageComboBox->addItem(Assets::Instance()->frenchFlagIcon, "Français");
    this->_languageComboBox->addItem(Assets::Instance()->turkishFlagIcon, "Türk");
    this->_languageComboBox->setCurrentText(Config::Instance()->language);
    this->_languageComboBox->setCursor(Qt::PointingHandCursor);
    this->_languageComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    connect(this->_languageComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(languageChanged(QString)));

    this->_languageLayout = new QHBoxLayout();
    this->_languageLayout->setContentsMargins(10, 0, 10, 0);
    this->_languageLayout->setSpacing(0);
    this->_languageLayout->addWidget(this->_languageLabel);
    this->_languageLayout->addSpacing(5);
    this->_languageLayout->addWidget(this->_languageComboBox);

    this->_logScriptErrors = new QCheckBox(QCoreApplication::translate("Settings", "Log launcher script errors to file \"run_yumi_bepinex.err\".", "Checkbox label text"));
    this->_logScriptErrors->setStyleSheet(Assets::Instance()->checkboxStyle);
    this->_logScriptErrors->setCursor(Qt::PointingHandCursor);
    this->_logScriptErrors->setChecked(Config::Instance()->logScriptErrors);

    this->_checkVersionAtStartup = new QCheckBox(QCoreApplication::translate("Settings", "Check for updates at startup.", "Checkbox label text"));
    this->_checkVersionAtStartup->setStyleSheet(Assets::Instance()->checkboxStyle);
    this->_checkVersionAtStartup->setCursor(Qt::PointingHandCursor);
    this->_checkVersionAtStartup->setChecked(Config::Instance()->checkVersionAtStartup);

    this->_downloadTimeoutLabel = new QLabel(QCoreApplication::translate("Settings", "Download timeout (in seconds):", "Label text"));
    this->_downloadTimeoutLabel->setStyleSheet("QLabel { " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");

    this->_downloadTimeoutSpinBox = new QSpinBox();
    this->_downloadTimeoutSpinBox->setMinimum(5);
    this->_downloadTimeoutSpinBox->setMaximum(10800);
    this->_downloadTimeoutSpinBox->setSingleStep(1);
    this->_downloadTimeoutSpinBox->setValue(Config::Instance()->downloadRequestTimeout);

    this->_downloadTimeoutLayout = new QHBoxLayout();
    this->_downloadTimeoutLayout->setContentsMargins(15, 0, 10, 0);
    this->_downloadTimeoutLayout->setAlignment(Qt::AlignVCenter);
    this->_downloadTimeoutLayout->addWidget(this->_downloadTimeoutLabel, Qt::AlignVCenter);
    this->_downloadTimeoutLayout->addSpacing(5);
    this->_downloadTimeoutLayout->addWidget(this->_downloadTimeoutSpinBox, Qt::AlignVCenter);

    this->_appearanceLabel = new QLabel(QCoreApplication::translate("Settings", "Appearance", "Label text"));
    this->_appearanceLabel->setStyleSheet("QLabel { margin-left: 5px; " + Assets::Instance()->SMALLER_TITLE_LABEL_STYLE + " }");

    this->_presetsLabel = new QLabel(QCoreApplication::translate("Settings", "Theme:", "Label text"));
    this->_presetsLabel->setAlignment(Qt::AlignVCenter);
    this->_presetsLabel->setStyleSheet("QLabel { margin-bottom: 2px; " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");

    this->_presetsComboBox = new QComboBox();
    QList<ThemeInfo> allThemes = Config::Instance()->getThemePresets();
    QStringList allPresets;
    int len = allThemes.count();
    if (len > 0)
        for (int i = 0; i < len; i++)
            if (allThemes[i].isValid && !allThemes[i].name.isEmpty())
                allPresets.push_back(allThemes[i].name);
    this->_presetsComboBox->addItems(allPresets);
    this->_presetsComboBox->setCurrentText("Default");
    this->_presetsComboBox->setCursor(Qt::PointingHandCursor);
    this->_presetsComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    connect(this->_presetsComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(presetChanged(QString)));

    this->_removePresetBtn = new QPushButton(QCoreApplication::translate("Settings", "Remove theme", "Button text"));
    this->_removePresetBtn->setStyleSheet(Assets::Instance()->secondaryBtnStyle);
    this->_removePresetBtn->setCursor(Qt::PointingHandCursor);
    connect(this->_removePresetBtn, SIGNAL(clicked()), this, SLOT(removeSelectedPreset()));

    this->_sharePresetBtn = new QPushButton(QCoreApplication::translate("Settings", "Share theme", "Button text"));
    this->_sharePresetBtn->setStyleSheet(Assets::Instance()->secondaryBtnStyle);
    this->_sharePresetBtn->setCursor(Qt::PointingHandCursor);
    connect(this->_sharePresetBtn, SIGNAL(clicked()), this, SLOT(shareSelectedPreset()));

    this->_presetsLayout = new QHBoxLayout();
    this->_presetsLayout->setContentsMargins(5, 0, 10, 0);
    this->_presetsLayout->setSpacing(0);
    this->_presetsLayout->addWidget(this->_presetsLabel);
    this->_presetsLayout->addSpacing(5);
    this->_presetsLayout->addWidget(this->_presetsComboBox);
    this->_presetsLayout->addSpacing(5);
    this->_presetsLayout->addWidget(this->_removePresetBtn);
    this->_presetsLayout->addSpacing(5);
    this->_presetsLayout->addWidget(this->_sharePresetBtn);

    this->_presetsLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    this->_fontFamily = new QComboBox();
    QStringList fontFamilies = QFontDatabase().families();
    this->_fontFamily->addItems(fontFamilies);
    this->_fontFamily->setCurrentText(Assets::Instance()->PRIMARY_FONT_FAMILY);
    this->_fontFamily->setCursor(Qt::PointingHandCursor);
    connect(this->_fontFamily, SIGNAL(currentTextChanged(QString)), this, SLOT(fontFamilyChanged(QString)));

    this->_smallFontSize = new QSpinBox();
    this->_smallFontSize->setMinimum(8);
    this->_smallFontSize->setMaximum(20);
    this->_smallFontSize->setSingleStep(2);
    this->_smallFontSize->setValue(Assets::Instance()->SM_FONT_SIZE_PX.toInt());
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    connect(this->_smallFontSize, SIGNAL(valueChanged(QString)), this, SLOT(smallFontSizeChanged(QString)));
#else
    connect(this->_smallFontSize, SIGNAL(textChanged(QString)), this, SLOT(smallFontSizeChanged(QString)));
#endif

    this->_regularFontSize = new QSpinBox();
    this->_regularFontSize->setMinimum(10);
    this->_regularFontSize->setMaximum(24);
    this->_regularFontSize->setSingleStep(2);
    this->_regularFontSize->setValue(Assets::Instance()->DEFAULT_FONT_SIZE_PX.toInt());
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    connect(this->_regularFontSize, SIGNAL(valueChanged(QString)), this, SLOT(regularFontSizeChanged(QString)));
#else
    connect(this->_regularFontSize, SIGNAL(textChanged(QString)), this, SLOT(regularFontSizeChanged(QString)));
#endif

    this->_largeFontSize = new QSpinBox();
    this->_largeFontSize->setMinimum(12);
    this->_largeFontSize->setMaximum(30);
    this->_largeFontSize->setSingleStep(2);
    this->_largeFontSize->setValue(Assets::Instance()->XL_FONT_SIZE_PX.toInt());
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    connect(this->_largeFontSize, SIGNAL(valueChanged(QString)), this, SLOT(largeFontSizeChanged(QString)));
#else
    connect(this->_largeFontSize, SIGNAL(textChanged(QString)), this, SLOT(largeFontSizeChanged(QString)));
#endif

    this->_extraLargeFontSize = new QSpinBox();
    this->_extraLargeFontSize->setMinimum(16);
    this->_extraLargeFontSize->setMaximum(36);
    this->_extraLargeFontSize->setSingleStep(2);
    this->_extraLargeFontSize->setValue(Assets::Instance()->XXL_FONT_SIZE_PX.toInt());
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    connect(this->_extraLargeFontSize, SIGNAL(valueChanged(QString)), this, SLOT(extraLargeFontSizeChanged(QString)));
#else
    connect(this->_extraLargeFontSize, SIGNAL(textChanged(QString)), this, SLOT(extraLargeFontSizeChanged(QString)));
#endif

    this->_primaryColor = new QColorDialog(Assets::Instance()->primaryColor);
    this->_primaryColorButton = new QPushButton(Assets::Instance()->PRIMARY_COLOR_HEX);
    this->_primaryColorButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->PRIMARY_COLOR_HEX));
    this->_primaryColorButton->setCursor(Qt::PointingHandCursor);
    connect(this->_primaryColorButton, SIGNAL(clicked()), this, SLOT(changePrimaryColor()));

    this->_primaryColorLight = new QColorDialog(QColor(Assets::Instance()->PRIMARY_COLOR_LIGHT_HEX));
    this->_primaryColorLightButton = new QPushButton(Assets::Instance()->PRIMARY_COLOR_LIGHT_HEX);
    this->_primaryColorLightButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->PRIMARY_COLOR_LIGHT_HEX));
    this->_primaryColorLightButton->setCursor(Qt::PointingHandCursor);
    connect(this->_primaryColorLightButton, SIGNAL(clicked()), this, SLOT(changePrimaryColorLight()));

    this->_primaryColorDeep = new QColorDialog(QColor(Assets::Instance()->PRIMARY_COLOR_DEEP_HEX));
    this->_primaryColorDeepButton = new QPushButton(Assets::Instance()->PRIMARY_COLOR_DEEP_HEX);
    this->_primaryColorDeepButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->PRIMARY_COLOR_DEEP_HEX));
    this->_primaryColorDeepButton->setCursor(Qt::PointingHandCursor);
    connect(this->_primaryColorDeepButton, SIGNAL(clicked()), this, SLOT(changePrimaryColorDeep()));

    this->_altPrimaryColor = new QColorDialog(QColor(Assets::Instance()->ALT_PRIMARY_COLOR_HEX));
    this->_altPrimaryColorButton = new QPushButton(Assets::Instance()->ALT_PRIMARY_COLOR_HEX);
    this->_altPrimaryColorButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->ALT_PRIMARY_COLOR_HEX));
    this->_altPrimaryColorButton->setCursor(Qt::PointingHandCursor);
    connect(this->_altPrimaryColorButton, SIGNAL(clicked()), this, SLOT(changeAltPrimaryColor()));

    this->_altPrimaryColorLight = new QColorDialog(QColor(Assets::Instance()->ALT_PRIMARY_COLOR_LIGHT_HEX));
    this->_altPrimaryColorLightButton = new QPushButton(Assets::Instance()->ALT_PRIMARY_COLOR_LIGHT_HEX);
    this->_altPrimaryColorLightButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->ALT_PRIMARY_COLOR_LIGHT_HEX));
    this->_altPrimaryColorLightButton->setCursor(Qt::PointingHandCursor);
    connect(this->_altPrimaryColorLightButton, SIGNAL(clicked()), this, SLOT(changeAltPrimaryColorLight()));

    this->_altPrimaryColorDeep = new QColorDialog(QColor(Assets::Instance()->ALT_PRIMARY_COLOR_DEEP_HEX));
    this->_altPrimaryColorDeepButton = new QPushButton(Assets::Instance()->ALT_PRIMARY_COLOR_DEEP_HEX);
    this->_altPrimaryColorDeepButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->ALT_PRIMARY_COLOR_DEEP_HEX));
    this->_altPrimaryColorDeepButton->setCursor(Qt::PointingHandCursor);
    connect(this->_altPrimaryColorDeepButton, SIGNAL(clicked()), this, SLOT(changeAltPrimaryColorDeep()));

    this->_secondaryColor = new QColorDialog(Assets::Instance()->secondaryColor);
    this->_secondaryColorButton = new QPushButton(Assets::Instance()->SECONDARY_COLOR_HEX);
    this->_secondaryColorButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->SECONDARY_COLOR_HEX));
    this->_secondaryColorButton->setCursor(Qt::PointingHandCursor);
    connect(this->_secondaryColorButton, SIGNAL(clicked()), this, SLOT(changeSecondaryColor()));

    this->_secondaryColorLight = new QColorDialog(Assets::Instance()->secondaryColorLight);
    this->_secondaryColorLightButton = new QPushButton(Assets::Instance()->SECONDARY_COLOR_LIGHT_HEX);
    this->_secondaryColorLightButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->SECONDARY_COLOR_LIGHT_HEX));
    this->_secondaryColorLightButton->setCursor(Qt::PointingHandCursor);
    connect(this->_secondaryColorLightButton, SIGNAL(clicked()), this, SLOT(changeSecondaryColorLight()));

    this->_secondaryColorLighter = new QColorDialog(QColor(Assets::Instance()->SECONDARY_COLOR_LIGHTER_HEX));
    this->_secondaryColorLighterButton = new QPushButton(Assets::Instance()->SECONDARY_COLOR_LIGHTER_HEX);
    this->_secondaryColorLighterButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->SECONDARY_COLOR_LIGHTER_HEX));
    this->_secondaryColorLighterButton->setCursor(Qt::PointingHandCursor);
    connect(this->_secondaryColorLighterButton, SIGNAL(clicked()), this, SLOT(changeSecondaryColorLighter()));

    this->_secondaryColorDeep = new QColorDialog(Assets::Instance()->secondaryColorDeep);
    this->_secondaryColorDeepButton = new QPushButton(Assets::Instance()->SECONDARY_COLOR_DEEP_HEX);
    this->_secondaryColorDeepButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->SECONDARY_COLOR_DEEP_HEX));
    this->_secondaryColorDeepButton->setCursor(Qt::PointingHandCursor);
    connect(this->_secondaryColorDeepButton, SIGNAL(clicked()), this, SLOT(changeSecondaryColorDeep()));

    this->_altSecondaryColor = new QColorDialog(QColor(Assets::Instance()->ALT_SECONDARY_COLOR_HEX));
    this->_altSecondaryColorButton = new QPushButton(Assets::Instance()->ALT_SECONDARY_COLOR_HEX);
    this->_altSecondaryColorButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->ALT_SECONDARY_COLOR_HEX));
    this->_altSecondaryColorButton->setCursor(Qt::PointingHandCursor);
    connect(this->_altSecondaryColorButton, SIGNAL(clicked()), this, SLOT(changeAltSecondaryColor()));

    this->_altSecondaryColorLight = new QColorDialog(QColor(Assets::Instance()->ALT_SECONDARY_COLOR_LIGHT_HEX));
    this->_altSecondaryColorLightButton = new QPushButton(Assets::Instance()->ALT_SECONDARY_COLOR_LIGHT_HEX);
    this->_altSecondaryColorLightButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->ALT_SECONDARY_COLOR_LIGHT_HEX));
    this->_altSecondaryColorLightButton->setCursor(Qt::PointingHandCursor);
    connect(this->_altSecondaryColorLightButton, SIGNAL(clicked()), this, SLOT(changeAltSecondaryColorLight()));

    this->_neutralColor = new QColorDialog(QColor(Assets::Instance()->NEUTRAL_COLOR_HEX));
    this->_neutralColorButton = new QPushButton(Assets::Instance()->NEUTRAL_COLOR_HEX);
    this->_neutralColorButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->NEUTRAL_COLOR_HEX));
    this->_neutralColorButton->setCursor(Qt::PointingHandCursor);
    connect(this->_neutralColorButton, SIGNAL(clicked()), this, SLOT(changeNeutralColor()));

    this->_neutralColorLight = new QColorDialog(QColor(Assets::Instance()->NEUTRAL_COLOR_LIGHT_HEX));
    this->_neutralColorLightButton = new QPushButton(Assets::Instance()->NEUTRAL_COLOR_LIGHT_HEX);
    this->_neutralColorLightButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->NEUTRAL_COLOR_LIGHT_HEX));
    this->_neutralColorLightButton->setCursor(Qt::PointingHandCursor);
    connect(this->_neutralColorLightButton, SIGNAL(clicked()), this, SLOT(changeNeutralColorLight()));

    this->_neutralColorLighter = new QColorDialog(QColor(Assets::Instance()->NEUTRAL_COLOR_LIGHTER_HEX));
    this->_neutralColorLighterButton = new QPushButton(Assets::Instance()->NEUTRAL_COLOR_LIGHTER_HEX);
    this->_neutralColorLighterButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->NEUTRAL_COLOR_LIGHTER_HEX));
    this->_neutralColorLighterButton->setCursor(Qt::PointingHandCursor);
    connect(this->_neutralColorLighterButton, SIGNAL(clicked()), this, SLOT(changeNeutralColorLighter()));

    this->_lightTextColor = new QColorDialog(Assets::Instance()->lightTextColor);
    this->_lightTextColorButton = new QPushButton(Assets::Instance()->LIGHT_TEXT_COLOR_HEX);
    this->_lightTextColorButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->LIGHT_TEXT_COLOR_HEX));
    this->_lightTextColorButton->setCursor(Qt::PointingHandCursor);
    connect(this->_lightTextColorButton, SIGNAL(clicked()), this, SLOT(changeLightTextColor()));

    this->_fontFamilyLabel = new QLabel(QCoreApplication::translate("Settings", "Font family:", "Label text"));
    this->_fontFamilyLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_smallFontSizeLabel = new QLabel(QCoreApplication::translate("Settings", "Small font size:", "Label text"));
    this->_smallFontSizeLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_regularFontSizeLabel = new QLabel(QCoreApplication::translate("Settings", "Regular font size:", "Label text"));
    this->_regularFontSizeLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_largeFontSizeLabel = new QLabel(QCoreApplication::translate("Settings", "Large font size:", "Label text"));
    this->_largeFontSizeLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_extraLargeFontSizeLabel = new QLabel(QCoreApplication::translate("Settings", "Extra large font size:", "Label text"));
    this->_extraLargeFontSizeLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_primaryColorLabel = new QLabel(QCoreApplication::translate("Settings", "Primary color:", "Label text"));
    this->_primaryColorLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_primaryColorLightLabel = new QLabel(QCoreApplication::translate("Settings", "Primary color light:", "Label text"));
    this->_primaryColorLightLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_primaryColorDeepLabel = new QLabel(QCoreApplication::translate("Settings", "Primary color deep:", "Label text"));
    this->_primaryColorDeepLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_altPrimaryColorLabel = new QLabel(QCoreApplication::translate("Settings", "Alternate primary color:", "Label text"));
    this->_altPrimaryColorLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_altPrimaryColorLightLabel = new QLabel(QCoreApplication::translate("Settings", "Alternate primary color light:", "Label text"));
    this->_altPrimaryColorLightLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_altPrimaryColorDeepLabel = new QLabel(QCoreApplication::translate("Settings", "Alternate primary color deep:", "Label text"));
    this->_altPrimaryColorDeepLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_secondaryColorLabel = new QLabel(QCoreApplication::translate("Settings", "Secondary color:", "Label text"));
    this->_secondaryColorLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_secondaryColorLightLabel = new QLabel(QCoreApplication::translate("Settings", "Secondary color light:", "Label text"));
    this->_secondaryColorLightLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_secondaryColorLighterLabel = new QLabel(QCoreApplication::translate("Settings", "Secondary color extra light:", "Label text"));
    this->_secondaryColorLighterLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_secondaryColorDeepLabel = new QLabel(QCoreApplication::translate("Settings", "Secondary color deep:", "Label text"));
    this->_secondaryColorDeepLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_altSecondaryColorLabel = new QLabel(QCoreApplication::translate("Settings", "Alternate secondary color:", "Label text"));
    this->_altSecondaryColorLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_altSecondaryColorLightLabel = new QLabel(QCoreApplication::translate("Settings", "Alternate secondary color light:", "Label text"));
    this->_altSecondaryColorLightLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_neutralColorLabel = new QLabel(QCoreApplication::translate("Settings", "Neutral color:", "Label text"));
    this->_neutralColorLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_neutralColorLightLabel = new QLabel(QCoreApplication::translate("Settings", "Neutral color light:", "Label text"));
    this->_neutralColorLightLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_neutralColorLighterLabel = new QLabel(QCoreApplication::translate("Settings", "Neutral color extra light:", "Label text"));
    this->_neutralColorLighterLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_lightTextColorLabel = new QLabel(QCoreApplication::translate("Settings", "Alternate text color:", "Label text"));
    this->_lightTextColorLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");

    this->_appearanceLayout = new QFormLayout();
    this->_appearanceLayout->setContentsMargins(5, 5, 5, 5);
    this->_appearanceLayout->setFieldGrowthPolicy(QFormLayout::FieldGrowthPolicy::AllNonFixedFieldsGrow);
    this->_appearanceLayout->setRowWrapPolicy(QFormLayout::RowWrapPolicy::DontWrapRows);
    this->_appearanceLayout->addRow(this->_fontFamilyLabel, this->_fontFamily);
    this->_appearanceLayout->addRow(this->_smallFontSizeLabel, this->_smallFontSize);
    this->_appearanceLayout->addRow(this->_regularFontSizeLabel, this->_regularFontSize);
    this->_appearanceLayout->addRow(this->_largeFontSizeLabel, this->_largeFontSize);
    this->_appearanceLayout->addRow(this->_extraLargeFontSizeLabel, this->_extraLargeFontSize);
    this->_appearanceLayout->addRow(this->_primaryColorLabel, this->_primaryColorButton);
    this->_appearanceLayout->addRow(this->_primaryColorLightLabel, this->_primaryColorLightButton);
    this->_appearanceLayout->addRow(this->_primaryColorDeepLabel, this->_primaryColorDeepButton);
    this->_appearanceLayout->addRow(this->_altPrimaryColorLabel, this->_altPrimaryColorButton);
    this->_appearanceLayout->addRow(this->_altPrimaryColorLightLabel, this->_altPrimaryColorLightButton);
    this->_appearanceLayout->addRow(this->_altPrimaryColorDeepLabel, this->_altPrimaryColorDeepButton);
    this->_appearanceLayout->addRow(this->_secondaryColorLabel, this->_secondaryColorButton);
    this->_appearanceLayout->addRow(this->_secondaryColorLightLabel, this->_secondaryColorLightButton);
    this->_appearanceLayout->addRow(this->_secondaryColorLighterLabel, this->_secondaryColorLighterButton);
    this->_appearanceLayout->addRow(this->_secondaryColorDeepLabel, this->_secondaryColorDeepButton);
    this->_appearanceLayout->addRow(this->_altSecondaryColorLabel, this->_altSecondaryColorButton);
    this->_appearanceLayout->addRow(this->_altSecondaryColorLightLabel, this->_altSecondaryColorLightButton);
    this->_appearanceLayout->addRow(this->_neutralColorLabel, this->_neutralColorButton);
    this->_appearanceLayout->addRow(this->_neutralColorLightLabel, this->_neutralColorLightButton);
    this->_appearanceLayout->addRow(this->_neutralColorLighterLabel, this->_neutralColorLighterButton);
    this->_appearanceLayout->addRow(this->_lightTextColorLabel, this->_lightTextColorButton);

    this->_appearanceGroupBox = new QGroupBox();
    this->_appearanceGroupBox->setStyleSheet("QGroupBox { border: none; background-color: transparent; }");
    this->_appearanceGroupBox->setLayout(this->_appearanceLayout);

    this->_saveButton = new QPushButton();
    this->_saveButton->setText(QCoreApplication::translate("Settings", "&Save", "Button text"));
    this->_saveButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->_saveButton->setCursor(Qt::PointingHandCursor);
    this->_saveButton->setContentsMargins(15, 0, 15, 0);
    connect(this->_saveButton, SIGNAL(clicked()), this, SLOT(save()));

    this->_cancelButton = new QPushButton();
    this->_cancelButton->setText(QCoreApplication::translate("Settings", "&Cancel", "Button text"));
    this->_cancelButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->_cancelButton->setCursor(Qt::PointingHandCursor);
    this->_cancelButton->setContentsMargins(15, 0, 15, 0);
    connect(this->_cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

    this->_buttonsLayout = new QHBoxLayout();
    this->_buttonsLayout->addWidget(this->_saveButton);
    this->_buttonsLayout->addSpacing(10);
    this->_buttonsLayout->addWidget(this->_cancelButton);
    this->_buttonsLayout->setContentsMargins(10, 5, 10, 10);
    this->_buttonsLayout->setAlignment(Qt::AlignBottom);

    this->_appearanceScrollArea = new QScrollArea();
    this->_appearanceScrollArea->setWidgetResizable(true);
    this->_appearanceScrollArea->setAlignment(Qt::AlignTop);
    this->_appearanceScrollArea->setStyleSheet("QScrollArea { margin-left: 10px; margin-right: 10px; " + Assets::Instance()->REGULAR_SCROLL_AREA_STYLE + " }");
    this->_appearanceScrollArea->setMaximumHeight(400);
    this->_appearanceScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->_appearanceScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->_appearanceScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    this->_layout.addWidget(this->_title);
    this->_layout.addSpacing(5);
    this->_layout.addWidget(this->_generalSettingsLabel);
    this->_layout.addSpacing(2);
    this->_layout.addLayout(this->_languageLayout);
    this->_layout.addSpacing(5);
#ifndef Q_OS_WIN
    this->_layout.addWidget(this->_logScriptErrors);
    this->_layout.addSpacing(5);
#endif
    this->_layout.addWidget(this->_checkVersionAtStartup);
    this->_layout.addSpacing(5);
    this->_layout.addLayout(this->_downloadTimeoutLayout);
    this->_layout.addSpacing(10);
    this->_layout.addWidget(this->_appearanceLabel);
    this->_layout.addSpacing(2);
    this->_layout.addLayout(this->_presetsLayout);
    this->_layout.addSpacing(5);
    this->_layout.addWidget(this->_appearanceScrollArea);
    this->_layout.addSpacing(5);
    this->_layout.addLayout(this->_buttonsLayout);
    this->_layout.setContentsMargins(0, 0, 0, 0);
    this->_layout.setSpacing(0);
    this->_layout.setAlignment(Qt::AlignTop);

    this->setStyleSheet("YumiSettings { " + Assets::Instance()->DEFAULT_WINDOW_STYLE + " }");
    this->setLayout(&this->_layout);
    this->_appearanceScrollArea->setWidget(this->_appearanceGroupBox);
    adjustSize();
    this->_appearanceScrollArea->setMinimumWidth(this->_appearanceLayout->sizeHint().width() + 50);
}

void YumiSettings::showEvent(QShowEvent*)
{
    this->_logScriptErrors->setChecked(Config::Instance()->logScriptErrors);
    this->_checkVersionAtStartup->setChecked(Config::Instance()->checkVersionAtStartup);
    this->_downloadTimeoutSpinBox->setValue(Config::Instance()->downloadRequestTimeout);
}

void YumiSettings::updateStyles()  
{
    Assets::Instance()->updateStyles();

    ((yumi*)_yumiPtr)->updateStyles();

    this->_primaryColorButton->setText(Assets::Instance()->PRIMARY_COLOR_HEX);
    this->_primaryColorButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->PRIMARY_COLOR_HEX));

    this->_primaryColorLightButton->setText(Assets::Instance()->PRIMARY_COLOR_LIGHT_HEX);
    this->_primaryColorLightButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->PRIMARY_COLOR_LIGHT_HEX));

    this->_primaryColorDeepButton->setText(Assets::Instance()->PRIMARY_COLOR_DEEP_HEX);
    this->_primaryColorDeepButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->PRIMARY_COLOR_DEEP_HEX));

    this->_altPrimaryColorButton->setText(Assets::Instance()->ALT_PRIMARY_COLOR_HEX);
    this->_altPrimaryColorButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->ALT_PRIMARY_COLOR_HEX));

    this->_altPrimaryColorLightButton->setText(Assets::Instance()->ALT_PRIMARY_COLOR_LIGHT_HEX);
    this->_altPrimaryColorLightButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->ALT_PRIMARY_COLOR_LIGHT_HEX));

    this->_altPrimaryColorDeepButton->setText(Assets::Instance()->ALT_PRIMARY_COLOR_DEEP_HEX);
    this->_altPrimaryColorDeepButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->ALT_PRIMARY_COLOR_DEEP_HEX));

    this->_secondaryColorButton->setText(Assets::Instance()->SECONDARY_COLOR_HEX);
    this->_secondaryColorButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->SECONDARY_COLOR_HEX));

    this->_secondaryColorLightButton->setText(Assets::Instance()->SECONDARY_COLOR_LIGHT_HEX);
    this->_secondaryColorLightButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->SECONDARY_COLOR_LIGHT_HEX));

    this->_secondaryColorLighterButton->setText(Assets::Instance()->SECONDARY_COLOR_LIGHTER_HEX);
    this->_secondaryColorLighterButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->SECONDARY_COLOR_LIGHTER_HEX));

    this->_secondaryColorDeepButton->setText(Assets::Instance()->SECONDARY_COLOR_DEEP_HEX);
    this->_secondaryColorDeepButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->SECONDARY_COLOR_DEEP_HEX));

    this->_altSecondaryColorButton->setText(Assets::Instance()->ALT_SECONDARY_COLOR_HEX);
    this->_altSecondaryColorButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->ALT_SECONDARY_COLOR_HEX));

    this->_altSecondaryColorLightButton->setText(Assets::Instance()->ALT_SECONDARY_COLOR_LIGHT_HEX);
    this->_altSecondaryColorLightButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->ALT_SECONDARY_COLOR_LIGHT_HEX));

    this->_neutralColorButton->setText(Assets::Instance()->NEUTRAL_COLOR_HEX);
    this->_neutralColorButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->NEUTRAL_COLOR_HEX));

    this->_neutralColorLightButton->setText(Assets::Instance()->NEUTRAL_COLOR_LIGHT_HEX);
    this->_neutralColorLightButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->NEUTRAL_COLOR_LIGHT_HEX));

    this->_neutralColorLighterButton->setText(Assets::Instance()->NEUTRAL_COLOR_LIGHTER_HEX);
    this->_neutralColorLighterButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->NEUTRAL_COLOR_LIGHTER_HEX));

    this->_lightTextColorButton->setText(Assets::Instance()->LIGHT_TEXT_COLOR_HEX);
    this->_lightTextColorButton->setStyleSheet(Assets::Instance()->getColorBtnStyle(Assets::Instance()->LIGHT_TEXT_COLOR_HEX));

    this->_title->updateStyles();
    this->_generalSettingsLabel->setStyleSheet("QLabel { margin-left: 5px; " + Assets::Instance()->SMALLER_TITLE_LABEL_STYLE + " }");
    this->_logScriptErrors->setStyleSheet(Assets::Instance()->checkboxStyle);
    this->_checkVersionAtStartup->setStyleSheet(Assets::Instance()->checkboxStyle);
    this->_downloadTimeoutLabel->setStyleSheet("QLabel { " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");
    this->_appearanceLabel->setStyleSheet("QLabel { margin-left: 5px; " + Assets::Instance()->SMALLER_TITLE_LABEL_STYLE + " }");
    this->_presetsLabel->setStyleSheet("QLabel { margin-bottom: 2px; " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");
    this->_removePresetBtn->setStyleSheet(Assets::Instance()->secondaryBtnStyle);
    this->_sharePresetBtn->setStyleSheet(Assets::Instance()->secondaryBtnStyle);
    this->_appearanceScrollArea->setStyleSheet("QScrollArea { margin-left: 10px; margin-right: 10px; " + Assets::Instance()->REGULAR_SCROLL_AREA_STYLE + " }");
    this->_fontFamilyLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_smallFontSizeLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_regularFontSizeLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_largeFontSizeLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_extraLargeFontSizeLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_primaryColorLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_primaryColorLightLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_primaryColorDeepLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_altPrimaryColorLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_altPrimaryColorLightLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_altPrimaryColorDeepLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_secondaryColorLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_secondaryColorLightLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_secondaryColorLighterLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_secondaryColorDeepLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_altSecondaryColorLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_altSecondaryColorLightLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_neutralColorLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_neutralColorLightLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_neutralColorLighterLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_lightTextColorLabel->setStyleSheet("QLabel { " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_saveButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->_cancelButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->setStyleSheet("YumiSettings { " + Assets::Instance()->DEFAULT_WINDOW_STYLE + " }");

    if (this->_saveThemePopup != NULL)
        this->_saveThemePopup->updateStyles();

    adjustSize();
    this->_appearanceScrollArea->setMinimumWidth(this->_appearanceLayout->sizeHint().width() + 50);
    update();
}

QPoint YumiSettings::getCenter()
{
    return QPoint(this->pos().x() + (this->width() / 2), this->pos().y() + (this->height() / 2));
}

void YumiSettings::doSave()
{
    if (((yumi*)_yumiPtr)->theme.compare(this->_presetsComboBox->currentText()) != 0)
    {
        ((yumi*)_yumiPtr)->theme = this->_presetsComboBox->currentText();
        Config::Instance()->saveConfig();
    }
}

void YumiSettings::save()
{
    bool saveConfig = false;
    bool logScriptErrors = this->_logScriptErrors->isChecked();
    if (logScriptErrors != Config::Instance()->logScriptErrors)
    {
        Config::Instance()->logScriptErrors = logScriptErrors;
        saveConfig = true;
    }
    bool checkVersionAtStartup = this->_checkVersionAtStartup->isChecked();
    if (checkVersionAtStartup != Config::Instance()->checkVersionAtStartup)
    {
        Config::Instance()->checkVersionAtStartup = checkVersionAtStartup;
        saveConfig = true;
    }
    int downloadTimeout = this->_downloadTimeoutSpinBox->value();
    if (downloadTimeout != Config::Instance()->downloadRequestTimeout && downloadTimeout >= 5 && downloadTimeout <= 10800)
    {
        Config::Instance()->downloadRequestTimeout = downloadTimeout;
        saveConfig = true;
    }
    QString lang = this->_languageComboBox->currentText();
    if (!lang.isEmpty() && (lang.compare("English", Qt::CaseInsensitive) == 0 || lang.compare("Français", Qt::CaseInsensitive) == 0 || lang.compare("Türk", Qt::CaseInsensitive) == 0) && Config::Instance()->language.compare(lang, Qt::CaseInsensitive) != 0)
    {
        Config::Instance()->language = lang;
        saveConfig = true;
        QMessageBox restartRequired(QMessageBox::Information, QCoreApplication::translate("Settings", "Please restart YUMI", "Popup title"), QCoreApplication::translate("Settings", "Please restart YUMI to load new language.", "Popup text"), QMessageBox::Ok, this);
        restartRequired.exec();
    }
    if (saveConfig)
        Config::Instance()->saveConfig();

    if (this->_themeChanged && this->hasThemeChanged())
    {
        if (_saveThemePopup == NULL)
            _saveThemePopup = new SaveThemeWindow(_yumiPtr, NULL);
        if (_saveThemePopup->isVisible())
            _saveThemePopup->close();
        _saveThemePopup->doShowAt(this->getCenter());
    }
    else
    {
        this->doSave();
        close();
    }
}

void YumiSettings::saveConfirmed(const QString& themeName)
{
    ThemeInfo theme;
    theme.name = themeName;
    theme.isValid = true;
    theme.primaryFontFamily = _fontFamily->currentText();
    theme.smallFontSizePx = _smallFontSize->text();
    theme.regularFontSizePx = _regularFontSize->text();
    theme.largeFontSizePx = _largeFontSize->text();
    theme.extraLargeFontSizePx = _extraLargeFontSize->text();
    theme.primaryColorHex = _primaryColorButton->text().toUpper();
    theme.primaryColorLightHex = _primaryColorLightButton->text().toUpper();
    theme.primaryColorDeepHex = _primaryColorDeepButton->text().toUpper();
    theme.altPrimaryColorHex = _altPrimaryColorButton->text().toUpper();
    theme.altPrimaryColorLightHex = _altPrimaryColorLightButton->text().toUpper();
    theme.altPrimaryColorDeepHex = _altPrimaryColorDeepButton->text().toUpper();
    theme.secondaryColorHex = _secondaryColorButton->text().toUpper();
    theme.secondaryColorLightHex = _secondaryColorLightButton->text().toUpper();
    theme.secondaryColorLighterHex = _secondaryColorLighterButton->text().toUpper();
    theme.secondaryColorDeepHex = _secondaryColorDeepButton->text().toUpper();
    theme.altSecondaryColorHex = _altSecondaryColorButton->text().toUpper();
    theme.altSecondaryColorLightHex = _altSecondaryColorLightButton->text().toUpper();
    theme.neutralColorHex = _neutralColorButton->text().toUpper();
    theme.neutralColorLightHex = _neutralColorLightButton->text().toUpper();
    theme.neutralColorLighterHex = _neutralColorLighterButton->text().toUpper();
    theme.alternateTextColorHex = _lightTextColorButton->text().toUpper();
    Config::Instance()->addTheme(theme);

    this->_presetsComboBox->addItem(themeName);
    this->_presetsComboBox->setCurrentText(themeName);
    this->doSave();
    this->updateStyles();
    this->close();
}

void YumiSettings::restoreTheme()
{
    QString selectedTheme = this->_presetsComboBox->currentText();
    ThemeInfo currentTheme = Config::Instance()->getThemePreset(selectedTheme);
    if (currentTheme.isValid)
    {
        Assets::Instance()->PRIMARY_FONT_FAMILY = currentTheme.primaryFontFamily;
        Assets::Instance()->SM_FONT_SIZE_PX = currentTheme.smallFontSizePx;
        Assets::Instance()->DEFAULT_FONT_SIZE_PX = currentTheme.regularFontSizePx;
        Assets::Instance()->XL_FONT_SIZE_PX = currentTheme.largeFontSizePx;
        Assets::Instance()->XXL_FONT_SIZE_PX = currentTheme.extraLargeFontSizePx;
        Assets::Instance()->PRIMARY_COLOR_HEX = currentTheme.primaryColorHex;
        Assets::Instance()->PRIMARY_COLOR_LIGHT_HEX = currentTheme.primaryColorLightHex;
        Assets::Instance()->PRIMARY_COLOR_DEEP_HEX = currentTheme.primaryColorDeepHex;
        Assets::Instance()->ALT_PRIMARY_COLOR_HEX = currentTheme.altPrimaryColorHex;
        Assets::Instance()->ALT_PRIMARY_COLOR_LIGHT_HEX = currentTheme.altPrimaryColorLightHex;
        Assets::Instance()->ALT_PRIMARY_COLOR_DEEP_HEX = currentTheme.altPrimaryColorDeepHex;
        Assets::Instance()->SECONDARY_COLOR_HEX = currentTheme.secondaryColorHex;
        Assets::Instance()->SECONDARY_COLOR_LIGHT_HEX = currentTheme.secondaryColorLightHex;
        Assets::Instance()->SECONDARY_COLOR_LIGHTER_HEX = currentTheme.secondaryColorLighterHex;
        Assets::Instance()->SECONDARY_COLOR_DEEP_HEX = currentTheme.secondaryColorDeepHex;
        Assets::Instance()->ALT_SECONDARY_COLOR_HEX = currentTheme.altSecondaryColorHex;
        Assets::Instance()->ALT_SECONDARY_COLOR_LIGHT_HEX = currentTheme.altSecondaryColorLightHex;
        Assets::Instance()->NEUTRAL_COLOR_HEX = currentTheme.neutralColorHex;
        Assets::Instance()->NEUTRAL_COLOR_LIGHT_HEX = currentTheme.neutralColorLightHex;
        Assets::Instance()->NEUTRAL_COLOR_LIGHTER_HEX = currentTheme.neutralColorLighterHex;
        Assets::Instance()->LIGHT_TEXT_COLOR_HEX = currentTheme.alternateTextColorHex;
    }
    this->_fontFamily->setCurrentText(Assets::Instance()->PRIMARY_FONT_FAMILY);
    this->_smallFontSize->setValue(Assets::Instance()->SM_FONT_SIZE_PX.toInt());
    this->_regularFontSize->setValue(Assets::Instance()->DEFAULT_FONT_SIZE_PX.toInt());
    this->_largeFontSize->setValue(Assets::Instance()->XL_FONT_SIZE_PX.toInt());
    this->_extraLargeFontSize->setValue(Assets::Instance()->XXL_FONT_SIZE_PX.toInt());
    this->_primaryColor->setCurrentColor(QColor(Assets::Instance()->PRIMARY_COLOR_HEX));
    this->_primaryColorButton->setText(Assets::Instance()->PRIMARY_COLOR_HEX);
    this->_primaryColorLight->setCurrentColor(QColor(Assets::Instance()->PRIMARY_COLOR_LIGHT_HEX));
    this->_primaryColorLightButton->setText(Assets::Instance()->PRIMARY_COLOR_LIGHT_HEX);
    this->_primaryColorDeep->setCurrentColor(QColor(Assets::Instance()->PRIMARY_COLOR_DEEP_HEX));
    this->_primaryColorDeepButton->setText(Assets::Instance()->PRIMARY_COLOR_DEEP_HEX);
    this->_altPrimaryColor->setCurrentColor(QColor(Assets::Instance()->ALT_PRIMARY_COLOR_HEX));
    this->_altPrimaryColorButton->setText(Assets::Instance()->ALT_PRIMARY_COLOR_HEX);
    this->_altPrimaryColorLight->setCurrentColor(QColor(Assets::Instance()->ALT_PRIMARY_COLOR_LIGHT_HEX));
    this->_altPrimaryColorLightButton->setText(Assets::Instance()->ALT_PRIMARY_COLOR_LIGHT_HEX);
    this->_altPrimaryColorDeep->setCurrentColor(QColor(Assets::Instance()->ALT_PRIMARY_COLOR_DEEP_HEX));
    this->_altPrimaryColorDeepButton->setText(Assets::Instance()->ALT_PRIMARY_COLOR_DEEP_HEX);
    this->_secondaryColor->setCurrentColor(QColor(Assets::Instance()->SECONDARY_COLOR_HEX));
    this->_secondaryColorButton->setText(Assets::Instance()->SECONDARY_COLOR_HEX);
    this->_secondaryColorLight->setCurrentColor(QColor(Assets::Instance()->SECONDARY_COLOR_LIGHT_HEX));
    this->_secondaryColorLightButton->setText(Assets::Instance()->SECONDARY_COLOR_LIGHT_HEX);
    this->_secondaryColorLighter->setCurrentColor(QColor(Assets::Instance()->SECONDARY_COLOR_LIGHTER_HEX));
    this->_secondaryColorLighterButton->setText(Assets::Instance()->SECONDARY_COLOR_LIGHTER_HEX);
    this->_secondaryColorDeep->setCurrentColor(QColor(Assets::Instance()->SECONDARY_COLOR_DEEP_HEX));
    this->_secondaryColorDeepButton->setText(Assets::Instance()->SECONDARY_COLOR_DEEP_HEX);
    this->_altSecondaryColor->setCurrentColor(QColor(Assets::Instance()->ALT_SECONDARY_COLOR_HEX));
    this->_altSecondaryColorButton->setText(Assets::Instance()->ALT_SECONDARY_COLOR_HEX);
    this->_altSecondaryColorLight->setCurrentColor(QColor(Assets::Instance()->ALT_SECONDARY_COLOR_LIGHT_HEX));
    this->_altSecondaryColorLightButton->setText(Assets::Instance()->ALT_SECONDARY_COLOR_LIGHT_HEX);
    this->_neutralColor->setCurrentColor(QColor(Assets::Instance()->NEUTRAL_COLOR_HEX));
    this->_neutralColorButton->setText(Assets::Instance()->NEUTRAL_COLOR_HEX);
    this->_neutralColorLight->setCurrentColor(QColor(Assets::Instance()->NEUTRAL_COLOR_LIGHT_HEX));
    this->_neutralColorLightButton->setText(Assets::Instance()->NEUTRAL_COLOR_LIGHT_HEX);
    this->_neutralColorLighter->setCurrentColor(QColor(Assets::Instance()->NEUTRAL_COLOR_LIGHTER_HEX));
    this->_neutralColorLighterButton->setText(Assets::Instance()->NEUTRAL_COLOR_LIGHTER_HEX);
    this->_lightTextColor->setCurrentColor(QColor(Assets::Instance()->LIGHT_TEXT_COLOR_HEX));
    this->_lightTextColorButton->setText(Assets::Instance()->LIGHT_TEXT_COLOR_HEX);
    this->_themeChanged = false;
}

void YumiSettings::cancel()
{
    if (this->_languageComboBox->currentText().compare(Config::Instance()->language, Qt::CaseInsensitive) != 0)
    {
        this->_languageComboBox->setCurrentText(Config::Instance()->language);
        Config::SwapLanguage(Config::Instance()->language, ((yumi*)_yumiPtr)->appPtr);
    }
    this->_presetsComboBox->setCurrentText(((yumi*)_yumiPtr)->theme);
    this->restoreTheme();
    this->updateStyles();
    this->close();
}

bool YumiSettings::hasThemeChanged()
{
    QString selectedTheme = this->_presetsComboBox->currentText();
    ThemeInfo currentTheme = Config::Instance()->getThemePreset(selectedTheme);
    if (!currentTheme.isValid)
        return false;
    if (currentTheme.smallFontSizePx.compare(this->_smallFontSize->text()) != 0)
        return true;
    if (currentTheme.regularFontSizePx.compare(this->_regularFontSize->text()) != 0)
        return true;
    if (currentTheme.largeFontSizePx.compare(this->_largeFontSize->text()) != 0)
        return true;
    if (currentTheme.extraLargeFontSizePx.compare(this->_extraLargeFontSize->text()) != 0)
        return true;
    if (currentTheme.primaryColorHex.compare(this->_primaryColorButton->text().toUpper()) != 0)
        return true;
    if (currentTheme.primaryColorLightHex.compare(this->_primaryColorLightButton->text().toUpper()) != 0)
        return true;
    if (currentTheme.primaryColorDeepHex.compare(this->_primaryColorDeepButton->text().toUpper()) != 0)
        return true;
    if (currentTheme.altPrimaryColorHex.compare(this->_altPrimaryColorButton->text().toUpper()) != 0)
        return true;
    if (currentTheme.altPrimaryColorLightHex.compare(this->_altPrimaryColorLightButton->text().toUpper()) != 0)
        return true;
    if (currentTheme.altPrimaryColorDeepHex.compare(this->_altPrimaryColorDeepButton->text().toUpper()) != 0)
        return true;
    if (currentTheme.secondaryColorHex.compare(this->_secondaryColorButton->text().toUpper()) != 0)
        return true;
    if (currentTheme.secondaryColorLightHex.compare(this->_secondaryColorLightButton->text().toUpper()) != 0)
        return true;
    if (currentTheme.secondaryColorLighterHex.compare(this->_secondaryColorLighterButton->text().toUpper()) != 0)
        return true;
    if (currentTheme.secondaryColorDeepHex.compare(this->_secondaryColorDeepButton->text().toUpper()) != 0)
        return true;
    if (currentTheme.altSecondaryColorHex.compare(this->_altSecondaryColorButton->text().toUpper()) != 0)
        return true;
    if (currentTheme.altSecondaryColorLightHex.compare(this->_altSecondaryColorLightButton->text().toUpper()) != 0)
        return true;
    if (currentTheme.neutralColorHex.compare(this->_neutralColorButton->text().toUpper()) != 0)
        return true;
    if (currentTheme.neutralColorLightHex.compare(this->_neutralColorLightButton->text().toUpper()) != 0)
        return true;
    if (currentTheme.neutralColorLighterHex.compare(this->_neutralColorLighterButton->text().toUpper()) != 0)
        return true;
    if (currentTheme.alternateTextColorHex.compare(this->_lightTextColorButton->text().toUpper()) != 0)
        return true;
    if (currentTheme.primaryFontFamily.compare(this->_fontFamily->currentText()) != 0)
        return true;
    return false;
}

void YumiSettings::updateTheme(const QString& themeName)
{
    if (themeName.isEmpty())
        return;
    this->_presetsComboBox->setCurrentText(themeName);
    this->restoreTheme();
    this->updateStyles();
}

void YumiSettings::removeSelectedPreset()
{
    int selectedPresetIndex = this->_presetsComboBox->currentIndex();
    QString selectedPreset = this->_presetsComboBox->currentText();
    if (selectedPresetIndex < 0 || selectedPreset.isEmpty() || selectedPreset.compare("Default") == 0 || selectedPreset.compare("Blue") == 0 || selectedPreset.compare("Green") == 0 || selectedPreset.compare("Purple") == 0)
    {
        QMessageBox incorrectPreset = QMessageBox(QMessageBox::Warning, QCoreApplication::translate("Settings", "Cannot remove theme", "Popup title"), QCoreApplication::translate("Settings", "You cannot remove builtin themes.", "Popup text"), QMessageBox::Ok, this);
        incorrectPreset.exec();
        return;
    }
    this->_presetsComboBox->removeItem(selectedPresetIndex);
    Config::Instance()->removeTheme(selectedPreset);
    ((yumi*)_yumiPtr)->theme = "Default";
    updateTheme("Default");
    Config::Instance()->saveThemePresets();
    Config::Instance()->saveConfig();
}

void YumiSettings::shareSelectedPreset()
{
    int selectedPresetIndex = this->_presetsComboBox->currentIndex();
    QString selectedPreset = this->_presetsComboBox->currentText();
    if (selectedPresetIndex < 0 || selectedPreset.isEmpty() || selectedPreset.compare("Default") == 0 || selectedPreset.compare("Blue") == 0 || selectedPreset.compare("Green") == 0 || selectedPreset.compare("Purple") == 0)
    {
        QMessageBox incorrectPreset = QMessageBox(QMessageBox::Warning, QCoreApplication::translate("Settings", "Cannot share theme", "Popup title"), QCoreApplication::translate("Settings", "You cannot share builtin themes.", "Popup text"), QMessageBox::Ok, this);
        incorrectPreset.exec();
        return;
    }
    ThemeInfo ti = Config::Instance()->getThemePreset(selectedPreset);
    if (ti.isValid)
    {
        try
        {
            QString jsonStr = Utils::stripSpecialCharacters(Config::Instance()->serializeThemePreset(ti));
            ((yumi*)_yumiPtr)->network->isShareThemeRequest = true;
            ((yumi*)_yumiPtr)->network->sendJsonPostRequest(YUMI_SHARE_THEME_URL, "{\"theme\":\"" + jsonStr + "\"}");
        }
        catch (const std::exception& ex) { qWarning().nospace() << "An exception got caught while sharing theme preset (Exception: " << ex.what() << ")."; }
        catch (...) { qWarning().nospace() << "An exception got caught while sharing theme preset."; }
    }
}

void YumiSettings::showSharingResult(bool success, const QString& msg)
{
    if (success)
    {
        QMessageBox presetShared = QMessageBox(QMessageBox::Information, QCoreApplication::translate("Settings", "Theme shared", "Popup title"), QCoreApplication::translate("Settings", "Your custom theme has been shared. It will be available in next YUMI release if it passes validation. Thank you.", "Popup text"), QMessageBox::Ok, this);
        presetShared.exec();
    }
    else
    {
        QMessageBox sharingError = QMessageBox(QMessageBox::Warning, QCoreApplication::translate("Settings", "Sharing theme failed", "Popup title"), QCoreApplication::translate("Settings", "An error happened when sharing your custom theme over network, please try again. You can also get help on the Discord server.\n\nError message:\n%1", "Popup text").arg(msg), QMessageBox::Ok, this);
        sharingError.exec();
    }
}

void YumiSettings::languageChanged(QString lang)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Language changed to: " << lang;
#endif
    if (lang.isEmpty())
        return;
    if (!Config::SwapLanguage(lang, ((yumi*)_yumiPtr)->appPtr))
        return;
    this->updateStyles();
}

void YumiSettings::presetChanged(QString preset)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Preset changed to: " << preset;
#endif
    if (preset.isEmpty())
        return;
    this->restoreTheme();
    this->updateStyles();
}

void YumiSettings::fontFamilyChanged(QString fontFamily)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Font family changed to: " << fontFamily;
#endif
    this->_themeChanged = true;
    Assets::Instance()->PRIMARY_FONT_FAMILY = fontFamily;
    this->updateStyles();
}

void YumiSettings::smallFontSizeChanged(QString fontSize)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Small font size changed to: " << fontSize;
#endif
    this->_themeChanged = true;
    Assets::Instance()->SM_FONT_SIZE_PX = fontSize;
    this->updateStyles();
}

void YumiSettings::regularFontSizeChanged(QString fontSize)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Regular font size changed to: " << fontSize;
#endif
    this->_themeChanged = true;
    Assets::Instance()->DEFAULT_FONT_SIZE_PX = fontSize;
    this->updateStyles();
}

void YumiSettings::largeFontSizeChanged(QString fontSize)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Large font size changed to: " << fontSize;
#endif
    this->_themeChanged = true;
    Assets::Instance()->XL_FONT_SIZE_PX = fontSize;
    this->updateStyles();
}

void YumiSettings::extraLargeFontSizeChanged(QString fontSize)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Extra large font size changed to: " << fontSize;
#endif
    this->_themeChanged = true;
    Assets::Instance()->XXL_FONT_SIZE_PX = fontSize;
    this->updateStyles();
}

void YumiSettings::changePrimaryColor()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug() << "changePrimaryColor clicked!";
#endif
    this->_primaryColor->open(this, SLOT(changedPrimaryColor(const QColor&)));
}

void YumiSettings::changePrimaryColorLight()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug() << "changePrimaryColorLight clicked!";
#endif
    this->_primaryColorLight->open(this, SLOT(changedPrimaryColorLight(const QColor&)));
}

void YumiSettings::changePrimaryColorDeep()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug() << "changePrimaryColorDeep clicked!";
#endif
    this->_primaryColorDeep->open(this, SLOT(changedPrimaryColorDeep(const QColor&)));
}

void YumiSettings::changeAltPrimaryColor()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug() << "changeAltPrimaryColor clicked!";
#endif
    this->_altPrimaryColor->open(this, SLOT(changedAltPrimaryColor(const QColor&)));
}

void YumiSettings::changeAltPrimaryColorLight()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug() << "changeAltPrimaryColorLight clicked!";
#endif
    this->_altPrimaryColorLight->open(this, SLOT(changedAltPrimaryColorLight(const QColor&)));
}

void YumiSettings::changeAltPrimaryColorDeep()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug() << "changeAltPrimaryColorDeep clicked!";
#endif
    this->_altPrimaryColorDeep->open(this, SLOT(changedAltPrimaryColorDeep(const QColor&)));
}

void YumiSettings::changeSecondaryColor()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug() << "changeSecondaryColor clicked!";
#endif
    this->_secondaryColor->open(this, SLOT(changedSecondaryColor(const QColor&)));
}

void YumiSettings::changeSecondaryColorLight()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug() << "changeSecondaryColorLight clicked!";
#endif
    this->_secondaryColorLight->open(this, SLOT(changedSecondaryColorLight(const QColor&)));
}

void YumiSettings::changeSecondaryColorLighter()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug() << "changeSecondaryColorLighter clicked!";
#endif
    this->_secondaryColorLighter->open(this, SLOT(changedSecondaryColorLighter(const QColor&)));
}

void YumiSettings::changeSecondaryColorDeep()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug() << "changeSecondaryColorDeep clicked!";
#endif
    this->_secondaryColorDeep->open(this, SLOT(changedSecondaryColorDeep(const QColor&)));
}

void YumiSettings::changeAltSecondaryColor()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug() << "changeAltSecondaryColor clicked!";
#endif
    this->_altSecondaryColor->open(this, SLOT(changedAltSecondaryColor(const QColor&)));
}

void YumiSettings::changeAltSecondaryColorLight()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug() << "changeAltSecondaryColorLight clicked!";
#endif
    this->_altSecondaryColorLight->open(this, SLOT(changedAltSecondaryColorLight(const QColor&)));
}

void YumiSettings::changeNeutralColor()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug() << "changeNeutralColor clicked!";
#endif
    this->_neutralColor->open(this, SLOT(changedNeutralColor(const QColor&)));
}

void YumiSettings::changeNeutralColorLight()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug() << "changeNeutralColorLight clicked!";
#endif
    this->_neutralColorLight->open(this, SLOT(changedNeutralColorLight(const QColor&)));
}

void YumiSettings::changeNeutralColorLighter()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug() << "changeNeutralColorLighter clicked!";
#endif
    this->_neutralColorLighter->open(this, SLOT(changedNeutralColorLighter(const QColor&)));
}

void YumiSettings::changeLightTextColor()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug() << "changeLightTextColor clicked!";
#endif
    this->_lightTextColor->open(this, SLOT(changedLightTextColor(const QColor&)));
}

void YumiSettings::changedPrimaryColor(const QColor& color)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Color changed to: " << color.red() << ";" << color.green() << ";" << color.blue();
#endif
    this->_themeChanged = true;
    Assets::Instance()->PRIMARY_COLOR_HEX = color.name().toUpper();
    this->updateStyles();
}

void YumiSettings::changedPrimaryColorLight(const QColor& color)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Color changed to: " << color.red() << ";" << color.green() << ";" << color.blue();
#endif
    this->_themeChanged = true;
    Assets::Instance()->PRIMARY_COLOR_LIGHT_HEX = color.name().toUpper();
    this->updateStyles();
}

void YumiSettings::changedPrimaryColorDeep(const QColor& color)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Color changed to: " << color.red() << ";" << color.green() << ";" << color.blue();
#endif
    this->_themeChanged = true;
    Assets::Instance()->PRIMARY_COLOR_DEEP_HEX = color.name().toUpper();
    this->updateStyles();
}

void YumiSettings::changedAltPrimaryColor(const QColor& color)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Color changed to: " << color.red() << ";" << color.green() << ";" << color.blue();
#endif
    this->_themeChanged = true;
    Assets::Instance()->ALT_PRIMARY_COLOR_HEX = color.name().toUpper();
    this->updateStyles();
}

void YumiSettings::changedAltPrimaryColorLight(const QColor& color)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Color changed to: " << color.red() << ";" << color.green() << ";" << color.blue();
#endif
    this->_themeChanged = true;
    Assets::Instance()->ALT_PRIMARY_COLOR_LIGHT_HEX = color.name().toUpper();
    this->updateStyles();
}

void YumiSettings::changedAltPrimaryColorDeep(const QColor& color)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Color changed to: " << color.red() << ";" << color.green() << ";" << color.blue();
#endif
    this->_themeChanged = true;
    Assets::Instance()->ALT_PRIMARY_COLOR_DEEP_HEX = color.name().toUpper();
    this->updateStyles();
}

void YumiSettings::changedSecondaryColor(const QColor& color)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Color changed to: " << color.red() << ";" << color.green() << ";" << color.blue();
#endif
    this->_themeChanged = true;
    Assets::Instance()->SECONDARY_COLOR_HEX = color.name().toUpper();
    this->updateStyles();
}

void YumiSettings::changedSecondaryColorLight(const QColor& color)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Color changed to: " << color.red() << ";" << color.green() << ";" << color.blue();
#endif
    this->_themeChanged = true;
    Assets::Instance()->SECONDARY_COLOR_LIGHT_HEX = color.name().toUpper();
    this->updateStyles();
}

void YumiSettings::changedSecondaryColorLighter(const QColor& color)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Color changed to: " << color.red() << ";" << color.green() << ";" << color.blue();
#endif
    this->_themeChanged = true;
    Assets::Instance()->SECONDARY_COLOR_LIGHTER_HEX = color.name().toUpper();
    this->updateStyles();
}

void YumiSettings::changedSecondaryColorDeep(const QColor& color)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Color changed to: " << color.red() << ";" << color.green() << ";" << color.blue();
#endif
    Assets::Instance()->SECONDARY_COLOR_DEEP_HEX = color.name().toUpper();
    this->updateStyles();
}

void YumiSettings::changedAltSecondaryColor(const QColor& color)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Color changed to: " << color.red() << ";" << color.green() << ";" << color.blue();
#endif
    this->_themeChanged = true;
    Assets::Instance()->ALT_SECONDARY_COLOR_HEX = color.name().toUpper();
    this->updateStyles();
}

void YumiSettings::changedAltSecondaryColorLight(const QColor& color)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Color changed to: " << color.red() << ";" << color.green() << ";" << color.blue();
#endif
    Assets::Instance()->ALT_SECONDARY_COLOR_LIGHT_HEX = color.name().toUpper();
    this->updateStyles();
}

void YumiSettings::changedNeutralColor(const QColor& color)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Color changed to: " << color.red() << ";" << color.green() << ";" << color.blue();
#endif
    this->_themeChanged = true;
    Assets::Instance()->NEUTRAL_COLOR_HEX = color.name().toUpper();
    this->updateStyles();
}

void YumiSettings::changedNeutralColorLight(const QColor& color)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Color changed to: " << color.red() << ";" << color.green() << ";" << color.blue();
#endif
    this->_themeChanged = true;
    Assets::Instance()->NEUTRAL_COLOR_LIGHT_HEX = color.name().toUpper();
    this->updateStyles();
}

void YumiSettings::changedNeutralColorLighter(const QColor& color)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Color changed to: " << color.red() << ";" << color.green() << ";" << color.blue();
#endif
    this->_themeChanged = true;
    Assets::Instance()->NEUTRAL_COLOR_LIGHTER_HEX = color.name().toUpper();
    this->updateStyles();
}

void YumiSettings::changedLightTextColor(const QColor& color)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Color changed to: " << color.red() << ";" << color.green() << ";" << color.blue();
#endif
    this->_themeChanged = true;
    Assets::Instance()->LIGHT_TEXT_COLOR_HEX = color.name().toUpper();
    this->updateStyles();
}

void YumiSettings::doShowAt(const QPoint& center)
{
    this->show();
    this->adjustSize();
    this->move(center.x() - (this->width() / 2), center.y() - (this->height() / 2));
    QApplication::restoreOverrideCursor();
}

void YumiSettings::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        _mousePos = event->globalPos() - pos();
        _windowMoving = true;
    }
    QWidget::mousePressEvent(event);
}

void YumiSettings::mouseMoveEvent(QMouseEvent* event)
{
    if (_windowMoving && (event->buttons() & Qt::LeftButton))
    {
        event->accept();
        move(event->globalPos() - _mousePos);
    }
    else
        QWidget::mouseMoveEvent(event);
}

void YumiSettings::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        _windowMoving = false;
    QWidget::mouseReleaseEvent(event);
}

void YumiSettings::paintEvent(QPaintEvent*)
{
#if IS_DEBUG && DEBUG_PAINTING
    qDebug() << "SettingsWindow paint event!";
#endif

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
