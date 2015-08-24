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

#ifndef VIPERSGUI_MODULEWIDGET_HPP
#define VIPERSGUI_MODULEWIDGET_HPP

#include <QFrame>
#include <QLabel>
#include <QIcon>
#include <QMap>
#include <QPointF>
#include <QVBoxLayout>

#include <Module.hpp>
#include "ModuleSlotWidget.hpp"

class QGraphicsScene;

using namespace VIPERS;

typedef QMap<QString, ModuleSlotWidget*> ModuleSlotWidgetMap;

class ModuleWidget : public QFrame
{
    Q_OBJECT

  public:

    explicit ModuleWidget(Module* ioModule, const QIcon& inIcon);
    virtual ~ModuleWidget();

    inline Module* getModule() const {return mModule;}
    inline ModuleSlotWidgetMap getInputModuleSlotWidget() const {return mInputSlotWidgetMap;}
    inline ModuleSlotWidgetMap getOutputModuleSlotWidget() const {return mOutputSlotWidgetMap;}

    inline static bool getInteractionEnabled() {return mInteractionEnabled;}
    inline static void setInteractionEnabled(bool inEnabled) {mInteractionEnabled=inEnabled;}

    void updateState();

  signals:

    void connectModuleSlot(ModuleSlotWidget* inModuleSlotWidget1, ModuleSlotWidget* inModuleSlotWidget2);
    void focusChanged(ModuleWidget* inModuleWidget, bool inHasFocus);

  protected:

    void createLayout(const QIcon& inIcon);

    void mouseMoveEvent(QMouseEvent *inEvent);
    void mousePressEvent(QMouseEvent *inEvent);
    void focusInEvent(QFocusEvent* inEvent);
    void focusOutEvent(QFocusEvent* inEvent);
    void moveEvent(QMoveEvent* inEvent);
    void closeEvent(QCloseEvent* inEvent);

  protected slots:

    void newModuleSlotConnection(ModuleSlotWidget* inModuleSlotWidget1, ModuleSlotWidget* inModuleSlotWidget2);

  private:

    QFrame* mTitleFrame;
    QLabel* mIcon;
    QLabel* mTitle;
    
    QVBoxLayout* mMainLayout;

    ModuleSlotWidgetMap mInputSlotWidgetMap;
    ModuleSlotWidgetMap mOutputSlotWidgetMap;

    Module* mModule;

    QPointF mMoveInitialPosition;

    static bool mInteractionEnabled;
};

#endif //VIPERSGUI_MODULEWIDGET_HPP
