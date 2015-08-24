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
 * \file VIPERS/Property.cpp
 * \brief Property class functions definition.
 * \author Frederic Jean
 * $Revision: 252 $
 * $Date: 2009-10-14 21:52:34 -0400 (Wed, 14 Oct 2009) $
 */

#include "Property.hpp"
#include <sstream>

using namespace VIPERS;
using namespace std;

/*! \todo
*/
Property::Property(const string& inName, Variable::VariableType inVariableType, const string& inDisplayName, const string& inDescription) throw()
	: Variable(inName, inVariableType, inDisplayName, inDescription)
{

}

/*! \todo
*/
Property::Property(const Property& inProperty)
	: Variable(inProperty)
{
	mPropertyMap.clear();
	for(PropertyMap::const_iterator lItr = inProperty.mPropertyMap.begin(); lItr != inProperty.mPropertyMap.end(); lItr++)
		mPropertyMap.insert(pair<string,Property>(lItr->second.getName().c_str(), lItr->second) );
}

/*! \todo
*/
Property& Property::operator=(const Property& inProperty) throw()
{
	::Variable::operator=(inProperty);

	mPropertyMap.clear();
	for(PropertyMap::const_iterator lItr = inProperty.mPropertyMap.begin(); lItr != inProperty.mPropertyMap.end(); lItr++)
		mPropertyMap.insert(pair<string,Property>(lItr->second.getName().c_str(), lItr->second) );

	return *this;
}

/*! \todo
*/
bool Property::operator==(const Property& inProperty) const throw()
{
	return mValue==inProperty.mValue;
}

/*! \todo
*/
bool Property::operator!=(const Property& inProperty) const throw()
{
	return mValue!=inProperty.mValue;
}

/*! \todo
*/
Property::operator bool() const throw()
{
	return getName()!="";
}

/*! \todo
*/
Property::~Property()
{
	mPropertyMap.clear();
}

/*! \todo
*/
void Property::setProperty(const Property& inProperty) throw()
{
	mPropertyMap.insert(pair<string,Property>(inProperty.getName().c_str(), inProperty));
}

/*! \todo
*/
Property Property::getProperty(const string& inName) const throw()
{
	PropertyMap::const_iterator lItr = mPropertyMap.find(inName.c_str());
	if(lItr!=mPropertyMap.end())
		return lItr->second;
	else
		return Property("", Variable::eVariableTypeString, "", "");
}

/*! \todo
*/
PropertyList Property::getPropertyList() const throw()
{
	PropertyList lPropertyList;
	PropertyMap::const_iterator lItr = mPropertyMap.begin();
	for(lItr; lItr!=mPropertyMap.end(); lItr++)
		lPropertyList.push_back(lItr->second);
	return lPropertyList;
}

/*! \todo
*/
void Property::setPropertyList(const PropertyList& inPropertyList) throw()
{
	PropertyList::const_iterator lItr = inPropertyList.begin();
	for(lItr; lItr!=inPropertyList.end(); lItr++)
		mPropertyMap.insert(pair<string,Property>(lItr->getName().c_str(), *lItr));
}

/*! \todo
*/
bool Property::eraseProperty(const string& inName) throw()
{
	return mPropertyMap.erase(inName);
}

/*! \todo
*/
void Property::clearPropertyList() throw()
{
	mPropertyMap.clear();
}
