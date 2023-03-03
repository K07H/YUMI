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
#include <QDir>
#include <QDebug>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QTimer>
#include <QThread>
#include "common.h"
#include "assets.h"
#include "yumi.h"
#include "utils.h"
#include "mods_loader.h"

QString yumi::appPath = QDir::currentPath();
int yumi::initialWindowWidth = 600;
int yumi::initialWindowHeight = 350;

yumi::yumi(QWidget *parent) : QMainWindow(parent)
{
    this->_yumiIsStarting = true;
    qDebug() << "YUMI is starting.";

    this->_aboutWidget = NULL;
    this->_licenseWidget = NULL;
    this->_settingsWidget = NULL;
    this->_windowButtons = NULL;
    this->_windowButtonsLayout = NULL;
    this->_windowButtonsWidget = NULL;
    this->_minimizeAction = NULL;
    this->_showMaximizedAction = NULL;
    this->_showNormalAction = NULL;
    this->_closeAction = NULL;
    this->selectedGame = NULL;
    this->_isMaximized = false;
    this->_windowMoving = false;
    this->_pointerEnabled = false;
    this->_monitoringInitialized = false;
    this->_monitoringIsRunning = true;
    this->theme = "Default";

    setWindowTitle("YUMI");
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);

    Assets::Init(this);

    this->mainWidget = new MainWidget(this);
    setCentralWidget(this->mainWidget);

    this->configuration = Config::Init(this);
    this->configuration->loadConfig();

    this->network = YumiNetwork::Init(this);

    this->gameLoader = GameLoader::Init(this);

    setToolButtonStyle(Qt::ToolButtonFollowStyle);

    menuBar()->installEventFilter(this);
    menuBar()->setStyleSheet(Assets::Instance()->menuBarStyle);

    setupYumiActions();
    setupHelpActions();
    setupWindowActions();

    statusBar()->setStyleSheet(Assets::Instance()->statusBarStyle);
    showStatusBarMsg(QCoreApplication::translate("MainWindow", "Welcome :)", "Tooltip text"));

    if (this->theme.compare("Default") != 0)
    {
        YumiSettings* settings = this->getSettingsWidget();
        if (settings != NULL)
            settings->updateTheme(this->theme);
    }

    bool gamesFound = (this->gamesInfo.count() > 0);
    if (gamesFound)
        QTimer::singleShot(2000, this, &yumi::checkForModsToInstall);

#if SHOW_LICENSE_AT_STARTUP
    if (!this->configuration->checkedNoticesAndLicense)
        QTimer::singleShot(500, this, &yumi::showLicense);
#endif

    if (this->configuration->checkVersionAtStartup)
        QTimer::singleShot(1000, this, &yumi::startupCheckForLatestVersion);

    this->_modsMonitoring = QtConcurrent::run(this, &yumi::monitorModsFolder);

#if IS_DEBUG && CALC_CODE_METRICS
    codeMetrics_calcNbLines();
#endif

    if (!gamesFound)
        this->_yumiIsStarting = false;
}

yumi::~yumi()
{
    if (this->_modsMonitoring.isRunning())
    {
        this->_monitoringIsRunning = false;
        this->_modsMonitoring.waitForFinished();
    }
}

