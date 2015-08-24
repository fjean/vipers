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
 * \file ImageDifference.cpp
 * \brief ImageDifferenceModule class functions definition.
 * \author Frederic Jean
 * $Revision: 335 $
 * $Date: 2011-06-13 19:43:48 -0700 (Mon, 13 Jun 2011) $
 */

#include "ImageDifference.hpp"
#include "ImageDifferenceConfig.hpp"

#include <sstream>

#define VIPERS_UTILS_OPENCV
#include <ImageUtils.hpp>

using namespace VIPERS;
using namespace std;

#define MODULE_NAME "imagedifference"
#define MODULE_DISPLAY_NAME "Image difference"

#define INPUT_SLOT_NAME_IMAGE_ONE "input-image-one"
#define INPUT_SLOT_DISPLAYNAME_IMAGE_ONE "First input image"
#define INPUT_SLOT_NAME_IMAGE_TWO "input-image-second"
#define INPUT_SLOT_DISPLAYNAME_IMAGE_TWO "Second input image"

#define OUTPUT_SLOT_NAME_FRAME "output-frame"
#define OUTPUT_SLOT_DISPLAYNAME_FRAME "Output difference image"

/*! TODO:
*/
ImageDifferenceModule::ImageDifferenceModule()
	:Module(MODULE_NAME, MODULE_DISPLAY_NAME, IMAGEDIFFERENCEMODULE_VERSION)
{
	mShortDescription = "Module for computing images difference";
	mLongDescription = "This module computes the difference of two images";

	mInputSlotImage1 = newSlot(new ModuleSlot(this, INPUT_SLOT_NAME_IMAGE_ONE, INPUT_SLOT_DISPLAYNAME_IMAGE_ONE, "First image"));
	mInputSlotImage2 = newSlot(new ModuleSlot(this, INPUT_SLOT_NAME_IMAGE_TWO, INPUT_SLOT_DISPLAYNAME_IMAGE_TWO, "Second image"));

	mOutputSlot = newSlot(new ModuleSlot(this, OUTPUT_SLOT_NAME_FRAME, OUTPUT_SLOT_DISPLAYNAME_FRAME, "Difference image", &mOutputFrame));

	mOutputFrame = NULL;
	mOutputFrameIpl = NULL;
}

/*! TODO:
*/
ImageDifferenceModule::~ImageDifferenceModule()
{
	if(mOutputFrameIpl)
	{
		cvReleaseImage(&mOutputFrameIpl);
		delete mOutputFrame;
	}
}

/*! TODO:
*/
void ImageDifferenceModule::initFunction()
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
void ImageDifferenceModule::startFunction()
{

}

/*! TODO:
*/
void ImageDifferenceModule::pauseFunction()
{

}

/*! TODO:
*/
void ImageDifferenceModule::processFunction(unsigned int inFrameNumber)
{
	const Image* lImageOne;
	const Image* lImageTwo;
	IplImage* lImageOneIpl;
	IplImage* lImageTwoIpl;

	unsigned int lWidth;
	unsigned int lHeight;
	Image::Channel lChannels;
	Image::Depth lDepth;
	
	if(!mInputSlotImage1->isConnected())
		throw(Exception(Exception::eCodeUseModule, mInputSlotImage1->getFullName().c_str() + string(" is not connected to an output slot") ));
	if(!mInputSlotImage2->isConnected())
		throw(Exception(Exception::eCodeUseModule, mInputSlotImage2->getFullName().c_str() + string(" is not connected to an output slot") ));

	mInputSlotImage1->lock();
	mInputSlotImage2->lock();
	mOutputSlot->lock();

	lImageOne = mInputSlotImage1->getImage();
	lImageTwo = mInputSlotImage2->getImage();

	// Check validity of image
	if(!lImageOne)
	{
		mOutputSlot->unlock();
		mInputSlotImage2->unlock();
		mInputSlotImage1->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotImage1->getFullName().c_str() + string(" does not have a valid image pointer (NULL)") ));
	}
	if(!lImageTwo)
	{
		mOutputSlot->unlock();
		mInputSlotImage2->unlock();
		mInputSlotImage1->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotImage2->getFullName().c_str() + string(" does not have a valid image pointer (NULL)") ));
	}


	lWidth = lImageOne->getWidth();
	lHeight = lImageOne->getHeight();
	lChannels = lImageOne->getNbChannels();
	lDepth = lImageOne->getDepth();

	//Verify image depth and number of channels
	if(lWidth!=lImageTwo->getWidth() || lHeight!=lImageTwo->getHeight())
	{
		mOutputSlot->unlock();
		mInputSlotImage2->unlock();
		mInputSlotImage1->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotImage1->getFullName().c_str() + string(" and ") + mInputSlotImage2->getFullName().c_str() + string(" must have the same dimension") ));
	}

	//Verify image depth and number of channels
	if(lDepth!=lImageTwo->getDepth())
	{
		mOutputSlot->unlock();
		mInputSlotImage2->unlock();
		mInputSlotImage1->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotImage1->getFullName().c_str() + string(" and ") + mInputSlotImage2->getFullName().c_str() + string(" must have the same depth (8 bits)") ));
	}

	//Verify image depth and number of channels
	if(lChannels!=lImageTwo->getNbChannels())
	{
		mOutputSlot->unlock();
		mInputSlotImage2->unlock();
		mInputSlotImage1->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotImage1->getFullName().c_str() + string(" and ") + mInputSlotImage2->getFullName().c_str() + string(" must have the same number of channels") ));
	}

	if(lChannels!=Image::eChannel1 || lDepth!=Image::eDepth8U)
	{
		mOutputSlot->unlock();
		mInputSlotImage2->unlock();
		mInputSlotImage1->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotImage1->getFullName().c_str() + string(" and ") + mInputSlotImage2->getFullName().c_str() + string(" must be a unsigned 8 bits 1 channel images") ));
	}

	setToIplImage(*lImageOne, &lImageOneIpl);
	setToIplImage(*lImageTwo, &lImageTwoIpl);

	// Create output image
	if(!mOutputFrameIpl)
	{
		mOutputFrameIpl = cvCreateImage(cvGetSize(lImageOneIpl), IPL_DEPTH_8U, 1);
		mOutputFrame = new Image(false);
		setFromIplImage(mOutputFrameIpl, *mOutputFrame);
		mOutputFrame->setModel(Image::eModelGray);
	}

	// Embed image
	cvAbsDiff(lImageOneIpl, lImageTwoIpl, mOutputFrameIpl);

	cvReleaseImageHeader(&lImageOneIpl);
	cvReleaseImageHeader(&lImageTwoIpl);

	mOutputSlot->unlock();
	mInputSlotImage2->unlock();
	mInputSlotImage1->unlock();
}

/*! TODO:
*/
void ImageDifferenceModule::stopFunction()
{

}

/*! TODO:
*/
void ImageDifferenceModule::resetFunction()
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
}

/*!
*/
void ImageDifferenceModule::updateParametersFunction()
{

}

/*!
*/
string ImageDifferenceModule::verifyParameterFunction(const Parameter& inParameter) const throw()
{
	string lResult = "";

	return lResult;
}

//Create entry points and modules functions
VIPERS_MODULE_ENTRY_POINT
VIPERS_MODULE_FUNCTIONS(ImageDifferenceModule)
