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
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include "mods_list_container.h"
#include "common.h"
#include "yumi.h"
#include "utils.h"
#include "assets.h"

ModsListContainer::ModsListContainer(void* yumiPtr, const bool isForPatchers, QWidget* parent) : QWidget(parent)
{
    _yumiPtr = yumiPtr;
    _parent = parent;
    _isForPatchers = isForPatchers;
    gameInfos = NULL;
    mods = NULL;
    disabledMods = NULL;

    if (_isForPatchers)
    {
        _typeName = QCoreApplication::translate("ModsListContainer", "patcher", "Mod type name");
        _topLabel = QCoreApplication::translate("ModsListContainer", "Installed patchers:", "Patchers list label");
        _topDescription = QCoreApplication::translate("ModsListContainer", "Patchers are mods loaded before the game starts.", "Patchers description");
    }
    else
    {
        _typeName = QCoreApplication::translate("ModsListContainer", "plugin", "Mod type name");
        _topLabel = QCoreApplication::translate("ModsListContainer", "Installed plugins:", "Plugins list label");
        _topDescription = QCoreApplication::translate("ModsListContainer", "Plugins are mods loaded when the game starts.", "Plugins description");
    }

    _modsFolderName = (_isForPatchers ? "patchers" : "plugins");
    _disabledModsFolderName = (_isForPatchers ? "disabled_patchers" : "disabled_plugins");
    _disabledPrefix = "<" + QCoreApplication::translate("ModsListContainer", "DISABLED", "Disabled mod label") + "> ";

    _modsListLbl.setText(_topLabel);
    _modsListLbl.setStatusTip(" " + _topDescription);
    _modsListLbl.setAlignment(Qt::AlignLeft);
    _modsListLbl.setStyleSheet("QLabel { " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");

    _modsListScrollArea.setWidgetResizable(true);
    _modsListScrollArea.setAlignment(Qt::AlignTop);
    _modsListScrollArea.setStyleSheet("QScrollArea { " + Assets::Instance()->REGULAR_SCROLL_AREA_STYLE + " }");
    _modsListScrollArea.setContentsMargins(0, 0, 0, 0);
    _modsListScrollArea.setStatusTip(" " + _topDescription);

    _modsList = new ModsList(_disabledPrefix, this);

    _disableModBtn.setText("Disable selected mod");
    _disableModBtn.setStyleSheet(Assets::Instance()->secondaryBtnStyle);
    _disableModBtn.setCursor(Qt::PointingHandCursor);
    _disableModBtn.setStatusTip(" This will disable mod \"ModName\".");
    connect(&_disableModBtn, SIGNAL(clicked()), this, SLOT(disableModButtonClicked()));

    _deleteModBtn.setText(QCoreApplication::translate("ModsListContainer", "Delete selected %1", "Delete patcher or plugin label").arg(_typeName));
    _deleteModBtn.setStyleSheet(Assets::Instance()->secondaryBtnStyle);
    _deleteModBtn.setCursor(Qt::PointingHandCursor);
    _deleteModBtn.setStatusTip(" This will delete mod \"ModName\".");
    connect(&_deleteModBtn, SIGNAL(clicked()), this, SLOT(deleteModButtonClicked()));

    _buttonsBox.addWidget(&_disableModBtn);
    _buttonsBox.addSpacing(10);
    _buttonsBox.addWidget(&_deleteModBtn);
    _buttonsBox.setContentsMargins(0, 3, 0, 0);

    _modsListBox = new QVBoxLayout();
    _modsListBox->setContentsMargins(0, 0, 0, 0);
    _modsListBox->setSpacing(0);
    _modsListBox->addWidget(&_modsListLbl);
    _modsListBox->addWidget(&_modsListScrollArea);
    _modsListBox->addLayout(&_buttonsBox);

    setLayout(_modsListBox);
    _modsListScrollArea.setWidget(_modsList);

    _buttonsVisible = false;
    _disableModBtn.setVisible(false);
    _deleteModBtn.setVisible(false);
}

void ModsListContainer::updateStyles()
{
    if (_isForPatchers)
    {
        _typeName = QCoreApplication::translate("ModsListContainer", "patcher", "Mod type name");
        _topLabel = QCoreApplication::translate("ModsListContainer", "Installed patchers:", "Patchers list label");
        _topDescription = QCoreApplication::translate("ModsListContainer", "Patchers are mods loaded before the game starts.", "Patchers description");
    }
    else
    {
        _typeName = QCoreApplication::translate("ModsListContainer", "plugin", "Mod type name");
        _topLabel = QCoreApplication::translate("ModsListContainer", "Installed plugins:", "Plugins list label");
        _topDescription = QCoreApplication::translate("ModsListContainer", "Plugins are mods loaded when the game starts.", "Plugins description");
    }
    _modsListLbl.setText(_topLabel);
    _modsListLbl.setStatusTip(" " + _topDescription);
    _modsListScrollArea.setStatusTip(" " + _topDescription);
    this->_disabledPrefix = "<" + QCoreApplication::translate("ModsListContainer", "DISABLED", "Disabled mod label") + "> ";
    _modsList->updatePrefix(this->_disabledPrefix);
    _deleteModBtn.setText(QCoreApplication::translate("ModsListContainer", "Delete selected %1", "Delete patcher or plugin label").arg(_typeName));

    _modsListLbl.setStyleSheet("QLabel { " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");
    _modsListScrollArea.setStyleSheet("QScrollArea { " + Assets::Instance()->REGULAR_SCROLL_AREA_STYLE + " }");
    _disableModBtn.setStyleSheet(Assets::Instance()->secondaryBtnStyle);
    _deleteModBtn.setStyleSheet(Assets::Instance()->secondaryBtnStyle);

    _modsList->updateStyles();

    adjustSize();
    update();
}

void ModsListContainer::showErrorMsg(int errNo)
{
    if (errNo == 0)
        return ((yumi*)_yumiPtr)->showStatusBarMsg(QCoreApplication::translate("ModsListContainer", "No %1 selected.", "No patcher/plugin selected error label").arg(_typeName));
    if (errNo == 1)
        return ((yumi*)_yumiPtr)->showStatusBarMsg(QCoreApplication::translate("ModsListContainer", "Selection error.", "Error label"));
    if (errNo == 2)
        return ((yumi*)_yumiPtr)->showStatusBarMsg(QCoreApplication::translate("ModsListContainer", "BepInEx folder not found.", "Error label"));
    if (errNo == 3)
        return ((yumi*)_yumiPtr)->showStatusBarMsg(QCoreApplication::translate("ModsListContainer", "Unable to create disabled %1 folder.", "Unable to create disabled patcher/plugin folder error label").arg(_typeName));
    if (errNo == 4)
        return ((yumi*)_yumiPtr)->showStatusBarMsg(QCoreApplication::translate("ModsListContainer", "Disabled %1 folder not found.", "Disabled patcher/plugin folder not found error label").arg(_typeName));
    if (errNo == 5)
        return ((yumi*)_yumiPtr)->showStatusBarMsg(QCoreApplication::translate("ModsListContainer", "Unable to parse %1 name.", "Unable to parse patcher/plugin name error label").arg(_typeName));
    if (errNo == 6)
        return ((yumi*)_yumiPtr)->showStatusBarMsg(QCoreApplication::translate("ModsListContainer", "%1 file not found.", "Patcher/plugin file not found error label").arg(Utils::capitalizeFirstLetter(_typeName)));
    if (errNo == 7)
        return ((yumi*)_yumiPtr)->showStatusBarMsg(QCoreApplication::translate("ModsListContainer", "Unable to copy %1 file.", "Unable to copy patcher/plugin file error label").arg(_typeName));
    if (errNo == 8)
        return ((yumi*)_yumiPtr)->showStatusBarMsg(QCoreApplication::translate("ModsListContainer", "%1 folder not found.", "Patcher/plugin folder not found error label").arg(Utils::capitalizeFirstLetter(_typeName)));
    if (errNo == 9)
        return ((yumi*)_yumiPtr)->showStatusBarMsg(QCoreApplication::translate("ModsListContainer", "Unable to copy %1 folder.", "Unable to copy patcher/plugin folder error label").arg(_typeName));
    if (errNo == 10)
        return ((yumi*)_yumiPtr)->showStatusBarMsg(QCoreApplication::translate("ModsListContainer", "Unable to delete %1 file.", "Unable to delete patcher/plugin file error label").arg(_typeName));
    if (errNo == 11)
        return ((yumi*)_yumiPtr)->showStatusBarMsg(QCoreApplication::translate("ModsListContainer", "Unable to delete %1 folder.", "Unable to delete patcher/plugin folder error label").arg(_typeName));
    if (errNo == 12)
        return ((yumi*)_yumiPtr)->showStatusBarMsg(QCoreApplication::translate("ModsListContainer", "Could not find disabled %1 (list is empty).", "Unable to find disabled plugin/patcher error label").arg(_typeName));
    if (errNo == 13)
        return ((yumi*)_yumiPtr)->showStatusBarMsg(QCoreApplication::translate("ModsListContainer", "Could not find %1 (list is empty).", "Unable to find plugin/patcher error label").arg(_typeName));
    if (errNo == 14)
        return ((yumi*)_yumiPtr)->showStatusBarMsg(QCoreApplication::translate("ModsListContainer", "Could not find disabled %1 (no %1 in the list).", "Unable to find disabled plugin/patcher error label").arg(_typeName));
    if (errNo == 15)
        return ((yumi*)_yumiPtr)->showStatusBarMsg(QCoreApplication::translate("ModsListContainer", "Could not find %1 (no %1 in the list).", "Unable to find plugin/patcher error label").arg(_typeName));
    if (errNo == 16)
        return ((yumi*)_yumiPtr)->showStatusBarMsg(QCoreApplication::translate("ModsListContainer", "Could not find disabled %1.", "Unable to find disabled plugin/patcher error label").arg(_typeName));
    if (errNo == 17)
        return ((yumi*)_yumiPtr)->showStatusBarMsg(QCoreApplication::translate("ModsListContainer", "Could not find %1.", "Unable to find plugin/patcher error label").arg(_typeName));

    return ((yumi*)_yumiPtr)->showStatusBarMsg(QCoreApplication::translate("ModsListContainer", "Unknown error.", "Error label").arg(_typeName));
}

ModInfo* ModsListContainer::getSelectedMod(bool showErrorTooltip)
{
    if (_modsList == NULL || _modsList->selectedMod == NULL)
    {
        if (showErrorTooltip)
            showErrorMsg(0);
        return NULL;
    }

    bool isDisabledMod = false;
    QString modName = _modsList->selectedMod->text();

    if (modName.startsWith(_disabledPrefix))
    {
        isDisabledMod = true;
        modName = modName.mid(_disabledPrefix.length());
    }
    int versionPos = modName.lastIndexOf(" (v");
    if (versionPos > 0 && modName.endsWith(")"))
        modName = modName.left(versionPos);

    int len = 0;
    if (isDisabledMod)
    {
        if (disabledMods == NULL)
        {
            if (showErrorTooltip)
                showErrorMsg(12);
            return NULL;
        }
        len = disabledMods->count();
    }
    else
    {
        if (mods == NULL)
        {
            if (showErrorTooltip)
                showErrorMsg(13);
            return NULL;
        }
        len = mods->count();
    }

    if (len <= 0)
    {
        if (showErrorTooltip)
            showErrorMsg(isDisabledMod ? 14 : 15);
        return NULL;
    }

    for (int i = 0; i < len; i++)
    {
        if (isDisabledMod)
        {
            if ((*disabledMods)[i].name.compare(modName) == 0)
                return &((*disabledMods)[i]);
        }
        else
        {
            if ((*mods)[i].name.compare(modName) == 0)
                return &((*mods)[i]);
        }
    }
    if (showErrorTooltip)
        showErrorMsg(isDisabledMod ? 16 : 17);
    return NULL;
}

bool ModsListContainer::isSelectedModDisabled()
{
    if (_modsList == NULL || _modsList->selectedMod == NULL)
        return false;
    return _modsList->selectedMod->text().startsWith(_disabledPrefix);
}

bool ModsListContainer::removeModFromList(QList<ModInfo>* list, ModInfo* mod)
{
    if (list == NULL || mod == NULL)
        return false;
    int removeAt = -1;
    int len = list->length();
    if (len > 0)
        for (int i = 0; i < len; i++)
            if ((*list)[i].name.compare(mod->name) == 0 && (*list)[i].path.compare(mod->path) == 0)
            {
                removeAt = i;
                break;
            }
    if (removeAt < 0)
        return false;
    list->removeAt(removeAt);
    return true;
}

bool ModsListContainer::swapModFromLists(ModInfo* mod, const bool fromDisabled)
{
    if (mod == NULL)
        return false;
    ModInfo copy(*mod);
    if (fromDisabled)
    {
        if (mods == NULL)
            mods = new QList<ModInfo>();
        mods->push_back(copy);
        return removeModFromList(disabledMods, mod);
    }
    else
    {
        if (disabledMods == NULL)
            disabledMods = new QList<ModInfo>();
        disabledMods->push_back(copy);
        return removeModFromList(mods, mod);
    }
}

void ModsListContainer::disableModButtonClicked()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Disable/enable mod button clicked!";
#endif
    ModInfo* selectedMod = getSelectedMod();
    if (selectedMod == NULL)
        return;
    if (gameInfos == NULL)
        return showErrorMsg(1);
    QDir disabledFolder(gameInfos->path);
    if (!disabledFolder.exists())
        return showErrorMsg(1);
    if (!disabledFolder.cd("BepInEx"))
        return showErrorMsg(2);

    if (!disabledFolder.cd(_disabledModsFolderName))
    {
        if (!disabledFolder.mkdir(_disabledModsFolderName))
            return showErrorMsg(3);
        if (!disabledFolder.cd(_disabledModsFolderName))
            return showErrorMsg(4);
    }
    if (!disabledFolder.exists())
        return showErrorMsg(4);

    QString selectedModPath(Utils::toUnixPath(selectedMod->path));
    int lastSeparatorPos = selectedModPath.lastIndexOf('/');
    if (lastSeparatorPos < 0)
        return showErrorMsg(5);
    QString modFileName(selectedModPath.length() > (lastSeparatorPos + 1) ? selectedModPath.mid(lastSeparatorPos + 1) : selectedModPath);
    if (modFileName.contains('/'))
        return showErrorMsg(5);

    bool selectedModDisabled = isSelectedModDisabled();
    QString destinationPath = Utils::toUnixPath(gameInfos->path + "/BepInEx/" + (selectedModDisabled ? _modsFolderName : _disabledModsFolderName) + "/" + modFileName);
    QString destFolderName(destinationPath.left(destinationPath.lastIndexOf('/')));
    bool isFile = selectedModPath.endsWith(".dll", Qt::CaseInsensitive);

    QString title(QCoreApplication::translate("ModsListContainer", "Confirm action", "Popup title"));
    QString argA;
    QString argB;
    if (selectedModDisabled)
    {
        argA = QCoreApplication::translate("ModsListContainer", "an enabled", "Popup text element");
        argB = QCoreApplication::translate("ModsListContainer", "enable", "Popup text element");
    }
    else
    {
        argA = QCoreApplication::translate("ModsListContainer", "a disabled", "Popup text element");
        argB = QCoreApplication::translate("ModsListContainer", "disable", "Popup text element");
    }
    QString msg(QCoreApplication::translate("ModsListContainer", "There is already %1 %5 called \"%2\" in folder \"%3\". Continue and %4 %5 \"%6\" (this will replace previous copy of the %5)?", "Popup text (%1=an enabled/a disabled, %2=Mod file name, %3=Destination folder name, %4=enable/disable, %5=patcher/plugin, %6=Mod name)").arg(argA, modFileName, destFolderName, argB, _typeName, selectedMod->name));
    if (isFile)
    {
        QFile destFile(destinationPath);
        if (destFile.exists())
        {
            QMessageBox confirmBox(QMessageBox::Warning, title, msg, QMessageBox::Yes | QMessageBox::No, (GameDetails*)_parent);
            if (confirmBox.exec() != QMessageBox::Yes)
                return;
            if (!destFile.remove())
                return showErrorMsg(10);
        }
    }
    else
    {
        QDir destFolder(destinationPath);
        if (destFolder.exists())
        {
            QMessageBox confirmBox(QMessageBox::Warning, title, msg, QMessageBox::Yes | QMessageBox::No, (GameDetails*)_parent);
            if (confirmBox.exec() != QMessageBox::Yes)
                return;
            if (!destFolder.removeRecursively())
                return showErrorMsg(11);
        }
    }

    if (isFile)
    {
        QFile modFile(selectedModPath);
        if (!modFile.exists())
            return showErrorMsg(6);
        if (!modFile.rename(destinationPath))
            return showErrorMsg(7);
    }
    else
    {
        QDir modFolder(selectedModPath);
        if (!modFolder.exists())
            return showErrorMsg(8);
        if (!disabledFolder.rename(selectedModPath, destinationPath))
            return showErrorMsg(9);
    }
    if (swapModFromLists(selectedMod, selectedModDisabled))
        qInfo().nospace() << "Mod " << modFileName << " has been " << (selectedModDisabled ? "Enabled" : "Disabled") << ".";
    else
        qCritical().nospace() << "Failed to swap mod " << modFileName << " from " << (selectedModDisabled ? "Disabled" : "Enabled") << " to " << (selectedModDisabled ? "Enabled" : "Disabled") << ".";
    ((GameDetails*)_parent)->updateGameDetails(gameInfos);
}

void ModsListContainer::deleteModButtonClicked()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug() << "Delete mod button clicked!";
#endif
    ModInfo* selectedMod = getSelectedMod();
    if (selectedMod == NULL)
        return;
    QString modName(selectedMod->name);
    QMessageBox confirmBox(QMessageBox::Question, QCoreApplication::translate("ModsListContainer", "Confirm removal", "Popup title"), QCoreApplication::translate("ModsListContainer", "Are you sure that you want to delete \"%1\" mod?", "Popup text (%1=Mod name)").arg(modName), QMessageBox::Yes | QMessageBox::No, (GameDetails*)_parent);
    int clickedBtn = confirmBox.exec();
    if (clickedBtn != QMessageBox::Yes)
        return;

    QString selectedModPath(Utils::toUnixPath(selectedMod->path));
    if (selectedModPath.endsWith(".dll", Qt::CaseInsensitive))
    {
        QFile modFile(selectedModPath);
        if (!modFile.exists())
            return showErrorMsg(6);
        if (!modFile.remove())
            return showErrorMsg(10);
    }
    else
    {
        QDir modFolder(selectedModPath);
        if (!modFolder.exists())
            return showErrorMsg(8);
        if (!modFolder.removeRecursively())
            return showErrorMsg(11);
    }
    if (removeModFromList((isSelectedModDisabled() ? disabledMods : mods), selectedMod))
        qInfo().nospace() << "Mod " << modName << " has been deleted.";
    else
        qCritical().nospace() << "Could not remove " << modName << " from internal list.";
    ((GameDetails*)_parent)->updateGameDetails(gameInfos);
}

