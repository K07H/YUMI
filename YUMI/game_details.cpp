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
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include <QStyleOption>
#include <QPainter>
#include "common.h"
#if IS_DEBUG
#include <QDebug>
#endif
#include "game_details.h"
#include "mods_loader.h"
#include "yumi.h"
#include "assets.h"

GameDetails::GameDetails(void* yumiPtr, QWidget* parent) : QWidget(parent)
{
    _uninstallWindow = NULL;
    _modsLoaderSettingsWindow = NULL;
    _yumiPtr = yumiPtr;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setHeight();

    _gameNameLbl.setText("Game Name");
    _gameNameLbl.setStyleSheet("QLabel { " + Assets::Instance()->TITLE_LABEL_STYLE + " }");
    _gameNameLblShadow = new QGraphicsDropShadowEffect(&_gameNameLbl);
    _gameNameLblShadow->setBlurRadius(10);
    _gameNameLblShadow->setOffset(3, 3);
    _gameNameLbl.setGraphicsEffect(_gameNameLblShadow);

    _modsLoaderNotInstalledLbl.setText(QCoreApplication::translate("GameDetails", "Mods loader is not installed for \"%1\".", "Label text").arg("Game Name"));
    _modsLoaderNotInstalledLbl.setStyleSheet("QLabel { margin-top: 40px; " + Assets::Instance()->ITALIC_LABEL_STYLE + " }");

    _installModsLoaderBtn.setText(QCoreApplication::translate("GameDetails", "&Install mods loader", "Button text"));
    _installModsLoaderBtn.setStatusTip(" " + QCoreApplication::translate("GameDetails", "Install mods loader for \"%1\".", "Tooltip text").arg("Game Name"));
    _installModsLoaderBtn.setStyleSheet(Assets::Instance()->mainBtnStyle);
    _installModsLoaderBtn.setCursor(Qt::PointingHandCursor);
    connect(&_installModsLoaderBtn, SIGNAL(clicked()), this, SLOT(installBepInExTrampoline()));
    _installModsLoaderBtnShadow = new QGraphicsDropShadowEffect(&_installModsLoaderBtn);
    _installModsLoaderBtnShadow->setBlurRadius(10);
    _installModsLoaderBtnShadow->setOffset(3, 3);
    _installModsLoaderBtn.setGraphicsEffect(_installModsLoaderBtnShadow);

    _removeGameBtn.setText(QCoreApplication::translate("GameDetails", "&Remove from games list", "Button text"));
    _removeGameBtn.setStatusTip(" " + QCoreApplication::translate("GameDetails", "Remove \"%1\" from the games list.", "Tooltip text").arg("Game Name"));
    _removeGameBtn.setStyleSheet(Assets::Instance()->mainBtnStyle);
    _removeGameBtn.setCursor(Qt::PointingHandCursor);
    connect(&_removeGameBtn, SIGNAL(clicked()), this, SLOT(removeFromGamesList()));
    _removeGameBtnShadow = new QGraphicsDropShadowEffect(&_removeGameBtn);
    _removeGameBtnShadow->setBlurRadius(10);
    _removeGameBtnShadow->setOffset(3, 3);
    _removeGameBtn.setGraphicsEffect(_removeGameBtnShadow);

    _uninstallModsLoaderBtn.setText(QCoreApplication::translate("GameDetails", "&Uninstall mods loader", "Button text"));
    _uninstallModsLoaderBtn.setStatusTip(" " + QCoreApplication::translate("GameDetails", "Uninstall mods loader for \"%1\".", "Tooltip text").arg("Game Name"));
    _uninstallModsLoaderBtn.setStyleSheet(Assets::Instance()->secondaryBtnStyle);
    _uninstallModsLoaderBtn.setCursor(Qt::PointingHandCursor);
    connect(&_uninstallModsLoaderBtn, SIGNAL(clicked()), this, SLOT(uninstallModsLoaderBtnClicked()));

    _openModsLoaderSettingsBtn.setText(QCoreApplication::translate("GameDetails", "&Mods loader settings", "Button text"));
    _openModsLoaderSettingsBtn.setStatusTip(" " + QCoreApplication::translate("GameDetails", "Open mods loader settings for \"%1\".", "Tooltip text").arg("Game Name"));
    _openModsLoaderSettingsBtn.setStyleSheet(Assets::Instance()->secondaryBtnStyle);
    _openModsLoaderSettingsBtn.setCursor(Qt::PointingHandCursor);
    connect(&_openModsLoaderSettingsBtn, SIGNAL(clicked()), this, SLOT(openModsLoaderSettingsBtnClicked()));

    _modsLoaderBtnsLayout = new QHBoxLayout();
    _modsLoaderBtnsLayout->setSpacing(0);
    _modsLoaderBtnsLayout->setContentsMargins(0, 0, 0, 0);
    _modsLoaderBtnsLayout->addWidget(&_uninstallModsLoaderBtn);
    _modsLoaderBtnsLayout->addSpacing(10);
    _modsLoaderBtnsLayout->addWidget(&_openModsLoaderSettingsBtn);

    _pluginsListContainer = new ModsListContainer(_yumiPtr, false, this);
    _pluginsListContainer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);

    _patchersListContainer = new ModsListContainer(_yumiPtr, true, this);
    _patchersListContainer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);

    _mainLayout.setContentsMargins(10, 0, 0, 0);
    _mainLayout.setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    _mainLayout.setSpacing(0);
    _mainLayout.addWidget(&_gameNameLbl, 1, Qt::AlignTop | Qt::AlignHCenter);
    _mainLayout.addWidget(&_modsLoaderNotInstalledLbl, 1, Qt::AlignHCenter);
    _mainLayout.addWidget(&_installModsLoaderBtn, 1, Qt::AlignHCenter);
    _mainLayout.addWidget(&_removeGameBtn, 1, Qt::AlignHCenter);
    _mainLayout.addSpacing(5);
    _mainLayout.addLayout(_modsLoaderBtnsLayout, 1);
    _mainLayout.addSpacing(5);
    _mainLayout.addWidget(_pluginsListContainer, 60);
    _mainLayout.addSpacing(5);
    _mainLayout.addWidget(_patchersListContainer, 38);

    setLayout(&_mainLayout);

    _uninstallModsLoaderBtn.setVisible(false);
    _openModsLoaderSettingsBtn.setVisible(false);
    _pluginsListContainer->setVisible(false);
    _patchersListContainer->setVisible(false);
}

