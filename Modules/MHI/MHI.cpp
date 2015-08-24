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
 * \file MHI.cpp
 * \brief MHIModule class functions definition.
 * \author Frederic Jean
 * $Revision: 333 $
 * $Date: 2011-05-20 10:48:09 -0700 (Fri, 20 May 2011) $
 */

#include "MHI.hpp"
#include "MHIConfig.hpp"

#define VIPERS_UTILS_OPENCV
#include <ImageUtils.hpp>

#include <sstream>

using namespace VIPERS;
using namespace std;

#define MODULE_NAME "mhi"
#define MODULE_DISPLAY_NAME "Motion History Image"

#define INPUT_SLOT_NAME_MASK "input-mask"
#define INPUT_SLOT_DISPLAYNAME_MASK "Input mask"
#define OUTPUT_SLOT_NAME_MHI "output-mhi"
#define OUTPUT_SLOT_DISPLAYNAME_MHI "Output MHI"
#define OUTPUT_SLOT_NAME_MHI_GRAY "output-mhi-gray"
#define OUTPUT_SLOT_DISPLAYNAME_MHI_GRAY "Output MHI gray"

#define PARAMETER_NAME_UNITS "units"
#define PARAMETER_NAME_DURATION "duration"

/*! TODO:
*/
MHIModule::MHIModule()
	:Module(MODULE_NAME, MODULE_DISPLAY_NAME, MHIMODULE_VERSION),
	mParamUnits(PARAMETER_NAME_UNITS, Variable::eVariableTypeString, "Duration units", "Units are either seconds or frames", false),
	mParamDuration(PARAMETER_NAME_DURATION, Variable::eVariableTypeDouble, "Duration", "Duration parameter for MHI, in seconds or in frames", true)
{
	mShortDescription = "Module for computing motion history image";
	mLongDescription = "This module computes a motion history image from binary images";

	ValueSet lUnitsValues;
	lUnitsValues.insert(Value("frames", "Frames", "Elapsed time is counted in frames"));
	lUnitsValues.insert(Value("seconds", "Seconds", "Elapsed time is counted in seconds"));

	mParamUnits.setPossibleValues(lUnitsValues);

	mParamUnits.setValueStr("frames");
	mParamDuration.setValue(100);
	mParamDuration.setMinValue("0");
	mParamDuration.setMaxValue("10000");

	newParameter(mParamUnits);
	newParameter(mParamDuration);

	mInputSlotMask = newSlot(new ModuleSlot(this, INPUT_SLOT_NAME_MASK, INPUT_SLOT_DISPLAYNAME_MASK, "Input mask image used to compute MHI"));
	mOutputSlotMHI = newSlot(new ModuleSlot(this, OUTPUT_SLOT_NAME_MHI, OUTPUT_SLOT_DISPLAYNAME_MHI, "Output MHI image (floating point)", &mOutputMHI));
	mOutputSlotMHIGray = newSlot(new ModuleSlot(this, OUTPUT_SLOT_NAME_MHI_GRAY, OUTPUT_SLOT_DISPLAYNAME_MHI_GRAY, "Output MHI image (gray scale image)", &mOutputMHIGray));

  mOutputMHIIpl = NULL;
  mOutputMHIGrayIpl = NULL;
	mOutputMHI = NULL;
	mOutputMHIGray = NULL;
	mEllapsedTime = 0;
}

/*! TODO:
*/
MHIModule::~MHIModule()
{
	if(mOutputMHIIpl)
	{
		cvReleaseImage(&mOutputMHIIpl);
		delete mOutputMHI;
	}
	if(mOutputMHIGrayIpl)
	{
		cvReleaseImage(&mOutputMHIGrayIpl);
		delete mOutputMHIGray;
	}
}

/*! TODO:
*/
void MHIModule::initFunction()
{
	try
	{
		mEllapsedTime = 0;
		mTimer.reset();
		processFunction(0);
	}
	catch(...)
	{
		throw;
	}
}

/*! TODO:
*/
void MHIModule::startFunction()
{
	mTimer.reset();
	mEllapsedTime = 0;
}

/*! TODO:
*/
void MHIModule::pauseFunction()
{

}

