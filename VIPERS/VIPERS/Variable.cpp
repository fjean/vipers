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
 * \file VIPERS/Variable.cpp
 * \brief Variable class functions definition.
 * \author Frederic Jean
 * $Revision: 252 $
 * $Date: 2009-10-14 21:52:34 -0400 (Wed, 14 Oct 2009) $
 */

#include "Variable.hpp"
#include <sstream>

using namespace VIPERS;
using namespace std;

/*! \todo
*/
Variable::Variable(const string& inName, VariableType inVariableType, const string& inDisplayName, const string& inDescription) throw()
{
	mName = inName.c_str();
	mVariableType = inVariableType;
	mDisplayName = inDisplayName.c_str();
	mDescription = inDescription.c_str();
}

/*! \todo
*/
Variable::Variable(const Variable& inVariable)
{
	mName = inVariable.mName.c_str();
	mDisplayName = inVariable.mDisplayName.c_str();
	mVariableType = inVariable.mVariableType;
	mDescription = inVariable.mDescription.c_str();
	mValue = inVariable.mValue.c_str();
}

/*! \todo
*/
Variable& Variable::operator=(const Variable& inVariable) throw()
{
	mValue = inVariable.mValue.c_str();
	return *this;
}

/*! \todo
*/
bool Variable::operator==(const Variable& inVariable) const throw()
{
	return mValue==inVariable.mValue;
}

/*! \todo
*/
bool Variable::operator!=(const Variable& inVariable) const throw()
{
	return mValue!=inVariable.mValue;
}

/*! \todo
*/
Variable::~Variable()
{

}

/*! \todo
*/
string Variable::getName() const throw()
{
	return mName.c_str();
}

/*! \todo
*/
string Variable::getDisplayName() const throw()
{
	return mDisplayName.c_str();
}

/*! \todo
*/
string Variable::getDescription() const throw()
{
	return mDescription.c_str();
}

/*! \todo
*/
Variable::VariableType Variable::getVariableType() const throw()
{
	return mVariableType;
}

/*! \todo
*/
void Variable::setValueStr(const string& inValueStr) throw()
{
	mValue = inValueStr.c_str();
}

/*! \todo
*/
void Variable::setValue(bool inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
}

/*! \todo
*/
void Variable::setValue(char inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
}

/*! \todo
*/
void Variable::setValue(unsigned char inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
}

/*! \todo
*/
void Variable::setValue(short inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
}

/*! \todo
*/
void Variable::setValue(unsigned short inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
}

/*! \todo
*/
void Variable::setValue(int inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
}

/*! \todo
*/
void Variable::setValue(unsigned int inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
}

/*! \todo
*/
void Variable::setValue(long inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
}

/*! \todo
*/
void Variable::setValue(unsigned long inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
}

/*! \todo
*/
void Variable::setValue(float inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr.precision(6);
	lTmpStr.setf(ios::scientific, ios::floatfield);
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
}

/*! \todo
*/
void Variable::setValue(double inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr.precision(15);
	lTmpStr.setf(ios::scientific, ios::floatfield);
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
}

/*! \todo
*/
void Variable::setValue(long double inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr.precision(19);
	lTmpStr.setf(ios::scientific, ios::floatfield);
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
}

/*! \todo
*/
void Variable::setValue(const Color& inValue) throw()
{
	mValue = inValue.getString();
}

/*! \todo
*/
void Variable::setValue(const Size& inValue) throw()
{
	mValue = inValue.getString();
}

/*! \todo
*/
Variable& Variable::operator=(const string& inValueStr) throw()
{
	mValue = inValueStr.c_str();
	return *this;
}

/*! \todo
*/
Variable& Variable::operator=(bool inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
	return *this;
}

/*! \todo
*/
Variable& Variable::operator=(char inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
	return *this;
}

/*! \todo
*/
Variable& Variable::operator=(unsigned char inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
	return *this;
}

/*! \todo
*/
Variable& Variable::operator=(short inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
	return *this;
}

/*! \todo
*/
Variable& Variable::operator=(unsigned short inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
	return *this;
}

/*! \todo
*/
Variable& Variable::operator=(int inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
	return *this;
}

/*! \todo
*/
Variable& Variable::operator=(unsigned int inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
	return *this;
}

/*! \todo
*/
Variable& Variable::operator=(long inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
	return *this;
}

/*! \todo
*/
Variable& Variable::operator=(unsigned long inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
	return *this;
}

/*! \todo
*/
Variable& Variable::operator=(float inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr.precision(6);
	lTmpStr.setf(ios::scientific, ios::floatfield);
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
	return *this;
}

/*! \todo
*/
Variable& Variable::operator=(double inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr.precision(15);
	lTmpStr.setf(ios::scientific, ios::floatfield);
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
	return *this;
}

/*! \todo
*/
Variable& Variable::operator=(long double inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr.precision(19);
	lTmpStr.setf(ios::scientific, ios::floatfield);
	lTmpStr << inValue;
	mValue = lTmpStr.str().c_str();
	return *this;
}

/*! \todo
*/
Variable& Variable::operator=(const Size& inValue) throw()
{
	mValue = inValue.getString();
	return *this;
}

