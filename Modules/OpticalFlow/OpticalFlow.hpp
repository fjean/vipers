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
 * \file OpticalFlow.hpp
 * \brief OpticalFlowModule class module header.
 * \author Frederic Jean
 * $Revision: 261 $
 * $Date: 2009-10-18 16:02:27 -0400 (Sun, 18 Oct 2009) $
 */

#ifndef VIPERS_OPTICALFLOWMODULE_HPP
#define VIPERS_OPTICALFLOWMODULE_HPP

#include <VIPERS.hpp>
#include <Module.hpp>
#include <Image.hpp>
#include <cv.h>
#include <highgui.h>
#include <string>
#include <map>

using namespace VIPERS;
using namespace std;

/*! \brief %OpticalFlowModule class.
	\author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada
	TODO: Long description
*/
class OpticalFlowModule: public Module
{
	public:

  //! Map of criterion type type
  typedef map<string,int> CriterionTypeMap;

	//! Default constructor
    OpticalFlowModule();
	//! Virtual destructor
	virtual ~OpticalFlowModule();

	protected:

	//! Verifies inputs and initializes outputs
	void initFunction();
	//! Put the module in a ready state for processing
	void startFunction();
  //! Put the module in a paused state
  void pauseFunction();
	//! Processes current inputs and/or generates outputs for one cycle
	void processFunction(unsigned int inFrameNumber);
	//! Put the module in a stopped state (no more ready for processing)
	void stopFunction();
	//! Resetting the module as it was when created
	void resetFunction();

	//! Update %Module parameters
	void updateParametersFunction();
	//! Verify if Parameter value is valid without actually changing the value
	string verifyParameterFunction(const Parameter& inParameter) const throw();

	private:

	Parameter mParamAlgorithm;
	Parameter mParamBlockSize;
	Parameter mParamShiftSize;
	Parameter mParamMaxRange;
	Parameter mParamUsePrevious;
	Parameter mParamLambda;
	Parameter mParamCriterionType;
	Parameter mParamCriterionMaxIter;
	Parameter mParamCriterionEpsilon;

	ModuleSlot* mInputSlot;
	ModuleSlot* mOutputSlotVelX;
	ModuleSlot* mOutputSlotVelY;
	ModuleSlot* mOutputSlotVelNorm;
	ModuleSlot* mOutputSlotVelNormGray;
	ModuleSlot* mOutputSlotVelVectors;

	IplImage* mVelXIpl;
	IplImage* mVelYIpl;
  IplImage* mVelNormIpl;
  IplImage* mVelNormGrayIpl;
  IplImage* mVelVectorsIpl;

	Image* mVelX;
	Image* mVelY;
  Image* mVelNorm;
  Image* mVelNormGray;
  Image* mVelVectors;

  IplImage* mPrevImageIpl;
  Image* mPrevImage;

  IplImage* mTmpImg1;
  IplImage* mTmpImg2;

  CriterionTypeMap mCriterionTypeMap;

  double mMinDist;
  double mMaxDist;

};

#endif //VIPERS_OPTICALFLOWMODULE_HPP
