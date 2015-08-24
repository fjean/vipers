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
 * \file VIPERS/Exception.cpp
 * \brief Exception class source file.
 * \author Frederic Jean
 * $Revision: 252 $
 * $Date: 2009-10-14 21:52:34 -0400 (Wed, 14 Oct 2009) $
 */

#include "Exception.hpp"
#include <sstream>

using namespace VIPERS;
using namespace std;

/*! \todo
*/
Exception::Exception(Code inCode, const string& inWhat)
{
  mCode = inCode;
  mWhat = inWhat.c_str();
  mLine = 0;
}

/*! \todo
*/
Exception::Exception(const Exception& inException)
{
  mCode = inException.mCode;
  mWhat = inException.mWhat.c_str();
  mFrom = inException.mFrom.c_str();
  mFile = inException.mFile.c_str();
  mLine = inException.mLine;

  for(CallStack::const_iterator lCSItr = inException.mCallStack.begin(); lCSItr != inException.mCallStack.end(); lCSItr++)
    mCallStack.push_back(lCSItr->c_str());
}

/*! \todo
*/
Exception& Exception::operator=(const Exception& inException)
{
  mCode = inException.mCode;
  mWhat = inException.mWhat.c_str();
  mFrom = inException.mFrom.c_str();
  mFile = inException.mFile.c_str();
  mLine = inException.mLine;

  mCallStack.clear();
  for(CallStack::const_iterator lCSItr = inException.mCallStack.begin(); lCSItr != inException.mCallStack.end(); lCSItr++)
    mCallStack.push_back(lCSItr->c_str());
}

/*! \todo
*/
Exception& Exception::clear()
{
  mCode = eCodeUndefined;
  mWhat = "";
  mFrom = "";
  mFile = "";
  mLine = 0;
  mCallStack.clear();
  return *this;
}

/*! \todo
*/
Exception& Exception::setCode(Code inCode)
{
  mCode = inCode;
  return *this;
}

/*! \todo
*/
string Exception::getCodeMessage() const
{
	ostringstream lMessage;

	switch(mCode)
	{
		case eCodeUndefined: lMessage << "An unknown error has been caught"; break;
		case eCodeUseModule: lMessage << "An error occurred in a module while using it"; break;
		case eCodeLoadModule: lMessage << "An error occurred while loading a module"; break;
		case eCodeCreateModule: lMessage << "An error occurred while creating a module"; break;
		case eCodeDeleteModule: lMessage << "An error occurred while deleting a module"; break;
		case eCodeAlreadyLoadedModule: lMessage << "Module with the same name already loaded"; break;
		case eCodeBuggyModule: lMessage << "A bug in a module has been detected"; break;
		case eCodeInvalidModule: lMessage << "An invalid module name as been requested"; break;
		case eCodeInvalidSlot: lMessage << "Error while connecting two module slots"; break;
		case eCodeInvalidImageSlot: lMessage << "Error while trying to obtain a pointer to a module slot image"; break;
		case eCodeNotConnectedSlot: lMessage << "The module slot is not connected"; break;
		case eCodeAlreadyConnectedSlot: lMessage << "The module slot is already connected"; break;
		case eCodeInvalidParameter: lMessage << "Parameter does not exist or has an invalid value"; break;
		case eCodeModuleCycleDetected: lMessage << "A cycle has been detected in the graph representing the connected modules"; break;
		case eCodeNotAttachedMonitor: lMessage << "Monitor is not attached to any module"; break;
		case eCodeInvalidModuleSlotMonitor: lMessage << "Invalid module slot requested by monitor"; break;
		case eCodeUnlockedModuleSlotMonitor: lMessage << "Monitor has not locked the module slot"; break;
		case eCodeUseMonitor: lMessage << "Error from Monitor while using it"; break;
		case eCodeInvalidOperationKernelState: lMessage << "Kernel is not in a state where it can perform the requested operation"; break;
		case eCodeInvalidOperationModuleState: lMessage << "Module is not in a state where it can perform the requested operation"; break;
		case eCodeInvalidSizeDimension: lMessage << "Invalid number of dimensions or dimension number"; break;
		case eCodeInvalidColorComponent: lMessage << "Invalid number of components or component number"; break;
		case eCodeIOXML: lMessage << "Invalid while performing input/output of XML data"; break;
		default: lMessage << "Invalid error (SHOULD NOT HAPPEN!)"; break;
	}

	lMessage << " (" << mCode << ")";

	return lMessage.str().c_str();
}

/*! \todo
*/
Exception& Exception::setWhat(const string& inWhat)
{
  mWhat = inWhat.c_str();
  return *this;
}

/*! \todo
*/
Exception& Exception::setFrom(const string& inFrom)
{
  mFrom = inFrom.c_str();
  return *this;
}

/*! \todo
*/
Exception& Exception::setFile(const string& inFile)
{
  mFile = inFile.c_str();
  return *this;
}

/*! \todo
*/
Exception& Exception::setLine(int inLine)
{
  mLine = inLine;
  return *this;
}

/*! \todo
*/
Exception& Exception::setFileLine(const string& inFile, int inLine)
{
  mFile = inFile.c_str();
  mLine = inLine;
  return *this;
}

/*! \todo
*/
Exception& Exception::addEntry(const string& inEntry)
{
  mCallStack.push_back(inEntry.c_str());
  return *this;
}

/*! \todo
*/
string Exception::toString() const
{
  ostringstream lMessage;

  if(!mFrom.empty())
    lMessage << "EXCEPTION from ";
  else
    lMessage << "EXCEPTION: ";

  lMessage << mWhat << endl << "Code: " << getCodeMessage() << endl;

  if(!mFile.empty())
    lMessage << "File: " << mFile << endl;
  if(mLine>0)
    lMessage << "Line: " << mLine << endl;

  if(!mCallStack.empty())
  {
    lMessage << "Call stack: ";
    for(CallStack::const_iterator lCSItr = mCallStack.begin(); lCSItr != mCallStack.end(); lCSItr++)
    {
      lMessage << *lCSItr;
      if(lCSItr!=(mCallStack.end()-1))
        lMessage <<  "/";
      lMessage << endl;
    }
  }

  return lMessage.str().c_str();
}