GameDetails::~GameDetails() { }

void GameDetails::updateStyles()
{
    _gameNameLbl.setStyleSheet("QLabel { " + Assets::Instance()->TITLE_LABEL_STYLE + " }");
    _modsLoaderNotInstalledLbl.setStyleSheet("QLabel { margin-top: 40px; " + Assets::Instance()->ITALIC_LABEL_STYLE + " }");
    _installModsLoaderBtn.setStyleSheet(Assets::Instance()->mainBtnStyle);
    _removeGameBtn.setStyleSheet(Assets::Instance()->mainBtnStyle);
    _uninstallModsLoaderBtn.setStyleSheet(Assets::Instance()->secondaryBtnStyle);
    _openModsLoaderSettingsBtn.setStyleSheet(Assets::Instance()->secondaryBtnStyle);

    _pluginsListContainer->updateStyles();
    _patchersListContainer->updateStyles();
    if (_uninstallWindow != NULL)
        _uninstallWindow->updateStyles();
    if (_modsLoaderSettingsWindow != NULL)
        _modsLoaderSettingsWindow->updateStyles();

    adjustSize();
    update();
}

void GameDetails::setHeight(int height)
{
    setMinimumHeight(height <= 0 ? _preferedHeight : height);
    QTimer::singleShot(1000, this, &GameDetails::adjustMinHeight);
}

void GameDetails::adjustMinHeight()
{
    setMinimumHeight(_minimumHeight);
}

