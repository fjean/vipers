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

#include "MainWindow.hpp"
#include "SharedObjects.hpp"
#include "VisualVipersConfig.hpp"
#include "ModuleWidget.hpp"
#include "ModuleSlotWidget.hpp"
#include "ModuleSlotLink.hpp"
#include "MonitorWindow.hpp"
#include "ModuleParameterStack.hpp"
#include "AboutDialog.hpp"

#include <QApplication>
#include <QToolButton>
#include <QButtonGroup>
#include <QMenuBar>
#include <QIcon>
#include <QStatusBar>
#include <QMessageBox>
#include <QInputDialog>
#include <QListWidgetItem>
#include <QSplitter>
#include <QFile>
#include <QVBoxLayout>
#include <QFrame>
#include <QGroupBox>
#include <QGraphicsProxyWidget>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QDrag>
#include <QMimeData>
#include <QList>
#include <QGraphicsItem>
#include <QCursor>

#include <QtGlobal>
#include <QDebug>

#include <SequentialKernel.hpp>
#include <iostream>

//-------------------------------------------------------------------------------

MainWindow::MainWindow()
: QMainWindow(),
  mMode(MainWindow::eModeEditNormal),
  mSelectedModuleWidget(0),
  mSelectedModuleSlotLink(0),
  mKernelStateNotifier(this)
{
  mTitleSoftware = QString(tr("Visual VIPERS"));
  QApplication::setApplicationName(mTitleSoftware);
  QApplication::setApplicationVersion(VISUALVIPERS_VERSION);
  setWindowIcon(QIcon(":/images/vipers"));

  #ifdef VIPERSGUI_DEBUG
    mTitleSoftware = mTitleSoftware + " " + QString(VIPERSGUI_VERSION);
  #endif

  setWindowTitle(mTitleSoftware);

  SharedObjects::instance()->init();

  try
  {
    mKernel = new SequentialKernel();
    mKernel->setKernelStateNotifier(&mKernelStateNotifier);

    mKernel->loadModulePathList();

    const ModuleFactoryMap& lModuleFactoryMap = mKernel->getModulesManager().getModuleFactoryMap();
    ModuleFactoryMap::const_iterator lModuleFactoryItr = lModuleFactoryMap.begin();

    for(lModuleFactoryItr; lModuleFactoryItr!=lModuleFactoryMap.end(); lModuleFactoryItr++)
    {
      ModuleInfo lModuleInfo(lModuleFactoryItr->second->getName().c_str(), lModuleFactoryItr->second->getDisplayName().c_str());
      lModuleInfo.setVersion(lModuleFactoryItr->second->getVersion().c_str());
      lModuleInfo.setFileName(lModuleFactoryItr->second->getFileName().c_str());
      lModuleInfo.setFilePath(lModuleFactoryItr->second->getFilePath().c_str());
      lModuleInfo.setShortDescription(lModuleFactoryItr->second->getShortDescription().c_str());
      lModuleInfo.setLongDescription(lModuleFactoryItr->second->getLongDescription().c_str());

      QFile lTmpIconFile(lModuleInfo.getFilePath() + lModuleInfo.getName() + ".png");
      if(lTmpIconFile.exists())
        lModuleInfo.setIcon(QIcon(lModuleInfo.getFilePath() + lModuleInfo.getName() + ".png"));
      else
        lModuleInfo.setIcon(QIcon(":/images/module"));

      mModuleInfoMap.insert(lModuleInfo.getName(), lModuleInfo);
    }

  }
  catch(Exception inException)
  {
    QMessageBox::critical(this, mTitleSoftware, inException.toString().c_str());
  }

  connect(&mKernelStateNotifier, SIGNAL(updatedState(void)), this, SLOT(updateState(void)));

  createActions();
  createMenu();
  createToolbar();
  createStatusbar();
  createLayout();

  updateApplicationState();
}

//-------------------------------------------------------------------------------

MainWindow::~MainWindow()
{
  SharedObjects::instance()->destroy();
}

//-------------------------------------------------------------------------------

void MainWindow::newModuleInstance(QPointF inPosition)
{
   if(mModulesList->currentItem())
     newModuleInstance(mModulesList->currentItem()->data(Qt::UserRole).toString(), inPosition);
}

//-------------------------------------------------------------------------------

void MainWindow::newModuleInstance(QString inModuleName, QPointF inPosition)
{
  Module* lModule = NULL;
  QGraphicsProxyWidget* lProxy = NULL;

  try
  {
    lModule = mKernel->newModule(inModuleName.toStdString());
  }
  catch(Exception inException)
  {
    QMessageBox::critical(this, mTitleSoftware, inException.toString().c_str());
    return;
  }

  if(!lModule)
  {
    QMessageBox::critical(this, mTitleSoftware, tr("Error: Cannot create a module of type \"") + inModuleName + "\"; the reason is unknown");
    return;
  }

  ModuleWidget* lModuleWidget = new ModuleWidget(lModule,mModuleInfoMap.value(inModuleName).getIcon());
  connect(lModuleWidget, SIGNAL(connectModuleSlot(ModuleSlotWidget*,ModuleSlotWidget*)), this, SLOT(newModuleSlotConnection(ModuleSlotWidget*,ModuleSlotWidget*)));
  connect(lModuleWidget, SIGNAL(focusChanged(ModuleWidget*,bool)), this, SLOT(moduleWidgetFocusChanged(ModuleWidget*,bool)));

  mModuleWidgetMap.insert(lModule->getLabel().c_str(), lModuleWidget);
  lProxy = mModulesLayoutScene->addWidget(lModuleWidget);
  lProxy->setPos(inPosition);
  lProxy->setZValue(2);

  emit updatedModuleInstanceInfoMap(getModuleInstanceInfoMap());
  emit addedModuleInstanceParameter(ModuleInstanceInfo(lModule->getLabel().c_str(), lModule, mModuleInfoMap.value(lModule->getName().c_str()).getIcon()));

  updateEditState();
}

//-------------------------------------------------------------------------------

void MainWindow::newModuleSlotConnection(ModuleSlotWidget* inModuleSlotWidget1, ModuleSlotWidget* inModuleSlotWidget2)
{
  ModuleSlotWidget* lInputModuleSlotWidget;
  ModuleSlotWidget* lOutputModuleSlotWidget;

  Q_ASSERT(inModuleSlotWidget1 && inModuleSlotWidget2);
  Q_ASSERT(inModuleSlotWidget1->getModuleSlot()->getType()!=inModuleSlotWidget2->getModuleSlot()->getType());


  if(inModuleSlotWidget1->getModuleSlot()->getType()==ModuleSlot::eSlotTypeInput)
  {
    lInputModuleSlotWidget = inModuleSlotWidget1;
    lOutputModuleSlotWidget = inModuleSlotWidget2;
  }
  else
  {
    lInputModuleSlotWidget = inModuleSlotWidget2;
    lOutputModuleSlotWidget = inModuleSlotWidget1;
  }

  try
  {
    lInputModuleSlotWidget->getModule()->connect(ModuleSlot::eSlotTypeInput,
        lInputModuleSlotWidget->getModuleSlot()->getName().c_str(),
        lOutputModuleSlotWidget->getModule(),
        lOutputModuleSlotWidget->getModuleSlot()->getName().c_str());
  }
  catch(Exception& inException)
  {
    QMessageBox::critical(this, mTitleSoftware, inException.toString().c_str());
    return;
  }
  catch(...)
  {
    QMessageBox::critical(this, mTitleSoftware, tr("Error: Could not connect two module slots together; the reason is unknown"));
    return;
  }

  ModuleSlotLink* lModuleSlotLink = new ModuleSlotLink(lInputModuleSlotWidget, lOutputModuleSlotWidget);
  mModulesLayoutScene->addItem(lModuleSlotLink);
  lModuleSlotLink->updateLink();

  lInputModuleSlotWidget->updateState();
  lOutputModuleSlotWidget->updateState();

  updateEditState();
}

//-------------------------------------------------------------------------------

void MainWindow::sceneSelectionChanged()
{
  bool lModuleSlotLinkSelected = false;
  mSelectedModuleSlotLink = NULL;

  foreach(QGraphicsItem* lItem, mModulesLayoutScene->selectedItems())
  {
    if(lItem->type()==ModuleSlotLink::Type)
    {
      lModuleSlotLinkSelected = true;
      mSelectedModuleSlotLink = qgraphicsitem_cast<ModuleSlotLink*>(lItem);
      break;
    }
  }

  updateEditState();
}

//-------------------------------------------------------------------------------

