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

#include "ModuleParameterWidget.hpp"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QPixmap>
#include <QMessageBox>
#include <QFileDialog>
#include <QColorDialog>

#include <QDebug>

#include <climits>
#include <cfloat>

using namespace VIPERS;

//-------------------------------------------------------------------------------

ModuleParameterWidget::ModuleParameterWidget(Module* inModule, QWidget* inParent)
  : QWidget(inParent)
{
  mModule = inModule;
  mHideDisabled = false;

  mFormLayout = 0;

  if(mModule)
    createLayout();
  else
    qDebug() << "WARNING: NULL module passed to ModuleParameterWidget constructor";
}

//-------------------------------------------------------------------------------

ModuleParameterWidget::~ModuleParameterWidget()
{
  foreach(ParameterData lParameterData, mParameterDataMap)
    delete lParameterData.parameterPtr;
}

//-------------------------------------------------------------------------------

void ModuleParameterWidget::updateParameters()
{
  for(ParameterDataMap::iterator lParameterDataItr = mParameterDataMap.begin(); lParameterDataItr != mParameterDataMap.end(); lParameterDataItr++)
  {
    updateParameter(*lParameterDataItr);
  }
}

//-------------------------------------------------------------------------------

void ModuleParameterWidget::setRuntimeState(bool inValue)
{
  foreach(ParameterData lParameterData, mParameterDataMap)
  {
    if(inValue)
      updateWidgetEnableHideState(lParameterData.widgetMap, lParameterData.parameterPtr->getRunTimeChange() && lParameterData.parameterPtr->getEnabled());
    else
      updateWidgetEnableHideState(lParameterData.widgetMap, lParameterData.parameterPtr->getEnabled());
  }
}

//-------------------------------------------------------------------------------

void ModuleParameterWidget::setHideOnDisabled(bool inValue)
{
  mHideDisabled = inValue;
  foreach(ParameterData lParameterData, mParameterDataMap)
  {
    updateWidgetEnableHideState(lParameterData.widgetMap, lParameterData.parameterPtr->getEnabled());
  }
}

//-------------------------------------------------------------------------------

void ModuleParameterWidget::activatedComboBox(const QString& inText)
{
  ParameterDataMap::iterator lParameterDataItr;
  WidgetParameterNameMap::iterator lWidgetParameterNameItr;

  QComboBox* lComboBox = qobject_cast<QComboBox*>(sender());
  Q_ASSERT(lComboBox);
  QString lItemName = lComboBox->itemData(lComboBox->findText(inText)).toString();

  lWidgetParameterNameItr = mWidgetParameterNameMap.find(lComboBox);
  Q_ASSERT(lWidgetParameterNameItr!=mWidgetParameterNameMap.end());

  lParameterDataItr = mParameterDataMap.find(*lWidgetParameterNameItr);
  Q_ASSERT(lParameterDataItr!=mParameterDataMap.end());

  lParameterDataItr->parameterPtr->setValueStr(lItemName.toStdString().c_str());
  updateModuleParameter(*(lParameterDataItr->parameterPtr));
}

//-------------------------------------------------------------------------------

void ModuleParameterWidget::editedLineEditString()
{
  ParameterDataMap::iterator lParameterDataItr;
  WidgetParameterNameMap::iterator lWidgetParameterNameItr;

  QLineEdit* lLineEdit = qobject_cast<QLineEdit*>(sender());
  Q_ASSERT(lLineEdit);

  lWidgetParameterNameItr = mWidgetParameterNameMap.find(lLineEdit);
  Q_ASSERT(lWidgetParameterNameItr!=mWidgetParameterNameMap.end());

  lParameterDataItr = mParameterDataMap.find(*lWidgetParameterNameItr);
  Q_ASSERT(lParameterDataItr!=mParameterDataMap.end());

  lParameterDataItr->parameterPtr->setValueStr(lLineEdit->text().toStdString().c_str());
  updateModuleParameter(*(lParameterDataItr->parameterPtr));
}

//-------------------------------------------------------------------------------

void ModuleParameterWidget::editedLineEditFile()
{
  ParameterDataMap::iterator lParameterDataItr;
  WidgetParameterNameMap::iterator lWidgetParameterNameItr;

  QLineEdit* lLineEdit = qobject_cast<QLineEdit*>(sender());
  Q_ASSERT(lLineEdit);

  lWidgetParameterNameItr = mWidgetParameterNameMap.find(lLineEdit);
  Q_ASSERT(lWidgetParameterNameItr!=mWidgetParameterNameMap.end());

  lParameterDataItr = mParameterDataMap.find(*lWidgetParameterNameItr);
  Q_ASSERT(lParameterDataItr!=mParameterDataMap.end());

  lParameterDataItr->parameterPtr->setValueStr(lLineEdit->text().toStdString().c_str());
  updateModuleParameter(*(lParameterDataItr->parameterPtr));
}

//-------------------------------------------------------------------------------

void ModuleParameterWidget::editedLineEditDirectory()
{
  ParameterDataMap::iterator lParameterDataItr;
  WidgetParameterNameMap::iterator lWidgetParameterNameItr;

  QLineEdit* lLineEdit = qobject_cast<QLineEdit*>(sender());
  Q_ASSERT(lLineEdit);

  lWidgetParameterNameItr = mWidgetParameterNameMap.find(lLineEdit);
  Q_ASSERT(lWidgetParameterNameItr!=mWidgetParameterNameMap.end());

  lParameterDataItr = mParameterDataMap.find(*lWidgetParameterNameItr);
  Q_ASSERT(lParameterDataItr!=mParameterDataMap.end());

  lParameterDataItr->parameterPtr->setValueStr(lLineEdit->text().toStdString().c_str());

  updateModuleParameter(*(lParameterDataItr->parameterPtr));
}

