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

#include "MonitorWindow.hpp"
#include "VisualVipersConfig.hpp"

#include <Module.hpp>

#include <QAction>
#include <QImage>
#include <QLabel>
#include <QSize>
#include <QScrollArea>
#include <QToolBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QSlider>
#include <QCloseEvent>
#include <QResizeEvent>
#include <QtGlobal>
#include <QVector>

#include <QDebug>

#include <cstdlib>
#include <stdint.h>

//-------------------------------------------------------------------------------

unsigned int MonitorWindow::mMonitorIDCount = 0;

//-------------------------------------------------------------------------------

MonitorWindow::MonitorWindow(QWidget* inParent)
: QDialog(inParent, Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowSystemMenuHint)
{
  setAttribute(Qt::WA_DeleteOnClose, true);
  setSizeGripEnabled(true);
  setWindowIcon(QIcon(":/images/monitor"));
  
  mMonitorLabel = QString(tr("Monitor%1")).arg(getID());
  setWindowTitle(mMonitorLabel);
  mMonitorAccessAction = NULL;
  mZoomChange = true;

  mCurrentModuleIndex = -1;
  mCurrentModuleSlotIndex = -1;

  mImageWidth = 0;
  mImageHeight = 0;
  mImageDepth = Image::eDepthUndefined;
  mImageNbChannels = Image::eChannel0;
  mImageColorModel = Image::eModelUndefined;

  mImageTmp = NULL;
  mImageTmpData = NULL;

  createActions();
  createLayout();

  mImageState = eImageStateNone;
  updateImageState(eImageStateNoModule);
}

//-------------------------------------------------------------------------------

MonitorWindow::~MonitorWindow()
{
  if(mImageTmp)
    delete mImageTmp;
  if(mImageTmpData)
    delete [] mImageTmpData;
}

//-------------------------------------------------------------------------------

void MonitorWindow::notify()
{
  Notifier::notify();
}

//-------------------------------------------------------------------------------

