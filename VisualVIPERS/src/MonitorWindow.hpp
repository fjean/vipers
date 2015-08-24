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

#ifndef VIPERSGUI_MONITORWINDOW_HPP
#define VIPERSGUI_MONITORWINDOW_HPP

#include <QDialog>
#include <QAction>
#include <QString>
#include <QMap>

#include <Monitor.hpp>
#include <Image.hpp>

using namespace VIPERS;

#include "ModuleInstanceInfo.hpp"

class QAction;
class QScrollArea;
class QVBoxLayout;
class QHBoxLayout;
class QComboBox;
class QSlider;
class QToolBar;
class QLabel;
class QImage;

typedef QMap<QString, ModuleInstanceInfo> ModuleInstanceInfoMap;

class MonitorWindow: public QDialog, public Monitor
{
    Q_OBJECT

  public:

    MonitorWindow(QWidget* inParent = NULL);
    virtual ~MonitorWindow();

    virtual void notify();

    inline void setMonitorAccessAction(QAction* inAction) {mMonitorAccessAction=inAction;}
    inline QAction* getMonitorAccessAction() const {return mMonitorAccessAction;}

    inline QString getLabel() const {return mMonitorLabel;}

  signals:

    void closeRequest(MonitorWindow* inMonitorWindow);

  public slots:

    void updateImage();
    void updateModuleInstanceInfoMap(const ModuleInstanceInfoMap& inModuleInstanceInfoMap);

  protected slots:

    void magnifyPlus();
    void magnifyMinus();
    void magnifyFit();
    void magnifyOriginal();
    void goFullScreen();
    void zoomChange(int inValue);
    void activateModule(const QString& inStr);
    void activateModuleSlot(const QString& inStr);

  protected:

    void closeEvent(QCloseEvent* inEvent);
    void resizeEvent(QResizeEvent* inEvent);
    void resizeImage();
    void resizeFitImage();
    void resizeOriginalImage();

  private:

    enum ImageState
    {
      eImageStateNone,
      eImageStateNoModule,
      eImageStateNoSlot,
      eImageStateNotInitialized,
      eImageStateValid,
      eImageStateUnsupportedFormat
    };

    void createActions();
    void createLayout();
    bool updateImageState(ImageState inImageState);

    QAction* mActionMagnifyPlus;
    QAction* mActionMagnifyMinus;
    QAction* mActionMagnifyFit;
    QAction* mActionMagnifyOriginal;
    QAction* mActionFullScreen;

    QVBoxLayout* mMainLayout;
    QHBoxLayout* mModuleLayout;
    QToolBar* mToolBar;
    QScrollArea* mScrollArea;
    QComboBox* mModuleComboBox;
    QComboBox* mModuleSlotComboBox;
    QSlider* mZoomSlider;
    QLabel* mImage;

    QString mMonitorLabel;
    QAction* mMonitorAccessAction;

    bool mZoomChange;

    int mCurrentModuleIndex;
    int mCurrentModuleSlotIndex;

    ModuleInstanceInfoMap mModuleInstanceInfoMap;

    inline static unsigned int getID() {return ++mMonitorIDCount;}
    static unsigned int mMonitorIDCount;

    unsigned int mImageWidth;
    unsigned int mImageHeight;
    Image::Depth mImageDepth;
    Image::Channel mImageNbChannels;
    Image::Model mImageColorModel;

    QImage* mImageTmp;
    uchar* mImageTmpData;
    ImageState mImageState;

};

#endif //VIPERSGUI_MONITORWINDOW_HPP
