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
 * \file VideoReader.cpp
 * \brief VideoReaderModule class functions definition.
 * \author Frederic Jean
 * $Revision: 333 $
 * $Date: 2011-05-20 10:48:09 -0700 (Fri, 20 May 2011) $
 */

#include "VideoReader.hpp"
#include "VideoReaderConfig.hpp"
#include <sstream>
#include <iostream>

#define VIPERS_UTILS_OPENCV
#include <ImageUtils.hpp>

using namespace VIPERS;
using namespace std;

#define MODULE_NAME "videoreader"
#define MODULE_DISPLAY_NAME "Video reader"

#define OUTPUT_SLOT_NAME_IMAGE "output-image"
#define OUTPUT_SLOT_DISPLAYNAME_IMAGE "Image"

#define PARAMETER_NAME_VIDEO_FILE "video-file"
#define PARAMETER_NAME_LOOP "loop"

/*! TODO:
*/
VideoReaderModule::VideoReaderModule()
	:Module(MODULE_NAME, MODULE_DISPLAY_NAME, VIDEOREADERMODULE_VERSION),
	mParamVideoFile(PARAMETER_NAME_VIDEO_FILE, Variable::eVariableTypeFile, "Video file", "Video of the image to read", false),
	mParamLoop(PARAMETER_NAME_LOOP, Variable::eVariableTypeBool, "Loop", "Loop on video", false)
{
	mShortDescription = "Module reading a video from a file";
	mLongDescription = "This module reads the frames from a video file";

	mParamLoop.setValue(false);

	newParameter(mParamVideoFile);
	newParameter(mParamLoop);

	mOutputSlot = newSlot(new ModuleSlot(this, OUTPUT_SLOT_NAME_IMAGE, OUTPUT_SLOT_DISPLAYNAME_IMAGE, "Frame read from specified file", &mOutputImage));

	mOutputImage = NULL;
	mOutputImageIpl = NULL;
	mVideoCapture = NULL;
	mCurrentFrame = 0;
	mNbFrames = 0;
}

/*! TODO:
*/
VideoReaderModule::~VideoReaderModule()
{
	if(mOutputImageIpl)
	{
		cvReleaseImage(&mOutputImageIpl);
		delete mOutputImage;
	}
	if(mVideoCapture)
		cvReleaseCapture(&mVideoCapture);
}

/*! TODO:
*/
void VideoReaderModule::initFunction()
{
	const IplImage* lTmpImage;

	// Refresh parameters
	lockParameters();
	mParamVideoFile = getLockedParameter(PARAMETER_NAME_VIDEO_FILE);
	mParamLoop = getLockedParameter(PARAMETER_NAME_LOOP);
	unlockParameters();

	mCurrentFrame = 0;
	mNbFrames = 0;
	setMaxNumberFrames(0);
	setFrameRate(0.0);

	if(mVideoCapture)
	{
		cvReleaseCapture(&mVideoCapture);
		mVideoCapture = NULL;
	}

	mVideoCapture = cvCaptureFromFile(mParamVideoFile.toString().c_str());

	if(!mVideoCapture)
		throw(Exception(Exception::eCodeUseModule, string("Module \"") + getLabel().c_str() + string("\" encountered an error while opening file \"") + mParamVideoFile.toString().c_str() + string("\"; Not a supported video file, or file does not exist.") ));

	lTmpImage = cvQueryFrame(mVideoCapture);
	if(!lTmpImage)
	{
		cvReleaseCapture(&mVideoCapture);
		mVideoCapture = NULL;
		ostringstream lStr;
		lStr << "Module \"" << getLabel().c_str() << "\" could not get an initialization frame from file \"" << mParamVideoFile.toString().c_str() << "\".";
		throw(Exception(Exception::eCodeUseModule, lStr.str().c_str()));
	}

	mNbFrames = static_cast<unsigned int>(cvGetCaptureProperty(mVideoCapture, CV_CAP_PROP_FRAME_COUNT))-1;
	setFrameRate(cvGetCaptureProperty(mVideoCapture, CV_CAP_PROP_FPS));

	if(mNbFrames<1)
	{
		cvReleaseCapture(&mVideoCapture);
		mVideoCapture = NULL;
		ostringstream lStr;
		lStr << "Module \"" << getLabel().c_str() << "\" could not get any frame from file \"" << mParamVideoFile.toString().c_str() << "\". The file is reported to have no frame.";
		throw(Exception(Exception::eCodeUseModule, lStr.str().c_str()));
	}

	if(mParamLoop.toBool())
		setMaxNumberFrames(0);
	else
		setMaxNumberFrames(mNbFrames);

	mOutputSlot->lock();

	if(mOutputImageIpl)
	{
		cvReleaseImage(&mOutputImageIpl);
		delete mOutputImage;
	}
	mOutputImageIpl = cvCloneImage(lTmpImage);
	mOutputImage = new Image(false);
	setFromIplImage(mOutputImageIpl, *mOutputImage);

	mOutputSlot->unlock();

}

