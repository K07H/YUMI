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
#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <QScrollBar>
#include <QStatusBar>
#include "common.h"
#if IS_DEBUG
#include <QDebug>
#endif
#include "side_menu.h"
#include "yumi.h"
#include "assets.h"

SideMenu::SideMenu(void* yumiPtr, QWidget* parent) : QWidget(parent)
{
    this->_yumiPtr = yumiPtr;
    this->parentScrollArea = NULL;
    this->_selectedAction = NULL;
    this->_hoveringAction = NULL;
    this->_pointerEnabled = false;
    this->_maxTextWidth = 120;
    this->_maxTextHeight = 30;
    this->_buttonsFont = new QFont();
    this->_buttonsFont->setFamily(Assets::Instance()->PRIMARY_FONT_FAMILY);
    this->_buttonsFont->setPixelSize(Assets::Instance()->DEFAULT_FONT_SIZE_PX.toInt());
    this->_buttonsFont->setStyleHint(QFont::StyleHint::SansSerif);
    this->setMouseTracking(true);
    this->installEventFilter(this);

    this->setStyleSheet("SideMenu { background-color: transparent; }");
}

void SideMenu::updateStyles()
{
    this->_buttonsFont->setFamily(Assets::Instance()->PRIMARY_FONT_FAMILY);
    this->_buttonsFont->setPixelSize(Assets::Instance()->DEFAULT_FONT_SIZE_PX.toInt());
    this->computeMaxDimensions();

    adjustSize();
    update();
}

bool SideMenu::removeSideMenuElem(const QString& gameName, const QString& gamePath)
{
    int toRemove = -1;
    int len = this->_actions.count();
    if (len > 0)
        for (int i = 0; i < len; i++)
            if (gameName.compare(this->_actions[i].action->text()) == 0 && gamePath.compare(this->_actions[i].gamePath) == 0)
            {
                toRemove = i;
                break;
            }
    if (toRemove >= 0)
    {
        this->_actions.removeAt(toRemove);
        return true;
    }
    return false;
}

bool SideMenu::eventFilter(QObject*, QEvent* event)
{
    if (this->_pointerEnabled && event->type() == QEvent::Leave)
    {
        this->setCursor(Qt::ArrowCursor);
        this->_pointerEnabled = false;
    }
    return false;
}

void SideMenu::computeMaxDimensions()
{
    this->_maxTextWidth = 120;
    this->_maxTextHeight = 30;
    if (parentScrollArea == NULL)
        return;
    int len = this->_actions.count();
    if (len > 0)
    {
        QFontMetrics fontMetric(*_buttonsFont, this);
        for (int i = 0; i < len; i++)
            if (this->_actions[i].action != NULL)
            {
                QSize textSize = fontMetric.size(Qt::TextSingleLine, this->_actions[i].action->text());
                int textWidth = textSize.width() + SideMenu::TEXT_MARGIN;
                int textHeight = textSize.height();
                if (this->_maxTextWidth < textWidth)
                    this->_maxTextWidth = textWidth;
                if (this->_maxTextHeight < textHeight)
                    this->_maxTextHeight = textHeight;
            }
    }
    int scrollViewWidth = this->_maxTextWidth + SideMenu::SCROLLVIEW_INNER_MARGIN;
    int finalScrollViewWidth = scrollViewWidth > SideMenu::MAX_WIDTH ? SideMenu::MAX_WIDTH : scrollViewWidth;
    bool elemsHeightBiggerThanScrollHeight = (len * this->_maxTextHeight) > (((yumi*)_yumiPtr)->size().height() - 120);
    parentScrollArea->setFixedWidth(finalScrollViewWidth + ((elemsHeightBiggerThanScrollHeight || parentScrollArea->verticalScrollBar()->isVisible()) ? SideMenu::SCROLLVIEW_MARGIN : 0));
}

void SideMenu::paintEvent(QPaintEvent*)
{
#if IS_DEBUG && DEBUG_PAINTING
    qDebug() << "SideMenu paint event!";
#endif

    if (parentScrollArea == NULL)
        return;

    QPainter p(this);

    p.setBackground(Qt::transparent);
    p.setBackgroundMode(Qt::TransparentMode);
    p.setRenderHint(QPainter::Antialiasing);

    p.setFont(*_buttonsFont);

    QPainterPath path;
    path.addRoundedRect(rect(), 10, 10);
    QPen pen(Assets::Instance()->secondaryColor, 2);
    p.setPen(pen);
    p.fillPath(path, Assets::Instance()->secondaryColorLight);
    p.drawPath(path);

    int currentHeight = 0;
    int len = this->_actions.count();
    if (len > 0)
        for (int i = 0; i < len; i++)
            if (this->_actions[i].action != NULL)
            {
                QRect actionRect(0, currentHeight, this->_maxTextWidth, this->_maxTextHeight);
                QPoint middlePoint = QPoint(actionRect.width() / 2 - this->_maxTextWidth / 2, actionRect.bottom() - this->_maxTextHeight);
                QRect actionTextRect(middlePoint, QSize(this->_maxTextWidth, this->_maxTextHeight));
                QPainterPath pathB;
                pathB.addRoundedRect(actionRect, 10, 10);
                if (this->_actions[i].action->isChecked())
                    p.fillPath(pathB, Assets::Instance()->secondaryColor);
                if (this->_actions[i].action == this->_hoveringAction)
                    p.fillPath(pathB, Assets::Instance()->secondaryColorDeep);
                p.setPen(Assets::Instance()->lightTextColor);
                p.drawText(actionTextRect, Qt::AlignCenter, this->_actions[i].action->text());

                currentHeight += actionRect.height();
            }
}

