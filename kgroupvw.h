/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
 *  Maintained by Adriaan de Groot <groot@kde.org>
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

#ifndef _KU_GROUPVW_H_
#define _KU_GROUPVW_H_

#include <qwidget.h>

#include <k3listview.h>

#include "ku_group.h"

class KGroupViewItem : public K3ListViewItem
{
public:
  KGroupViewItem(K3ListView *parent, int index);
  int index() { return mIndex; }
private:
  virtual QString text ( int ) const;
  virtual int compare( Q3ListViewItem *i, int col, bool ascending ) const;

  int mIndex;
};

class KGroupView : public K3ListView
{
    Q_OBJECT

public:
  KGroupView( QWidget* parent = 0, const char* name = 0 );

  virtual ~KGroupView();

  void insertItem(int index);
  void removeItem(int index);
  int getCurrentIndex();
  void init();
};

#endif // _KGROUPVW_H_
