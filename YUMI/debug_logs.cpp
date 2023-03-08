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
#include <QApplication>
#include <QDesktopServices>
#include <QClipboard>
#include <QStyleOption>
#include <QPainter>
#include <QMessageBox>
#ifndef Q_OS_WINDOWS
#include <QThread>
#endif
#include "debug_logs.h"
#include "assets.h"
#include "debug_info.h"
#include "yumi.h"
#include "utils.h"

DebugLogs::DebugLogs(void* yumiPtr, QWidget* parent) : QWidget(parent)
{
    this->_yumiPtr = yumiPtr;
    this->_windowMoving = false;

    QString windowTitle(QCoreApplication::translate("DebugLogs", "Debug Logs", "Window title"));

    this->setWindowTitle(windowTitle);
    this->setWindowIcon(Assets::Instance()->appIcon);
    this->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_NoSystemBackground, true);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setContentsMargins(0, 0, 0, 0);

    this->_title = new WindowTitle(windowTitle, this);

    this->_yumiLogsTitle = new QLabel(QCoreApplication::translate("DebugLogs", "YUMI logs:", "Label text"));
    this->_yumiLogsTitle->setStyleSheet("QLabel { margin-left: 5px; " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");

    this->_yumiLogsScroller = new QScrollArea();
    this->_yumiLogsScroller->setWidgetResizable(false);
    this->_yumiLogsScroller->setFixedWidth(765);
    this->_yumiLogsScroller->setFixedHeight(200);
    this->_yumiLogsScroller->setAlignment(Qt::AlignTop);
    this->_yumiLogsScroller->setContentsMargins(0, 0, 0, 0);
    this->_yumiLogsScroller->setStyleSheet("QScrollArea { margin-left: 10px; margin-right: 10px; padding-left: 5px; " + Assets::Instance()->REGULAR_SCROLL_AREA_STYLE +  " }");

    DebugInfo::Init((yumi*)_yumiPtr);
    this->_yumiLogsLabel = new QLabel(this->_yumiLogsScroller);
    this->_yumiLogsLabel->setFixedWidth(700);
    this->_yumiLogsLabel->setText(DebugInfo::Instance()->GetYUMILogs(false));
    this->_yumiLogsLabel->setWordWrap(true);
    this->_yumiLogsLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->_yumiLogsLabel->setContentsMargins(0, 0, 0, 0);
    this->_yumiLogsLabel->setStyleSheet("QLabel { background-color: transparent; " + Assets::Instance()->LICENSE_FONT_STYLE + " }");

#if DISPLAY_FILE_TREES
    this->_yumiFileTreeTitle = new QLabel(QCoreApplication::translate("DebugLogs", "YUMI files tree:", "Label text"));
    this->_yumiFileTreeTitle->setStyleSheet("QLabel { margin-left: 5px; " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");

    this->_yumiFileTreeScroller = new QScrollArea();
    this->_yumiFileTreeScroller->setWidgetResizable(false);
    this->_yumiFileTreeScroller->setFixedWidth(765);
    this->_yumiFileTreeScroller->setFixedHeight(200);
    this->_yumiFileTreeScroller->setAlignment(Qt::AlignTop);
    this->_yumiFileTreeScroller->setContentsMargins(0, 0, 0, 0);
    this->_yumiFileTreeScroller->setStyleSheet("QScrollArea { margin-left: 10px; margin-right: 10px; padding-left: 5px; " + Assets::Instance()->REGULAR_SCROLL_AREA_STYLE +  " }");

    DebugInfo::Init((yumi*)_yumiPtr);
    this->_yumiFileTreeLabel = new QLabel(this->_yumiLogsScroller);
    this->_yumiFileTreeLabel->setFixedWidth(700);
    this->_yumiFileTreeLabel->setText(DebugInfo::Instance()->GetYUMIFileTree(false));
    this->_yumiFileTreeLabel->setWordWrap(true);
    this->_yumiFileTreeLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->_yumiFileTreeLabel->setContentsMargins(0, 0, 0, 0);
    this->_yumiFileTreeLabel->setStyleSheet("QLabel { background-color: transparent; " + Assets::Instance()->LICENSE_FONT_STYLE + " }");
#endif

    this->_selectGameLabel = new QLabel(QCoreApplication::translate("DebugLogs", "Select game:", "Label text"));
    this->_selectGameLabel->setStyleSheet("QLabel { margin-left: 5px; " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");

    this->_selectGameDropdown = new QComboBox();
    QStringList allGames;
    allGames.push_back("");
    int nbGames = ((yumi*)yumiPtr)->gamesInfo.count();
    for (int i = 0; i < nbGames; i++)
        allGames.push_back((((yumi*)yumiPtr)->gamesInfo)[i].name);
    this->_selectGameDropdown->addItems(allGames);
    this->_selectGameDropdown->setCurrentText("");
    this->_selectGameDropdown->setCursor(Qt::PointingHandCursor);
    this->_selectGameDropdown->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    this->_selectGameDropdown->setStyleSheet("QComboBox { " + Assets::Instance()->COMBOBOX_LABEL_STYLE + " }");
    connect(this->_selectGameDropdown, SIGNAL(currentTextChanged(QString)), this, SLOT(selectedGameChanged(QString)));

    this->_selectedGameLayout = new QHBoxLayout();
    this->_selectedGameLayout->setContentsMargins(0, 0, 0, 0);
    this->_selectedGameLayout->setSpacing(0);
    this->_selectedGameLayout->addWidget(this->_selectGameLabel);
    this->_selectedGameLayout->addSpacing(5);
    this->_selectedGameLayout->addWidget(this->_selectGameDropdown);
    this->_selectedGameLayout->addSpacing(10);

    this->_gameLogsTitle = new QLabel(QCoreApplication::translate("DebugLogs", "Game logs:", "Label text"));
    this->_gameLogsTitle->setStyleSheet("QLabel { margin-left: 5px; " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");

    this->_gameLogsScroller = new QScrollArea();
    this->_gameLogsScroller->setWidgetResizable(false);
    this->_gameLogsScroller->setFixedWidth(765);
    this->_gameLogsScroller->setFixedHeight(200);
    this->_gameLogsScroller->setAlignment(Qt::AlignTop);
    this->_gameLogsScroller->setStyleSheet("QScrollArea { margin-left: 10px; margin-right: 10px; padding-left: 5px; " + Assets::Instance()->REGULAR_SCROLL_AREA_STYLE +  " }");
    this->_gameLogsScroller->setContentsMargins(0, 0, 0, 0);

    this->_gameLogsLabel = new QLabel(this->_gameLogsScroller);
    this->_gameLogsLabel->setFixedWidth(700);
    this->_gameLogsLabel->setText(DebugInfo::Instance()->GetGameLogs(NULL, false));
    this->_gameLogsLabel->setWordWrap(true);
    this->_gameLogsLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->_gameLogsLabel->setContentsMargins(0, 0, 0, 0);
    this->_gameLogsLabel->setStyleSheet("QLabel { background-color: transparent; " + Assets::Instance()->LICENSE_FONT_STYLE + " }");

#if DISPLAY_FILE_TREES
    this->_gameFileTreeTitle = new QLabel(QCoreApplication::translate("DebugLogs", "Game files tree:", "Label text"));
    this->_gameFileTreeTitle->setStyleSheet("QLabel { margin-left: 5px; " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");

    this->_gameFileTreeScroller = new QScrollArea();
    this->_gameFileTreeScroller->setWidgetResizable(false);
    this->_gameFileTreeScroller->setFixedWidth(765);
    this->_gameFileTreeScroller->setFixedHeight(200);
    this->_gameFileTreeScroller->setAlignment(Qt::AlignTop);
    this->_gameFileTreeScroller->setStyleSheet("QScrollArea { margin-left: 10px; margin-right: 10px; padding-left: 5px; " + Assets::Instance()->REGULAR_SCROLL_AREA_STYLE +  " }");
    this->_gameFileTreeScroller->setContentsMargins(0, 0, 0, 0);

    this->_gameFileTreeLabel = new QLabel(this->_gameFileTreeScroller);
    this->_gameFileTreeLabel->setFixedWidth(700);
    this->_gameFileTreeLabel->setText(DebugInfo::Instance()->GetGameFileTree(NULL, false));
    this->_gameFileTreeLabel->setWordWrap(true);
    this->_gameFileTreeLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->_gameFileTreeLabel->setContentsMargins(0, 0, 0, 0);
    this->_gameFileTreeLabel->setStyleSheet("QLabel { background-color: transparent; " + Assets::Instance()->LICENSE_FONT_STYLE + " }");
#endif

    this->_copyToClipboardButton = new QPushButton();
    this->_copyToClipboardButton->setText(QCoreApplication::translate("DebugLogs", "C&opy to clipboard", "Button text"));
    this->_copyToClipboardButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->_copyToClipboardButton->setCursor(Qt::PointingHandCursor);
    this->_copyToClipboardButton->setContentsMargins(15, 0, 15, 0);
    this->connect(this->_copyToClipboardButton, SIGNAL(clicked()), this, SLOT(copyLogsToClipboard()));

    this->_exportButton = new QPushButton();
    this->_exportButton->setText(QCoreApplication::translate("DebugLogs", "&Export to file", "Button text"));
    this->_exportButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->_exportButton->setCursor(Qt::PointingHandCursor);
    this->_exportButton->setContentsMargins(15, 0, 15, 0);
    this->connect(this->_exportButton, SIGNAL(clicked()), this, SLOT(exportLogs()));

    this->_closeButton = new QPushButton();
    this->_closeButton->setText(QCoreApplication::translate("DebugLogs", "&Close", "Button text"));
    this->_closeButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->_closeButton->setCursor(Qt::PointingHandCursor);
    this->_closeButton->setContentsMargins(15, 0, 15, 0);
    this->connect(this->_closeButton, SIGNAL(clicked()), this, SLOT(close()));

    this->_buttonsLayout = new QHBoxLayout();
    this->_buttonsLayout->setContentsMargins(10, 5, 10, 10);
    this->_buttonsLayout->addWidget(this->_copyToClipboardButton);
    this->_buttonsLayout->addSpacing(10);
    this->_buttonsLayout->addWidget(this->_exportButton);
    this->_buttonsLayout->addSpacing(10);
    this->_buttonsLayout->addWidget(this->_closeButton);

    this->_layout.setContentsMargins(0, 0, 0, 0);
    this->_layout.setSpacing(0);
    this->_layout.setAlignment(Qt::AlignTop);
    this->_layout.addWidget(this->_title);
    this->_layout.addSpacing(5);
    this->_layout.addWidget(this->_yumiLogsTitle);
    this->_layout.addSpacing(2);
    this->_layout.addWidget(this->_yumiLogsScroller);
#if DISPLAY_FILE_TREES
    this->_layout.addSpacing(5);
    this->_layout.addWidget(this->_yumiFileTreeTitle);
    this->_layout.addSpacing(2);
    this->_layout.addWidget(this->_yumiFileTreeScroller);
#endif
    this->_layout.addSpacing(20);
    this->_layout.addLayout(this->_selectedGameLayout);
    this->_layout.addSpacing(5);
    this->_layout.addWidget(this->_gameLogsTitle);
    this->_layout.addSpacing(2);
    this->_layout.addWidget(this->_gameLogsScroller);
#if DISPLAY_FILE_TREES
    this->_layout.addSpacing(5);
    this->_layout.addWidget(this->_gameFileTreeTitle);
    this->_layout.addSpacing(2);
    this->_layout.addWidget(this->_gameFileTreeScroller);
#endif
    this->_layout.addSpacing(5);
    this->_layout.addLayout(this->_buttonsLayout);

    this->setStyleSheet("DebugLogs { " + Assets::Instance()->DEFAULT_WINDOW_STYLE + " }");
    this->setLayout(&this->_layout);
    this->_yumiLogsScroller->setWidget(this->_yumiLogsLabel);
    this->_gameLogsScroller->setWidget(this->_gameLogsLabel);
#if DISPLAY_FILE_TREES
    this->_yumiFileTreeScroller->setWidget(this->_yumiFileTreeLabel);
    this->_gameFileTreeScroller->setWidget(this->_gameFileTreeLabel);
#endif

    this->_gameLogsTitle->setVisible(false);
    this->_gameLogsScroller->setVisible(false);
#if DISPLAY_FILE_TREES
    this->_gameFileTreeTitle->setVisible(false);
    this->_gameFileTreeScroller->setVisible(false);
#endif
}

void DebugLogs::updateStyles()
{
    this->_yumiLogsTitle->setStyleSheet("QLabel { margin-left: 5px; " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");
    this->_yumiLogsLabel->setStyleSheet("QLabel { background-color: transparent; " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_yumiLogsScroller->setStyleSheet("QScrollArea { margin-left: 10px; margin-right: 10px; padding-left: 5px; " + Assets::Instance()->REGULAR_SCROLL_AREA_STYLE +  " }");
    this->_selectGameLabel->setStyleSheet("QLabel { margin-left: 5px; " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");
    this->_selectGameDropdown->setStyleSheet("QComboBox { " + Assets::Instance()->COMBOBOX_LABEL_STYLE + " }");
    this->_gameLogsTitle->setStyleSheet("QLabel { margin-left: 5px; " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");
    this->_gameLogsLabel->setStyleSheet("QLabel { background-color: transparent; " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_gameLogsScroller->setStyleSheet("QScrollArea { margin-left: 10px; margin-right: 10px; padding-left: 5px; " + Assets::Instance()->REGULAR_SCROLL_AREA_STYLE +  " }");
#if DISPLAY_FILE_TREES
    this->_yumiFileTreeTitle->setStyleSheet("QLabel { margin-left: 5px; " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");
    this->_yumiFileTreeLabel->setStyleSheet("QLabel { background-color: transparent; " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_yumiFileTreeScroller->setStyleSheet("QScrollArea { margin-left: 10px; margin-right: 10px; padding-left: 5px; " + Assets::Instance()->REGULAR_SCROLL_AREA_STYLE +  " }");
    this->_gameFileTreeTitle->setStyleSheet("QLabel { margin-left: 5px; " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");
    this->_gameFileTreeLabel->setStyleSheet("QLabel { background-color: transparent; " + Assets::Instance()->ALTERNATE_LABEL_STYLE + " }");
    this->_gameFileTreeScroller->setStyleSheet("QScrollArea { margin-left: 10px; margin-right: 10px; padding-left: 5px; " + Assets::Instance()->REGULAR_SCROLL_AREA_STYLE +  " }");
#endif
    this->_exportButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->_closeButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->setStyleSheet("DebugLogs { " + Assets::Instance()->DEFAULT_WINDOW_STYLE + " }");

    this->_title->updateStyles();

    adjustSize();
    update();
}

void DebugLogs::doShowAt(const QPoint& center)
{
    this->show();
    this->adjustSize();
    this->move(center.x() - (this->width() / 2), center.y() - (this->height() / 2));
}

void DebugLogs::refreshSelectedGameData(const QString& selectedGame)
{
    bool found = false;
    if (!selectedGame.isEmpty())
    {
        int nbGames = ((yumi*)_yumiPtr)->gamesInfo.count();
        for (int i = 0; i < nbGames; i++)
            if (selectedGame.compare((((yumi*)_yumiPtr)->gamesInfo)[i].name) == 0)
            {
                found = true;
                GameInfo* gi = &(((yumi*)_yumiPtr)->gamesInfo[i]);
                this->_gameLogsLabel->setText(DebugInfo::Instance()->GetGameLogs(gi, false));
                this->_gameLogsLabel->adjustSize();
#if DISPLAY_FILE_TREES
                this->_gameFileTreeLabel->setText(DebugInfo::Instance()->GetGameFileTree(gi, false));
                this->_gameFileTreeLabel->adjustSize();
#endif
                this->_gameLogsTitle->setVisible(true);
                this->_gameLogsScroller->setVisible(true);
#if DISPLAY_FILE_TREES
                this->_gameFileTreeTitle->setVisible(true);
                this->_gameFileTreeScroller->setVisible(true);
#endif
                break;
            }
    }
    if (!found)
    {
        this->_gameLogsTitle->setVisible(false);
        this->_gameLogsScroller->setVisible(false);
#if DISPLAY_FILE_TREES
        this->_gameFileTreeTitle->setVisible(false);
        this->_gameFileTreeScroller->setVisible(false);
#endif
    }
}

void DebugLogs::selectedGameChanged(QString selectedGame)
{
    refreshSelectedGameData(selectedGame);
    adjustSize();
    update();
}

QString DebugLogs::getFullLogs()
{
    GameInfo* gi = NULL;
    QString selectedGame = this->_selectGameDropdown->currentText();
    if (!selectedGame.isEmpty())
    {
        int nbGames = ((yumi*)_yumiPtr)->gamesInfo.count();
        for (int i = 0; i < nbGames; i++)
            if (selectedGame.compare(((yumi*)_yumiPtr)->gamesInfo[i].name) == 0)
            {
                gi = &(((yumi*)_yumiPtr)->gamesInfo[i]);
                break;
            }
    }
    return DebugInfo::Instance()->GetFullDebugLogs(gi, true);
}

void DebugLogs::copyLogsToClipboard()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug() << "Copy logs to clipboard clicked!";
#endif
    QString toLog = getFullLogs();
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(toLog, QClipboard::Clipboard);
    if (clipboard->supportsSelection())
        clipboard->setText(toLog, QClipboard::Selection);
#ifndef Q_OS_WINDOWS
    QThread::msleep(1);
#endif
    QMessageBox copiedToClipboard = QMessageBox(QMessageBox::Information, QCoreApplication::translate("DebugLogs", "Copied to clipboard", "Popup title"), QCoreApplication::translate("DebugLogs", "Debug logs have been copied to clipboard.", "Popup text"), QMessageBox::Ok, this);
    copiedToClipboard.exec();
}

void DebugLogs::exportLogs()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug() << "Export logs clicked!";
#endif
    QString toLog = getFullLogs();
    QString debugLogFilePath(Utils::toUnixPath(yumi::appPath + QDir::separator() + DEBUG_LOG_FILENAME));
    QFile debugLogFile(debugLogFilePath);
    if (debugLogFile.open(QIODevice::WriteOnly))
    {
        if (debugLogFile.write(toLog.toUtf8()))
        {
            QString folderUrl = "file:///" + Utils::toUnixPath(yumi::appPath);
            QUrl folderQUrl = QUrl(folderUrl.replace("file:////", "file:///"), QUrl::ParsingMode::TolerantMode);
            if (!QDesktopServices::openUrl(folderQUrl))
                qWarning().nospace() << "Unable to open folder " << folderQUrl.toString() << " (QDesktopServices failure).";
            QMessageBox exportedToFile = QMessageBox(QMessageBox::Information, QCoreApplication::translate("DebugLogs", "Exported to file", "Popup title"), QCoreApplication::translate("DebugLogs", "Debug logs have been exported to file \"%1\".", "Popup text").arg(debugLogFilePath), QMessageBox::Ok, this);
            exportedToFile.exec();
        }
        else
            qCritical().nospace() << "Unable to write logs to file " << debugLogFilePath << ".";
        debugLogFile.close();
    }
    else
        qCritical().nospace() << "Unable to open file " << debugLogFilePath << " for writing.";
}

void DebugLogs::showEvent(QShowEvent*)
{
    this->_yumiLogsLabel->setText(DebugInfo::Instance()->GetYUMILogs(false));
#if DISPLAY_FILE_TREES
    this->_yumiFileTreeLabel->setText(DebugInfo::Instance()->GetYUMIFileTree(false));
#endif
    refreshSelectedGameData(this->_selectGameDropdown->currentText());
}

void DebugLogs::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        _mousePos = event->globalPos() - pos();
        _windowMoving = true;
    }
    QWidget::mousePressEvent(event);
}

void DebugLogs::mouseMoveEvent(QMouseEvent* event)
{
    if (_windowMoving && (event->buttons() & Qt::LeftButton))
    {
        event->accept();
        move(event->globalPos() - _mousePos);
    }
    else
        QWidget::mouseMoveEvent(event);
}

void DebugLogs::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        _windowMoving = false;
    QWidget::mouseReleaseEvent(event);
}

void DebugLogs::paintEvent(QPaintEvent*)
{
#if IS_DEBUG && DEBUG_PAINTING
    qDebug() << "DebugLogs paint event!";
#endif

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
