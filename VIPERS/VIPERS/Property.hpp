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
 * \file VIPERS/Property.hpp
 * \brief Property class header.
 * \author Frederic Jean
 * $Revision: 252 $
 * $Date: 2009-10-14 21:52:34 -0400 (Wed, 14 Oct 2009) $
 */

#ifndef VIPERS_PROPERTY_HPP
#define VIPERS_PROPERTY_HPP

#include "Variable.hpp"

#include <string>
#include <vector>
#include <map>

namespace VIPERS
{

	using namespace std;

	// Forward declaration
	class Property;

	//! Property map
	typedef map<string, Property> PropertyMap;
	//! Property list
	typedef vector<Property> PropertyList;

	/*! \brief %Property class.
		\author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada

		\todo
	*/
	class Property: public Variable
	{
		public:

		//! Default explicit constructor
		explicit Property(const string& inName, Variable::VariableType inVariableType, const string& inDisplayName, const string& inDescription) throw();
        //! Copy constructor
        Property(const Property& inProperty);
        //! Assignment operator
		Property& operator=(const Property& inProperty) throw();

		//! Comparison operator
		bool operator==(const Property& inProperty) const throw();
		//! Comparison operator
		bool operator!=(const Property& inProperty) const throw();

		//! Operator bool (test if property is valid)
		operator bool() const throw();

		//! Destructor
		virtual ~Property();

        //! Set property
        void setProperty(const Property& inProperty) throw();
        //! Get property
        Property getProperty(const string& inName) const throw();

		//! Get property list
		PropertyList getPropertyList() const throw();
		//! Get property list
		void setPropertyList(const PropertyList& inPropertyList) throw();

		//! Erase property
		bool eraseProperty(const string& inName) throw();
		//! Clear property list
		void clearPropertyList() throw();

		protected:

		PropertyMap mPropertyMap; //!< List of properties in a map

	};

}

#endif //VIPERS_PROPERTY_HPP
