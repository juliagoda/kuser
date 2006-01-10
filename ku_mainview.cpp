/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
 *  Former maintainer: Adriaan de Groot <groot@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License version 2 as published by the Free Software Foundation.
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


#include <stdio.h>

#include <qtooltip.h>
#include <qfile.h>
//Added by qt3to4:
#include <Q3PtrList>

#include <kinputdialog.h>
#include <ktoolbar.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include "ku_misc.h"
#include "ku_edituser.h"
#include "ku_adduser.h"
#include "ku_deluser.h"
#include "ku_pwdlg.h"
#include "ku_editgroup.h"
#include "ku_global.h"

#include "ku_mainview.h"

KU_MainView::KU_MainView(QWidget *parent) : QTabWidget(parent)
{
  init();
}

void KU_MainView::init() {

  lbusers = new KUserView( this, "lbusers" );
  addTab( lbusers, i18n("Users") );

  lbgroups = new KGroupView( this, "lbgroups" );
  addTab( lbgroups, i18n("Groups"));

  connect(lbusers, SIGNAL(doubleClicked(Q3ListViewItem *)), this, SLOT(userSelected()));
  connect(lbusers, SIGNAL(returnPressed(Q3ListViewItem *)), this, SLOT(userSelected()));

  connect(lbgroups, SIGNAL(doubleClicked(Q3ListViewItem *)), this, SLOT(groupSelected()));
  connect(lbgroups, SIGNAL(returnPressed(Q3ListViewItem *)), this, SLOT(groupSelected()));

  connect(this, SIGNAL(currentChanged(QWidget *)), this, SLOT(slotTabChanged()));
}

KU_MainView::~KU_MainView()
{
}

void KU_MainView::slotTabChanged()
{
  if (currentPage() == lbusers)
  {
     emit userSelected(true);
     emit groupSelected(false);
  }
  else
  {
     emit userSelected(false);
     emit groupSelected(true);
  }
}

void KU_MainView::clearUsers()
{
  lbusers->clear();
}

void KU_MainView::clearGroups()
{
  lbgroups->clear();
}

void KU_MainView::reloadUsers()
{
  KU_User *ku;

  lbusers->clear();
  lbusers->init();
  uid_t uid = kug->kcfg()->firstUID();

  ku = kug->getUsers().first();
  while ( ku ) {
    if ( ku->getUID() >= uid || mShowSys ) lbusers->insertItem( ku );
    ku = kug->getUsers().next();
  }
  if (lbusers->firstChild())
    lbusers->setSelected(lbusers->firstChild(), true);
}

void KU_MainView::reloadGroups()
{
  KU_Group *kg;

  lbgroups->clear();
  lbgroups->init();
  gid_t gid = kug->kcfg()->firstGID();

  kg = kug->getGroups().first();
  while ( kg ) {
    if ( kg->getGID() >= gid || mShowSys ) lbgroups->insertItem(kg);
    kg = kug->getGroups().next();
  }
}

void KU_MainView::useredit()
{
  userSelected();
}

void KU_MainView::userdel()
{
  KU_User *user = lbusers->getCurrentUser();
  if (!user)
    return;

  QString username = user->getName();
  gid_t gid = user->getGID();
  KU_DelUser dlg(user, this);

  if ( dlg.exec() == QDialog::Rejected )
     return;

  user->setDeleteHome( dlg.getDeleteHomeDir() );
  user->setDeleteMailBox( dlg.getDeleteMailBox() );

  kug->getUsers().del( user );
  if ( !updateUsers() ) return;

  KU_Group *group = 0;
  group = kug->getGroups().first();
  while ( group ) {
    kdDebug() << "group: " << group->getName() << endl;
    if ( group->lookup_user( username ) ) {
      kdDebug() << "group: " << group->getName() << " found user: " << username << endl;
      KU_Group newgroup( group );
      newgroup.removeUser( username );
      kug->getGroups().mod( group, newgroup );
    }
    group = kug->getGroups().next();
  }

  if ( kug->kcfg()->userPrivateGroup() ) {

    group = kug->getGroups().lookup( gid );

    if ( group &&
      KMessageBox::questionYesNo( 0, i18n("You are using private groups.\n"
        "Do you want to delete the user's private group '%1'?")
        .arg(group->getName()), QString::null,
        KStdGuiItem::del(), i18n("Do Not Delete")) == KMessageBox::Yes) {
      kdDebug() << "del private group" << endl;
      kug->getGroups().del( group );
    }
  }
  kdDebug() << "update groups" << endl;
  updateGroups();
}

