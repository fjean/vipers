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
 * \file VIPERS/Converter.cpp
 * \brief Converter class functions definition.
 * \author Frederic Jean
 * $Revision: 252 $
 * $Date: 2009-10-14 21:52:34 -0400 (Wed, 14 Oct 2009) $
 */

#include "Converter.hpp"
#include <sstream>

using namespace VIPERS;
using namespace std;

/*! \todo
*/
Converter::Converter(const Converter& inConverter) throw()
{
	mValue = inConverter.mValue;
}

/*! \todo
*/
Converter::Converter(const string& inValue) throw()
{
	*this = inValue.c_str();
}

/*! \todo
*/
Converter::Converter(bool inValue) throw()
{
	*this = inValue;
}

/*! \todo
*/
Converter::Converter(char inValue) throw()
{
	*this = inValue;
}

/*! \todo
*/
Converter::Converter(unsigned char inValue) throw()
{
	*this = inValue;
}

/*! \todo
*/
Converter::Converter(short inValue) throw()
{
	*this = inValue;
}

/*! \todo
*/
Converter::Converter(unsigned short inValue) throw()
{
	*this = inValue;
}

/*! \todo
*/
Converter::Converter(int inValue) throw()
{
	*this = inValue;
}

/*! \todo
*/
Converter::Converter(unsigned int inValue) throw()
{
	*this = inValue;
}

/*! \todo
*/
Converter::Converter(long inValue) throw()
{
	*this = inValue;
}

/*! \todo
*/
Converter::Converter(unsigned long inValue) throw()
{
	*this = inValue;
}

/*! \todo
*/
Converter::Converter(float inValue) throw()
{
	*this = inValue;
}

/*! \todo
*/
Converter::Converter(double inValue) throw()
{
	*this = inValue;
}

/*! \todo
*/
Converter::Converter(long double inValue) throw()
{
	*this = inValue;
}

/*! \todo
*/
Converter::~Converter()
{

}

/*! \todo
*/
void Converter::operator=(const string& inValue) throw()
{
	mValue = inValue.c_str();
}

/*! \todo
*/
void Converter::operator=(bool inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str();
}

/*! \todo
*/
void Converter::operator=(char inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str();
}

/*! \todo
*/
void Converter::operator=(unsigned char inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str();
}

/*! \todo
*/
void Converter::operator=(short inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str();
}

/*! \todo
*/
void Converter::operator=(unsigned short inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str();
}

/*! \todo
*/
void Converter::operator=(int inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str();
}

/*! \todo
*/
void Converter::operator=(unsigned int inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str();
}

/*! \todo
*/
void Converter::operator=(long inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str();
}

/*! \todo
*/
void Converter::operator=(unsigned long inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr << inValue;
	mValue = lTmpStr.str();
}

/*! \todo
*/
void Converter::operator=(float inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr.precision(6);
	lTmpStr.setf(ios::scientific, ios::floatfield);
	lTmpStr << inValue;
	mValue = lTmpStr.str();
}

/*! \todo
*/
void Converter::operator=(double inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr.precision(15);
	lTmpStr.setf(ios::scientific, ios::floatfield);
	lTmpStr << inValue;
	mValue = lTmpStr.str();
}

/*! \todo
*/
void Converter::operator=(long double inValue) throw()
{
	ostringstream lTmpStr;
	lTmpStr.precision(19);
	lTmpStr.setf(ios::scientific, ios::floatfield);
	lTmpStr << inValue;
	mValue = lTmpStr.str();
}

/*! \todo
*/
Converter::operator string() const throw()
{
	return mValue.c_str();
}

/*! \todo
*/
Converter::operator bool() const throw()
{
	bool lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
Converter::operator char() const throw()
{
	char lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
Converter::operator unsigned char() const throw()
{
	unsigned char lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
Converter::operator short() const throw()
{
	short lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
Converter::operator unsigned short() const throw()
{
	unsigned short lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
Converter::operator int() const throw()
{
	int lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
Converter::operator unsigned int() const throw()
{
	unsigned int lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
Converter::operator long() const throw()
{
	long lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
Converter::operator unsigned long() const throw()
{
	unsigned long lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
Converter::operator float() const throw()
{
	float lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
Converter::operator double() const throw()
{
	double lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
Converter::operator long double() const throw()
{
	long double lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
bool Converter::toBool() const throw()
{
	bool lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
char Converter::toChar() const throw()
{
	char lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
unsigned char Converter::toUChar() const throw()
{
	unsigned char lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
short Converter::toShort() const throw()
{
	short lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
unsigned short Converter::toUShort() const throw()
{
	unsigned short lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
int Converter::toInt() const throw()
{
	int lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
unsigned int Converter::toUInt() const throw()
{
	unsigned int lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
long Converter::toLong() const throw()
{
	long lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
unsigned long Converter::toULong() const throw()
{
	unsigned long lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
float Converter::toFloat() const throw()
{
	float lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
double Converter::toDouble() const throw()
{
	double lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}

/*! \todo
*/
long double Converter::toLDouble() const throw()
{
	long double lValue;
	istringstream lTmpStr(mValue);
	lTmpStr >> lValue;
	return lValue;
}
