/*
 *  Video and Image Processing Environment for Real-time Systems (VIPERS)
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
 * \file VIPERS/Converter.hpp
 * \brief Converter class header.
 * \author Frederic Jean
 * $Revision: 242 $
 * $Date: 2009-10-12 16:30:29 -0400 (Mon, 12 Oct 2009) $
 */

#ifndef VIPERS_CONVERTER_HPP
#define VIPERS_CONVERTER_HPP

#include <string>

namespace VIPERS
{

	using namespace std;

	/*! \brief %Converter class.
		\author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada

		\todo
	*/
	class Converter
	{
		public:

		//! Constructor with string
		Converter(const Converter& inConverter) throw();
		//! Constructor with string
		Converter(const string& inValue) throw();
		//! Constructor with bool
		Converter(bool inValue) throw();
		//! Constructor with char
		Converter(char inValue) throw();
		//! Constructor with unsigned char
		Converter(unsigned char inValue) throw();
		//! Constructor with short
		Converter(short inValue) throw();
		//! Constructor with unsigned short
		Converter(unsigned short inValue) throw();
		//! Constructor with int
		Converter(int inValue) throw();
		//! Constructor with unsigned int
		Converter(unsigned int inValue) throw();
		//! Constructor with long
		Converter(long inValue) throw();
		//! Constructor with unsigned long
		Converter(unsigned long inValue) throw();
		//! Constructor with float
		Converter(float inValue) throw();
		//! Constructor with double
		Converter(double inValue) throw();
		//! Constructor with long double
		Converter(long double inValue) throw();

		//! Destructor
		~Converter();

    //! Set value as a string
    void operator=(const string& inValue) throw();
    //! Set value as a bool
    void operator=(bool inValue) throw();
    //! Set value as a char
    void operator=(char inValue) throw();
    //! Set value as an unsigned char
    void operator=(unsigned char inValue) throw();
    //! Set value as a short
    void operator=(short inValue) throw();
    //! Set value as an unsigned short
    void operator=(unsigned short inValue) throw();
    //! Set value as a int
    void operator=(int inValue) throw();
    //! Set value as an unsigned int
    void operator=(unsigned int inValue) throw();
    //! Set value as a long
    void operator=(long inValue) throw();
    //! Set value as an unsigned long
    void operator=(unsigned long inValue) throw();
    //! Set value as a float
    void operator=(float inValue) throw();
    //! Set value as a double
    void operator=(double inValue) throw();
    //! Set value as a long double
    void operator=(long double inValue) throw();

    //! Get value as an string
    operator string() const throw();
    //! Get value as a bool
    operator bool() const throw();
    //! Get value as a char
    operator char() const throw();
    //! Get value as an unsigned char
    operator unsigned char() const throw();
    //! Get value as a short
    operator short() const throw();
    //! Get value as an unsigned short
    operator unsigned short() const throw();
    //! Get value as an int
    operator int() const throw();
    //! Get value as an unsigned int
    operator unsigned int() const throw();
    //! Get value as a long
    operator long() const throw();
    //! Get value as an unsigned long
    operator unsigned long() const throw();
    //! Get value as a float
    operator float() const throw();
    //! Get value as an double
    operator double() const throw();
    //! Get value as an long double
    operator long double() const throw();

    //! Get value as an string
    string toString() const throw();
    //! Get value as a bool
    bool toBool() const throw();
    //! Get value as a char
    char toChar() const throw();
    //! Get value as an unsigned char
    unsigned char toUChar() const throw();
    //! Get value as a short
    short toShort() const throw();
    //! Get value as an unsigned short
    unsigned short toUShort() const throw();
    //! Get value as an int
    int toInt() const throw();
    //! Get value as an unsigned int
    unsigned int toUInt() const throw();
    //! Get value as a long
    long toLong() const throw();
    //! Get value as an unsigned long
    unsigned long toULong() const throw();
    //! Get value as a float
    float toFloat() const throw();
    //! Get value as an double
    double toDouble() const throw();
    //! Get value as an long double
    long double toLDouble() const throw();

		private:

		//! Disable default constructor
		Converter();

		string mValue; //!< String value

	};

}

#endif //VIPERS_CONVERTER_HPP
