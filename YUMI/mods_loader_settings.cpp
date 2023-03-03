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
#include <QCoreApplication>
#include <QStyleOption>
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include <QStringList>
#include <QIODevice>
#include <QFile>
#include <QTextStream>
#include "mods_loader_settings.h"
#include "assets.h"
#include "common.h"
#include "mods_loader.h"
#include "utils.h"
#include "game_details.h"
#include "yumi.h"

ModsLoaderSettings::ModsLoaderSettings(void* yumiPtr, GameInfo* gameInfo, void* gameDetails, QWidget* parent) : QWidget(parent)
{
    this->_yumiPtr = yumiPtr;
    this->_gameDetails = gameDetails;
    this->_gameInfo = gameInfo;
    this->_windowMoving = false;

    QString windowTitle(QCoreApplication::translate("ModsLoaderSettings", "Mods loader settings", "Window title"));

    this->setWindowTitle(windowTitle);
    this->setWindowIcon(Assets::Instance()->appIcon);
    this->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_NoSystemBackground, true);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setContentsMargins(0, 0, 0, 0);

    this->_title = new WindowTitle(windowTitle, this);

    this->_gameExePathLabel = new QLabel(QCoreApplication::translate("ModsLoaderSettings", "Game executable location:", "Label text"));
    this->_gameExePathLabel->setStyleSheet("QLabel { " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");

    this->_gameExePath = new QLabel(this->_gameInfo->exePath);
    this->_gameExePath->setStyleSheet("QLabel { " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");

    this->_gameExePathBtn = new QPushButton();
    this->_gameExePathBtn->setText(QCoreApplication::translate("ModsLoaderSettings", "&Modify", "Button text"));
    this->_gameExePathBtn->setStyleSheet(Assets::Instance()->secondaryBtnStyle);
    this->_gameExePathBtn->setCursor(Qt::PointingHandCursor);
    connect(this->_gameExePathBtn, SIGNAL(clicked()), this, SLOT(modifyGameExePath()));

    this->_gameExePathLayout = new QVBoxLayout();
    this->_gameExePathLayout->setContentsMargins(10, 0, 10, 0);
    this->_gameExePathLayout->setSpacing(0);
    this->_gameExePathLayout->addWidget(this->_gameExePathLabel);
    this->_gameExePathLayout->addWidget(this->_gameExePath);
    this->_gameExePathLayout->addWidget(this->_gameExePathBtn);

    this->_gameExeTypeLabel = new QLabel(QCoreApplication::translate("ModsLoaderSettings", "Game executable type:", "Label text"));
    this->_gameExeTypeLabel->setStyleSheet("QLabel {" + Assets::Instance()->REGULAR_LABEL_STYLE + "}");

    this->_gameExeType = new QComboBox();
    this->_gameExeType->addItems(QStringList() << "Windows 64 bits" << "Windows 32 bits" << "Unix 64 bits (Linux, SteamOS, ...)" << "Unix 32 bits (Linux, SteamOS, ...)" << "Darwin (iOS/macOS)" << "Unknown");
    this->_gameExeType->setCurrentText(getExeTypeComboBoxStr(this->_gameInfo->exeType));
    this->_gameExeType->setCursor(Qt::PointingHandCursor);
    this->_gameExeType->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    this->_gameExeTypeLayout = new QHBoxLayout();
    this->_gameExeTypeLayout->setContentsMargins(10, 0, 10, 0);
    this->_gameExeTypeLayout->setSpacing(0);
    this->_gameExeTypeLayout->setAlignment(Qt::AlignVCenter);
    this->_gameExeTypeLayout->addWidget(this->_gameExeTypeLabel);
    this->_gameExeTypeLayout->addSpacing(5);
    this->_gameExeTypeLayout->addWidget(this->_gameExeType);

    this->_applyButton = new QPushButton();
    this->_applyButton->setText(QCoreApplication::translate("ModsLoaderSettings", "&Apply", "Button text"));
    this->_applyButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->_applyButton->setCursor(Qt::PointingHandCursor);
    this->_applyButton->setContentsMargins(15, 0, 15, 0);
    connect(this->_applyButton, SIGNAL(clicked()), this, SLOT(apply()));

    this->_cancelButton = new QPushButton();
    this->_cancelButton->setText(QCoreApplication::translate("ModsLoaderSettings", "&Cancel", "Button text"));
    this->_cancelButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->_cancelButton->setCursor(Qt::PointingHandCursor);
    this->_cancelButton->setContentsMargins(15, 0, 15, 0);
    connect(this->_cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

    this->_buttonsLayout = new QHBoxLayout();
    this->_buttonsLayout->addWidget(this->_applyButton);
    this->_buttonsLayout->addSpacing(10);
    this->_buttonsLayout->addWidget(this->_cancelButton);
    this->_buttonsLayout->setContentsMargins(10, 10, 10, 10);
    this->_buttonsLayout->setSpacing(0);
    this->_buttonsLayout->setAlignment(Qt::AlignBottom);

    this->_layout.addWidget(this->_title);
    this->_layout.addSpacing(5);
    this->_layout.addLayout(this->_gameExePathLayout);
    this->_layout.addSpacing(15);
    this->_layout.addLayout(this->_gameExeTypeLayout);
    this->_layout.addSpacing(25);
    this->_layout.addLayout(this->_buttonsLayout);
    this->_layout.setContentsMargins(0, 0, 0, 0);
    this->_layout.setSpacing(0);
    this->_layout.setAlignment(Qt::AlignTop);

    this->setStyleSheet("ModsLoaderSettings { " + Assets::Instance()->DEFAULT_WINDOW_STYLE + " }");
    this->setLayout(&this->_layout);
}

QString ModsLoaderSettings::getExeTypeComboBoxStr(int exeType)
{
    if (exeType == WIN_GAME_X64)
        return "Windows 64 bits";
    if (exeType == WIN_GAME_X86)
        return "Windows 32 bits";
    if (exeType == WIN_GAME_IL2CPP_X64)
        return "Windows 64 bits (IL2CPP)";
    if (exeType == WIN_GAME_IL2CPP_X86)
        return "Windows 32 bits (IL2CPP)";
    if (exeType == UNIX_GAME_X64)
        return "Unix 64 bits (Linux, SteamOS, ...)";
    if (exeType == UNIX_GAME_X86)
        return "Unix 32 bits (Linux, SteamOS, ...)";
    if (exeType == DARWIN_GAME)
        return "Darwin (iOS/macOS)";

    return "Unknown";
}

int ModsLoaderSettings::getExeTypeComboBoxInt(const QString& exeType)
{
    if (exeType.compare("Windows 64 bits") == 0)
        return WIN_GAME_X64;
    if (exeType.compare("Windows 32 bits") == 0)
        return WIN_GAME_X86;
    if (exeType.compare("Windows 64 bits (IL2CPP)") == 0)
        return WIN_GAME_IL2CPP_X64;
    if (exeType.compare("Windows 32 bits (IL2CPP)") == 0)
        return WIN_GAME_IL2CPP_X86;
    if (exeType.compare("Unix 64 bits (Linux, SteamOS, ...)") == 0)
        return UNIX_GAME_X64;
    if (exeType.compare("Unix 32 bits (Linux, SteamOS, ...)") == 0)
        return UNIX_GAME_X86;
    if (exeType.compare("Darwin (iOS/macOS)") == 0)
        return DARWIN_GAME;

    return UNKNOWN_GAME;
}

void ModsLoaderSettings::updateStyles()
{
    this->_gameExePathLabel->setStyleSheet("QLabel { " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");
    this->_gameExePath->setStyleSheet("QLabel { " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");
    this->_gameExePathBtn->setStyleSheet(Assets::Instance()->secondaryBtnStyle);
    this->_gameExeTypeLabel->setStyleSheet("QLabel {" + Assets::Instance()->REGULAR_LABEL_STYLE + "}");
    this->_applyButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->_cancelButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->setStyleSheet("ModsLoaderSettings { " + Assets::Instance()->DEFAULT_WINDOW_STYLE + " }");

    adjustSize();
    update();
}

void ModsLoaderSettings::modifyGameExePath()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug() << "ModsLoaderSettings modify game exe path button clicked!";
#endif
    QFileDialog fileDialog(this, QCoreApplication::translate("ModsLoaderSettings", "Select game executable...", "File selection dialog title"));
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    if (fileDialog.exec() == QDialog::Accepted)
    {
        QStringList selectedFiles = fileDialog.selectedFiles();
        if (selectedFiles.count() > 0)
        {
            QString exePath = selectedFiles.constFirst();
            if (!exePath.isEmpty())
            {
                exePath = Utils::toUnixPath(exePath);
                this->_gameExePath->setText(exePath);
                int exeType = ModsLoader::Instance()->getGameExeType(exePath);
                this->_gameExeType->setCurrentText(this->getExeTypeComboBoxStr(exeType));
                if (exeType == UNKNOWN_GAME)
                {
                    QMessageBox selectionFailed = QMessageBox(QMessageBox::Warning, QCoreApplication::translate("ModsLoaderSettings", "Unknown executable type", "Popup title"), QCoreApplication::translate("ModsLoaderSettings", "It was not possible to automatically determine the type of the selected game executable. Please specify game executable type manually.", "Popup text"), QMessageBox::Ok, this);
                    selectionFailed.exec();
                }
            }
        }
    }
}

void ModsLoaderSettings::apply()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug() << "ModsLoaderSettings apply button clicked!";
#endif
    bool exePathChanged = (this->_gameInfo->exePath.compare(this->_gameExePath->text()) != 0);
    bool exeTypeChanged = (this->getExeTypeComboBoxStr(this->_gameInfo->exeType).compare(this->_gameExeType->currentText()) != 0);
    if (!exePathChanged && !exeTypeChanged)
    {
        close();
        return;
    }
    if (this->_gameExeType->currentText().compare("Unknown") == 0)
    {
        QMessageBox error = QMessageBox(QMessageBox::Warning, QCoreApplication::translate("ModsLoaderSettings", "Error", "Popup title"), QCoreApplication::translate("ModsLoaderSettings", "Unknown game executable is not supported.", "Popup text"), QMessageBox::Yes | QMessageBox::No, this);
        error.exec();
        return;
    }
    QMessageBox warning = QMessageBox(QMessageBox::Warning, QCoreApplication::translate("ModsLoaderSettings", "Warning", "Popup title"), QCoreApplication::translate("ModsLoaderSettings", "It is not recommended to modify the mods loader settings. Are you sure that you want to apply modifications?", "Popup text"), QMessageBox::Yes | QMessageBox::No, this);
    int clickedBtn = warning.exec();
    if (clickedBtn != QMessageBox::Yes)
        return;

    this->_gameInfo->exePath = this->_gameExePath->text();
    this->_gameInfo->exeType = this->getExeTypeComboBoxInt(this->_gameExeType->currentText());
    if (((yumi*)_yumiPtr)->selectedGame != NULL)
    {
        if (this->_gameInfo->exePath.compare(((yumi*)_yumiPtr)->selectedGame->exePath) != 0)
            ((yumi*)_yumiPtr)->selectedGame->exePath = this->_gameInfo->exePath;
        if (this->_gameInfo->exeType != ((yumi*)_yumiPtr)->selectedGame->exeType)
            ((yumi*)_yumiPtr)->selectedGame->exeType = this->_gameInfo->exeType;
    }
    int len = ((yumi*)_yumiPtr)->gamesInfo.count();
    if (len > 0)
        for (int i = 0; i < len; i++)
            if (this->_gameInfo->name.compare(((yumi*)_yumiPtr)->gamesInfo[i].name) == 0 && this->_gameInfo->path.compare(((yumi*)_yumiPtr)->gamesInfo[i].path) == 0)
            {
                if (this->_gameInfo->exePath.compare(((yumi*)_yumiPtr)->gamesInfo[i].exePath) != 0)
                    ((yumi*)_yumiPtr)->gamesInfo[i].exePath = this->_gameInfo->exePath;
                if (this->_gameInfo->exeType !=  ((yumi*)_yumiPtr)->gamesInfo[i].exeType)
                    ((yumi*)_yumiPtr)->gamesInfo[i].exeType = this->_gameInfo->exeType;
                break;
            }

    ((GameDetails*)_gameDetails)->updateGameDetails(this->_gameInfo);

    Config::Instance()->saveConfig();

    QString scriptPath = Utils::toUnixPath(this->_gameInfo->path + QDir::separator() + RUN_YUMI_SCRIPT_NAME + ".sh");
    QFile scriptFile = QFile(scriptPath);
    if (!scriptFile.exists())
    {
        qInfo().nospace() << "Script file not found at " << scriptPath << ". Skipping UNIX modification.";
        close();
        return;
    }
    if (!scriptFile.open(QIODevice::ReadOnly | QIODevice::ExistingOnly | QIODevice::Text))
    {
        qWarning().nospace() << "Unable to open script file at " << scriptPath << " for reading.";
        close();
        return;
    }

    QString data;
    try { data = scriptFile.readAll(); }
    catch (const std::exception& ex) { scriptFile.close(); qWarning().nospace() << "Exception caught while reading BepInEx script. Exception=[" << ex.what() << "]"; close(); return; }
    catch (...) { scriptFile.close(); qWarning() << "Exception caught while reading BepInEx script."; close(); return; }
    scriptFile.close();

    QString delim = "\nexecutable_name=\"";
    if (data.isEmpty() || !data.contains(delim))
    {
        qWarning().nospace() << "Unable to find \"executable_name\" variable in script file at " << scriptPath << ".";
        close();
        return;
    }

    int exeLineStt = data.indexOf(delim);
    exeLineStt += delim.length();
    int exeLineEnd = data.indexOf('"', exeLineStt);
    QString currentExeName = "";
    if (exeLineEnd > exeLineStt)
        currentExeName = data.mid(exeLineStt, exeLineEnd - exeLineStt);
    QString toReplace = delim + currentExeName + "\"";

    int lastSeparatorPos = this->_gameInfo->exePath.lastIndexOf('/');
    QString newExeName((lastSeparatorPos >= 0 && this->_gameInfo->exePath.length() > lastSeparatorPos + 1) ? this->_gameInfo->exePath.mid(lastSeparatorPos + 1) : this->_gameInfo->exePath);
    QString replaceWith = delim + newExeName + "\"";

    qDebug().nospace() << "Replacing " << toReplace << " with " << replaceWith << " in BepInEx script file.";
    QString edited(data.replace(toReplace, replaceWith));

    if (!scriptFile.open(QIODevice::WriteOnly | QIODevice::ExistingOnly | QIODevice::Truncate | QIODevice::Text))
    {
        if (!scriptFile.setPermissions(scriptFile.permissions() | QFileDevice::WriteOwner | QFileDevice::WriteUser))
        {
            qWarning().nospace() << "Unable to get write permissions to script file at " << scriptPath << " for writing (permissions: " << scriptFile.permissions() << ").";
            close();
            return;
        }
        if (!scriptFile.open(QIODevice::WriteOnly | QIODevice::ExistingOnly | QIODevice::Truncate | QIODevice::Text))
        {
            qWarning().nospace() << "Unable to open script file at " << scriptPath << " for writing (permissions: " << scriptFile.permissions() << ").";
            close();
            return;
        }
    }
    if (!scriptFile.isWritable())
    {
        scriptFile.close();
        qWarning().nospace() << "Script file at " << scriptPath << " is not writable.";
        close();
        return;
    }

    try
    {
        QTextStream ts(&scriptFile);
        ts << edited;
        ts.flush();
        scriptFile.close();
    }
    catch (const std::exception& ex)
    {
        scriptFile.close();
        qWarning().nospace() << "Exception caught while writing executable name inside BepInEx script. Exception=[" << ex.what() << "]";
        close();
        return;
    }
    catch (...)
    {
        scriptFile.close();
        qWarning() << "Exception caught while writing executable name inside BepInEx script.";
        close();
        return;
    }
    close();
}

void ModsLoaderSettings::cancel()
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug() << "ModsLoaderSettings cancel button clicked!";
#endif
    close();
}

void ModsLoaderSettings::doShowAt(const QPoint& center)
{
    this->show();
    this->adjustSize();
    this->move(center.x() - (this->width() / 2), center.y() - (this->height() / 2));
}

void ModsLoaderSettings::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        _mousePos = event->globalPos() - pos();
        _windowMoving = true;
    }
    QWidget::mousePressEvent(event);
}

void ModsLoaderSettings::mouseMoveEvent(QMouseEvent* event)
{
    if (_windowMoving && (event->buttons() & Qt::LeftButton))
    {
        event->accept();
        move(event->globalPos() - _mousePos);
    }
    else
        QWidget::mouseMoveEvent(event);
}

void ModsLoaderSettings::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        _windowMoving = false;
    QWidget::mouseReleaseEvent(event);
}

void ModsLoaderSettings::paintEvent(QPaintEvent*)
{
#if IS_DEBUG && DEBUG_PAINTING
    qDebug() << "ModsLoaderSettings paint event!";
#endif

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
