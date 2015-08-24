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
 * \file MEI.cpp
 * \brief MEIModule class functions definition.
 * \author Frederic Jean
 * $Revision: 333 $
 * $Date: 2011-05-20 10:48:09 -0700 (Fri, 20 May 2011) $
 */

#include "MEI.hpp"
#include "MEIConfig.hpp"

#define VIPERS_UTILS_OPENCV
#include <ImageUtils.hpp>

#include <sstream>

using namespace VIPERS;
using namespace std;

#define MODULE_NAME "mei"
#define MODULE_DISPLAY_NAME "Motion Energy Image"

#define INPUT_SLOT_NAME_MASK "input-mask"
#define INPUT_SLOT_DISPLAYNAME_MASK "Input mask"
#define INPUT_SLOT_NAME_COLOR "input-image"
#define INPUT_SLOT_DISPLAYNAME_COLOR "Input image"

#define OUTPUT_SLOT_NAME_MEI "output-mei"
#define OUTPUT_SLOT_DISPLAYNAME_MEI "Output MEI"

#define PARAMETER_NAME_UNITS "units"
#define PARAMETER_NAME_DURATION "duration"

/*! TODO:
*/
MEIModule::MEIModule()
	:Module(MODULE_NAME, MODULE_DISPLAY_NAME, MEIMODULE_VERSION)
{
	mShortDescription = "Module for computing motion energy image";
	mLongDescription = "This module computes a motion energy image from a binary mask, or from a color image along with a binary mask";

	mInputSlotMask = newSlot(new ModuleSlot(this, INPUT_SLOT_NAME_MASK, INPUT_SLOT_DISPLAYNAME_MASK, "Input mask image used to compute MEI. Can be binary, gray scale, or color image"));
	mInputSlotColor = newSlot(new ModuleSlot(this, INPUT_SLOT_NAME_COLOR, INPUT_SLOT_DISPLAYNAME_COLOR, "Input image used to compute MEI. Can be gray scale of color image, and a mask must be provided"));
	mOutputSlotMEI = newSlot(new ModuleSlot(this, OUTPUT_SLOT_NAME_MEI, OUTPUT_SLOT_DISPLAYNAME_MEI, "Output MEI image", &mOutputMEI));

	mOutputMEIIpl = NULL;
	mOutputMEI = NULL;
}

/*! TODO:
*/
MEIModule::~MEIModule()
{
	if(mOutputMEIIpl)
	{
		cvReleaseImage(&mOutputMEIIpl);
		delete mOutputMEI;
	}

}

/*! TODO:
*/
void MEIModule::initFunction()
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
void MEIModule::startFunction()
{

}

/*! TODO:
*/
void MEIModule::pauseFunction()
{

}

