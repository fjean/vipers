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
 * \file Resize.cpp
 * \brief ResizeModule class functions definition.
 * \author Frederic Jean
 * $Revision: 337 $
 * $Date: 2011-06-14 09:44:06 -0700 (Tue, 14 Jun 2011) $
 */

#include "Resize.hpp"
#include "ResizeConfig.hpp"

#include <iostream>

#define VIPERS_UTILS_OPENCV
#include <ImageUtils.hpp>

using namespace VIPERS;
using namespace std;

#define MODULE_NAME "resize"
#define MODULE_DISPLAY_NAME "Resize"

#define INPUT_SLOT_NAME_IMAGE "input-image"
#define INPUT_SLOT_DISPLAYNAME_IMAGE "Input image"

#define OUTPUT_SLOT_NAME_RESIZED_IMAGE "output-resized"
#define OUTPUT_SLOT_DISPLAYNAME_RESIZED_IMAGE "Resized image"

#define PARAMETER_NAME_NEWSIZE "new-size"
#define PARAMETER_NAME_RESIZE_METHOD "resize-method"

/*! TODO:
*/
ResizeModule::ResizeModule()
	:Module(MODULE_NAME, MODULE_DISPLAY_NAME, RESIZEMODULE_VERSION),
	mParamImageNewSize(PARAMETER_NAME_NEWSIZE, Variable::eVariableTypeSize, "New size", "New image size", false),
	mParamResizeMethod(PARAMETER_NAME_RESIZE_METHOD, Variable::eVariableTypeString, "Resize method", "Method used to resize the input image", true)
{
	mShortDescription = "Module for resizing image";
	mLongDescription = "This module resizes an image using common interpolation methods";

	ValueSet lTmpMethodName;
	lTmpMethodName.insert(Value("nearest", "Nearest", "Nearest-neigbor interpolation"));
	lTmpMethodName.insert(Value("bilinear", "Bilinear", "Bilinear interpolation"));
	lTmpMethodName.insert(Value("area", "Area", "Resampling using pixel area relation"));
	lTmpMethodName.insert(Value("bicubic", "Bicubic", "Bicubic interpolation"));

	mParamResizeMethod.setPossibleValues(lTmpMethodName);

	mResizeMethodMap["nearest"] = CV_INTER_NN;
	mResizeMethodMap["bilinear"] = CV_INTER_LINEAR;
	mResizeMethodMap["area"] = CV_INTER_AREA;
	mResizeMethodMap["bicubic"] = CV_INTER_CUBIC;

	mParamImageNewSize.setValue(Size(640,480));
	mParamResizeMethod.setValueStr("bilinear");

	newParameter(mParamImageNewSize);
	newParameter(mParamResizeMethod);

	mInputSlot = newSlot(new ModuleSlot(this, INPUT_SLOT_NAME_IMAGE, INPUT_SLOT_DISPLAYNAME_IMAGE, "Image to be resized"));
	mOutputSlot = newSlot(new ModuleSlot(this, OUTPUT_SLOT_NAME_RESIZED_IMAGE, OUTPUT_SLOT_DISPLAYNAME_RESIZED_IMAGE, "Resized image", &mOutputImage));

	mOutputImageIpl = NULL;
	mOutputImage = NULL;
}

/*! TODO:
*/
ResizeModule::~ResizeModule()
{
	if(mOutputImageIpl)
	{
		cvReleaseImage(&mOutputImageIpl);
		delete mOutputImage;
	}
}

/*! TODO:
*/
void ResizeModule::initFunction()
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
void ResizeModule::startFunction()
{

}

/*! TODO:
*/
void ResizeModule::pauseFunction()
{

}

/*! TODO:
*/
void ResizeModule::processFunction(unsigned int inFrameNumber)
{
  const Image* lTmpImage = NULL;
	IplImage* lTmpImageIpl = NULL;
	Size lSize;

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

	setToIplImage(*lTmpImage, &lTmpImageIpl);

	lockParameters();
	lSize = getLockedParameter(PARAMETER_NAME_NEWSIZE).toSize();
	mParamResizeMethod = getLockedParameter(PARAMETER_NAME_RESIZE_METHOD);
	unlockParameters();

	mOutputSlot->lock();

	if(!mOutputImageIpl)
	{
		mOutputImageIpl = cvCreateImage(cvSize(lSize[0], lSize[1]), lTmpImageIpl->depth, lTmpImageIpl->nChannels);
		mOutputImage = new Image(false);
		setFromIplImage(mOutputImageIpl, *mOutputImage);
		mParamImageNewSize.setValue(lSize);
		mOutputImage->setModel(lTmpImage->getModel());
	}
	else if(mParamImageNewSize.toSize()!=lSize)
	{
		mParamImageNewSize.setValue(lSize);
		cvReleaseImage(&mOutputImageIpl);
		delete mOutputImage;
		mOutputImageIpl = cvCreateImage(cvSize(lSize[0], lSize[1]), lTmpImageIpl->depth, lTmpImageIpl->nChannels);
    mOutputImage = new Image(false);
    setFromIplImage(mOutputImageIpl, *mOutputImage);
    mOutputImage->setModel(lTmpImage->getModel());
	}

	cvResize(lTmpImageIpl, mOutputImageIpl, mResizeMethodMap[mParamResizeMethod.toString()]);

	cvReleaseImageHeader(&lTmpImageIpl);

	mOutputSlot->unlock();
	mInputSlot->unlock();
}

/*! TODO:
*/
void ResizeModule::stopFunction()
{
	// Nothing to do
}

/*! TODO:
*/
void ResizeModule::resetFunction()
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
void ResizeModule::updateParametersFunction()
{
	// Nothing to do for Testing module parameters, since no parameter has influence on other parameters
}

/*!
*/
string ResizeModule::verifyParameterFunction(const Parameter& inParameter) const throw()
{
	string lResult = "";

	return lResult;
}

//Create entry points and modules functions
VIPERS_MODULE_ENTRY_POINT
VIPERS_MODULE_FUNCTIONS(ResizeModule)
