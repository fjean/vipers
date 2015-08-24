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
 * \file OpticalFlow.cpp
 * \brief DistanceTransformModule class functions definition.
 * \author Frederic Jean
 * $Revision: 283 $
 * $Date: 2009-10-28 21:58:55 -0400 (Wed, 28 Oct 2009) $
 */

#include "OpticalFlow.hpp"
#include "OpticalFlowConfig.hpp"
#include <cmath>

#define VIPERS_UTILS_OPENCV
#include <ImageUtils.hpp>

using namespace VIPERS;
using namespace std;

#define MODULE_NAME "opticalflow"
#define MODULE_DISPLAY_NAME "Optical Flow"

#define INPUT_SLOT_NAME "input-gray-image"
#define INPUT_SLOT_DISPLAYNAME "Gray image"
#define OUTPUT_SLOT_NAME_VEL_X "output-velx"
#define OUTPUT_SLOT_DISPLAYNAME_VEL_X "X velocity"
#define OUTPUT_SLOT_NAME_VEL_Y "output-vely"
#define OUTPUT_SLOT_DISPLAYNAME_VEL_Y "Y velocity"
#define OUTPUT_SLOT_NAME_VEL_NORM "output-vel-norm"
#define OUTPUT_SLOT_DISPLAYNAME_VEL_NORM "Velocity norm"
#define OUTPUT_SLOT_NAME_VEL_NORM_GRAY "output-vel-norm-gray"
#define OUTPUT_SLOT_DISPLAYNAME_VEL_NORM_GRAY "Velocity norm gray"
#define OUTPUT_SLOT_NAME_VEL_VECTORS "output-vel-vectors"
#define OUTPUT_SLOT_DISPLAYNAME_VEL_VECTORS "Velocity vectors"

#define PARAMETER_NAME_ALGORITHM "algorithm"
#define PARAMETER_DISPLAYNAME_ALGORITHM "Algorithm"
#define PARAMETER_NAME_BLOCK_SIZE "block-size"
#define PARAMETER_DISPLAYNAME_BLOCK_SIZE "Block size"
#define PARAMETER_NAME_SHIFT_SIZE "shift-size"
#define PARAMETER_DISPLAYNAME_SHIFT_SIZE "Shift size"
#define PARAMETER_NAME_MAX_RANGE "max-range"
#define PARAMETER_DISPLAYNAME_MAX_RANGE "Maximum range"
#define PARAMETER_NAME_USE_PREVIOUS "use-previous"
#define PARAMETER_DISPLAYNAME_USE_PREVIOUS "Use previous velocities"
#define PARAMETER_NAME_LAMBDA "lambda"
#define PARAMETER_DISPLAYNAME_LAMBDA "Lagrangian multiplier"
#define PARAMETER_NAME_CRITERION_TYPE "criterion-type"
#define PARAMETER_DISPLAYNAME_CRITERION_TYPE "Termination criterion type"
#define PARAMETER_NAME_CRITERION_MAX_ITER "criterion-max-iter"
#define PARAMETER_DISPLAYNAME_CRITERION_MAX_ITER "Maximum iteration"
#define PARAMETER_NAME_CRITERION_EPSILON "criterion-epsilon"
#define PARAMETER_DISPLAYNAME_CRITERION_EPSILON "Epsilon"

