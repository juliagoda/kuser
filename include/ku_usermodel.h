/*
 *  Copyright (c) 2006 Szombathelyi György <gyurco@freemail.hu>
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

#ifndef _KU_USERMODEL_H_
#define _KU_USERMODEL_H_

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>

#include "ku_user.h"

class KU_UserModel : public QAbstractTableModel
{
public:
  virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const override;
  virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const override;
  virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;
  virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const override;
  virtual bool insertRows( int row, int count, const QModelIndex & parent = QModelIndex() ) override;
  virtual bool removeRows( int row, int count, const QModelIndex & parent = QModelIndex() ) override;
  virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole ) override;

  void init() { beginResetModel(); endResetModel(); }
  void commitMod();
  void commitDel();
  void commitAdd();
};

class KU_UserSortingProxyModel : public QSortFilterProxyModel
{
public:
  void setFirstUser( uint first ) { mFirstUser = first; }
protected:
  virtual bool lessThan( const QModelIndex & left, const QModelIndex & right ) const override;
  virtual bool filterAcceptsRow( int source_row, const QModelIndex & source_parent ) const override;
private:
  uint mFirstUser;
};

#endif // _KU_USERMODEL_H_
