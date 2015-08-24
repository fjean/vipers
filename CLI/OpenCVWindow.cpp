/*
 *  Video and Image Processing Environment for Real-time Systems CLI (VIPERSCLI)
 *  Copyright (C) 2009 by Frederic Jean
 *
 *  VIPERSCLI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, either version 3 of the License,
 *  or (at your option) any later version.
 *
 *  VIPERSCLI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with VIPERSCLI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Contact:
 *  Computer Vision and Systems Laboratory
 *  Departement of Electrical and Computer Engineering
 *  Universite Laval, Quebec, Canada, G1V 0A6
 *  http://vision.gel.ulaval.ca
 *
 */

 /*!
 * \file OpenCVWindow.cpp
 * \brief OpenCVWindow class functions definition.
 * \author Frederic Jean
 * $Revision: 261 $
 * $Date: 2009-10-18 16:02:27 -0400 (Sun, 18 Oct 2009) $
 */

#include "OpenCVWindow.hpp"
#include <Converter.hpp>
#include <sstream>
#include <highgui.h>

#define VIPERS_UTILS_OPENCV
#include <ImageUtils.hpp>

using namespace VIPERS;
using namespace std;

OpenCVWindow::TrackbarModuleParameter::TrackbarModuleParameter(Module* inModule, const string& inParamName, int inCurrentVal, int inNbSteps, double inMaxValue)
{
	mModule = inModule;
	mParamName = inParamName.c_str();
	mParamLastValue = inCurrentVal;
	mNbSteps = inNbSteps;
	mMaxValue = inMaxValue;
}

/*! \todo
*/
OpenCVWindow::OpenCVWindow()
{
	mImage = NULL;
	mAutosize = true;
}

/*! \todo
*/
OpenCVWindow::~OpenCVWindow()
{
	if(!mWindowName.empty())
		cvDestroyWindow(mWindowName.c_str());
	if(mImage)
		cvReleaseImage(&mImage);
}

/*! \todo
*/
void OpenCVWindow::create(bool inAutoSize)
{
	string lCurrentSlotFullName;
	ostringstream lTmpNameStr;
	unsigned int lNumber = 0;

	mAutosize = inAutoSize;

	Module* lTmpModule = getModule();

	if(!lTmpModule)
		throw(Exception(Exception::eCodeUseMonitor, "Not attached to any module"));

	try
	{
		lCurrentSlotFullName = lTmpModule->getLabel();
	}
	catch(...)
	{
		return;
	}

	if(mModuleSlotFullName != lCurrentSlotFullName)
	{
		if(mWindowName!="")
			cvDestroyWindow(mWindowName.c_str());

		mModuleSlotFullName = lCurrentSlotFullName;

		lTmpNameStr << lCurrentSlotFullName << "-window" << lNumber;
		while(cvGetWindowHandle(lTmpNameStr.str().c_str()) != NULL )
		{
			lNumber +=1;
			lTmpNameStr.str("");
			lTmpNameStr << lCurrentSlotFullName << "-window" << lNumber;
		}

		mWindowName = lTmpNameStr.str().c_str();
		cvNamedWindow(mWindowName.c_str(), mAutosize);
	}

}

