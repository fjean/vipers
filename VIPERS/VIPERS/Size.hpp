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
 *  Department of Electrical and Computer Engineering
 *  Universite Laval, Quebec, Canada, G1V 0A6
 *  http://vision.gel.ulaval.ca
 *
 */

/*!
 * \file Size.hpp
 * \brief Size class module header.
 * \author Frederic Jean
 * $Revision: 247 $
 * $Date: 2009-10-12 23:26:19 -0400 (Mon, 12 Oct 2009) $
 */

#ifndef VIPERS_SIZE_HPP
#define VIPERS_SIZE_HPP

#include <string>

namespace VIPERS
{

	using namespace std;

	/*! \brief %Size class.
		\author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada
		TODO: Long description
	*/
	class Size
	{
		public:

		//! Default constructor
		Size(unsigned int inNbDimensions = 2);
		//! Default constructor with 2 dimension
		Size(unsigned int inDim1, unsigned int inDim2) throw();
		//! Constructor with string
		Size(const string& inSizeStr);
		//! Copy constructor
		Size(const Size& inSize) throw();
		//! Destructor
		~Size();

		//! Get reference on a dimension
		unsigned int& operator[](unsigned int inDimension);
		//! Get number of dimension
		unsigned int getNbDimension() const throw();

		//! Assignment operator with Size object
		Size& operator=(const Size& inSize);
		//! Assignment operator with string object
		Size& operator=(const string& inSizeStr);

		//! Get string representation
		string getString() const throw();

		//! Compare Size
		bool operator==(const Size& inSize) const throw();
		//! Compare size
		bool operator!=(const Size& inSize) const throw();

		private:

		unsigned int* mSizeArray; //!< Pointer to size array
		unsigned int mNbDimensions; //!< Number of dimension

	};

}

#endif //VIPERS_SIZE_HPP
