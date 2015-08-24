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

#ifndef VIPERSGUI_MODULEPARAMETERSTACK_HPP
#define VIPERSGUI_MODULEPARAMETERSTACK_HPP

#include <QWidget>
#include <QMap>
#include <QString>
#include <Module.hpp>

#include "ModuleInstanceInfo.hpp"

using namespace VIPERS;

class QComboBox;
class QScrollArea;
class ModuleParameterWidget;

typedef QMap<QString, ModuleParameterWidget*> ModuleParameterWidgetMap;

class ModuleParameterStack: public QWidget
{
    Q_OBJECT

  public:

    ModuleParameterStack(QWidget* inParent = 0);
    virtual ~ModuleParameterStack();

    void setRuntimeState(bool inValue);
    void updateModuleParameterWidgets();

  public slots:

    void addModuleInstanceParameter(const ModuleInstanceInfo& inModuleInstanceInfo);
    void removeModuleInstanceParameter(const QString& inLabel);
    void updateModuleInstanceParameter(const QString& inOldLabel, const QString& inNewLabel);

    void activateModuleParameter(const QString& inLabel);

  private:

    void createLayout();

    QScrollArea* mModuleParameterScrollArea;
    QComboBox* mModuleParameterList;

    ModuleParameterWidgetMap mModuleParameterWidgetMap;

};

#endif //VIPERSGUI_MODULEPARAMETERSTACK_HPP