//-------------------------------------------------------------------------------

void ModuleParameterWidget::clickedButtonChooseFile(bool inChecked)
{
  ParameterDataMap::iterator lParameterDataItr;
  WidgetParameterNameMap::iterator lWidgetParameterNameItr;
  WidgetTypeMap::iterator lWidgetTypeItr;

  QPushButton* lPushButton = qobject_cast<QPushButton*>(sender());
  Q_ASSERT(lPushButton);

  lWidgetParameterNameItr = mWidgetParameterNameMap.find(lPushButton);
  Q_ASSERT(lWidgetParameterNameItr!=mWidgetParameterNameMap.end());

  lParameterDataItr = mParameterDataMap.find(*lWidgetParameterNameItr);
  Q_ASSERT(lParameterDataItr!=mParameterDataMap.end());

  lWidgetTypeItr = lParameterDataItr->widgetMap.find(eWidgetTypeLineEdit);
  Q_ASSERT(lWidgetTypeItr!=lParameterDataItr->widgetMap.end());
  QLineEdit* lLineEdit = qobject_cast<QLineEdit*>(*lWidgetTypeItr);

  QFileDialog lFileDialog(this);
  
  if(mModule->getOutputSlots().size()==0)
  {
    lFileDialog.setAcceptMode(QFileDialog::AcceptSave);
    lFileDialog.setFileMode(QFileDialog::AnyFile);
    lFileDialog.setDirectory(QDir(lLineEdit->text()));
    lFileDialog.setNameFilter(tr("All Files (*.*)"));
    lFileDialog.setWindowTitle(tr("Specify a file for parameter ") + QString(lParameterDataItr->parameterPtr->getDisplayName().c_str()));    
  }
  else
  {
    lFileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    lFileDialog.setFileMode(QFileDialog::ExistingFile);
    lFileDialog.setDirectory(QDir(lLineEdit->text()));
    lFileDialog.setNameFilter(tr("All Files (*.*)"));
    lFileDialog.setWindowTitle(tr("Choose a file for parameter ") + QString(lParameterDataItr->parameterPtr->getDisplayName().c_str()));
  }
  
  if(lFileDialog.exec())
  {
    lLineEdit->setText(lFileDialog.selectedFiles().front());
    lParameterDataItr->parameterPtr->setValueStr(lLineEdit->text().toStdString().c_str());
    updateModuleParameter(*(lParameterDataItr->parameterPtr));
  }

}

//-------------------------------------------------------------------------------

void ModuleParameterWidget::clickedButtonChooseDirectory(bool inChecked)
{
  ParameterDataMap::iterator lParameterDataItr;
  WidgetParameterNameMap::iterator lWidgetParameterNameItr;
  WidgetTypeMap::iterator lWidgetTypeItr;

  QPushButton* lPushButton = qobject_cast<QPushButton*>(sender());
  Q_ASSERT(lPushButton);

  lWidgetParameterNameItr = mWidgetParameterNameMap.find(lPushButton);
  Q_ASSERT(lWidgetParameterNameItr!=mWidgetParameterNameMap.end());

  lParameterDataItr = mParameterDataMap.find(*lWidgetParameterNameItr);
  Q_ASSERT(lParameterDataItr!=mParameterDataMap.end());

  lWidgetTypeItr = lParameterDataItr->widgetMap.find(eWidgetTypeLineEdit);
  Q_ASSERT(lWidgetTypeItr!=lParameterDataItr->widgetMap.end());
  QLineEdit* lLineEdit = qobject_cast<QLineEdit*>(*lWidgetTypeItr);

  QFileDialog lFileDialog(this);
  lFileDialog.setAcceptMode(QFileDialog::AcceptOpen);
  lFileDialog.setFileMode(QFileDialog::Directory);
  lFileDialog.setOption(QFileDialog::ShowDirsOnly, true);
  lFileDialog.setDirectory(QDir(lLineEdit->text()));
  lFileDialog.setWindowTitle(tr("Choose a directory for parameter ") + QString(lParameterDataItr->parameterPtr->getDisplayName().c_str()));

  if(lFileDialog.exec())
  {
    lLineEdit->setText(lFileDialog.selectedFiles().front());
    lParameterDataItr->parameterPtr->setValueStr(lLineEdit->text().toStdString().c_str());
    updateModuleParameter(*(lParameterDataItr->parameterPtr));
  }

}

//-------------------------------------------------------------------------------

void ModuleParameterWidget::clickedButtonChooseColor(bool inChecked)
{
  ParameterDataMap::iterator lParameterDataItr;
  WidgetParameterNameMap::iterator lWidgetParameterNameItr;

  QPushButton* lPushButton = qobject_cast<QPushButton*>(sender());
  Q_ASSERT(lPushButton);

  lWidgetParameterNameItr = mWidgetParameterNameMap.find(lPushButton);
  Q_ASSERT(lWidgetParameterNameItr!=mWidgetParameterNameMap.end());

  lParameterDataItr = mParameterDataMap.find(*lWidgetParameterNameItr);
  Q_ASSERT(lParameterDataItr!=mParameterDataMap.end());

  Color lColor = lParameterDataItr->parameterPtr->toColor();
  QColorDialog lColorDialog(this);
  lColorDialog.setWindowTitle(tr("Choose a color for parameter ") + QString(lParameterDataItr->parameterPtr->getDisplayName().c_str()));
  lColorDialog.setCurrentColor(QColor(qRgb(lColor.getComponent1(), lColor.getComponent2(), lColor.getComponent3())));
  lColorDialog.setOption(QColorDialog::ShowAlphaChannel, false);

  if(lColorDialog.exec())
  {
    QColor lColorTmp = lColorDialog.selectedColor();
    Color lTmpColor(lColorTmp.red(), lColorTmp.green(), lColorTmp.blue());
    lPushButton->setIcon(createIconFromColor(lTmpColor));
    lParameterDataItr->parameterPtr->setValue(lTmpColor);
    updateModuleParameter(*(lParameterDataItr->parameterPtr));
  }
}