void MainWindow::moduleWidgetFocusChanged(ModuleWidget* inModuleWidget, bool inHasFocus)
{
  if(inHasFocus)
  {
    mSelectedModuleWidget = inModuleWidget;
    mModulesLayoutScene->clearSelection();
  }
  else
  {
    mSelectedModuleWidget = NULL;
  }

  updateEditState();
}

//-------------------------------------------------------------------------------

void MainWindow::closeMonitorWindow(MonitorWindow* inMonitorWindow)
{
  Q_ASSERT(inMonitorWindow);
  inMonitorWindow->detach();
  mMenuListMonitor->removeAction(inMonitorWindow->getMonitorAccessAction());
  delete inMonitorWindow->getMonitorAccessAction();
  mMonitorWindowList.remove(inMonitorWindow);
}

//-------------------------------------------------------------------------------

void MainWindow::createActions()
{
  // New project
  mActionNewProject = new QAction(tr("&New project"), this);
  mActionNewProject->setShortcuts(QKeySequence::New);
  mActionNewProject->setStatusTip(tr("Create a new project"));
  mActionNewProject->setToolTip(mActionNewProject->statusTip());
  mActionNewProject->setIcon(QIcon(":/images/filenew"));
  connect(mActionNewProject, SIGNAL(triggered()), this, SLOT(newProject()));

  // Open project
  mActionOpenProject = new QAction(tr("&Open project file ..."), this);
  mActionOpenProject->setShortcuts(QKeySequence::Open);
  mActionOpenProject->setStatusTip(tr("Open a project file"));
  mActionOpenProject->setToolTip(mActionOpenProject->statusTip());
  mActionOpenProject->setIcon(QIcon(":/images/fileopen"));
  connect(mActionOpenProject, SIGNAL(triggered()), this, SLOT(openFileProject()));
  
  // Save project
  mActionSaveProject = new QAction(tr("&Save project file"), this);
  mActionSaveProject->setShortcuts(QKeySequence::Save);
  mActionSaveProject->setStatusTip(tr("Save project"));
  mActionSaveProject->setToolTip(mActionSaveProject->statusTip());
  mActionSaveProject->setIcon(QIcon(":/images/filesave"));
  connect(mActionSaveProject, SIGNAL(triggered()), this, SLOT(saveFileProject()));
  
  // Save project as
  mActionSaveProjectAs = new QAction(tr("Save project file &as ..."), this);
  mActionSaveProjectAs->setShortcuts(QKeySequence::SaveAs);
  mActionSaveProjectAs->setStatusTip(tr("Save project in specified file"));
  mActionSaveProjectAs->setToolTip(mActionSaveProjectAs->statusTip());
  mActionSaveProjectAs->setIcon(QIcon(":/images/filesaveas"));
  connect(mActionSaveProjectAs, SIGNAL(triggered()), this, SLOT(saveFileProjectAs()));
  
  // Show project info
  mActionFileInfo = new QAction(tr("Project &info ..."), this);
  mActionFileInfo->setStatusTip(tr("Show information about current project"));
  mActionFileInfo->setToolTip(mActionFileInfo->statusTip());
  mActionFileInfo->setIcon(QIcon(":/images/info"));
  connect(mActionFileInfo, SIGNAL(triggered()), this, SLOT(showFileInfo()));
  
  // Quit
  mActionQuitCheck = new QAction(tr("&Quit"), this);
  mActionQuitCheck->setStatusTip(tr("Quit application"));
  mActionQuitCheck->setToolTip(mActionQuitCheck->statusTip());
  mActionQuitCheck->setIcon(QIcon(":/images/exit"));
  connect(mActionQuitCheck, SIGNAL(triggered()), this, SLOT(quitCheck()));
     
  // Show about
  mActionShowAbout = new QAction(tr("&About Visual VIPERS ..."), this);
  mActionShowAbout->setStatusTip(tr("Show information about Visual VIPERS version"));
  mActionShowAbout->setToolTip(mActionShowAbout->statusTip());
  connect(mActionShowAbout, SIGNAL(triggered()), this, SLOT(showAbout()));

  // Control group
  mActionGroupControls = new QActionGroup(this);

  // Stop processing
  mActionStop = new QAction(tr("St&op"), mActionGroupControls);
  mActionStop->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Escape));
  mActionStop->setStatusTip(tr("Stop processing"));
  mActionStop->setToolTip(mActionStop->statusTip());
  mActionStop->setIcon(QIcon(":/images/stop"));
  mActionStop->setCheckable(true);
  mActionStop->setChecked(true);

  connect(mActionStop, SIGNAL(triggered()), this, SLOT(stopProcessing()));
  
  // Start processing
  mActionStart = new QAction(tr("St&art"), mActionGroupControls);
  mActionStart->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Enter));
  mActionStart->setStatusTip(tr("Start processing"));
  mActionStart->setToolTip(mActionStart->statusTip());
  mActionStart->setIcon(QIcon(":/images/start"));
  mActionStart->setCheckable(true);
  connect(mActionStart, SIGNAL(triggered()), this, SLOT(startProcessing()));
  
  // Pause processing
  mActionPause = new QAction(tr("&Pause"), mActionGroupControls);
  mActionPause->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Backspace));
  mActionPause->setStatusTip(tr("Pause processing"));
  mActionPause->setToolTip(mActionPause->statusTip());
  mActionPause->setIcon(QIcon(":/images/pause"));
  mActionPause->setCheckable(true);
  connect(mActionPause, SIGNAL(triggered()), this, SLOT(pauseProcessing()));
  
  // Step processing
  mActionStep = new QAction(tr("&Step"), this);
  mActionStep->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Space));
  mActionStep->setStatusTip(tr("Process next frame only"));
  mActionStep->setToolTip(mActionStep->statusTip());
  mActionStep->setIcon(QIcon(":/images/step"));
  connect(mActionStep, SIGNAL(triggered()), this, SLOT(stepProcessing()));
  
  // Refresh processing
  mActionRefresh = new QAction(tr("&Refresh"), this);
  mActionRefresh->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F5));
  mActionRefresh->setStatusTip(tr("Refresh (re-process) the current frame"));
  mActionRefresh->setToolTip(mActionRefresh->statusTip());
  mActionRefresh->setIcon(QIcon(":/images/refresh"));
  connect(mActionRefresh, SIGNAL(triggered()), this, SLOT(refreshProcessing()));

  // Reset processing
  mActionReset = new QAction(tr("&Reset"), this);
  mActionReset->setStatusTip(tr("Reset all modules to put them back in their initial state"));
  mActionReset->setToolTip(mActionReset->statusTip());
  mActionReset->setIcon(QIcon(":/images/reset"));
  connect(mActionReset, SIGNAL(triggered()), this, SLOT(resetProcessing()));

  // Add module
  mActionAddModule = new QAction(tr("&Add module mode"), this);
  mActionAddModule->setShortcut(QKeySequence(Qt::Key_Insert));
  mActionAddModule->setStatusTip(tr("Add new module instances by clicking at the wanted position"));
  mActionAddModule->setToolTip(mActionAddModule->statusTip());
  mActionAddModule->setIcon(QIcon(":/images/module-add"));
  mActionAddModule->setEnabled(false);
  mActionAddModule->setCheckable(true);
  mActionAddModule->setChecked(false);
  connect(mActionAddModule, SIGNAL(triggered()), this, SLOT(addModule()));

  // Remove module
  mActionRemoveModule = new QAction(tr("&Remove module"), this);
  mActionRemoveModule->setStatusTip(tr("Remove the selected module instance"));
  mActionRemoveModule->setToolTip(mActionRemoveModule->statusTip());
  mActionRemoveModule->setIcon(QIcon(":/images/module-remove"));
  mActionRemoveModule->setEnabled(false);
  connect(mActionRemoveModule, SIGNAL(triggered()), this, SLOT(removeModule()));

  // Connect slots
  mActionConnectModuleSlot = new QAction(tr("&Connect module mode"), this);
  mActionConnectModuleSlot->setStatusTip(tr("Connect modules simply by clicking on the slots"));
  mActionConnectModuleSlot->setToolTip(mActionConnectModuleSlot->statusTip());
  mActionConnectModuleSlot->setIcon(QIcon(":/images/slot-connect"));
  mActionConnectModuleSlot->setEnabled(false);
  mActionConnectModuleSlot->setCheckable(true);
  mActionConnectModuleSlot->setChecked(false);
  connect(mActionConnectModuleSlot, SIGNAL(triggered()), this, SLOT(connectModuleSlot()));

  // Disconnect slots
  mActionDisconnectModuleSlot = new QAction(tr("&Disconnect module slots"), this);
  mActionDisconnectModuleSlot->setStatusTip(tr("Disconnect the slots pointed by the selected link"));
  mActionDisconnectModuleSlot->setToolTip(mActionDisconnectModuleSlot->statusTip());
  mActionDisconnectModuleSlot->setIcon(QIcon(":/images/slot-disconnect"));
  mActionDisconnectModuleSlot->setEnabled(false);
  connect(mActionDisconnectModuleSlot, SIGNAL(triggered()), this, SLOT(disconnectModuleSlot()));

  // Rename module
  mActionRenameModule = new QAction(tr("Re&name module"), this);
  mActionRenameModule->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
  mActionRenameModule->setStatusTip(tr("Rename the selected module"));
  mActionRenameModule->setToolTip(mActionRenameModule->statusTip());
  mActionRenameModule->setIcon(QIcon(":/images/rename"));
  mActionRenameModule->setEnabled(false);
  connect(mActionRenameModule, SIGNAL(triggered()), this, SLOT(renameModule()));

  // Modify application preferences
  mActionModifyAppPreferences = new QAction(tr("&Preferences ..."), this);
  mActionModifyAppPreferences->setStatusTip(tr("Modify Visual VIPERS preferences"));
  mActionModifyAppPreferences->setToolTip(mActionModifyAppPreferences->statusTip());
  mActionModifyAppPreferences->setIcon(QIcon(":/images/application-preferences"));
  connect(mActionModifyAppPreferences, SIGNAL(triggered()), this, SLOT(modifyAppPreferences()));

  // View modules list window
  mActionViewModulesListDock = new QAction(tr("&Modules"), this);
  mActionViewModulesListDock->setStatusTip(tr("View modules panel"));
  mActionViewModulesListDock->setToolTip(mActionViewModulesListDock->statusTip());
  connect(mActionViewModulesListDock, SIGNAL(triggered()), this, SLOT(viewModulesListDock()));
  mActionViewModulesListDock->setCheckable(true);
  mActionViewModulesListDock->setChecked(true);

  // View module parameters window
  mActionViewModuleParametersDock = new QAction(tr("&Parameters"), this);
  mActionViewModuleParametersDock->setStatusTip(tr("View module parameters panel"));
  mActionViewModuleParametersDock->setToolTip(mActionViewModuleParametersDock->statusTip());
  connect(mActionViewModuleParametersDock, SIGNAL(triggered()), this, SLOT(viewModuleParametersDock()));
  mActionViewModuleParametersDock->setCheckable(true);
  mActionViewModuleParametersDock->setChecked(true);

  // View module parameters window
  mActionViewModuleDescription = new QAction(tr("Module &description"), this);
  mActionViewModuleDescription->setStatusTip(tr("View module description panel"));
  mActionViewModuleDescription->setToolTip(mActionViewModuleDescription->statusTip());
  connect(mActionViewModuleDescription, SIGNAL(triggered()), this, SLOT(viewModuleDescription()));
  mActionViewModuleDescription->setCheckable(true);
  mActionViewModuleDescription->setChecked(true);

  // View status bar
  mActionViewStatusbar = new QAction(tr("&Status bar"), this);
  mActionViewStatusbar->setStatusTip(tr("View status bar"));
  mActionViewStatusbar->setToolTip(mActionViewStatusbar->statusTip());
  connect(mActionViewStatusbar, SIGNAL(triggered()), this, SLOT(viewStatusbar()));
  mActionViewStatusbar->setCheckable(true);
  mActionViewStatusbar->setChecked(true);

  // View file toolbar
  mActionViewFileToolbar = new QAction(tr("&File toolbar"), this);
  mActionViewFileToolbar->setStatusTip(tr("View file bar"));
  mActionViewFileToolbar->setToolTip(mActionViewFileToolbar->statusTip());
  connect(mActionViewFileToolbar, SIGNAL(triggered()), this, SLOT(viewFileToolbar()));
  mActionViewFileToolbar->setCheckable(true);
  mActionViewFileToolbar->setChecked(true);

  // View control tool bar
  mActionViewControlToolbar = new QAction(tr("&Control bar"), this);
  mActionViewControlToolbar->setStatusTip(tr("View control bar"));
  mActionViewControlToolbar->setToolTip(mActionViewControlToolbar->statusTip());
  connect(mActionViewControlToolbar, SIGNAL(triggered()), this, SLOT(viewControlToolbar()));
  mActionViewControlToolbar->setCheckable(true);
  mActionViewControlToolbar->setChecked(true);

  // View edit tool bar
  mActionViewEditToolbar = new QAction(tr("&Edit bar"), this);
  mActionViewEditToolbar->setStatusTip(tr("View edit bar"));
  mActionViewEditToolbar->setToolTip(mActionViewEditToolbar->statusTip());
  connect(mActionViewEditToolbar, SIGNAL(triggered()), this, SLOT(viewEditToolbar()));
  mActionViewEditToolbar->setCheckable(true);
  mActionViewEditToolbar->setChecked(true);

  // View monitor tool bar
  mActionViewMonitorToolbar = new QAction(tr("&Monitor bar"), this);
  mActionViewMonitorToolbar->setStatusTip(tr("View monitor bar"));
  mActionViewMonitorToolbar->setToolTip(mActionViewMonitorToolbar->statusTip());
  connect(mActionViewMonitorToolbar, SIGNAL(triggered()), this, SLOT(viewMonitorToolbar()));
  mActionViewMonitorToolbar->setCheckable(true);
  mActionViewMonitorToolbar->setChecked(true);

  // New monitor
  mActionNewMonitor = new QAction(tr("&New monitor"), this);
  mActionNewMonitor->setStatusTip(tr("Create a new monitor window"));
  mActionNewMonitor->setToolTip(mActionNewMonitor->statusTip());
  mActionNewMonitor->setIcon(QIcon(":/images/monitor-new"));
  connect(mActionNewMonitor, SIGNAL(triggered()), this, SLOT(newMonitor()));

}

