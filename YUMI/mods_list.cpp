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

#include <QDebug>
#include "mods_list.h"
#include "common.h"
#include "mods_list_container.h"
#include "assets.h"

ModsList::ModsList(const QString& disabledPrefix, QWidget* parent) : QWidget(parent)
{
    _parent = (void*)parent;
    _disabledPrefix = disabledPrefix;
    selectedMod = NULL;
    setStyleSheet("ModsList { background-color: transparent; }");
    _mainLayout.setAlignment(Qt::AlignTop);
    _mainLayout.setContentsMargins(0, 0, 0, 0);
    _mainLayout.setSpacing(5);
    setLayout(&_mainLayout);
}

ModsList::~ModsList()
{
    int len = _installedMods.count();
    for (int i = 0; i < len; i++)
        if (_installedMods[i] != NULL)
            delete _installedMods[i];
}

void ModsList::updatePrefix(const QString& prefix)
{
    QString currentBtnText;
    int len = this->_installedMods.count();
    for (int i = 0; i < len; i++)
    {
        currentBtnText = this->_installedMods.at(i)->text();
        if (currentBtnText.startsWith(this->_disabledPrefix))
            this->_installedMods.at(i)->setText(currentBtnText.replace(this->_disabledPrefix, prefix));
    }
    if (this->selectedMod != NULL)
    {
        currentBtnText = this->selectedMod->text();
        if (currentBtnText.startsWith(this->_disabledPrefix))
            this->selectedMod->setText(currentBtnText.replace(this->_disabledPrefix, prefix));
    }
    this->_disabledPrefix = prefix;
}

void ModsList::updateStyles()
{
    refreshButtonsStyles();

    adjustSize();
    update();
}

void ModsList::setButtonStyle(QPushButton* btn, const bool selected, const bool disabled)
{
    if (btn == NULL)
        return;
    if (selected && disabled)
        btn->setStyleSheet(Assets::Instance()->secondaryBtnStyleSelectedDisabled);
    else if (disabled)
        btn->setStyleSheet(Assets::Instance()->secondaryBtnStyleDisabled);
    else if (selected)
        btn->setStyleSheet(Assets::Instance()->secondaryBtnStyleSelected);
    else
        btn->setStyleSheet(Assets::Instance()->secondaryBtnStyle);
}

void ModsList::refreshButtonsStyles()
{
    int len = _installedMods.count();
    if (len > 0)
        for (int i = 0; i < len; i++)
            if (_installedMods[i] != NULL)
                setButtonStyle(_installedMods[i], (selectedMod == _installedMods[i]), _installedMods[i]->text().startsWith(_disabledPrefix));
}

void ModsList::modButtonClicked()
{
    QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
    if (buttonSender == NULL)
        return;
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug() << "Mod button clicked!" << buttonSender->text();
#endif
    if (selectedMod != buttonSender)
    {
        selectedMod = buttonSender;
        refreshButtonsStyles();
        ((ModsListContainer*)_parent)->updateDisableButtonText();
        ((ModsListContainer*)_parent)->toggleButtons(true);
    }
}

void ModsList::addNewItem(const QString& item, const bool disabled)
{
    QPushButton* toAdd = new QPushButton(item);
    setButtonStyle(toAdd, false, disabled);
    toAdd->setGraphicsEffect(NULL);
    toAdd->setCursor(Qt::PointingHandCursor);
    connect(toAdd, SIGNAL(clicked()), this, SLOT(modButtonClicked()));
    _installedMods.append(toAdd);
    _mainLayout.addWidget(toAdd);
    update();
}

void ModsList::clearItems()
{
    selectedMod = NULL;
    _installedMods.clear();
    QLayoutItem* item;
    while ((item = _mainLayout.takeAt(0)) != NULL)
    {
        delete item->widget();
        delete item;
    }
    update();
}
