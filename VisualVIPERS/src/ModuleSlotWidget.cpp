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

#include "ModuleSlotWidget.hpp"
#include "ModuleSlotLink.hpp"
#include "ModuleWidget.hpp"
#include "SharedObjects.hpp"
#include "VisualVipersConfig.hpp"

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QMimeData>
#include <QDrag>
#include <QtGlobal>
#include <QDebug>

//-------------------------------------------------------------------------------

 ModuleSlotWidget* ModuleSlotWidget::mModuleSlotSelectedFirst = 0;
 ModuleSlotWidget* ModuleSlotWidget::mModuleSlotHighlighted = 0;
 bool ModuleSlotWidget::mSlotSelectionMode = false;
 ModuleSlotWidget::Mode ModuleSlotWidget::mMode = ModuleSlotWidget::eModeNormal;

//-------------------------------------------------------------------------------

ModuleSlotWidget::ModuleSlotWidget(ModuleWidget* inModuleWidget, const ModuleSlot* inModuleSlot)
  : QFrame(NULL)
{
  Q_ASSERT(inModuleWidget);
  Q_ASSERT(inModuleSlot);

  mModuleWidget = inModuleWidget;
  mModuleSlot = inModuleSlot;
  //setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  setAcceptDrops(true);
  createLayout();
}

//-------------------------------------------------------------------------------

ModuleSlotWidget::~ModuleSlotWidget()
{
  
}

//-------------------------------------------------------------------------------

Module* ModuleSlotWidget::getModule() const
{
  return const_cast<Module*>(mModuleSlot->getModule());
}

//-------------------------------------------------------------------------------

void ModuleSlotWidget::createLayout()
{
  mTitle = new QLabel(mModuleSlot->getDisplayName().c_str());
  mTitle->setAlignment(Qt::AlignLeft);
  mIcon = new QLabel();

  setState(eStateDisconnected);
  
  QHBoxLayout* lHLayout = new QHBoxLayout();
  
  #ifdef Q_OS_WIN32
  lHLayout->setContentsMargins(2,1,2,1);
  #else
  lHLayout->setContentsMargins(2,3,2,3);
  #endif
  
  lHLayout->addWidget(mIcon, 0, Qt::AlignLeft);
  lHLayout->addWidget(mTitle, 1, Qt::AlignLeft);

  setLayout(lHLayout);
}

//-------------------------------------------------------------------------------

void ModuleSlotWidget::updateState()
{
  if(mModuleSlot->isConnected())
    setState(eStateConnected);
  else
    setState(eStateDisconnected);
}

//-------------------------------------------------------------------------------

void ModuleSlotWidget::setState(State inState)
{
  if(inState==eStateConnected)
    mIcon->setPixmap(SharedObjects::instance()->getPixmapSlotConnected());
  else if(inState==eStateSelected)
    mIcon->setPixmap(SharedObjects::instance()->getPixmapSlotSelected());
  else
    mIcon->setPixmap(SharedObjects::instance()->getPixmapSlotDisconnected());
}

//-------------------------------------------------------------------------------

void ModuleSlotWidget::updateLinks()
{
  ModuleSlotLinkList::iterator lModuleSlotLinkItr;
  for(lModuleSlotLinkItr = mModuleSlotLinkList.begin(); lModuleSlotLinkItr!=mModuleSlotLinkList.end(); lModuleSlotLinkItr++)
    (*lModuleSlotLinkItr)->updateLink();
}

//-------------------------------------------------------------------------------

void ModuleSlotWidget::mouseMoveEvent(QMouseEvent* inEvent)
{
  // Drag and drop mode
  if(getMode()==eModeNormal && (inEvent->buttons() & Qt::LeftButton) && inEvent->modifiers()==Qt::NoModifier)
  {
    inEvent->accept();
    setState(eStateSelected);

    QDrag* lDrag = new QDrag(this);
    QMimeData* lMimeData = new QMimeData();
    QString lModuleLabel(mModuleSlot->getModule()->getLabel().c_str());
    QString lModuleSlotName(mModuleSlot->getName().c_str());
    QString lModuleSlotType;

    if(mModuleSlot->getType()==ModuleSlot::eSlotTypeInput)
      lModuleSlotType = "input";
    else
      lModuleSlotType = "output";

    lMimeData->setData("vipers/module-label", lModuleLabel.toAscii());
    lMimeData->setData("vipers/module-slot-name", lModuleSlotName.toAscii());
    lMimeData->setData("vipers/module-slot-type", lModuleSlotType.toAscii());
    lDrag->setMimeData(lMimeData);
    lDrag->setPixmap(SharedObjects::instance()->getPixmapSlotConnectDrag());
    lDrag->setHotSpot(QPoint(0,-16));

    setModuleSlotSelectedFirst(this);
    lDrag->exec(Qt::CopyAction);
	  
	// Workaround for bug in OSX: the dropEvent of the destination slot is never called...  
#ifdef Q_OS_DARWIN
	if( ModuleSlotWidget::getModuleSlotHighlighted())
	{
		emit connectModuleSlot(this, getModuleSlotHighlighted());
		ModuleSlotWidget::setModuleSlotSelectedFirst(NULL);
		ModuleSlotWidget::setModuleSlotHighlighted(NULL);
	}
#endif
	  
    updateState();
  }
  else
    QFrame::mouseMoveEvent(inEvent);
}