/*! TODO:
*/
OpticalFlowModule::OpticalFlowModule()
	:Module(MODULE_NAME, MODULE_DISPLAY_NAME, OPTICALFLOWMODULE_VERSION),
	mParamAlgorithm(PARAMETER_NAME_ALGORITHM, Variable::eVariableTypeString, PARAMETER_DISPLAYNAME_ALGORITHM, "Optical flow algorithm", false),
	mParamBlockSize(PARAMETER_NAME_BLOCK_SIZE, Variable::eVariableTypeSize, PARAMETER_DISPLAYNAME_BLOCK_SIZE, "Size of the compared block (BM); Size of averaging window (LK)", false),
	mParamShiftSize(PARAMETER_NAME_SHIFT_SIZE, Variable::eVariableTypeSize, PARAMETER_DISPLAYNAME_SHIFT_SIZE, "Block coordinate increments", false),
  mParamMaxRange(PARAMETER_NAME_MAX_RANGE, Variable::eVariableTypeSize, PARAMETER_DISPLAYNAME_MAX_RANGE, "Size of the scanned neighborhood in pixels around the block", false),
  mParamUsePrevious(PARAMETER_NAME_USE_PREVIOUS, Variable::eVariableTypeBool, PARAMETER_DISPLAYNAME_USE_PREVIOUS, "Uses the previous velocity field", true),
  mParamLambda(PARAMETER_NAME_LAMBDA, Variable::eVariableTypeDouble, PARAMETER_DISPLAYNAME_LAMBDA, "The Lagrangian multiplier in the Horn & Schunck algorithm", true),
  mParamCriterionType(PARAMETER_NAME_CRITERION_TYPE, Variable::eVariableTypeString, PARAMETER_DISPLAYNAME_CRITERION_TYPE, "The type of termination criterion", true),
  mParamCriterionMaxIter(PARAMETER_NAME_CRITERION_MAX_ITER, Variable::eVariableTypeUInt, PARAMETER_DISPLAYNAME_CRITERION_MAX_ITER, "Maximum number of iteration (termination criterion)", true),
  mParamCriterionEpsilon(PARAMETER_NAME_CRITERION_EPSILON, Variable::eVariableTypeDouble, PARAMETER_DISPLAYNAME_CRITERION_EPSILON, "Epsilon (termination criterion)", true)
{
	mShortDescription = "Module for computing optical flow";
	mLongDescription = "This module computes optical flow from a gray scale image";

	ValueSet lAlgoValues;
	lAlgoValues.insert(Value("bm", "Block Matching", "Block matching algorithm"));
	lAlgoValues.insert(Value("hs", "Horn Schunck", "The Horn & Schunck algorithm"));
	lAlgoValues.insert(Value("lk", "Lucas Kanade", "The Lucas & Kanade algorithm"));

	ValueSet lCriterionValues;
	lCriterionValues.insert(Value("max-iter", "Maximum iteration", "Maximum number of iteration (termination criterion)"));
	lCriterionValues.insert(Value("epsilon", "Epsilon", "Epsilon value (termination criterion)"));
	lCriterionValues.insert(Value("both", "Both", "Both maximum iteration and epsilon (termination criteria)"));

  NameSet lAlgorithmDependentParameterSet;
  lAlgorithmDependentParameterSet.insert(PARAMETER_NAME_BLOCK_SIZE);
  lAlgorithmDependentParameterSet.insert(PARAMETER_NAME_SHIFT_SIZE);
  lAlgorithmDependentParameterSet.insert(PARAMETER_NAME_MAX_RANGE);
  lAlgorithmDependentParameterSet.insert(PARAMETER_NAME_USE_PREVIOUS);
  lAlgorithmDependentParameterSet.insert(PARAMETER_NAME_LAMBDA);
  lAlgorithmDependentParameterSet.insert(PARAMETER_NAME_CRITERION_TYPE);
  lAlgorithmDependentParameterSet.insert(PARAMETER_NAME_CRITERION_MAX_ITER);
  lAlgorithmDependentParameterSet.insert(PARAMETER_NAME_CRITERION_EPSILON);

  NameSet lCriterionTypeDependentParameterSet;
  lCriterionTypeDependentParameterSet.insert(PARAMETER_NAME_CRITERION_MAX_ITER);
  lCriterionTypeDependentParameterSet.insert(PARAMETER_NAME_CRITERION_EPSILON);

  mCriterionTypeMap["both"] = CV_TERMCRIT_ITER + CV_TERMCRIT_EPS;
  mCriterionTypeMap["max-iter"] = CV_TERMCRIT_ITER;
  mCriterionTypeMap["epsilon"] = CV_TERMCRIT_EPS;

	mParamAlgorithm.setPossibleValues(lAlgoValues);
	mParamCriterionType.setPossibleValues(lCriterionValues);
  mParamAlgorithm.setDependentParameterNameSet(lAlgorithmDependentParameterSet);
  mParamCriterionType.setDependentParameterNameSet(lCriterionTypeDependentParameterSet);

	mParamAlgorithm.setValueStr("bm");
	mParamBlockSize.setValue(Size(15,15));
	mParamBlockSize.setMinValue("1 1");
  mParamShiftSize.setValue(Size(1,1));
  mParamShiftSize.setMinValue("1 1");
  mParamMaxRange.setValue(Size(1,1));
  mParamMaxRange.setMinValue("1 1");
  mParamUsePrevious.setValue(false);
  mParamLambda.setValue(0.5);
  mParamLambda.setEnabled(false);
  mParamCriterionType.setValue("both");
  mParamCriterionType.setEnabled(false);
  mParamCriterionMaxIter.setValue(20);
  mParamCriterionMaxIter.setEnabled(false);
  mParamCriterionEpsilon.setValue(1.0e-6);
  mParamCriterionEpsilon.setEnabled(false);

	newParameter(mParamAlgorithm);
  newParameter(mParamBlockSize);
  newParameter(mParamShiftSize);
  newParameter(mParamMaxRange);
  newParameter(mParamUsePrevious);
  newParameter(mParamLambda);
  newParameter(mParamCriterionType);
  newParameter(mParamCriterionMaxIter);
  newParameter(mParamCriterionEpsilon);

	mInputSlot = newSlot(new ModuleSlot(this, INPUT_SLOT_NAME, INPUT_SLOT_DISPLAYNAME, "Input gray image used to compute optical flow"));
	mOutputSlotVelX = newSlot(new ModuleSlot(this, OUTPUT_SLOT_NAME_VEL_X, OUTPUT_SLOT_DISPLAYNAME_VEL_X, "Output X velocity (float image)", &mVelX));
	mOutputSlotVelY = newSlot(new ModuleSlot(this, OUTPUT_SLOT_NAME_VEL_Y, OUTPUT_SLOT_DISPLAYNAME_VEL_Y, "Output Y velocity (float image)", &mVelY));
	mOutputSlotVelNorm = newSlot(new ModuleSlot(this, OUTPUT_SLOT_NAME_VEL_NORM, OUTPUT_SLOT_DISPLAYNAME_VEL_NORM, "Output velocity norm (float image)", &mVelNorm));
	mOutputSlotVelNormGray = newSlot(new ModuleSlot(this, OUTPUT_SLOT_NAME_VEL_NORM_GRAY, OUTPUT_SLOT_DISPLAYNAME_VEL_NORM_GRAY, "Output velocity norm (gray image)", &mVelNormGray));
	mOutputSlotVelVectors = newSlot(new ModuleSlot(this, OUTPUT_SLOT_NAME_VEL_VECTORS, OUTPUT_SLOT_DISPLAYNAME_VEL_VECTORS, "Output velocity vectors (vectors are drawn in an image)", &mVelVectors));

  mVelXIpl = NULL;
  mVelYIpl = NULL;
  mVelNormIpl = NULL;
  mVelNormGrayIpl = NULL;
  mVelVectorsIpl = NULL;

  mVelX = NULL;
  mVelY = NULL;
  mVelNorm = NULL;
  mVelNormGray = NULL;
  mVelVectors = NULL;

  mPrevImageIpl = NULL;
  mPrevImage = NULL;

  mTmpImg1 = NULL;
  mTmpImg2 = NULL;

  mMinDist = 1000000;
  mMaxDist = -1000000;
}

