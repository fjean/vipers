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
 * \file VIPERS/Color.cpp
 * \brief Color class functions definition.
 * \author Frederic Jean
 * $Revision: 252 $
 * $Date: 2009-10-14 21:52:34 -0400 (Wed, 14 Oct 2009) $
 */

#include "Color.hpp"
#include "Exception.hpp"
#include <sstream>

using namespace VIPERS;
using namespace std;

/*! \todo
*/
Color::Color(double inCmp1, double inCmp2, double inCmp3) throw()
{
	mColor[0] = inCmp1;
	mColor[1] = inCmp2;
	mColor[2] = inCmp3;
}

/*! \todo
*/
Color::Color(double inColor[3]) throw()
{
	mColor[0] = inColor[0];
	mColor[1] = inColor[1];
	mColor[2] = inColor[2];
}

/*! \todo
*/
Color::Color(const Color& inColor) throw()
{
	mColor[0] = inColor.mColor[0];
	mColor[1] = inColor.mColor[1];
	mColor[2] = inColor.mColor[2];
}

/*! \todo
*/
Color::Color(const ColorArray& inColorArray)
{
	if(inColorArray.size()!=3)
		throw(Exception(Exception::eCodeInvalidColorComponent, "Color array does not have 3 components"));
	mColor[0] = inColorArray[0];
	mColor[1] = inColorArray[1];
	mColor[2] = inColorArray[2];
}

/*! \todo
*/
Color::Color(const string& inColorStr)
{
	double lDbl;
	istringstream lStr(inColorStr.c_str());
	ColorArray lColorArray;

	while(lStr >> lDbl)
		lColorArray.push_back(lDbl);

	if(lColorArray.size()!=3)
		throw(Exception(Exception::eCodeInvalidColorComponent, "String does not have 3 color components"));

	mColor[0] = lColorArray[0];
	mColor[1] = lColorArray[1];
	mColor[2] = lColorArray[2];
}

/*! \todo
*/
Color::~Color()
{

}

/*! \todo
*/
ColorArray Color::getColor() const throw()
{
	ColorArray lColorArray;
	lColorArray.push_back(mColor[0]);
	lColorArray.push_back(mColor[1]);
	lColorArray.push_back(mColor[2]);
	return lColorArray;
}

/*! \todo
*/
string Color::getString() const throw()
{
	ostringstream lStr;
	lStr << mColor[0] << " " << mColor[1] << " " << mColor[2];
	return lStr.str().c_str();
}

/*! \todo
*/
void Color::setColor(const ColorArray& inColorArray)
{
	if(inColorArray.size()!=3)
		throw(Exception(Exception::eCodeInvalidColorComponent, "Color array does not have 3 components"));
	mColor[0] = inColorArray[0];
	mColor[1] = inColorArray[1];
	mColor[2] = inColorArray[2];
}

/*! \todo
*/
void Color::setColor(double inColor[3]) throw()
{
	mColor[0] = inColor[0];
	mColor[1] = inColor[1];
	mColor[2] = inColor[2];
}

/*! \todo
*/
void Color::setColor(const string& inColorStr)
{
	double lDbl;
	istringstream lStr(inColorStr.c_str());
	ColorArray lColorArray;

	while(lStr >> lDbl)
		lColorArray.push_back(lDbl);

	if(lColorArray.size()!=3)
		throw(Exception(Exception::eCodeInvalidColorComponent, "String does not have 3 color components"));

	mColor[0] = lColorArray[0];
	mColor[1] = lColorArray[1];
	mColor[2] = lColorArray[2];
}

/*! \todo
*/
double Color::getComponent1() const throw()
{
	return mColor[0];
}

/*! \todo
*/
double Color::getComponent2() const throw()
{
	return mColor[1];
}

/*! \todo
*/
double Color::getComponent3() const throw()
{
	return mColor[2];
}

/*! \todo
*/
void Color::setComponent1(double inCmp) throw()
{
	mColor[0] = inCmp;
}

/*! \todo
*/
void Color::setComponent2(double inCmp) throw()
{
	mColor[1] = inCmp;
}

/*! \todo
*/
void Color::setComponent3(double inCmp) throw()
{
	mColor[2] = inCmp;
}

/*! \todo
*/
double& Color::operator[](unsigned int inCmpIdx)
{
	if(inCmpIdx>2)
		throw(Exception(Exception::eCodeInvalidColorComponent, "Invalid component index"));
	return mColor[inCmpIdx];
}

/*! \todo
*/
Color& Color::operator=(const ColorArray& inColorArray)
{
	if(inColorArray.size()!=3)
		throw(Exception(Exception::eCodeInvalidColorComponent, "Color array does not have 3 components"));
	mColor[0] = inColorArray[0];
	mColor[1] = inColorArray[1];
	mColor[2] = inColorArray[2];

	return *this;
}

/*! \todo
*/
Color& Color::operator=(double inColor[3]) throw()
{
	mColor[0] = inColor[0];
	mColor[1] = inColor[1];
	mColor[2] = inColor[2];

	return *this;
}

/*! \todo
*/
Color& Color::operator=(const string& inColorStr)
{
	double lDbl;
	istringstream lStr(inColorStr.c_str());
	ColorArray lColorArray;

	while(lStr >> lDbl)
		lColorArray.push_back(lDbl);

	if(lColorArray.size()!=3)
		throw(Exception(Exception::eCodeInvalidColorComponent, "String does not have 3 color components"));

	mColor[0] = lColorArray[0];
	mColor[1] = lColorArray[1];
	mColor[2] = lColorArray[2];

	return *this;
}

/*! \todo
*/
bool Color::operator==(const Color& inColor) const throw()
{
	return (mColor[0]==inColor.mColor[0] && mColor[1]==inColor.mColor[1] && mColor[2]==inColor.mColor[2]);
}