//-------------------------------------------------------------------------------

void MainWindow::createMenu()
{
  // File menu
  QMenu* lFileMenu = menuBar()->addMenu(tr("&File"));
  lFileMenu->addAction(mActionNewProject);
  lFileMenu->addAction(mActionOpenProject);
  lFileMenu->addAction(mActionSaveProject);
  lFileMenu->addAction(mActionSaveProjectAs);
  lFileMenu->addSeparator();
  lFileMenu->addAction(mActionFileInfo);
  lFileMenu->addSeparator();
  lFileMenu->addAction(mActionQuitCheck);
 
  // Edit menu
  QMenu* lEditMenu = menuBar()->addMenu(tr("&Edit"));
  lEditMenu->addAction(mActionAddModule);
  lEditMenu->addAction(mActionConnectModuleSlot);
  lEditMenu->addSeparator();
  lEditMenu->addAction(mActionRemoveModule);
  lEditMenu->addAction(mActionDisconnectModuleSlot);
  lEditMenu->addAction(mActionRenameModule);
  lEditMenu->addSeparator();
  lEditMenu->addAction(mActionModifyAppPreferences);

  // Control menu
  QMenu* lControlMenu = menuBar()->addMenu(tr("&Control"));
  lControlMenu->addAction(mActionStop);
  lControlMenu->addAction(mActionStart);
  lControlMenu->addAction(mActionPause);
  lControlMenu->addSeparator();
  lControlMenu->addAction(mActionStep);
  lControlMenu->addAction(mActionRefresh);
  lControlMenu->addAction(mActionReset);

  // View menu
  QMenu* lViewMenu = menuBar()->addMenu(tr("&View"));
  QMenu* lViewMenuToolbars = lViewMenu->addMenu(tr("&Toolbars"));
  lViewMenuToolbars->addAction(mActionViewFileToolbar);
  lViewMenuToolbars->addAction(mActionViewControlToolbar);
  lViewMenuToolbars->addAction(mActionViewEditToolbar);
  lViewMenuToolbars->addAction(mActionViewMonitorToolbar);

  QMenu* lViewMenuPanels = lViewMenu->addMenu(tr("&Panels"));
  lViewMenuPanels->addAction(mActionViewModulesListDock);
  lViewMenuPanels->addAction(mActionViewModuleParametersDock);

  lViewMenu->addAction(mActionViewStatusbar);
  lViewMenu->addAction(mActionViewModuleDescription);
  lViewMenu->addSeparator();
  lViewMenu->addAction(mActionNewMonitor);

  mMenuListMonitor = new QMenu(tr("Monitors &list"));
  mMenuListMonitor->setToolTip(tr("Monitors window list"));
  mMenuListMonitor->setStatusTip(mMenuListMonitor->toolTip());
  mMenuListMonitor->setIcon(QIcon(":/images/monitor-list"));
  lViewMenu->addMenu(mMenuListMonitor);

  // Help menu
  QMenu* lHelpMenu = menuBar()->addMenu(tr("&Help"));
  lHelpMenu->addAction(mActionShowAbout);
    
}

