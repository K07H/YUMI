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

#ifndef __YUMI_H__
#define __YUMI_H__

#include <QMainWindow>
#include <QString>
#include <QAction>
#include <QIcon>
#include <QVector>
#include <QDateTime>
#include <QtConcurrent/QtConcurrentRun>
#include <tuple>
#include "game_info.h"
#include "main_widget.h"
#include "about.h"
#include "config.h"
#include "game_loader.h"
#include "license_viewer.h"
#include "yumi_settings.h"
#include "yumi_network.h"
#include "debug_logs.h"
#include "installing_mod_window.h"

class yumi : public QMainWindow
{
    Q_OBJECT

public:
    explicit yumi(QApplication* app, QWidget *parent = nullptr);
    ~yumi();

    static QString appPath;
    static int initialWindowWidth;
    static int initialWindowHeight;

    QApplication* appPtr;
    MainWidget* mainWidget;
    Config* configuration;
    YumiNetwork* network;
    GameLoader* gameLoader;
    QList<GameInfo> gamesInfo;
    GameInfo* selectedGame;
    QString theme;
    bool yumiIsStarting;
    bool pendingDragAndDrop;

    void updateStyles();
    QPoint getCenter();
    QPoint getRelativeCenter();
    void doResizeWidth(const int width);
    void doResizeHeight(const int height);
    void doResize(const int width, const int height);
    void setSelectedGame(GameInfo* currentGame);
    void showStatusBarMsg(const QString& text);
    GameInfo* getGameInfo(const QString& gameName, const QString& gamePath);
    GameInfo* getGameInfo(const QString& gameName);
    YumiSettings* getSettingsWidget();
    void showLicense();
    void showGetLatestVersionResult(const bool success, const QString& msg, const bool isStartup);
    void forceRefreshModsMonitoring();
    void showInstallingModWindow(const bool hide);

public slots:
    void gameFolderOpen();
    void changeSettings();
    void startupCheckForLatestVersion();
    void checkForLatestVersion();
    void checkForModsToInstall();
    void createInstallingModWindow();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    QAction* addWindowAction(const QIcon& icon, const QString& statusTip, const char* slot);
private:
    yumi(QWidget *parent);
    yumi();
    int codeMetrics_calcNbLines();
    void setupYumiActions();
    void setupHelpActions();
    void setupWindowActions();
    std::tuple<QString, QDateTime> getMonitoredElem(const QString& toSearch);
    void updateCacheMonitoredFolder(const QString& modsFolderPath, const QDir& modsFolder);
    bool hasFileChange(const QString& modsFolderPath, const QDir& modsFolder);
    void monitorModsFolder();

    About* _aboutWidget;
    LicenseViewer* _licenseWidget;
    DebugLogs* _debugLogsWidget;
    YumiSettings* _settingsWidget;
    QPixmap* _appIcon;
    QLabel* _menuBarAppIcon;
    QVBoxLayout* _menuBarAppIconLayout;
    QWidget* _menuBarAppIconWidget;
    QMenu* _windowMenu;
    QMenuBar* _windowButtons;
    QHBoxLayout* _windowButtonsLayout;
    QWidget* _windowButtonsWidget;

    QAction* _minimizeAction;
    QAction* _showMaximizedAction;
    QAction* _showNormalAction;
    QAction* _closeAction;

    bool _isMaximized;
    bool _windowMoving;
    bool _pointerEnabled;
    QPoint _mousePos;

    QFuture<void> _modsMonitoring;
    bool _monitoringInitialized;
    bool _monitoringIsRunning;
    QVector<std::tuple<QString, QDateTime>> _monitoredFolder;

    InstallingModWindow* installingModWindow;
    bool _showInstallingModWindow;

private slots:
    void wiki();
    void discord();
    void logs();
    void about();
    void doMaximizeWindow();

signals:

};

#endif
