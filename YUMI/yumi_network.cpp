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
#include <QtDebug>
#include <QMessageBox>
#include <QUrl>
#include <QDesktopServices>
#include <QtNetwork/QNetworkRequest>
#include "yumi_network.h"
#include "common.h"
#include "yumi.h"

YumiNetwork* YumiNetwork::_instance = NULL;

YumiNetwork::YumiNetwork(void* yumiPtr)
{
    this->_yumiPtr = yumiPtr;
    this->_manager = new QNetworkAccessManager((yumi*)this->_yumiPtr);
    connect(this->_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
#ifndef QT_NO_SSL
    connect(this->_manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(requestSslError(QNetworkReply*,QList<QSslError>)));
#endif
    this->isShareThemeRequest = false;
    this->isGetLatestVersionRequest = false;
    this->isStartupGetLatestVersionRequest = false;
}

YumiNetwork::~YumiNetwork()
{
    if (this->_manager != NULL)
    {
        delete this->_manager;
        this->_manager = NULL;
    }
}

YumiNetwork* YumiNetwork::Init(void* yumiPtr)
{
    if (_instance == NULL)
        _instance = new YumiNetwork(yumiPtr);
    return _instance;
}

YumiNetwork* YumiNetwork::Instance()
{
    return _instance;
}

void YumiNetwork::openLink(const QString& link, QWidget* msgBoxParent)
{
#if IS_DEBUG && DEBUG_CLICK_EVENTS
    qDebug().nospace() << "Link clicked: " << link;
#endif
    if (link.isEmpty())
        return;
    if (link.compare(YUMI_LICENSE_URL) == 0)
        ((yumi*)_yumiPtr)->showLicense();
    else
        if (!QDesktopServices::openUrl(QUrl(link)))
        {
            QMessageBox cannotOpenUrl(QMessageBox::Warning, QCoreApplication::translate("MainWindow", "Unable to open URL", "Popup title"), QCoreApplication::translate("MainWindow", "No valid application was found to open URL \"%1\".", "Popup text").arg(link), QMessageBox::Ok, msgBoxParent);
            cannotOpenUrl.exec();
        }
}

void YumiNetwork::requestFinished(QNetworkReply* reply)
{
    QString failureMsg = "";
    if (reply != NULL)
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            bool isError = true;
            QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            if (statusCode.isValid())
            {
                bool convertOk;
                int httpStatusCode = statusCode.toInt(&convertOk);
                if (convertOk && httpStatusCode == 200)
                    isError = false;
            }
            if (!isError)
            {
                QString successMsg = reply->readAll();
                qDebug() << "Web request success:" << successMsg;
                if (this->isShareThemeRequest)
                {
                    this->isShareThemeRequest = false;
                    ((yumi*)_yumiPtr)->getSettingsWidget()->showSharingResult(true, successMsg);
                }
                if (this->isStartupGetLatestVersionRequest)
                {
                    this->isStartupGetLatestVersionRequest = false;
                    ((yumi*)_yumiPtr)->showGetLatestVersionResult(true, successMsg, true);
                }
                if (this->isGetLatestVersionRequest)
                {
                    this->isGetLatestVersionRequest = false;
                    ((yumi*)_yumiPtr)->showGetLatestVersionResult(true, successMsg, false);
                }
            }
            else
                failureMsg = reply->readAll();
        }
        else
            failureMsg = reply->errorString();
        reply->deleteLater();
    }
    else
        failureMsg = "No network reply.";

    if (!failureMsg.isEmpty())
    {
        qDebug() << "Web request failure:" << failureMsg;
        if (this->isShareThemeRequest)
        {
            this->isShareThemeRequest = false;
            ((yumi*)_yumiPtr)->getSettingsWidget()->showSharingResult(false, failureMsg);
        }
        if (this->isStartupGetLatestVersionRequest)
        {
            this->isStartupGetLatestVersionRequest = false;
            ((yumi*)_yumiPtr)->showGetLatestVersionResult(false, failureMsg, true);
        }
        if (this->isGetLatestVersionRequest)
        {
            this->isGetLatestVersionRequest = false;
            ((yumi*)_yumiPtr)->showGetLatestVersionResult(false, failureMsg, false);
        }
    }
}

#ifndef QT_NO_SSL
void YumiNetwork::requestSslError(QNetworkReply* reply, QList<QSslError> errors)
{
    QString errorString = "";
    if (errors.count() > 0)
        for (const QSslError& error : errors)
        {
            if (!errorString.isEmpty())
                errorString += '\n';
            errorString += error.errorString();
        }
    qWarning() << "Ignoring web request SSL error:" << errorString;
    if (reply != NULL)
        reply->ignoreSslErrors();
}
#endif

void YumiNetwork::sendJsonPostRequest(const QString& url, const QString& jsonData)
{
    QByteArray postData = jsonData.toUtf8();
    QNetworkRequest request = QNetworkRequest(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");
    this->_manager->post(request, postData);
}

void YumiNetwork::sendGetRequest(const QString& url)
{
    QNetworkRequest request = QNetworkRequest(QUrl(url));
    this->_manager->get(request);
}
