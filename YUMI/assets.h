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

#ifndef __ASSETS_H__
#define __ASSETS_H__

#include <QObject>
#include <QString>
#include <QIcon>
#include <QFile>
#include <QPixmap>
#include "bepinexfile.h"
#include "yumi_filedownloader.h"

class Assets : public QObject
{
    Q_OBJECT

private:
    explicit Assets(QObject* parent = NULL);
    virtual ~Assets();
    Assets() = delete;
    Assets(const Assets& other) = delete;
    Assets& operator=(const Assets&) = delete;

    bool extractBepInExArchive(const QString& bepInExName, const QString& downloadsFolderPath);
    bool downloadBepInExArchive(const QString& bepInExName, const QString& downloadsFolderPath);
    bool isBepInExUpToDate(const QString& bepInExFolderPath);
    bool getLocalBepInEx(const QString& bepInExName, const QString& bepInExFolderPath, const QString& downloadsFolderPath);

    static Assets* _instance;

    void* _yumiPtr;
    YumiFileDownloader* _fileDownloader;
    QList<BepInExFile*>* _unixBepInEx;
    QList<BepInExFile*>* _x64BepInEx;
    QList<BepInExFile*>* _x86BepInEx;
    QList<BepInExFile*>* _il2cppx64BepInEx;
    QList<BepInExFile*>* _il2cppx86BepInEx;

public:
    static Assets* Init(QObject* parent = NULL);
    static Assets* Instance();

    QList<BepInExFile*>* getUnixFiles();
    QList<BepInExFile*>* getX64Files();
    QList<BepInExFile*>* getX86Files();
    QList<BepInExFile*>* getIL2CPPX64Files();
    QList<BepInExFile*>* getIL2CPPX86Files();
    QString getColorBtnStyle(const QString& color);
    void updateStyles();
    void resetStyles();

    const QString licenseFile = ":/yumiresources/FULL_LICENSE.txt";
#ifdef Q_OS_DARWIN
    const QString imgRsrcPath = ":/yumiresources/images/mac";
#else
    const QString imgRsrcPath = ":/yumiresources/images/win";
#endif

    const QPixmap appLogoImg = QPixmap(imgRsrcPath + "/yumilogo.png");
    const QPixmap appIconImg = QPixmap(imgRsrcPath + "/yumilogo_transparent.png");
    const QIcon appIcon = QIcon(imgRsrcPath + "/yumilogo_transparent.png");
    const QIcon openIcon = QIcon(imgRsrcPath + "/fileopen.png");
    const QIcon settingsIcon = QIcon(imgRsrcPath + "/settings.png");
    const QIcon exitIcon = QIcon(imgRsrcPath + "/exitsign.png");
    const QIcon bookIcon = QIcon(imgRsrcPath + "/book.png");
    const QIcon updatesIcon = QIcon(imgRsrcPath + "/updates.png");
    const QIcon infoIcon = QIcon(imgRsrcPath + "/info.png");
    const QIcon minimizeIcon = QIcon(imgRsrcPath + "/minimize.png");
    const QIcon maximizeIcon = QIcon(imgRsrcPath + "/expand-window.png");
    const QIcon showNormalIcon = QIcon(imgRsrcPath + "/shrink-window.png");
    const QIcon closeIcon = QIcon(imgRsrcPath + "/close.png");
    const QIcon checkboxUnchecked = QIcon(imgRsrcPath + "/checkbox_unchecked.png");
    const QIcon checkboxUncheckedHover = QIcon(imgRsrcPath + "/checkbox_unchecked_hover.png");
    const QIcon checkboxUncheckedPressed = QIcon(imgRsrcPath + "/checkbox_unchecked_pressed.png");
    const QIcon checkboxChecked = QIcon(imgRsrcPath + "/checkbox_checked.png");
    const QIcon checkboxCheckedHover = QIcon(imgRsrcPath + "/checkbox_checked_hover.png");
    const QIcon checkboxCheckedPressed = QIcon(imgRsrcPath + "/checkbox_checked_pressed.png");

    QString PRIMARY_FONT_FAMILY = QString("Gill Sans MT");
    QString SECONDARY_FONT_FAMILY = QString("Courier New");

