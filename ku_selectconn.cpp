/*
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
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

#include <QLabel>
#include <QRegExp>
#include <QVBoxLayout>
#include <QObject>
#include <QPushButton>

#include <QDebug>
#include <QLocale>
#include <kmessagebox.h>
#include <QInputDialog>
#include <KLocalizedString>

#include "ku_global.h"
#include "ku_selectconn.h"
#include "ku_configdlg.h"

KU_SelectConn::KU_SelectConn(const QString &selected, QWidget *parent) :
  QDialog( parent)
{
  QStringList conns;
  button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel);
  QPushButton* ok_button = button_box->button(QDialogButtonBox::Ok);

  ok_button->setAutoDefault(true);
  setWindowTitle(i18n("Connection Selection"));

  button_box->addButton(new QPushButton(i18n("&Delete")), QDialogButtonBox::ActionRole);
  button_box->addButton(new QPushButton(i18n("&Edit")), QDialogButtonBox::ActionRole);
  button_box->addButton(new QPushButton(i18n("&New...")), QDialogButtonBox::ActionRole);

  QFrame *page = new QFrame(this);
  QVBoxLayout *topLayout = new QVBoxLayout( page );
  topLayout->setSpacing( 3 );
  QLabel *label = new QLabel( i18n("Defined connections:"), page );
  mCombo = new KComboBox( page );
  mSelected = selected;
  qDebug() << "selected item: " << mSelected;

  conns = KSharedConfig::openConfig()->groupList();
  QStringList::iterator it = conns.begin();
  int i = 0, sel = 0;
  while ( it != conns.end() ) {
    if ( (*it).startsWith( QLatin1String( "connection-" ) ) ) {
      (*it).remove( QRegExp(QLatin1String( "^connection-" )) );
      if ( (*it) == mSelected ) sel = i;
      i++;
      it++;
    } else
      it = conns.erase( it );
  }
  mCombo->insertItems( 0, conns );
  //TODO i18n ?
  if ( mCombo->count() == 0 ) mCombo->addItem( QLatin1String( "default" ) );
  mCombo->setCurrentIndex( sel );
  mSelected = connSelected();
  topLayout->addWidget( label );
  topLayout->addWidget( mCombo );
  topLayout->addWidget(button_box);

  connect( this, SIGNAL(user1Clicked()), SLOT(slotUser1()) );
  connect( this, SIGNAL(user2Clicked()), SLOT(slotUser2()) );
  connect( this, SIGNAL(user3Clicked()), SLOT(slotUser3()) );
  connect( this, SIGNAL(applyClicked()), SLOT(slotApply()));
}

QString KU_SelectConn::connSelected()
{
  return mCombo->currentText();
}

void KU_SelectConn::slotUser3()
{
  newconn = QInputDialog::getText(nullptr, QString(), i18n("Please type the name of the new connection:"));

  if ( newconn.isEmpty() ) return;
  if ( KSharedConfig::openConfig()->groupList().contains( QLatin1String( "connection-" ) + newconn ) ) {
    KMessageBox::sorry( 0, i18n("A connection with this name already exists.") );
    return;
  }

  KSharedConfig::Ptr config( KSharedConfig::openConfig() );
  KU_PrefsBase kcfg( config, newconn );

  KU_ConfigDlg cfgdlg( &kcfg, this );
  connect(&cfgdlg, SIGNAL(settingsChanged(QString)), this, SLOT(slotNewApplySettings()));
  cfgdlg.exec();

  if ( newconn.isEmpty() )
    emit button_box->clicked(button_box->button(QDialogButtonBox::Apply));
}

void KU_SelectConn::slotNewApplySettings()
{
  if ( !newconn.isEmpty() ) {
    mCombo->addItem( newconn );
    mCombo->setCurrentIndex( mCombo->count()-1 );
    mSelected = newconn;
  }
}

void KU_SelectConn::slotUser2()
{
  qDebug() << "slotUser2: " << connSelected();

  KSharedConfig::Ptr config( KSharedConfig::openConfig() );
  KU_PrefsBase kcfg( config, connSelected() );
  kcfg.load();

  KU_ConfigDlg cfgdlg( &kcfg, this );
  connect( &cfgdlg, SIGNAL(settingsChanged(QString)), this, SLOT(slotApplySettings()) );
  cfgdlg.exec();
}

void KU_SelectConn::slotUser1()
{
  QString conn = connSelected();
  if ( KMessageBox::warningContinueCancel( 0, i18n("Do you really want to delete the connection '%1'?",
     conn ),i18n("Delete Connection"),KStandardGuiItem::del() ) == KMessageBox::Cancel ) return;

  KSharedConfig::openConfig()->deleteGroup( QLatin1String( "connection-" ) + conn );
  KSharedConfig::openConfig()->sync();
  mCombo->removeItem( mCombo->currentIndex() );
  if ( mCombo->count() == 0 ) {
      //TODO i18n ?
    mCombo->addItem( QLatin1String( "default" ) );
    mCombo->setCurrentIndex( 0 );
  }
  qDebug() << "slotUser1: " << conn << " " << mSelected;
  if ( mSelected == conn )
    emit button_box->clicked(button_box->button(QDialogButtonBox::Apply));
}

void KU_SelectConn::slotApply()
{
  qDebug() << "slotApply()";
  if ( connSelected() != mSelected ) {
    mSelected = connSelected();
    emit button_box->clicked(button_box->button(QDialogButtonBox::Apply));
  }
}

void KU_SelectConn::slotApplySettings()
{
  qDebug() << "slotApplySettings()";
  if ( connSelected() == mSelected )
    emit button_box->clicked(button_box->button(QDialogButtonBox::Apply));
}
