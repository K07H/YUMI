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
#include <QEvent>
#include <QGridLayout>
#include <QPainter>
#include <QPainterPath>
#include <QScrollBar>
#include "common.h"
#if IS_DEBUG && DEBUG_PAINTING
#include <QDebug>
#endif
#include "main_widget.h"
#include "yumi.h"
#include "assets.h"

MainWidget::MainWidget(QWidget* parent) : QWidget(parent)
{
    yumiPtr = parent;
    _gameDetailsFirstToggle = true;

    _noGamesInListLbl.setText(QCoreApplication::translate("MainWindow", "You didn't add any game yet.", "Label text"));
    _noGamesInListLbl.setAlignment(Qt::AlignCenter);
    _noGamesInListLbl.setStyleSheet("QLabel { " + Assets::Instance()->ITALIC_LABEL_STYLE + " }");

    _addAGameBtn.setText(QCoreApplication::translate("MainWindow", "&Add a game", "Button text"));
    _addAGameBtn.setStyleSheet(Assets::Instance()->mainBtnStyle);
    _addAGameBtn.setStatusTip(" " + QCoreApplication::translate("MainWindow", "Add a new game to the games list.", "Tooltip text"));
    _addAGameBtn.setCursor(Qt::PointingHandCursor);
    connect(&_addAGameBtn, &QPushButton::released, ((yumi*)yumiPtr), &yumi::gameFolderOpen);
    _addAGameBtnShadow = new QGraphicsDropShadowEffect(&_addAGameBtn);
    _addAGameBtnShadow->setBlurRadius(10);
    _addAGameBtnShadow->setOffset(3, 3);
    _addAGameBtn.setGraphicsEffect(_addAGameBtnShadow);

    _noGamesInListLayout = new QVBoxLayout();
    _noGamesInListLayout->addStretch();
    _noGamesInListLayout->addWidget(&_noGamesInListLbl);
    _noGamesInListLayout->addWidget(&_addAGameBtn);
    _noGamesInListLayout->addStretch();

    sideMenu = new SideMenu(parent, this);
    sideMenu->parentScrollArea = &_scrollArea;

    _scrollArea.setWidgetResizable(true);
    _scrollArea.setStyleSheet("QScrollArea { " + Assets::Instance()->SIDE_MENU_SCROLL_AREA_STYLE + " }");

    _scrollAreaShadow = new QGraphicsDropShadowEffect(&_scrollArea);
    _scrollAreaShadow->setBlurRadius(10);
    _scrollAreaShadow->setOffset(3, 3);
    _scrollArea.setGraphicsEffect(_scrollAreaShadow);

    _gamesListLbl.setParent(&_scrollArea);
    _gamesListLbl.setText(QCoreApplication::translate("MainWindow", "GAMES LIST", "Label text"));
    _gamesListLbl.setAlignment(Qt::AlignCenter);
    _gamesListLbl.setStyleSheet("QLabel { " + Assets::Instance()->SMALLER_TITLE_LABEL_STYLE + " }");

    _gamesListLblShadow = new QGraphicsDropShadowEffect(&_gamesListLbl);
    _gamesListLblShadow->setBlurRadius(10);
    _gamesListLblShadow->setOffset(3, 3);
    _gamesListLbl.setGraphicsEffect(_gamesListLblShadow);

    _sideMenuLayout = new QVBoxLayout();
    _sideMenuLayout->addWidget(&_gamesListLbl);
    _sideMenuLayout->addWidget(&_scrollArea);

    gameDetails = new GameDetails(parent, this);

    _mainLayout.addLayout(_sideMenuLayout);
    _mainLayout.addLayout(_noGamesInListLayout);
    _mainLayout.addWidget(gameDetails);
    setLayout(&_mainLayout);

    _scrollArea.setWidget(sideMenu);
    sideMenu->computeMaxDimensions();

    _gamesListLbl.setVisible(false);
    _scrollArea.setVisible(false);
    gameDetails->setVisible(false);
}

void MainWidget::toggleGameDetails()
{
    bool visible = (((yumi*)yumiPtr)->selectedGame != NULL);
    if (_gameDetailsFirstToggle && visible)
    {
        _gameDetailsFirstToggle = false;
        gameDetails->setHeight();
    }
    gameDetails->setVisible(visible);
}

void MainWidget::toggleSideMenu()
{
    bool visible = (((yumi*)yumiPtr)->gamesInfo.count() > 0);
    _gamesListLbl.setVisible(visible);
    _scrollArea.setVisible(visible);
    _noGamesInListLbl.setVisible(!visible);
    _addAGameBtn.setVisible(!visible);
}

void MainWidget::updateStyles()
{
    _noGamesInListLbl.setStyleSheet("QLabel { " + Assets::Instance()->ITALIC_LABEL_STYLE + " }");
    _addAGameBtn.setStyleSheet(Assets::Instance()->mainBtnStyle);
    _scrollArea.setStyleSheet("QScrollArea { " + Assets::Instance()->SIDE_MENU_SCROLL_AREA_STYLE + " }");
    _gamesListLbl.setStyleSheet("QLabel { " + Assets::Instance()->SMALLER_TITLE_LABEL_STYLE + " }");

    sideMenu->updateStyles();
    gameDetails->updateStyles();

    adjustSize();
    update();
}

void MainWidget::paintEvent(QPaintEvent*)
{
#if IS_DEBUG && DEBUG_PAINTING
    qDebug() << "MainWidget paint event!";
#endif

    QPainter p(this);

    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRect(rect());
    QPen pen(Assets::Instance()->secondaryColor, 2);
    p.setPen(pen);
    p.fillPath(path, Assets::Instance()->primaryColor);
    p.drawPath(path);
}
