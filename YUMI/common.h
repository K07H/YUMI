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

#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef QT_DEBUG
#define IS_DEBUG true
#define CALC_CODE_METRICS true
#define DEBUG_PAINTING false
#define DEBUG_CLICK_EVENTS true
#define IS_DEVELOPMENT true
#else
#define IS_DEBUG false
#define CALC_CODE_METRICS false
#define DEBUG_PAINTING false
#define DEBUG_CLICK_EVENTS false
#define IS_DEVELOPMENT false
#endif
#ifndef QT_NO_SSL
#define QT_NO_SSL
#endif

#define APP_VERSION "1.0.5"
#if IS_DEVELOPMENT
#define YUMI_BEPINEX_VERSION_CHECK_URL "http://osubmarin.fr/yumi/latest_bepinex_version_dev.php"
#define YUMI_VERSION_CHECK_URL "http://osubmarin.fr/yumi/latest_version_dev.php"
#else
#define YUMI_BEPINEX_VERSION_CHECK_URL "http://osubmarin.fr/yumi/latest_bepinex_version.php"
#define YUMI_VERSION_CHECK_URL "http://osubmarin.fr/yumi/latest_version.php"
#endif

#define RUN_YUMI_SCRIPT_NAME "run_yumi_bepinex"
#define SHOW_LICENSE_AT_STARTUP false
#define YUMI_LICENSE_URL "https://lc-it-solutions.com/resources/yumi-open-source-license"
#define YUMI_SHARE_THEME_URL "http://osubmarin.fr/yumi/share_theme.php"
#define DOWNLOAD_BEPINEX_URL "http://osubmarin.fr/yumi/bepinex/"

#define YUMI_WIKI_URL "https://github.com/K07H/YUMI/wiki"
#define YUMI_REPOSITORY_URL "https://github.com/K07H/YUMI"
#define YUMI_AUTHOR_EMAIL "contact@osubmarin.fr"
#define YUMI_DISCORD_URL "https://discord.gg/WDvjt7csWh"
#define MODDING_DISCORD_URL "https://discord.gg/VAMuXyd"
#define WEBSITE_QT_URL "https://www.qt.io/"
#define WEBSITE_BEPINEX_URL "https://github.com/BepInEx/BepInEx/"

#define UNIX_BEPINEX_NAME "unix_bepinex"
#define WINX64_BEPINEX_NAME "winx64_bepinex"
#define WINX86_BEPINEX_NAME "winx86_bepinex"
#define IL2CPPX64_BEPINEX_NAME "il2cppx64_bepinex"
#define IL2CPPX86_BEPINEX_NAME "il2cppx86_bepinex"

#define DEFAULT_DOWNLOAD_REQUEST_TIMEOUT 600
#define DEFAULT_GET_REQUEST_TIMEOUT 120

#define UNKNOWN_GAME -1
#define WIN_GAME_X64 0
#define WIN_GAME_X86 1
#define UNIX_GAME_X64 2
#define UNIX_GAME_X86 3
#define DARWIN_GAME 4
#define WIN_GAME_IL2CPP_X64 5
#define WIN_GAME_IL2CPP_X86 6

#endif
