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
 * \file Embedding.cpp
 * \brief EmbeddingModule class functions definition.
 * \author Frederic Jean
 * $Revision: 333 $
 * $Date: 2011-05-20 10:48:09 -0700 (Fri, 20 May 2011) $
 */

#include "Embedding.hpp"
#include "EmbeddingConfig.hpp"

#include <sstream>

#define VIPERS_UTILS_OPENCV
#include <ImageUtils.hpp>

using namespace VIPERS;
using namespace std;

#define MODULE_NAME "embedding"
#define MODULE_DISPLAY_NAME "Embedding"

#define INPUT_SLOT_NAME_BGIMAGE "input-background-image"
#define INPUT_SLOT_DISPLAYNAME_BGIMAGE "Input background image"
#define INPUT_SLOT_NAME_EBDIMAGE "input-embedding-image"
#define INPUT_SLOT_DISPLAYNAME_EBDIMAGE "Input image to embed"
#define INPUT_SLOT_NAME_EBDMASK "input-embedding-mask"
#define INPUT_SLOT_DISPLAYNAME_EBDMASK "Input embeding mask"

#define OUTPUT_SLOT_NAME_FRAME "output-frame"
#define OUTPUT_SLOT_DISPLAYNAME_FRAME "Output embedded frame"

/*! TODO:
*/
EmbeddingModule::EmbeddingModule()
	:Module(MODULE_NAME, MODULE_DISPLAY_NAME, EMBEDDINGMODULE_VERSION)
{
	mShortDescription = "Module for embedding a color image into another one";
	mLongDescription = "This module embeds an image into another one using a binary mask";

	mInputSlotBackgroundColorImage = newSlot(new ModuleSlot(this, INPUT_SLOT_NAME_BGIMAGE, INPUT_SLOT_DISPLAYNAME_BGIMAGE, "Backgound image on which embedding with be performed"));
	mInputSlotEmbeddingColorImage = newSlot(new ModuleSlot(this, INPUT_SLOT_NAME_EBDIMAGE, INPUT_SLOT_DISPLAYNAME_EBDIMAGE, "Image that is going to be embedded"));
	mInputSlotEmbeddingMaskImage = newSlot(new ModuleSlot(this, INPUT_SLOT_NAME_EBDMASK, INPUT_SLOT_DISPLAYNAME_EBDMASK, "Mask used to embed image"));

	mOutputSlot = newSlot(new ModuleSlot(this, OUTPUT_SLOT_NAME_FRAME, OUTPUT_SLOT_DISPLAYNAME_FRAME, "Embedded frame", &mOutputFrame));

	mOutputFrame = NULL;
	mOutputFrameIpl = NULL;
}

/*! TODO:
*/
EmbeddingModule::~EmbeddingModule()
{
	if(mOutputFrameIpl)
	{
		cvReleaseImage(&mOutputFrameIpl);
		delete mOutputFrame;
	}
}

/*! TODO:
*/
void EmbeddingModule::initFunction()
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
void EmbeddingModule::startFunction()
{

}

/*! TODO:
*/
void EmbeddingModule::pauseFunction()
{

}

