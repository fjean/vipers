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

#include "ModuleSlotLink.hpp"
#include "ModuleSlotWidget.hpp"
#include "ModuleWidget.hpp"

#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPointF>
#include <QStyleOptionGraphicsItem>
#include <QFocusEvent>
#include <QtGlobal>

//-------------------------------------------------------------------------------

bool ModuleSlotLink::mInteractionEnabled = true;
ModuleSlotLink::LinkType ModuleSlotLink::mDefaultLinkType = ModuleSlotLink::eLinkTypeBezierCurve;
double ModuleSlotLink::mMinimumModuleDistance = 25;
double ModuleSlotLink::mPointModuleDistance = 25;
double ModuleSlotLink::mCtrlPointModuleDistance = 50;
double ModuleSlotLink::mArrowSize = 7;
double ModuleSlotLink::mLineWidth = 3;
double ModuleSlotLink::mModuleDistanceAdjustment = 2;
Qt::GlobalColor ModuleSlotLink::mNormalColor = Qt::black;
Qt::GlobalColor ModuleSlotLink::mSelectedColor = Qt::red;

//-------------------------------------------------------------------------------

ModuleSlotLink::ModuleSlotLink(ModuleSlotWidget* inInputModuleSlotWidget, ModuleSlotWidget* inOutputModuleSlotWidget)
  : QGraphicsItem(NULL)
{
  Q_ASSERT(inInputModuleSlotWidget);
  Q_ASSERT(inOutputModuleSlotWidget);

  mInputModuleSlotWidget = inInputModuleSlotWidget;
  mOutputModuleSlotWidget = inOutputModuleSlotWidget;

  mInputModuleSlotWidget->getModuleSlotLinkList().insert(this);
  mOutputModuleSlotWidget->getModuleSlotLinkList().insert(this);

  mLinkType = mDefaultLinkType;

  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);

  mPen = QPen(mNormalColor, mLineWidth, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin);
  mBrush = QBrush(mNormalColor);

  setZValue(0);
}

//-------------------------------------------------------------------------------

ModuleSlotLink::~ModuleSlotLink()
{

}

//-------------------------------------------------------------------------------

void ModuleSlotLink::detach(QGraphicsScene* inQGraphicsScene)
{
  Q_ASSERT(inQGraphicsScene);
  Q_ASSERT(mInputModuleSlotWidget);
  Q_ASSERT(mOutputModuleSlotWidget);

  inQGraphicsScene->removeItem(this);

  mInputModuleSlotWidget->getModuleSlotLinkList().remove(this);
  mOutputModuleSlotWidget->getModuleSlotLinkList().remove(this);

  mInputModuleSlotWidget = NULL;
  mOutputModuleSlotWidget = NULL;
}

//-------------------------------------------------------------------------------

void ModuleSlotLink::setLinkType(LinkType inLinkType)
{
  mLinkType = inLinkType;
  updateLink();
}

//-------------------------------------------------------------------------------

QRectF ModuleSlotLink::boundingRect() const
{
  QPainterPath lPath = mPath;
  qreal lExtra = mLineWidth / 2.0;
  if(!mArrow.isEmpty())
      lPath.addPolygon(mArrow);
  return lPath.boundingRect().normalized().adjusted(-lExtra, -lExtra, lExtra, lExtra);
}

//-------------------------------------------------------------------------------

QPainterPath ModuleSlotLink::shape() const
{
  QPainterPath lPath = mPath;
  if(!mArrow.isEmpty())
    lPath.addPolygon(mArrow);
  return lPath;
}

//-------------------------------------------------------------------------------

void ModuleSlotLink::updateLink()
{
  prepareGeometryChange();
  updatePath();
  update();
}

//-------------------------------------------------------------------------------

void ModuleSlotLink::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  updatePath();
  QPen lPen = mPen;
  QBrush lBrush = mBrush;

  painter->setClipRect(option->exposedRect);
  painter->setRenderHint(QPainter::Antialiasing, true);
  
  if(mLinkType==eLinkTypeStraightLine)
    lPen.setCapStyle(Qt::RoundCap);
  else if(mLinkType==eLinkTypeLinePath)
    lPen.setCapStyle(Qt::SquareCap);
  else if(mLinkType==eLinkTypeBezierCurve)
    lPen.setCapStyle(Qt::SquareCap);

  if(isSelected())
  {
    lPen.setColor(mSelectedColor);
    lBrush.setColor(mSelectedColor);
  }

  painter->setPen(lPen);
  painter->drawPath(mPath);

  if(mLinkType!=eLinkTypeStraightLine)
  {
    lPen.setWidthF(1);
    painter->setPen(lPen);
    painter->setBrush(lBrush);
    painter->drawPolygon(mArrow);
  }

  painter->setRenderHint(QPainter::Antialiasing, false);
}

//-------------------------------------------------------------------------------

