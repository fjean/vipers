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
 * \file DistanceTransform.cpp
 * \brief DistanceTransformModule class functions definition.
 * \author Frederic Jean
 * $Revision: 333 $
 * $Date: 2011-05-20 10:48:09 -0700 (Fri, 20 May 2011) $
 */

#include "DistanceTransform.hpp"
#include "DistanceTransformConfig.hpp"

#include <sstream>

#define VIPERS_UTILS_OPENCV
#include <ImageUtils.hpp>

using namespace VIPERS;
using namespace std;

#define MODULE_NAME "distancetransform"
#define MODULE_DISPLAY_NAME "Distance transform"

#define INPUT_SLOT_NAME_MASK "input-mask"
#define INPUT_SLOT_DISPLAYNAME_MASK "Input mask"
#define OUTPUT_SLOT_NAME_DIST "output-distance"
#define OUTPUT_SLOT_DISPLAYNAME_DIST "Output distance"
#define OUTPUT_SLOT_NAME_DIST_GRAY "output-distance-gray"
#define OUTPUT_SLOT_DISPLAYNAME_DIST_GRAY "Output distance gray"

#define PARAMETER_NAME_DISTTYPE "distance-type"
#define PARAMETER_NAME_MASKSIZE "mask-size"
#define PARAMETER_NAME_INVERT "invert"

/*! TODO:
*/
DistanceTransformModule::DistanceTransformModule()
	:Module(MODULE_NAME, MODULE_DISPLAY_NAME, DISTTRANSFORMMODULE_VERSION),
	mParamDistanceType(PARAMETER_NAME_DISTTYPE, Variable::eVariableTypeString, "Distance type", "Type of distance that is computed", true),
	mParamMaskSize(PARAMETER_NAME_MASKSIZE, Variable::eVariableTypeUInt, "Mask size", "Size of the mask used to compute the distance", true),
	mParamInvert(PARAMETER_NAME_INVERT, Variable::eVariableTypeBool, "Invert gray image", "Invert gray image so the minimum distance is white", true)
{
	mShortDescription = "Module for computing distance transform image";
	mLongDescription = "This module computes a distance transform on a binary image";

	ValueSet lTypeValues;
	lTypeValues.insert(Value("l1", "L1", "L1 distance"));
	lTypeValues.insert(Value("l2", "L2", "L2 distance"));
	lTypeValues.insert(Value("c", "C", "C distance"));

	mDistanceTypeMap["l1"] = CV_DIST_L1;
	mDistanceTypeMap["l2"] = CV_DIST_L2;
	mDistanceTypeMap["c"] = CV_DIST_C;

	ValueSet lSizeValues;
	lSizeValues.insert(Value("3x3", "3x3", "3x3 mask"));
	lSizeValues.insert(Value("5x5", "5x5", "5x5 mask"));

	mMaskSizeMap["3x3"] = 3;
	mMaskSizeMap["5x5"] = 5;

	mParamDistanceType.setPossibleValues(lTypeValues);
	mParamMaskSize.setPossibleValues(lSizeValues);

	mParamDistanceType.setValueStr("l2");
	mParamMaskSize.setValue(3);
	mParamInvert.setValue(false);

	newParameter(mParamDistanceType);
	newParameter(mParamMaskSize);
	newParameter(mParamInvert);

	mInputSlotMask = newSlot(new ModuleSlot(this, INPUT_SLOT_NAME_MASK, INPUT_SLOT_DISPLAYNAME_MASK, "Input mask image used to compute distance"));
	mOutputSlotDistanceTransform = newSlot(new ModuleSlot(this, OUTPUT_SLOT_NAME_DIST, OUTPUT_SLOT_DISPLAYNAME_DIST, "Output distance transform image (floating point)", &mOutputDistanceTransform));
	mOutputSlotDistanceTransformGray = newSlot(new ModuleSlot(this, OUTPUT_SLOT_NAME_DIST_GRAY, OUTPUT_SLOT_DISPLAYNAME_DIST_GRAY, "Output distance transform image (gray scale image)", &mOutputDistanceTransformGray));

	mOutputDistanceTransformIpl = NULL;
	mOutputDistanceTransformGrayIpl = NULL;
  mOutputDistanceTransform = NULL;
  mOutputDistanceTransformGray = NULL;
}

/*! TODO:
*/
DistanceTransformModule::~DistanceTransformModule()
{
	if(mOutputDistanceTransformIpl)
	{
		cvReleaseImage(&mOutputDistanceTransformIpl);
		delete mOutputDistanceTransform;
	}
	if(mOutputDistanceTransformGrayIpl)
	{
		cvReleaseImage(&mOutputDistanceTransformGrayIpl);
		delete mOutputDistanceTransformGray;
	}
}

/*! TODO:
*/
void DistanceTransformModule::initFunction()
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
void DistanceTransformModule::startFunction()
{

}

/*! TODO:
*/
void DistanceTransformModule::pauseFunction()
{

}