void MonitorWindow::updateImage()
{
  Module* lModule = 0;
  const ModuleSlot* lModuleSlot = 0;
  const Image* lImage = 0;
  bool lImageChanged = false;
  bool lImageToShow = false;
  bool lImageStateUpdated = false;
  int lChannelIdx1, lChannelIdx2, lChannelIdx3;

  lock();

  lModule = getModuleMutexLocked();
  lModuleSlot = getModuleSlotMutexLocked();

  //Check module validity
  if(!lModule)
  {
    unlock();
    updateImageState(eImageStateNoModule);
    return;
  }

  //Check module slot validity
  if(!lModuleSlot)
  {
    unlock();
    updateImageState(eImageStateNoSlot);
    return;
  }

  //Check image validity
  lModuleSlot->lock();
  lImage = lModuleSlot->getImage();

  if(!lImage || !lImage->isValid())
  {
    lModuleSlot->unlock();
    unlock();
    updateImageState(eImageStateNotInitialized);
    return;
  }

  // Check if format has changed
  if(lImage->getWidth()!=mImageWidth || lImage->getHeight()!=mImageHeight || lImage->getDepth()!=mImageDepth || lImage->getNbChannels()!=mImageNbChannels || lImage->getModel()!=mImageColorModel)
  {
    mImageWidth = lImage->getWidth();
    mImageHeight = lImage->getHeight();
    mImageDepth = lImage->getDepth();
    mImageNbChannels = lImage->getNbChannels();
    mImageColorModel = lImage->getModel();
    lImageChanged = true;

    if(mImageTmp)
    {
      delete mImageTmp;
      mImageTmp = NULL;

      if(mImageTmpData)
      {
        delete [] mImageTmpData;
        mImageTmpData = NULL;
      }
    }
  }

  //Convert image to RGBA or 8 bits gray scale for QT
  // 8 bpp
  if(lImage->getDepth()==Image::eDepth8U)
  {
    // 1 channel
    if(lImage->getNbChannels()==Image::eChannel1)
    {
      lImageToShow = true;
      if(!mImageTmp)
      {
        mImageTmpData = new uchar[mImageWidth*mImageHeight];
        mImageTmp = new QImage(mImageTmpData, mImageWidth, mImageHeight, QImage::Format_Indexed8);
        QVector<QRgb> lColorTable;
        for(int i = 0; i < 256; i++)
          lColorTable.push_back(qRgb(i, i, i));
        mImageTmp->setColorTable(lColorTable);
      }

      const uchar* lBuffSrc = (const uchar*)(lImage->getData());
      uchar* lBuffDest = mImageTmpData;

      // Make a line by line copy of the image
      for(unsigned int lY = 0; lY < mImageHeight; ++lY)
      {
        ::memcpy(lBuffDest, lBuffSrc, mImageWidth);
        lBuffDest += mImageWidth;
        lBuffSrc += lImage->getRowLengthBytes();
      }

    }
    // 3 channels
    else if(lImage->getNbChannels()==Image::eChannel3)
    {
      lImageToShow = true;
      if(!mImageTmp)
      {
        mImageTmpData = new uchar[mImageWidth*mImageHeight*4];
        mImageTmp = new QImage(mImageTmpData, mImageWidth, mImageHeight, QImage::Format_RGB32);
      }

      const uchar* lBuffSrc = (const uchar*)(lImage->getData());
      uchar* lBuffDest = mImageTmpData;

      if(lImage->getModel()==Image::eModelRGB)
      {
        lChannelIdx1 = 0;
        lChannelIdx2 = 1;
        lChannelIdx3 = 2;
      }
      else
      {
        lChannelIdx1 = 2;
        lChannelIdx2 = 1;
        lChannelIdx3 = 0;
      }

      for (unsigned int lY = 0; lY < mImageHeight; ++lY)
      {
        for (unsigned int lX = 0; lX < mImageWidth; ++lX)
        {
          lBuffDest[0] = lBuffSrc[lChannelIdx1];
          lBuffDest[1] = lBuffSrc[lChannelIdx2];
          lBuffDest[2] = lBuffSrc[lChannelIdx3];
          lBuffDest[3] = 0;

          lBuffDest += 4;
          lBuffSrc += 3;
        }
        lBuffSrc += lImage->getRowLengthBytes() - 3*mImageWidth;
      }

    }
    // 4 channels
    else if(lImage->getNbChannels()==Image::eChannel4)
    {
      lImageToShow = true;
      if(!mImageTmp)
      {
        mImageTmpData = new uchar[mImageWidth*mImageHeight*4];
        mImageTmp = new QImage(mImageTmpData, mImageWidth, mImageHeight, QImage::Format_RGB32);
      }

      const uchar* lBuffSrc = (const uchar*)(lImage->getData());
      uchar* lBuffDest = mImageTmpData;

      for (unsigned int lY = 0; lY < mImageHeight; ++lY)
      {
        for (unsigned int lX = 0; lX < mImageWidth; ++lX)
        {
          lBuffDest[0] = lBuffSrc[2];
          lBuffDest[1] = lBuffSrc[1];
          lBuffDest[2] = lBuffSrc[0];
          lBuffDest[3] = 0;

          lBuffDest += 4;
          lBuffSrc += 4;
        }
      }

    }
  }
  // 16 bpp, 1 channel
  else if(mImageDepth==Image::eDepth16U && lImage->getNbChannels()==Image::eChannel1 && (lImage->getModel()==Image::eModelUndefined || lImage->getModel()==Image::eModelGray))
  {
    lImageToShow = true;
    if(!mImageTmp)
    {
      mImageTmpData = new uchar[mImageWidth*mImageHeight];
      mImageTmp = new QImage(mImageTmpData, mImageWidth, mImageHeight, QImage::Format_Indexed8);
      QVector<QRgb> lColorTable;
      for(int i = 0; i < 256; i++)
        lColorTable.push_back(qRgb(i, i, i));
      mImageTmp->setColorTable(lColorTable);
    }

    const uint16_t* lBuffSrc = (const uint16_t*)(lImage->getData());
    uchar* lBuffDest = mImageTmpData;

    for(unsigned int lY = 0; lY < mImageHeight; ++lY)
    {
      for(unsigned int lX = 0; lX < mImageWidth; ++lX)
      {
        // Divide by 256 (only keep the highest part of the 16 bits value)
        *lBuffDest++ = ((*lBuffSrc++) >> 8);
      }
      lBuffSrc += lImage->getRowLengthBytes() / sizeof(uint16_t) - mImageWidth;
    }

  }
  // 32 bpp, 1 channel
  else if(mImageDepth==Image::eDepth32F && lImage->getNbChannels()==Image::eChannel1 && (lImage->getModel()==Image::eModelUndefined || lImage->getModel()==Image::eModelGray))
  {
    lImageToShow = true;
    if(!mImageTmp)
    {
      mImageTmpData = new uchar[mImageWidth*mImageHeight];
      mImageTmp = new QImage(mImageTmpData, mImageWidth, mImageHeight, QImage::Format_Indexed8);
      QVector<QRgb> lColorTable;
      for(int i = 0; i < 256; i++)
        lColorTable.push_back(qRgb(i, i, i));
      mImageTmp->setColorTable(lColorTable);
    }

    const float* lBuffSrc = (const float*)(lImage->getData());
    uchar* lBuffDest = mImageTmpData;

    for(unsigned int lY = 0; lY < mImageHeight; ++lY)
    {
      for(unsigned int lX = 0; lX < mImageWidth; ++lX)
      {
        *lBuffDest++ = (uchar)(255 * ((*lBuffSrc++)));
      }
      lBuffSrc += lImage->getRowLengthBytes() / sizeof(float) - mImageWidth;
    }

  }
  // 64 bpp, 1 channel
  else if(mImageDepth==Image::eDepth64F && lImage->getNbChannels()==Image::eChannel1 && (lImage->getModel()==Image::eModelUndefined || lImage->getModel()==Image::eModelGray))
  {
    lImageToShow = true;
    if(!mImageTmp)
    {
      mImageTmpData = new uchar[mImageWidth*mImageHeight];
      mImageTmp = new QImage(mImageTmpData, mImageWidth, mImageHeight, QImage::Format_Indexed8);
      QVector<QRgb> lColorTable;
      for(int i = 0; i < 256; i++)
        lColorTable.push_back(qRgb(i, i, i));
      mImageTmp->setColorTable(lColorTable);
    }

    const double* lBuffSrc = (const double*)(lImage->getData());
    uchar* lBuffDest = mImageTmpData;

    for(unsigned int lY = 0; lY < mImageHeight; ++lY)
    {
      for(unsigned int lX = 0; lX < mImageWidth; ++lX)
      {
        *lBuffDest++ = (uchar)(255 * ((*lBuffSrc++)));
      }
      lBuffSrc += lImage->getRowLengthBytes() / sizeof(double) - mImageWidth;
    }

  }

  lModuleSlot->unlock();
  unlock();

  if(lImageToShow)
  {
    lImageStateUpdated = updateImageState(eImageStateValid);

    // Bug for Windows: If previous pixmap is the same size, it is not replaced by new one
    // Workaround: Set the pixmap to nothing before setting it to the new frame
    #ifdef Q_OS_WIN32
    mImage->setPixmap(QPixmap());
    #endif

    mImage->setPixmap(QPixmap::fromImage(*mImageTmp));

    if(lImageChanged || lImageStateUpdated)
      resizeImage();	
  }
  else
  {
    updateImageState(eImageStateUnsupportedFormat);
  }

}

