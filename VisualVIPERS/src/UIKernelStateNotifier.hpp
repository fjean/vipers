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

#ifndef VIPERSGUI_UIKERNEL_STATE_NOTIFIER_HPP
#define VIPERSGUI_UIKERNEL_STATE_NOTIFIER_HPP

#include <KernelStateNotifier.hpp>
#include <QEvent>
#include <QTime>
#include <QObject>

using namespace VIPERS;

// Forward declarations
class MainWindow;

class UIKernelStateNotifier: public QObject, public KernelStateNotifier
{
  Q_OBJECT

  public:

    UIKernelStateNotifier(MainWindow* inMainWindow = 0);
    virtual ~UIKernelStateNotifier();

    //! Send kernel state notification
    virtual void notify(const KernelState& inKernelState);

    //! Send kernel state notification (must lock and unlock mutex manually)
    virtual void notifyLocked(const KernelState& inKernelState);

    void setMainWindow(MainWindow* inMainWindow);
    MainWindow* getMainWindow() const;

    void setPostUIEvent(bool inValue);
    bool getPostUIEvent() const;

    void setMinimumRefreshTime(int inValue);
    int getMinimumRefreshTime() const;

    inline void setPostUIEventLocked(bool inValue) {mPostUIEvent=inValue;}
    inline bool getPostUIEventLocked() const {return mPostUIEvent;}

    inline QEvent::Type getEventType() const {return static_cast<QEvent::Type>(mEventType);}

  signals:

    void updatedState();

  protected:

    MainWindow* mMainWindow;
    int mEventType;
    bool mPostUIEvent;

    QTime mRefreshTime;
    int mMinRefreshTimeMs;
};

#endif //VIPERSGUI_UIKERNEL_STATE_NOTIFIER_HPP
