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

#ifndef VIPERSGUI_MODULEPARAMETERWIDGET_HPP
#define VIPERSGUI_MODULEPARAMETERWIDGET_HPP

#include <QWidget>
#include <QPair>
#include <QMap>
#include <QString>
#include <QIcon>
#include <Module.hpp>
#include <Parameter.hpp>

class QFormLayout;

using namespace VIPERS;

class ModuleParameterWidget: public QWidget
{
    Q_OBJECT

  public:

    enum WidgetType
    {
      eWidgetTypeFieldLabel,
      eWidgetTypeLabel,
      eWidgetTypeLineEdit,
      eWidgetTypePushButton,
      eWidgetTypeComboBox,
      eWidgetTypeCheckBox,
      eWidgetTypeSpinBox,
      eWidgetTypeSpinBoxHeight,
      eWidgetTypeSpinBoxWidth,
      eWidgetTypeDoubleSpinBox,
      eWidgetTypeSlider
    };

    typedef QMap<WidgetType, QWidget*> WidgetTypeMap;

    struct ParameterData
    {
      int layoutRow;
      WidgetTypeMap widgetMap;
      Parameter* parameterPtr;
    };

    typedef QMap<QString, ParameterData> ParameterDataMap;
    typedef QMap<QWidget*, QString> WidgetParameterNameMap;

    explicit ModuleParameterWidget(Module* inModule, QWidget* inParent = 0);
    virtual ~ModuleParameterWidget();

    inline Module* getModule() const {return mModule;}

    void updateParameters();
    void setRuntimeState(bool inValue);
    void setHideOnDisabled(bool inValue);

  protected slots:

    void activatedComboBox(const QString& inText);

    void editedLineEditString();
    void editedLineEditFile();
    void editedLineEditDirectory();

    void clickedButtonChooseFile(bool inChecked);
    void clickedButtonChooseDirectory(bool inChecked);
    void clickedButtonChooseColor(bool inChecked);

    void changedCheckBoxBool(int inState);

    void changedSpinBox(int inValue);
    void changedSpinBoxSize(int inValue);
    void changedDoubleSpinBox(double inValue);

  private:

    void createLayout();

    QString formatTooltipString(const Parameter& inParameter) const;
    QIcon createIconFromColor(const Color& inColor) const;

    void updateModuleParameter(Parameter& inParameter);
    void updateParameter(const QString& inName);
    void updateParameter(ParameterData& inParameterData);
    void updateWidgetEnableHideState(WidgetTypeMap& inWidgetTypeMap, bool inParameterState);

    QFormLayout* mFormLayout;

    Module* mModule;

    ParameterDataMap mParameterDataMap;
    WidgetParameterNameMap mWidgetParameterNameMap;

    bool mHideDisabled;

};

#endif //VIPERSGUI_MODULEPARAMETERWIDGET_HPP