void KU_MainView::useradd()
{
  KU_User *tk;

  showPage(lbusers);

  uid_t uid, rid = 0;
  bool samba = kug->getUsers().getCaps() & KU_Users::Cap_Samba;

  if ((uid = kug->getUsers().first_free()) == KU_Users::NO_FREE) {
    KMessageBox::sorry( 0, i18n("You have run out of uid space.") );
    return;
  }
/*
  if ( samba && (rid = kug->getUsers().first_free_sam()) == 0) {
    KMessageBox::sorry( 0, i18n("You have run out of user RID space.") );
    return;
  }
*/
  if ( samba ) rid = SID::uid2rid( uid );
  bool ok;
  QString name = KInputDialog::getText( QString::null,
    i18n("Please type the name of the new user:"),
    QString::null, &ok );

  if ( !ok ) return;

  if ( kug->getUsers().lookup( name ) ) {
    KMessageBox::sorry( 0, i18n("User with name %1 already exists.").arg( name ) );
    return;
  }

  tk = new KU_User();
  tk->setCaps( samba ? KU_User::Cap_POSIX | KU_User::Cap_Samba : KU_User::Cap_POSIX );
  tk->setUID( uid );
  tk->setName( name );

  if ( samba ) {
    SID sid;
    sid.setDOM( kug->getUsers().getDOMSID() );
    sid.setRID( rid );
    tk->setSID( sid );
    tk->setProfilePath( kug->kcfg()->samprofilepath().replace( "%U",name ) );
    tk->setHomePath( kug->kcfg()->samhomepath().replace( "%U", name ) );
    tk->setHomeDrive( kug->kcfg()->samhomedrive() );
    tk->setLoginScript( kug->kcfg()->samloginscript() );
    tk->setDomain( kug->kcfg()->samdomain() );
  }

  tk->setShell( kug->kcfg()->shell() );
  tk->setHomeDir( kug->kcfg()->homepath().replace( "%U", name ) );
  if ( kug->getUsers().getCaps() & KU_Users::Cap_Shadow || samba ) {
    tk->setLastChange( now() );
  }

  tk->setMin( kug->kcfg()->smin() );
  tk->setMax( kug->kcfg()->smax() );
  tk->setWarn( kug->kcfg()->swarn() );
  tk->setInactive( kug->kcfg()->sinact() );
  tk->setExpire( kug->kcfg()->sneverexpire() ? (uint) -1 :
    (kug->kcfg()->sexpire()).toTime_t() );

  bool privgroup = kug->kcfg()->userPrivateGroup();

  if ( !privgroup ) tk->setGID( kug->kcfg()->defaultgroup() );

  KU_AddUser au( tk, privgroup, this );

  au.setCreateHomeDir( kug->kcfg()->createHomeDir() );
  au.setCopySkel( kug->kcfg()->copySkel() );

  if ( au.exec() == QDialog::Rejected ) {
    delete tk;
    return;
  }
  if ( privgroup ) {
    KU_Group *tg;

    if ((tg = kug->getGroups().lookup(tk->getName())) == 0) {
      gid_t gid;
      if ( kug->getGroups().lookup( tk->getUID() ) == 0 ) {
        gid = tk->getUID();
      } else {
        gid = kug->getGroups().first_free();
      }
      kdDebug() << "private group GID: " << gid << endl;
      uid_t rid = 0;
//      if ( samba ) rid = kug->getGroups().first_free_sam();
      if ( samba ) rid = SID::gid2rid( gid );
      if ( gid == KU_Groups::NO_FREE || ( samba && rid == 0 ) ) {
        kug->getGroups().cancelMods();
        delete tk;
        return;
      }
      tg = new KU_Group();
      tg->setGID( gid );
      if ( samba && ( tk->getCaps() & KU_User::Cap_Samba ) ) {
        SID sid;
        sid.setDOM( kug->getGroups().getDOMSID() );
        sid.setRID( rid );
        tg->setSID( sid );
        tg->setDisplayName( tk->getName() );
        tg->setCaps( KU_Group::Cap_Samba );
      }
      tg->setName( tk->getName() );
      kug->getGroups().add( tg );
    }
    tk->setGID( tg->getGID() );
    tk->setPGSID( tg->getSID() );
  }
  kug->getUsers().add( tk );
  if ( !updateUsers() ) {
    kug->getGroups().cancelMods();
    return;
  }
  updateGroups();
}

