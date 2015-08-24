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

#ifndef VIPERSGUI_SHAREDOBJECTS_HPP
#define VIPERSGUI_SHAREDOBJECTS_HPP

#include <QPixmap>

class SharedObjects
{

  public:

    enum ApplicationMode
    {
      eModeEditNormal,
      eModeEditConnectSlots,
      eModeProcessing
    };

    ~SharedObjects();

    static SharedObjects* instance();
    static void destroy();
    void init();

    inline const QPixmap& getPixmapSlotConnectDrag() const {return mSlotConnectDragPixmap;}
    inline const QPixmap& getPixmapSlotConnected() const {return mSlotConnectedPixmap;}
    inline const QPixmap& getPixmapSlotDisconnected() const {return mSlotDisconnectedIconPixmap;}
    inline const QPixmap& getPixmapSlotSelected() const {return mSlotSelectedPixmap;}

  protected:

    QPixmap mSlotConnectDragPixmap;
    QPixmap mSlotConnectedPixmap;
    QPixmap mSlotDisconnectedIconPixmap;
    QPixmap mSlotSelectedPixmap;

  private:

    // Disable constructors (singleton)
    SharedObjects();
    SharedObjects(const SharedObjects&);

    static SharedObjects* mInstance;

};

#endif //VIPERSGUI_SHAREDOBJECTS_HPP
