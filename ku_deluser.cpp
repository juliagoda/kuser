/*
 *  Copyright (c) 2002 Waldo Bastian <bastian@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License version 2 or at your option version 3 as published by
 *  the Free Software Foundation.
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

#include "ku_deluser.h"

#include "globals.h"

#include <QLabel>
#include <QFile>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <KI18n/KLocalizedString>

#include <QLocale>

KU_DelUser::KU_DelUser(KU_User *AUser, QWidget *parent)
 : QDialog( parent)
{
  setWindowTitle(i18n("Delete User"));
  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  QPushButton* ok_button = buttonBox->button(QDialogButtonBox::Ok);
  ok_button->setAutoDefault(true);

  QVBoxLayout *page = new QVBoxLayout(this);
  QLabel* label = new QLabel( i18n("<p>Deleting user <b>%1</b>"
                   "<br />Also perform the following actions:</p>", AUser->getName()));
  page->addWidget(label);
  m_deleteHomeDir = new QCheckBox(i18n("Delete &home folder: %1", AUser->getHomeDir()));
  page->addWidget(m_deleteHomeDir);
  QString mailboxpath = QFile::decodeName(MAIL_SPOOL_DIR) + QLatin1Char( '/' ) + AUser->getName();
  m_deleteMailBox = new QCheckBox(i18n("Delete &mailbox: %1", mailboxpath));
  page->addWidget(m_deleteHomeDir);
  page->addWidget(buttonBox);
}

#include "ku_deluser.moc"