void ModuleSlotLink::focusInEvent(QFocusEvent* inEvent)
{
  if(getInteractionEnabled())
  {
    setZValue(1);
    QGraphicsItem::focusInEvent(inEvent);
  }
  else
    inEvent->ignore();
}

//-------------------------------------------------------------------------------

void ModuleSlotLink::focusOutEvent(QFocusEvent* inEvent)
{
  if(getInteractionEnabled())
  {
    setZValue(0);
    QGraphicsItem::focusOutEvent(inEvent);
  }
  else
    inEvent->ignore();
}

//-------------------------------------------------------------------------------

void ModuleSlotLink::mousePressEvent(QGraphicsSceneMouseEvent* inEvent)
{
  if(getInteractionEnabled())
    QGraphicsItem::mousePressEvent(inEvent);
  else
    inEvent->ignore();
}

//-------------------------------------------------------------------------------

void ModuleSlotLink::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* inEvent)
{
  if(getInteractionEnabled())
  {
    switch(mLinkType)
    {
      case eLinkTypeStraightLine:
        mLinkType = eLinkTypeLinePath;
        break;
      case eLinkTypeLinePath:
        mLinkType = eLinkTypeBezierCurve;
        break;
      case eLinkTypeBezierCurve:
        mLinkType = eLinkTypeStraightLine;
        break;
      default:
        mLinkType = mLinkType;
        break;
    }

    inEvent->accept();
    updateLink();
  }
  else
    inEvent->ignore();
}

//-------------------------------------------------------------------------------