    QString SM_FONT_SIZE_PX = QString("14");
    QString DEFAULT_FONT_SIZE_PX = QString("16");
    QString XL_FONT_SIZE_PX = QString("20");
    QString XXL_FONT_SIZE_PX = QString("24");

    QString PRIMARY_COLOR_HEX = QString("#FFC65A");
    QString PRIMARY_COLOR_LIGHT_HEX = QString("#FFE4B0");
    QString PRIMARY_COLOR_DEEP_HEX = QString("#FFAC0E");

    QString ALT_PRIMARY_COLOR_HEX = QString("#E67200");
    QString ALT_PRIMARY_COLOR_LIGHT_HEX = QString("#FFA940");
    QString ALT_PRIMARY_COLOR_DEEP_HEX = QString("#BB4600");

    QString SECONDARY_COLOR_HEX = QString("#152340");
    QString SECONDARY_COLOR_LIGHT_HEX = QString("#28457D");
    QString SECONDARY_COLOR_LIGHTER_HEX = QString("#285AA0");
    QString SECONDARY_COLOR_DEEP_HEX = QString("#022C8C");

    QString ALT_SECONDARY_COLOR_HEX = QString("#24425C");
    QString ALT_SECONDARY_COLOR_LIGHT_HEX = QString("#3393BA");

    QString NEUTRAL_COLOR_HEX = QString("#373737");
    QString NEUTRAL_COLOR_LIGHT_HEX = QString("#565656");
    QString NEUTRAL_COLOR_LIGHTER_HEX = QString("#6C6C6C");

    QString LIGHT_TEXT_COLOR_HEX = QString("#FFFFFF");

    QColor primaryColor = QColor(PRIMARY_COLOR_HEX);
    QColor secondaryColor = QColor(SECONDARY_COLOR_HEX);
    QColor secondaryColorLight = QColor(SECONDARY_COLOR_LIGHT_HEX);
    QColor secondaryColorDeep = QColor(SECONDARY_COLOR_DEEP_HEX);
    QColor lightTextColor = QColor(LIGHT_TEXT_COLOR_HEX);

    QString DEFAULT_WINDOW_STYLE = QString("background-color: " + PRIMARY_COLOR_HEX + "; border: 1px solid " + SECONDARY_COLOR_HEX + "; border-radius: 10px; margin: 0; padding: 0;");
    QString DEFAULT_WINDOW_TITLE_STYLE = QString("background-color: " + PRIMARY_COLOR_LIGHT_HEX + "; border-top: 1px solid " + SECONDARY_COLOR_HEX + "; border-left: 1px solid " + SECONDARY_COLOR_HEX + "; border-right: 1px solid " + SECONDARY_COLOR_HEX + "; border-bottom: 3px solid " + SECONDARY_COLOR_HEX + "; border-top-left-radius: 10px; border-top-right-radius: 10px;");
    QString REGULAR_SCROLL_AREA_STYLE = QString("background-color: " + SECONDARY_COLOR_LIGHT_HEX + "; border: 1px solid " + SECONDARY_COLOR_HEX + "; border-radius: 6px;");
    QString SIDE_MENU_SCROLL_AREA_STYLE = QString("background-color: " + PRIMARY_COLOR_LIGHT_HEX + "; border-radius: 10px;");

    QString TITLE_LABEL_STYLE = QString("color: " + SECONDARY_COLOR_HEX + "; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; font-size: " + XXL_FONT_SIZE_PX + "px; font-weight: 500;");
    QString SMALLER_TITLE_LABEL_STYLE = QString("color: " + SECONDARY_COLOR_HEX + "; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; font-size: " + XL_FONT_SIZE_PX + "px; font-weight: 500;");
    QString REGULAR_LABEL_STYLE = QString("color: " + SECONDARY_COLOR_HEX + "; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; font-size: " + DEFAULT_FONT_SIZE_PX + "px;");
    QString ALTERNATE_LABEL_STYLE = QString("color: " + LIGHT_TEXT_COLOR_HEX + "; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; font-size: " + DEFAULT_FONT_SIZE_PX + "px;");
    QString ITALIC_LABEL_STYLE = QString("color: " + SECONDARY_COLOR_HEX + "; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; font-size: " + DEFAULT_FONT_SIZE_PX + "px; font-style: italic;");
    QString BOLD_LABEL_STYLE = QString("color: " + SECONDARY_COLOR_HEX + "; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; font-size: " + DEFAULT_FONT_SIZE_PX + "px; font-weight: bold;");
    QString ABOUT_LABEL_STYLE = QString("color: " + SECONDARY_COLOR_HEX + "; font-family: \"" + SECONDARY_FONT_FAMILY + "\", sans-serif; font-size: " + SM_FONT_SIZE_PX + "px;");
    QString LICENSE_FONT_STYLE = QString("color: " + LIGHT_TEXT_COLOR_HEX + "; font-family: \"" + SECONDARY_FONT_FAMILY + "\", sans-serif; font-size: 12px;");
    QString COMBOBOX_LABEL_STYLE = QString("color: black; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; font-size: " + DEFAULT_FONT_SIZE_PX + "px;");