//-------------------------------------------------------------------------------

void ModuleSlotWidget::mousePressEvent(QMouseEvent* inEvent)
{
  if(getMode()==eModeEditClick)
  {
    inEvent->accept();
    if(inEvent->button() == Qt::LeftButton)
    {
      if(getModuleSlotSelectedFirst()==NULL)
      {
        if((mModuleSlot->getType()==ModuleSlot::eSlotTypeInput && !mModuleSlot->isConnected()) || mModuleSlot->getType()==ModuleSlot::eSlotTypeOutput)
        {
          setModuleSlotHighlighted(NULL);
          setModuleSlotSelectedFirst(this);
          setState(eStateSelected);
          inEvent->accept();
        }
        else
        {
          inEvent->accept();
          QApplication::beep();
        }
      }
      else
      {
        if(getModuleSlotSelectedFirst()==this)
        {
          inEvent->accept();
          setModuleSlotHighlighted(NULL);
          setModuleSlotSelectedFirst(NULL);
          updateState();
        }
        else if(isSlotCompatible(getModuleSlotSelectedFirst()))
        {
          inEvent->accept();
          emit connectModuleSlot(this, getModuleSlotSelectedFirst());
          setModuleSlotSelectedFirst(NULL);
          setModuleSlotHighlighted(NULL);
        }
        else
        {
          inEvent->accept();
          QApplication::beep();
        }
      }
    }
    else
      QFrame::mousePressEvent(inEvent);
  }
  else if(getMode()==eModeNormal)
  {
    // Drag and drop mode
    if(inEvent->button() == Qt::LeftButton && inEvent->modifiers()==Qt::NoModifier)
    {
      mDragStartPos = inEvent->pos();
      inEvent->accept();
    }
    // CTRL left click mode
    else if(inEvent->button() == Qt::LeftButton && inEvent->modifiers()==Qt::ControlModifier)
    {
      if(getModuleSlotSelectedFirst()==NULL)
      {
        if((mModuleSlot->getType()==ModuleSlot::eSlotTypeInput && !mModuleSlot->isConnected()) || mModuleSlot->getType()==ModuleSlot::eSlotTypeOutput)
        {
          inEvent->accept();
          setModuleSlotSelectedFirst(this);
          setModuleSlotHighlighted(NULL);
          setState(eStateSelected);
        }
        else
        {
          inEvent->accept();
          QApplication::beep();
        }
      }
      else
      {
        if(isSlotCompatible(getModuleSlotSelectedFirst()))
        {
          inEvent->accept();
          emit connectModuleSlot(this, getModuleSlotSelectedFirst());
          setModuleSlotSelectedFirst(NULL);
          setModuleSlotHighlighted(NULL);
        }
        else
        {
          inEvent->accept();
          QApplication::beep();
        }
      }
    }
    else
      QFrame::mousePressEvent(inEvent);
  }
  else
    QFrame::mousePressEvent(inEvent);
}

//-------------------------------------------------------------------------------

void ModuleSlotWidget::enterEvent(QEvent* inEvent)
{

  if(getMode()==eModeEditClick || (getMode()==eModeNormal && QApplication::keyboardModifiers()==Qt::ControlModifier))
  {
    // First chosen slot
    if(getModuleSlotSelectedFirst()==NULL)
    {
      if((mModuleSlot->getType()==ModuleSlot::eSlotTypeInput && !mModuleSlot->isConnected()) || mModuleSlot->getType()==ModuleSlot::eSlotTypeOutput)
      {
        setModuleSlotHighlighted(this);
        setState(eStateSelected);
      }
      else
      {
        setModuleSlotHighlighted(NULL);
      }
      inEvent->accept();
    }
    else // Second slot
    {
      if(isSlotCompatible(getModuleSlotSelectedFirst()))
      {
        setModuleSlotHighlighted(this);
        setState(eStateSelected);
      }
      else
      {
        setModuleSlotHighlighted(NULL);
      }
      inEvent->accept();
    }
  }
  else
    inEvent->ignore();
}