void ModuleSlotLink::updatePath()
{
  QPainterPath lPath;
  QPolygonF lArrow;
  QPointF lArrowPoint;
  QPointF lStartPoint;
  QPointF lStopPoint;
  QPointF lCtrlPoint1;
  QPointF lCtrlPoint2;
  int lArrowDirection = 0; // -1=left, 1=right

  QGraphicsProxyWidget* lInputModuleProxyWidget = mInputModuleSlotWidget->getModuleWidget()->graphicsProxyWidget();
  QGraphicsProxyWidget* lOutputModuleProxyWidget = mOutputModuleSlotWidget->getModuleWidget()->graphicsProxyWidget();

  Q_ASSERT(lInputModuleProxyWidget && lOutputModuleProxyWidget);

  QRectF lInputModuleRect = lInputModuleProxyWidget->sceneBoundingRect();
  QRectF lOutputModuleRect = lOutputModuleProxyWidget->sceneBoundingRect();
  QRectF lInputModuleSlotRect = lInputModuleProxyWidget->mapRectToScene(lInputModuleProxyWidget->subWidgetRect(mInputModuleSlotWidget));
  QRectF lOutputModuleSlotRect = lOutputModuleProxyWidget->mapRectToScene(lOutputModuleProxyWidget->subWidgetRect(mOutputModuleSlotWidget));

  if(mLinkType==eLinkTypeStraightLine)
  {
    if(lInputModuleRect.x()<lOutputModuleRect.x())
    {
      lPath.moveTo(QPointF(lInputModuleRect.right(), lInputModuleSlotRect.center().y()));
      lPath.lineTo(QPointF(lOutputModuleRect.left(), lOutputModuleSlotRect.center().y()));
    }
    else
    {
      lPath.moveTo(QPointF(lOutputModuleRect.right(), lOutputModuleSlotRect.center().y()));
      lPath.lineTo(QPointF(lInputModuleRect.left(), lInputModuleSlotRect.center().y()));
    }
  }
  else
  {
    lInputModuleRect.setLeft(lInputModuleRect.left() - mModuleDistanceAdjustment);
    lInputModuleRect.setRight(lInputModuleRect.right() + mModuleDistanceAdjustment);
    lOutputModuleRect.setLeft(lOutputModuleRect.left() - mModuleDistanceAdjustment);
    lOutputModuleRect.setRight(lOutputModuleRect.right() + mModuleDistanceAdjustment);

    if(lInputModuleRect.x()<lOutputModuleRect.x())
    {
      if((lInputModuleRect.right()+mMinimumModuleDistance)>=lOutputModuleRect.x())
      {
        qreal lMiddleXPos = lOutputModuleRect.right() + mCtrlPointModuleDistance;
        lStartPoint = QPointF(lOutputModuleRect.right(), lOutputModuleSlotRect.center().y());
        lCtrlPoint1 = QPointF(lMiddleXPos, lOutputModuleSlotRect.center().y());
        lCtrlPoint2 = QPointF(lMiddleXPos, lInputModuleSlotRect.center().y());
        lStopPoint = QPointF(lInputModuleRect.right()+mArrowSize-1, lInputModuleSlotRect.center().y());
        lArrowDirection = -1;
        lArrowPoint = QPointF(lInputModuleRect.right(), lInputModuleSlotRect.center().y());
      }
      else
      {
        qreal lMiddleXPos = (lInputModuleRect.right() + lOutputModuleRect.left())/2.0;
        lStartPoint = QPointF(lOutputModuleRect.left(), lOutputModuleSlotRect.center().y());
        lCtrlPoint1 = QPointF(lMiddleXPos, lOutputModuleSlotRect.center().y());
        lCtrlPoint2 = QPointF(lMiddleXPos, lInputModuleSlotRect.center().y());
        lStopPoint = QPointF(lInputModuleRect.right()+mArrowSize-1, lInputModuleSlotRect.center().y());
        lArrowDirection = -1;
        lArrowPoint = QPointF(lInputModuleRect.right(), lInputModuleSlotRect.center().y());
      }
    }
    else if(lInputModuleRect.right()>lOutputModuleRect.right())
    {
      if((lInputModuleRect.left()-mMinimumModuleDistance)<=lOutputModuleRect.right())
      {
        qreal lMiddleXPos = lOutputModuleRect.left() - mCtrlPointModuleDistance;
        lStartPoint = QPointF(lOutputModuleRect.left(), lOutputModuleSlotRect.center().y());
        lCtrlPoint1 = QPointF(lMiddleXPos, lOutputModuleSlotRect.center().y());
        lCtrlPoint2 = QPointF(lMiddleXPos, lInputModuleSlotRect.center().y());
        lStopPoint = QPointF(lInputModuleRect.left()-mArrowSize+1, lInputModuleSlotRect.center().y());
        lArrowDirection = 1;
        lArrowPoint = QPointF(lInputModuleRect.left(), lInputModuleSlotRect.center().y());
      }
      else
      {
        qreal lMiddleXPos = (lInputModuleRect.x() + lOutputModuleRect.right())/2.0;
        lStartPoint = QPointF(lOutputModuleRect.right(), lOutputModuleSlotRect.center().y());
        lCtrlPoint1 = QPointF(lMiddleXPos, lOutputModuleSlotRect.center().y());
        lCtrlPoint2 = QPointF(lMiddleXPos, lInputModuleSlotRect.center().y());
        lStopPoint = QPointF(lInputModuleRect.left()-mArrowSize+1, lInputModuleSlotRect.center().y());
        lArrowDirection = 1;
        lArrowPoint = QPointF(lInputModuleRect.left(), lInputModuleSlotRect.center().y());
      }
    }
    else
    {
      if((lOutputModuleRect.right()-lInputModuleRect.right())>(lInputModuleRect.x()-lOutputModuleRect.x()))
      {
        qreal lMiddleXPos = lOutputModuleRect.right() + mCtrlPointModuleDistance;
        lStartPoint = QPointF(lOutputModuleRect.right(), lOutputModuleSlotRect.center().y());
        lCtrlPoint1 = QPointF(lMiddleXPos, lOutputModuleSlotRect.center().y());
        lCtrlPoint2 = QPointF(lMiddleXPos, lInputModuleSlotRect.center().y());
        lStopPoint = QPointF(lInputModuleRect.right()+mArrowSize-1, lInputModuleSlotRect.center().y());
        lArrowDirection = -1;
        lArrowPoint = QPointF(lInputModuleRect.right(), lInputModuleSlotRect.center().y());
      }
      else
      {
        qreal lMiddleXPos = lOutputModuleRect.left() - mCtrlPointModuleDistance;
        lStartPoint = QPointF(lOutputModuleRect.left(), lOutputModuleSlotRect.center().y());
        lCtrlPoint1 = QPointF(lMiddleXPos, lOutputModuleSlotRect.center().y());
        lCtrlPoint2 = QPointF(lMiddleXPos, lInputModuleSlotRect.center().y());
        lStopPoint = QPointF(lInputModuleRect.left()-mArrowSize+1, lInputModuleSlotRect.center().y());
        lArrowDirection = 1;
        lArrowPoint = QPointF(lInputModuleRect.left(), lInputModuleSlotRect.center().y());
      }
    }

    if(lArrowDirection==1)
      lArrow << lArrowPoint << QPointF(lArrowPoint.x()-mArrowSize, lArrowPoint.y()-mArrowSize) << QPointF(lArrowPoint.x()-mArrowSize, lArrowPoint.y()+mArrowSize) << lArrowPoint;
    else
      lArrow << lArrowPoint << QPointF(lArrowPoint.x()+mArrowSize, lArrowPoint.y()-mArrowSize) << QPointF(lArrowPoint.x()+mArrowSize, lArrowPoint.y()+mArrowSize) << lArrowPoint;

    if(mLinkType==eLinkTypeLinePath)
    {
      lPath.moveTo(lStartPoint);
      lPath.lineTo(lCtrlPoint1);
      lPath.lineTo(lCtrlPoint2);
      lPath.lineTo(lStopPoint);
    }
    else if(mLinkType==eLinkTypeBezierCurve)
    {

      lPath.moveTo(lStartPoint);
      lPath.cubicTo(lCtrlPoint1, lCtrlPoint2, lStopPoint);
    }
  }

  mPath = lPath;
  mArrow = lArrow;
}