/*! TODO:
*/
void MHIModule::processFunction(unsigned int inFrameNumber)
{
	const Image* lMaskImage;
	IplImage* lMaskImageIpl;
	string lUnits;

	if(!mInputSlotMask->isConnected())
		throw(Exception(Exception::eCodeUseModule, mInputSlotMask->getFullName().c_str() + string(" is not connected to an output slot") ));

	lockParameters();
	lUnits = getLockedParameter(PARAMETER_NAME_UNITS).toString();
	mParamDuration = getLockedParameter(PARAMETER_NAME_DURATION);
	unlockParameters();

	mInputSlotMask->lock();

	lMaskImage = mInputSlotMask->getImage();

	if(lMaskImage->getNbChannels()!=Image::eChannel1)
	{
		mInputSlotMask->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotMask->getFullName().c_str() + string(" must be a 1 channel image mask") ));
	}

	mOutputSlotMHI->lock();
	mOutputSlotMHIGray->lock();

	setToIplImage(*lMaskImage, &lMaskImageIpl);

	if(!mOutputMHIIpl)
	{
		mOutputMHIIpl = cvCreateImage(cvSize(lMaskImageIpl->width, lMaskImageIpl->height), IPL_DEPTH_32F, 1);
		mOutputMHI = new Image(false);
		setFromIplImage(mOutputMHIIpl, *mOutputMHI);
		mOutputMHIGrayIpl = cvCreateImage(cvSize(lMaskImageIpl->width, lMaskImageIpl->height), IPL_DEPTH_8U, 1);
		mOutputMHIGray = new Image(false);
		setFromIplImage(mOutputMHIGrayIpl, *mOutputMHIGray);
	}
	else if(mOutputMHIIpl->width!=lMaskImageIpl->width || mOutputMHIIpl->height!=lMaskImageIpl->height)
	{
		cvReleaseImage(&mOutputMHIIpl);
		delete mOutputMHI;
		cvReleaseImage(&mOutputMHIGrayIpl);
		delete mOutputMHIGray;
		mOutputMHIIpl = cvCreateImage(cvSize(lMaskImageIpl->width, lMaskImageIpl->height), IPL_DEPTH_32F, 1);
    mOutputMHI = new Image(false);
    setFromIplImage(mOutputMHIIpl, *mOutputMHI);
		mOutputMHIGrayIpl = cvCreateImage(cvSize(lMaskImageIpl->width, lMaskImageIpl->height), IPL_DEPTH_8U, 1);
    mOutputMHIGray = new Image(false);
    setFromIplImage(mOutputMHIGrayIpl, *mOutputMHIGray);
		mEllapsedTime = 0;
		mTimer.reset();
	}

	if(lUnits!=mParamUnits.toString())
	{
		mParamUnits.setValueStr(lUnits);
		cvSetZero(mOutputMHIIpl);
		cvSetZero(mOutputMHIGrayIpl);
		mEllapsedTime = 0;
		mTimer.reset();
	}

	if(lUnits=="frames")
	{
		mEllapsedTime++;
		cvUpdateMotionHistory(lMaskImageIpl, mOutputMHIIpl, inFrameNumber, mParamDuration.toDouble());
	}
	else
	{
		mEllapsedTime = mTimer.getValue();
		cvUpdateMotionHistory(lMaskImageIpl, mOutputMHIIpl, mEllapsedTime, mParamDuration.toDouble());
	}

	if(mOutputSlotMHIGray->getUseCount() && mEllapsedTime>mParamDuration.toDouble())
		cvConvertScale(mOutputMHIIpl, mOutputMHIGrayIpl, 255.0/mParamDuration.toDouble(), -(inFrameNumber-mParamDuration.toDouble())*( 255.0/mParamDuration.toDouble()) );

	cvReleaseImageHeader(&lMaskImageIpl);

	mOutputSlotMHIGray->unlock();
	mOutputSlotMHI->unlock();
	mInputSlotMask->unlock();
}

/*! TODO:
*/
void MHIModule::stopFunction()
{
	mEllapsedTime = 0;
	mTimer.reset();
}

/*! TODO:
*/
void MHIModule::resetFunction()
{
	mEllapsedTime = 0;
	mTimer.reset();
	if(mOutputMHIIpl)
	{
		mOutputSlotMHI->lock();
		cvReleaseImage(&mOutputMHIIpl);
		delete mOutputMHI;
		mOutputMHIIpl = NULL;
		mOutputMHI = NULL;
		mOutputSlotMHI->unlock();
	}
	if(mOutputMHIGrayIpl)
	{
		mOutputSlotMHIGray->lock();
		cvReleaseImage(&mOutputMHIGrayIpl);
		delete mOutputMHIGray;
		mOutputMHIGrayIpl = NULL;
		mOutputMHIGray = NULL;
		mOutputSlotMHIGray->unlock();
	}
}

/*!
*/
void MHIModule::updateParametersFunction()
{

}

/*!
*/
string MHIModule::verifyParameterFunction(const Parameter& inParameter) const throw()
{
	string lResult = "";

	return lResult;
}

//Create entry points and modules functions
VIPERS_MODULE_ENTRY_POINT
VIPERS_MODULE_FUNCTIONS(MHIModule)