/*! TODO:
*/
OpticalFlowModule::~OpticalFlowModule()
{
  if(mPrevImageIpl)
  {
    cvReleaseImage(&mPrevImageIpl);
    delete mPrevImage;
  }
	if(mVelXIpl)
	{
		cvReleaseImage(&mVelXIpl);
		delete mVelX;
	}
  if(mVelYIpl)
  {
    cvReleaseImage(&mVelYIpl);
    delete mVelY;
  }
  if(mVelNormIpl)
  {
    cvReleaseImage(&mVelNormIpl);
    delete mVelNorm;
  }
  if(mVelNormGrayIpl)
  {
    cvReleaseImage(&mVelNormGrayIpl);
    delete mVelNormGray;
  }
  if(mVelVectorsIpl)
  {
    cvReleaseImage(&mVelVectorsIpl);
    delete mVelVectors;
  }

  if(mTmpImg1)
    cvReleaseImage(&mTmpImg1);

  if(mTmpImg2)
    cvReleaseImage(&mTmpImg2);

}

/*! TODO:
*/
void OpticalFlowModule::initFunction()
{
  const Image* lInputImage;
  CvSize lSize;

  if(!mInputSlot->isConnected())
    throw(Exception(Exception::eCodeUseModule, mInputSlot->getFullName().c_str() + string(" is not connected to an output slot") ));

  lockParameters();
  mParamAlgorithm = getLockedParameter(PARAMETER_NAME_ALGORITHM);
  mParamBlockSize = getLockedParameter(PARAMETER_NAME_BLOCK_SIZE);
  mParamShiftSize = getLockedParameter(PARAMETER_NAME_SHIFT_SIZE);
  unlockParameters();

  mInputSlot->lock();

  lInputImage = mInputSlot->getImage();

  if(!lInputImage)
  {
     mInputSlot->unlock();
     throw(Exception(Exception::eCodeUseModule, mInputSlot->getFullName().c_str() + string(" does not have a valid image") ));
  }

  if(lInputImage->getNbChannels()!=Image::eChannel1 || lInputImage->getDepth()!=Image::eDepth8U)
  {
    mInputSlot->unlock();
    throw(Exception(Exception::eCodeUseModule, mInputSlot->getFullName().c_str() + string(" must be a 1 channel gray image") ));
  }

  resetFunction();

  if(mParamAlgorithm.toString()=="bm")
  {
    Size lBlockSize = mParamBlockSize.toSize();
    Size lShiftSize = mParamShiftSize.toSize();
    lSize = cvSize(::floor( ((double)(lInputImage->getWidth()-lBlockSize[0]))/((double)lShiftSize[0]) ),
                  ::floor( ((double)(lInputImage->getHeight()-lBlockSize[1]))/((double)lShiftSize[1]) ) );
  }
  else
  {
    lSize = cvSize(lInputImage->getWidth(), lInputImage->getHeight());
  }

  mOutputSlotVelX->lock();
  mOutputSlotVelY->lock();
  mOutputSlotVelNorm->lock();
  mOutputSlotVelNormGray->lock();
  mOutputSlotVelVectors->lock();

  mPrevImageIpl = cvCreateImage(cvSize(lInputImage->getWidth(), lInputImage->getHeight()), IPL_DEPTH_8U, 1);
  mVelXIpl = cvCreateImage(lSize, IPL_DEPTH_32F, 1);
  mVelYIpl = cvCreateImage(lSize, IPL_DEPTH_32F, 1);
  mVelNormIpl = cvCreateImage(lSize, IPL_DEPTH_32F, 1);
  mVelNormGrayIpl = cvCreateImage(lSize, IPL_DEPTH_8U, 1);
  mVelVectorsIpl = cvCreateImage(lSize, IPL_DEPTH_8U, 3);

  mTmpImg1 = cvCreateImage(lSize, IPL_DEPTH_32F, 1);
  mTmpImg2 = cvCreateImage(lSize, IPL_DEPTH_32F, 1);

  mPrevImage = new Image(false);
  mVelX = new Image(false);
  mVelY = new Image(false);
  mVelNorm = new Image(false);
  mVelNormGray = new Image(false);
  mVelVectors = new Image(false);

  setFromIplImage(mPrevImageIpl, *mPrevImage);
  setFromIplImage(mVelXIpl, *mVelX);
  setFromIplImage(mVelYIpl, *mVelY);
  setFromIplImage(mVelNormIpl, *mVelNorm);
  setFromIplImage(mVelNormGrayIpl, *mVelNormGray);
  setFromIplImage(mVelVectorsIpl, *mVelVectors);

  mOutputSlotVelVectors->unlock();
  mOutputSlotVelNormGray->unlock();
  mOutputSlotVelNorm->unlock();
  mOutputSlotVelY->unlock();
  mOutputSlotVelX->unlock();

  mInputSlot->unlock();

}