//-------------------------------------------------------------------------------

void MainWindow::createToolbar()
{
  mFileToolBar = addToolBar(tr("File toolbar"));
  mFileToolBar->addAction(mActionNewProject);
  mFileToolBar->addAction(mActionOpenProject);
  mFileToolBar->addAction(mActionSaveProject);
  mFileToolBar->addAction(mActionSaveProjectAs);

  mFileToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
  addToolBar(Qt::TopToolBarArea, mFileToolBar);


  mEditToolBar = addToolBar(tr("Edit toolbar"));
  mEditToolBar->addAction(mActionAddModule);
  mEditToolBar->addAction(mActionConnectModuleSlot);
  mEditToolBar->addSeparator();
  mEditToolBar->addAction(mActionRemoveModule);
  mEditToolBar->addAction(mActionDisconnectModuleSlot);

  mEditToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
  addToolBar(Qt::TopToolBarArea, mEditToolBar);


  mControlToolBar = addToolBar(tr("Control toolbar"));
  mControlToolBar->addAction(mActionStop);
  mControlToolBar->addAction(mActionStart);
  mControlToolBar->addAction(mActionPause);

  mControlToolBar->addSeparator();
  mControlToolBar->addAction(mActionStep);
  mControlToolBar->addAction(mActionRefresh);
  mControlToolBar->addAction(mActionReset);

  mControlToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
  addToolBar(Qt::TopToolBarArea, mControlToolBar);


  mMonitorToolBar = addToolBar(tr("Monitor toolbar"));
  mMonitorToolBar->addAction(mActionNewMonitor);
  mButtonListMonitor = new QToolButton();
  mButtonListMonitor->setText("Monitors list");
  mButtonListMonitor->setToolTip("Show the list of monitors");
  mButtonListMonitor->setStatusTip(mButtonListMonitor->toolTip());
  mButtonListMonitor->setMenu(mMenuListMonitor);
  mButtonListMonitor->setPopupMode(QToolButton::InstantPopup);
  mButtonListMonitor->setIcon(QIcon(":/images/monitor-list"));
  mMonitorToolBar->addWidget(mButtonListMonitor);
  mMonitorToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
  addToolBar(Qt::TopToolBarArea, mMonitorToolBar);
  setIconSize(QSize(16,16));
}

//-------------------------------------------------------------------------------

void MainWindow::createStatusbar()
{
  mStatusLabel = new QLabel(tr("Status: Stopped"));
  mStatusLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  mStatusLabel->setMinimumWidth(200);
  statusBar()->insertPermanentWidget(0, mStatusLabel, 0);

  mFrameLabel = new QLabel(tr("Frame: N/A"));
  mFrameLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  mFrameLabel->setMinimumWidth(100);
  statusBar()->insertPermanentWidget(1, mFrameLabel, 0);

  statusBar()->showMessage(tr("VIPERS is ready!"), 4000);
}

//-------------------------------------------------------------------------------

void MainWindow::createLayout()
{
  // Modules list dock
  mModulesListDock = new QDockWidget(tr("Modules panel"), this);
  mModulesListDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

  QSplitter* lSplitter = new QSplitter(mModulesListDock);
  lSplitter->setOrientation(Qt::Vertical);

  mModulesList = new ModulesList();
  connect(mModulesList, SIGNAL(currentRowChanged(int)), this, SLOT(changeModuleDescription(int)));
  connect(mModulesList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(doubleClickedModuleListItem(QListWidgetItem*)));
  lSplitter->addWidget(mModulesList);

  QListWidgetItem* lTmpListItem;
  ModuleInfoMap::iterator lModuleInfoItr = mModuleInfoMap.begin();
  for(lModuleInfoItr; lModuleInfoItr!=mModuleInfoMap.end(); lModuleInfoItr++)
  {
    lTmpListItem = new QListWidgetItem(lModuleInfoItr->getDisplayName());
    lTmpListItem->setData(Qt::UserRole, lModuleInfoItr->getName());
    lTmpListItem->setIcon(lModuleInfoItr->getIcon());
    lTmpListItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    mModuleNameMap.insert(mModulesList->count(), lModuleInfoItr->getName());
    mModulesList->insertItem(mModulesList->count(), lTmpListItem);
  }
  mModulesList->setIconSize(QSize(32,32));

  mModuleDescriptionArea = new QScrollArea();
  mModuleDescription = new QLabel(mModuleDescriptionArea);
  mModuleDescription->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  mModuleDescription->setMargin(5);
  mModuleDescription->setWordWrap(true);
  mModuleDescriptionArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  mModuleDescriptionArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  mModuleDescriptionArea->setWidgetResizable(true);
  mModuleDescriptionArea->setWidget(mModuleDescription);
  lSplitter->addWidget(mModuleDescriptionArea);

  mModulesListDock->setWidget(lSplitter);
  addDockWidget(Qt::LeftDockWidgetArea, mModulesListDock);

  // Module parameters dock
  mModuleParameterDock = new QDockWidget(tr("Parameters panel"), this);
  mModuleParameterDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  mModuleParameterStack = new ModuleParameterStack(mModuleParameterDock);
  mModuleParameterDock->setWidget(mModuleParameterStack);

  connect(this, SIGNAL(addedModuleInstanceParameter(const ModuleInstanceInfo&)), mModuleParameterStack, SLOT(addModuleInstanceParameter(const ModuleInstanceInfo&)));
  connect(this, SIGNAL(removedModuleInstanceParameter(const QString&)), mModuleParameterStack, SLOT(removeModuleInstanceParameter(const QString&)));
  connect(this, SIGNAL(updatedModuleInstanceParameter(const QString&, const QString&)), mModuleParameterStack, SLOT(updateModuleInstanceParameter(const QString&, const QString&)));

  addDockWidget(Qt::RightDockWidgetArea, mModuleParameterDock);

  // Dock options
  setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
  setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

  // Graphics scene and view
  mModulesLayoutScene = new ModulesLayoutScene();
  mGraphicsView = new QGraphicsView(mModulesLayoutScene, this);
  mGraphicsView->setInteractive(true);
  mGraphicsView->setAcceptDrops(true);
  //mGraphicsView->setRenderHint(QPainter::Antialiasing, true);
  connect(mModulesLayoutScene, SIGNAL(moduleDropped(QString, QPointF)), this, SLOT(newModuleInstance(QString, QPointF)));
  connect(mModulesLayoutScene, SIGNAL(clickAdd(QPointF)), this, SLOT(newModuleInstance(QPointF)));
  connect(mModulesLayoutScene, SIGNAL(selectionChanged()), this, SLOT(sceneSelectionChanged()));
  setCentralWidget(mGraphicsView);

  if(mModulesList->count())
    mModulesList->setCurrentRow(0);
  mGraphicsView->setFocus();
}

//-------------------------------------------------------------------------------

void MainWindow::newProject()
{
  QMessageBox::warning(this, mTitleSoftware, tr("Sorry, this functionality is not yet implemented!"), QMessageBox::Ok);
}

//-------------------------------------------------------------------------------

void MainWindow::openFileProject()
{
  QMessageBox::warning(this, mTitleSoftware, tr("Sorry, this functionality is not yet implemented!"), QMessageBox::Ok);
}

//-------------------------------------------------------------------------------

void MainWindow::saveFileProject()
{
  QMessageBox::warning(this, mTitleSoftware, tr("Sorry, this functionality is not yet implemented!"), QMessageBox::Ok);
}

//-------------------------------------------------------------------------------

void MainWindow::saveFileProjectAs()
{
  QMessageBox::warning(this, mTitleSoftware, tr("Sorry, this functionality is not yet implemented!"), QMessageBox::Ok);
}

//-------------------------------------------------------------------------------

void MainWindow::showFileInfo()
{
  QMessageBox::warning(this, mTitleSoftware, tr("Sorry, this functionality is not yet implemented!"), QMessageBox::Ok);
}

//-------------------------------------------------------------------------------

void MainWindow::quitCheck()
{
  close();
}

//-------------------------------------------------------------------------------

void MainWindow::showAbout()
{
  AboutDialog lAboutDialog(this);
  lAboutDialog.exec();
}

//-------------------------------------------------------------------------------