//-------------------------------------------------------------------------------

void ModuleParameterWidget::changedCheckBoxBool(int inState)
{
  ParameterDataMap::iterator lParameterDataItr;
  WidgetParameterNameMap::iterator lWidgetParameterNameItr;

  QCheckBox* lCheckBox = qobject_cast<QCheckBox*>(sender());
  Q_ASSERT(lCheckBox);

  lWidgetParameterNameItr = mWidgetParameterNameMap.find(lCheckBox);
  Q_ASSERT(lWidgetParameterNameItr!=mWidgetParameterNameMap.end());

  lParameterDataItr = mParameterDataMap.find(*lWidgetParameterNameItr);
  Q_ASSERT(lParameterDataItr!=mParameterDataMap.end());

  lParameterDataItr->parameterPtr->setValue(lCheckBox->isChecked());

  updateModuleParameter(*(lParameterDataItr->parameterPtr));
}

//-------------------------------------------------------------------------------

void ModuleParameterWidget::changedSpinBox(int inValue)
{
  ParameterDataMap::iterator lParameterDataItr;
  WidgetParameterNameMap::iterator lWidgetParameterNameItr;

  QSpinBox* lSpinBox = qobject_cast<QSpinBox*>(sender());
  Q_ASSERT(lSpinBox);

  lWidgetParameterNameItr = mWidgetParameterNameMap.find(lSpinBox);
  Q_ASSERT(lWidgetParameterNameItr!=mWidgetParameterNameMap.end());

  lParameterDataItr = mParameterDataMap.find(*lWidgetParameterNameItr);
  Q_ASSERT(lParameterDataItr!=mParameterDataMap.end());

  lParameterDataItr->parameterPtr->setValue(inValue);

  updateModuleParameter(*(lParameterDataItr->parameterPtr));
}

//-------------------------------------------------------------------------------

void ModuleParameterWidget::changedSpinBoxSize(int inValue)
{
  ParameterDataMap::iterator lParameterDataItr;
  WidgetParameterNameMap::iterator lWidgetParameterNameItr;
  WidgetTypeMap::iterator lWidgetType1,lWidgetType2;

  QWidget* lWidget = qobject_cast<QWidget*>(sender());
  Q_ASSERT(lWidget);

  lWidgetParameterNameItr = mWidgetParameterNameMap.find(lWidget);
  Q_ASSERT(lWidgetParameterNameItr!=mWidgetParameterNameMap.end());

  lParameterDataItr = mParameterDataMap.find(*lWidgetParameterNameItr);
  Q_ASSERT(lParameterDataItr!=mParameterDataMap.end());

  lWidgetType1 = lParameterDataItr->widgetMap.find(eWidgetTypeSpinBoxWidth);
  lWidgetType2 = lParameterDataItr->widgetMap.find(eWidgetTypeSpinBoxHeight);
  Q_ASSERT(lWidgetType1!=lParameterDataItr->widgetMap.end() && lWidgetType2!=lParameterDataItr->widgetMap.end());

  QSpinBox* lSponBoxWidth = qobject_cast<QSpinBox*>(*lWidgetType1);
  QSpinBox* lSponBoxHeight = qobject_cast<QSpinBox*>(*lWidgetType2);

  lParameterDataItr->parameterPtr->setValue(Size(lSponBoxWidth->value(), lSponBoxHeight->value()));

  updateModuleParameter(*(lParameterDataItr->parameterPtr));
}

//-------------------------------------------------------------------------------

void ModuleParameterWidget::changedDoubleSpinBox(double inValue)
{
  ParameterDataMap::iterator lParameterDataItr;
  WidgetParameterNameMap::iterator lWidgetParameterNameItr;

  QDoubleSpinBox* lDoubleSpinBox = qobject_cast<QDoubleSpinBox*>(sender());
  Q_ASSERT(lDoubleSpinBox);

  lWidgetParameterNameItr = mWidgetParameterNameMap.find(lDoubleSpinBox);
  Q_ASSERT(lWidgetParameterNameItr!=mWidgetParameterNameMap.end());

  lParameterDataItr = mParameterDataMap.find(*lWidgetParameterNameItr);
  Q_ASSERT(lParameterDataItr!=mParameterDataMap.end());

  lParameterDataItr->parameterPtr->setValue(inValue);

  updateModuleParameter(*(lParameterDataItr->parameterPtr));
}

//-------------------------------------------------------------------------------

