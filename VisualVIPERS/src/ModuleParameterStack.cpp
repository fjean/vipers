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

#include "ModuleParameterStack.hpp"
#include "ModuleParameterWidget.hpp"

#include <QComboBox>
#include <QScrollArea>
#include <QVBoxLayout>

#include <QDebug>

//-------------------------------------------------------------------------------

ModuleParameterStack::ModuleParameterStack(QWidget* inParent)
  : QWidget(inParent)
{
  createLayout();
}

//-------------------------------------------------------------------------------

ModuleParameterStack::~ModuleParameterStack()
{
  mModuleParameterScrollArea->takeWidget();
  foreach(ModuleParameterWidget* lModuleParameterWidget, mModuleParameterWidgetMap)
    delete lModuleParameterWidget;
}

//-------------------------------------------------------------------------------

void ModuleParameterStack::setRuntimeState(bool inValue)
{
  foreach(ModuleParameterWidget* lModuleParameterWidget, mModuleParameterWidgetMap)
    lModuleParameterWidget->setRuntimeState(inValue);
}

//-------------------------------------------------------------------------------

void ModuleParameterStack::updateModuleParameterWidgets()
{
  foreach(ModuleParameterWidget* lModuleParameterWidget, mModuleParameterWidgetMap)
    lModuleParameterWidget->updateParameters();
}

//-------------------------------------------------------------------------------

void ModuleParameterStack::addModuleInstanceParameter(const ModuleInstanceInfo& inModuleInstanceInfo)
{
  mModuleParameterList->addItem(inModuleInstanceInfo.getIcon(), inModuleInstanceInfo.getLabel());
  ModuleParameterWidget* lModuleParameterWidget = new ModuleParameterWidget(inModuleInstanceInfo.getModule());

  mModuleParameterWidgetMap.insert(inModuleInstanceInfo.getLabel(), lModuleParameterWidget);

  if(mModuleParameterScrollArea->widget()==0)
  {
    mModuleParameterList->setEnabled(true);
    mModuleParameterScrollArea->setWidget(lModuleParameterWidget);
    lModuleParameterWidget->show();
  }
}

//-------------------------------------------------------------------------------

void ModuleParameterStack::removeModuleInstanceParameter(const QString& inLabel)
{
  ModuleParameterWidgetMap::Iterator lModuleParameterWidgetMapItr = mModuleParameterWidgetMap.find(inLabel);

  if(lModuleParameterWidgetMapItr!=mModuleParameterWidgetMap.end())
  {
    if(*lModuleParameterWidgetMapItr==mModuleParameterScrollArea->widget())
    {
      mModuleParameterScrollArea->takeWidget();
    }

    delete lModuleParameterWidgetMapItr.value();

    mModuleParameterWidgetMap.erase(lModuleParameterWidgetMapItr);
    mModuleParameterList->removeItem(mModuleParameterList->findText(inLabel));

    if(mModuleParameterList->currentIndex()>=0)
      activateModuleParameter(mModuleParameterList->currentText());
    else
      mModuleParameterList->setEnabled(false);
  }
  else
  {
    qDebug() << "WARNING: Cannot find parameters widget for module labelled \"" << inLabel << "\"";
  }
}

//-------------------------------------------------------------------------------

void ModuleParameterStack::updateModuleInstanceParameter(const QString& inOldLabel, const QString& inNewLabel)
{
  ModuleParameterWidgetMap::Iterator lModuleParameterWidgetMapItr = mModuleParameterWidgetMap.find(inOldLabel);
  ModuleParameterWidget* lTmpWidget;

  if(lModuleParameterWidgetMapItr!=mModuleParameterWidgetMap.end())
  {
    lTmpWidget = *lModuleParameterWidgetMapItr;
    mModuleParameterWidgetMap.erase(lModuleParameterWidgetMapItr);
    mModuleParameterWidgetMap.insert(inNewLabel, lTmpWidget);
    mModuleParameterList->setItemText(mModuleParameterList->findText(inOldLabel), inNewLabel);
  }
  else
  {
    qDebug() << "WARNING: Cannot find parameters widget for module labelled \"" << inOldLabel << "\"";
  }
}

//-------------------------------------------------------------------------------

void ModuleParameterStack::activateModuleParameter(const QString& inLabel)
{
  ModuleParameterWidgetMap::Iterator lModuleParameterWidgetMapItr = mModuleParameterWidgetMap.find(inLabel);

  if(lModuleParameterWidgetMapItr!=mModuleParameterWidgetMap.end())
  {
    mModuleParameterScrollArea->takeWidget();
    mModuleParameterScrollArea->setWidget(*lModuleParameterWidgetMapItr);
    (*lModuleParameterWidgetMapItr)->show();
  }
  else
  {
    qDebug() << "WARNING: Cannot find parameters widget for module labelled \"" << inLabel << "\"";
  }
}

//-------------------------------------------------------------------------------

void ModuleParameterStack::createLayout()
{
  QVBoxLayout* lLayout = new QVBoxLayout(this);

  mModuleParameterList = new QComboBox();
  lLayout->addWidget(mModuleParameterList, 1);

  mModuleParameterList->setEnabled(false);

  connect(mModuleParameterList, SIGNAL(activated (const QString&)), this, SLOT(activateModuleParameter(const QString&)));

  mModuleParameterScrollArea = new QScrollArea();
  mModuleParameterScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  mModuleParameterScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  mModuleParameterScrollArea->setWidgetResizable(true);
  lLayout->addWidget(mModuleParameterScrollArea, 1);

}