#if IS_DEBUG && CALC_CODE_METRICS
int yumi::codeMetrics_calcNbLines()
{
    QDir sourcesFolder(yumi::appPath);
    if (!sourcesFolder.exists())
        return -1;
    int folderNameSttPos = yumi::appPath.indexOf("build-");
    if (folderNameSttPos < 0 || yumi::appPath.length() <= folderNameSttPos + 6)
        return -1;
    folderNameSttPos += 6;
    int folderNameEndPos = yumi::appPath.indexOf("-", folderNameSttPos);
    if (folderNameEndPos < 0 || folderNameEndPos < folderNameSttPos)
        return -1;
    QString sourcesFolderName = yumi::appPath.mid(folderNameSttPos, folderNameEndPos - folderNameSttPos);
    if (sourcesFolderName.isEmpty())
        return -1;
    if (!sourcesFolder.cdUp())
        return -1;
    if (!sourcesFolder.cd(sourcesFolderName))
        return -1;
    QStringList files = sourcesFolder.entryList(QStringList() << "*.cpp" << "*.h", QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    int nbFiles = 0;
    int realLineCount = 0;
    int bracketLineCount = 0;
    for (const QString& file : files)
    {
        QFile current(Utils::toUnixPath(sourcesFolder.absolutePath() + QDir::separator() + file));
        if (current.exists())
        {
            if (current.open(QIODevice::ReadOnly | QIODevice::ExistingOnly | QIODevice::Text))
            {
                try
                {
                    QTextStream in(&current);
                    QString line;
                    bool commentOpened = false;
                    while (!in.atEnd())
                    {
                        line = in.readLine();
                        line = line.remove(' ').remove('\t').remove('\r').remove('\n');
                        if (line.startsWith("/*"))
                            commentOpened = true;
                        if (!commentOpened && !line.isEmpty() && !line.startsWith("//"))
                        {
                            if (line.compare("{") == 0 || line.compare("}") == 0)
                                ++bracketLineCount;
                            else
                                ++realLineCount;
                        }
                        if (commentOpened && line.startsWith("*/"))
                            commentOpened = false;
                    }
                }
                catch (...) { }
                current.close();
            }
        }
        ++nbFiles;
    }
    qDebug().nospace() << "Code metrics: Found " << realLineCount << " lines of code (" << (realLineCount + bracketLineCount) <<" if counting scope's braces) in " << nbFiles << " files.";
    return realLineCount;
}
#endif

void yumi::monitorModsFolder()
{
    while (this->_monitoringIsRunning)
    {
        QCoreApplication::processEvents();

        if (!this->_yumiIsStarting && !ModsLoader::Instance()->installInProgress)
        {
            QString modsFolderPath = Utils::toUnixPath(yumi::appPath + QDir::separator() + "mods");
            QDir modsFolder(modsFolderPath);
            if (modsFolder.exists())
            {
                if (!this->_monitoringInitialized)
                    this->_monitoringInitialized = true;
                else
                {
                    QStringList current = modsFolder.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
                    bool hasChanged = (current.count() != this->_monitoredFolder.count());
                    if (!hasChanged)
                    {
                        foreach (const QString& fileOrFolder, current)
                            if (!this->_monitoredFolder.contains(fileOrFolder))
                            {
                                hasChanged = true;
                                break;
                            }
                    }
                    if (hasChanged)
                    {
#if IS_DEBUG
                        qDebug() << "Mods folder content has changed!";
#endif
                        QMetaObject::invokeMethod(this, "checkForModsToInstall", Qt::AutoConnection);
                    }
                }
                this->_monitoredFolder = modsFolder.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
            }
        }

        QThread::currentThread()->msleep(3000);
    }
}

YumiSettings* yumi::getSettingsWidget()
{
    if (this->_settingsWidget == NULL)
        this->_settingsWidget = new YumiSettings(this, NULL);
    return this->_settingsWidget;
}

void yumi::updateStyles()
{
    menuBar()->setStyleSheet(Assets::Instance()->menuBarStyle);
    statusBar()->setStyleSheet(Assets::Instance()->statusBarStyle);
    this->_windowButtons->setStyleSheet(Assets::Instance()->windowBtnsMenuBarStyle);
    this->mainWidget->updateStyles();
    if (this->_aboutWidget != NULL)
        this->_aboutWidget->updateStyles();
    update();
}

QPoint yumi::getCenter()
{
    return QPoint(this->pos().x() + (this->width() / 2), this->pos().y() + (this->height() / 2));
}

QPoint yumi::getRelativeCenter()
{
    return QPoint(this->width() / 2, this->height() / 2);
}

void yumi::showLicense()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (this->_licenseWidget == NULL)
        this->_licenseWidget = new LicenseViewer(NULL);
    if (this->_licenseWidget->isVisible())
        this->_licenseWidget->close();
    this->_licenseWidget->doShowAt(getCenter());
}

void yumi::checkForModsToInstall()
{
    ModsLoader::Instance()->installMods(this);
    this->_yumiIsStarting = false;
    this->_monitoringInitialized = false;
}

void yumi::startupCheckForLatestVersion()
{
    this->network->isStartupGetLatestVersionRequest = true;
    this->network->sendGetRequest(YUMI_VERSION_CHECK_URL);
}

void yumi::checkForLatestVersion()
{
    this->network->isGetLatestVersionRequest = true;
    this->network->sendGetRequest(YUMI_VERSION_CHECK_URL);
}