void ModsListContainer::toggleButtons(const bool visible)
{
    if (_buttonsVisible == visible)
        return;
    _buttonsVisible = visible;
    _disableModBtn.setVisible(visible);
    _deleteModBtn.setVisible(visible);
}

void ModsListContainer::updateDisableButtonText()
{
    bool selectedModDisabled = isSelectedModDisabled();

    ModInfo* selectedMod = getSelectedMod();
    QString modName = (selectedMod == NULL ? "ModName" : selectedMod->name);

    if (selectedModDisabled)
    {
        _disableModBtn.setText(QCoreApplication::translate("ModsListContainer", "Enable selected %1", "Enable selected plugin or patcher button text").arg(_typeName));
        _disableModBtn.setStatusTip(" " + QCoreApplication::translate("ModsListContainer", "Enable %1 \"%2\".", "Enable selected plugin or patcher tooltip text").arg(_typeName, modName));
    }
    else
    {
        _disableModBtn.setText(QCoreApplication::translate("ModsListContainer", "Disable selected %1", "Disable selected plugin or patcher button text").arg(_typeName));
        _disableModBtn.setStatusTip(" " + QCoreApplication::translate("ModsListContainer", "Disable %1 \"%2\".", "Disable selected plugin or patcher tooltip text").arg(_typeName, modName));
    }
    _deleteModBtn.setStatusTip(" " + QCoreApplication::translate("ModsListContainer", "Delete %1 \"%2\".", "Delete selected plugin or patcher tooltip text").arg(_typeName, modName));
}

void ModsListContainer::addNewItem(const QString& item, const QString& version, const bool isDisabled)
{
    _modsList->addNewItem((isDisabled ? _disabledPrefix + item : item) + (!version.isEmpty() ? " (v" + version + ")" : ""), isDisabled);
}

void ModsListContainer::clearItems()
{
    toggleButtons(false);
    _modsList->clearItems();
}