//-------------------------------------------------------------------------------

void MonitorWindow::updateModuleInstanceInfoMap(const ModuleInstanceInfoMap& inModuleInstanceInfoMap)
{
  Module* lModule;

  if(mModuleInstanceInfoMap.count()==0)
  {
    mCurrentModuleIndex = -1;
    mCurrentModuleSlotIndex = -1;
    mModuleComboBox->clear();
    mModuleSlotComboBox->clear();
    mModuleComboBox->addItem("(None selected)");
    mModuleSlotComboBox->setEnabled(false);
    foreach(ModuleInstanceInfo lModuleIntInfo, inModuleInstanceInfoMap)
    {
      mModuleComboBox->addItem(lModuleIntInfo.getIcon(), lModuleIntInfo.getLabel());
    }
    mModuleInstanceInfoMap = inModuleInstanceInfoMap;
    updateImage();
  }
  else
  {
    lock();
    lModule = getModuleMutexLocked();

    if(lModule)
    {
      int lCount = 0;
      mCurrentModuleIndex = 0;

      mModuleComboBox->clear();
      mModuleComboBox->addItem("(None selected)");

      foreach(ModuleInstanceInfo lModuleIntInfo, inModuleInstanceInfoMap)
      {
        lCount++;
        if(lModuleIntInfo.getModule()==lModule)
          mCurrentModuleIndex = lCount;
        mModuleComboBox->addItem(lModuleIntInfo.getIcon(), lModuleIntInfo.getLabel());
      }

      Q_ASSERT(mCurrentModuleIndex);

      mModuleInstanceInfoMap = inModuleInstanceInfoMap;

      unlock();

      mModuleComboBox->setCurrentIndex(mCurrentModuleIndex);
      updateImage();
    }
    else
    {
      unlock();
      mCurrentModuleIndex = -1;
      mCurrentModuleSlotIndex = -1;
      mModuleComboBox->clear();
      mModuleSlotComboBox->clear();
      mModuleSlotComboBox->setEnabled(false);
      mModuleComboBox->addItem("(None selected)");
      foreach(ModuleInstanceInfo lModuleIntInfo, inModuleInstanceInfoMap)
        mModuleComboBox->addItem(lModuleIntInfo.getIcon(), lModuleIntInfo.getLabel());
      mModuleInstanceInfoMap = inModuleInstanceInfoMap;
      updateImage();
    }
  }
}

