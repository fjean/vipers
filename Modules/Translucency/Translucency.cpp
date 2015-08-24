/*
 *  Video and Image Processing Environment for Real-time Systems (VIPERS)
 *	Testing Module
 *  Copyright (C) 2009 by Frederic Jean
 *
 *  VIPERS is a free library: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License,
 *  or (at your option) any later version.
 *
 *  VIPERS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with VIPERS.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Contact:
 *  Computer Vision and Systems Laboratory
 *  Departement of Electrical and Computer Engineering
 *  Universite Laval, Quebec, Canada, G1V 0A6
 *  http://vision.gel.ulaval.ca
 *
 */

/*!
 * \file Translucency.cpp
 * \brief TranslucencyModule class functions definition.
 * \author Frederic Jean
 * $Revision: 333 $
 * $Date: 2011-05-20 10:48:09 -0700 (Fri, 20 May 2011) $
 */

#include "Translucency.hpp"
#include "TranslucencyConfig.hpp"

#include <sstream>

#define VIPERS_UTILS_OPENCV
#include <ImageUtils.hpp>

using namespace VIPERS;
using namespace std;

#define MODULE_NAME "translucency"
#define MODULE_DISPLAY_NAME "Translucency"

#define INPUT_SLOT_NAME_BGIMAGE "input-background-image"
#define INPUT_SLOT_DISPLAYNAME_BGIMAGE "Input background image"
#define INPUT_SLOT_NAME_TRANSIMAGE "input-translucency-image"
#define INPUT_SLOT_DISPLAYNAME_TRANSIMAGE "Input translucency image"
#define INPUT_SLOT_NAME_ALPHAMASK "input-alpha-mask"
#define INPUT_SLOT_DISPLAYNAME_ALPHAMASK "Input alpha mask"

#define OUTPUT_SLOT_NAME_IMAGE "output-image"
#define OUTPUT_SLOT_DISPLAYNAME_IMAGE "Output image"

#define PARAMETER_NAME_MODE "mode"
#define PARAMETER_NAME_ALPHA "alpha"
#define PARAMETER_NAME_INVERT "invert"

/*! TODO:
*/
TranslucencyModule::TranslucencyModule()
	:Module(MODULE_NAME, MODULE_DISPLAY_NAME, TRANSLUCENCYMODULE_VERSION),
	mParamMode(PARAMETER_NAME_MODE, Variable::eVariableTypeString, "Translucency mode", "The type of translucency to perform", false),
	mParamAlpha(PARAMETER_NAME_ALPHA, Variable::eVariableTypeDouble, "Translucency alpha", "Percentage of translucency (0=opaque, 1=invisible)", true),
	mParamInvert(PARAMETER_NAME_INVERT, Variable::eVariableTypeBool, "Invert mask", "Invert plain or alpha mask", true)
{
	mShortDescription = "Module for embedding a color image in another one with translucency effect";
	mLongDescription = "This module embeds an image into another one using an alpha mask (translucency)";

	ValueSet lModeName;
	lModeName.insert(Value("image", "Whole Image", "Whole image is made translucent using the specified alpha value"));
	lModeName.insert(Value("plainmask", "Plain Mask", "Only the image pixels where the provided mask is non zero are made translucent using the specified alpha value"));
	lModeName.insert(Value("alphamask", "Alpha Mask", "Each pixel of the mask specify the alpha value to apply to the corresponding image pixel"));
	mParamMode.setPossibleValues(lModeName);

  NameSet lDependentParameterSet;
  lDependentParameterSet.insert(PARAMETER_NAME_ALPHA);
  lDependentParameterSet.insert(PARAMETER_NAME_INVERT);
  mParamMode.setDependentParameterNameSet(lDependentParameterSet);

	mParamMode.setValueStr("plainmask");
	mParamAlpha.setValue(0.5);
	mParamAlpha.setMinValue("0");
	mParamAlpha.setMaxValue("1");
	mParamInvert.setValue(false);

	newParameter(mParamMode);
	newParameter(mParamAlpha);
	newParameter(mParamInvert);

	mInputSlotBackgroundColorImage  = newSlot(new ModuleSlot(this, INPUT_SLOT_NAME_BGIMAGE, INPUT_SLOT_DISPLAYNAME_BGIMAGE, "Background image on which embedding with be performed"));
	mInputSlotTranslucencyColorImage  = newSlot(new ModuleSlot(this, INPUT_SLOT_NAME_TRANSIMAGE, INPUT_SLOT_DISPLAYNAME_TRANSIMAGE, "Image that is going to be made translucent"));
	mInputSlotTranslucencyMaskImage  = newSlot(new ModuleSlot(this, INPUT_SLOT_NAME_ALPHAMASK, INPUT_SLOT_DISPLAYNAME_ALPHAMASK, "Alpha mask used to make translucent image"));

	mOutputSlot = newSlot(new ModuleSlot(this, OUTPUT_SLOT_NAME_IMAGE, OUTPUT_SLOT_DISPLAYNAME_IMAGE, "Output image", &mOutputFrame));

	mOutputFrameIpl = NULL;
	mOutputFrame = NULL;
	mTmpFrame1 = NULL;
	mTmpFrame2 = NULL;
	mTmpFrame3 = NULL;
	mTmpFrame4 = NULL;
	mTmpFrame5 = NULL;
}