//-------------------------------------------------------------------------------

void ModuleSlotWidget::leaveEvent(QEvent* inEvent)
{
  if((getMode()==eModeEditClick || (getMode()==eModeNormal && QApplication::keyboardModifiers()==Qt::ControlModifier)) && this!=getModuleSlotSelectedFirst())
  {
    setModuleSlotHighlighted(NULL);
    inEvent->accept();
    updateState();
  }
  else
    inEvent->ignore();
}

//-------------------------------------------------------------------------------

void ModuleSlotWidget::keyReleaseEvent(QKeyEvent* inEvent)
{
  if(inEvent->key()==Qt::Key_Control && getMode()==eModeNormal)
  {
    updateState();
    if(getModuleSlotSelectedFirst()!=NULL)
    {
      getModuleSlotSelectedFirst()->updateState();
      setModuleSlotSelectedFirst(NULL);
    }
    setModuleSlotHighlighted(NULL);
    inEvent->accept();
  }
  else
    QFrame::keyReleaseEvent(inEvent);
}

//-------------------------------------------------------------------------------

void ModuleSlotWidget::dragEnterEvent(QDragEnterEvent* inEvent)
{
  if(getMode()==eModeNormal && inEvent->mimeData()->hasFormat("vipers/module-label") && inEvent->mimeData()->hasFormat("vipers/module-slot-name") && inEvent->mimeData()->hasFormat("vipers/module-slot-type"))
  {
    if(isSlotCompatible(getModuleSlotSelectedFirst()))
    {
	  setModuleSlotHighlighted(this);
      setState(eStateSelected);
      inEvent->acceptProposedAction();
    }
    else
      inEvent->ignore();
  }
  else
    inEvent->ignore();
}

//-------------------------------------------------------------------------------

void ModuleSlotWidget::dragMoveEvent(QDragMoveEvent* inEvent)
{
  if(getMode()==eModeNormal && inEvent->mimeData()->hasFormat("vipers/module-label") && inEvent->mimeData()->hasFormat("vipers/module-slot-name") && inEvent->mimeData()->hasFormat("vipers/module-slot-type"))
  {
    if(isSlotCompatible(getModuleSlotSelectedFirst()))
      inEvent->acceptProposedAction();
    else
      inEvent->ignore();
  }
  else
    inEvent->ignore();
}

//-------------------------------------------------------------------------------

void ModuleSlotWidget::dropEvent(QDropEvent* inEvent)
{ 
#ifndef Q_OS_DARWIN	
  if(getMode()==eModeNormal && inEvent->mimeData()->hasFormat("vipers/module-label") && inEvent->mimeData()->hasFormat("vipers/module-slot-name") && inEvent->mimeData()->hasFormat("vipers/module-slot-type"))
  { 
    if(isSlotCompatible(getModuleSlotSelectedFirst()))
    {
      emit connectModuleSlot(this, getModuleSlotSelectedFirst());
      setModuleSlotSelectedFirst(NULL);
      updateState();
      inEvent->acceptProposedAction();
    }
    else
      inEvent->ignore();
  }
  else
    inEvent->ignore();
#endif
}

//-------------------------------------------------------------------------------

void ModuleSlotWidget::dragLeaveEvent(QDragLeaveEvent* inEvent)
{
  if(getMode()==eModeNormal)
  {
	setModuleSlotHighlighted(NULL);
    updateState();
    inEvent->accept();
  }
  else
    inEvent->ignore();
}

//-------------------------------------------------------------------------------

bool ModuleSlotWidget::isSlotCompatible(ModuleSlotWidget* inSlotWidget)
{
  if(inSlotWidget && mModuleSlot->getModule()!=inSlotWidget->mModuleSlot->getModule() &&
      ((mModuleSlot->getType()==ModuleSlot::eSlotTypeInput && inSlotWidget->mModuleSlot->getType()==ModuleSlot::eSlotTypeOutput && !mModuleSlot->isConnected()) ||
          (mModuleSlot->getType()==ModuleSlot::eSlotTypeOutput && inSlotWidget->mModuleSlot->getType()==ModuleSlot::eSlotTypeInput && !inSlotWidget->mModuleSlot->isConnected())))
    return true;
  else
    return false;
}

//-------------------------------------------------------------------------------