/*! TODO:
*/
void VideoReaderModule::startFunction()
{

}

/*! TODO:
*/
void VideoReaderModule::pauseFunction()
{

}

/*! TODO:
*/
void VideoReaderModule::processFunction(unsigned int inFrameNumber)
{
	const IplImage* lTmpImage;
	unsigned int lFrameNumber = inFrameNumber;

	if(!mVideoCapture)
	{
		ostringstream lStr;
		lStr << "Module \"" << getLabel().c_str() << "\" cannot read any frame since it is not initialized";
		throw(Exception(Exception::eCodeUseModule, lStr.str().c_str()));
	}

	if(mParamLoop.toBool())
		lFrameNumber = inFrameNumber % mNbFrames;

	if(lFrameNumber>=mNbFrames)
	{
		ostringstream lStr;
		lStr << "Module \"" << getLabel().c_str() << "\" could not get frame " << lFrameNumber << " from file \"" << mParamVideoFile.toString().c_str() << "\".";
		throw(Exception(Exception::eCodeUseModule, lStr.str().c_str()));
	}

	if(lFrameNumber == mCurrentFrame)
	{
		return;
	}
	else if(lFrameNumber==(mCurrentFrame+1))
	{
		mCurrentFrame++;
	}
	else if(lFrameNumber < mCurrentFrame || lFrameNumber > mCurrentFrame )
	{
		cvSetCaptureProperty(mVideoCapture, CV_CAP_PROP_POS_FRAMES, lFrameNumber);
		mCurrentFrame = lFrameNumber;
	}

	// Grab one frame for initialization
	lTmpImage = cvQueryFrame(mVideoCapture);
	if(!lTmpImage)
	{
		ostringstream lStr;
		lStr << "Module \"" << getLabel().c_str() << "\" could not get a frame from file \"" << mParamVideoFile.toString().c_str() << "\".";
		throw(Exception(Exception::eCodeUseModule, lStr.str().c_str()));
	}

	mOutputSlot->lock();
	cvCopy(lTmpImage, mOutputImageIpl);
	mOutputSlot->unlock();

}

/*! TODO:
*/
void VideoReaderModule::stopFunction()
{

}

/*! TODO:
*/
void VideoReaderModule::resetFunction()
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
	if(mVideoCapture)
		cvReleaseCapture(&mVideoCapture);

	mCurrentFrame = 0;
	mNbFrames = 0;

	setFrameRate(0.0);
	setMaxNumberFrames(0);
}

/*!
*/
void VideoReaderModule::updateParametersFunction()
{
	// Nothing to do for Testing module parameters, since no parameter has influence on other parameters
}

/*!
*/
string VideoReaderModule::verifyParameterFunction(const Parameter& inParameter) const throw()
{
	string lResult = "";

	return lResult;
}

//Create entry points and modules functions
VIPERS_MODULE_ENTRY_POINT
VIPERS_MODULE_FUNCTIONS(VideoReaderModule)