void ModuleParameterWidget::createLayout()
{
  ParameterMap lParameterMap = mModule->getParameterMap();
  Variable::VariableType lParameterType;
  QString lParameterName;
  QString lParameterDisplayName;
  QString lParameterTooltip;
  ParameterData lParameterData;

  if(lParameterMap.size()==0)
  {
    QVBoxLayout* lLayout = new QVBoxLayout(this);
    QLabel* lLabel = new QLabel(tr("This module does not have parameters"));
    lLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    lLabel->setWordWrap(true);
    lLayout->addWidget(lLabel, 1);
    setLayout(lLayout);
    return;
  }

  mFormLayout = new QFormLayout(this);
  //lFormLayout->setRowWrapPolicy(QFormLayout::WrapAllRows);
  //lFormLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

  // Create and initialize widgets for each parameter
  for(ParameterMap::iterator lParameterItr = lParameterMap.begin(); lParameterItr != lParameterMap.end(); lParameterItr++)
  {
    lParameterData.parameterPtr = new Parameter(lParameterItr->second);
    lParameterData.layoutRow = mFormLayout->rowCount();
    lParameterData.widgetMap.clear();

    lParameterType = lParameterItr->second.getVariableType();
    lParameterName = lParameterItr->second.getName().c_str();
    lParameterDisplayName = lParameterItr->second.getDisplayName().c_str();
    lParameterTooltip = formatTooltipString(lParameterItr->second);

    // If there is a list of possible value, then show a combo box
    if(lParameterItr->second.getPossibleValuesCount()>0)
    {
      QComboBox* lComboBox = new QComboBox();
      lComboBox->setToolTip(lParameterTooltip);
      connect(lComboBox, SIGNAL(activated (const QString&)), this, SLOT(activatedComboBox(const QString&)));

      mFormLayout->addRow(lParameterDisplayName + " : ", lComboBox);
      QLabel* lFieldLabel = qobject_cast<QLabel*>(mFormLayout->itemAt(lParameterData.layoutRow, QFormLayout::LabelRole)->widget());
      lFieldLabel->setToolTip(lComboBox->toolTip());

      lParameterData.widgetMap.insert(eWidgetTypeFieldLabel, lFieldLabel);
      lParameterData.widgetMap.insert(eWidgetTypeComboBox, lComboBox);
      mWidgetParameterNameMap.insert(lComboBox, lParameterName);

      mParameterDataMap.insert(lParameterName, lParameterData);
      updateParameter(lParameterData);
    }
    else if(lParameterType==Variable::eVariableTypeString)
    {
      QLineEdit* lLineEdit = new QLineEdit();
      lLineEdit->setToolTip(lParameterTooltip);
      connect(lLineEdit, SIGNAL(editingFinished()), this, SLOT(editedLineEditString()));

      mFormLayout->addRow(lParameterDisplayName + " : ", lLineEdit);
      QLabel* lFieldLabel = qobject_cast<QLabel*>(mFormLayout->itemAt(lParameterData.layoutRow, QFormLayout::LabelRole)->widget());
      lFieldLabel->setToolTip(lLineEdit->toolTip());

      lParameterData.widgetMap.insert(eWidgetTypeFieldLabel, lFieldLabel);
      lParameterData.widgetMap.insert(eWidgetTypeLineEdit, lLineEdit);
      mWidgetParameterNameMap.insert(lLineEdit, lParameterName);

      mParameterDataMap.insert(lParameterName, lParameterData);
      updateParameter(lParameterData);
    }
    else if(lParameterType==Variable::eVariableTypeFile)
    {
      QHBoxLayout* lLayout = new QHBoxLayout();
      QLineEdit* lLineEdit = new QLineEdit();
      QPushButton* lButton = new QPushButton("...");
      lLayout->addWidget(lLineEdit, 1);
      lLayout->addWidget(lButton, 0);
      lButton->setToolTip(tr("Choose a file"));
      lLineEdit->setToolTip(lParameterTooltip);

      connect(lButton, SIGNAL(clicked(bool)), this, SLOT(clickedButtonChooseFile(bool)));
      connect(lLineEdit, SIGNAL(editingFinished()), this, SLOT(editedLineEditFile()));

      mFormLayout->addRow(lParameterDisplayName + " : ", lLayout);
      QLabel* lFieldLabel = qobject_cast<QLabel*>(mFormLayout->itemAt(lParameterData.layoutRow, QFormLayout::LabelRole)->widget());
      lFieldLabel->setToolTip(lLineEdit->toolTip());

      lParameterData.widgetMap.insert(eWidgetTypeFieldLabel, lFieldLabel);
      lParameterData.widgetMap.insert(eWidgetTypeLineEdit, lLineEdit);
      lParameterData.widgetMap.insert(eWidgetTypePushButton, lButton);
      mWidgetParameterNameMap.insert(lButton, lParameterName);
      mWidgetParameterNameMap.insert(lLineEdit, lParameterName);

      mParameterDataMap.insert(lParameterName, lParameterData);
      updateParameter(lParameterData);
    }
    else if(lParameterType==Variable::eVariableTypeDirectory)
    {
      QHBoxLayout* lLayout = new QHBoxLayout();
      QLineEdit* lLineEdit = new QLineEdit();
      QPushButton* lButton = new QPushButton("...");
      lLayout->addWidget(lLineEdit, 1);
      lLayout->addWidget(lButton, 0);
      lButton->setToolTip(tr("Choose a directory"));
      lLineEdit->setToolTip(lParameterTooltip);

      connect(lButton, SIGNAL(clicked(bool)), this, SLOT(clickedButtonChooseDirectory(bool)));
      connect(lLineEdit, SIGNAL(editingFinished()), this, SLOT(editedLineEditDirectory()));

      mFormLayout->addRow(lParameterDisplayName + " : ", lLayout);
      QLabel* lFieldLabel = qobject_cast<QLabel*>(mFormLayout->itemAt(lParameterData.layoutRow, QFormLayout::LabelRole)->widget());
      lFieldLabel->setToolTip(lLineEdit->toolTip());

      lParameterData.widgetMap.insert(eWidgetTypeFieldLabel, lFieldLabel);
      lParameterData.widgetMap.insert(eWidgetTypeLineEdit, lLineEdit);
      lParameterData.widgetMap.insert(eWidgetTypePushButton, lButton);
      mWidgetParameterNameMap.insert(lButton, lParameterName);
      mWidgetParameterNameMap.insert(lLineEdit, lParameterName);

      mParameterDataMap.insert(lParameterName, lParameterData);
      updateParameter(lParameterData);
    }
    else if(lParameterType==Variable::eVariableTypeColor)
    {
      QPushButton* lButton = new QPushButton("");
      lButton->setToolTip(lParameterTooltip);

      connect(lButton, SIGNAL(clicked(bool)), this, SLOT(clickedButtonChooseColor(bool)));

      mFormLayout->addRow(lParameterDisplayName + " : ", lButton);
      QLabel* lFieldLabel = qobject_cast<QLabel*>(mFormLayout->itemAt(lParameterData.layoutRow, QFormLayout::LabelRole)->widget());
      lFieldLabel->setToolTip(lButton->toolTip());

      lParameterData.widgetMap.insert(eWidgetTypeFieldLabel, lFieldLabel);
      lParameterData.widgetMap.insert(eWidgetTypePushButton, lButton);
      mWidgetParameterNameMap.insert(lButton, lParameterName);

      mParameterDataMap.insert(lParameterName, lParameterData);
      updateParameter(lParameterData);
    }
    else if(lParameterType==Variable::eVariableTypeSize)
    {
      QHBoxLayout* lLayout = new QHBoxLayout();
      QSpinBox* lSpinBoxWidth = new QSpinBox();
      QSpinBox* lSpinBoxHeight = new QSpinBox();
      QLabel* lLabelTime = new QLabel(tr("X"));
      lLayout->addWidget(lSpinBoxWidth, 1);
      lLayout->addWidget(lLabelTime, 0);
      lLayout->addWidget(lSpinBoxHeight, 1);
      lLabelTime->setToolTip(lParameterTooltip);
      lSpinBoxWidth->setToolTip(tr("Width"));
      lSpinBoxHeight->setToolTip(tr("Height"));

      connect(lSpinBoxWidth, SIGNAL(valueChanged(int)), this, SLOT(changedSpinBoxSize(int)));
      connect(lSpinBoxHeight, SIGNAL(valueChanged(int)), this, SLOT(changedSpinBoxSize(int)));

      mFormLayout->addRow(lParameterDisplayName + " : ", lLayout);
      QLabel* lFieldLabel = qobject_cast<QLabel*>(mFormLayout->itemAt(lParameterData.layoutRow, QFormLayout::LabelRole)->widget());
      lFieldLabel->setToolTip(lLabelTime->toolTip());

      lParameterData.widgetMap.insert(eWidgetTypeFieldLabel, lFieldLabel);
      lParameterData.widgetMap.insert(eWidgetTypeSpinBoxWidth, lSpinBoxWidth);
      lParameterData.widgetMap.insert(eWidgetTypeSpinBoxHeight, lSpinBoxHeight);
      lParameterData.widgetMap.insert(eWidgetTypeLabel, lLabelTime);
      mWidgetParameterNameMap.insert(lSpinBoxWidth, lParameterName);
      mWidgetParameterNameMap.insert(lSpinBoxHeight, lParameterName);

      mParameterDataMap.insert(lParameterName, lParameterData);
      updateParameter(lParameterData);

    }
    else if(lParameterType==Variable::eVariableTypeBool)
    {
      QCheckBox* lCheckbox = new QCheckBox("");
      lCheckbox->setToolTip(lParameterTooltip);

      connect(lCheckbox, SIGNAL(stateChanged(int)), this, SLOT(changedCheckBoxBool(int)));

      mFormLayout->addRow(lParameterDisplayName + " : ", lCheckbox);
      QLabel* lFieldLabel = qobject_cast<QLabel*>(mFormLayout->itemAt(lParameterData.layoutRow, QFormLayout::LabelRole)->widget());
      lFieldLabel->setToolTip(lCheckbox->toolTip());

      lParameterData.widgetMap.insert(eWidgetTypeFieldLabel, lFieldLabel);
      lParameterData.widgetMap.insert(eWidgetTypeCheckBox, lCheckbox);
      mWidgetParameterNameMap.insert(lCheckbox, lParameterName);

      mParameterDataMap.insert(lParameterName, lParameterData);
      updateParameter(lParameterData);
    }
    else if(lParameterType==Variable::eVariableTypeChar || lParameterType==Variable::eVariableTypeUChar ||
            lParameterType==Variable::eVariableTypeShort || lParameterType==Variable::eVariableTypeUShort ||
            lParameterType==Variable::eVariableTypeInt || lParameterType==Variable::eVariableTypeUInt ||
            lParameterType==Variable::eVariableTypeLong || lParameterType==Variable::eVariableTypeULong)
    {
      QSpinBox* lSpinbox = new QSpinBox();
      lSpinbox->setToolTip(lParameterTooltip);

      connect(lSpinbox, SIGNAL(valueChanged(int)), this, SLOT(changedSpinBox(int)));

      mFormLayout->addRow(lParameterDisplayName + " : ", lSpinbox);
      QLabel* lFieldLabel = qobject_cast<QLabel*>(mFormLayout->itemAt(lParameterData.layoutRow, QFormLayout::LabelRole)->widget());
      lFieldLabel->setToolTip(lSpinbox->toolTip());

      lParameterData.widgetMap.insert(eWidgetTypeFieldLabel, lFieldLabel);
      lParameterData.widgetMap.insert(eWidgetTypeSpinBox, lSpinbox);
      mWidgetParameterNameMap.insert(lSpinbox, lParameterName);

      mParameterDataMap.insert(lParameterName, lParameterData);
      updateParameter(lParameterData);
    }
    else if(lParameterType==Variable::eVariableTypeFloat || lParameterType==Variable::eVariableTypeDouble)
    {
      QDoubleSpinBox* lSpinbox = new QDoubleSpinBox();
      lSpinbox->setToolTip(lParameterTooltip);

      connect(lSpinbox, SIGNAL(valueChanged(double)), this, SLOT(changedDoubleSpinBox(double)));

      mFormLayout->addRow(lParameterDisplayName + " : ", lSpinbox);
      QLabel* lFieldLabel = qobject_cast<QLabel*>(mFormLayout->itemAt(lParameterData.layoutRow, QFormLayout::LabelRole)->widget());
      lFieldLabel->setToolTip(lSpinbox->toolTip());

      lParameterData.widgetMap.insert(eWidgetTypeFieldLabel, lFieldLabel);
      lParameterData.widgetMap.insert(eWidgetTypeDoubleSpinBox, lSpinbox);
      mWidgetParameterNameMap.insert(lSpinbox, lParameterName);

      mParameterDataMap.insert(lParameterName, lParameterData);
      updateParameter(lParameterData);
    }
    else
    {
      qDebug() << "WARNING: ModuleParameterWidget: Unknown parameter type "
               << lParameterItr->second.getVariableType()
               << " in module "
               << mModule->getName().c_str()
               << " (" << mModule->getLabel().c_str() << ")";
    }
  }

  setLayout(mFormLayout);
}