void yumi::showGetLatestVersionResult(const bool success, const QString& msg, const bool isStartup)
{
    QString latestVersionUrl = QString(YUMI_REPOSITORY_URL) + "/releases/latest";
    if (success)
    {
        QString currentVersion(APP_VERSION);
        if (currentVersion.compare(msg) != 0)
        {
            QMessageBox notLatestVersion = QMessageBox(QMessageBox::Information, QCoreApplication::translate("MainWindow", "Please update YUMI", "Popup title"), QCoreApplication::translate("MainWindow", "You do not have the latest YUMI version. Your version is v%1 and latest version is v%2.\n\nLatest YUMI version is available at:\n%3\nOpen this URL in your web browser?", "Popup text").arg(APP_VERSION, msg, latestVersionUrl), QMessageBox::Yes | QMessageBox::No, this);
            int btnClicked = notLatestVersion.exec();
            if (btnClicked == QMessageBox::Yes)
                this->network->openLink(latestVersionUrl, this);
        }
        else
        {
            qInfo() << "YUMI version is up-to-date.";
            if (!isStartup)
            {
                QMessageBox isUpToDate = QMessageBox(QMessageBox::Information, QCoreApplication::translate("MainWindow", "YUMI is up-to-date", "Popup title"), QCoreApplication::translate("MainWindow", "YUMI is up-to-date: You have the latest YUMI version (v%1).", "Popup text").arg(msg), QMessageBox::Ok, this);
                isUpToDate.exec();
            }
        }
    }
    else
    {
        QMessageBox failure = QMessageBox(QMessageBox::Warning, QCoreApplication::translate("MainWindow", "Latest version check failure", "Popup title"), QCoreApplication::translate("MainWindow", "Unable to check for latest YUMI version, please check your internet connection. Alternatively you can get the latest version at:\n%1\nOpen this URL in your web browser?\n\nError message:\n%2", "Popup text").arg(latestVersionUrl, msg), QMessageBox::Yes | QMessageBox::No, this);
        int btnClicked = failure.exec();
        if (btnClicked == QMessageBox::Yes)
            this->network->openLink(latestVersionUrl, this);
    }
}

GameInfo* yumi::getGameInfo(const QString& gameName, const QString& gamePath)
{
    if (gameName.isEmpty() || gamePath.isEmpty())
        return NULL;
    int len = this->gamesInfo.count();
    if (len > 0)
        for (int i = 0; i < len; i++)
            if (gameName.compare(this->gamesInfo[i].name) == 0 && gamePath.compare(this->gamesInfo[i].path) == 0)
                return &(this->gamesInfo[i]);
    return NULL;
}

void yumi::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        _mousePos = event->globalPos() - pos();
        _windowMoving = true;
    }
    QWidget::mousePressEvent(event);
}

void yumi::mouseMoveEvent(QMouseEvent* event)
{
    if (_windowMoving && (event->buttons() & Qt::LeftButton))
    {
        event->accept();
        this->move(event->globalPos() - _mousePos);
    }
    else
        QWidget::mouseMoveEvent(event);
}

void yumi::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        _windowMoving = false;
    QWidget::mouseReleaseEvent(event);
}

bool yumi::eventFilter(QObject* watched, QEvent* event)
{
    QEvent::Type eType = event->type();
    if (eType == QEvent::Leave)
    {
        if (this->_pointerEnabled || (QString::compare(watched->metaObject()->className(), "QMenu") == 0))
        {
            this->_pointerEnabled = false;
            this->setCursor(Qt::ArrowCursor);
        }
        return false;
    }
    else if (eType == QEvent::Enter)
    {
        if (QString::compare(watched->metaObject()->className(), "QMenu") == 0)
        {
            this->_pointerEnabled = true;
            this->setCursor(Qt::PointingHandCursor);
        }
        return false;
    }
    bool isMouseButtonPressed = (eType == QEvent::MouseButtonPress);
    bool isMouseButtonReleased = (eType == QEvent::MouseButtonRelease);
    bool isMouseMove = (eType == QEvent::MouseMove);
    if (!(isMouseButtonPressed || isMouseButtonReleased || isMouseMove))
        return false;
    if (watched == menuBar())
    {
        QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
        if (isMouseButtonPressed)
        {
            if (mouse_event->button() == Qt::LeftButton)
            {
                if (menuBar()->actionAt(mouse_event->pos()) != NULL)
                    return false;
                _mousePos = mouse_event->globalPos() - pos();
                _windowMoving = true;
            }
        }
        else if (isMouseButtonReleased)
        {
            if (mouse_event->button() == Qt::LeftButton)
                _windowMoving = false;
        }
        else if (isMouseMove)
        {
            if (_windowMoving && (mouse_event->buttons() & Qt::LeftButton))
                this->move(mouse_event->globalPos() - _mousePos);
            else
            {
                if (menuBar()->actionAt(mouse_event->pos()) != NULL)
                {
                    this->_pointerEnabled = true;
                    this->setCursor(Qt::PointingHandCursor);
                }
                else if (this->_pointerEnabled)
                {
                    this->_pointerEnabled = false;
                    this->setCursor(Qt::ArrowCursor);
                }
            }
        }
    }
    return false;
}