/*! TODO:
*/
void OpticalFlowModule::startFunction()
{
  cvSetZero(mPrevImageIpl);
  cvSetZero(mVelXIpl);
  cvSetZero(mVelYIpl);
  mMinDist = 1000000;
  mMaxDist = -1000000;
}

/*! TODO:
*/
void OpticalFlowModule::pauseFunction()
{

}

/*! TODO:
*/
void OpticalFlowModule::processFunction(unsigned int inFrameNumber)
{

	const Image* lInputImage;
	IplImage* lInputImageIpl;
	double lMinDist;
	double lMaxDist;

	if(!mInputSlot->isConnected())
		throw(Exception(Exception::eCodeUseModule, mInputSlot->getFullName().c_str() + string(" is not connected to an output slot") ));

	lockParameters();
  mParamBlockSize = getLockedParameter(PARAMETER_NAME_BLOCK_SIZE);
  mParamShiftSize = getLockedParameter(PARAMETER_NAME_SHIFT_SIZE);
  mParamMaxRange = getLockedParameter(PARAMETER_NAME_MAX_RANGE);
	mParamUsePrevious = getLockedParameter(PARAMETER_NAME_USE_PREVIOUS);
	mParamLambda = getLockedParameter(PARAMETER_NAME_LAMBDA);
	mParamCriterionType = getLockedParameter(PARAMETER_NAME_CRITERION_TYPE);
	mParamCriterionMaxIter = getLockedParameter(PARAMETER_NAME_CRITERION_MAX_ITER);
	mParamCriterionEpsilon = getLockedParameter(PARAMETER_NAME_CRITERION_EPSILON);
	unlockParameters();

	mInputSlot->lock();

	lInputImage = mInputSlot->getImage();

	if(!lInputImage)
	{
	   mInputSlot->unlock();
	   throw(Exception(Exception::eCodeUseModule, mInputSlot->getFullName().c_str() + string(" does not have a valid image") ));
	}

	if(lInputImage->getNbChannels()!=mPrevImage->getNbChannels() || lInputImage->getWidth()!=mPrevImage->getWidth() || lInputImage->getHeight()!=mPrevImage->getHeight() || lInputImage->getDepth()!=mPrevImage->getDepth())
	{
		mInputSlot->unlock();
		throw(Exception(Exception::eCodeUseModule, mInputSlot->getFullName().c_str() + string(" has changed image format since initialization.  Image format (size, depth, nb channels) must not change during processing") ));
	}

	setToIplImage(*lInputImage, &lInputImageIpl);

	mOutputSlotVelX->lock();
	mOutputSlotVelY->lock();

	if(mParamAlgorithm.toString()=="bm")
	{
	  Size lBlockSize = mParamBlockSize.toSize();
	  Size lShiftSize = mParamShiftSize.toSize();
	  Size lMaxRange = mParamMaxRange.toSize();

	  cvCalcOpticalFlowBM(mPrevImageIpl,
                        lInputImageIpl,
                        cvSize(lBlockSize[0], lBlockSize[1]),
                        cvSize(lShiftSize[0], lShiftSize[1]),
                        cvSize(lMaxRange[0], lMaxRange[1]),
                        mParamUsePrevious.toBool(),
                        mVelXIpl,
                        mVelYIpl);
	}
	else if(mParamAlgorithm.toString()=="hs")
	{
    cvCalcOpticalFlowHS(mPrevImageIpl,
                        lInputImageIpl,
                        mParamUsePrevious.toBool(),
                        mVelXIpl,
                        mVelYIpl,
                        mParamLambda.toDouble(),
                        cvTermCriteria(mCriterionTypeMap[mParamCriterionType.toString()], mParamCriterionMaxIter.toUInt(), mParamCriterionEpsilon.toDouble()));
	}
	else if(mParamAlgorithm.toString()=="lk")
	{
    Size lBlockSize = mParamBlockSize.toSize();

    cvCalcOpticalFlowLK(mPrevImageIpl,
                        lInputImageIpl,
                        cvSize(lBlockSize[0], lBlockSize[1]),
                        mVelXIpl,
                        mVelYIpl);
	}

  cvCopy(lInputImageIpl, mPrevImageIpl);
  cvReleaseImageHeader(&lInputImageIpl);
  mInputSlot->unlock();

	// Optionnal outputs
	if(mOutputSlotVelNorm->getUseCount()>0 || mOutputSlotVelNormGray->getUseCount()>0 || mOutputSlotVelVectors->getUseCount()>0)
	{
	  mOutputSlotVelNorm->lock();

	  cvCartToPolar(mVelXIpl, mVelYIpl, mVelNormIpl, mTmpImg1, 0);

	  if(mOutputSlotVelNormGray->getUseCount()>0)
	  {
	    double lMinDist, lMaxDist;

	    mOutputSlotVelNormGray->lock();

	    cvMinMaxLoc(mVelNormIpl, &lMinDist, &lMaxDist);

	    if(lMinDist<mMinDist)
	      mMinDist = lMinDist;
      if(lMaxDist>mMaxDist)
        mMaxDist = lMaxDist;

	    cvConvertScale(mVelNormIpl, mVelNormGrayIpl, 255.0/(mMaxDist-mMinDist), mMinDist*255.0/(mMaxDist-mMinDist));

	    mOutputSlotVelNormGray->unlock();
	  }

    if(mOutputSlotVelVectors->getUseCount()>0)
    {
      mOutputSlotVelVectors->lock();

      mOutputSlotVelVectors->unlock();
    }

    mOutputSlotVelNorm->unlock();
	}

	mOutputSlotVelY->unlock();
	mOutputSlotVelX->unlock();

}

