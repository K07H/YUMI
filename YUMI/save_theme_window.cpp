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
#include <QStyleOption>
#include <QPainter>
#include <QMessageBox>
#include "save_theme_window.h"
#include "common.h"
#include "assets.h"
#include "yumi.h"

SaveThemeWindow::SaveThemeWindow(void* yumiPtr, QWidget* parent) : QWidget(parent)
{
    this->_yumiPtr = yumiPtr;
    this->_windowMoving = false;

    QString windowTitle(QCoreApplication::translate("SaveThemeWindow", "Save theme", "Window title"));

    this->setWindowTitle(windowTitle);
    this->setWindowIcon(Assets::Instance()->appIcon);
    this->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_NoSystemBackground, true);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setContentsMargins(0, 0, 0, 0);

    this->_title = new WindowTitle(windowTitle, this);

    this->_newThemeLabel = new QLabel(QCoreApplication::translate("SaveThemeWindow", "Theme name:", "Label text"));
    this->_newThemeLabel->setStyleSheet("QLabel { " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");

    this->_newThemeName = new QLineEdit();
    this->_newThemeName->setStyleSheet("QLineEdit { " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");
    this->_newThemeName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    this->_newThemeLayout = new QHBoxLayout();
    this->_newThemeLayout->setContentsMargins(15, 10, 15, 10);
    this->_newThemeLayout->setSpacing(0);
    this->_newThemeLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    this->_newThemeLayout->addWidget(this->_newThemeLabel);
    this->_newThemeLayout->addSpacing(10);
    this->_newThemeLayout->addWidget(this->_newThemeName);

    this->_okButton = new QPushButton();
    this->_okButton->setText(QCoreApplication::translate("SaveThemeWindow", "&Save", "Button text"));
    this->_okButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->_okButton->setCursor(Qt::PointingHandCursor);
    this->_okButton->setContentsMargins(15, 0, 15, 0);
    this->connect(this->_okButton, SIGNAL(clicked()), this, SLOT(confirmSave()));

    this->_cancelButton = new QPushButton();
    this->_cancelButton->setText(QCoreApplication::translate("SaveThemeWindow", "&Cancel", "Button text"));
    this->_cancelButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->_cancelButton->setCursor(Qt::PointingHandCursor);
    this->_cancelButton->setContentsMargins(15, 0, 15, 0);
    this->connect(this->_cancelButton, SIGNAL(clicked()), this, SLOT(cancelSave()));

    this->_buttonsLayout = new QHBoxLayout();
    this->_buttonsLayout->addWidget(this->_okButton);
    this->_buttonsLayout->addSpacing(10);
    this->_buttonsLayout->addWidget(this->_cancelButton);
    this->_buttonsLayout->setContentsMargins(10, 10, 10, 10);

    this->_layout.setContentsMargins(0, 0, 0, 0);
    this->_layout.setSpacing(0);
    this->_layout.setAlignment(Qt::AlignTop);
    this->_layout.addWidget(this->_title);
    this->_layout.addLayout(this->_newThemeLayout);
    this->_layout.addLayout(this->_buttonsLayout);

    this->setStyleSheet("SaveThemeWindow { " + Assets::Instance()->DEFAULT_WINDOW_STYLE + " }");
    this->setLayout(&this->_layout);
}

void SaveThemeWindow::confirmSave()
{
    QString themeName = this->_newThemeName->text();
    if (themeName.isEmpty())
    {
        QMessageBox emptyName = QMessageBox(QMessageBox::Warning, QCoreApplication::translate("SaveThemeWindow", "Empty theme name", "Popup title"), QCoreApplication::translate("SaveThemeWindow", "Please enter a theme name.", "Popup text"), QMessageBox::Ok, this);
        emptyName.exec();
        return;
    }
    this->_newThemeName->setText("");
    ThemeInfo ti = Config::Instance()->getThemePreset(themeName);
    if (themeName.compare(ti.name) == 0)
    {
        QMessageBox nameExist = QMessageBox(QMessageBox::Warning, QCoreApplication::translate("SaveThemeWindow", "Theme name exist", "Popup title"), QCoreApplication::translate("SaveThemeWindow", "This theme name already exists. Please choose another name for your theme.", "Popup text"), QMessageBox::Ok, this);
        nameExist.exec();
        return;
    }
    ((yumi*)_yumiPtr)->getSettingsWidget()->saveConfirmed(themeName);
    close();
}

void SaveThemeWindow::cancelSave()
{
    close();
}

void SaveThemeWindow::doShowAt(const QPoint& center)
{
    this->show();
    this->adjustSize();
    this->move(center.x() - (this->width() / 2), center.y() - (this->height() / 2));
}

void SaveThemeWindow::updateStyles()
{
    this->_newThemeLabel->setStyleSheet("QLabel { " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");
    this->_newThemeName->setStyleSheet("QLineEdit { " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");
    this->_okButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->_cancelButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->setStyleSheet("SaveThemeWindow { " + Assets::Instance()->DEFAULT_WINDOW_STYLE + " }");

    adjustSize();
    update();
}

void SaveThemeWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        _mousePos = event->globalPos() - pos();
        _windowMoving = true;
    }
    QWidget::mousePressEvent(event);
}

void SaveThemeWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (_windowMoving && (event->buttons() & Qt::LeftButton))
    {
        event->accept();
        move(event->globalPos() - _mousePos);
    }
    else
        QWidget::mouseMoveEvent(event);
}

void SaveThemeWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        _windowMoving = false;
    QWidget::mouseReleaseEvent(event);
}

void SaveThemeWindow::paintEvent(QPaintEvent*)
{
#if IS_DEBUG && DEBUG_PAINTING
    qDebug() << "UninstallWindow paint event!";
#endif

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
