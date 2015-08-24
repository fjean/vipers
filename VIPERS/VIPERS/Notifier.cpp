/*
 *  Video and Image Processing Environment for Real-time Systems (VIPERS)
 *  Copyright (C) 2009 by Frederic Jean
 *
 *  VIPERS is a free library: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License,
 *  or (at your option) any later version.
 *
 *  VIPERS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with VIPERS.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Contact:
 *  Computer Vision and Systems Laboratory
 *  Department of Electrical and Computer Engineering
 *  Universite Laval, Quebec, Canada, G1V 0A6
 *  http://vision.gel.ulaval.ca
 *
 */

 /*!
 * \file VIPERS/Notifier.cpp
 * \brief Notifier class functions definition.
 * \author Frederic Jean
 * $Revision: 252 $
 * $Date: 2009-10-14 21:52:34 -0400 (Wed, 14 Oct 2009) $
 */

#include "Notifier.hpp"

using namespace VIPERS;

/*! \todo
*/
Notifier::Notifier()
{
  mNotification = false;
}

/*! \todo
*/
Notifier::~Notifier()
{
}

/*! \todo
*/
void Notifier::notify()
{
  lock();
  mNotification = true;
  broadcast();
  unlock();
}

/*! \todo
*/
void Notifier::waitNotification()
{
  lock();
  while(!mNotification)
    wait();
  mNotification = false;
  unlock();
}
