/*
 *  Visual Video and Image Processing Environment for Real-time Systems (Visual VIPERS)
 *  Copyright (C) 2009 by Frederic Jean
 *
 *  Visual VIPERS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, either version 3 of the License,
 *  or (at your option) any later version.
 *
 *  Visual VIPERS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Visual VIPERS.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Contact:
 *  Computer Vision and Systems Laboratory
 *  Department of Electrical and Computer Engineering
 *  Universite Laval, Quebec, Canada, G1V 0A6
 *  http://vision.gel.ulaval.ca
 *
 */

#include "UIKernelStateNotifier.hpp"
#include "MainWindow.hpp"

#include <QApplication>
#include <iostream>

using namespace VIPERS;

//-------------------------------------------------------------------------------

UIKernelStateNotifier::UIKernelStateNotifier(MainWindow* inMainWindow)
: KernelStateNotifier(), mRefreshTime(QTime::currentTime())
{
  mMainWindow = inMainWindow;
  mEventType = QEvent::registerEventType();
  mPostUIEvent = false;
  mMinRefreshTimeMs = 15;
}

//-------------------------------------------------------------------------------

UIKernelStateNotifier::~UIKernelStateNotifier()
{
}

//-------------------------------------------------------------------------------

void UIKernelStateNotifier::notify(const KernelState& inKernelState)
{
  QEvent* lKernelStateEvent = 0;

  
  
  lock();

  setStateLocked(inKernelState);
  broadcast();

  if(mMainWindow && mPostUIEvent ) // && mRefreshTime.elapsed()>=mMinRefreshTimeMs)
  {
    //lKernelStateEvent = new QEvent(getEventType());
    //QApplication::postEvent(mMainWindow, lKernelStateEvent, Qt::NormalEventPriority);
    //std::cout << "SEND: " << inKernelState.getState() << std::endl;
    mRefreshTime.start();
    unlock();

    emit updatedState();
    return;
  }

  unlock();
}

//-------------------------------------------------------------------------------

void UIKernelStateNotifier::notifyLocked(const KernelState& inKernelState)
{
  QEvent* lKernelStateEvent = 0;

  setStateLocked(inKernelState);
  broadcast();

  if(mMainWindow && mPostUIEvent && mRefreshTime.elapsed()>=mMinRefreshTimeMs)
  {
    //lKernelStateEvent = new QEvent(getEventType());
    //QApplication::postEvent(mMainWindow, lKernelStateEvent, Qt::NormalEventPriority);

    mRefreshTime.start();
    emit updatedState();
    return;
  }
}

//-------------------------------------------------------------------------------

void UIKernelStateNotifier::setMainWindow(MainWindow* inMainWindow)
{
  lock();
  mMainWindow = inMainWindow;
  unlock();
}

//-------------------------------------------------------------------------------

MainWindow* UIKernelStateNotifier::getMainWindow() const
{
  MainWindow* lMainWindow;
  lock();
  lMainWindow = mMainWindow;
  unlock();
  return lMainWindow;
}

//-------------------------------------------------------------------------------

void UIKernelStateNotifier::setPostUIEvent(bool inValue)
{
  lock();
  mPostUIEvent = inValue;
  mRefreshTime.start();
  unlock();
}

//-------------------------------------------------------------------------------

bool UIKernelStateNotifier::getPostUIEvent() const
{
  bool lValue;
  lock();
  lValue = mPostUIEvent;
  unlock();
  return lValue;
}

//-------------------------------------------------------------------------------

void UIKernelStateNotifier::setMinimumRefreshTime(int inValue)
{
  lock();
  mMinRefreshTimeMs = inValue;
  unlock();
}

//-------------------------------------------------------------------------------

int UIKernelStateNotifier::getMinimumRefreshTime() const
{
  int lValue;
  lock();
  lValue = mMinRefreshTimeMs;
  unlock();
  return lValue;
}

//-------------------------------------------------------------------------------
