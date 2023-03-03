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
#include <QStyleOption>
#include <QPainter>
#include <QMessageBox>
#include "select_game_window.h"
#include "assets.h"
#include "common.h"
#include "yumi.h"

SelectGameWindow::SelectGameWindow(void* yumiPtr, const QString& modFileName, QWidget* parent) : QWidget(parent)
{
    this->_yumiPtr = yumiPtr;
    this->_modFileName = modFileName;
    this->_windowMoving = false;
    this->selectedGame = "";

    QString windowTitle(QCoreApplication::translate("SelectGameWindow", "Select game for mod installation", "Window title"));

    this->setWindowTitle(windowTitle);
    this->setWindowIcon(Assets::Instance()->appIcon);
    this->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_NoSystemBackground, true);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setContentsMargins(0, 0, 0, 0);

    this->_title = new WindowTitle(windowTitle, this);

    this->_explanation = new QLabel(QCoreApplication::translate("SelectGameWindow", "Please select for which game the mod \"%1\" is made then click on \"OK\", or click on \"Skip\" to skip installation of this mod.", "Label text").arg(this->_modFileName));
    this->_explanation->setWordWrap(true);
    this->_explanation->setStyleSheet("QLabel { margin-left: 10px; margin-right: 10px; " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");
    this->_explanation->setMinimumWidth(420);

    this->_gameComboBox = new QComboBox();
    QStringList allGames;
    allGames.push_back("");
    int nbGames = ((yumi*)yumiPtr)->gamesInfo.count();
    for (int i = 0; i < nbGames; i++)
        allGames.push_back((((yumi*)yumiPtr)->gamesInfo)[i].name);
    this->_gameComboBox->addItems(allGames);
    this->_gameComboBox->setCurrentText("");
    this->_gameComboBox->setCursor(Qt::PointingHandCursor);
    this->_gameComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    this->_gameComboBox->setStyleSheet("QComboBox { " + Assets::Instance()->COMBOBOX_LABEL_STYLE + " }");

    this->_comboBoxLayout = new QHBoxLayout();
    this->_comboBoxLayout->setContentsMargins(10, 5, 10, 0);
    this->_comboBoxLayout->addWidget(this->_gameComboBox);

    this->_okButton = new QPushButton();
    this->_okButton->setText(QCoreApplication::translate("SelectGameWindow", "&OK", "Button text"));
    this->_okButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->_okButton->setCursor(Qt::PointingHandCursor);
    this->_okButton->setContentsMargins(15, 0, 15, 0);
    this->connect(this->_okButton, SIGNAL(clicked()), this, SLOT(doSelect()));

    this->_cancelButton = new QPushButton();
    this->_cancelButton->setText(QCoreApplication::translate("SelectGameWindow", "&Skip", "Button text"));
    this->_cancelButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->_cancelButton->setCursor(Qt::PointingHandCursor);
    this->_cancelButton->setContentsMargins(15, 0, 15, 0);
    this->connect(this->_cancelButton, SIGNAL(clicked()), this, SLOT(doCancel()));

    this->_buttonsLayout = new QHBoxLayout();
    this->_buttonsLayout->addWidget(this->_okButton);
    this->_buttonsLayout->addSpacing(10);
    this->_buttonsLayout->addWidget(this->_cancelButton);
    this->_buttonsLayout->setContentsMargins(10, 15, 10, 10);

    this->_layout.setContentsMargins(0, 0, 0, 0);
    this->_layout.setSpacing(0);
    this->_layout.addWidget(this->_title, Qt::AlignTop);
    this->_layout.addSpacing(10);
    this->_layout.addWidget(this->_explanation, Qt::AlignTop);
    this->_layout.addLayout(this->_comboBoxLayout, Qt::AlignVCenter);
    this->_layout.addLayout(this->_buttonsLayout, Qt::AlignBottom);

    this->setStyleSheet("SelectGameWindow { " + Assets::Instance()->DEFAULT_WINDOW_STYLE + " }");
    this->setLayout(&this->_layout);
}

void SelectGameWindow::doSelect()
{
    QString selection = this->_gameComboBox->currentText();
    if (selection.isEmpty())
    {
        QMessageBox confirmCancel = QMessageBox(QMessageBox::Warning, QCoreApplication::translate("SelectGameWindow", "No valid game selected", "Popup title"), QCoreApplication::translate("SelectGameWindow", "You did not select a valid game from the list, installation of \"%1\" mod will be canceled. Are you sure that you want to continue?", "Popup text").arg(this->_modFileName), QMessageBox::Yes | QMessageBox::No, this);
        int clickedBtn = confirmCancel.exec();
        if (clickedBtn == QMessageBox::Yes)
        {
            this->selectedGame = "";
            emit doClose();
            close();
        }
    }
    else
    {
        this->selectedGame = selection;
        emit doClose();
        close();
    }
}

void SelectGameWindow::doCancel()
{
    this->selectedGame = "";
    emit doClose();
    close();
}

void SelectGameWindow::updateStyles()
{
    this->_explanation->setStyleSheet("QLabel { margin-left: 10px; margin-right: 10px; " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");
    this->_gameComboBox->setStyleSheet("QComboBox { margin-left: 10px; margin-right: 10px; " + Assets::Instance()->COMBOBOX_LABEL_STYLE + " }");
    this->_okButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->_cancelButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->setStyleSheet("SelectGameWindow { " + Assets::Instance()->DEFAULT_WINDOW_STYLE + " }");

    this->_title->updateStyles();

    adjustSize();
    update();
}

void SelectGameWindow::doShowAt(const QPoint& center)
{
    this->show();
    this->adjustSize();
    this->move(center.x() - (this->width() / 2), center.y() - (this->height() / 2));
}

void SelectGameWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        _mousePos = event->globalPos() - pos();
        _windowMoving = true;
    }
    QWidget::mousePressEvent(event);
}

void SelectGameWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (_windowMoving && (event->buttons() & Qt::LeftButton))
    {
        event->accept();
        move(event->globalPos() - _mousePos);
    }
    else
        QWidget::mouseMoveEvent(event);
}

void SelectGameWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        _windowMoving = false;
    QWidget::mouseReleaseEvent(event);
}

void SelectGameWindow::paintEvent(QPaintEvent*)
{
#if IS_DEBUG && DEBUG_PAINTING
    qDebug() << "SelectGameWindow paint event!";
#endif

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
