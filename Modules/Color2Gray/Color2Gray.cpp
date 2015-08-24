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
 * \file Color2Gray.cpp
 * \brief Color2GrayModule class functions definition.
 * \author Frederic Jean
 * $Revision: 308 $
 * $Date: 2009-11-05 21:45:49 -0500 (Thu, 05 Nov 2009) $
 */

#include "Color2Gray.hpp"
#include "Color2GrayConfig.hpp"

#define VIPERS_UTILS_OPENCV
#include <ImageUtils.hpp>

using namespace VIPERS;
using namespace std;

#define MODULE_NAME "color2gray"
#define MODULE_DISPLAY_NAME "Color to Gray"

#define INPUT_SLOT_NAME_IMAGE "input-image"
#define INPUT_SLOT_DISPLAYNAME_IMAGE "Color image"

#define OUTPUT_SLOT_NAME_IMAGE "output-image"
#define OUTPUT_SLOT_DISPLAYNAME_IMAGE "Gray image"

/*! TODO:
*/
Color2GrayModule::Color2GrayModule()
	:Module(MODULE_NAME, MODULE_DISPLAY_NAME, COLOR2GRAYMODULE_VERSION)
{
	mShortDescription = "Module for converting color image to gray image";
	mLongDescription = "This module converts color images to gray scale images";

	mInputSlot = newSlot(new ModuleSlot(this, INPUT_SLOT_NAME_IMAGE, INPUT_SLOT_DISPLAYNAME_IMAGE, "Color image"));
	mOutputSlot = newSlot(new ModuleSlot(this, OUTPUT_SLOT_NAME_IMAGE, OUTPUT_SLOT_DISPLAYNAME_IMAGE, "Gray image", &mOutImg));

	mOutImgIpl = NULL;
	mOutImg = NULL;
}

/*! TODO:
*/
Color2GrayModule::~Color2GrayModule()
{
	if(mOutImgIpl)
	{
		cvReleaseImage(&mOutImgIpl);
		delete mOutImg;
	}
}

/*! TODO:
*/
void Color2GrayModule::initFunction()
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
void Color2GrayModule::startFunction()
{

}

/*! TODO:
*/
void Color2GrayModule::pauseFunction()
{

}

/*! TODO:
*/
void Color2GrayModule::processFunction(unsigned int inFrameNumber)
{
  const Image* lTmpImage = NULL;
	IplImage* lTmpImageIpl = NULL;

	if(!mInputSlot->isConnected())
		throw(Exception(Exception::eCodeUseModule, mInputSlot->getFullName().c_str() + string(" is not connected to an output slot") ));

	mInputSlot->lock();

	lTmpImage = mInputSlot->getImage();
	if(!lTmpImage)
	{
		mInputSlot->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlot->getFullName().c_str() + string(" does not have a valid image pointer (NULL)") ));
	}

  if(lTmpImage->getNbChannels()!=Image::eChannel3 || lTmpImage->getDepth()!=Image::eDepth8U)
  {
    mInputSlot->unlock();
    throw(Exception(Exception::eCodeUseModule, mInputSlot->getFullName().c_str() + string(" is not a valid color image with 3 channels and a depth of 8 unsigned bits") ));
  }

	setToIplImage(*lTmpImage, &lTmpImageIpl);

	mOutputSlot->lock();

	if(!mOutImgIpl)
	{
		mOutImgIpl = cvCreateImage(cvSize(lTmpImage->getWidth(), lTmpImage->getHeight()), IPL_DEPTH_8U, 1);
		mOutImg = new Image(false);
		setFromIplImage(mOutImgIpl, *mOutImg);
		mOutImg->setModel(Image::eModelGray);
	}
	else if(lTmpImage->getWidth()!=mOutImg->getWidth() || lTmpImage->getWidth()!=mOutImg->getHeight())
	{
		cvReleaseImage(&mOutImgIpl);
		delete mOutImg;
		mOutImgIpl = cvCreateImage(cvSize(lTmpImage->getWidth(), lTmpImage->getHeight()), IPL_DEPTH_8U, 1);
		mOutImg = new Image(false);
    setFromIplImage(mOutImgIpl, *mOutImg);
    mOutImg->setModel(lTmpImage->getModel());
	}

	if(lTmpImage->getModel()==Image::eModelRGB)
	  cvCvtColor(lTmpImageIpl, mOutImgIpl, CV_RGB2GRAY);
	else
	  cvCvtColor(lTmpImageIpl, mOutImgIpl, CV_BGR2GRAY);

	cvReleaseImageHeader(&lTmpImageIpl);

	mOutputSlot->unlock();
	mInputSlot->unlock();
}

/*! TODO:
*/
void Color2GrayModule::stopFunction()
{
	// Nothing to do
}

/*! TODO:
*/
void Color2GrayModule::resetFunction()
{
	if(mOutImgIpl)
	{
		mOutputSlot->lock();
		cvReleaseImage(&mOutImgIpl);
		delete mOutImg;
		mOutImgIpl = NULL;
		mOutImg = NULL;
		mOutputSlot->unlock();
	}
}

/*!
*/
void Color2GrayModule::updateParametersFunction()
{
	// Nothing to do for Testing module parameters, since no parameter has influence on other parameters
}

/*!
*/
string Color2GrayModule::verifyParameterFunction(const Parameter& inParameter) const throw()
{
	string lResult = "";

	return lResult;
}

//Create entry points and modules functions
VIPERS_MODULE_ENTRY_POINT
VIPERS_MODULE_FUNCTIONS(Color2GrayModule)
