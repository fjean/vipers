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
 * \file VIPERS/Variable.hpp
 * \brief Variable class header.
 * \author Frederic Jean
 * $Revision: 252 $
 * $Date: 2009-10-14 21:52:34 -0400 (Wed, 14 Oct 2009) $
 */

#ifndef VIPERS_VARIABLE_HPP
#define VIPERS_VARIABLE_HPP

#include "Size.hpp"
#include "Color.hpp"

#include <string>

namespace VIPERS
{

	using namespace std;

	/*! \brief %Variable class.
		\author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada

		\todo
	*/
	class Variable
	{
		public:

		/*! \brief Type of variables
		\author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada
		*/
		enum VariableType
		{
			eVariableTypeBool, //!< Boolean
			eVariableTypeChar, //!< Char
			eVariableTypeUChar, //!< Unsigned char
			eVariableTypeShort,	//!< Signed short
			eVariableTypeUShort, //!< Unsigned short
			eVariableTypeInt, //!< Signed integer
			eVariableTypeUInt, //!< Unsigned integer
			eVariableTypeLong, //!< Signed long
			eVariableTypeULong, //!< Unsigned long
			eVariableTypeFloat, //!< Float
			eVariableTypeDouble, //!< Double
			eVariableTypeLDouble, //!< Long double
			eVariableTypeString, //!< String
			eVariableTypeFile, //!< File in a string
			eVariableTypeDirectory, //!< Directory in a string
			eVariableTypeColor, //!< Color in a string
			eVariableTypeSize //!< Size in a string
		};

		//! Default explicit constructor
		explicit Variable(const string& inName, VariableType inVariableType, const string& inDisplayName, const string& inDescription) throw();
        //! Copy constructor
        Variable(const Variable& inVariable);
        //! Assignment operator
		Variable& operator=(const Variable& inVariable) throw();
		//! Comparison operator
		bool operator==(const Variable& inVariable) const throw();
		//! Comparison operator
		bool operator!=(const Variable& inVariable) const throw();

		//! Destructor
		virtual ~Variable();

        //! Get variable name
        string getName() const throw();
        //! Get variable display name
        string getDisplayName() const throw();
        //! Get description
        string getDescription() const throw();
        //! Get type
        VariableType getVariableType() const throw();

        //! Set value as a string
        void setValueStr(const string& inValueStr) throw();
        //! Set value as a bool
        void setValue(bool inValue) throw();
        //! Set value as a char
        void setValue(char inValue) throw();
        //! Set value as an unsigned char
        void setValue(unsigned char inValue) throw();
        //! Set value as a short
        void setValue(short inValue) throw();
        //! Set value as an unsigned short
        void setValue(unsigned short inValue) throw();
        //! Set value as a int
        void setValue(int inValue) throw();
        //! Set value as an unsigned int
        void setValue(unsigned int inValue) throw();
        //! Set value as a long
        void setValue(long inValue) throw();
        //! Set value as an unsigned long
        void setValue(unsigned long inValue) throw();
        //! Set value as a float
        void setValue(float inValue) throw();
        //! Set value as a double
        void setValue(double inValue) throw();
        //! Set value as a long double
        void setValue(long double inValue) throw();
        //! Set value as a Size
        void setValue(const Size& inValue) throw();
        //! Set value as a Color
        void setValue(const Color& inValue) throw();

        //! Set value as a string
        Variable& operator=(const string& inValueStr) throw();
        //! Set value as a bool
        Variable& operator=(bool inValue) throw();
        //! Set value as a char
        Variable& operator=(char inValue) throw();
        //! Set value as an unsigned char
        Variable& operator=(unsigned char inValue) throw();
        //! Set value as a short
        Variable& operator=(short inValue) throw();
        //! Set value as an unsigned short
        Variable& operator=(unsigned short inValue) throw();
        //! Set value as a int
        Variable& operator=(int inValue) throw();
        //! Set value as an unsigned int
        Variable& operator=(unsigned int inValue) throw();
        //! Set value as a long
        Variable& operator=(long inValue) throw();
        //! Set value as an unsigned long
        Variable& operator=(unsigned long inValue) throw();
        //! Set value as a float
        Variable& operator=(float inValue) throw();
        //! Set value as a double
        Variable& operator=(double inValue) throw();
        //! Set value as a long double
        Variable& operator=(long double inValue) throw();
        //! Set value as a Size
        Variable& operator=(const Size& inValue) throw();
        //! Set value as a Color
        Variable& operator=(const Color& inValue) throw();

        //! Get value as a string
        string getValue() const throw();

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
        //! Get value as a Size
        Size toSize() const;
        //! Get value as a Color
        Color toColor() const;

        //! Compare value as a string
		bool operator==(const string& inValueStr) const throw();
        //! Compare value as a bool
        bool operator==(bool inValue) const throw();
        //! Compare value as a char
        bool operator==(char inValue) const throw();
		//! Compare value as an unsigned char
        bool operator==(unsigned char inValue) const throw();
        //! Compare value as a short
        bool operator==(short inValue) const throw();
        //! Compare value as an unsigned short
        bool operator==(unsigned short inValue) const throw();
        //! Compare value as a int
        bool operator==(int inValue) const throw();
        //! Compare value as an unsigned int
        bool operator==(unsigned int inValue) const throw();
        //! Compare value as a long
        bool operator==(long inValue) const throw();
        //! Compare value as an unsigned long
        bool operator==(unsigned long inValue) const throw();
        //! Compare value as a float
        bool operator==(float inValue) const throw();
        //! Compare value as a double
        bool operator==(double inValue) const throw();
        //! Compare value as a long double
        bool operator==(long double inValue) const throw();
        //! Compare value as a Size
        bool operator==(const Size& inValue) const throw();
        //! Compare value as a Color
        bool operator==(const Color& inValue) const throw();

        //! Compare value as a string
		bool operator!=(const string& inValueStr) const throw();
        //! Compare value as a bool
        bool operator!=(bool inValue) const throw();
        //! Compare value as a char
        bool operator!=(char inValue) const throw();
		//! Compare value as an unsigned char
        bool operator!=(unsigned char inValue) const throw();
        //! Compare value as a short
        bool operator!=(short inValue) const throw();
        //! Compare value as an unsigned short
        bool operator!=(unsigned short inValue) const throw();
        //! Compare value as a int
        bool operator!=(int inValue) const throw();
        //! Compare value as an unsigned int
        bool operator!=(unsigned int inValue) const throw();
        //! Compare value as a long
        bool operator!=(long inValue) const throw();
        //! Compare value as an unsigned long
        bool operator!=(unsigned long inValue) const throw();
        //! Compare value as a float
        bool operator!=(float inValue) const throw();
        //! Compare value as a double
        bool operator!=(double inValue) const throw();
        //! Compare value as a long double
        bool operator!=(long double inValue) const throw();
        //! Compare value as a Size
        bool operator!=(const Size& inValue) const throw();
        //! Compare value as a Color
        bool operator!=(const Color& inValue) const throw();

		protected:

		string mValue; //! The current value

		private:

		string mName; //!< Unique name of the variable
		string mDisplayName; //!< Display name
		string mDescription; //!< Description
		VariableType mVariableType; //!< Variable type

	};

}

#endif //VIPERS_VARIABLE_HPP