//-------------------------------------------------------------------------------

QString ModuleParameterWidget::formatTooltipString(const Parameter& inParameter) const
{
  QString lFormattedToolTip;
  ValueSet lParameterValueSet = inParameter.getPossibleValues();

  lFormattedToolTip = QString("<b>%1</b><br/>%2<br/>").arg(inParameter.getDisplayName().c_str()).arg(inParameter.getDescription().c_str());

  if(lParameterValueSet.size()>0)
  {
    lFormattedToolTip += "<br/>";
    for(ValueSet::iterator lValueItr = lParameterValueSet.begin(); lValueItr != lParameterValueSet.end(); lValueItr++)
      lFormattedToolTip += QString("<b>%1</b> : %2<br/>").arg(lValueItr->getDisplayName().c_str()).arg(lValueItr->getDescription().c_str());
  }

  return lFormattedToolTip;
}

//-------------------------------------------------------------------------------

QIcon ModuleParameterWidget::createIconFromColor(const Color& inColor) const
{
  QColor lColor;
  QPixmap lPixmapColor(QSize(16,16));

  lColor.setRgb(qRgb(inColor.getComponent1(), inColor.getComponent2(), inColor.getComponent3()));
  lPixmapColor.fill(lColor);

  return QIcon(lPixmapColor);
}

