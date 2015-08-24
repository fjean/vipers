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

#ifndef VIPERSGUI_MODULELIST_HPP
#define VIPERSGUI_MODULELIST_HPP

#include <QListWidget>
#include <QPoint>

class ModulesList : public QListWidget
{
    Q_OBJECT

  public:

    ModulesList();
    virtual ~ModulesList();

  protected:

    void mousePressEvent(QMouseEvent* inEvent);
    void mouseMoveEvent(QMouseEvent* inEvent);

  private:

    QPoint mDragStartPos;

};

#endif //VIPERSGUI_MODULELIST_HPP
