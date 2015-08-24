/*
 *  Video and Image Processing Environment for Real-time Systems (VIPERS)
 *
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
 * \file ImageWriter.cpp
 * \brief ImageWriterModule class functions definition.
 * \author Frederic Jean
 * $Revision: 334 $
 * $Date: 2011-06-13 17:45:53 -0700 (Mon, 13 Jun 2011) $
 */

#include "ImageWriter.hpp"
#include "ImageWriterConfig.hpp"

#define VIPERS_UTILS_OPENCV
#include <ImageUtils.hpp>

#include <highgui.h>

using namespace VIPERS;
using namespace std;

#define MODULE_NAME "imagewriter"
#define MODULE_DISPLAY_NAME "Image writer"

#define INPUT_SLOT_NAME_IMAGE "input-image"
#define INPUT_SLOT_DISPLAYNAME_IMAGE "Image"

#define PARAMETER_NAME_IMAGE_FILE "image-file"

/*! TODO:
*/
ImageWriterModule::ImageWriterModule()
	:Module(MODULE_NAME, MODULE_DISPLAY_NAME, IMAGEWRITERMODULE_VERSION),
	mParamImageFile(PARAMETER_NAME_IMAGE_FILE, Variable::eVariableTypeFile, "Output file", "Output file for the image", false)
{
	mShortDescription = "Module for saving an image to a file";
	mLongDescription = "This module saves an image to a file";

	newParameter(mParamImageFile);

	mInputSlot = newSlot(new ModuleSlot(this, INPUT_SLOT_NAME_IMAGE, INPUT_SLOT_DISPLAYNAME_IMAGE, "Image to write to specified file"));

}

/*! TODO:
*/
ImageWriterModule::~ImageWriterModule()
{

}

/*! TODO:
*/
void ImageWriterModule::initFunction()
{
	const Image* lTmpImage = NULL;
	IplImage* lTmpImageIpl = NULL;

	lockParameters();
	mParamImageFile = getLockedParameter(PARAMETER_NAME_IMAGE_FILE);
	unlockParameters();

	if(!mInputSlot->isConnected())
		throw(Exception(Exception::eCodeUseModule, mInputSlot->getFullName().c_str() + string(" is not connected to an output slot") ));

	mInputSlot->lock();

	lTmpImage = mInputSlot->getImage();
	if(!lTmpImage)
	{
		mInputSlot->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlot->getFullName().c_str() + string(" does not have a valid image pointer (NULL)") ));
	}

	setToIplImage(*lTmpImage, &lTmpImageIpl);

	cvSaveImage(mParamImageFile.toString().c_str(), lTmpImageIpl);
	
	/*
	if(!cvSaveImage())
	{
		mInputSlot->unlock();
		throw(Exception(Exception::eCodeUseModule, string("Module \"") + getLabel().c_str() + string("\" encountered an error while writing file \"") + mParamImageFile.toString().c_str() + string("\"; Not a supported image file, or file cannot be written.") ));
	}
	*/
	
	cvReleaseImageHeader(&lTmpImageIpl);
	mInputSlot->unlock();

}

/*! TODO:
*/
void ImageWriterModule::startFunction()
{

}

/*! TODO:
*/
void ImageWriterModule::pauseFunction()
{

}

/*! TODO:
*/
void ImageWriterModule::processFunction(unsigned int inFrameNumber)
{

}

/*! TODO:
*/
void ImageWriterModule::stopFunction()
{

}

/*! TODO:
*/
void ImageWriterModule::resetFunction()
{

}

/*!
*/
void ImageWriterModule::updateParametersFunction()
{
	// Nothing to do for Testing module parameters, since no parameter has influence on other parameters
}

/*!
*/
string ImageWriterModule::verifyParameterFunction(const Parameter& inParameter) const throw()
{
	string lResult = "";

	return lResult;
}

//Create entry points and modules functions
VIPERS_MODULE_ENTRY_POINT
VIPERS_MODULE_FUNCTIONS(ImageWriterModule)
