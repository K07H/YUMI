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
#include <QStyleOption>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QFileInfo>
#include "drop_mod_window.h"
#include "common.h"
#include "utils.h"
#include "assets.h"
#include "yumi.h"
#include "mods_loader.h"

DropModWindow::DropModWindow(void* yumiPtr, QWidget* parent) : QWidget(parent)
{
    this->_windowMoving = false;
    this->_yumiPtr = yumiPtr;
    this->_drawTransparentArea = true;
    this->_modsFolderPath = Utils::toUnixPath(yumi::appPath + QDir::separator() + "mods");
    this->_gameName = ((yumi*)yumiPtr)->selectedGame->name;
    this->_areaColor = QColor(Assets::Instance()->secondaryColorLight);
    this->_areaColor.setAlpha(220);
    this->_areaColorOpaque = QColor(Assets::Instance()->secondaryColorLight);
    this->_dropModHereColor = QColor(Assets::Instance()->lightTextColor);
    this->_dropModHereColor.setAlpha(200);
    this->_dropModHereFont = new QFont();
    this->_dropModHereFont->setFamily(Assets::Instance()->PRIMARY_FONT_FAMILY);
    this->_dropModHereFont->setPixelSize(Assets::Instance()->XXL_FONT_SIZE_PX.toInt());
    this->_dropModHereFont->setStyleHint(QFont::StyleHint::SansSerif);
    this->_dropModHereFont->setItalic(true);

    QString windowTitle(QCoreApplication::translate("DropModWindow", "Install mod", "Window title"));

    this->setWindowTitle(windowTitle);
    this->setWindowIcon(Assets::Instance()->appIcon);
    this->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_NoSystemBackground, true);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setContentsMargins(0, 0, 0, 0);
    this->setAcceptDrops(true);

    this->_title = new WindowTitle(windowTitle, this);

    this->_methodLabel = new QLabel(QCoreApplication::translate("DropModWindow", "Drop a mod archive or a mod folder in the area below and it will be added to \"%1\".", "Label text").arg(this->_gameName));
    this->_methodLabel->setWordWrap(true);
    this->_methodLabel->setMinimumWidth(700);
    this->_methodLabel->setStyleSheet("QLabel { margin-left: 10px; margin-right: 10px; margin-bottom: 5px; " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");

    this->_alternateMethodLabel = new QLabel("<i>" + QCoreApplication::translate("DropModWindow", "(Alternatively you can also install a mod archive or a mod folder by placing it inside folder \"%1\")", "Label text").arg(this->_modsFolderPath) + "</i>");
    this->_alternateMethodLabel->setTextFormat(Qt::RichText);
    this->_alternateMethodLabel->setWordWrap(true);
    this->_alternateMethodLabel->setMinimumWidth(700);
    this->_alternateMethodLabel->setStyleSheet("QLabel { margin-left: 10px; margin-right: 10px; margin-bottom: 5px; " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");

    this->_closeButton = new QPushButton();
    this->_closeButton->setText(QCoreApplication::translate("DropModWindow", "&Close", "Button text"));
    this->_closeButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->_closeButton->setCursor(Qt::PointingHandCursor);
    this->_closeButton->setContentsMargins(15, 0, 15, 0);
    this->connect(this->_closeButton, SIGNAL(clicked()), this, SLOT(close()));

    this->_buttonsLayout = new QVBoxLayout();
    this->_buttonsLayout->addWidget(this->_closeButton);
    this->_buttonsLayout->setContentsMargins(10, 10, 10, 10);

    this->_layout.setContentsMargins(0, 0, 0, 0);
    this->_layout.setSpacing(0);
    this->_layout.setAlignment(Qt::AlignTop);
    this->_layout.addWidget(this->_title);
    this->_layout.addSpacing(5);
    this->_layout.addWidget(this->_methodLabel);
    this->_layout.addSpacing(200);
    this->_layout.addWidget(this->_alternateMethodLabel);
    this->_layout.addLayout(this->_buttonsLayout);

    this->setStyleSheet("DropModWindow { " + Assets::Instance()->DEFAULT_WINDOW_STYLE + " }");
    this->setLayout(&this->_layout);
}

void DropModWindow::updateStyles()
{
    this->_areaColor = QColor(Assets::Instance()->secondaryColorLight);
    this->_areaColor.setAlpha(220);
    this->_areaColorOpaque = QColor(Assets::Instance()->secondaryColorLight);
    this->_dropModHereColor = QColor(Assets::Instance()->lightTextColor);
    this->_dropModHereColor.setAlpha(200);
    this->_dropModHereFont->setFamily(Assets::Instance()->PRIMARY_FONT_FAMILY);
    this->_dropModHereFont->setPixelSize(Assets::Instance()->XXL_FONT_SIZE_PX.toInt());

    this->_methodLabel->setStyleSheet("QLabel { margin-left: 10px; margin-right: 10px; margin-bottom: 5px; " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");
    this->_alternateMethodLabel->setStyleSheet("QLabel { margin-left: 10px; margin-right: 10px; margin-bottom: 5px; " + Assets::Instance()->REGULAR_LABEL_STYLE + " }");
    this->_closeButton->setStyleSheet(Assets::Instance()->mainBtnStyleFullWidth);
    this->setStyleSheet("DropModWindow { " + Assets::Instance()->DEFAULT_WINDOW_STYLE + " }");

    this->_title->updateStyles();

    adjustSize();
    update();
}

void DropModWindow::doShowAt(const QPoint& center)
{
    this->show();
    this->adjustSize();
    this->move(center.x() - (this->width() / 2), center.y() - (this->height() / 2));
    QApplication::restoreOverrideCursor();
}

QStringList DropModWindow::getDroppedFilesList(const QMimeData* mimeData)
{
    QStringList pathList = QStringList();
    QList<QUrl> urlList = mimeData->urls();
    int nbUrls = urlList.size();
    for (int i = 0; i < nbUrls && i < MAX_FILES_DROP_AT_ONCE; i++)
        pathList.append(urlList.at(i).toLocalFile());
    int nbPaths = pathList.count();
    QStringList validPaths = QStringList();
    for (int i = 0; i < nbPaths; i++)
    {
        QFileInfo fi(pathList.at(i));
        if (fi.exists() && (fi.isDir() || (fi.isFile() && fi.completeSuffix().endsWith("zip", Qt::CaseInsensitive))))
        {
            qDebug().nospace() << "Receiving valid dropped file " << pathList.at(i) << ".";
            validPaths.push_back(pathList.at(i));
        }
        else
            qWarning().nospace() << "Skipping unsupported dropped file " << pathList.at(i) << ".";
    }
    return validPaths;
}

void DropModWindow::processDroppedMods()
{
    int nbFiles = this->_modsToProcess.count();
    if (nbFiles <= 0)
    {
        ((yumi*)_yumiPtr)->pendingDragAndDrop = false;
        return;
    }

    QDir modsFolder(this->_modsFolderPath);
    if (!modsFolder.exists())
    {
        QDir yumiFolder(yumi::appPath);
        if (yumiFolder.exists())
            yumiFolder.mkdir("mods");
    }
    if (!modsFolder.exists())
    {
        qWarning().nospace() << "Unable to create mods folder at " << this->_modsFolderPath << ". Skipping mods installation.";
        this->_modsToProcess.clear();
        ((yumi*)_yumiPtr)->pendingDragAndDrop = false;
        return;
    }

    ((yumi*)_yumiPtr)->showInstallingModWindow(false);

    int nbDroppedModsQueued = 0;
    for (int i = 0; i < nbFiles; i++)
    {
        QFileInfo fi(this->_modsToProcess[i]);
        if (fi.isDir())
        {
            QDir toCopy(this->_modsToProcess[i]);
            if (toCopy.exists())
            {
                QString modName = toCopy.dirName();
                if (!modName.isEmpty())
                {
                    QString destPath(Utils::toUnixPath(this->_modsFolderPath + QDir::separator() + modName));
                    QDir destFolder(destPath);
                    if (destFolder.exists())
                        if (!destFolder.removeRecursively())
                            qWarning().nospace() << "Unable to remove old folder for mod " << modName << " at " << destPath << ".";
                    if (!destFolder.exists())
                        modsFolder.mkdir(modName);
                    if (destFolder.exists())
                    {
                        if (Utils::copyRecursively(this->_modsToProcess[i], destPath))
                        {
                            nbDroppedModsQueued++;
                            if (!this->_gameName.isEmpty())
                                ModsLoader::Instance()->installModInfo->insert(modName, this->_gameName);
                            qInfo().nospace() << "Successfully copied files for mod " << modName << ".";
                        }
                        else
                            qWarning().nospace() << "Failed to copy some files from " << this->_modsToProcess[i] << " to " << destPath;
                    }
                    else
                        qWarning().nospace() << "Unable to create folder for mod " << modName << " at " << destPath << ".";
                }
            }
        }
        else if (fi.isFile() && fi.completeSuffix().endsWith("zip", Qt::CaseInsensitive) && fi.fileName().length() > 4)
        {
            QFile toCopy(this->_modsToProcess[i]);
            if (toCopy.exists())
            {
                QString fileName = fi.fileName();
                QString modName = fileName.left(fileName.length() - 4);
                QString destPath(Utils::toUnixPath(this->_modsFolderPath + QDir::separator() + fileName));
                QFile destFile(destPath);
                if (destFile.exists())
                    if (!destFile.remove())
                        qWarning().nospace() << "Unable to remove old file for mod " << modName << " at " << destPath << " (Error message: " << destFile.errorString() << ").";
                if (!destFile.exists())
                {
                    if (toCopy.copy(destPath))
                    {
                        nbDroppedModsQueued++;
                        if (!this->_gameName.isEmpty())
                            ModsLoader::Instance()->installModInfo->insert(fileName, this->_gameName);
                        qInfo().nospace() << "Successfully copied zip file for mod " << modName << ".";
                    }
                    else
                        qWarning().nospace() << "Failed to copy zip files from " << this->_modsToProcess[i] << " to " << destPath;
                }
            }
        }
    }

    qDebug().nospace() << nbFiles << " dropped mods have been processed. Mod install will now trigger automatically if needed.";
    this->_modsToProcess.clear();
    if (nbDroppedModsQueued > 0)
        ((yumi*)_yumiPtr)->forceRefreshModsMonitoring();
    else
        ((yumi*)_yumiPtr)->showInstallingModWindow(true);
    ((yumi*)_yumiPtr)->pendingDragAndDrop = false;
}

void DropModWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData() != NULL && event->mimeData()->hasUrls() && !((yumi*)_yumiPtr)->yumiIsStarting && !((yumi*)_yumiPtr)->pendingDragAndDrop && !ModsLoader::Instance()->installInProgress)
    {
        QStringList files = getDroppedFilesList(event->mimeData());
        if (files.count() > 0)
        {
            event->acceptProposedAction();
            this->_drawTransparentArea = false;
            update();
        }
    }
}