void SideMenu::updateSelectedButton(const SideMenuElem& menuElem)
{
    if (this->_selectedAction)
        this->_selectedAction->setChecked(false);
    this->_selectedAction = menuElem.action;
    menuElem.action->setChecked(true);
    GameInfo* selectedGame = ((yumi*)this->_yumiPtr)->getGameInfo(menuElem.action->text(), menuElem.gamePath);
    ((yumi*)_yumiPtr)->setSelectedGame(selectedGame);
    if (this->_hoveringAction == menuElem.action)
        this->_hoveringAction = NULL;
    ((yumi*)_yumiPtr)->mainWidget->update();
    update();
}

void SideMenu::setSelectedButton(const QString& gamePath)
{
    SideMenuElem menuElem = getActionFromGamePath(gamePath);
    if (menuElem.action == NULL)
        return;
    updateSelectedButton(menuElem);
}

void SideMenu::mousePressEvent(QMouseEvent* event)
{
    SideMenuElem currentElem = getActionAt(event->pos());
    if (currentElem.action == NULL || currentElem.action->isChecked())
        return;
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Game button " << currentElem.action->text() << " clicked!";
#endif
    updateSelectedButton(currentElem);
    QWidget::mousePressEvent(event);
}

void SideMenu::mouseMoveEvent(QMouseEvent* event)
{
    SideMenuElem currentElem = getActionAt(event->pos());
    if (currentElem.action == NULL)
    {
        this->_hoveringAction = NULL;
        if (this->_pointerEnabled)
        {
            this->setCursor(Qt::ArrowCursor);
            this->_pointerEnabled = false;
        }
        update();
        return;
    }
    bool stillHoveringSameBtn = this->_hoveringAction == currentElem.action;
    if (currentElem.action->isChecked() || stillHoveringSameBtn)
    {
        if (!stillHoveringSameBtn)
        {
            this->_hoveringAction = NULL;
            update();
        }
        if (!this->_pointerEnabled)
        {
            this->setCursor(Qt::PointingHandCursor);
            this->_pointerEnabled = true;
        }
        return;
    }
    this->_hoveringAction = currentElem.action;
    if (!this->_pointerEnabled)
    {
        this->setCursor(Qt::PointingHandCursor);
        this->_pointerEnabled = true;
    }
    update();
    QWidget::mouseMoveEvent(event);
}

void SideMenu::leaveEvent(QEvent* event)
{
    this->_hoveringAction = NULL;
    update();
    QWidget::leaveEvent(event);
}

SideMenuElem SideMenu::getActionFromGamePath(const QString& gamePath)
{
    int len = this->_actions.count();
    if (len > 0 && !gamePath.isEmpty())
        for (int i = 0; i < len; i++)
            if (gamePath.compare(this->_actions[i].gamePath) == 0)
                return this->_actions[i];

    SideMenuElem empty;
    empty.action = NULL;
    empty.gamePath = QString("");
    return empty;
}

SideMenuElem SideMenu::getActionAt(const QPoint& at)
{
    int currentHeight = 0;
    int len = this->_actions.count();
    if (len > 0)
        for (int i = 0; i < len; i++)
            if (this->_actions[i].action != NULL)
            {
                QRect actionRect(0, currentHeight, this->_maxTextWidth, this->_maxTextHeight);
                if (actionRect.contains(at))
                    return this->_actions[i];
                currentHeight += this->_maxTextHeight;
            }

    SideMenuElem empty;
    empty.action = NULL;
    empty.gamePath = QString("");
    return empty;
}

QSize SideMenu::minimumSizeHint() const
{
    return QSize(this->_maxTextWidth, this->_maxTextHeight * this->_actions.size());
}

void SideMenu::insertActionSorted(QAction* action, const QString& gamePath)
{
    qsizetype atPos = 0;
    QString actionText = action->text().toLower();
    int len = this->_actions.count();
    if (len > 0)
        for (int i = 0; i < len; i++)
        {
            if (this->_actions[i].action != NULL && actionText.compare(this->_actions[i].action->text().toLower(), Qt::CaseSensitive) < 0)
                break;
            else
                atPos++;
        }

    SideMenuElem toAdd;
    toAdd.action = action;
    toAdd.gamePath = gamePath;
    qDebug() << "Inserting action" << actionText << "at" << atPos << ".";
    this->_actions.insert(atPos, toAdd);
    computeMaxDimensions();
    ((yumi*)_yumiPtr)->mainWidget->toggleSideMenu();

}

void SideMenu::addAction(QAction* action, const QString& gamePath)
{
    if (action == NULL || action->text().isEmpty() || gamePath.isEmpty())
        return;

    action->setCheckable(true);
    insertActionSorted(action, gamePath);
    update();
}

QAction* SideMenu::addAction(const QString& gameName, const QString& gamePath)
{
    if (gameName.isEmpty() || gamePath.isEmpty())
        return NULL;

    QAction* action = new QAction(gameName, this);
    action->setCheckable(true);
    insertActionSorted(action, gamePath);
    update();
    return action;
}

QAction* SideMenu::addAction(const GameInfo& gameInfo)
{
    if (gameInfo.name.isEmpty() || gameInfo.path.isEmpty())
        return NULL;

    QAction* action = new QAction(gameInfo.name, this);
    action->setCheckable(true);
    insertActionSorted(action, gameInfo.path);
    update();
    return action;
}