/*! TODO:
*/
void EmbeddingModule::processFunction(unsigned int inFrameNumber)
{
  const Image* lBackgroundColorImage;
  const Image* lEmbeddingColorImage;
  const Image* lEmbeddingMaskImage;
	IplImage* lBackgroundColorImageIpl;
	IplImage* lEmbeddingColorImageIpl;
	IplImage* lEmbeddingMaskImageIpl;

	unsigned int lWidth;
	unsigned int lHeight;
	Image::Channel lChannels;
	Image::Depth lDepth;

	if(!mInputSlotBackgroundColorImage->isConnected())
		throw(Exception(Exception::eCodeUseModule, mInputSlotBackgroundColorImage->getFullName().c_str() + string(" is not connected to an output slot") ));
	if(!mInputSlotEmbeddingColorImage->isConnected())
		throw(Exception(Exception::eCodeUseModule, mInputSlotEmbeddingColorImage->getFullName().c_str() + string(" is not connected to an output slot") ));
	if(!mInputSlotEmbeddingMaskImage->isConnected())
		throw(Exception(Exception::eCodeUseModule, mInputSlotEmbeddingMaskImage->getFullName().c_str() + string(" is not connected to an output slot") ));

	mInputSlotBackgroundColorImage->lock();
	mInputSlotEmbeddingColorImage->lock();
	mInputSlotEmbeddingMaskImage->lock();
	mOutputSlot->lock();

	lBackgroundColorImage = mInputSlotBackgroundColorImage->getImage();
	lEmbeddingColorImage = mInputSlotEmbeddingColorImage->getImage();
	lEmbeddingMaskImage = mInputSlotEmbeddingMaskImage->getImage();

	// Check validity of image
	if(!lBackgroundColorImage)
	{
		mOutputSlot->unlock();
		mInputSlotEmbeddingMaskImage->unlock();
		mInputSlotEmbeddingColorImage->unlock();
		mInputSlotBackgroundColorImage->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotBackgroundColorImage->getFullName().c_str() + string(" does not have a valid image pointer (NULL)") ));
	}
	if(!lEmbeddingColorImage)
	{
		mOutputSlot->unlock();
		mInputSlotEmbeddingMaskImage->unlock();
		mInputSlotEmbeddingColorImage->unlock();
		mInputSlotBackgroundColorImage->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotEmbeddingColorImage->getFullName().c_str() + string(" does not have a valid image pointer (NULL)") ));
	}
	if(!lEmbeddingMaskImage)
	{
		mOutputSlot->unlock();
		mInputSlotEmbeddingMaskImage->unlock();
		mInputSlotEmbeddingColorImage->unlock();
		mInputSlotBackgroundColorImage->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotEmbeddingMaskImage->getFullName().c_str() + string(" does not have a valid image pointer (NULL)") ));
	}

	lWidth = lBackgroundColorImage->getWidth();
	lHeight = lBackgroundColorImage->getHeight();
	lChannels = lBackgroundColorImage->getNbChannels();
	lDepth = lBackgroundColorImage->getDepth();

	//Verify image depth and number of channels
	if(lWidth!=lEmbeddingColorImage->getWidth() || lHeight!=lEmbeddingColorImage->getHeight())
	{
		mOutputSlot->unlock();
		mInputSlotEmbeddingMaskImage->unlock();
		mInputSlotEmbeddingColorImage->unlock();
		mInputSlotBackgroundColorImage->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotBackgroundColorImage->getFullName().c_str() + string(" and ") + mInputSlotEmbeddingColorImage->getFullName().c_str() + string(" must have the same dimension") ));
	}

	//Verify image depth and number of channels
	if(lDepth!=lEmbeddingColorImage->getDepth())
	{
		mOutputSlot->unlock();
		mInputSlotEmbeddingMaskImage->unlock();
		mInputSlotEmbeddingColorImage->unlock();
		mInputSlotBackgroundColorImage->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotBackgroundColorImage->getFullName().c_str() + string(" and ") + mInputSlotEmbeddingColorImage->getFullName().c_str() + string(" must have the same depth") ));
	}

	//Verify image depth and number of channels
	if(lChannels!=lEmbeddingColorImage->getNbChannels())
	{
		mOutputSlot->unlock();
		mInputSlotEmbeddingMaskImage->unlock();
		mInputSlotEmbeddingColorImage->unlock();
		mInputSlotBackgroundColorImage->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotBackgroundColorImage->getFullName().c_str() + string(" and ") + mInputSlotEmbeddingColorImage->getFullName().c_str() + string(" must have the same number of channels") ));
	}

	//Verify image depth and number of channels
	if(lWidth!=lEmbeddingMaskImage->getWidth() || lHeight!=lEmbeddingMaskImage->getHeight())
	{
		mOutputSlot->unlock();
		mInputSlotEmbeddingMaskImage->unlock();
		mInputSlotEmbeddingColorImage->unlock();
		mInputSlotBackgroundColorImage->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotBackgroundColorImage->getFullName().c_str() + string(" and ") + mInputSlotEmbeddingMaskImage->getFullName().c_str() + string(" must have the same dimension") ));
	}

	if(lEmbeddingMaskImage->getNbChannels()!=Image::eChannel1 || lEmbeddingMaskImage->getDepth()!=Image::eDepth8U)
	{
		mOutputSlot->unlock();
		mInputSlotEmbeddingMaskImage->unlock();
		mInputSlotEmbeddingColorImage->unlock();
		mInputSlotBackgroundColorImage->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlotEmbeddingMaskImage->getFullName().c_str() + string(" must be a unsigned 8 bits 1 channel image") ));
	}

	setToIplImage(*lBackgroundColorImage, &lBackgroundColorImageIpl);
	setToIplImage(*lEmbeddingColorImage, &lEmbeddingColorImageIpl);
	setToIplImage(*lEmbeddingMaskImage, &lEmbeddingMaskImageIpl);

	// Create output image
	if(!mOutputFrameIpl)
	{
		mOutputFrameIpl = cvCloneImage(lBackgroundColorImageIpl);
		mOutputFrame = new Image(false);
		setFromIplImage(mOutputFrameIpl, *mOutputFrame);
		mOutputFrame->setModel(lBackgroundColorImage->getModel());
	}
	else
	{
		if(mOutputFrameIpl->width!=lWidth || mOutputFrameIpl->height!=lHeight || mOutputFrameIpl->nChannels!=lChannels || mOutputFrameIpl->depth!=lDepth)
		{
			cvReleaseImage(&mOutputFrameIpl);
			delete mOutputFrame;
			mOutputFrameIpl = cvCloneImage(lBackgroundColorImageIpl);
	    mOutputFrame = new Image(false);
	    setFromIplImage(mOutputFrameIpl, *mOutputFrame);
	    mOutputFrame->setModel(lBackgroundColorImage->getModel());
		}
		else
		{
			cvCopy(lBackgroundColorImageIpl, mOutputFrameIpl);
		}
	}

	// Embed image
	cvCopy(lEmbeddingColorImageIpl, mOutputFrameIpl, lEmbeddingMaskImageIpl);

	cvReleaseImageHeader(&lBackgroundColorImageIpl);
	cvReleaseImageHeader(&lEmbeddingColorImageIpl);
	cvReleaseImageHeader(&lEmbeddingMaskImageIpl);

	mOutputSlot->unlock();
	mInputSlotEmbeddingMaskImage->unlock();
	mInputSlotEmbeddingColorImage->unlock();
	mInputSlotBackgroundColorImage->unlock();

}

/*! TODO:
*/
void EmbeddingModule::stopFunction()
{

}

/*! TODO:
*/
void EmbeddingModule::resetFunction()
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
void EmbeddingModule::updateParametersFunction()
{

}

/*!
*/
string EmbeddingModule::verifyParameterFunction(const Parameter& inParameter) const throw()
{
	string lResult = "";

	return lResult;
}

//Create entry points and modules functions
VIPERS_MODULE_ENTRY_POINT
VIPERS_MODULE_FUNCTIONS(EmbeddingModule)