void DropModWindow::dragLeaveEvent(QDragLeaveEvent*)
{
    this->_drawTransparentArea = true;
    update();
}

void DropModWindow::dropEvent(QDropEvent* event)
{
    ((yumi*)_yumiPtr)->pendingDragAndDrop = true;
    this->_drawTransparentArea = true;

    this->_modsToProcess = getDroppedFilesList(event->mimeData());
    if (this->_modsToProcess.count() > 0)
        this->_processDroppedMods = QtConcurrent::run(this, &DropModWindow::processDroppedMods);

    update();
}

void DropModWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        _mousePos = event->globalPos() - pos();
        _windowMoving = true;
    }
    QWidget::mousePressEvent(event);
}

void DropModWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (_windowMoving && (event->buttons() & Qt::LeftButton))
    {
        event->accept();
        move(event->globalPos() - _mousePos);
    }
    else
        QWidget::mouseMoveEvent(event);
}

void DropModWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        _windowMoving = false;
    QWidget::mouseReleaseEvent(event);
}

void DropModWindow::paintEvent(QPaintEvent*)
{
#if IS_DEBUG && DEBUG_PAINTING
    qDebug() << "DropModWindow paint event!";
#endif

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    p.setBackground(Qt::transparent);
    p.setBackgroundMode(Qt::TransparentMode);
    p.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    QRect rectArea = rect();
    rectArea.setX(rectArea.x() + 15);
    rectArea.setY(rectArea.y() + 115);
    rectArea.setWidth(rectArea.width() - 15);
    rectArea.setHeight(rectArea.height() - 160);
    path.addRoundedRect(rectArea, 10, 10);
    QPen pen(Assets::Instance()->secondaryColor, 5, Qt::DashLine, Qt::RoundCap);
    p.setPen(pen);
    p.fillPath(path, (this->_drawTransparentArea ? this->_areaColor : this->_areaColorOpaque));
    p.drawPath(path);

    p.setFont(*_dropModHereFont);

    p.setPen(this->_dropModHereColor);
    p.drawText(rectArea, Qt::AlignCenter, QCoreApplication::translate("DropModWindow", "DROP MOD HERE", "Label text"));
}