//-------------------------------------------------------------------------------

void ModuleParameterWidget::updateModuleParameter(Parameter& inParameter)
{
  NameSet lNameSet;

  try
  {
    mModule->setParameterValue(inParameter);
  }
  catch(Exception& inException)
  {
    QMessageBox::critical(this, mModule->getLabel().c_str(), inException.toString().c_str());
    return;
  }

  lNameSet = inParameter.getDependentParameterNameSet();

  if(lNameSet.size()>0)
  {
    ParameterMap lParameterMap;
    ParameterMap::iterator lParameterItr;
    ParameterDataMap::iterator lParameterDataItr;

    try
    {
      mModule->updateParameters();
      lParameterMap = mModule->getParameterMap();
    }
    catch(Exception& inException)
    {
      QMessageBox::critical(this, mModule->getLabel().c_str(), inException.toString().c_str());
      return;
    }

    for(NameSet::iterator lNameItr = lNameSet.begin(); lNameItr != lNameSet.end(); lNameItr++)
    {
      lParameterDataItr = mParameterDataMap.find(lNameItr->c_str());
      lParameterItr = lParameterMap.find(lNameItr->c_str());
      if(lParameterItr!=lParameterMap.end() && lParameterDataItr!=mParameterDataMap.end())
      {
        *(lParameterDataItr->parameterPtr) = lParameterItr->second;
        updateParameter(*lParameterDataItr);
      }
    }
  }

}

//-------------------------------------------------------------------------------

void ModuleParameterWidget::updateParameter(const QString& inName)
{
  ParameterDataMap::iterator lParameterDataItr = mParameterDataMap.find(inName);
  if(lParameterDataItr!=mParameterDataMap.end())
    updateParameter(*lParameterDataItr);
}

//-------------------------------------------------------------------------------