/*! TODO:
*/
TranslucencyModule::~TranslucencyModule()
{
	if(mOutputFrameIpl)
	{
		cvReleaseImage(&mOutputFrameIpl);
		delete mOutputFrame;
	}
	if(mTmpFrame1)
		cvReleaseImage(&mTmpFrame1);
	if(mTmpFrame2)
		cvReleaseImage(&mTmpFrame2);
	if(mTmpFrame3)
		cvReleaseImage(&mTmpFrame3);
	if(mTmpFrame4)
		cvReleaseImage(&mTmpFrame4);
	if(mTmpFrame5)
		cvReleaseImage(&mTmpFrame5);
}

/*! TODO:
*/
void TranslucencyModule::initFunction()
{
	try
	{
		processFunction(0);
	}
	catch(...)
	{
		throw;
	}
}

/*! TODO:
*/
void TranslucencyModule::startFunction()
{

}

/*! TODO:
*/
void TranslucencyModule::pauseFunction()
{

}

/*! TODO:
*/
void TranslucencyModule::processFunction(unsigned int inFrameNumber)
{
  const Image* lBackgroundColorImage;
  const Image* lTranslucencyColorImage;
  const Image* lTranslucencyMaskImage;

	IplImage* lBackgroundColorImageIpl;
	IplImage* lTranslucencyColorImageIpl;
	IplImage* lTranslucencyMaskImageIpl;

	unsigned int lWidth;
	unsigned int lHeight;
	int lChannels;
	int lDepth;
	string lMode;
	double lAlpha;
	double lScale;
	double lShift;

	lockParameters();
	mParamMode = getLockedParameter(PARAMETER_NAME_MODE);
	mParamAlpha = getLockedParameter(PARAMETER_NAME_ALPHA);
	mParamInvert = getLockedParameter(PARAMETER_NAME_INVERT);
	unlockParameters();

	lMode = mParamMode.toString();
	lAlpha = mParamAlpha.toDouble();

	if(!mInputSlotBackgroundColorImage->isConnected())
		throw(Exception(Exception::eCodeUseModule, mInputSlotBackgroundColorImage->getFullName().c_str() + string(" is not connected to an output slot") ));
	if(!mInputSlotTranslucencyColorImage->isConnected())
		throw(Exception(Exception::eCodeUseModule, mInputSlotTranslucencyColorImage->getFullName().c_str() + string(" is not connected to an output slot") ));
	if(lMode!="image" && !mInputSlotTranslucencyMaskImage->isConnected())
		throw(Exception(Exception::eCodeUseModule, mInputSlotTranslucencyMaskImage->getFullName().c_str() + string(" is not connected to an output slot. It is needed in mode \"") + lMode.c_str() + string("\".") ));

	mInputSlotBackgroundColorImage->lock();
	mInputSlotTranslucencyColorImage->lock();
	mInputSlotTranslucencyMaskImage->lock();
	mOutputSlot->lock();

	lBackgroundColorImage = mInputSlotBackgroundColorImage->getImage();
	lTranslucencyColorImage = mInputSlotTranslucencyColorImage->getImage();
	lTranslucencyMaskImage = mInputSlotTranslucencyMaskImage->getImage();

	// Check validity of image
	if(!lBackgroundColorImage)
	{
		mOutputSlot->unlock();
		mInputSlotTranslucencyMaskImage->unlock();
		mInputSlotTranslucencyColorImage->unlock();
		mInputSlotBackgroundColorImage->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotBackgroundColorImage->getFullName().c_str() + string(" does not have a valid image pointer (NULL)") ));
	}
	if(!lTranslucencyColorImage)
	{
		mOutputSlot->unlock();
		mInputSlotTranslucencyMaskImage->unlock();
		mInputSlotTranslucencyColorImage->unlock();
		mInputSlotBackgroundColorImage->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotTranslucencyColorImage->getFullName().c_str() + string(" does not have a valid image pointer (NULL)") ));
	}
	if(!lTranslucencyMaskImage)
	{
		mOutputSlot->unlock();
		mInputSlotTranslucencyMaskImage->unlock();
		mInputSlotTranslucencyColorImage->unlock();
		mInputSlotBackgroundColorImage->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotTranslucencyMaskImage->getFullName().c_str() + string(" does not have a valid image pointer (NULL)") ));
	}

	lWidth = lBackgroundColorImage->getWidth();
	lHeight = lBackgroundColorImage->getHeight();
	lChannels = lBackgroundColorImage->getNbChannels();
	lDepth = convertDepthToIpl(lBackgroundColorImage->getDepth());

	//Verify image depth and number of channels
	if(lWidth!=lTranslucencyColorImage->getWidth() || lHeight!=lTranslucencyColorImage->getHeight())
	{
		mOutputSlot->unlock();
		mInputSlotTranslucencyMaskImage->unlock();
		mInputSlotTranslucencyColorImage->unlock();
		mInputSlotBackgroundColorImage->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotBackgroundColorImage->getFullName().c_str() + string(" and ") + mInputSlotTranslucencyColorImage->getFullName().c_str() + string(" must have the same dimension") ));
	}

	//Verify image depth and number of channels
	if(lDepth!=convertDepthToIpl(lTranslucencyColorImage->getDepth()))
	{
		mOutputSlot->unlock();
		mInputSlotTranslucencyMaskImage->unlock();
		mInputSlotTranslucencyColorImage->unlock();
		mInputSlotBackgroundColorImage->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotBackgroundColorImage->getFullName().c_str() + string(" and ") + mInputSlotTranslucencyColorImage->getFullName().c_str() + string(" must have the same depth") ));
	}

	//Verify image depth and number of channels
	if(lChannels!=lTranslucencyColorImage->getNbChannels())
	{
		mOutputSlot->unlock();
		mInputSlotTranslucencyMaskImage->unlock();
		mInputSlotTranslucencyColorImage->unlock();
		mInputSlotBackgroundColorImage->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotBackgroundColorImage->getFullName().c_str() + string(" and ") + mInputSlotTranslucencyColorImage->getFullName().c_str() + string(" must have the same number of channels") ));
	}

	//Verify image depth and number of channels
	if(lWidth!=lTranslucencyMaskImage->getWidth() || lHeight!=lTranslucencyMaskImage->getHeight())
	{
		mOutputSlot->unlock();
		mInputSlotTranslucencyMaskImage->unlock();
		mInputSlotTranslucencyColorImage->unlock();
		mInputSlotBackgroundColorImage->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotBackgroundColorImage->getFullName().c_str() + string(" and ") + mInputSlotTranslucencyMaskImage->getFullName().c_str() + string(" must have the same dimension") ));
	}

	if(lTranslucencyMaskImage->getNbChannels()!=1 || convertDepthToIpl(lTranslucencyMaskImage->getDepth())!=IPL_DEPTH_8U)
	{
		mOutputSlot->unlock();
		mInputSlotTranslucencyMaskImage->unlock();
		mInputSlotTranslucencyColorImage->unlock();
		mInputSlotBackgroundColorImage->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotTranslucencyMaskImage->getFullName().c_str() + string(" must be a unsigned 8 bits 1 channel image") ));
	}

	setToIplImage(*lBackgroundColorImage, &lBackgroundColorImageIpl);
	setToIplImage(*lTranslucencyColorImage, &lTranslucencyColorImageIpl);
	setToIplImage(*lTranslucencyMaskImage, &lTranslucencyMaskImageIpl);

	// Create output image
	if(!mOutputFrameIpl)
	{
		mOutputFrameIpl = cvCreateImage(cvSize(lWidth, lHeight), IPL_DEPTH_8U, 3);
		mOutputFrame = new Image(false);
		setFromIplImage(mOutputFrameIpl, *mOutputFrame);
		mOutputFrame->setModel(Image::eModelRGB);
		if(lMode!="image")
		{
			if(mTmpFrame1)
				cvReleaseImage(&mTmpFrame1);
			mTmpFrame1 = cvCreateImage(cvSize(lWidth, lHeight), IPL_DEPTH_8U, 3);
			if(mTmpFrame2)
				cvReleaseImage(&mTmpFrame2);
			mTmpFrame2 = cvCreateImage(cvSize(lWidth, lHeight), IPL_DEPTH_8U, 3);
			if(lMode=="alphamask")
			{
				if(mTmpFrame3)
					cvReleaseImage(&mTmpFrame3);
				mTmpFrame3 = cvCreateImage(cvSize(lWidth, lHeight), IPL_DEPTH_32F, 3);
				if(mTmpFrame4)
					cvReleaseImage(&mTmpFrame4);
				mTmpFrame4 = cvCreateImage(cvSize(lWidth, lHeight), IPL_DEPTH_32F, 3);
				if(mTmpFrame5)
					cvReleaseImage(&mTmpFrame5);
				mTmpFrame5 = cvCreateImage(cvSize(lWidth, lHeight), IPL_DEPTH_32F, 3);
			}
		}
	}
	else if(mOutputFrameIpl->width!=lWidth || mOutputFrameIpl->height!=lHeight || mOutputFrameIpl->nChannels!=lChannels || mOutputFrameIpl->depth!=lDepth)
	{
		cvReleaseImage(&mOutputFrameIpl);
		delete mOutputFrame;
		mOutputFrameIpl = cvCreateImage(cvSize(lWidth, lHeight), IPL_DEPTH_8U, 3);
    mOutputFrame = new Image(false);
    setFromIplImage(mOutputFrameIpl, *mOutputFrame);
    mOutputFrame->setModel(Image::eModelRGB);

		if(lMode!="image")
		{
			if(mTmpFrame1)
				cvReleaseImage(&mTmpFrame1);
			mTmpFrame1 = cvCreateImage(cvSize(lWidth, lHeight), IPL_DEPTH_8U, 3);
			if(mTmpFrame2)
				cvReleaseImage(&mTmpFrame2);
			mTmpFrame2 = cvCreateImage(cvSize(lWidth, lHeight), IPL_DEPTH_8U, 3);
			if(lMode=="alphamask")
			{
				if(mTmpFrame3)
					cvReleaseImage(&mTmpFrame3);
				mTmpFrame3 = cvCreateImage(cvSize(lWidth, lHeight), IPL_DEPTH_32F, 3);
				if(mTmpFrame4)
					cvReleaseImage(&mTmpFrame4);
				mTmpFrame4 = cvCreateImage(cvSize(lWidth, lHeight), IPL_DEPTH_32F, 3);
				if(mTmpFrame5)
					cvReleaseImage(&mTmpFrame5);
				mTmpFrame5 = cvCreateImage(cvSize(lWidth, lHeight), IPL_DEPTH_32F, 3);
			}
		}
	}

	if(lMode=="image")
	{
		cvAddWeighted(lBackgroundColorImageIpl, lAlpha, lTranslucencyColorImageIpl, 1-lAlpha, 0.0, mOutputFrameIpl);
	}
	else if(lMode=="plainmask")
	{
		cvCopy(lBackgroundColorImageIpl, mOutputFrameIpl);
		cvConvertScale(lBackgroundColorImageIpl, mTmpFrame1, lAlpha);
		cvConvertScale(lTranslucencyColorImageIpl, mTmpFrame2, 1-lAlpha);
		cvAdd(mTmpFrame1, mTmpFrame2, mOutputFrameIpl, lTranslucencyMaskImageIpl);
	}
	else if(lMode=="alphamask")
	{
		if(mParamInvert.toBool())
		{
			lScale = 1.0/255.0;
			lShift = 0;
		}
		else
		{
			lScale = -1.0/255.0;
			lShift = 1.0;
		}

		cvConvertScale(lBackgroundColorImageIpl, mTmpFrame3);
		cvConvertScale(lTranslucencyColorImageIpl, mTmpFrame4);

		cvMerge(lTranslucencyMaskImageIpl, lTranslucencyMaskImageIpl, lTranslucencyMaskImageIpl, NULL, mTmpFrame1);
		cvConvertScale(mTmpFrame1, mTmpFrame5, lScale, lShift);
		cvMul(mTmpFrame3, mTmpFrame5, mTmpFrame3);
		cvConvertScale(mTmpFrame5, mTmpFrame5, -1, 1);
		cvMul(mTmpFrame4, mTmpFrame5, mTmpFrame4);

		cvAdd(mTmpFrame3, mTmpFrame4, mTmpFrame5);
		cvConvertScale(mTmpFrame5, mOutputFrameIpl);
	}
	else
	{
	  cvReleaseImageHeader(&lBackgroundColorImageIpl);
	  cvReleaseImageHeader(&lTranslucencyColorImageIpl);
	  cvReleaseImageHeader(&lTranslucencyMaskImageIpl);

		mOutputSlot->unlock();
		mInputSlotTranslucencyMaskImage->unlock();
		mInputSlotTranslucencyColorImage->unlock();
		mInputSlotBackgroundColorImage->unlock();
		throw(Exception(Exception::eCodeUseModule, string("Module \"") + getLabel().c_str() + string("\" does not a a mode named \"") + lMode.c_str() + string("\"")));
	}

  cvReleaseImageHeader(&lBackgroundColorImageIpl);
  cvReleaseImageHeader(&lTranslucencyColorImageIpl);
  cvReleaseImageHeader(&lTranslucencyMaskImageIpl);

	mOutputSlot->unlock();
	mInputSlotTranslucencyMaskImage->unlock();
	mInputSlotTranslucencyColorImage->unlock();
	mInputSlotBackgroundColorImage->unlock();

}

