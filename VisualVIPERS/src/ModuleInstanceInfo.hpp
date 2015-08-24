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

#ifndef VIPERSGUI_MODULEINSTANCEINFO_HPP
#define VIPERSGUI_MODULEINSTANCEINFO_HPP

#include <QString>
#include <QIcon>
#include <Module.hpp>

using namespace VIPERS;

class ModuleInstanceInfo
{
  public:

    ModuleInstanceInfo();
    ModuleInstanceInfo(const QString& inLabel, Module* inModule, const QIcon& inIcon);

    inline void setLabel(const QString& inLabel) {mLabel=inLabel;}
    inline QString getLabel() const {return mLabel;}
    inline void setIcon(const QIcon& inIcon) {mIcon = inIcon;}
    inline const QIcon& getIcon() const {return mIcon;}
    inline void setModule(Module* inModule) {mModule=inModule;}
    inline Module* getModule() const {return mModule;}

  protected:

    QString mLabel;
    Module* mModule;
    QIcon mIcon;
};

#endif //VIPERSGUI_MODULEINSTANCEINFO_HPP
