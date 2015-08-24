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
 * $Revision: 274 $
 * $Date: 2009-10-23 20:34:51 -0400 (Fri, 23 Oct 2009) $
 */

#include "KernelState.hpp"

using namespace VIPERS;

/*! \todo
*/
KernelState::KernelState()
{
  mState = eStateUninitialized;
  mFrame = 0;
  mMaximumFrame = -1;
  mIsExceptionRaised = false;
}

/*! \todo
*/
KernelState::~KernelState()
{
}

/*! \todo
*/
KernelState::State KernelState::getState() const throw()
{
  return mState;
}

/*! \todo
*/
unsigned int KernelState::getFrame() const throw()
{
  return mFrame;
}

/*! \todo
*/
unsigned int KernelState::getMaximumFrame() const throw()
{
  return mMaximumFrame;
}

/*! \todo
*/
KernelState::operator KernelState::State() const throw()
{
  return mState;
}

/*! \todo
*/
KernelState::operator unsigned int() const throw()
{
  return mFrame;
}

/*! \todo
*/
void KernelState::setState(KernelState::State inState) throw()
{
  mState = inState;
}

/*! \todo
*/
void KernelState::setFrame(unsigned int inFrame) throw()
{
  mFrame = inFrame;
}

/*! \todo
*/
void KernelState::setMaximumFrame(unsigned int inMaximumFrame) throw()
{
  mMaximumFrame = inMaximumFrame;
}

/*! \todo
*/
KernelState& KernelState::operator=(const KernelState::State& inState) throw()
{
  mState = inState;
  return *this;
}

/*! \todo
*/
KernelState& KernelState::operator=(unsigned int inFrame) throw()
{
  mFrame = inFrame;
  return *this;
}

/*! \todo
*/
bool KernelState::isExceptionRaised() const throw()
{
  return mIsExceptionRaised;
}

/*! \todo
*/
Exception KernelState::getException() throw()
{
  Exception lException;
  if(mException)
  {
    lException = mException;
    mException.clear();
    mIsExceptionRaised = false;
    return lException;
  }
  else
  {
    mIsExceptionRaised = false;
    return Exception(Exception::eCodeUndefined, "No exception was raised");
  }
}

/*! \todo
*/
void KernelState::setException(const Exception& inException) throw()
{
  mIsExceptionRaised = true;
  mException = inException;
}