bool KU_MainView::queryClose()
{
  return true;
}

void KU_MainView::setpwd()
{
  int count = lbusers->selectedItems().count();
  if ( count == 0 ) return;
  if ( count > 1 ) {
    if ( KMessageBox::questionYesNo( 0,
      i18n("You have selected %1 users. Do you really want to change the password for all the selected users?")
		.arg( count ), QString::null, i18n("Change"), i18n("Do Not Change") ) == KMessageBox::No ) return;
  }
  KU_PwDlg d( this );
  if ( d.exec() != QDialog::Accepted ) return;

  KU_User newuser, *user;
  Q3ListViewItem *item;

  item = lbusers->firstChild();
  while ( item ) {
    if ( item->isSelected() ) {
      user = ((KUserViewItem*) item)->user();
      newuser.copy( user );
      kug->getUsers().createPassword( &newuser, d.getPassword() );
      newuser.setLastChange( now() );
      newuser.setDisabled( false );
      kug->getUsers().mod( user, newuser );
    }
    item = item->nextSibling();
  }
  updateUsers();
}

void KU_MainView::groupSelected()
{
  bool samba = kug->getGroups().getCaps() & KU_Groups::Cap_Samba;
  KU_Group *tmpKG = lbgroups->getCurrentGroup();
  if ( !tmpKG ) return;
  KU_Group newGroup( tmpKG );

  kdDebug() << "The SID for group " << newGroup.getName() << " is: '" << newGroup.getSID().getSID() << "'" << endl;
  if ( samba && ( newGroup.getCaps() & KU_Group::Cap_Samba ) && 
      newGroup.getSID().isEmpty() ) {
    SID sid;
    sid.setDOM( kug->getGroups().getDOMSID() );
//    sid.setRID( kug->getGroups().first_free_sam() );
    sid.setRID( SID::gid2rid( newGroup.getGID() ) );
    newGroup.setSID( sid );
    kdDebug() << "The new SID for group " << newGroup.getName() << " is: " << sid.getSID() << endl;
  }
  KU_EditGroup egdlg( &newGroup, samba, false );

  if ( egdlg.exec() == QDialog::Accepted ) {
    kug->getGroups().mod( tmpKG, newGroup );
    updateGroups();
  }
}

void KU_MainView::userSelected()
{
  Q3ListViewItem *item;
  Q3PtrList<KU_User> ulist;

  item = lbusers->firstChild();
  while ( item ) {
    if ( item->isSelected() ) {
      ulist.append( ((KUserViewItem*) item)->user() );
    }
    item = item->nextSibling();
  }
  if ( ulist.isEmpty() ) return;

  KU_EditUser editUser( ulist, this );
  if ( editUser.exec() == QDialog::Rejected ) return;

  KU_User *user, newuser;
  user = ulist.first();
  while ( user ) {
    editUser.mergeUser( user, &newuser );
    kug->getUsers().mod( user, newuser );
    user = ulist.next();
  }
  updateUsers();
  updateGroups();
}