void MainWindow::startProcessing()
{
    
  KernelState lKernelState;
  
  //Check kernel state
  lKernelState = mKernel->getState();
  if(lKernelState==KernelState::eStateStarted)
    return;
  //Q_ASSERT(lKernelState!=KernelState::eStateStarted);  
  
  mMode = eModeProcessing;
  updateApplicationState();

  // Check if there is modules instantiated
  if(mKernel->getModuleCount()==0)
  {
    QMessageBox::warning(this, mTitleSoftware, tr("Cannot start processing since there is no module instantiated!  At least one module must be present in the module layout area."), QMessageBox::Ok);
    mMode = eModeEditNormal;
    updateApplicationState();
    updateProcessingState(KernelState::eStateStopped);
    return;
  }

  //If processing was paused, start it again
  if(lKernelState==KernelState::eStatePaused)
  {
    mKernelStateNotifier.setPostUIEvent(true);

    try
    {
      mKernel->start();
    }
    catch(Exception& inException)
    {
      QMessageBox::critical(this, mTitleSoftware, inException.toString().c_str());
      mMode = eModeEditNormal;
      updateApplicationState();
      updateProcessingState(lKernelState);
      mKernelStateNotifier.setPostUIEvent(false);
      return;
    }
  }
  //Else reinit all modules and start processing
  else
  {
    mKernelStateNotifier.setPostUIEvent(false);

    try
    {
      mKernel->init();
    }
    catch(Exception& inException)
    {
      QMessageBox::critical(this, mTitleSoftware, inException.toString().c_str());
      mMode = eModeEditNormal;
      updateApplicationState();
      updateProcessingState(lKernelState);
      return;
    }

    //TODO: Make a wait function that accepts a timeout so the application doesn't freeze in
    //case of a buggy module (at least to exit gracefully)
    lKernelState = mKernelStateNotifier.waitNotification();

    if(lKernelState.isExceptionRaised())
    {
      Exception lException = lKernelState.getException();
      QMessageBox::critical(this, mTitleSoftware, lException.toString().c_str());
      mMode = eModeEditNormal;
      updateApplicationState();
      updateProcessingState(lKernelState);
      return;
    }

    mKernelStateNotifier.setPostUIEvent(true);

    try
    {
      mKernel->start();
    }
    catch(Exception& inException)
    {
      QMessageBox::critical(this, mTitleSoftware, inException.toString().c_str());
      mMode = eModeEditNormal;
      updateApplicationState();
      updateProcessingState(lKernelState);
      mKernelStateNotifier.setPostUIEvent(false);
      return;
    }
  }


}

//-------------------------------------------------------------------------------

void MainWindow::stopProcessing()
{ 
  KernelState lKernelState;
  
  //Check kernel state
  lKernelState = mKernel->getState();
  if(lKernelState!=KernelState::eStateStarted && lKernelState!=KernelState::eStatePaused)
    return;
  //Q_ASSERT(lKernelState==KernelState::eStateStarted || lKernelState==KernelState::eStatePaused);

  mKernelStateNotifier.setPostUIEvent(false);

  try
  {
    mKernel->stop();
  }
  catch(Exception& inException)
  {
    QMessageBox::critical(this, mTitleSoftware, inException.toString().c_str());
    updateProcessingState(lKernelState);
    return;
  }

  //TODO: Make a wait function that accepts a timeout so the application doesn't freeze in
  //case of a buggy module (at least to exit gracefully)
  lKernelState = mKernelStateNotifier.waitNotification();

  while(lKernelState!=KernelState::eStateStopped)
  {
    if(lKernelState.isExceptionRaised())
    {
      Exception lException = lKernelState.getException();
      QMessageBox::critical(this, mTitleSoftware, lException.toString().c_str());

      updateProcessingState(lKernelState);
      return;
    }
    lKernelState = mKernelStateNotifier.waitNotification();
  }

  mMode = eModeEditNormal;
  updateApplicationState();

}

//-------------------------------------------------------------------------------

void MainWindow::pauseProcessing()
{  
  KernelState lKernelState;
  
  //Check kernel state
  lKernelState = mKernel->getState();
  if(lKernelState!=KernelState::eStateStarted)
    return;
  //Q_ASSERT(lKernelState==KernelState::eStateStarted);

  mKernelStateNotifier.setPostUIEvent(false);

  try
  {
    mKernel->pause();
  }
  catch(Exception& inException)
  {
    QMessageBox::critical(this, mTitleSoftware, inException.toString().c_str());
    updateProcessingState(lKernelState);
    return;
  }

  //TODO: Make a wait function that accepts a timeout so the application doesn't freeze in
  //case of a buggy module (at least to exit gracefully)
  lKernelState = mKernelStateNotifier.waitNotification();

  while(lKernelState!=KernelState::eStatePaused)
  {
    if(lKernelState.isExceptionRaised())
    {
      Exception lException = lKernelState.getException();
      QMessageBox::critical(this, mTitleSoftware, lException.toString().c_str());
      updateProcessingState(lKernelState);
      return;
    }
    mFrameLabel->setText(QString(tr("Frame: %1")).arg(lKernelState.getFrame()));
    lKernelState = mKernelStateNotifier.waitNotification();
  }

  mFrameLabel->setText(QString(tr("Frame: %1")).arg(lKernelState.getFrame()));
  updateMonitors();

  updateProcessingState(lKernelState);
  
}

//-------------------------------------------------------------------------------

void MainWindow::stepProcessing()
{
  KernelState lKernelState;

  //Check kernel state
  lKernelState = mKernel->getState();
  Q_ASSERT(lKernelState==KernelState::eStatePaused);

  mKernelStateNotifier.setPostUIEvent(false);

  try
  {
    mKernel->step();
  }
  catch(Exception& inException)
  {
    QMessageBox::critical(this, mTitleSoftware, inException.toString().c_str());
    updateProcessingState(lKernelState);
    return;
  }

  lKernelState = mKernelStateNotifier.waitNotification();

  if(lKernelState.isExceptionRaised())
  {
    Exception lException = lKernelState.getException();
    QMessageBox::critical(this, mTitleSoftware, lException.toString().c_str());
    updateProcessingState(lKernelState);
    return;
  }

  mFrameLabel->setText(QString(tr("Frame: %1")).arg(lKernelState.getFrame()));

  updateMonitors();

}

//-------------------------------------------------------------------------------

void MainWindow::refreshProcessing()
{
  KernelState lKernelState;

  //Check kernel state
  lKernelState = mKernel->getState();
  Q_ASSERT(lKernelState==KernelState::eStatePaused);

  mKernelStateNotifier.setPostUIEvent(false);

  try
  {
    mKernel->refresh();
  }
  catch(Exception& inException)
  {
    QMessageBox::critical(this, mTitleSoftware, inException.toString().c_str());
    updateProcessingState(lKernelState);
    return;
  }

  lKernelState = mKernelStateNotifier.waitNotification();

  if(lKernelState.isExceptionRaised())
  {
    Exception lException = lKernelState.getException();
    QMessageBox::critical(this, mTitleSoftware, lException.toString().c_str());
    updateProcessingState(lKernelState);
    return;
  }

  updateMonitors();
}

//-------------------------------------------------------------------------------

void MainWindow::resetProcessing()
{
  // Check if there is modules instantiated
  if(mKernel->getModuleCount()==0)
  {
    QMessageBox::warning(this, mTitleSoftware, tr("There is no module to reset in the module layout area."), QMessageBox::Ok);
    return;
  }

  KernelState lKernelState;

  //Check kernel state
  lKernelState = mKernel->getState();
  Q_ASSERT(lKernelState==KernelState::eStateStopped || lKernelState==KernelState::eStateInitialized || lKernelState==KernelState::eStateUninitialized);

  try
  {
    mKernel->reset();
  }
  catch(Exception& inException)
  {
    QMessageBox::critical(this, mTitleSoftware, inException.toString().c_str());
    return;
  }

  lKernelState = mKernelStateNotifier.waitNotification();

  if(lKernelState.isExceptionRaised())
  {
    Exception lException = lKernelState.getException();
    QMessageBox::critical(this, mTitleSoftware, lException.toString().c_str());
    updateProcessingState(lKernelState);
    return;
  }

}

//-------------------------------------------------------------------------------

void MainWindow::addModule()
{
  if(mActionAddModule->isChecked())
  {
    mMode = MainWindow::eModeEditModuleAdd;
    ModuleSlotWidget::setMode(ModuleSlotWidget::eModeDisabled);
    ModuleSlotLink::setInteractionEnabled(false);
    ModulesLayoutScene::setClickAddEnabled(true);
    updateApplicationState();
    mGraphicsView->setCursor(QCursor(Qt::CrossCursor));
  }
  else
  {
    mMode = MainWindow::eModeEditNormal;
    ModuleSlotWidget::setMode(ModuleSlotWidget::eModeNormal);
    ModuleSlotLink::setInteractionEnabled(true);
    ModulesLayoutScene::setClickAddEnabled(false);
    updateApplicationState();
    mGraphicsView->unsetCursor();
  }
}