/*! TODO:
*/
void MEIModule::processFunction(unsigned int inFrameNumber)
{
	const Image* lMaskImage = NULL;
	const Image* lColorImage = NULL;
  IplImage* lMaskImageIpl = NULL;
  IplImage* lColorImageIpl = NULL;

	string lUnits;

	if(!mInputSlotMask->isConnected())
		throw(Exception(Exception::eCodeUseModule, mInputSlotMask->getFullName().c_str() + string(" is not connected to an output slot") ));

	if(!mInputSlotColor->isConnected())
	{
		mInputSlotMask->lock();

		lMaskImage = mInputSlotMask->getImage();

		if(lMaskImage->getNbChannels()!=Image::eChannel1)
		{
			mInputSlotMask->unlock();
			throw(Exception(Exception::eCodeUseModule, mInputSlotMask->getFullName().c_str() + string(" must be a 1 channel image") ));
		}
		if(lMaskImage->getDepth()!=Image::eDepth8U)
		{
			mInputSlotMask->unlock();
			throw(Exception(Exception::eCodeUseModule, mInputSlotMask->getFullName().c_str() + string(" must be an unsigned 8 bits image") ));
		}

		mOutputSlotMEI->lock();

		if(!mOutputMEIIpl)
		{
			mOutputMEIIpl = cvCreateImage(cvSize(lMaskImage->getWidth(), lMaskImage->getHeight()), IPL_DEPTH_8U, 1);
			mOutputMEI = new Image(false);
			setFromIplImage(mOutputMEIIpl, *mOutputMEI);
			mOutputMEI->setModel(Image::eModelGray);
		}
		else if(mOutputMEI->getWidth()!=lMaskImage->getWidth() || mOutputMEI->getHeight()!=lMaskImage->getHeight())
		{
			cvReleaseImage(&mOutputMEIIpl);
			delete mOutputMEI;
			mOutputMEIIpl = cvCreateImage(cvSize(lMaskImage->getWidth(), lMaskImage->getHeight()), IPL_DEPTH_8U, 1);
      mOutputMEI = new Image(false);
      setFromIplImage(mOutputMEIIpl, *mOutputMEI);
      mOutputMEI->setModel(Image::eModelGray);
		}

		setToIplImage(*lMaskImage, &lMaskImageIpl);
		cvCopy(lMaskImageIpl, mOutputMEIIpl, lMaskImageIpl);
		cvReleaseImageHeader(&lMaskImageIpl);

		mOutputSlotMEI->unlock();
		mInputSlotMask->unlock();
	}
	else
	{
		mInputSlotMask->lock();

		lMaskImage = mInputSlotMask->getImage();

		if(lMaskImage->getNbChannels()!=Image::eChannel1)
		{
			mInputSlotMask->unlock();
			throw(Exception(Exception::eCodeUseModule, mInputSlotMask->getFullName().c_str() + string(" must be a 1 channel image") ));
		}
		if(lMaskImage->getDepth()!=Image::eDepth8U)
		{
			mInputSlotMask->unlock();
			throw(Exception(Exception::eCodeUseModule, mInputSlotMask->getFullName().c_str() + string(" must be an unsigned 8 bits image") ));
		}

		mInputSlotColor->lock();

		lColorImage = mInputSlotColor->getImage();

		if(lColorImage->getWidth()!=lMaskImage->getWidth() || lColorImage->getHeight()!=lMaskImage->getHeight())
		{
			mInputSlotColor->unlock();
			mInputSlotMask->unlock();
			throw(Exception(Exception::eCodeUseModule, mInputSlotMask->getFullName().c_str() + string(" and ") + mInputSlotColor->getFullName().c_str() + string(" must have the same size") ));
		}

		mOutputSlotMEI->lock();

    setToIplImage(*lMaskImage, &lMaskImageIpl);
		setToIplImage(*lColorImage, &lColorImageIpl);

		if(!mOutputMEIIpl)
		{
			mOutputMEIIpl = cvCreateImage(cvSize(lColorImageIpl->width, lColorImageIpl->height), lColorImageIpl->depth, lColorImageIpl->nChannels);
			mOutputMEI = new Image(false);
			setFromIplImage(mOutputMEIIpl, *mOutputMEI);
			mOutputMEI->setModel(Image::eModelRGB);
		}
		else if(mOutputMEIIpl->width!=lColorImageIpl->width || mOutputMEIIpl->height!=lColorImageIpl->height || mOutputMEIIpl->nChannels!=lColorImageIpl->nChannels || mOutputMEIIpl->depth!=lColorImageIpl->depth )
		{
			cvReleaseImage(&mOutputMEIIpl);
			delete mOutputMEI;
			mOutputMEIIpl = cvCreateImage(cvSize(lColorImageIpl->width, lColorImageIpl->height), lColorImageIpl->depth, lColorImageIpl->nChannels);
			mOutputMEI = new Image(false);
			setFromIplImage(mOutputMEIIpl, *mOutputMEI);
			mOutputMEI->setModel(Image::eModelRGB);
		}

		cvCopy(lColorImageIpl, mOutputMEIIpl, lMaskImageIpl);

		cvReleaseImageHeader(&lColorImageIpl);
    cvReleaseImageHeader(&lMaskImageIpl);

		mOutputSlotMEI->unlock();
		mInputSlotColor->unlock();
		mInputSlotMask->unlock();
	}
}

/*! TODO:
*/
void MEIModule::stopFunction()
{

}

/*! TODO:
*/
void MEIModule::resetFunction()
{
	if(mOutputMEIIpl)
	{
		mOutputSlotMEI->lock();
		cvReleaseImage(&mOutputMEIIpl);
		delete mOutputMEI;
		mOutputMEIIpl = NULL;
		mOutputMEI = NULL;
		mOutputSlotMEI->unlock();
	}

}

/*!
*/
void MEIModule::updateParametersFunction()
{

}

/*!
*/
string MEIModule::verifyParameterFunction(const Parameter& inParameter) const throw()
{
	string lResult = "";

	return lResult;
}

//Create entry points and modules functions
VIPERS_MODULE_ENTRY_POINT
VIPERS_MODULE_FUNCTIONS(MEIModule)
