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
 * \file Size.cpp
 * \brief Size class functions definition.
 * \author Frederic Jean
 * $Revision: 252 $
 * $Date: 2009-10-14 21:52:34 -0400 (Wed, 14 Oct 2009) $
 */

#include "Size.hpp"
#include "Exception.hpp"
#include <sstream>
#include <vector>

using namespace VIPERS;
using namespace std;

/*! \todo
*/
Size::Size(unsigned int inNbDimensions)
{
	if(inNbDimensions)
	{
		mNbDimensions = inNbDimensions;
		mSizeArray = new unsigned int[mNbDimensions];
	}
	else
		throw(Exception(Exception::eCodeInvalidSizeDimension, "Cannot have 0 dimension"));
}

/*! \todo
*/
Size::Size(unsigned int inDim1, unsigned int inDim2) throw()
{
	mNbDimensions = 2;
	mSizeArray = new unsigned int[2];
	mSizeArray[0] = inDim1;
	mSizeArray[1] = inDim2;
}

/*! \todo
*/
Size::Size(const string& inSizeStr)
{
	istringstream lInStr(inSizeStr.c_str());
	vector<unsigned int> lTmpArray;
	unsigned int lTmpDim;

	while(lInStr >> lTmpDim)
		lTmpArray.push_back(lTmpDim);

	if(lTmpArray.size()==0)
		throw(Exception(Exception::eCodeInvalidSizeDimension, "Cannot have 0 dimension (string empty)"));

	mNbDimensions = lTmpArray.size();
	mSizeArray = new unsigned int[mNbDimensions];
	for(unsigned int lDim = 0; lDim<mNbDimensions; lDim++)
		mSizeArray[lDim] = lTmpArray[lDim];
}

/*! \todo
*/
Size::Size(const Size& inSize) throw()
{
	mNbDimensions = inSize.mNbDimensions;
	mSizeArray = new unsigned int[mNbDimensions];
	for(unsigned int lDim = 0; lDim<mNbDimensions; lDim++)
		mSizeArray[lDim] = inSize.mSizeArray[lDim];
}

/*! \todo
*/
Size::~Size()
{
	if(mSizeArray)
		delete [] mSizeArray;
}

/*! \todo
*/
unsigned int& Size::operator[](unsigned int inDimension)
{
	if(inDimension < mNbDimensions)
		return mSizeArray[inDimension];
	else
		throw(Exception(Exception::eCodeInvalidSizeDimension, "Dimension does not exist"));
}

/*! \todo
*/
unsigned int Size::getNbDimension() const throw()
{
	return mNbDimensions;
}

/*! \todo
*/
Size& Size::operator=(const Size& inSize)
{
	if(inSize.mNbDimensions!=mNbDimensions)
		throw(Exception(Exception::eCodeInvalidSizeDimension, "Size objects does not have the same number of dimensions"));
	for(unsigned int lDim = 0; lDim<mNbDimensions; lDim++)
		mSizeArray[lDim] = inSize.mSizeArray[lDim];
	return *this;
}

/*! \todo
*/
Size& Size::operator=(const string& inSizeStr)
{
	istringstream lInStr(inSizeStr);
	vector<unsigned int> lTmpArray;
	unsigned int lTmpDim;

	while(lInStr >> lTmpDim)
		lTmpArray.push_back(lTmpDim);

	if(lTmpArray.size()!=mNbDimensions)
		throw(Exception(Exception::eCodeInvalidSizeDimension, "Size object and string does not have the same number of dimensions"));

	for(unsigned int lDim = 0; lDim<mNbDimensions; lDim++)
		mSizeArray[lDim] = lTmpArray[lDim];
	return *this;
}

/*! \todo
*/
string Size::getString() const throw()
{
	ostringstream lStr;
	for(unsigned int lDim = 0; lDim<mNbDimensions; lDim++)
		lStr << mSizeArray[lDim] << " ";
	return lStr.str().c_str();
}

/*! \todo
*/
bool Size::operator==(const Size& inSize) const throw()
{
	if(mNbDimensions!=inSize.mNbDimensions)
		return false;

	for(unsigned int lDim = 0; lDim<mNbDimensions; lDim++)
		if(mSizeArray[lDim]!=inSize.mSizeArray[lDim])
			return false;

	return true;
}

/*! \todo
*/
bool Size::operator!=(const Size& inSize) const throw()
{
	if(mNbDimensions!=inSize.mNbDimensions)
		return true;

	for(unsigned int lDim = 0; lDim<mNbDimensions; lDim++)
		if(mSizeArray[lDim]!=inSize.mSizeArray[lDim])
			return true;

	return false;
}
