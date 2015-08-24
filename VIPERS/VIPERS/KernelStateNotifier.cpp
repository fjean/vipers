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
 * \file VIPERS/KernelState.cpp
 * \brief KernelState class functions definition.
 * \author Frederic Jean
 * $Revision: 285 $
 * $Date: 2009-10-29 22:26:19 -0400 (Thu, 29 Oct 2009) $
 */

#include "KernelStateNotifier.hpp"

using namespace VIPERS;

/*! \todo
*/
KernelStateNotifier::KernelStateNotifier()
{
  mNotification = false;
}

/*! \todo
*/
KernelStateNotifier::~KernelStateNotifier()
{
}

/*! \todo
*/
void KernelStateNotifier::notify(const KernelState& inKernelState)
{
  lock();
  mKernelState = inKernelState;
  mNotification = true;
  broadcast();
  unlock();
}

/*! \todo
*/
KernelState KernelStateNotifier::waitNotification()
{
  KernelState lKernelState;
  lock();
  while(!mNotification)
    wait();
  mNotification = false;
  lKernelState = mKernelState;
  //mKernelState = KernelState();
  unlock();
  return lKernelState;
}

/*! \todo
*/
void KernelStateNotifier::notifyLocked(const KernelState& inKernelState)
{
  mKernelState = inKernelState;
  mNotification = true;
  broadcast();
}

/*! \todo
*/
KernelState KernelStateNotifier::waitNotificationLocked()
{
  KernelState lKernelState;

  while(!mNotification)
    wait();
  mNotification = false;
  lKernelState = mKernelState;
  //mKernelState = KernelState();

  return lKernelState;
}

/*! \todo
*/
void KernelStateNotifier::setState(const KernelState& inKernelState)
{
  lock();
  mKernelState = inKernelState;
  mNotification = true;
  unlock();
}

/*! \todo
*/
KernelState KernelStateNotifier::getState()
{
  KernelState lKernelState;

  lock();
  lKernelState = mKernelState;
  //mKernelState = KernelState();
  mNotification = false;
  unlock();

  return lKernelState;
}

/*! \todo
*/
void KernelStateNotifier::setStateLocked(const KernelState& inKernelState)
{
  mKernelState = inKernelState;
  mNotification = true;
}

/*! \todo
*/
KernelState KernelStateNotifier::getStateLocked()
{
  KernelState lKernelState = mKernelState;
  //mKernelState = KernelState();
  mNotification = false;
  return lKernelState;
}
