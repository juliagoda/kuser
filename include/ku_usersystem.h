/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
 *  Maintained by Adriaan de Groot <groot@kde.org>
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

#ifndef _KU_USERSYSTEM_H_
#define _KU_USERSYSTEM_H_

#include "ku_user.h"

class KU_UserSystem : public KU_Users {
public:
  KU_UserSystem(KU_PrefsBase *cfg);
  virtual ~KU_UserSystem();

  virtual bool dbcommit() override { return true; }
  virtual bool reload() override;
  virtual void createPassword( KU_User & /*user*/, const QString & /*password*/ ) override {}

private:

  bool loadpwd();
  bool loadsdw();
};
#endif // _KU_USERSYSTEM_H_