void yumi::resizeEvent(QResizeEvent* event)
{
    this->mainWidget->sideMenu->computeMaxDimensions();
    QWidget::resizeEvent(event);
}

void yumi::closeEvent(QCloseEvent* event)
{
    if (this->configuration->configChanged)
        this->configuration->saveConfig();
    qDebug() << "YUMI is closing.";
    event->accept();
}

void yumi::doMaximizeWindow()
{
    if (this->_isMaximized)
        showNormal();
    else
        showMaximized();
    this->_isMaximized = !this->_isMaximized;
    this->_showMaximizedAction->setVisible(!this->_isMaximized);
    this->_showNormalAction->setVisible(this->_isMaximized);
    this->setStatusTip(" ");
}

void yumi::setupYumiActions()
{
    _menuBarAppIcon = new QLabel("");
    _menuBarAppIcon->setPixmap(Assets::Instance()->appIconImg);
    menuBar()->setCornerWidget(_menuBarAppIcon, Qt::TopLeftCorner);

    QMenu* menu = menuBar()->addMenu(QCoreApplication::translate("MainWindow", "&YUMI", "Menu button text"));

    QAction* action = menu->addAction(Assets::Instance()->openIcon, QCoreApplication::translate("MainWindow", "&Add a game", "Menu button text"), this, &yumi::gameFolderOpen);
    action->setShortcut(QKeySequence::Open);
    action->setStatusTip(" " + QCoreApplication::translate("MainWindow", "Add a new game to the games list.", "Tooltip text"));
    action = menu->addAction(Assets::Instance()->settingsIcon, QCoreApplication::translate("MainWindow", "&Settings", "Menu button text"), this, &yumi::changeSettings);
    action->setShortcut(Qt::CTRL | Qt::Key_S);
    action->setStatusTip(" " + QCoreApplication::translate("MainWindow", "Modify YUMI settings.", "Tooltip text"));
    menu->addSeparator();

    action = menu->addAction(Assets::Instance()->exitIcon, QCoreApplication::translate("MainWindow", "&Quit", "Menu button text"), this, &QWidget::close);
    action->setShortcut(Qt::CTRL | Qt::Key_Q);
    action->setStatusTip(" " + QCoreApplication::translate("MainWindow", "Close YUMI.", "Tooltip text"));

    menu->setCursor(Qt::PointingHandCursor);
    menu->installEventFilter(this);
}

void yumi::setupHelpActions()
{
    QMenu* helpMenu = menuBar()->addMenu(QCoreApplication::translate("MainWindow", "&Help", "Menu button text"));

    QAction* action = helpMenu->addAction(Assets::Instance()->bookIcon, QCoreApplication::translate("MainWindow", "&Wiki", "Menu button text"), this, &yumi::wiki);
    action->setShortcut(Qt::CTRL | Qt::Key_W);
    action->setStatusTip(" " + QCoreApplication::translate("MainWindow", "Open YUMI's wiki web page.", "Tooltip text"));

    action = helpMenu->addAction(Assets::Instance()->updatesIcon, QCoreApplication::translate("MainWindow", "&Check for updates", "Menu button text"), this, &yumi::checkForLatestVersion);
    action->setShortcut(Qt::CTRL | Qt::Key_C);
    action->setStatusTip(" " + QCoreApplication::translate("MainWindow", "Check if there is a new YUMI version available.", "Tooltip text"));

    action = helpMenu->addAction(Assets::Instance()->infoIcon, QCoreApplication::translate("MainWindow", "&About", "Menu button text"), this, &yumi::about);
    action->setShortcut(Qt::CTRL | Qt::Key_A);
    action->setStatusTip(" " + QCoreApplication::translate("MainWindow", "Get information about this app.", "Tooltip text"));

    helpMenu->setCursor(Qt::PointingHandCursor);
    helpMenu->installEventFilter(this);
}