void GameDetails::openModsLoaderSettingsBtnClicked()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Open mods loader settings button clicked!";
#endif
    GameInfo* selectedGame = ((yumi*)_yumiPtr)->selectedGame;
    if (selectedGame == NULL || selectedGame->name.isEmpty())
        return;

    if (this->_modsLoaderSettingsWindow != NULL)
    {
        if (this->_modsLoaderSettingsWindow->isVisible())
            this->_modsLoaderSettingsWindow->close();
        delete this->_modsLoaderSettingsWindow;
        this->_modsLoaderSettingsWindow = NULL;
    }
    this->_modsLoaderSettingsWindow = new ModsLoaderSettings(this->_yumiPtr, selectedGame, this, NULL);
    this->_modsLoaderSettingsWindow->doShowAt(((yumi*)_yumiPtr)->getCenter());
}

void GameDetails::uninstallModsLoaderBtnClicked()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Uninstall mods loader button clicked!";
#endif
    GameInfo* selectedGame = ((yumi*)_yumiPtr)->selectedGame;
    if (selectedGame == NULL || selectedGame->name.isEmpty())
        return;

    if (this->_uninstallWindow != NULL)
    {
        if (this->_uninstallWindow->isVisible())
            this->_uninstallWindow->close();
        delete this->_uninstallWindow;
        this->_uninstallWindow = NULL;
    }
    this->_uninstallWindow = new UninstallWindow(this->_yumiPtr, selectedGame, this, NULL);
    this->_uninstallWindow->doShowAt(((yumi*)_yumiPtr)->getCenter());
}

void GameDetails::installBepInExTrampoline()
{
    ModsLoader::Instance()->installBepInEx(_yumiPtr, this, ((yumi*)_yumiPtr)->selectedGame, false);
}

void GameDetails::removeFromGamesList()
{
    QString gameName = ((yumi*)_yumiPtr)->selectedGame->name;
    QString gamePath = ((yumi*)_yumiPtr)->selectedGame->path;

    int toRemove = -1;
    int len = ((yumi*)_yumiPtr)->gamesInfo.count();
    if (len > 0)
        for (int i = 0; i < len; i++)
            if (gameName.compare(((yumi*)_yumiPtr)->gamesInfo[i].name) == 0 && gamePath.compare(((yumi*)_yumiPtr)->gamesInfo[i].path) == 0)
            {
                toRemove = i;
                break;
            }
    if (toRemove >= 0)
        ((yumi*)_yumiPtr)->gamesInfo.removeAt(toRemove);
    ((yumi*)_yumiPtr)->selectedGame = NULL;
    ((yumi*)_yumiPtr)->mainWidget->sideMenu->removeSideMenuElem(gameName, gamePath);
    ((yumi*)_yumiPtr)->mainWidget->toggleGameDetails();
    Config::Instance()->saveConfig();
}

QString GameDetails::askSteamFolderLocation()
{
    QMessageBox messageBox(this);
    messageBox.setWindowTitle(QCoreApplication::translate("GameDetails", "Steam folder not found", "Popup title"));
    messageBox.setTextFormat(Qt::PlainText);
    messageBox.setText(QCoreApplication::translate("GameDetails", "It was not possible to automatically find your Steam folder. Please click on OK then select your Steam folder.", "Popup text"));
    QAbstractButton* okButton = messageBox.addButton(QCoreApplication::translate("GameDetails", "&OK", "Button text"), QMessageBox::ActionRole);
    messageBox.addButton(QCoreApplication::translate("GameDetails", "&Cancel installation", "Button text"), QMessageBox::ActionRole);
    messageBox.exec();
    if (messageBox.clickedButton() == okButton)
    {
        QFileDialog fileDialog(this, QCoreApplication::translate("GameDetails", "Select Steam folder...", "Folder selection dialog title"));
        fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
        fileDialog.setFileMode(QFileDialog::Directory);
        if (fileDialog.exec() != QDialog::Accepted)
            return "";
        return fileDialog.selectedFiles().constFirst();
    }
    return "";
}