/*! TODO:
*/
void DistanceTransformModule::processFunction(unsigned int inFrameNumber)
{
	const Image* lMaskImage;
	IplImage* lMaskImageIpl;
	double lMinDist;
	double lMaxDist;

	if(!mInputSlotMask->isConnected())
		throw(Exception(Exception::eCodeUseModule, mInputSlotMask->getFullName().c_str() + string(" is not connected to an output slot") ));

	lockParameters();
	mParamDistanceType = getLockedParameter(PARAMETER_NAME_DISTTYPE);
	mParamMaskSize = getLockedParameter(PARAMETER_NAME_MASKSIZE);
	mParamInvert = getLockedParameter(PARAMETER_NAME_INVERT);
	unlockParameters();

	mInputSlotMask->lock();

	lMaskImage = mInputSlotMask->getImage();

	if(!lMaskImage)
	{
	   mInputSlotMask->unlock();
	   throw(Exception(Exception::eCodeUseModule, mInputSlotMask->getFullName().c_str() + string(" does not have a valid image") ));
	}

	if(lMaskImage->getNbChannels()!=Image::eChannel1)
	{
		mInputSlotMask->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotMask->getFullName().c_str() + string(" must be a 1 channel image mask") ));
	}

	setToIplImage(*lMaskImage, &lMaskImageIpl);

	mOutputSlotDistanceTransform->lock();
	mOutputSlotDistanceTransformGray->lock();

	if(!mOutputDistanceTransformIpl)
	{
		mOutputDistanceTransformIpl = cvCreateImage(cvSize(lMaskImageIpl->width, lMaskImageIpl->height), IPL_DEPTH_32F, 1);
		mOutputDistanceTransformGrayIpl = cvCreateImage(cvSize(lMaskImageIpl->width, lMaskImageIpl->height), IPL_DEPTH_8U, 1);
		mOutputDistanceTransform = new Image(false);
		mOutputDistanceTransformGray = new Image(false);
		setFromIplImage(mOutputDistanceTransformIpl, *mOutputDistanceTransform);
		setFromIplImage(mOutputDistanceTransformGrayIpl, *mOutputDistanceTransformGray);
	}
	else if(mOutputDistanceTransformIpl->width!=lMaskImageIpl->width || mOutputDistanceTransformIpl->height!=lMaskImageIpl->height)
	{
		cvReleaseImage(&mOutputDistanceTransformIpl);
		cvReleaseImage(&mOutputDistanceTransformGrayIpl);
		mOutputDistanceTransformIpl = cvCreateImage(cvSize(lMaskImageIpl->width, lMaskImageIpl->height), IPL_DEPTH_32F, 1);
		mOutputDistanceTransformGrayIpl = cvCreateImage(cvSize(lMaskImageIpl->width, lMaskImageIpl->height), IPL_DEPTH_8U, 1);
	}

	cvDistTransform(lMaskImageIpl, mOutputDistanceTransformIpl, mDistanceTypeMap[mParamDistanceType.toString()], mMaskSizeMap[mParamMaskSize.toString()]);

	if(mOutputSlotDistanceTransformGray->getUseCount())
	{
		cvMinMaxLoc(mOutputDistanceTransformIpl, &lMinDist, &lMaxDist);
		if(lMaxDist>0 || lMinDist>0)
		{
			if(mParamInvert.toBool())
				cvConvertScale(mOutputDistanceTransformIpl, mOutputDistanceTransformGrayIpl, -255.0/((lMaxDist-lMinDist)), 255);
			else
				cvConvertScale(mOutputDistanceTransformIpl, mOutputDistanceTransformGrayIpl, 255.0/(lMaxDist-lMinDist), lMinDist*255.0/(lMaxDist-lMinDist));
		}
	}

	cvReleaseImageHeader(&lMaskImageIpl);

	mOutputSlotDistanceTransformGray->unlock();
	mOutputSlotDistanceTransform->unlock();
	mInputSlotMask->unlock();
}

/*! TODO:
*/
void DistanceTransformModule::stopFunction()
{

}

/*! TODO:
*/
void DistanceTransformModule::resetFunction()
{
	if(mOutputDistanceTransformIpl)
	{
		mOutputSlotDistanceTransform->lock();
		cvReleaseImage(&mOutputDistanceTransformIpl);
		delete mOutputDistanceTransform;
		mOutputDistanceTransformIpl = NULL;
		mOutputDistanceTransform = NULL;
		mOutputSlotDistanceTransform->unlock();
	}
	if(mOutputDistanceTransformGrayIpl)
	{
		mOutputSlotDistanceTransformGray->lock();
		cvReleaseImage(&mOutputDistanceTransformGrayIpl);
		delete mOutputDistanceTransformGray;
		mOutputDistanceTransformGrayIpl = NULL;
		mOutputDistanceTransformGray = NULL;
		mOutputSlotDistanceTransformGray->unlock();
	}
}

/*!
*/
void DistanceTransformModule::updateParametersFunction()
{

}

/*!
*/
string DistanceTransformModule::verifyParameterFunction(const Parameter& inParameter) const throw()
{
	string lResult = "";

	return lResult;
}

//Create entry points and modules functions
VIPERS_MODULE_ENTRY_POINT
VIPERS_MODULE_FUNCTIONS(DistanceTransformModule)