/*! TODO:
*/
void OpticalFlowModule::stopFunction()
{

}

/*! TODO:
*/
void OpticalFlowModule::resetFunction()
{
  if(mPrevImageIpl)
  {
    cvReleaseImage(&mPrevImageIpl);
    delete mPrevImage;
    mPrevImageIpl = NULL;
    mPrevImage = NULL;
  }

	if(mVelXIpl)
	{
	  mOutputSlotVelX->lock();
		cvReleaseImage(&mVelXIpl);
		delete mVelX;
		mVelXIpl = NULL;
		mVelX = NULL;
		mOutputSlotVelX->unlock();
	}
  if(mVelYIpl)
  {
    mOutputSlotVelY->lock();
    cvReleaseImage(&mVelYIpl);
    delete mVelY;
    mVelYIpl = NULL;
    mVelY = NULL;
    mOutputSlotVelY->unlock();
  }
  if(mVelNormIpl)
  {
    mOutputSlotVelNorm->lock();
    cvReleaseImage(&mVelNormIpl);
    delete mVelNorm;
    mVelNormIpl = NULL;
    mVelNorm = NULL;
    mOutputSlotVelNorm->unlock();
  }
  if(mVelNormGrayIpl)
  {
    mOutputSlotVelNormGray->lock();
    cvReleaseImage(&mVelNormGrayIpl);
    delete mVelNormGray;
    mVelNormGrayIpl = NULL;
    mVelNormGray = NULL;
    mOutputSlotVelNormGray->unlock();
  }
  if(mVelVectorsIpl)
  {
    mOutputSlotVelVectors->lock();
    cvReleaseImage(&mVelVectorsIpl);
    delete mVelVectors;
    mVelVectorsIpl = NULL;
    mVelVectors = NULL;
    mOutputSlotVelVectors->unlock();
  }

  if(mTmpImg1)
    cvReleaseImage(&mTmpImg1);

  if(mTmpImg2)
    cvReleaseImage(&mTmpImg2);
}