//-------------------------------------------------------------------------------

void MonitorWindow::activateModule(const QString& inStr)
{
  if(mCurrentModuleIndex!=mModuleComboBox->currentIndex())
  {
    detach();

    mCurrentModuleIndex = mModuleComboBox->currentIndex();
    mCurrentModuleSlotIndex = -1;
    mModuleSlotComboBox->clear();

    if(mCurrentModuleIndex!=0)
    {
      mModuleSlotComboBox->setEnabled(true);
      ModuleInstanceInfo lModuleInstInfo = mModuleInstanceInfoMap.find(inStr).value();

      Q_ASSERT(lModuleInstInfo.getModule()!=NULL);
      attach(lModuleInstInfo.getModule());

      const ModuleSlotMap& lOutputSlotsMap = lModuleInstInfo.getModule()->getOutputSlots();
      //const ModuleSlotMap& lInputSlotsMap = lModuleInstInfo.getModule()->getInputSlots();

      mModuleSlotComboBox->addItem(tr("(None selected)"));

      for(ModuleSlotMap::const_iterator lOutputSlotItr = lOutputSlotsMap.begin(); lOutputSlotItr != lOutputSlotsMap.end(); lOutputSlotItr++)
        mModuleSlotComboBox->addItem(lOutputSlotItr->second->getDisplayName().c_str(), QString(lOutputSlotItr->second->getName().c_str()));
      //for(ModuleSlotMap::const_iterator lInputSlotItr = lInputSlotsMap.begin(); lInputSlotItr != lInputSlotsMap.end(); lInputSlotItr++)
      //  mModuleSlotComboBox->addItem(lInputSlotItr->second->getDisplayName().c_str(), "I" + QString(lInputSlotItr->second->getName().c_str()));
    }
    else
    {
      mModuleSlotComboBox->setEnabled(false);
    }

    updateImage();
  }

}

//-------------------------------------------------------------------------------