/*! \todo
*/
Variable& Variable::operator=(const Color& inValue) throw()
{
	mValue = inValue.getString();
	return *this;
}

/*! \todo
*/
string Variable::getValue() const throw()
{
	return mValue.c_str();
}

/*! \todo
*/
string Variable::toString() const throw()
{
	return mValue.c_str();
}

/*! \todo
*/
bool Variable::toBool() const throw()
{
	bool lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
char Variable::toChar() const throw()
{
	char lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
unsigned char Variable::toUChar() const throw()
{
	unsigned char lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
short Variable::toShort() const throw()
{
	short lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
unsigned short Variable::toUShort() const throw()
{
	unsigned short lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
int Variable::toInt() const throw()
{
	int lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
unsigned int Variable::toUInt() const throw()
{
	unsigned int lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
long Variable::toLong() const throw()
{
	long lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
unsigned long Variable::toULong() const throw()
{
	unsigned long lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
float Variable::toFloat() const throw()
{
	float lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
double Variable::toDouble() const throw()
{
	double lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
long double Variable::toLDouble() const throw()
{
	long double lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
Size Variable::toSize() const
{
	try
	{
		return Size(mValue);
	}
	catch(...)
	{
		throw;
	}
}

/*! \todo
*/
Color Variable::toColor() const
{
	try
	{
		return Color(mValue);
	}
	catch(...)
	{
		throw;
	}
}

/*! \todo
*/
bool Variable::operator==(const string& inValueStr) const throw()
{
	return mValue==inValueStr;
}

/*! \todo
*/
bool Variable::operator==(bool inValue) const throw()
{
	return toBool()==inValue;
}

/*! \todo
*/
bool Variable::operator==(char inValue) const throw()
{
	return toChar()==inValue;
}

/*! \todo
*/
bool Variable::operator==(unsigned char inValue) const throw()
{
	return toUChar()==inValue;
}

/*! \todo
*/
bool Variable::operator==(short inValue) const throw()
{
	return toShort()==inValue;
}

/*! \todo
*/
bool Variable::operator==(unsigned short inValue) const throw()
{
	return toUShort()==inValue;
}

/*! \todo
*/
bool Variable::operator==(int inValue) const throw()
{
	return toInt()==inValue;
}


/*! \todo
*/
bool Variable::operator==(unsigned int inValue) const throw()
{
	return toUInt()==inValue;
}

/*! \todo
*/
bool Variable::operator==(long inValue) const throw()
{
	return toLong()==inValue;
}

/*! \todo
*/
bool Variable::operator==(unsigned long inValue) const throw()
{
	return toULong()==inValue;
}

/*! \todo
*/
bool Variable::operator==(float inValue) const throw()
{
	return toFloat()==inValue;
}

/*! \todo
*/
bool Variable::operator==(double inValue) const throw()
{
	return toDouble()==inValue;
}

/*! \todo
*/
bool Variable::operator==(long double inValue) const throw()
{
	return toLDouble()==inValue;
}

/*! \todo
*/
bool Variable::operator==(const Size& inValue) const throw()
{
	return mValue==inValue.getString();
}

/*! \todo
*/
bool Variable::operator==(const Color& inValue) const throw()
{
	return mValue==inValue.getString();
}

/*! \todo
*/
bool Variable::operator!=(const string& inValueStr) const throw()
{
	return mValue!=inValueStr;
}

/*! \todo
*/
bool Variable::operator!=(bool inValue) const throw()
{
	return toBool()!=inValue;
}

/*! \todo
*/
bool Variable::operator!=(char inValue) const throw()
{
	return toChar()!=inValue;
}

/*! \todo
*/
bool Variable::operator!=(unsigned char inValue) const throw()
{
	return toUChar()!=inValue;
}

/*! \todo
*/
bool Variable::operator!=(short inValue) const throw()
{
	return toShort()!=inValue;
}

/*! \todo
*/
bool Variable::operator!=(unsigned short inValue) const throw()
{
	return toUShort()!=inValue;
}

/*! \todo
*/
bool Variable::operator!=(int inValue) const throw()
{
	return toInt()!=inValue;
}


/*! \todo
*/
bool Variable::operator!=(unsigned int inValue) const throw()
{
	return toUInt()!=inValue;
}

/*! \todo
*/
bool Variable::operator!=(long inValue) const throw()
{
	return toLong()!=inValue;
}

/*! \todo
*/
bool Variable::operator!=(unsigned long inValue) const throw()
{
	return toULong()!=inValue;
}

/*! \todo
*/
bool Variable::operator!=(float inValue) const throw()
{
	return toFloat()!=inValue;
}

/*! \todo
*/
bool Variable::operator!=(double inValue) const throw()
{
	return toDouble()!=inValue;
}

/*! \todo
*/
bool Variable::operator!=(long double inValue) const throw()
{
	return toLDouble()!=inValue;
}

/*! \todo
*/
bool Variable::operator!=(const Size& inValue) const throw()
{
	return mValue!=inValue.getString();
}

/*! \todo
*/
bool Variable::operator!=(const Color& inValue) const throw()
{
	return mValue!=inValue.getString();
}