/*! TODO:
*/
void TranslucencyModule::stopFunction()
{

}

/*! TODO:
*/
void TranslucencyModule::resetFunction()
{
	if(mOutputFrameIpl)
	{
		mOutputSlot->lock();
		cvReleaseImage(&mOutputFrameIpl);
		delete mOutputFrame;
		mOutputFrameIpl = NULL;
		mOutputFrame = NULL;
		mOutputSlot->unlock();
	}
	if(mTmpFrame1)
	{
		cvReleaseImage(&mTmpFrame1);
		mTmpFrame1 = NULL;
	}
	if(mTmpFrame2)
	{
		cvReleaseImage(&mTmpFrame2);
		mTmpFrame2 = NULL;
	}
	if(mTmpFrame3)
	{
		cvReleaseImage(&mTmpFrame3);
		mTmpFrame3 = NULL;
	}
	if(mTmpFrame4)
	{
		cvReleaseImage(&mTmpFrame4);
		mTmpFrame4 = NULL;
	}
	if(mTmpFrame5)
	{
		cvReleaseImage(&mTmpFrame5);
		mTmpFrame5 = NULL;
	}
}

/*!
*/
void TranslucencyModule::updateParametersFunction()
{
  lockParameters();

  string lMode = getLockedParameter(PARAMETER_NAME_MODE).toString();

  mParamMode.setValueStr(lMode.c_str());

  if(lMode=="image")
  {
    mParamAlpha.setEnabled(true);
    mParamInvert.setEnabled(false);
  }
  else if(lMode=="plainmask")
  {
    mParamAlpha.setEnabled(true);
    mParamInvert.setEnabled(true);
  }
  else if(lMode=="alphamask")
  {
    mParamAlpha.setEnabled(false);
    mParamInvert.setEnabled(true);
  }

  setLockedParameter(mParamAlpha);
  setLockedParameter(mParamInvert);

  unlockParameters();
}

/*!
*/
string TranslucencyModule::verifyParameterFunction(const Parameter& inParameter) const throw()
{
	string lResult = "";

	return lResult;
}

//Create entry points and modules functions
VIPERS_MODULE_ENTRY_POINT
VIPERS_MODULE_FUNCTIONS(TranslucencyModule)
