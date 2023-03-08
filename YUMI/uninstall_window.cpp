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
#include "uninstall_window.h"
#include "common.h"
#include "assets.h"
#include "mods_loader.h"

UninstallWindow::UninstallWindow(void* yumiPtr, GameInfo* game, void* gameDetails, QWidget* parent) : QWidget(parent)
{
    this->_yumiPtr = yumiPtr;
    this->_gameDetails = gameDetails;
    this->_game = game;
    this->_windowMoving = false;

    QString windowTitle(QCoreApplication::translate("UninstallWindow", "Uninstall mods loader", "Window title"));

    this->setWindowTitle(windowTitle);
    this->setWindowIcon(Assets::Instance()->appIcon);
    this->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_NoSystemBackground, true);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setContentsMargins(0, 0, 0, 0);

    this->_title = new WindowTitle(windowTitle, this);

    this->_includeBepInExConfigFile = new QCheckBox(QCoreApplication::translate("UninstallWindow", "Also remove mods loader config file (recommended).", "CheckBox label"));
    this->_includeBepInExConfigFile->setStyleSheet(Assets::Instance()->checkboxStyle);
    this->_includeBepInExConfigFile->setCursor(Qt::PointingHandCursor);
    this->_includeBepInExConfigFile->setChecked(true);

    this->_includeAllModsAndSavedData = new QCheckBox(QCoreApplication::translate("UninstallWindow", "Also remove all mods and their data (not recommended).", "CheckBox label"));
    this->_includeAllModsAndSavedData->setStyleSheet(Assets::Instance()->checkboxStyle);
    this->_includeAllModsAndSavedData->setCursor(Qt::PointingHandCursor);
    this->_includeAllModsAndSavedData->setChecked(false);

    this->_okButton = new QPushButton();
    this->_okButton->setText(QCoreApplication::translate("UninstallWindow", "&Uninstall", "Button text"));
    this->_okButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->_okButton->setCursor(Qt::PointingHandCursor);
    this->_okButton->setContentsMargins(15, 0, 15, 0);
    this->connect(this->_okButton, SIGNAL(clicked()), this, SLOT(doUninstall()));

    this->_cancelButton = new QPushButton();
    this->_cancelButton->setText(QCoreApplication::translate("UninstallWindow", "&Cancel", "Button text"));
    this->_cancelButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->_cancelButton->setCursor(Qt::PointingHandCursor);
    this->_cancelButton->setContentsMargins(15, 0, 15, 0);
    this->connect(this->_cancelButton, SIGNAL(clicked()), this, SLOT(close()));

    this->_buttonsLayout = new QHBoxLayout();
    this->_buttonsLayout->addWidget(this->_okButton);
    this->_buttonsLayout->addSpacing(10);
    this->_buttonsLayout->addWidget(this->_cancelButton);
    this->_buttonsLayout->setContentsMargins(10, 5, 10, 10);

    this->_layout.setContentsMargins(0, 0, 0, 0);
    this->_layout.setSpacing(0);
    this->_layout.setAlignment(Qt::AlignTop);
    this->_layout.addWidget(this->_title);
    this->_layout.addSpacing(10);
    this->_layout.addWidget(this->_includeBepInExConfigFile);
    this->_layout.addSpacing(5);
    this->_layout.addWidget(this->_includeAllModsAndSavedData);
    this->_layout.addSpacing(5);
    this->_layout.addLayout(this->_buttonsLayout);

    this->setStyleSheet("UninstallWindow { " + Assets::Instance()->DEFAULT_WINDOW_STYLE + " }");
    this->setLayout(&this->_layout);
}

void UninstallWindow::updateStyles()
{
    this->_includeBepInExConfigFile->setStyleSheet(Assets::Instance()->checkboxStyle);
    this->_includeAllModsAndSavedData->setStyleSheet(Assets::Instance()->checkboxStyle);
    this->_okButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->_cancelButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->setStyleSheet("UninstallWindow { " + Assets::Instance()->DEFAULT_WINDOW_STYLE + " }");

    this->_title->updateStyles();

    adjustSize();
    update();
}

void UninstallWindow::doUninstall()
{
    bool includingBepInExConfigFile = this->_includeBepInExConfigFile->isChecked();
    bool includingAllModsAndSavedData = this->_includeAllModsAndSavedData->isChecked();
    if (includingAllModsAndSavedData)
    {
        QMessageBox confirmBox(QMessageBox::Warning, QCoreApplication::translate("UninstallWindow", "Confirm uninstall", "Popup title"), QCoreApplication::translate("UninstallWindow", "Are you sure that you also want to remove all mods and their data? This will remove \"config\", \"patchers\" and \"plugins\" folders located in \"%1\".", "Popup text").arg(this->_game->path), QMessageBox::Yes | QMessageBox::No, this);
        int clickedBtn = confirmBox.exec();
        if (clickedBtn != QMessageBox::Yes)
            return;
    }
    try
    {
        qInfo().nospace() << "Uninstalling BepInEx (Including config file: " << (includingBepInExConfigFile ? "true" : "false") << ". Including mods and data: " << (includingAllModsAndSavedData ? "true" : "false") << ")...";
        ModsLoader::Instance()->uninstallBepInEx(this->_yumiPtr, this->_gameDetails, this->_game, includingBepInExConfigFile, includingAllModsAndSavedData);
    }
    catch (const std::exception& ex)
    {
        qCritical().nospace() << "BepInEx uninstallation error: An exception got caught while uninstalling mods loader for " << this->_game->name << " (Exception: " << ex.what() << ").";
        QMessageBox confirmBox(QMessageBox::Critical, QCoreApplication::translate("UninstallWindow", "Uninstall failed", "Popup title"), QCoreApplication::translate("UninstallWindow", "An error happened while uninstalling mods loader for \"%1\" (Error message: %2).", "Popup text").arg(this->_game->name, ex.what()), QMessageBox::Ok, this);
        confirmBox.exec();
    }
    catch (...)
    {
        qCritical().nospace() << "BepInEx uninstallation error: An exception got caught while uninstalling mods loader for " << this->_game->name << ".";
        QMessageBox confirmBox(QMessageBox::Critical, QCoreApplication::translate("UninstallWindow", "Uninstall failed", "Popup title"), QCoreApplication::translate("UninstallWindow", "An error happened while uninstalling mods loader for \"%1\".", "Popup text").arg(this->_game->name), QMessageBox::Ok, this);
        confirmBox.exec();
    }
    close();
}

void UninstallWindow::doShowAt(const QPoint& center)
{
    this->show();
    this->adjustSize();
    this->move(center.x() - (this->width() / 2), center.y() - (this->height() / 2));
}

void UninstallWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        _mousePos = event->globalPos() - pos();
        _windowMoving = true;
    }
    QWidget::mousePressEvent(event);
}

void UninstallWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (_windowMoving && (event->buttons() & Qt::LeftButton))
    {
        event->accept();
        move(event->globalPos() - _mousePos);
    }
    else
        QWidget::mouseMoveEvent(event);
}

void UninstallWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        _windowMoving = false;
    QWidget::mouseReleaseEvent(event);
}

void UninstallWindow::paintEvent(QPaintEvent*)
{
#if IS_DEBUG && DEBUG_PAINTING
    qDebug() << "UninstallWindow paint event!";
#endif

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