void MonitorWindow::activateModuleSlot(const QString& inStr)
{
  if(mCurrentModuleSlotIndex!=mModuleSlotComboBox->currentIndex())
  {
    mCurrentModuleSlotIndex = mModuleSlotComboBox->currentIndex();

    if(mCurrentModuleSlotIndex!=0)
    {
      QString lName = mModuleSlotComboBox->itemData(mModuleSlotComboBox->currentIndex()).toString();

      try
      {
        setModuleSlot(ModuleSlot::eSlotTypeOutput, lName.toStdString().c_str());
      }
      catch(Exception& inException)
      {
        qFatal("%s", inException.toString().c_str());
      }

      /*
      Q_ASSERT(lName.count()>2);

      if(lName[0]=='I')
      {
        try
        {
          setModuleSlot(ModuleSlot::eSlotTypeInput, lName.mid(1).toStdString().c_str());
        }
        catch(Exception& inException)
        {
          qFatal("%s", inException.toString().c_str());
        }
      }
      else if(lName[0]=='O')
      {
        try
        {
          setModuleSlot(ModuleSlot::eSlotTypeOutput, lName.mid(1).toStdString().c_str());
        }
        catch(Exception& inException)
        {
          qFatal("%s", inException.toString().c_str());
        }
      }
      else
      {
        qFatal("%s", "Cannot find the slot type at the beginning of the string");
      }
      */
    }
    else
    {
      unsetModuleSlot();
    }

    updateImage();
  }
}

//-------------------------------------------------------------------------------

void MonitorWindow::magnifyPlus()
{
  mZoomSlider->setValue(mZoomSlider->value()+1);
}

//-------------------------------------------------------------------------------

void MonitorWindow::magnifyMinus()
{
  mZoomSlider->setValue(mZoomSlider->value()-1);
}

//-------------------------------------------------------------------------------

void MonitorWindow::magnifyFit()
{
  if(mActionMagnifyFit->isChecked())
  {
    mZoomChange = false;
    mZoomSlider->setEnabled(false);
    mActionMagnifyMinus->setEnabled(false);
    mActionMagnifyPlus->setEnabled(false);
    mZoomSlider->setValue(0);
    resizeFitImage();
  }
  else
  {
    mZoomChange = true;
    mZoomSlider->setEnabled(true);
    mActionMagnifyMinus->setEnabled(true);
    mActionMagnifyPlus->setEnabled(true);
  }
}

//-------------------------------------------------------------------------------

void MonitorWindow::magnifyOriginal()
{
  if(mActionMagnifyFit->isChecked())
  {
    mActionMagnifyFit->setChecked(false);
    mZoomSlider->setEnabled(true);
    mActionMagnifyMinus->setEnabled(true);
    mActionMagnifyPlus->setEnabled(true);
  }

  resizeOriginalImage();
}

//-------------------------------------------------------------------------------

void MonitorWindow::goFullScreen()
{
  if(mActionFullScreen->isChecked())
    showFullScreen();
  else
    showNormal();
}

//-------------------------------------------------------------------------------

void MonitorWindow::zoomChange(int inValue)
{
  if(mZoomChange && mImage->pixmap())
  {
    QSize lZoomIncrement = mImage->pixmap()->size() / (mZoomSlider->maximum()+1);
    mImage->resize(mImage->pixmap()->size() + inValue*lZoomIncrement);
    mImage->update();
  }
}

//-------------------------------------------------------------------------------

void MonitorWindow::closeEvent(QCloseEvent* inEvent)
{
  detach();
  emit closeRequest(this);
  inEvent->accept();
}

//-------------------------------------------------------------------------------

void MonitorWindow::resizeEvent(QResizeEvent* inEvent)
{
  if(mActionMagnifyFit->isChecked())
  {
    resizeFitImage();
    inEvent->accept();
  }
  else
    QDialog::resizeEvent(inEvent);
}

//-------------------------------------------------------------------------------

void MonitorWindow::resizeFitImage()
{
  if(mImage->pixmap() && mImage->pixmap()->width()>0 && mImage->pixmap()->height()>0)
  {
    QSize lImageSize = mImage->pixmap()->size();
    lImageSize.scale(mScrollArea->maximumViewportSize(), Qt::KeepAspectRatio);
    mImage->resize(lImageSize);
    mImage->update();
  }
}

//-------------------------------------------------------------------------------