QAction* yumi::addWindowAction(const QIcon& icon, const QString& statusTip, const char* slot)
{
    if (this->_windowButtons == NULL)
        return NULL;
    QAction* action = new QAction(icon, "", this->_windowButtons);
    this->_windowButtons->addAction(action);
    connect(action, SIGNAL(triggered()), this, slot);
    action->setStatusTip(" " + statusTip);
    return action;
}

void yumi::setupWindowActions()
{
    if (this->_windowButtons == NULL)
    {
        this->_windowButtons = new QMenuBar(menuBar());

        this->_minimizeAction = addWindowAction(Assets::Instance()->minimizeIcon, QCoreApplication::translate("MainWindow", "Minimize window.", "Tooltip text"), SLOT(showMinimized()));
        this->_showMaximizedAction = addWindowAction(Assets::Instance()->maximizeIcon, QCoreApplication::translate("MainWindow", "Expand window.", "Tooltip text"), SLOT(doMaximizeWindow()));
        this->_showNormalAction = addWindowAction(Assets::Instance()->showNormalIcon, QCoreApplication::translate("MainWindow", "Shrink window.", "Tooltip text"), SLOT(doMaximizeWindow()));
        this->_showNormalAction->setVisible(false);
        this->_closeAction = addWindowAction(Assets::Instance()->closeIcon, QCoreApplication::translate("MainWindow", "Close YUMI.", "Tooltip text"), SLOT(close()));

        this->_windowButtons->setStyleSheet(Assets::Instance()->windowBtnsMenuBarStyle);
        this->_windowButtons->setCursor(Qt::PointingHandCursor);
        this->_windowButtons->installEventFilter(this);
        this->_windowButtons->setContentsMargins(0, 0, 0, 0);

        this->_windowButtonsLayout = new QHBoxLayout();
        this->_windowButtonsLayout->addWidget(this->_windowButtons);
        this->_windowButtonsLayout->setContentsMargins(0, 0, 9, 0);
        this->_windowButtonsLayout->setSpacing(0);
        this->_windowButtonsWidget = new QWidget();
        this->_windowButtonsWidget->setLayout(this->_windowButtonsLayout);
        menuBar()->setCornerWidget(this->_windowButtonsWidget);
    }
}

void yumi::setSelectedGame(GameInfo* currentGame)
{
    if (this->selectedGame != NULL)
        delete this->selectedGame;
    this->selectedGame = (currentGame == NULL ? NULL : new GameInfo(*currentGame));

#if IS_DEBUG
    if (this->selectedGame == NULL)
        qDebug().nospace() << "Game selection changed: No game selected.";
    else
        qDebug().nospace() << "Game selection changed: " << this->selectedGame->name << " (path: " << this->selectedGame->path << ")";
#endif
    if (this->selectedGame == NULL)
    {
        this->showStatusBarMsg(QCoreApplication::translate("MainWindow", "No game selected.", "Tooltip text"));
        this->mainWidget->toggleGameDetails();
        return;
    }

    this->showStatusBarMsg(QCoreApplication::translate("MainWindow", "Game \"%1\" has been selected.", "Tooltip text").arg(this->selectedGame->name));
    this->mainWidget->gameDetails->updateGameDetails(this->selectedGame);
    this->mainWidget->toggleGameDetails();
}

void yumi::showStatusBarMsg(const QString& text)
{
    statusBar()->showMessage(" " + text);
}

void yumi::gameFolderOpen()
{
    GameLoader::Instance()->openGameFolder();
}

void yumi::changeSettings()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (this->_settingsWidget == NULL)
        this->_settingsWidget = new YumiSettings(this, NULL);
    if (this->_settingsWidget->isVisible())
        this->_settingsWidget->close();
    this->_settingsWidget->doShowAt(getCenter());
}

void yumi::wiki()
{
    this->network->openLink(YUMI_WIKI_URL, this);
}

void yumi::about()
{
    if (this->_aboutWidget == NULL)
        this->_aboutWidget = new About(this, NULL);
    if (this->_aboutWidget->isVisible())
        this->_aboutWidget->close();
    this->_aboutWidget->doShowAt(getCenter());
}
