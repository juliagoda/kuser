/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
 *  Former maintainer: Adriaan de Groot <groot@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 **/

#include "globals.h"

#include <QApplication>

#include <KLocalizedString>
#include <kiconloader.h>
#include <kconfig.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kdebug.h>

#include "ku_global.h"
#include "ku_misc.h"
#include "ku_mainwidget.h"

#include <klocale.h>
#include <KAboutData>
#include <QApplication>

#include <QMessageBox>
#include <unistd.h>

static const char *description =
	I18N_NOOP("KDE User Editor");

int main(int argc, char **argv) 
{
    QApplication a(argc, argv);
    KAboutData aboutData(
                             QStringLiteral("kuser"),
                             i18n("KUser"),
                             QStringLiteral(_KU_VERSION),
                             i18n(description),
                             KAboutLicense::LGPL,
                             i18n("(c) 1997-2000, Denis Perchine\n(c) 2004, Szombathelyi György"),
                             i18n(""),
                             QStringLiteral("http://kde.org/applications/system/kuser/"));
  
  aboutData.addAuthor(QStringLiteral("Denis Perchine"), QStringLiteral("kuser author"),
    QStringLiteral("dyp@perchine.com"), QStringLiteral("http://www.perchine.com/dyp/"));
  aboutData.addAuthor(QStringLiteral("Szombathelyi György"), QStringLiteral("kuser author"),
    QStringLiteral("gyurco@freemail.hu"));
  KAboutData::setApplicationData(aboutData);
  KU_MainWidget *mw = 0;

  KGlobal::locale()->insertCatalog( QLatin1String( "libkldap" ));
  KConfigGroup group( KSharedConfig::openConfig(), "general" );
  KU_Global::initCfg( group.readEntry( "connection", "default" ) );

  if(getuid()) {
    QMessageBox::critical(0, i18n("KDE User Manager"), i18n("This application needs to be run with root privileges."), QMessageBox::Ok, QMessageBox::Ok);
    return 1;
  }

  mw = new KU_MainWidget();
  mw->setCaption(i18n("KDE User Manager"));
  mw->show();

  a.exec();

  KU_Global::kcfg()->writeConfig();
}
