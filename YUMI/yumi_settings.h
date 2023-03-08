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

#ifndef __YUMI_SETTINGS_H__
#define __YUMI_SETTINGS_H__

#include <QObject>
#include <QWidget>
#include <QPoint>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QColorDialog>
#include <QGroupBox>
#include <QScrollArea>
#include <QStringList>
#include <QCheckBox>
#include "window_title.h"
#include "save_theme_window.h"

class YumiSettings : public QWidget
{
    Q_OBJECT

public:
    explicit YumiSettings(void* yumiPtr, QWidget* parent = NULL);
    void doShowAt(const QPoint& center);
    void doSave();
    void saveConfirmed(const QString& themeName);
    void restoreTheme();
    QPoint getCenter();
    void updateTheme(const QString& themeName);
    void showSharingResult(bool success, const QString& msg);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    YumiSettings() = delete;
    YumiSettings(QWidget* parent) = delete;
    void updateStyles();
    void saveCurrentTheme();
    bool hasThemeChanged();

    void* _yumiPtr;
    bool _themeChanged;
    WindowTitle* _title;
    SaveThemeWindow* _saveThemePopup;

    QLabel* _generalSettingsLabel;
    QLabel* _languageLabel;
    QComboBox* _languageComboBox;
    QHBoxLayout* _languageLayout;
    QCheckBox* _logScriptErrors;
    QCheckBox* _checkVersionAtStartup;
    QLabel* _downloadTimeoutLabel;
    QSpinBox* _downloadTimeoutSpinBox;
    QHBoxLayout* _downloadTimeoutLayout;

    QLabel* _appearanceLabel;
    QHBoxLayout* _presetsLayout;
    QLabel* _presetsLabel;
    QComboBox* _presetsComboBox;
    QPushButton* _removePresetBtn;
    QPushButton* _sharePresetBtn;

    QComboBox* _fontFamily;
    QLabel* _fontFamilyLabel;
    QSpinBox* _smallFontSize;
    QLabel* _smallFontSizeLabel;
    QSpinBox* _regularFontSize;
    QLabel* _regularFontSizeLabel;
    QSpinBox* _largeFontSize; 
    QLabel* _largeFontSizeLabel;
    QSpinBox* _extraLargeFontSize; 
    QLabel* _extraLargeFontSizeLabel;

    QColorDialog* _primaryColor;
    QLabel* _primaryColorLabel;
    QPushButton* _primaryColorButton;
    QColorDialog* _primaryColorLight;
    QLabel* _primaryColorLightLabel;
    QPushButton* _primaryColorLightButton;
    QColorDialog* _primaryColorDeep;
    QLabel* _primaryColorDeepLabel;
    QPushButton* _primaryColorDeepButton;
    QColorDialog* _altPrimaryColor;
    QLabel* _altPrimaryColorLabel;
    QPushButton* _altPrimaryColorButton;
    QColorDialog* _altPrimaryColorLight;
    QLabel* _altPrimaryColorLightLabel;
    QPushButton* _altPrimaryColorLightButton;
    QColorDialog* _altPrimaryColorDeep;
    QLabel* _altPrimaryColorDeepLabel;
    QPushButton* _altPrimaryColorDeepButton;
    QColorDialog* _secondaryColor;
    QLabel* _secondaryColorLabel;
    QPushButton* _secondaryColorButton;
    QColorDialog* _secondaryColorLight;
    QLabel* _secondaryColorLightLabel;
    QPushButton* _secondaryColorLightButton;
    QColorDialog* _secondaryColorLighter;
    QLabel* _secondaryColorLighterLabel;
    QPushButton* _secondaryColorLighterButton;
    QColorDialog* _secondaryColorDeep;
    QLabel* _secondaryColorDeepLabel;
    QPushButton* _secondaryColorDeepButton;
    QColorDialog* _altSecondaryColor;
    QLabel* _altSecondaryColorLabel;
    QPushButton* _altSecondaryColorButton;
    QColorDialog* _altSecondaryColorLight;
    QLabel* _altSecondaryColorLightLabel;
    QPushButton* _altSecondaryColorLightButton;
    QColorDialog* _neutralColor;
    QLabel* _neutralColorLabel;
    QPushButton* _neutralColorButton;
    QColorDialog* _neutralColorLight;
    QLabel* _neutralColorLightLabel;
    QPushButton* _neutralColorLightButton;
    QColorDialog* _neutralColorLighter;
    QLabel* _neutralColorLighterLabel;
    QPushButton* _neutralColorLighterButton;
    QColorDialog* _lightTextColor;
    QLabel* _lightTextColorLabel;
    QPushButton* _lightTextColorButton;

    QFormLayout* _appearanceLayout;
    QGroupBox* _appearanceGroupBox;
    QScrollArea* _appearanceScrollArea;
    QPushButton* _saveButton;
    QPushButton* _cancelButton;
    QHBoxLayout* _buttonsLayout;
    QVBoxLayout _layout;
    bool _windowMoving;
    QPoint _mousePos;

private slots:
    void save();
    void cancel();
    void removeSelectedPreset();
    void shareSelectedPreset();
    void languageChanged(QString language);
    void presetChanged(QString preset);
    void fontFamilyChanged(QString fontFamily);
    void smallFontSizeChanged(QString fontSize);
    void regularFontSizeChanged(QString fontSize);
    void largeFontSizeChanged(QString fontSize);
    void extraLargeFontSizeChanged(QString fontSize);
    void changePrimaryColor();
    void changedPrimaryColor(const QColor& color);
    void changePrimaryColorLight();
    void changedPrimaryColorLight(const QColor& color);
    void changePrimaryColorDeep();
    void changedPrimaryColorDeep(const QColor& color);
    void changeAltPrimaryColor();
    void changedAltPrimaryColor(const QColor& color);
    void changeAltPrimaryColorLight();
    void changedAltPrimaryColorLight(const QColor& color);
    void changeAltPrimaryColorDeep();
    void changedAltPrimaryColorDeep(const QColor& color);
    void changeSecondaryColor();
    void changedSecondaryColor(const QColor& color);
    void changeSecondaryColorLight();
    void changedSecondaryColorLight(const QColor& color);
    void changeSecondaryColorLighter();
    void changedSecondaryColorLighter(const QColor& color);
    void changeSecondaryColorDeep();
    void changedSecondaryColorDeep(const QColor& color);
    void changeAltSecondaryColor();
    void changedAltSecondaryColor(const QColor& color);
    void changeAltSecondaryColorLight();
    void changedAltSecondaryColorLight(const QColor& color);
    void changeNeutralColor();
    void changedNeutralColor(const QColor& color);
    void changeNeutralColorLight();
    void changedNeutralColorLight(const QColor& color);
    void changeNeutralColorLighter();
    void changedNeutralColorLighter(const QColor& color);
    void changeLightTextColor();
    void changedLightTextColor(const QColor& color);
};

#endif  
