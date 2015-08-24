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
 * \file Camera.cpp
 * \brief CameraModule class functions definition.
 * \author Frederic Jean
 * $Revision: 333 $
 * $Date: 2011-05-20 10:48:09 -0700 (Fri, 20 May 2011) $
 */

#include "Camera.hpp"
#include "CameraConfig.hpp"

#include <sstream>

#define VIPERS_UTILS_OPENCV
#include <ImageUtils.hpp>

using namespace VIPERS;
using namespace std;

#define MODULE_NAME "camera"
#define MODULE_DISPLAY_NAME "Camera"

#define OUTPUT_SLOT_NAME_FRAME "output-frame"
#define OUTPUT_SLOT_DISPLAYNAME_FRAME "Output frame"

#define PARAMETER_NAME_FRAMESIZE "frame-size"
#define PARAMETER_NAME_FRAMERATE "frame-rate"
#define PARAMETER_NAME_CAMERAINDEX "camera-index"

/*! TODO:
*/
CameraModule::CameraModule()
	:Module(MODULE_NAME, MODULE_DISPLAY_NAME, CAMERAMODULE_VERSION),
	 mParamCameraIndex(PARAMETER_NAME_CAMERAINDEX, Variable::eVariableTypeInt, "Camera index", "Camera index (starts from 0)", false),
	 mParamFrameSize(PARAMETER_NAME_FRAMESIZE, Variable::eVariableTypeSize, "Size", "Frame size", false),
	 mParamFrameRate(PARAMETER_NAME_FRAMERATE, Variable::eVariableTypeDouble, "FPS", "Frame rate of the camera", false)
{
	mShortDescription = "Module for acquiring frames from a camera";
	mLongDescription = "This is a simple module to acquire frames from a camera that is known to work with OpenCV";

	mParamCameraIndex.setValue(0);
	mParamFrameSize.setValue(Size(320, 240));
	mParamFrameRate.setValue(15.0);

	mParamCameraIndex.setMinValue("0");
	mParamCameraIndex.setMaxValue("16");
	mParamFrameRate.setMinValue("0");
	mParamFrameRate.setMaxValue("200");

	newParameter(mParamCameraIndex);
	newParameter(mParamFrameSize);
	newParameter(mParamFrameRate);

	mOutputSlot = newSlot(new ModuleSlot(this, OUTPUT_SLOT_NAME_FRAME, OUTPUT_SLOT_DISPLAYNAME_FRAME, "Frame coming from the specified camera", &mOutputFrame));

	mOutputFrame = NULL;
	mOutputFrameIpl = NULL;
	mCameraCapture = NULL;
}

/*! TODO:
*/
CameraModule::~CameraModule()
{
	if(mOutputFrameIpl)
	{
		cvReleaseImage(&mOutputFrameIpl);
	  delete mOutputFrameIpl;
	}
	if(mCameraCapture)
		cvReleaseCapture(&mCameraCapture);
}

/*! TODO:
*/
void CameraModule::initFunction()
{
	const IplImage* lTmpImage;

	// Refresh parameters
	lockParameters();
	Parameter lParamCameraIndex = getLockedParameter(PARAMETER_NAME_CAMERAINDEX);
	Parameter lParamFrameSize = getLockedParameter(PARAMETER_NAME_FRAMESIZE);
	Parameter lParamFrameRate = getLockedParameter(PARAMETER_NAME_FRAMERATE);
	unlockParameters();

	if(mCameraCapture)
	{
		cvReleaseCapture(&mCameraCapture);
		mCameraCapture = NULL;
	}

	mCameraCapture = cvCreateCameraCapture(lParamCameraIndex.toInt());

	if(!mCameraCapture)
	{
		ostringstream lStr;
		lStr << "Module \"" << getLabel().c_str() << "\" could not initialize camera with index \"" << lParamCameraIndex.toInt() << "\".";
		throw(Exception(Exception::eCodeUseModule, lStr.str().c_str()));
	}

	cvSetCaptureProperty(mCameraCapture, CV_CAP_PROP_FRAME_WIDTH, lParamFrameSize.toSize()[0]);
	cvSetCaptureProperty(mCameraCapture, CV_CAP_PROP_FRAME_HEIGHT, lParamFrameSize.toSize()[1]);
	cvSetCaptureProperty(mCameraCapture, CV_CAP_PROP_FPS, lParamFrameRate.toDouble());

	// Grab one frame for initialization
	lTmpImage = cvQueryFrame(mCameraCapture);
	if(!lTmpImage)
	{
		cvReleaseCapture(&mCameraCapture);
		mCameraCapture = NULL;
		ostringstream lStr;
		lStr << "Module \"" << getLabel().c_str() << "\" could not get an initialization frame from camera with index \"" << mParamCameraIndex.toInt() << "\".";
		throw(Exception(Exception::eCodeUseModule, lStr.str().c_str()));
	}

	mParamCameraIndex = lParamCameraIndex;
	mParamFrameSize = lParamFrameSize;
	mParamFrameRate = lParamFrameRate;

	mOutputSlot->lock();

	if(mOutputFrameIpl)
	{
		cvReleaseImage(&mOutputFrameIpl);
		delete mOutputFrame;
	}

	mOutputFrameIpl = cvCloneImage(lTmpImage);
	mOutputFrame = new Image(false);
	setFromIplImage(mOutputFrameIpl, *mOutputFrame);
	mOutputFrame->setModel(Image::eModelRGB);

	mOutputSlot->unlock();

}

/*! TODO:
*/
void CameraModule::startFunction()
{

}

/*! TODO:
*/
void CameraModule::pauseFunction()
{

}

/*! TODO:
*/
void CameraModule::processFunction(unsigned int inFrameNumber)
{
	const IplImage* lTmpImage;

	if(!mCameraCapture)
	{
		ostringstream lStr;
		lStr << "Module \"" << getLabel().c_str() << "\" cannot process any frame since it is not initialized";
		throw(Exception(Exception::eCodeUseModule, lStr.str().c_str()));
	}

	// Grab one frame for initialization
	lTmpImage = cvQueryFrame(mCameraCapture);
	if(!lTmpImage)
	{
		cvReleaseCapture(&mCameraCapture);
		cvReleaseImage(&mOutputFrameIpl);
		mCameraCapture = NULL;
		mOutputFrameIpl = NULL;
		delete mOutputFrame;
		mOutputFrame = NULL;
		ostringstream lStr;
		lStr << "Module \"" << getLabel().c_str() << "\" could not get a frame from camera with index \"" << mParamCameraIndex.toInt() << "\".";
		throw(Exception(Exception::eCodeUseModule, lStr.str().c_str()));
	}

	mOutputSlot->lock();
	cvCopy(lTmpImage, mOutputFrameIpl);
	mOutputSlot->unlock();

}

/*! TODO:
*/
void CameraModule::stopFunction()
{

}

/*! TODO:
*/
void CameraModule::resetFunction()
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
	if(mCameraCapture)
	{
		cvReleaseCapture(&mCameraCapture);
		mCameraCapture = NULL;
	}
}

/*!
*/
void CameraModule::updateParametersFunction()
{

}

/*!
*/
string CameraModule::verifyParameterFunction(const Parameter& inParameter) const throw()
{
	string lResult = "";

	return lResult;
}

//Create entry points and modules functions
VIPERS_MODULE_ENTRY_POINT
VIPERS_MODULE_FUNCTIONS(CameraModule)