void KU_MainView::grpadd()
{
  showPage(lbgroups);

  gid_t gid;
  uid_t rid = 0;
  bool samba;

  samba = kug->getGroups().getCaps() & KU_Groups::Cap_Samba;

  if ( (gid = kug->getGroups().first_free()) == KU_Groups::NO_FREE )
  {
    KMessageBox::sorry( 0, i18n("You have run out of gid space.") );
    return;
  }
/*
  if ( samba && (rid = kug->getGroups().first_free_sam()) == 0 )
  {
    KMessageBox::sorry( 0, i18n("You have run out of group RID space.") );
    return;
  }
*/
  if ( samba ) rid = SID::gid2rid( gid );

  KU_Group *tk = new KU_Group();
  tk->setGID(gid);
  if ( samba ) {
    SID sid;
    sid.setRID( rid );
    sid.setDOM( kug->getGroups().getDOMSID() );
    tk->setSID( sid );
  }
  KU_EditGroup egdlg( tk, samba, true );

  if ( egdlg.exec() == QDialog::Rejected ) {
    delete tk;
    return;
  }
  kug->getGroups().add(tk);
  updateGroups();
}

void KU_MainView::grpedit()
{
  groupSelected();
}

void KU_MainView::grpdel()
{
  Q3ListViewItem *item;
  KU_Group *group = NULL;
  int selected = 0;

  item = lbgroups->firstChild();
  while ( item ) {
    if ( item->isSelected() ) {

      selected++;
      group = ((KGroupViewItem*) item)->group();

      KU_User *user = kug->getUsers().first();
      while ( user ) {
        if ( user->getGID() == group->getGID() ) {
          KMessageBox::error( 0, i18n( "The group '%1' is the primary group of one or more users (such as '%2'); it cannot be deleted." ).arg( group->getName() ).arg( user->getName() ) );
          return;
        }
        user = kug->getUsers().next();
      }
    }
    item = item->nextSibling();
  }

  switch ( selected ) {
    case 0: return;
    case 1:
      if (KMessageBox::warningContinueCancel( 0,
        i18n("Do you really want to delete the group '%1'?").arg(group->getName()),
        QString::null, KStdGuiItem::del()) != KMessageBox::Continue) return;
      break;
    default:
      if (KMessageBox::warningContinueCancel( 0,
        i18n("Do you really want to delete the %1 selected groups?").arg(selected),
        QString::null, KStdGuiItem::del()) != KMessageBox::Continue) return;
  }

  item = lbgroups->firstChild();
  while ( item ) {
    if ( item->isSelected() ) {
      group = ((KGroupViewItem*) item)->group();
      kug->getGroups().del( group );
    }
    item = item->nextSibling();
  }
  updateGroups();
}

bool KU_MainView::updateGroups()
{
  bool ret;
  kdDebug() << "updateGroups() " << endl;
  ret = kug->getGroups().dbcommit();

  KU_Group *group;
  KU_Groups::DelIt dit( kug->getGroups().mDelSucc );
  KU_Groups::AddIt ait( kug->getGroups().mAddSucc );

  while ( (group = dit.current()) != 0 ) {
    ++dit;
    lbgroups->removeItem( group );
  }
  while ( (group = ait.current()) != 0 ) {
    ++ait;
    lbgroups->insertItem( group );
  }
  kdDebug() << "commit groups" << endl;
  kug->getGroups().commit();
  return ret;
}

bool KU_MainView::updateUsers()
{
  bool ret;
  kdDebug() << "updateUsers() " << endl;
  ret = kug->getUsers().dbcommit();

  KU_User *user;
  KU_Users::DelIt dit( kug->getUsers().mDelSucc );
  KU_Users::AddIt ait( kug->getUsers().mAddSucc );

  while ( (user = dit.current()) != 0 ) {
    ++dit;
    lbusers->removeItem( user );
  }
  while ( (user = ait.current()) != 0 ) {
    ++ait;
    lbusers->insertItem( user );
  }
  kug->getUsers().commit();
  return ret;
}

#include "ku_mainview.moc"
