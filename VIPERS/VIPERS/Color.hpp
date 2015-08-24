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
 * \file VIPERS/Color.hpp
 * \brief Color class module header.
 * \author Frederic Jean
 * $Revision: 247 $
 * $Date: 2009-10-12 23:26:19 -0400 (Mon, 12 Oct 2009) $
 */

#ifndef VIPERS_COLOR_HPP
#define VIPERS_COLOR_HPP

#include <vector>
#include <string>

namespace VIPERS
{
	using namespace std;

	//! Color array
	typedef vector<double> ColorArray;

	/*! \brief %Color class.
		\author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada
		TODO: Long description
	*/
	class Color
	{
		public:

		//! Default constructor
		Color(double inCmp1=0, double inCmp2=0, double inCmp3=0) throw();
		//! Constructor with color array
		Color(double inColor[3]) throw();
		//! Constructor with ColorArray
		Color(const ColorArray& inColorArray);
		//! Constructor with string
		Color(const string& inColorStr);
		//! Copy constructor
		Color(const Color& inColor) throw();
		//! Destructor
		~Color();

		//! Get color array
		ColorArray getColor() const throw();
		//! Get color string
		string getString() const throw();

		//! Set color array
		void setColor(const ColorArray& inColorArray);
		//! Set color array
		void setColor(double inColor[3]) throw();
		//! Set color from string
		void setColor(const string& inColorStr);

		//! Get component 1
		double getComponent1() const throw();
		//! Get component 2
		double getComponent2() const throw();
		//! Get component 3
		double getComponent3() const throw();
		//! Set component 1
		void setComponent1(double inCmp) throw();
		//! Set component 2
		void setComponent2(double inCmp) throw();
		//! Set component 3
		void setComponent3(double inCmp) throw();

		//! Get reference to specified component
		double& operator[](unsigned int inCmpIdx);

		//! Assigment operator with ColorArray
		Color& operator=(const ColorArray& inColorArray);
		//! Assigment operator with color array
		Color& operator=(double inColor[3]) throw();
		//! Assigment operator with string
		Color& operator=(const string& inColorStr);

		//! Compare Color
		bool operator==(const Color& inColor) const throw();

		private:

		double mColor[3]; //!< Color array

	};

}

#endif //VIPERS_COLOR_HPP
