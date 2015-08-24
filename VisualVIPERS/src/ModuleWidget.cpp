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

#include "ModuleWidget.hpp"
#include "SharedObjects.hpp"
#include "MainWindow.hpp"

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QGraphicsProxyWidget>
#include <QFont>
#include <QFocusEvent>
#include <QtGlobal>

#include <QDebug>

#include <iostream>
using namespace std;

//-------------------------------------------------------------------------------

bool ModuleWidget::mInteractionEnabled = true;

//-------------------------------------------------------------------------------

ModuleWidget::ModuleWidget(Module* ioModule, const QIcon& inIcon)
  : QFrame(NULL)
{
  Q_ASSERT(ioModule);

  mModule = ioModule;

  #if WIN32
  setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
  setLineWidth(3);
  #else
  setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
  #endif

  setFocusPolicy(Qt::StrongFocus);
  //setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

  createLayout(inIcon);
}

//-------------------------------------------------------------------------------

ModuleWidget::~ModuleWidget()
{
  
}

//-------------------------------------------------------------------------------

void ModuleWidget::updateState()
{
  mTitle->setText(mModule->getLabel().c_str());
  //resize(1,1);
}

//-------------------------------------------------------------------------------

void ModuleWidget::createLayout(const QIcon& inIcon)
{
  ModuleSlotWidget* lModuleSlotWidget;
  ModuleSlotMap::const_iterator lSlotItr;
  QFont lFont;

  mTitleFrame = new QFrame();
  mTitleFrame->setAutoFillBackground(true);

  /*
  #ifdef WIN32
  mTitleFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
  #else
  mTitleFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
  #endif
  */

  mTitleFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);

  mIcon = new QLabel();
  mIcon->setPixmap(inIcon.pixmap(QSize(16,16)));
  mIcon->setAutoFillBackground(true);

  mTitle = new QLabel(mModule->getLabel().c_str());
  mTitle->setAutoFillBackground(true);
  lFont = mTitle->font();
  lFont.setWeight(QFont::Bold);
  mTitle->setFont(lFont);
  lFont.setWeight(QFont::Normal);
  lFont.setPointSize(lFont.pointSize()-1);
	
  mTitle->setAlignment(Qt::AlignCenter);
  mTitle->setContentsMargins(5,0,5,0);

  QHBoxLayout* lTitleLayout = new QHBoxLayout();
  lTitleLayout->setContentsMargins(2,3,2,3);

  lTitleLayout->addWidget(mIcon, 0, Qt::AlignLeft );
  lTitleLayout->addWidget(mTitle, 1, Qt::AlignHCenter | Qt::AlignVCenter);

  //mTitleFrame->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  mTitleFrame->setLayout(lTitleLayout);

  mMainLayout = new QVBoxLayout();
  mMainLayout->setContentsMargins(2,2,2,2);
  mMainLayout->addWidget(mTitleFrame, 1, Qt::AlignTop);



  const ModuleSlotMap& lInputSlotsMap = mModule->getInputSlots();
  const ModuleSlotMap& lOutputSlotsMap = mModule->getOutputSlots();

  // Inputs separator
  if(lInputSlotsMap.size())
  {
    QHBoxLayout* lInputSeparatorLayout = new QHBoxLayout();
    lInputSeparatorLayout->setContentsMargins(1,0,1,0);
	
    QFrame* lInputSeparator1= new QFrame();
    QFrame* lInputSeparator2 = new QFrame();
    lInputSeparator1->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    lInputSeparator2->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    //lInputSeparator1->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    //lInputSeparator2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    QLabel* lInputSeparatorText = new QLabel();
    if(lInputSlotsMap.size()==1)
      lInputSeparatorText->setText(tr("Input"));
    else
      lInputSeparatorText->setText(tr("Inputs"));
    lInputSeparatorText->setFont(lFont);
    lInputSeparatorText->setContentsMargins(3,0,3,0);

    lInputSeparatorLayout->addWidget(lInputSeparator1, 1);
    lInputSeparatorLayout->addWidget(lInputSeparatorText, 0, Qt::AlignHCenter);
    lInputSeparatorLayout->addWidget(lInputSeparator2, 1);
    mMainLayout->addLayout(lInputSeparatorLayout, 1);
  }

  // Input slots
  for(lSlotItr=lInputSlotsMap.begin(); lSlotItr!=lInputSlotsMap.end(); lSlotItr++)
  {
    lModuleSlotWidget = new ModuleSlotWidget(this, lSlotItr->second);
    connect(lModuleSlotWidget, SIGNAL(connectModuleSlot(ModuleSlotWidget*,ModuleSlotWidget*)), this, SLOT(newModuleSlotConnection(ModuleSlotWidget*,ModuleSlotWidget*)));
    mInputSlotWidgetMap.insert(lSlotItr->second->getName().c_str(), lModuleSlotWidget);
    //lModuleSlotWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    mMainLayout->addWidget(lModuleSlotWidget, 1);
  }

  // Outputs separator
  if(lOutputSlotsMap.size())
  {
    QHBoxLayout* lOutputSeparatorLayout = new QHBoxLayout();
    lOutputSeparatorLayout->setContentsMargins(1,0,1,0);
	
    QFrame* lOutputSeparator1= new QFrame();
    QFrame* lOutputSeparator2 = new QFrame();
    lOutputSeparator1->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    lOutputSeparator2->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    //lOutputSeparator1->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    //lOutputSeparator2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    QLabel* lOutputSeparatorText = new QLabel();
    if(lOutputSlotsMap.size()==1)
      lOutputSeparatorText->setText(tr("Output"));
    else
      lOutputSeparatorText->setText(tr("Outputs"));
    lOutputSeparatorText->setFont(lFont);
    lOutputSeparatorText->setContentsMargins(3,0,3,0);

    lOutputSeparatorLayout->addWidget(lOutputSeparator1, 1);
    lOutputSeparatorLayout->addWidget(lOutputSeparatorText, 0, Qt::AlignHCenter);
    lOutputSeparatorLayout->addWidget(lOutputSeparator2, 1);
    mMainLayout->addLayout(lOutputSeparatorLayout, 1);
  }

  // Output slots
  for(lSlotItr=lOutputSlotsMap.begin(); lSlotItr!=lOutputSlotsMap.end(); lSlotItr++)
  {
    lModuleSlotWidget = new ModuleSlotWidget(this, lSlotItr->second);
    connect(lModuleSlotWidget, SIGNAL(connectModuleSlot(ModuleSlotWidget*,ModuleSlotWidget*)), this, SLOT(newModuleSlotConnection(ModuleSlotWidget*,ModuleSlotWidget*)));
    mOutputSlotWidgetMap.insert(lSlotItr->second->getName().c_str(), lModuleSlotWidget);
    //lModuleSlotWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    mMainLayout->addWidget(lModuleSlotWidget, 1);
  }

  mMainLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
  setLayout(mMainLayout);
  resize(1,1);
 }