/*!
*/
void OpticalFlowModule::updateParametersFunction()
{
  lockParameters();

  string lAlgo = getLockedParameter(PARAMETER_NAME_ALGORITHM).toString();
  string lCriterionType = getLockedParameter(PARAMETER_NAME_CRITERION_TYPE).toString();

  if(lAlgo!=mParamAlgorithm.toString())
  {
    mParamAlgorithm.setValueStr(lAlgo.c_str());

    if(mParamAlgorithm.toString()=="bm")
    {
      mParamBlockSize.setEnabled(true);
      mParamShiftSize.setEnabled(true);
      mParamMaxRange.setEnabled(true);
      mParamUsePrevious.setEnabled(true);
      mParamLambda.setEnabled(false);
      mParamCriterionType.setEnabled(false);
      mParamCriterionMaxIter.setEnabled(false);
      mParamCriterionEpsilon.setEnabled(false);
    }
    else if(mParamAlgorithm.toString()=="hs")
    {
      mParamBlockSize.setEnabled(false);
      mParamShiftSize.setEnabled(false);
      mParamMaxRange.setEnabled(false);
      mParamUsePrevious.setEnabled(true);
      mParamLambda.setEnabled(true);
      mParamCriterionType.setEnabled(true);

      if(lCriterionType=="both")
      {
        mParamCriterionMaxIter.setEnabled(true);
        mParamCriterionEpsilon.setEnabled(true);
      }
      else if(lCriterionType=="max-iter")
      {
        mParamCriterionMaxIter.setEnabled(true);
        mParamCriterionEpsilon.setEnabled(false);
      }
      else if(lCriterionType=="epsilon")
      {
        mParamCriterionMaxIter.setEnabled(false);
        mParamCriterionEpsilon.setEnabled(true);
      }

    }
    else if(mParamAlgorithm.toString()=="lk")
    {
      mParamBlockSize.setEnabled(true);
      mParamShiftSize.setEnabled(false);
      mParamMaxRange.setEnabled(false);
      mParamUsePrevious.setEnabled(false);
      mParamLambda.setEnabled(false);
      mParamCriterionType.setEnabled(false);
      mParamCriterionMaxIter.setEnabled(false);
      mParamCriterionEpsilon.setEnabled(false);
    }

    setLockedParameter(mParamBlockSize);
    setLockedParameter(mParamShiftSize);
    setLockedParameter(mParamMaxRange);
    setLockedParameter(mParamUsePrevious);
    setLockedParameter(mParamLambda);
    setLockedParameter(mParamCriterionType);
    setLockedParameter(mParamCriterionMaxIter);
    setLockedParameter(mParamCriterionEpsilon);
  }
  else if(lCriterionType!=mParamCriterionType.toString())
  {
    mParamCriterionType.setValueStr(lCriterionType.c_str());

    if(lCriterionType=="both")
    {
      mParamCriterionMaxIter.setEnabled(true);
      mParamCriterionEpsilon.setEnabled(true);
    }
    else if(lCriterionType=="max-iter")
    {
      mParamCriterionMaxIter.setEnabled(true);
      mParamCriterionEpsilon.setEnabled(false);
    }
    else if(lCriterionType=="epsilon")
    {
      mParamCriterionMaxIter.setEnabled(false);
      mParamCriterionEpsilon.setEnabled(true);
    }

    setLockedParameter(mParamCriterionMaxIter);
    setLockedParameter(mParamCriterionEpsilon);
  }

  unlockParameters();
}

/*!
*/
string OpticalFlowModule::verifyParameterFunction(const Parameter& inParameter) const throw()
{
	string lResult = "";

	return lResult;
}

//Create entry points and modules functions
VIPERS_MODULE_ENTRY_POINT
VIPERS_MODULE_FUNCTIONS(OpticalFlowModule)
