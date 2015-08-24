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

#ifndef VIPERSGUI_MODULESLOTWIDGET_HPP
#define VIPERSGUI_MODULESLOTWIDGET_HPP

#include <QFrame>
#include <QLabel>
#include <QPixmap>
#include <QPoint>
#include <QSet>

#include <ModuleSlot.hpp>

using namespace VIPERS;

class ModuleWidget;
class ModuleSlotLink;

typedef QSet<ModuleSlotLink*> ModuleSlotLinkList;

class ModuleSlotWidget : public QFrame
{
    Q_OBJECT

  public:

    enum Mode
    {
      eModeDisabled,
      eModeNormal,
      eModeEditClick
    };

    enum State
    {
      eStateDisconnected,
      eStateSelected,
      eStateConnected
    };

    explicit ModuleSlotWidget(ModuleWidget* inModuleWidget, const ModuleSlot* inModuleSlot);
    virtual ~ModuleSlotWidget();

    inline const ModuleSlot* getModuleSlot() const {return mModuleSlot;}
    inline ModuleWidget* getModuleWidget() const {return mModuleWidget;}
    Module* getModule() const;

    inline ModuleSlotLinkList& getModuleSlotLinkList() {return mModuleSlotLinkList;}

    void updateState();
    void setState(State inState);

    void updateLinks();

    inline static ModuleSlotWidget* getModuleSlotSelectedFirst() {return mModuleSlotSelectedFirst;}
    inline static void setModuleSlotSelectedFirst(ModuleSlotWidget* inModuleSlotWidget) {mModuleSlotSelectedFirst=inModuleSlotWidget;}
    inline static ModuleSlotWidget* getModuleSlotHighlighted() {return mModuleSlotHighlighted;}
    inline static void setModuleSlotHighlighted(ModuleSlotWidget* inModuleSlotWidget) {mModuleSlotHighlighted=inModuleSlotWidget;}

    inline static bool getSlotSelectionMode() {return mSlotSelectionMode;}
    inline static void setSlotSelectionMode(bool inMode) {mSlotSelectionMode=inMode;}

    inline static Mode getMode() {return mMode;}
    inline static void setMode(Mode inMode) {mMode=inMode;}

  signals:

    void connectModuleSlot(ModuleSlotWidget* inModuleSlotWidget1, ModuleSlotWidget* inModuleSlotWidget2);

  protected:

    void mouseMoveEvent(QMouseEvent* inEvent);
    void mousePressEvent(QMouseEvent* inEvent);

    void enterEvent(QEvent* inEvent);
    void leaveEvent(QEvent* inEvent);

    void keyReleaseEvent(QKeyEvent* inEvent);

    void dragEnterEvent(QDragEnterEvent* inEvent);
    void dragMoveEvent(QDragMoveEvent* inEvent);
    void dropEvent(QDropEvent* inEvent);
    void dragLeaveEvent(QDragLeaveEvent* inEvent);

    bool isSlotCompatible(ModuleSlotWidget* inSlotWidget);

  private:

    void createLayout();
    
    QLabel* mIcon;
    QLabel* mTitle;

    QPoint mDragStartPos;
    
    const ModuleSlot* mModuleSlot;
    ModuleWidget* mModuleWidget;

    ModuleSlotLinkList mModuleSlotLinkList;

    static ModuleSlotWidget* mModuleSlotSelectedFirst;
    static ModuleSlotWidget* mModuleSlotHighlighted;

    static bool mSlotSelectionMode;
    static Mode mMode;
};

#endif //VIPERSGUI_MODULESLOTWIDGET_HPP
