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
 * \file VIPERS/Parameter.cpp
 * \brief Parameter class functions definition.
 * \author Frederic Jean
 * $Revision: 289 $
 * $Date: 2009-10-30 20:19:36 -0400 (Fri, 30 Oct 2009) $
 */

#include "Parameter.hpp"

using namespace VIPERS;
using namespace std;


/*! \todo
*/
Value::Value()
{

}

/*! \todo
*/
Value::Value(const string& inValue)
{
  mValue = inValue.c_str();
}

/*! \todo
*/
Value::Value(const string& inValue, const string& inDisplayName, const string& inDescription)
{
  mValue = inValue.c_str();
  mDisplayName = inDisplayName.c_str();
  mDescription = inDescription.c_str();
}

/*! \todo
*/
Value::Value(const Value& inValue)
{
  mValue = inValue.mValue.c_str();
  mDisplayName = inValue.mDisplayName.c_str();
  mDescription = inValue.mDescription.c_str();
}

/*! \todo
*/
Value::~Value()
{

}

/*! \todo
*/
Value& Value::operator=(const Value& inValue)
{
  mValue = inValue.mValue.c_str();
  mDisplayName = inValue.mDisplayName.c_str();
  mDescription = inValue.mDescription.c_str();
  return *this;
}

/*! \todo
*/
Value& Value::operator=(const string& inValue)
{
  mValue = inValue.c_str();
  return *this;
}

/*! \todo
*/
bool Value::operator==(const Value& inValue) const
{
  return mValue==inValue.mValue;
}

/*! \todo
*/
bool Value::operator==(const string& inValue) const
{
  return mValue==inValue;
}

/*! \todo
*/
bool Value::operator<(const Value& inValue) const
{
  return mValue<inValue.mValue;
}

/*! \todo
*/
bool Value::operator<(const string& inValue) const
{
  return mValue<inValue;
}

/*! \todo
*/
Value::operator string() const
{
  return mValue.c_str();
}

/*! \todo
*/
string Value::getValue() const
{
  return mValue.c_str();
}

/*! \todo
*/
void Value::setValue(const string& inValue)
{
  mValue = inValue.c_str();
}

/*! \todo
*/
string Value::getDisplayName() const
{
  return mDisplayName.c_str();
}

/*! \todo
*/
void Value::setDisplayName(const string& inDisplayName)
{
  mDisplayName = inDisplayName.c_str();
}

/*! \todo
*/
string Value::getDescription() const
{
  return mDescription.c_str();
}

/*! \todo
*/
void Value::setDescription(const string& inDescription)
{
  mDescription = inDescription.c_str();
}

/*! \todo
*/
Parameter::Parameter(const string& inName, VariableType inVarType, const string& inDisplayName, const string& inDescription, bool inRunTimeChange) throw()
	: Variable(inName, inVarType, inDisplayName, inDescription)
{
	mEnabled = true;
	mRunTimeChange = inRunTimeChange;
}

/*! \todo
*/
Parameter::Parameter(const Parameter& inParameter)
	: Variable(inParameter)
{
	mRunTimeChange = inParameter.mRunTimeChange;
	mEnabled = inParameter.mEnabled;

	mPossibleValues.clear();
	mPossibleValues = inParameter.mPossibleValues;

  for(NameSet::iterator lNameItr = inParameter.mDependentParameterNameSet.begin(); lNameItr != inParameter.mDependentParameterNameSet.end(); lNameItr++)
    mDependentParameterNameSet.insert(lNameItr->c_str());

	mMinValue = inParameter.mMinValue.c_str();
	mMaxValue = inParameter.mMaxValue.c_str();
	mStepValue = inParameter.mStepValue.c_str();
}

/*! \todo
*/
Parameter& Parameter::operator=(const Parameter& inParameter)
{
	::Variable::operator=(inParameter);
	mEnabled = inParameter.mEnabled;

  mPossibleValues.clear();
  mPossibleValues = inParameter.mPossibleValues;

	mMinValue = inParameter.mMinValue.c_str();
	mMaxValue = inParameter.mMaxValue.c_str();
	mStepValue = inParameter.mStepValue.c_str();
	return *this;
}

/*! \todo
*/
Parameter::~Parameter()
{
	mPossibleValues.clear();
}

/*! \todo
*/
bool Parameter::getRunTimeChange() const throw()
{
	return mRunTimeChange;
}

/*! \todo
*/
bool Parameter::getEnabled() const throw()
{
	return mEnabled;
}

/*! \todo
*/
ValueSet Parameter::getPossibleValues() const throw()
{
	return mPossibleValues;
}

/*! \todo
*/
unsigned int Parameter::getPossibleValuesCount() const throw()
{
  return mPossibleValues.size();
}

/*! \todo
*/
string Parameter::getMinValue() const throw()
{
	return mMinValue.c_str();
}

/*! \todo
*/
string Parameter::getMaxValue() const throw()
{
	return mMaxValue.c_str();
}

/*! \todo
*/
string Parameter::getStepValue() const throw()
{
	return mStepValue.c_str();
}

/*! \todo
*/
NameSet Parameter::getDependentParameterNameSet() const throw()
{
  NameSet lNameSet;
  for(NameSet::const_iterator lItr = mDependentParameterNameSet.begin(); lItr != mDependentParameterNameSet.end(); lItr++)
    lNameSet.insert(lItr->c_str());
  return lNameSet;
}

/*! \todo
*/
void Parameter::setEnabled(bool inEnabled) throw()
{
	mEnabled = inEnabled;
}

/*! \todo
*/
void Parameter::setPossibleValues(const ValueSet& inValues) throw()
{
	mPossibleValues.clear();
	mPossibleValues = inValues;
}

/*! \todo
*/
void Parameter::setMinValue(const string& inValue) throw()
{
	mMinValue = inValue.c_str();
}

/*! \todo
*/
void Parameter::setMaxValue(const string& inValue) throw()
{
	mMaxValue = inValue.c_str();
}

/*! \todo
*/
void Parameter::setStepValue(const string& inValue) throw()
{
	mStepValue = inValue.c_str();
}

/*! \todo
*/
void Parameter::setDependentParameterNameSet(const NameSet& inNameSet) throw()
{
  mDependentParameterNameSet.clear();
  for(NameSet::const_iterator lItr = inNameSet.begin(); lItr != inNameSet.end(); lItr++)
    mDependentParameterNameSet.insert(lItr->c_str());
}
