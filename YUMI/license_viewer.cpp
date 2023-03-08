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

#include <QApplication>
#include <QCoreApplication>
#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include "license_viewer.h"
#include "common.h"
#include "config.h"
#include "assets.h"

LicenseViewer::LicenseViewer(QWidget* parent) : QWidget(parent)
{
    this->_licenseText = NULL;
    this->_licenseFile = NULL;
    this->_windowMoving = false;

    QString windowTitle(QCoreApplication::translate("LicenseViewer", "YUMI Open Source License", "Window title"));

    this->setWindowTitle(windowTitle);
    this->setWindowIcon(Assets::Instance()->appIcon);
    this->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_NoSystemBackground, true);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setMinimumWidth(LICENSE_VIEWER_WIN_MIN_WIDTH);
    this->setContentsMargins(0, 0, 0, 0);

    this->_title = new WindowTitle(windowTitle, this);

    this->loadLicense();

    this->_licenseScroller = new QScrollArea();
    this->_licenseScroller->setWidgetResizable(true);
    this->_licenseScroller->setAlignment(Qt::AlignTop);
    this->_licenseScroller->setStyleSheet("QScrollArea { margin-top: 5px; margin-left: 10px; margin-right: 10px; " + Assets::Instance()->REGULAR_SCROLL_AREA_STYLE +  " }");
    this->_licenseScroller->setContentsMargins(0, 0, 0, 0);

    this->_licenseLink = new QLabel();
    this->_licenseLink->setText(QCoreApplication::translate("LicenseViewer", "The YUMI Open Source License can also be viewed at:<br><a href=\"%1\">%1</a>", "Label text").arg(YUMI_LICENSE_URL));
    this->_licenseLink->setTextFormat(Qt::RichText);
    this->_licenseLink->setTextInteractionFlags(Qt::TextBrowserInteraction);
    this->_licenseLink->setOpenExternalLinks(true);
    this->_licenseLink->setStyleSheet("QLabel { " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");
    this->_licenseLink->setContentsMargins(10, 10, 10, 10);

    this->_okButton = new QPushButton();
    this->_okButton->setText(QCoreApplication::translate("LicenseViewer", "&OK", "Button text"));
    this->_okButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->_okButton->setCursor(Qt::PointingHandCursor);
    this->_okButton->setContentsMargins(15, 0, 15, 0);
    this->connect(this->_okButton, SIGNAL(clicked()), this, SLOT(licenseAccepted()));

    this->_buttonsLayout = new QVBoxLayout();
    this->_buttonsLayout->addWidget(this->_okButton);
    this->_buttonsLayout->setContentsMargins(10, 5, 10, 10);

    this->_layout.addWidget(this->_title);
    this->_layout.addSpacing(5);
    this->_layout.addWidget(this->_licenseScroller);
    this->_layout.addWidget(this->_licenseLink);
    this->_layout.addLayout(this->_buttonsLayout);
    this->_layout.setContentsMargins(0, 0, 0, 0);
    this->_layout.setSpacing(0);
    this->_layout.setAlignment(Qt::AlignTop);

    this->setStyleSheet("LicenseViewer { " + Assets::Instance()->DEFAULT_WINDOW_STYLE + " }");
    this->setLayout(&this->_layout);
    this->_licenseScroller->setWidget(this->_licenseText);
}

void LicenseViewer::updateStyles()
{
    this->_licenseScroller->setStyleSheet("QScrollArea { margin-top: 5px; margin-left: 10px; margin-right: 10px; " + Assets::Instance()->REGULAR_SCROLL_AREA_STYLE +  " }");
    this->_licenseLink->setStyleSheet("QLabel { " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");
    this->_okButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->setStyleSheet("LicenseViewer { " + Assets::Instance()->DEFAULT_WINDOW_STYLE + " }");
    if (this->_licenseText != NULL)
        this->_licenseText->setStyleSheet("QLabel { margin: 5px; background-color: transparent; " + Assets::Instance()->LICENSE_FONT_STYLE + " }");

    this->_title->updateStyles();

    adjustSize();
    update();
}

void LicenseViewer::doShowAt(const QPoint& center)
{
    this->show();
    this->adjustSize();
    this->move(center.x() - (this->width() / 2), center.y() - (this->height() / 2));
    QApplication::restoreOverrideCursor();
}

void LicenseViewer::licenseAccepted()
{
    Config::Instance()->checkedNoticesAndLicense = true;
    Config::Instance()->configChanged = true;
    close();
}

void LicenseViewer::loadLicense()
{
    if (this->_licenseText != NULL || this->_licenseFile != NULL)
        return;
    this->_licenseFile = new QFile(Assets::Instance()->licenseFile);
    if (!this->_licenseFile->exists())
        qWarning().nospace() << "License file not found.";
    if (!this->_licenseFile->open(QIODevice::ReadOnly | QIODevice::ExistingOnly | QIODevice::Text))
        qWarning().nospace() << "Failed to open license file.";
    QByteArray data;
    try
    {
        data = this->_licenseFile->readAll();
        this->_licenseFile->close();
    }
    catch (const std::exception& ex) { data = QByteArray(); this->_licenseFile->close(); qWarning().nospace() << "Exception caught while reading license file. Exception=[" << ex.what() << "]"; }
    catch (...) { data = QByteArray(); this->_licenseFile->close(); qWarning().nospace() << "Exception caught while reading license file."; }
    QString licenseText = "";
    if (data.length() > 0)
        licenseText = QString::fromUtf8(data);
    if (licenseText.length() <= 0)
        qWarning().nospace() << "License file is empty.";
    this->_licenseText = new QLabel(licenseText);
    this->_licenseText->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->_licenseText->setStyleSheet("QLabel { margin: 5px; background-color: transparent; " + Assets::Instance()->LICENSE_FONT_STYLE + " }");
}

void LicenseViewer::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        _mousePos = event->globalPos() - pos();
        _windowMoving = true;
    }
    QWidget::mousePressEvent(event);
}

void LicenseViewer::mouseMoveEvent(QMouseEvent* event)
{
    if (_windowMoving && (event->buttons() & Qt::LeftButton))
    {
        event->accept();
        move(event->globalPos() - _mousePos);
    }
    else
        QWidget::mouseMoveEvent(event);
}

void LicenseViewer::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        _windowMoving = false;
    QWidget::mouseReleaseEvent(event);
}

void LicenseViewer::paintEvent(QPaintEvent*)
{
#if IS_DEBUG && DEBUG_PAINTING
    qDebug() << "LicenseViewer paint event!";
#endif

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
