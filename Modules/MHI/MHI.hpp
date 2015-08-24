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
 * \file MHI.hpp
 * \brief MHIModule class module header.
 * \author Frederic Jean
 * $Revision: 261 $
 * $Date: 2009-10-18 16:02:27 -0400 (Sun, 18 Oct 2009) $
 */

#ifndef VIPERS_MHIMODULE_HPP
#define VIPERS_MHIMODULE_HPP

#include <VIPERS.hpp>
#include <Module.hpp>
#include <Image.hpp>
#include <cv.h>
#include <highgui.h>
#include <PACC/Util/Timer.hpp>

using namespace VIPERS;
using namespace PACC;

/*! \brief %MHIModule class.
	\author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada
	TODO: Long description
*/
class MHIModule: public Module
{
	public:

	//! Default constructor
	MHIModule();
	//! Virtual destructor
	virtual ~MHIModule();

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

	Parameter mParamUnits; //!< Units use for MHI duration
	Parameter mParamDuration; //!< MHI duration parameter

	ModuleSlot* mInputSlotMask; //!< Mask slot
	ModuleSlot* mOutputSlotMHI; //!< MHI slot
	ModuleSlot* mOutputSlotMHIGray; //!< MHI gray image slot

	IplImage* mOutputMHIIpl; //!< Output MHI
	IplImage* mOutputMHIGrayIpl; //!< Output MHI gray image
  Image* mOutputMHI;
  Image* mOutputMHIGray;

	double mEllapsedTime; //!< Ellapsed time
	Timer mTimer; //!< Duration timer
};

#endif //VIPERS_MHIMODULE_HPP
