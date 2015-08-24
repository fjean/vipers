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
 * \file BgColor.cpp
 * \brief BgColorModule class functions definition.
 * \author Frederic Jean
 * $Revision: 333 $
 * $Date: 2011-05-20 10:48:09 -0700 (Fri, 20 May 2011) $
 */

#include "BgColor.hpp"
#include "BgColorConfig.hpp"

#include <iostream>

#define VIPERS_UTILS_OPENCV
#include <ImageUtils.hpp>

using namespace VIPERS;
using namespace std;

#define MODULE_NAME "bgcolor"
#define MODULE_DISPLAY_NAME "Background color"

#define OUTPUT_SLOT_NAME_BACKGROUND_IMAGE "output-background"
#define OUTPUT_SLOT_DISPLAYNAME_BACKGROUND_IMAGE "Background color image"

#define PARAMETER_NAME_IMSIZE "image-size"
#define PARAMETER_NAME_IMCOLOR "image-color"
#define PARAMETER_NAME_GENERATE_RANDOM_COLOR "generate-random-color"
#define PARAMETER_NAME_COLOR_CHANGE_DELAY "color-change-delay"

/*! TODO:
*/
BgColorModule::BgColorModule()
	:Module(MODULE_NAME, MODULE_DISPLAY_NAME, BGCOLORMODULE_VERSION)
{
	mShortDescription = "Module for generating uniform background image";
	mLongDescription = "This module generates uniform background image, with a specified color, or random colors generated using a specified delay.";

	mParamImageSize = new Parameter(PARAMETER_NAME_IMSIZE, Variable::eVariableTypeSize, "Size", "Image size", false);
	mParamImageColor = new Parameter(PARAMETER_NAME_IMCOLOR, Variable::eVariableTypeColor, "Color", "Image background color", true);
	mParamGenerateRandomColor = new Parameter(PARAMETER_NAME_GENERATE_RANDOM_COLOR, Variable::eVariableTypeBool, "Random color", "Generate random background color", true);
	mParamColorChangeDelay = new Parameter(PARAMETER_NAME_COLOR_CHANGE_DELAY, Variable::eVariableTypeDouble, "Delay", "Random color change delay (in secondes)", true);

	mParamImageSize->setValueStr("640 480");
	mParamImageColor->setValueStr("255 0 0");
	mParamGenerateRandomColor->setValue(false);
	mParamColorChangeDelay->setValue(0.0);

  NameSet lDependentParameterSet;
  lDependentParameterSet.insert(PARAMETER_NAME_COLOR_CHANGE_DELAY);
  lDependentParameterSet.insert(PARAMETER_NAME_IMCOLOR);
  mParamGenerateRandomColor->setDependentParameterNameSet(lDependentParameterSet);
  mParamColorChangeDelay->setEnabled(false);

	mParamImageSize->setMaxValue("4096 4096");
	mParamColorChangeDelay->setMinValue("0");
	mParamColorChangeDelay->setMaxValue("10000");

	newParameter(*mParamImageSize);
	newParameter(*mParamImageColor);
	newParameter(*mParamGenerateRandomColor);
	newParameter(*mParamColorChangeDelay);

	mOutputSlot = newSlot(new ModuleSlot(this, OUTPUT_SLOT_NAME_BACKGROUND_IMAGE, OUTPUT_SLOT_DISPLAYNAME_BACKGROUND_IMAGE, "Image of a speficied with with a specified background color, or random color", &mOutputImage));

	mRandomizer = new Randomizer();

	mOutputImage = NULL;
	mOutputImageIpl = NULL;
}

/*! TODO:
*/
BgColorModule::~BgColorModule()
{
	delete mParamImageSize;
	delete mParamImageColor;
	delete mParamGenerateRandomColor;
	delete mParamColorChangeDelay;

	delete mRandomizer;

	if(mOutputImageIpl)
	{
		cvReleaseImage(&mOutputImageIpl);
    delete mOutputImage;
	}
}

/*! TODO:
*/
void BgColorModule::initFunction()
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
void BgColorModule::startFunction()
{
	mTimer.reset();
}

/*! TODO:
*/
void BgColorModule::pauseFunction()
{

}

/*! TODO:
*/
void BgColorModule::processFunction(unsigned int inFrameNumber)
{
	Size lSize = getParameter(PARAMETER_NAME_IMSIZE).toSize();
	Color lColor = getParameter(PARAMETER_NAME_IMCOLOR).toColor();
	bool lRandomColor = getParameter(PARAMETER_NAME_GENERATE_RANDOM_COLOR).toBool();
	double lChangeDelay = getParameter(PARAMETER_NAME_COLOR_CHANGE_DELAY).toDouble();

	bool lNeedRedraw = false;

	mOutputSlot->lock();

	if( *mParamImageSize != lSize)
	{
		mParamImageSize->setValue(lSize);
		if(mOutputImageIpl)
		{
			cvReleaseImage(&mOutputImageIpl);
		  delete mOutputImage;
		}

		mOutputImageIpl = cvCreateImage(cvSize(lSize[0], lSize[1]), IPL_DEPTH_8U, 3);
		mOutputImage = new Image(false);
		setFromIplImage(mOutputImageIpl, *mOutputImage);
		mOutputImage->setModel(Image::eModelRGB);

		lNeedRedraw = true;
	}

	if(*mParamGenerateRandomColor != lRandomColor)
		mParamGenerateRandomColor->setValue(lRandomColor);

	if(*mParamImageColor != lColor)
	{
		mParamImageColor->setValue(lColor);
		lNeedRedraw = true;
	}

	if(*mParamColorChangeDelay != lChangeDelay)
		mParamColorChangeDelay->setValue(lChangeDelay);

	if(lRandomColor)
	{
		if(mTimer.getValue() > lChangeDelay)
		{
			cvSet(mOutputImageIpl, cvScalar(mRandomizer->getInteger(255), mRandomizer->getInteger(255), mRandomizer->getInteger(255)));
			mTimer.reset();
		}
	}
	else
	{
		if(lNeedRedraw)
			cvSet(mOutputImageIpl, cvScalar(lColor[2], lColor[1], lColor[0]));
	}


	mOutputSlot->unlock();

}

/*! TODO:
*/
void BgColorModule::stopFunction()
{
	// Nothing to do
}

/*! TODO:
*/
void BgColorModule::resetFunction()
{
	if(mOutputImage)
	{
		mOutputSlot->lock();
		cvReleaseImage(&mOutputImageIpl);
		mOutputImageIpl = NULL;
		delete mOutputImage;
		mOutputImage = NULL;
		mOutputSlot->unlock();
	}
}

/*!
*/
void BgColorModule::updateParametersFunction()
{
  lockParameters();

  bool lRandomColor = getLockedParameter(PARAMETER_NAME_GENERATE_RANDOM_COLOR).toBool();

  mParamGenerateRandomColor->setValue(lRandomColor);
  mParamColorChangeDelay->setEnabled(lRandomColor);
  mParamImageColor->setEnabled(!lRandomColor);

  setLockedParameter(*mParamColorChangeDelay);
  setLockedParameter(*mParamImageColor);

  unlockParameters();
}

/*!
*/
string BgColorModule::verifyParameterFunction(const Parameter& inParameter) const throw()
{
	string lResult = "";

	return lResult;
}

//Create entry points and modules functions
VIPERS_MODULE_ENTRY_POINT
VIPERS_MODULE_FUNCTIONS(BgColorModule)
