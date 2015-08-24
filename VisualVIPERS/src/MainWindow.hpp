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

#ifndef VIPERSGUI_MAINWINDOW_HPP
#define VIPERSGUI_MAINWINDOW_HPP

#include <QMainWindow>
#include <QAction>
#include <QActionGroup>
#include <QToolBar>
#include <QToolButton>
#include <QString>
#include <QLabel>
#include <QGraphicsView>
#include <QLabel>
#include <QDockWidget>
#include <QListWidget>
#include <QPointF>
#include <QMap>
#include <QSet>
#include <QString>
#include <QMenu>

#include <Kernel.hpp>

#include "ModulesList.hpp"
#include "ModulesLayoutScene.hpp"
#include "ModuleInfo.hpp"
#include "ModuleInstanceInfo.hpp"
#include "UIKernelStateNotifier.hpp"

using namespace VIPERS;

class ModuleWidget;
class ModuleSlotWidget;
class ModuleSlotLink;
class MonitorWindow;
class ModuleParameterStack;

typedef QMap<int, QString> ModuleNameMap;
typedef QMap<QString, ModuleInfo> ModuleInfoMap;
typedef QMap<QString, ModuleWidget*> ModuleWidgetMap;
typedef QSet<MonitorWindow*> MonitorWindowList;
typedef QMap<QString, ModuleInstanceInfo> ModuleInstanceInfoMap;

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:

    MainWindow();
    virtual ~MainWindow();

    enum Mode
    {
      eModeEditNormal,
      eModeEditModuleAdd,
      eModeEditConnectSlots,
      eModeProcessing
    };

  signals:

    void updatedModuleInstanceInfoMap(const ModuleInstanceInfoMap& inModuleInstanceInfoMap);
    void addedModuleInstanceParameter(const ModuleInstanceInfo& inModuleInstanceInfo);
    void removedModuleInstanceParameter(const QString& inLabel);
    void updatedModuleInstanceParameter(const QString& inOldLabel, const QString& inNewLabel);

  public slots:

    void newModuleInstance(QPointF inPosition);
    void newModuleInstance(QString inModuleName, QPointF inPosition);
    void newModuleSlotConnection(ModuleSlotWidget* inModuleSlotWidget1, ModuleSlotWidget* inModuleSlotWidget2);
    void sceneSelectionChanged();
    void moduleWidgetFocusChanged(ModuleWidget* inModuleWidget, bool inHasFocus);
    void closeMonitorWindow(MonitorWindow* MonitorWindow);

  protected slots:
  
    void newProject();
    void openFileProject();
    void saveFileProject();
    void saveFileProjectAs();
    void showFileInfo();
    void quitCheck();
    void showAbout();
    
    void stopProcessing();
    void startProcessing();    
    void pauseProcessing();
    void stepProcessing();
    void refreshProcessing();
    void resetProcessing();
    
    void addModule();
    void removeModule();
    void connectModuleSlot();
    void disconnectModuleSlot();
    void renameModule();
    void modifyAppPreferences();

    void newMonitor();
    void listMonitor();

    void viewModulesListDock();
    void viewModuleParametersDock();
    void viewModuleDescription();
    void viewStatusbar();
    void viewFileToolbar();
    void viewControlToolbar();
    void viewEditToolbar();
    void viewMonitorToolbar();
    
    void changeModuleDescription(int inCurrentRow);
    void doubleClickedModuleListItem(QListWidgetItem* inItem);

    void updateState();

  protected:

    void closeEvent(QCloseEvent* inEvent);
    void dragEnterEvent(QDragEnterEvent* inEvent);
    void dragLeaveEvent(QDragLeaveEvent* inEvent);
    void dragMoveEvent(QDragMoveEvent* inEvent);
    void dropEvent(QDropEvent* inEvent);
    bool event(QEvent* inEvent);
    void keyPressEvent(QKeyEvent* inEvent);
    void keyReleaseEvent(QKeyEvent* inEvent);
    void mouseMoveEvent(QMouseEvent* inEvent);
    void mousePressEvent(QMouseEvent* inEvent);
    void mouseReleaseEvent(QMouseEvent* inEvent);

  private:

    // GUI related stuff

    void createActions();
    void createMenu();
    void createToolbar();    
    void createStatusbar();
    void createLayout();
    
    ModuleInstanceInfoMap getModuleInstanceInfoMap() const;

    void processKernelStateNotifications();
    void updateMonitors();

    QAction* mActionNewProject;
    QAction* mActionOpenProject;
    QAction* mActionSaveProject;
    QAction* mActionSaveProjectAs;
    QAction* mActionFileInfo;
    QAction* mActionQuitCheck;           
    QAction* mActionShowAbout;

    QActionGroup* mActionGroupControls;
    QAction* mActionStop;
    QAction* mActionStart;
    QAction* mActionPause;
    QAction* mActionStep;
    QAction* mActionRefresh;
    QAction* mActionReset;

    QAction* mActionAddModule;
    QAction* mActionRemoveModule;
    QAction* mActionConnectModuleSlot;
    QAction* mActionDisconnectModuleSlot;
    QAction* mActionRenameModule;
    QAction* mActionModifyAppPreferences;

    QAction* mActionNewMonitor;
    QToolButton* mButtonListMonitor;
    QMenu* mMenuListMonitor;

    QAction* mActionViewModulesListDock;
    QAction* mActionViewModuleParametersDock;
    QAction* mActionViewModuleDescription;
    QAction* mActionViewStatusbar;
    QAction* mActionViewFileToolbar;
    QAction* mActionViewControlToolbar;
    QAction* mActionViewEditToolbar;
    QAction* mActionViewMonitorToolbar;
    
    QLabel* mStatusLabel;
    QLabel* mFrameLabel;
    
    QDockWidget* mModulesListDock;
    QDockWidget* mModuleParameterDock;
    
    ModulesList* mModulesList;
    QScrollArea* mModuleDescriptionArea;
    QLabel* mModuleDescription;
    
    ModuleParameterStack* mModuleParameterStack;
    
    ModulesLayoutScene* mModulesLayoutScene;
    QGraphicsView* mGraphicsView;

    QToolBar* mFileToolBar;
    QToolBar* mControlToolBar;
    QToolBar* mEditToolBar;
    QToolBar* mMonitorToolBar;

    QString mTitleSoftware;
    QPoint mClickedPosition;

    ModuleWidgetMap mModuleWidgetMap;
    MonitorWindowList mMonitorWindowList;

    ModuleWidget* mSelectedModuleWidget;
    ModuleSlotLink* mSelectedModuleSlotLink;

    // VIPERS related stuff

    ModuleNameMap mModuleNameMap;
    ModuleInfoMap mModuleInfoMap;
    Kernel* mKernel;

    // State stuff

    void updateApplicationState();
    void updateEditState();
    void updateProcessingState(KernelState::State inKernelState);
    void removeModule(ModuleWidget* inModuleWidget);
    void disconnectModuleSlot(ModuleSlotLink* inModuleSlotLink, ModuleSlotWidget** ioInputModuleSlotWidget = NULL, ModuleSlotWidget** ioOutputModuleSlotWidget = NULL);
    void clear();

    Mode mMode;

    UIKernelStateNotifier mKernelStateNotifier;
    KernelState mKernelState;
};

#endif //VIPERSGUI_MAINWINDOW_HPP