void MonitorWindow::resizeOriginalImage()
{
  if(mImage->pixmap())
  {
    mZoomChange = false;
    mZoomSlider->setValue(0);
    mZoomChange = true;
    resize(size() + (mImage->pixmap()->size() - mScrollArea->maximumViewportSize()));
    mImage->adjustSize();
    mImage->update();
  }
}

//-------------------------------------------------------------------------------

void MonitorWindow::resizeImage()
{
  if(mActionMagnifyFit->isChecked())
    resizeFitImage();
  else
    resizeOriginalImage();
}

//-------------------------------------------------------------------------------

void MonitorWindow::createActions()
{
  mActionMagnifyPlus = new QAction(tr("Zoom in"), this);
  mActionMagnifyPlus->setToolTip(tr("Zoom in"));
  mActionMagnifyPlus->setIcon(QIcon(":/images/viewmag+"));
  connect(mActionMagnifyPlus, SIGNAL(triggered()), this, SLOT(magnifyPlus()));

  mActionMagnifyMinus = new QAction(tr("Zoom out"), this);
  mActionMagnifyMinus->setToolTip(tr("Zoom out"));
  mActionMagnifyMinus->setIcon(QIcon(":/images/viewmag-"));
  connect(mActionMagnifyMinus, SIGNAL(triggered()), this, SLOT(magnifyMinus()));

  mActionMagnifyFit = new QAction(tr("Fit"), this);
  mActionMagnifyFit->setToolTip(tr("Toggle mode fit image into window"));
  mActionMagnifyFit->setIcon(QIcon(":/images/viewmagfit"));
  connect(mActionMagnifyFit, SIGNAL(triggered()), this, SLOT(magnifyFit()));
  mActionMagnifyFit->setCheckable(true);
  mActionMagnifyFit->setChecked(false);

  mActionMagnifyOriginal = new QAction(tr("Original"), this);
  mActionMagnifyOriginal->setToolTip(tr("Original image size"));
  mActionMagnifyOriginal->setIcon(QIcon(":/images/viewmag1"));
  connect(mActionMagnifyOriginal, SIGNAL(triggered()), this, SLOT(magnifyOriginal()));

  mActionFullScreen = new QAction(tr("Full screen"), this);
  mActionFullScreen->setToolTip(tr("Put window full screen"));
  mActionFullScreen->setIcon(QIcon(":/images/fullscreen"));
  connect(mActionFullScreen, SIGNAL(triggered()), this, SLOT(goFullScreen()));
  mActionFullScreen->setCheckable(true);
  mActionFullScreen->setChecked(false);
}

//-------------------------------------------------------------------------------

void MonitorWindow::createLayout()
{
  QLabel* lModuleIcon = new QLabel();
  QLabel* lModuleSlotIcon = new QLabel();
  QLabel* lModuleLabel = new QLabel(tr("Module:"));
  QLabel* lModuleSlotLabel = new QLabel(tr("Slot:"));
  mScrollArea = new QScrollArea();
  mModuleComboBox = new QComboBox();
  mModuleSlotComboBox = new QComboBox();
  mZoomSlider = new QSlider(Qt::Horizontal);
  mImage = new QLabel();
  mToolBar = new QToolBar();

  mZoomSlider->setTickPosition(QSlider::NoTicks);
  mZoomSlider->setRange(-25, 25);
  mZoomSlider->setValue(0);
  connect(mZoomSlider, SIGNAL(valueChanged (int)), this, SLOT(zoomChange(int)));

  mImage->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  mImage->setScaledContents(true);

  connect(mModuleComboBox, SIGNAL(activated (const QString&)), this, SLOT(activateModule(const QString&)));
  connect(mModuleSlotComboBox, SIGNAL(activated (const QString&)), this, SLOT(activateModuleSlot(const QString&)));

  lModuleIcon->setPixmap(QIcon(":/images/module").pixmap(QSize(16,16)));
  lModuleSlotIcon->setPixmap(QIcon(":/images/slot-connect").pixmap(QSize(16,16)));

  mScrollArea->setWidget(mImage);
  mScrollArea->setBackgroundRole(QPalette::Dark);
  mScrollArea->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  mScrollArea->setWidgetResizable(false);

  mMainLayout = new QVBoxLayout();
  mModuleLayout = new QHBoxLayout();

  mModuleLayout->addWidget(lModuleIcon, 0);
  mModuleLayout->addWidget(lModuleLabel, 0);
  mModuleLayout->addWidget(mModuleComboBox, 1);
  mModuleLayout->addSpacing(10);
  mModuleLayout->addWidget(lModuleSlotIcon, 0);
  mModuleLayout->addWidget(lModuleSlotLabel, 0);
  mModuleLayout->addWidget(mModuleSlotComboBox, 1);

  mToolBar->setIconSize(QSize(16,16));
  mToolBar->setMovable(false);
  mToolBar->setFloatable(false);
  mToolBar->setOrientation(Qt::Horizontal);
  mToolBar->addAction(mActionFullScreen);
  mToolBar->addSeparator();
  mToolBar->addAction(mActionMagnifyOriginal);
  mToolBar->addAction(mActionMagnifyFit);
  mToolBar->addSeparator();
  mToolBar->addAction(mActionMagnifyMinus);
  mToolBar->addWidget(mZoomSlider);
  mToolBar->addAction(mActionMagnifyPlus);

  mMainLayout->addLayout(mModuleLayout, 1);
  mMainLayout->addWidget(mScrollArea, 1);
  mMainLayout->addWidget(mToolBar, 0, Qt::AlignLeft);

  mMainLayout->setContentsMargins(4,4,4,4);

  setLayout(mMainLayout);

}