//-------------------------------------------------------------------------------

void MainWindow::removeModule()
{
  ModuleWidget* lModuleWidget = mSelectedModuleWidget;
  if(lModuleWidget)
  {
    mSelectedModuleWidget = NULL;

    emit removedModuleInstanceParameter(lModuleWidget->getModule()->getLabel().c_str());

    removeModule(lModuleWidget);

    emit updatedModuleInstanceInfoMap(getModuleInstanceInfoMap());

    updateEditState();
  }
}

//-------------------------------------------------------------------------------

void MainWindow::connectModuleSlot()
{
  if(mActionConnectModuleSlot->isChecked())
  {
    mMode = MainWindow::eModeEditConnectSlots;
    ModuleWidget::setInteractionEnabled(false);
    ModuleSlotWidget::setMode(ModuleSlotWidget::eModeEditClick);
    updateApplicationState();
  }
  else
  {
    mMode = MainWindow::eModeEditNormal;
    ModuleWidget::setInteractionEnabled(true);
    ModuleSlotWidget::setMode(ModuleSlotWidget::eModeNormal);
    updateApplicationState();
  }
}

//-------------------------------------------------------------------------------

void MainWindow::disconnectModuleSlot()
{
  ModuleSlotLink* lModuleSlotLink = mSelectedModuleSlotLink;

  if(lModuleSlotLink)
  {
    mSelectedModuleSlotLink = NULL;

    ModuleSlotWidget* lInputModuleSlotWidget;
    ModuleSlotWidget* lOutputModuleSlotWidget;

    disconnectModuleSlot(lModuleSlotLink, &lInputModuleSlotWidget, &lOutputModuleSlotWidget);

    lInputModuleSlotWidget->updateState();
    lOutputModuleSlotWidget->updateState();

    updateEditState();
  }
}

//-------------------------------------------------------------------------------