    QString MAIN_BUTTON_STYLE = QString("min-height: 40px; max-width: 350px; margin: 5px; padding-left: 15px; padding-right: 15px; color: " + LIGHT_TEXT_COLOR_HEX + "; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; font-size: " + XL_FONT_SIZE_PX + "px; font-weight: 500; border: 2px solid " + SECONDARY_COLOR_HEX + "; border-radius: 6px; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 " + SECONDARY_COLOR_LIGHTER_HEX + ", stop: 1 " + SECONDARY_COLOR_HEX + ");");
    QString MAIN_BUTTON_STYLE_FULLWIDTH = QString("min-height: 40px; color: " + LIGHT_TEXT_COLOR_HEX + "; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; font-size: " + XL_FONT_SIZE_PX + "px; font-weight: 500; border: 2px solid " + SECONDARY_COLOR_HEX + "; border-radius: 6px; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 " + SECONDARY_COLOR_LIGHTER_HEX + ", stop: 1 " + SECONDARY_COLOR_HEX + ");");
    QString MAIN_BUTTON_STYLE_HOVER = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 " + ALT_SECONDARY_COLOR_LIGHT_HEX + ", stop: 1 " + ALT_SECONDARY_COLOR_HEX + ");");
    QString MAIN_BUTTON_STYLE_PRESSED = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 " + ALT_PRIMARY_COLOR_LIGHT_HEX + ", stop: 1 " + ALT_PRIMARY_COLOR_HEX + ");");
    const QString MAIN_BUTTON_STYLE_FLAT = QString("border: none;");
    const QString MAIN_BUTTON_STYLE_DEFAULT = QString("border-color: navy;");

    QString SECONDARY_BUTTON_STYLE = QString("min-height: 25px; padding-left: 5px; padding-right: 5px; font-size: " + SM_FONT_SIZE_PX + "px; color: " + LIGHT_TEXT_COLOR_HEX + "; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; border: 1px solid " + SECONDARY_COLOR_HEX + "; border-radius: 6px; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 " + SECONDARY_COLOR_LIGHTER_HEX + ", stop: 1 " + SECONDARY_COLOR_HEX + ");");
    QString SECONDARY_BUTTON_STYLE_DISABLED = QString("min-height: 25px; padding-left: 5px; padding-right: 5px; font-size: " + SM_FONT_SIZE_PX + "px; color: " + LIGHT_TEXT_COLOR_HEX + "; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; border: 1px solid " + SECONDARY_COLOR_HEX + "; border-radius: 6px; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 " + NEUTRAL_COLOR_LIGHTER_HEX + ", stop: 1 " + NEUTRAL_COLOR_HEX + ");");
    QString SECONDARY_BUTTON_STYLE_SELECTED = QString("min-height: 30px; padding-left: 5px; padding-right: 5px; font-size: " + SM_FONT_SIZE_PX + "px; font-weight: 500; color: " + LIGHT_TEXT_COLOR_HEX + "; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; border: 3px solid " + SECONDARY_COLOR_HEX + "; border-radius: 6px; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 " + ALT_PRIMARY_COLOR_HEX + ", stop: 1 " + ALT_PRIMARY_COLOR_DEEP_HEX + ");");
    QString SECONDARY_BUTTON_STYLE_HOVER = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 " + ALT_SECONDARY_COLOR_LIGHT_HEX + ", stop: 1 " + ALT_SECONDARY_COLOR_HEX + ");");
    QString SECONDARY_BUTTON_STYLE_SELECTED_HOVER = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 " + ALT_PRIMARY_COLOR_HEX + ", stop: 1 " + ALT_PRIMARY_COLOR_DEEP_HEX + ");");
    QString SECONDARY_BUTTON_STYLE_PRESSED = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 " + ALT_PRIMARY_COLOR_LIGHT_HEX + ", stop: 1 " + ALT_PRIMARY_COLOR_HEX + ");");
    const QString SECONDARY_BUTTON_STYLE_FLAT = QString("border: none;");
    const QString SECONDARY_BUTTON_STYLE_DEFAULT = QString("border-color: navy;");

    QString mainBtnStyle = "QPushButton { " + MAIN_BUTTON_STYLE +  " } "
        "QPushButton:hover { " + MAIN_BUTTON_STYLE_HOVER + " } "
        "QPushButton:pressed { " + MAIN_BUTTON_STYLE_PRESSED + " } "
        "QPushButton:flat { " + MAIN_BUTTON_STYLE_FLAT + " } "
        "QPushButton:default { " + MAIN_BUTTON_STYLE_DEFAULT + " }";

    QString mainBtnStyleFullWidth = "QPushButton { " + MAIN_BUTTON_STYLE_FULLWIDTH +  " } "
        "QPushButton:hover { " + MAIN_BUTTON_STYLE_HOVER + " } "
        "QPushButton:pressed { " + MAIN_BUTTON_STYLE_PRESSED + " } "
        "QPushButton:flat { " + MAIN_BUTTON_STYLE_FLAT + " } "
        "QPushButton:default { " + MAIN_BUTTON_STYLE_DEFAULT + " }";

    QString secondaryBtnStyle = "QPushButton { " + SECONDARY_BUTTON_STYLE +  " } "
        "QPushButton:hover { " + SECONDARY_BUTTON_STYLE_HOVER + " } "
        "QPushButton:pressed { " + SECONDARY_BUTTON_STYLE_PRESSED + " } "
        "QPushButton:flat { " + SECONDARY_BUTTON_STYLE_FLAT + " } "
        "QPushButton:default { " + SECONDARY_BUTTON_STYLE_DEFAULT + " }";

    QString secondaryBtnStyleDisabled = "QPushButton { " + SECONDARY_BUTTON_STYLE_DISABLED +  " } "
        "QPushButton:hover { " + SECONDARY_BUTTON_STYLE_HOVER + " } "
        "QPushButton:pressed { " + SECONDARY_BUTTON_STYLE_PRESSED + " } "
        "QPushButton:flat { " + SECONDARY_BUTTON_STYLE_FLAT + " } "
        "QPushButton:default { " + SECONDARY_BUTTON_STYLE_DEFAULT + " }";

    QString secondaryBtnStyleSelected = "QPushButton { " + SECONDARY_BUTTON_STYLE_SELECTED +  " } "
        "QPushButton:hover { " + SECONDARY_BUTTON_STYLE_SELECTED_HOVER + " } "
        "QPushButton:pressed { " + SECONDARY_BUTTON_STYLE_PRESSED + " } "
        "QPushButton:flat { " + SECONDARY_BUTTON_STYLE_FLAT + " } "
        "QPushButton:default { " + SECONDARY_BUTTON_STYLE_DEFAULT + " }";

    QString secondaryBtnStyleSelectedDisabled = "QPushButton { " + SECONDARY_BUTTON_STYLE_DISABLED +  " } "
        "QPushButton:hover { " + SECONDARY_BUTTON_STYLE_SELECTED_HOVER + " } "
        "QPushButton:pressed { " + SECONDARY_BUTTON_STYLE_PRESSED + " } "
        "QPushButton:flat { " + SECONDARY_BUTTON_STYLE_FLAT + " } "
        "QPushButton:default { " + SECONDARY_BUTTON_STYLE_DEFAULT + " }";

    QString checkboxStyle = "QCheckBox { margin-left: 10px; margin-right: 10px; padding: 5px; font-size: " + DEFAULT_FONT_SIZE_PX + "px; font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; color: " + SECONDARY_COLOR_HEX + "; } "
        "QCheckBox:hover { color: " + SECONDARY_COLOR_LIGHTER_HEX + "; border: 1px solid transparent; } "
        "QCheckBox::indicator { width: 25px; height: 25px; } "
        "QCheckBox::indicator:unchecked { image: url(" + imgRsrcPath + "/checkbox_unchecked.png); } "
        "QCheckBox::indicator:unchecked:hover { image: url(" + imgRsrcPath + "/checkbox_unchecked_hover.png); } "
        "QCheckBox::indicator:unchecked:pressed { image: url(" + imgRsrcPath + "/checkbox_unchecked_pressed.png); } "
        "QCheckBox::indicator:checked { image: url(" + imgRsrcPath + "/checkbox_checked.png); } "
        "QCheckBox::indicator:checked:hover { image: url(" + imgRsrcPath + "/checkbox_checked_hover.png); } "
        "QCheckBox::indicator:checked:pressed { image: url(" + imgRsrcPath + "/checkbox_checked_pressed.png); } "
        "QCheckBox::indicator:indeterminate:hover { image: url(" + imgRsrcPath + "/checkbox_indeterminated_hover.png); } "
        "QCheckBox::indicator:indeterminate:pressed { image: url(" + imgRsrcPath + "/checkbox_indeterminated_pressed.png); }";

    QString menuBarStyle = "QMenuBar { font-family: \"" + PRIMARY_FONT_FAMILY + "\", sans-serif; font-weight: bold; font-size: " + DEFAULT_FONT_SIZE_PX + "px; color: " + SECONDARY_COLOR_HEX + "; background-color: " + PRIMARY_COLOR_LIGHT_HEX + "; border-top: 1px solid " + SECONDARY_COLOR_HEX + "; border-left: 1px solid " + SECONDARY_COLOR_HEX + "; border-right: 1px solid " + SECONDARY_COLOR_HEX + "; border-bottom: 2px solid " + SECONDARY_COLOR_HEX + "; border-top-left-radius: 10px; border-top-right-radius: 10px; padding: 0; margin: 0; } "