//-------------------------------------------------------------------------------

bool MonitorWindow::updateImageState(ImageState inImageState)
{
  if(mImageState==inImageState)
    return false;

  mImageState = inImageState;

  if(inImageState==eImageStateValid)
  {
    mImage->setText("");
    mActionMagnifyFit->setEnabled(true);
    mActionMagnifyOriginal->setEnabled(true);
    if(!mActionMagnifyFit->isChecked())
    {
      mZoomSlider->setEnabled(true);
      mActionMagnifyMinus->setEnabled(true);
      mActionMagnifyPlus->setEnabled(true);
    }
  }
  else if(inImageState==eImageStateNoModule)
  {
    mZoomSlider->setEnabled(false);
    mActionMagnifyMinus->setEnabled(false);
    mActionMagnifyPlus->setEnabled(false);
    mActionMagnifyFit->setEnabled(false);
    mActionMagnifyOriginal->setEnabled(false);

    mImage->setPixmap(QPixmap());
    mImage->setText(tr("Not attached to a module"));
    mImage->adjustSize();
  }
  else if(inImageState==eImageStateNoSlot)
  {
    mZoomSlider->setEnabled(false);
    mActionMagnifyMinus->setEnabled(false);
    mActionMagnifyPlus->setEnabled(false);
    mActionMagnifyFit->setEnabled(false);
    mActionMagnifyOriginal->setEnabled(false);

    mImage->setPixmap(QPixmap());
    mImage->setText(tr("Not monitoring a module slot"));
    mImage->adjustSize();
  }
  else if(inImageState==eImageStateNotInitialized)
  {
    mZoomSlider->setEnabled(false);
    mActionMagnifyMinus->setEnabled(false);
    mActionMagnifyPlus->setEnabled(false);
    mActionMagnifyFit->setEnabled(false);
    mActionMagnifyOriginal->setEnabled(false);

    mImage->setPixmap(QPixmap());
    mImage->setText(tr("Image is not yet initialized in the module"));
    mImage->adjustSize();
  }
  else if(inImageState==eImageStateUnsupportedFormat)
  {
    mZoomSlider->setEnabled(false);
    mActionMagnifyMinus->setEnabled(false);
    mActionMagnifyPlus->setEnabled(false);
    mActionMagnifyFit->setEnabled(false);
    mActionMagnifyOriginal->setEnabled(false);

    mImage->setPixmap(QPixmap());
    mImage->setText(tr("Unsupported image format"));
    mImage->adjustSize();
  }

  return true;
}

//-------------------------------------------------------------------------------