void MainWindow::renameModule()
{
  if(mSelectedModuleWidget)
  {
    ModuleWidget* lTmpModuleWidget = mSelectedModuleWidget;
    bool lOk;
    QString lOldLabel = lTmpModuleWidget->getModule()->getLabel().c_str();
    QString lNewLabel;

    while(true)
    {
      lNewLabel = QInputDialog::getText(this, tr("Rename module"), tr("New module label:"), QLineEdit::Normal, lOldLabel, &lOk);

      if(!lOk)
        break;

      if(lOldLabel!=lNewLabel)
      {
        if(mModuleWidgetMap.find(lNewLabel)!=mModuleWidgetMap.end())
        {
          int lAnswer = QMessageBox::warning(this, mTitleSoftware, QString(tr("A module with name %1 already exist.  Do you want to choose another name?")).arg(lNewLabel), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
          if(lAnswer!=QMessageBox::Yes)
            break;
        }
        else
        {
          mModuleWidgetMap.remove(lOldLabel);
          mModuleWidgetMap.insert(lNewLabel, lTmpModuleWidget);
          lTmpModuleWidget->getModule()->setLabel(lNewLabel.toStdString().c_str());
          lTmpModuleWidget->updateState();

          emit updatedModuleInstanceInfoMap(getModuleInstanceInfoMap());
          emit updatedModuleInstanceParameter(lOldLabel, lNewLabel);

          break;
        }
      }
      else
      {
        break;
      }
    }

  }
}

//-------------------------------------------------------------------------------

void MainWindow::modifyAppPreferences()
{
  QMessageBox::warning(this, mTitleSoftware, tr("Sorry, this functionality is not yet implemented!"), QMessageBox::Ok);
}

//-------------------------------------------------------------------------------

void MainWindow::newMonitor()
{
  MonitorWindow* lMonitorWindow = new MonitorWindow(NULL);
  mMonitorWindowList.insert(lMonitorWindow);

  connect(lMonitorWindow, SIGNAL(closeRequest(MonitorWindow*)), this, SLOT(closeMonitorWindow(MonitorWindow*)));
  connect(this, SIGNAL(updatedModuleInstanceInfoMap(const ModuleInstanceInfoMap&)), lMonitorWindow, SLOT(updateModuleInstanceInfoMap(const ModuleInstanceInfoMap&)));

  QAction* lAction = new QAction(lMonitorWindow->getLabel(), this);
  connect(lAction, SIGNAL(triggered()), this, SLOT(listMonitor()));
  lMonitorWindow->setMonitorAccessAction(lAction);
  lMonitorWindow->updateModuleInstanceInfoMap(getModuleInstanceInfoMap());

  mMenuListMonitor->addAction(lAction);

  lMonitorWindow->show();
}

//-------------------------------------------------------------------------------

void MainWindow::listMonitor()
{
  QAction *lAction = qobject_cast<QAction *>(sender());
  if(lAction)
  {
    foreach(MonitorWindow* lMonitorWindow, mMonitorWindowList)
    {
      if(lMonitorWindow->getMonitorAccessAction()==lAction)
      {
        lMonitorWindow->raise();
        break;
      }
    }
  }
}

//-------------------------------------------------------------------------------

void MainWindow::viewModulesListDock()
{
  if(mActionViewModulesListDock->isChecked())
    mModulesListDock->show();
  else
    mModulesListDock->hide();
}

//-------------------------------------------------------------------------------

void MainWindow::viewModuleParametersDock()
{
  if(mActionViewModuleParametersDock->isChecked())
    mModuleParameterDock->show();
  else
    mModuleParameterDock->hide();
}

//-------------------------------------------------------------------------------

void MainWindow::viewModuleDescription()
{
  if(mActionViewModuleDescription->isChecked())
    mModuleDescriptionArea->show();
  else
    mModuleDescriptionArea->hide();
}

//-------------------------------------------------------------------------------

void MainWindow::viewStatusbar()
{
  if(mActionViewStatusbar->isChecked())
    statusBar()->show();
  else
    statusBar()->hide();
}

//-------------------------------------------------------------------------------

void MainWindow::viewFileToolbar()
{
  if(mActionViewFileToolbar->isChecked())
    mFileToolBar->show();
  else
    mFileToolBar->hide();
}

//-------------------------------------------------------------------------------

void MainWindow::viewControlToolbar()
{
  if(mActionViewControlToolbar->isChecked())
    mControlToolBar->show();
  else
    mControlToolBar->hide();
}

//-------------------------------------------------------------------------------

void MainWindow::viewEditToolbar()
{
  if(mActionViewEditToolbar->isChecked())
    mEditToolBar->show();
  else
    mEditToolBar->hide();
}

//-------------------------------------------------------------------------------

void MainWindow::viewMonitorToolbar()
{
  if(mActionViewMonitorToolbar->isChecked())
    mMonitorToolBar->show();
  else
    mMonitorToolBar->hide();
}

//-------------------------------------------------------------------------------

void MainWindow::changeModuleDescription(int inCurrentRow)
{
  if(inCurrentRow>=0)
  {
    QString lName = mModuleNameMap.value(inCurrentRow);
    mModuleDescription->setText(mModuleInfoMap.value(lName).getLongDescription());
  }
  else
    mModuleDescription->setText("");
}

//-------------------------------------------------------------------------------

void MainWindow::doubleClickedModuleListItem(QListWidgetItem* inItem)
{

}

//-------------------------------------------------------------------------------

void MainWindow::updateState()
{
  processKernelStateNotifications();
}

//-------------------------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent* inEvent)
{
  clear();
  if(mKernel)
    mKernel->clear();
  QMainWindow::closeEvent(inEvent);
}

//-------------------------------------------------------------------------------

void MainWindow::dragEnterEvent(QDragEnterEvent* inEvent)
{
  QMainWindow::dragEnterEvent(inEvent);
}

//-------------------------------------------------------------------------------

void MainWindow::dragLeaveEvent(QDragLeaveEvent* inEvent)
{
  QMainWindow::dragLeaveEvent(inEvent);
}

//-------------------------------------------------------------------------------

void MainWindow::dragMoveEvent(QDragMoveEvent* inEvent)
{
  QMainWindow::dragMoveEvent(inEvent);
}

//-------------------------------------------------------------------------------

void MainWindow::dropEvent(QDropEvent* inEvent)
{
  QMainWindow::dropEvent(inEvent);
}

//-------------------------------------------------------------------------------

bool MainWindow::event(QEvent* inEvent)
{
  if(inEvent->type()==mKernelStateNotifier.getEventType())
  {
    processKernelStateNotifications();
    return true;
  }
  else
    return QMainWindow::event(inEvent);
}

//-------------------------------------------------------------------------------

void MainWindow::keyPressEvent(QKeyEvent* inEvent)
{
  QMainWindow::keyPressEvent(inEvent);
  if(!inEvent->isAccepted() && inEvent->key()==Qt::Key_Escape)
  {
    if((mMode==MainWindow::eModeEditConnectSlots || mMode==MainWindow::eModeEditNormal)  && ModuleSlotWidget::getModuleSlotSelectedFirst()!=NULL)
    {
      ModuleSlotWidget::getModuleSlotSelectedFirst()->updateState();
      ModuleSlotWidget::setModuleSlotSelectedFirst(NULL);
      inEvent->accept();
    }
    else
    {
      inEvent->ignore();
    }
  }
  else if(!inEvent->isAccepted() && inEvent->key()==Qt::Key_Delete)
  {
    if((mMode==MainWindow::eModeEditModuleAdd || mMode==MainWindow::eModeEditNormal) && mSelectedModuleWidget)
    {
      removeModule();
      inEvent->accept();
    }
    else if((mMode==MainWindow::eModeEditConnectSlots || mMode==MainWindow::eModeEditNormal) && mSelectedModuleSlotLink)
    {
      disconnectModuleSlot();
      inEvent->accept();
    }
    else
    {
      inEvent->ignore();
    }
  }
  else
    inEvent->ignore();
}

//-------------------------------------------------------------------------------

void MainWindow::keyReleaseEvent(QKeyEvent* inEvent)
{
  QMainWindow::keyReleaseEvent(inEvent);
  if(!inEvent->isAccepted() && inEvent->key()==Qt::Key_Control && mMode==MainWindow::eModeEditNormal)
  {
    if(ModuleSlotWidget::getModuleSlotSelectedFirst())
    {
      ModuleSlotWidget::getModuleSlotSelectedFirst()->updateState();
      ModuleSlotWidget::setModuleSlotSelectedFirst(NULL);
    }
    if(ModuleSlotWidget::getModuleSlotHighlighted())
    {
      ModuleSlotWidget::getModuleSlotHighlighted()->updateState();
      ModuleSlotWidget::setModuleSlotHighlighted(NULL);
    }
    inEvent->accept();
  }
  else
    inEvent->ignore();
}

//-------------------------------------------------------------------------------

void MainWindow::mouseMoveEvent(QMouseEvent* inEvent)
{
  QMainWindow::mouseMoveEvent(inEvent);
}

//-------------------------------------------------------------------------------

void MainWindow::mousePressEvent(QMouseEvent* inEvent)
{
  QMainWindow::mousePressEvent(inEvent);
}

//-------------------------------------------------------------------------------

void MainWindow::mouseReleaseEvent(QMouseEvent* inEvent)
{
  QMainWindow::mouseReleaseEvent(inEvent);
}

//-------------------------------------------------------------------------------

void MainWindow::updateApplicationState()
{
  if(mMode==MainWindow::eModeEditNormal)
  {
    mStatusLabel->setText(tr("Status: Stopped"));
    mFrameLabel->setText(tr("Frame: N/A"));

    mModulesListDock->setEnabled(true);
    mModuleParameterDock->setEnabled(true);

    mActionNewProject->setEnabled(true);
    mActionOpenProject->setEnabled(true);
    mActionSaveProject->setEnabled(true);
    mActionSaveProjectAs->setEnabled(true);
    mActionFileInfo->setEnabled(true);

    mActionStop->setEnabled(true);
    mActionStart->setEnabled(true);
    mActionPause->setEnabled(false);
    mActionStep->setEnabled(false);
    mActionRefresh->setEnabled(false);
    mActionReset->setEnabled(true);

    mActionNewMonitor->setEnabled(true);
    mMenuListMonitor->setEnabled(true);
    mButtonListMonitor->setEnabled(true);

    mModuleParameterStack->setRuntimeState(false);

    updateEditState();
  }
  else if(mMode==MainWindow::eModeEditModuleAdd)
  {
    mModulesListDock->setEnabled(true);
    mModuleParameterDock->setEnabled(false);

    mActionNewProject->setEnabled(false);
    mActionOpenProject->setEnabled(false);
    mActionSaveProject->setEnabled(false);
    mActionSaveProjectAs->setEnabled(false);
    mActionFileInfo->setEnabled(false);

    mActionStop->setEnabled(false);
    mActionStart->setEnabled(false);
    mActionPause->setEnabled(false);
    mActionStep->setEnabled(false);
    mActionRefresh->setEnabled(false);
    mActionReset->setEnabled(false);

    mActionAddModule->setChecked(true);

    mActionConnectModuleSlot->setEnabled(false);
    mActionAddModule->setEnabled(true);
    mActionRemoveModule->setEnabled(false);
    mActionDisconnectModuleSlot->setEnabled(false);

    mActionNewMonitor->setEnabled(false);
    mMenuListMonitor->setEnabled(false);
    mButtonListMonitor->setEnabled(false);

    updateEditState();
  }
  else if(mMode==MainWindow::eModeEditConnectSlots)
  {
    mModulesListDock->setEnabled(false);
    mModuleParameterDock->setEnabled(false);

    mActionNewProject->setEnabled(false);
    mActionOpenProject->setEnabled(false);
    mActionSaveProject->setEnabled(false);
    mActionSaveProjectAs->setEnabled(false);
    mActionFileInfo->setEnabled(false);

    mActionStop->setEnabled(false);
    mActionStart->setEnabled(false);
    mActionPause->setEnabled(false);
    mActionStep->setEnabled(false);
    mActionRefresh->setEnabled(false);
    mActionReset->setEnabled(false);

    mActionConnectModuleSlot->setChecked(true);

    mActionConnectModuleSlot->setEnabled(true);
    mActionAddModule->setEnabled(false);
    mActionRemoveModule->setEnabled(false);
    mActionDisconnectModuleSlot->setEnabled(false);

    mActionNewMonitor->setEnabled(false);
    mMenuListMonitor->setEnabled(false);
    mButtonListMonitor->setEnabled(false);

    updateEditState();
  }
  else if(mMode==MainWindow::eModeProcessing)
  {
    mModulesListDock->setEnabled(false);
    mModuleParameterDock->setEnabled(true);

    mActionNewProject->setEnabled(false);
    mActionOpenProject->setEnabled(false);
    mActionSaveProject->setEnabled(false);
    mActionSaveProjectAs->setEnabled(false);
    mActionFileInfo->setEnabled(false);

    mActionStop->setEnabled(true);
    mActionStart->setEnabled(true);
    mActionPause->setEnabled(true);
    mActionStep->setEnabled(false);
    mActionRefresh->setEnabled(false);
    mActionReset->setEnabled(false);

    mActionConnectModuleSlot->setEnabled(false);
    mActionAddModule->setEnabled(false);
    mActionRemoveModule->setEnabled(false);
    mActionDisconnectModuleSlot->setEnabled(false);

    mActionNewMonitor->setEnabled(true);
    mMenuListMonitor->setEnabled(true);
    mButtonListMonitor->setEnabled(true);

    mModuleParameterStack->setRuntimeState(true);
  }
}

//-------------------------------------------------------------------------------

void MainWindow::updateEditState()
{
  if(mMode==MainWindow::eModeEditNormal)
  {
    mActionConnectModuleSlot->setEnabled(mModuleWidgetMap.count()>1);
    mActionAddModule->setEnabled(mModulesList->currentRow()>=0);
    mActionRemoveModule->setEnabled(mSelectedModuleWidget!=NULL);
    mActionDisconnectModuleSlot->setEnabled(mSelectedModuleSlotLink!=NULL);
    mActionRenameModule->setEnabled(mSelectedModuleWidget!=NULL);
  }
  else if(mMode==MainWindow::eModeEditModuleAdd)
  {
    mActionRemoveModule->setEnabled(mSelectedModuleWidget!=NULL);
    mActionRenameModule->setEnabled(mSelectedModuleWidget!=NULL);
  }
  else if(mMode==MainWindow::eModeEditConnectSlots)
  {
    mActionDisconnectModuleSlot->setEnabled(mSelectedModuleSlotLink!=NULL);
  }
}

//-------------------------------------------------------------------------------

void MainWindow::updateProcessingState(KernelState::State inKernelState)
{
  if(inKernelState==KernelState::eStateStarted)
  {
    mActionStop->setEnabled(true);
    mActionStart->setEnabled(true);
    mActionPause->setEnabled(true);
    mActionStep->setEnabled(false);
    mActionRefresh->setEnabled(false);
    mActionReset->setEnabled(false);
    mActionStart->setChecked(true);
  }
  else if(inKernelState==KernelState::eStatePaused)
  {
    mActionStop->setEnabled(true);
    mActionStart->setEnabled(true);
    mActionPause->setEnabled(true);
    mActionStep->setEnabled(true);
    mActionRefresh->setEnabled(true);
    mActionReset->setEnabled(false);
    mActionPause->setChecked(true);
  }
  else
  {
    mActionStop->setEnabled(true);
    mActionStart->setEnabled(true);
    mActionPause->setEnabled(false);
    mActionStep->setEnabled(false);
    mActionRefresh->setEnabled(false);
    mActionReset->setEnabled(true);
    mActionStop->setChecked(true);
  }
}

//-------------------------------------------------------------------------------

void MainWindow::removeModule(ModuleWidget* inModuleWidget)
{
  Q_ASSERT(inModuleWidget);

  ModuleSlotWidget* lTmpInputModuleSlotWidget;
  ModuleSlotWidget* lTmpOutputModuleSlotWidget;

  // Disconnect all input slots
  foreach(ModuleSlotWidget* lInputModuleSlot, inModuleWidget->getInputModuleSlotWidget())
  {
    foreach(ModuleSlotLink* lInputModuleSlotLink, lInputModuleSlot->getModuleSlotLinkList())
    {
      disconnectModuleSlot(lInputModuleSlotLink, &lTmpInputModuleSlotWidget, &lTmpOutputModuleSlotWidget);
      lTmpOutputModuleSlotWidget->updateState();
    }
  }

  // Disconnect all output slots
  foreach(ModuleSlotWidget* lOutputModuleSlot, inModuleWidget->getOutputModuleSlotWidget())
  {
    foreach(ModuleSlotLink* lOutputModuleSlotLink, lOutputModuleSlot->getModuleSlotLinkList())
    {
      disconnectModuleSlot(lOutputModuleSlotLink, &lTmpInputModuleSlotWidget, &lTmpOutputModuleSlotWidget);
      lTmpInputModuleSlotWidget->updateState();
    }
  }

  mModuleWidgetMap.remove(inModuleWidget->getModule()->getLabel().c_str());
  
  try
  {
    mKernel->deleteModule(inModuleWidget->getModule());
  }
  catch(Exception& inException)
  {
    QMessageBox::critical(this, mTitleSoftware, inException.toString().c_str());
  }
  catch(...)
  {
    QMessageBox::critical(this, mTitleSoftware, tr("Error: Could not disconnect two module slots; the reason is unknown"));
  }

  inModuleWidget->disconnect();
  inModuleWidget->close();  
  
  delete inModuleWidget;
}

//-------------------------------------------------------------------------------

void MainWindow::disconnectModuleSlot(ModuleSlotLink* inModuleSlotLink, ModuleSlotWidget** ioInputModuleSlotWidget, ModuleSlotWidget** ioOutputModuleSlotWidget)
{
  Q_ASSERT(inModuleSlotLink);

  Module* lInputModule = inModuleSlotLink->getInputModuleSlotWidget()->getModule();
  ModuleSlotWidget* lInputModuleSlotWidget = inModuleSlotLink->getInputModuleSlotWidget();
  ModuleSlotWidget* lOutputModuleSlotWidget = inModuleSlotLink->getOutputModuleSlotWidget();
  QString lSlotName = inModuleSlotLink->getInputModuleSlotWidget()->getModuleSlot()->getName().c_str();

  inModuleSlotLink->detach(mModulesLayoutScene);

  try
  {
    lInputModule->disconnect(lSlotName.toStdString().c_str());
  }
  catch(Exception& inException)
  {
    QMessageBox::critical(this, mTitleSoftware, inException.toString().c_str());
  }
  catch(...)
  {
    QMessageBox::critical(this, mTitleSoftware, tr("Error: Could not disconnect two module slots; the reason is unknown"));
  }

  delete inModuleSlotLink;

  if(ioInputModuleSlotWidget)
    *ioInputModuleSlotWidget = lInputModuleSlotWidget;
  if(ioOutputModuleSlotWidget)
    *ioOutputModuleSlotWidget = lOutputModuleSlotWidget;
}

//-------------------------------------------------------------------------------

void MainWindow::clear()
{
  MonitorWindowList lMonitorWindowList = mMonitorWindowList;
  ModuleWidgetMap lModuleWidgetMap = mModuleWidgetMap;

  foreach(MonitorWindow* lMonitorWindow, lMonitorWindowList)
    lMonitorWindow->close();
  foreach(ModuleWidget* lModuleWidget, lModuleWidgetMap)
    removeModule(lModuleWidget);

  Q_ASSERT(mKernel->getModuleCount()==0);
  Q_ASSERT(mModuleWidgetMap.count()==0);
  Q_ASSERT(mMonitorWindowList.count()==0);
}

//-------------------------------------------------------------------------------

ModuleInstanceInfoMap MainWindow::getModuleInstanceInfoMap() const
{
  ModuleInstanceInfoMap lModuleInstanceInfoMap;

  if(mKernel && mKernel->getModuleCount()>0)
  {
    ModuleSet lModuleSet = mKernel->getModules();
    for(ModuleSet::iterator lModuleItr = lModuleSet.begin(); lModuleItr != lModuleSet.end(); lModuleItr++)
      lModuleInstanceInfoMap.insert((*lModuleItr)->getLabel().c_str(), ModuleInstanceInfo((*lModuleItr)->getLabel().c_str(), *lModuleItr, mModuleInfoMap[(*lModuleItr)->getName().c_str()].getIcon()));
  }

  return lModuleInstanceInfoMap;
}

//-------------------------------------------------------------------------------

void MainWindow::processKernelStateNotifications()
{
  KernelState lKernelState;
  bool lPostUIEvent;
  KernelState::State lNewState;
  KernelState::State lOldState;

  mKernelStateNotifier.lock();
  lKernelState = mKernelStateNotifier.getStateLocked();
  lPostUIEvent = mKernelStateNotifier.getPostUIEventLocked();
  mKernelStateNotifier.unlock();

  lNewState = lKernelState.getState();
  lOldState = mKernelState.getState();

  //std::cout << "RECV1: " << lNewState << std::endl;
  
  if(lPostUIEvent)
  {
    if(lKernelState.isExceptionRaised())
    {
      Exception lException = lKernelState.getException();
      QMessageBox::critical(this, mTitleSoftware, lException.toString().c_str());
    }
    //std::cout << "RECV2: " << lNewState << std::endl;
  
    switch(lNewState)
    {
      case KernelState::eStateUninitialized:
        if(lNewState!=lOldState)
        {
          mStatusLabel->setText(tr("State: Uninitialized"));
          mMode = eModeEditNormal;
          updateApplicationState();
          updateProcessingState(lKernelState);
        }
        mFrameLabel->setText(tr("Frame: N/A"));
        break;
      case KernelState::eStateInitialized:
        if(lNewState!=lOldState)
          mStatusLabel->setText(tr("State: Initialized"));
        mFrameLabel->setText(QString(tr("Frame: %1")).arg(lKernelState.getFrame()));
        break;
      case KernelState::eStatePaused:
        if(lNewState!=lOldState)
        {
          mStatusLabel->setText(tr("State: Paused"));
          updateProcessingState(lKernelState);
        }
        mFrameLabel->setText(QString(tr("Frame: %1")).arg(lKernelState.getFrame()));
        break;
      case KernelState::eStateStarted:
        if(lNewState!=lOldState)
        {
          mStatusLabel->setText(tr("State: Started"));
          updateProcessingState(lKernelState);
        }
        mFrameLabel->setText(QString(tr("Frame: %1")).arg(lKernelState.getFrame()));
        break;
      case KernelState::eStateStopped:
        //if(lNewState!=lOldState)
        //{
          mStatusLabel->setText(tr("State: Stopped"));
          mMode = eModeEditNormal;
          updateApplicationState();
          updateProcessingState(lKernelState);
        //}
        mFrameLabel->setText(QString(tr("Frame: %1")).arg(lKernelState.getFrame()));
        break;
      default:
        qFatal("Unknown kernel state %i", lKernelState.getState());
    }

    updateMonitors();
    mKernelState.setState(lKernelState);

  }

  QApplication::processEvents();
}

//-------------------------------------------------------------------------------


void MainWindow::updateMonitors()
{
  foreach(MonitorWindow* lMonitorWindow, mMonitorWindowList)
  {
    lMonitorWindow->updateImage();
    QApplication::processEvents();
  }
}