#ifdef Q_OS_LINUX
        "QMenuBar::item { padding: 10px 14px 10px 14px; margin-top: 0px; margin-bottom: 0px; border: none; } "
#else
        "QMenuBar::item { padding: 9px 14px 8px 14px; margin-top: 0px; margin-bottom: 0px; border: none; } "
#endif
        "QMenuBar::item:selected { background-color: " + PRIMARY_COLOR_HEX + "; } "
        "QMenuBar::item:pressed { background-color: " + PRIMARY_COLOR_DEEP_HEX + "; } "
        "QMenu { background-color: " + PRIMARY_COLOR_DEEP_HEX + "; color: " + SECONDARY_COLOR_HEX + "; padding: 5px; } "
        "QMenu::icon { padding-left: 5px; } "
        "QMenu::item { color: " + SECONDARY_COLOR_HEX + "; min-width: 100px; font-weight: 600; font-size: " + SM_FONT_SIZE_PX + "px; padding: 7px; border: 1px solid transparent; border-radius: 6px; } "
        "QMenu::item:selected { background-color: " + PRIMARY_COLOR_LIGHT_HEX + "; }";

#ifdef Q_OS_LINUX
    const QString windowBtnsMenuBarStyle = "QMenuBar { border: none; background-color: transparent; } QMenuBar::item { border: none; padding: 12px 6px 11px 6px; margin: 0; }";
#else
    const QString windowBtnsMenuBarStyle = "QMenuBar { border: none; background-color: transparent; } QMenuBar::item { border: none; padding: 0; margin: 8% 5%; }";
#endif

    QString statusBarStyle = "QStatusBar { background-color: " + PRIMARY_COLOR_LIGHT_HEX + "; border: 1px solid " + SECONDARY_COLOR_HEX + "; border-bottom-left-radius: 10px; border-bottom-right-radius: 10px; font-style: italic; color: " + NEUTRAL_COLOR_LIGHT_HEX + "; } "
        "QStatusBar::item { font-style: italic; color: " + NEUTRAL_COLOR_LIGHT_HEX + "; } "
        "QSizeGrip { padding-right: 5px; padding-bottom: 5px; }";
};

#endif
