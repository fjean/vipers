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

#ifndef VIPERSGUI_MODULESLOTLINK_HPP
#define VIPERSGUI_MODULESLOTLINK_HPP

#include <QGraphicsItem>
#include <QPainterPath>
#include <QPolygonF>
#include <QPen>
#include <QBrush>

class ModuleSlotWidget;
class QGraphicsScene;

class ModuleSlotLink : public QGraphicsItem
{

  public:

    enum LinkType
    {
      eLinkTypeStraightLine,
      eLinkTypeLinePath,
      eLinkTypeBezierCurve
    };

    explicit ModuleSlotLink(ModuleSlotWidget* inInputModuleSlotWidget, ModuleSlotWidget* inOutputModuleSlotWidget);
    virtual ~ModuleSlotLink();

    void detach(QGraphicsScene* inQGraphicsScene);

    inline ModuleSlotWidget* getInputModuleSlotWidget() const {return mInputModuleSlotWidget;}
    inline ModuleSlotWidget* getOutputModuleSlotWidget() const {return mOutputModuleSlotWidget;}

    inline static bool getInteractionEnabled() {return mInteractionEnabled;}
    inline static void setInteractionEnabled(bool inEnabled) {mInteractionEnabled=inEnabled;}

    inline static LinkType getDefaultLinkType() {return mDefaultLinkType;}
    inline static void setDefaultLinkType(LinkType inLinkType) {mDefaultLinkType=inLinkType;}

    inline LinkType getLinkType() {return mLinkType;}
    void setLinkType(LinkType inLinkType);

    QRectF boundingRect() const;
    QPainterPath shape() const;

    void updateLink();

    enum { Type = UserType + 1 };
    int type() const { return Type; }

  protected:

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    void focusInEvent(QFocusEvent* inEvent);
    void focusOutEvent(QFocusEvent* inEvent);
    void mousePressEvent(QGraphicsSceneMouseEvent* inEvent);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* inEvent);

  private:

    void updatePath();

    ModuleSlotWidget* mInputModuleSlotWidget;
    ModuleSlotWidget* mOutputModuleSlotWidget;

    LinkType mLinkType;
    static LinkType mDefaultLinkType;

    static bool mInteractionEnabled;
    static double mMinimumModuleDistance;
    static double mPointModuleDistance;
    static double mCtrlPointModuleDistance;
    static double mArrowSize;
    static double mLineWidth;
    static double mModuleDistanceAdjustment;

    static Qt::GlobalColor mNormalColor;
    static Qt::GlobalColor mSelectedColor;

    QPainterPath mPath;
    QPolygonF mArrow;

    QPen mPen;
    QBrush mBrush;
};

#endif //VIPERSGUI_MODULESLOTLINK_HPP