void GameDetails::updateGameDetails(GameInfo* details)
{
    if (details == NULL)
        return;
    _gameNameLbl.setText(details->name);
    _pluginsListContainer->clearItems();
    _patchersListContainer->clearItems();
    _pluginsListContainer->gameInfos = details;
    _patchersListContainer->gameInfos = details;
    _uninstallModsLoaderBtn.setStatusTip(" " + QCoreApplication::translate("GameDetails", "Uninstall mods loader for \"%1\".", "Tooltip text").arg(details->name));
    _openModsLoaderSettingsBtn.setStatusTip(" " + QCoreApplication::translate("GameDetails", "Open mods loader settings for \"%1\".", "Tooltip text").arg(details->name));
    QDir gameFolder(details->path);
    bool bepInExIsInstalled = ModsLoader::Instance()->isBepInExInstalled(details, gameFolder, this);
    if (!bepInExIsInstalled)
    {
        _modsLoaderNotInstalledLbl.setText(QCoreApplication::translate("GameDetails", "Mods loader is not installed for \"%1\".", "Button text").arg(details->name));
        _installModsLoaderBtn.setStatusTip(" " + QCoreApplication::translate("GameDetails", "Install mods loader for \"%1\".", "Tooltip text").arg(details->name));
        _removeGameBtn.setStatusTip(" " + QCoreApplication::translate("GameDetails", "Remove \"%1\" from the games list.", "Tooltip text").arg(details->name));
    }
    else
    {
        _pluginsListContainer->mods = ModsLoader::Instance()->getModsList(ModsLoader::Instance()->getBepInExSubFolderPath(gameFolder, "plugins"));
        if (_pluginsListContainer->mods != NULL)
        {
            int len = _pluginsListContainer->mods->count();
            if (len > 0)
                for (int i = 0; i < len; i++)
                    _pluginsListContainer->addNewItem((*_pluginsListContainer->mods)[i].name, false);
        }
        _pluginsListContainer->disabledMods = ModsLoader::Instance()->getModsList(ModsLoader::Instance()->getBepInExSubFolderPath(gameFolder, "disabled_plugins"));
        if (_pluginsListContainer->disabledMods != NULL)
        {
            int len = _pluginsListContainer->disabledMods->count();
            if (len > 0)
                for (int i = 0; i < len; i++)
                    _pluginsListContainer->addNewItem((*_pluginsListContainer->disabledMods)[i].name, true);
        }
        _patchersListContainer->mods = ModsLoader::Instance()->getModsList(ModsLoader::Instance()->getBepInExSubFolderPath(gameFolder, "patchers"));
        if (_patchersListContainer->mods != NULL)
        {
            int len = _patchersListContainer->mods->count();
            if (len > 0)
                for (int i = 0; i < len; i++)
                    _patchersListContainer->addNewItem((*_patchersListContainer->mods)[i].name, false);
        }
        _patchersListContainer->disabledMods = ModsLoader::Instance()->getModsList(ModsLoader::Instance()->getBepInExSubFolderPath(gameFolder, "disabled_patchers"));
        if (_patchersListContainer->disabledMods != NULL)
        {
            int len = _patchersListContainer->disabledMods->count();
            if (len > 0)
                for (int i = 0; i < len; i++)
                    _patchersListContainer->addNewItem((*_patchersListContainer->disabledMods)[i].name, true);
        }
    }
    _modsLoaderNotInstalledLbl.setVisible(!bepInExIsInstalled);
    _installModsLoaderBtn.setVisible(!bepInExIsInstalled);
    _removeGameBtn.setVisible(!bepInExIsInstalled);
    _uninstallModsLoaderBtn.setVisible(bepInExIsInstalled);
    _openModsLoaderSettingsBtn.setVisible(bepInExIsInstalled);
    _pluginsListContainer->setVisible(bepInExIsInstalled);
    _patchersListContainer->setVisible(bepInExIsInstalled);
    update();
}

#if IS_DEBUG && DEBUG_PAINTING
void GameDetails::paintEvent(QPaintEvent*)
{
    qDebug() << "GameDetails paint event!";
}
#endif