void ModuleParameterWidget::updateParameter(ParameterData& inParameterData)
{
  WidgetTypeMap::iterator lWidgetItr, lWidgetItr2;
  QComboBox* lComboBox;
  QPushButton* lPushButton;
  QLineEdit* lLineEdit;
  QSpinBox* lSpinBox;
  QSpinBox* lSpinBox2;
  QDoubleSpinBox* lDoubleSpinBox;
  QCheckBox* lCheckBox;
  Size lSize;

  QString lParameterValue = inParameterData.parameterPtr->getValue().c_str();
  QString lParameterMinValue = inParameterData.parameterPtr->getMinValue().c_str();
  QString lParameterMaxValue = inParameterData.parameterPtr->getMaxValue().c_str();

  if(inParameterData.parameterPtr->getPossibleValuesCount()>0)
  {
    lWidgetItr = inParameterData.widgetMap.find(eWidgetTypeComboBox);
    Q_ASSERT(lWidgetItr!=inParameterData.widgetMap.end());
    lComboBox = qobject_cast<QComboBox*>(*lWidgetItr);
    Q_ASSERT(lComboBox);

    ValueSet lParameterValueSet = inParameterData.parameterPtr->getPossibleValues();

    for(ValueSet::iterator lValueItr = lParameterValueSet.begin(); lValueItr != lParameterValueSet.end(); lValueItr++)
      lComboBox->addItem(lValueItr->getDisplayName().c_str(), QString(lValueItr->getValue().c_str()));

    int lTmpIdx = lComboBox->findData(lParameterValue);
    if(lTmpIdx>=0)
      lComboBox->setCurrentIndex(lTmpIdx);

    updateWidgetEnableHideState(inParameterData.widgetMap, inParameterData.parameterPtr->getEnabled());

    return;
  }

  switch(inParameterData.parameterPtr->getVariableType())
  {
    case Variable::eVariableTypeString:

      lWidgetItr = inParameterData.widgetMap.find(eWidgetTypeLineEdit);
      Q_ASSERT(lWidgetItr!=inParameterData.widgetMap.end());
      lLineEdit = qobject_cast<QLineEdit*>(*lWidgetItr);
      Q_ASSERT(lLineEdit);

      lLineEdit->setText(lParameterValue);

      break;

    case Variable::eVariableTypeFile:

      lWidgetItr = inParameterData.widgetMap.find(eWidgetTypeLineEdit);
      Q_ASSERT(lWidgetItr!=inParameterData.widgetMap.end());
      lLineEdit = qobject_cast<QLineEdit*>(*lWidgetItr);
      Q_ASSERT(lLineEdit);

      lLineEdit->setText(lParameterValue);

      break;

    case Variable::eVariableTypeDirectory:

      lWidgetItr = inParameterData.widgetMap.find(eWidgetTypeLineEdit);
      Q_ASSERT(lWidgetItr!=inParameterData.widgetMap.end());
      lLineEdit = qobject_cast<QLineEdit*>(*lWidgetItr);
      Q_ASSERT(lLineEdit);

      lLineEdit->setText(lParameterValue);

      break;

    case Variable::eVariableTypeColor:

      lWidgetItr = inParameterData.widgetMap.find(eWidgetTypePushButton);
      Q_ASSERT(lWidgetItr!=inParameterData.widgetMap.end());
      lPushButton = qobject_cast<QPushButton*>(*lWidgetItr);
      Q_ASSERT(lPushButton);

      lPushButton->setIcon(createIconFromColor(inParameterData.parameterPtr->toColor()));

      break;

    case Variable::eVariableTypeSize:

      lWidgetItr = inParameterData.widgetMap.find(eWidgetTypeSpinBoxWidth);
      lWidgetItr2 = inParameterData.widgetMap.find(eWidgetTypeSpinBoxHeight);
      Q_ASSERT(lWidgetItr!=inParameterData.widgetMap.end() && lWidgetItr2!=inParameterData.widgetMap.end());
      lSpinBox = qobject_cast<QSpinBox*>(*lWidgetItr);
      lSpinBox2 = qobject_cast<QSpinBox*>(*lWidgetItr2);
      Q_ASSERT(lSpinBox && lSpinBox2);

      lSpinBox->setMinimum(0);
      lSpinBox2->setMinimum(0);
      lSpinBox->setMaximum(10000);
      lSpinBox2->setMaximum(10000);

      lSize = inParameterData.parameterPtr->toSize();

      lSpinBox->setValue(lSize[0]);
      lSpinBox2->setValue(lSize[1]);

      break;

    case Variable::eVariableTypeBool:

      lWidgetItr = inParameterData.widgetMap.find(eWidgetTypeCheckBox);
      Q_ASSERT(lWidgetItr!=inParameterData.widgetMap.end());
      lCheckBox = qobject_cast<QCheckBox*>(*lWidgetItr);
      Q_ASSERT(lSpinBox);

      lCheckBox->setChecked(inParameterData.parameterPtr->toBool());

      break;

    case Variable::eVariableTypeUChar:

      lWidgetItr = inParameterData.widgetMap.find(eWidgetTypeSpinBox);
      Q_ASSERT(lWidgetItr!=inParameterData.widgetMap.end());
      lSpinBox = qobject_cast<QSpinBox*>(*lWidgetItr);
      Q_ASSERT(lSpinBox);

      if(lParameterMinValue.isEmpty())
        lSpinBox->setMinimum(0);
      else
        lSpinBox->setMinimum(lParameterMinValue.toInt());

      if(lParameterMaxValue.isEmpty())
        lSpinBox->setMaximum(UCHAR_MAX);
      else
        lSpinBox->setMaximum(lParameterMaxValue.toInt());

      lSpinBox->setValue(lParameterValue.toInt());

      break;

    case Variable::eVariableTypeChar:

      lWidgetItr = inParameterData.widgetMap.find(eWidgetTypeSpinBox);
      Q_ASSERT(lWidgetItr!=inParameterData.widgetMap.end());
      lSpinBox = qobject_cast<QSpinBox*>(*lWidgetItr);
      Q_ASSERT(lSpinBox);

      if(lParameterMinValue.isEmpty())
        lSpinBox->setMinimum(SCHAR_MIN);
      else
        lSpinBox->setMinimum(lParameterMinValue.toInt());

      if(lParameterMaxValue.isEmpty())
        lSpinBox->setMaximum(SCHAR_MAX);
      else
        lSpinBox->setMaximum(lParameterMaxValue.toInt());

      lSpinBox->setValue(lParameterValue.toInt());

      break;

    case Variable::eVariableTypeUShort:

      lWidgetItr = inParameterData.widgetMap.find(eWidgetTypeSpinBox);
      Q_ASSERT(lWidgetItr!=inParameterData.widgetMap.end());
      lSpinBox = qobject_cast<QSpinBox*>(*lWidgetItr);
      Q_ASSERT(lSpinBox);

      if(lParameterMinValue.isEmpty())
        lSpinBox->setMinimum(0);
      else
        lSpinBox->setMinimum(lParameterMinValue.toUShort());

      if(lParameterMaxValue.isEmpty())
        lSpinBox->setMaximum(USHRT_MAX);
      else
        lSpinBox->setMaximum(lParameterMaxValue.toUShort());

      lSpinBox->setValue(lParameterValue.toUShort());

      break;

    case Variable::eVariableTypeShort:

      lWidgetItr = inParameterData.widgetMap.find(eWidgetTypeSpinBox);
      Q_ASSERT(lWidgetItr!=inParameterData.widgetMap.end());
      lSpinBox = qobject_cast<QSpinBox*>(*lWidgetItr);
      Q_ASSERT(lSpinBox);

      if(lParameterMinValue.isEmpty())
        lSpinBox->setMinimum(SHRT_MIN);
      else
        lSpinBox->setMinimum(lParameterMinValue.toShort());

      if(lParameterMaxValue.isEmpty())
        lSpinBox->setMaximum(SHRT_MAX);
      else
        lSpinBox->setMaximum(lParameterMaxValue.toShort());

      lSpinBox->setValue(lParameterValue.toShort());

      break;

    case Variable::eVariableTypeUInt:

      lWidgetItr = inParameterData.widgetMap.find(eWidgetTypeSpinBox);
      Q_ASSERT(lWidgetItr!=inParameterData.widgetMap.end());
      lSpinBox = qobject_cast<QSpinBox*>(*lWidgetItr);
      Q_ASSERT(lSpinBox);

      if(lParameterMinValue.isEmpty())
        lSpinBox->setMinimum(0);
      else
        lSpinBox->setMinimum(lParameterMinValue.toUInt());

      if(!lParameterMaxValue.isEmpty())
        lSpinBox->setMaximum(lParameterMaxValue.toUInt());

      lSpinBox->setValue(lParameterValue.toUInt());

      break;

    case Variable::eVariableTypeInt:

      lWidgetItr = inParameterData.widgetMap.find(eWidgetTypeSpinBox);
      Q_ASSERT(lWidgetItr!=inParameterData.widgetMap.end());
      lSpinBox = qobject_cast<QSpinBox*>(*lWidgetItr);
      Q_ASSERT(lSpinBox);

      if(lParameterMinValue.isEmpty())
        lSpinBox->setMinimum(INT_MIN);
      else
        lSpinBox->setMinimum(lParameterMinValue.toInt());

      if(lParameterMaxValue.isEmpty())
        lSpinBox->setMaximum(INT_MAX);
      else
        lSpinBox->setMaximum(lParameterMaxValue.toInt());

      lSpinBox->setValue(lParameterValue.toInt());

      break;

    case Variable::eVariableTypeULong:

      lWidgetItr = inParameterData.widgetMap.find(eWidgetTypeSpinBox);
      Q_ASSERT(lWidgetItr!=inParameterData.widgetMap.end());
      lSpinBox = qobject_cast<QSpinBox*>(*lWidgetItr);
      Q_ASSERT(lSpinBox);

      if(lParameterMinValue.isEmpty())
        lSpinBox->setMinimum(INT_MIN);
      else
        lSpinBox->setMinimum(lParameterMinValue.toLong());

      if(!lParameterMaxValue.isEmpty())
        lSpinBox->setMaximum(lParameterMaxValue.toLong());

      lSpinBox->setValue(lParameterValue.toLong());

      break;

    case Variable::eVariableTypeLong:

      lWidgetItr = inParameterData.widgetMap.find(eWidgetTypeSpinBox);
      Q_ASSERT(lWidgetItr!=inParameterData.widgetMap.end());
      lSpinBox = qobject_cast<QSpinBox*>(*lWidgetItr);
      Q_ASSERT(lSpinBox);

      if(lParameterMinValue.isEmpty())
        lSpinBox->setMinimum(INT_MIN);
      else
        lSpinBox->setMinimum(lParameterMinValue.toLong());

      if(!lParameterMaxValue.isEmpty())
        lSpinBox->setMaximum(lParameterMaxValue.toLong());

      lSpinBox->setValue(lParameterValue.toLong());

      break;

    case Variable::eVariableTypeFloat:

      lWidgetItr = inParameterData.widgetMap.find(eWidgetTypeDoubleSpinBox);
      Q_ASSERT(lWidgetItr!=inParameterData.widgetMap.end());
      lDoubleSpinBox = qobject_cast<QDoubleSpinBox*>(*lWidgetItr);
      Q_ASSERT(lDoubleSpinBox);

      if(lParameterMinValue.isEmpty())
        lDoubleSpinBox->setMinimum(FLT_MIN);
      else
        lDoubleSpinBox->setMinimum(lParameterMinValue.toFloat());

      if(lParameterMaxValue.isEmpty())
        lDoubleSpinBox->setMaximum(FLT_MAX);
      else
        lDoubleSpinBox->setMaximum(lParameterMaxValue.toFloat());

      lDoubleSpinBox->setValue(lParameterValue.toFloat());

      break;

    case Variable::eVariableTypeDouble:

      lWidgetItr = inParameterData.widgetMap.find(eWidgetTypeDoubleSpinBox);
      Q_ASSERT(lWidgetItr!=inParameterData.widgetMap.end());
      lDoubleSpinBox = qobject_cast<QDoubleSpinBox*>(*lWidgetItr);
      Q_ASSERT(lDoubleSpinBox);

      if(lParameterMinValue.isEmpty())
        lDoubleSpinBox->setMinimum(DBL_MIN);
      else
        lDoubleSpinBox->setMinimum(lParameterMinValue.toDouble());

      if(lParameterMaxValue.isEmpty())
        lDoubleSpinBox->setMaximum(DBL_MAX);
      else
        lDoubleSpinBox->setMaximum(lParameterMaxValue.toDouble());

      lDoubleSpinBox->setValue(lParameterValue.toDouble());

      break;

    default:

      //Should not happen
      Q_ASSERT(0);

      break;
  }

  updateWidgetEnableHideState(inParameterData.widgetMap, inParameterData.parameterPtr->getEnabled());

}

//-------------------------------------------------------------------------------

void ModuleParameterWidget::updateWidgetEnableHideState(WidgetTypeMap& inWidgetTypeMap, bool inParameterState)
{
  foreach(QWidget* lWidget, inWidgetTypeMap)
  {
    lWidget->setEnabled(inParameterState);
    lWidget->setVisible(inParameterState || !mHideDisabled);
  }
}

//-------------------------------------------------------------------------------
