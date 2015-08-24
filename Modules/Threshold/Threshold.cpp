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
 * \file Threshold.cpp
 * \brief ThresholdModule class functions definition.
 * \author Frederic Jean
 * $Revision: 337 $
 * $Date: 2011-06-14 09:44:06 -0700 (Tue, 14 Jun 2011) $
 */

#include "Threshold.hpp"
#include "ThresholdConfig.hpp"

#include <iostream>

#define VIPERS_UTILS_OPENCV
#include <ImageUtils.hpp>

using namespace VIPERS;
using namespace std;

#define MODULE_NAME "threshold"
#define MODULE_DISPLAY_NAME "Threshold"

#define INPUT_SLOT_NAME_IMAGE "input-image"
#define INPUT_SLOT_DISPLAYNAME_IMAGE "Input image"

#define OUTPUT_SLOT_NAME_RESIZED_IMAGE "output-thresholded"
#define OUTPUT_SLOT_DISPLAYNAME_RESIZED_IMAGE "Thresholded image"

#define PARAMETER_NAME_THRESHOLD_VALUE "threshold-value"
#define PARAMETER_NAME_THRESHOLD_TYPE "threshold-type"

/*! TODO:
*/
ThresholdModule::ThresholdModule()
	:Module(MODULE_NAME, MODULE_DISPLAY_NAME, THRESHOLDMODULE_VERSION),
	mParamThresholdType(PARAMETER_NAME_THRESHOLD_TYPE, Variable::eVariableTypeString, "Threshold type", "Type of thresholding", true),
	mParamThresholdValue(PARAMETER_NAME_THRESHOLD_VALUE, Variable::eVariableTypeInt, "Threshold value", "Threshold value", true)
{
	mShortDescription = "Module for applying a threshold on an image";
	mLongDescription = "This module applies a threshold on a gray scale image";

	ValueSet lTmpTypeName;
	lTmpTypeName.insert(Value("greater", "Greater than", "Pixel value greater than threshold value"));
	lTmpTypeName.insert(Value("less", "Less than", "Pixel value less than threshold value"));

	mParamThresholdType.setPossibleValues(lTmpTypeName);

	mThresholdTypeMap["greater"] = CV_THRESH_BINARY;
	mThresholdTypeMap["less"] = CV_THRESH_BINARY_INV;

	mParamThresholdType.setValueStr("greater");

	mParamThresholdValue.setValue(128);
	mParamThresholdValue.setMinValue("0");
	mParamThresholdValue.setMaxValue("255");	
	
	newParameter(mParamThresholdValue);
	newParameter(mParamThresholdType);

	mInputSlot = newSlot(new ModuleSlot(this, INPUT_SLOT_NAME_IMAGE, INPUT_SLOT_DISPLAYNAME_IMAGE, "Image to be thresholded"));
	mOutputSlot = newSlot(new ModuleSlot(this, OUTPUT_SLOT_NAME_RESIZED_IMAGE, OUTPUT_SLOT_DISPLAYNAME_RESIZED_IMAGE, "Thresholded image", &mOutputImage));

	mOutputImageIpl = NULL;
	mOutputImage = NULL;
}

/*! TODO:
*/
ThresholdModule::~ThresholdModule()
{
	if(mOutputImageIpl)
	{
		cvReleaseImage(&mOutputImageIpl);
		delete mOutputImage;
	}
}

/*! TODO:
*/
void ThresholdModule::initFunction()
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
void ThresholdModule::startFunction()
{

}

/*! TODO:
*/
void ThresholdModule::pauseFunction()
{

}

/*! TODO:
*/
void ThresholdModule::processFunction(unsigned int inFrameNumber)
{
	const Image* lTmpImage = NULL;
	IplImage* lTmpImageIpl = NULL;

	mInputSlot->lock();
	
	if(!mInputSlot->isConnected())
	{
		mInputSlot->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlot->getFullName().c_str() + string(" is not connected to an output slot") ));
	}

	lTmpImage = mInputSlot->getImage();
	if(!lTmpImage)
	{
		mInputSlot->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlot->getFullName().c_str() + string(" does not have a valid image pointer (NULL)") ));
	}

	if(lTmpImage->getNbChannels()!=Image::eChannel1 || lTmpImage->getDepth()!=Image::eDepth8U)
	{
		mInputSlot->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlot->getFullName().c_str() + string(" must be a unsigned 8 bits 1 channel image (gray scale)") ));
	}

	setToIplImage(*lTmpImage, &lTmpImageIpl);

	lockParameters();
	mParamThresholdValue = getLockedParameter(PARAMETER_NAME_THRESHOLD_VALUE);
	mParamThresholdType = getLockedParameter(PARAMETER_NAME_THRESHOLD_TYPE);
	unlockParameters();

	mOutputSlot->lock();

	if(!mOutputImageIpl)
	{
		mOutputImageIpl = cvCreateImage(cvGetSize(lTmpImageIpl), IPL_DEPTH_8U, 1);
		mOutputImage = new Image(false);
		setFromIplImage(mOutputImageIpl, *mOutputImage);
		mOutputImage->setModel(Image::eModelGray);
	}

	cvThreshold(lTmpImageIpl, mOutputImageIpl, mParamThresholdValue.toInt(), 255,  mThresholdTypeMap[mParamThresholdType.toString()]);

	cvReleaseImageHeader(&lTmpImageIpl);

	mOutputSlot->unlock();
	mInputSlot->unlock();
}

/*! TODO:
*/
void ThresholdModule::stopFunction()
{
	// Nothing to do
}

/*! TODO:
*/
void ThresholdModule::resetFunction()
{
	if(mOutputImageIpl)
	{
		mOutputSlot->lock();
		cvReleaseImage(&mOutputImageIpl);
		delete mOutputImage;
		mOutputImageIpl = NULL;
		mOutputImage = NULL;
		mOutputSlot->unlock();
	}
}

/*!
*/
void ThresholdModule::updateParametersFunction()
{
	// Nothing to do for Testing module parameters, since no parameter has influence on other parameters
}

/*!
*/
string ThresholdModule::verifyParameterFunction(const Parameter& inParameter) const throw()
{
	string lResult = "";

	return lResult;
}

//Create entry points and modules functions
VIPERS_MODULE_ENTRY_POINT
VIPERS_MODULE_FUNCTIONS(ThresholdModule)