//-------------------------------------------------------------------------------

void ModuleWidget::mouseMoveEvent(QMouseEvent *inEvent)
{
  QFrame::mouseMoveEvent(inEvent);

  if(!inEvent->isAccepted() && getInteractionEnabled())
  {
    #ifdef Q_OS_DARWIN
      graphicsProxyWidget()->setPos(graphicsProxyWidget()->mapToScene(inEvent->globalPos())-mMoveInitialPosition);
    #else
      graphicsProxyWidget()->setPos(graphicsProxyWidget()->mapToScene(inEvent->pos())-mMoveInitialPosition);
    #endif
	  inEvent->accept();
  }
  else
    inEvent->ignore();
}

//-------------------------------------------------------------------------------

void ModuleWidget::mousePressEvent(QMouseEvent *inEvent)
{
  QFrame::mousePressEvent(inEvent);

  if(!inEvent->isAccepted() && inEvent->button()==Qt::LeftButton && getInteractionEnabled() && (childAt(inEvent->pos())==mTitleFrame || childAt(inEvent->pos())==mIcon || childAt(inEvent->pos())==mTitle))
  {
    #ifdef Q_OS_DARWIN
      mMoveInitialPosition = graphicsProxyWidget()->mapToScene(inEvent->globalPos()) - graphicsProxyWidget()->scenePos();
    #else
      mMoveInitialPosition = graphicsProxyWidget()->mapToScene(inEvent->pos()) - graphicsProxyWidget()->scenePos();
    #endif
	inEvent->accept();
  }
  else
    inEvent->ignore();
}

//-------------------------------------------------------------------------------

void ModuleWidget::focusInEvent(QFocusEvent* inEvent)
{
  if(getInteractionEnabled())
  {
    QFrame::focusInEvent(inEvent);

    /*
    #ifdef WIN32
    mTitleFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    #else
    mTitleFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    #endif
    */

	mTitleFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	
    mTitleFrame->setBackgroundRole(QPalette::Highlight);
    mTitle->setForegroundRole(QPalette::HighlightedText);

    graphicsProxyWidget()->setZValue(3);

    emit focusChanged(this, true);
  }
  else
    inEvent->ignore();
}

//-------------------------------------------------------------------------------

void ModuleWidget::focusOutEvent(QFocusEvent* inEvent)
{
  if(getInteractionEnabled())
  {
    QFrame::focusOutEvent(inEvent);

    /*
    #ifdef WIN32
    mTitleFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
    #else
    mTitleFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    #endif
    graphicsProxyWidget()->setZValue(2);
    */

	mTitleFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
	
    mTitleFrame->setBackgroundRole(QPalette::Window);
    mTitle->setForegroundRole(QPalette::WindowText);

    if(inEvent->reason()!=Qt::PopupFocusReason)
      emit focusChanged(this, false);
  }
  else
    inEvent->ignore();
}

//-------------------------------------------------------------------------------

void ModuleWidget::moveEvent(QMoveEvent* inEvent)
{
  QFrame::moveEvent(inEvent);

  ModuleSlotWidgetMap::iterator lInputSlotWidgetItr = mInputSlotWidgetMap.begin();
  ModuleSlotWidgetMap::iterator lOutputSlotWidgetItr = mOutputSlotWidgetMap.begin();

  for(lInputSlotWidgetItr; lInputSlotWidgetItr!=mInputSlotWidgetMap.end(); lInputSlotWidgetItr++)
    lInputSlotWidgetItr.value()->updateLinks();
  for(lOutputSlotWidgetItr; lOutputSlotWidgetItr!=mOutputSlotWidgetMap.end(); lOutputSlotWidgetItr++)
    lOutputSlotWidgetItr.value()->updateLinks();
}

//-------------------------------------------------------------------------------

void ModuleWidget::closeEvent(QCloseEvent* inEvent)
{
  QFrame::closeEvent(inEvent);
}

//-------------------------------------------------------------------------------

void ModuleWidget::newModuleSlotConnection(ModuleSlotWidget* inModuleSlotWidget1, ModuleSlotWidget* inModuleSlotWidget2)
{
  Q_ASSERT(inModuleSlotWidget1 && inModuleSlotWidget2);
  emit connectModuleSlot(inModuleSlotWidget1, inModuleSlotWidget2);
}
