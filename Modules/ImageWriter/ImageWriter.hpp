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
 * \file ImageWriter.hpp
 * \brief ImageWriterModule class module header.
 * \author Frederic Jean
 * $Revision: 334 $
 * $Date: 2011-06-13 17:45:53 -0700 (Mon, 13 Jun 2011) $
 */

#ifndef VIPERS_IMAGEWRITERMODULE_HPP
#define VIPERS_IMAGEWRITERMODULE_HPP

#include <VIPERS.hpp>
#include <Module.hpp>
#include <Image.hpp>
#include <cv.h>
#include <string>
#include <map>

using namespace VIPERS;

/*! \brief %ImageWriterModule class.
	\author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada
	TODO: Long description
*/
class ImageWriterModule: public Module
{
	public:

	//! Default constructor
	ImageWriterModule();
	//! Virtual destructor
	virtual ~ImageWriterModule();

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

	Parameter mParamImageFile; //!< Image file param

	ModuleSlot* mInputSlot; //!< Image input slot

};

#endif //VIPERS_IMAGEWRITERMODULE_HPP