/*! \todo
*/
void OpenCVWindow::update()
{
	string lCurrentSlotFullName;
	ostringstream lTmpNameStr;
	unsigned int lNumber = 0;
	TrackbarModuleParameterList::iterator lTrackbarItr;
	int lTmpValue;

	if(!isMonitoring())
		throw(Exception(Exception::eCodeUseMonitor, "No module and/or module slot is monitored"));

	try
	{
		lCurrentSlotFullName = getModule()->getLabel();
	}
	catch(...)
	{
		return;
	}

	if(mModuleSlotFullName != lCurrentSlotFullName)
	{
		if(mWindowName!="")
			cvDestroyWindow(mWindowName.c_str());

		mModuleSlotFullName = lCurrentSlotFullName;

		lTmpNameStr << lCurrentSlotFullName << "-window" << lNumber;
		while(cvGetWindowHandle(lTmpNameStr.str().c_str()) != NULL )
		{
			lNumber +=1;
			lTmpNameStr.str("");
			lTmpNameStr << lCurrentSlotFullName << "-window" << lNumber;
		}

		mWindowName = lTmpNameStr.str().c_str();
		cvNamedWindow(mWindowName.c_str(), mAutosize);
	}

	try
	{
		lockModuleSlot();

		const Image* lTmpImg = getModuleSlotImage();
		IplImage* lTmpImgIpl;
		setToIplImage(*lTmpImg, &lTmpImgIpl);

		if(!mImage)
			mImage = cvCloneImage(lTmpImgIpl);
		else if(mImage->width!=lTmpImgIpl->width || mImage->height!=lTmpImgIpl->height || mImage->nChannels!=lTmpImgIpl->nChannels || mImage->depth!=lTmpImgIpl->depth)
		{
			cvReleaseImage(&mImage);
			mImage = cvCloneImage(lTmpImgIpl);
		}
		else
			cvCopy(lTmpImgIpl, mImage);

		cvReleaseImageHeader(&lTmpImgIpl);

		unlockModuleSlot();

		cvShowImage(mWindowName.c_str(), mImage);
	}
	catch(...)
	{
		throw;
	}

	for(lTrackbarItr=mTrackbarModuleParameterList.begin(); lTrackbarItr!=mTrackbarModuleParameterList.end(); lTrackbarItr++)
	{
		lTmpValue = cvGetTrackbarPos(lTrackbarItr->mParamName.c_str(), mWindowName.c_str());

		if(lTmpValue!=lTrackbarItr->mParamLastValue)
		{
			lTrackbarItr->mParamLastValue = lTmpValue;
			if(lTrackbarItr->mNbSteps)
			{
				lTrackbarItr->mModule->setParameterValue(lTrackbarItr->mParamName.c_str(), string(Converter(lTmpValue*(lTrackbarItr->mMaxValue/lTrackbarItr->mNbSteps))));
			}
			else
			{
				lTrackbarItr->mModule->setParameterValue(lTrackbarItr->mParamName.c_str(), string(Converter(lTmpValue)));
			}
		}
	}

}

/*! \todo
*/
string OpenCVWindow::getWindowName() const throw()
{
	return mWindowName.c_str();
}

/*! \todo
*/
void OpenCVWindow::addTrackbar(Module* inModule, const string& inParamName, double inMaxValue,  int inSteps)
{
	Module* lTmpModule = getModule();
	Parameter* lParameter = NULL;
	Variable::VariableType lType;
	double lMaxValue = inMaxValue;

	if(!lTmpModule)
		throw(Exception(Exception::eCodeUseMonitor, "Not attached to any module"));

	if(!inModule)
		throw(Exception(Exception::eCodeUseMonitor, "Empty module provided"));

	try
	{
		lParameter = new Parameter(inModule->getParameter(inParamName));
	}
	catch(...)
	{
		throw;
	}

	if(lParameter->getMaxValue()!="")
	{
		istringstream lTmpIntStr(lParameter->getMaxValue());
		lTmpIntStr >> lMaxValue;
	}

	if(inSteps)
	{
		mTrackBarPosition = (int)(lParameter->toDouble()*inSteps);
		mTrackbarModuleParameterList.push_back(TrackbarModuleParameter(inModule, inParamName.c_str(), mTrackBarPosition, inSteps, lMaxValue));
		cvCreateTrackbar(inParamName.c_str(), mWindowName.c_str(), &mTrackBarPosition, (int)(lMaxValue*inSteps), setTrackbarPosition);
	}
	else
	{
		mTrackBarPosition = lParameter->toInt();
		mTrackbarModuleParameterList.push_back(TrackbarModuleParameter(inModule, inParamName.c_str(), mTrackBarPosition, inSteps));
		cvCreateTrackbar(inParamName.c_str(), mWindowName.c_str(), &mTrackBarPosition, (int)lMaxValue, setTrackbarPosition);
	}


}

/*! \todo
*/
void OpenCVWindow::setTrackbarPosition(int inPos)
{

}
