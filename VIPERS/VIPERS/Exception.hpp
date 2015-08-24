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
 * \file VIPERS/Exception.hpp
 * \brief Exception class header.
 * \author Frederic Jean
 * $Revision: 242 $
 * $Date: 2009-10-12 16:30:29 -0400 (Mon, 12 Oct 2009) $
 */

#ifndef VIPERS_EXCEPTION_HPP
#define VIPERS_EXCEPTION_HPP

#include <string>
#include <vector>
#include <iostream>

namespace VIPERS
{

	using namespace std;

	/*! \brief %Exception handling class.
		\author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada

		An instance of this object is thrown when VIPERS classes raise exceptions.
		\todo
	*/
	class Exception
	{
	  public:

	    //! Call stack for an exception
	    typedef vector<string> CallStack;

	    /*! \brief Type of errors
      \author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada
	     */
	    enum Code
	    {
	      eCodeUndefined, //!< Undefined exception
	      eCodeUseModule, //!< Error coming from a %Module while using it
	      eCodeLoadModule, //!< Error while loading a VIPERS %Module
	      eCodeCreateModule, //!< Error while creating a %Module
	      eCodeDeleteModule, //!< Error while deleting a %Module
	      eCodeAlreadyLoadedModule, //!< %Module with the same name already loaded
	      eCodeBuggyModule, //!< Error when a bug has been detected in a %Module
	      eCodeInvalidModule, //!< An invalid module name as been requested
	      eCodeInvalidSlot, //!< Error while using an invalid %ModuleSlot (like trying to connect two input slots together)
	      eCodeInvalidImageSlot, //!< Error while trying to obtain a pointer in the %ModuleSlot image
	      eCodeNotConnectedSlot, //!< %ModuleSlot is not connected (to another slot)
	      eCodeAlreadyConnectedSlot, //!< %ModuleSlot is already connected to another slot
	      eCodeInvalidParameter, //!< %Parameter does not exist or has an invalid value
	      eCodeModuleCycleDetected, //!< A cycle has been detected in the graph representing the connected modules
	      eCodeNotAttachedMonitor, //!< Monitor is not attached to any module
	      eCodeInvalidModuleSlotMonitor, //!< Invalid module slot requested by monitor
	      eCodeUnlockedModuleSlotMonitor, //!< Monitor has not locked the slot before accessing it
	      eCodeUseMonitor, //!< Error from Monitor while using it
	      eCodeInvalidOperationKernelState, //!< Kernel is not in a state where it can perform the requested operation
	      eCodeInvalidOperationModuleState, //!< Module is not in a state where it can perform the requested operation
	      eCodeInvalidSizeDimension, //! Invalid number of dimensions or dimension number
	      eCodeInvalidColorComponent, //! Invalid number of components or component number
	      eCodeIOXML //! Invalid while performing input/output of XML data
	    };

	    //! Construct for error code and message
	    Exception(Code inCode=eCodeUndefined, const string& inWhat="");
	    //! Copy constructor
	    Exception(const Exception& inException);
	    //! Assignment operator
	    Exception& operator=(const Exception& inException);

	    //! Clear all information in the exception
	    Exception& clear();

	    //! Set error code
	    Exception& setCode(Code inCode);
	    //! Return error code.
	    inline Code getCode() const {return mCode;}
	    //! Return error code
	    inline operator Code() const {return mCode;}
	    //! Return the general message associated with the code set in the object
	    string getCodeMessage() const;

      //! Return the message set when the exception was raised
	    Exception& setWhat(const string& inWhat);
      //! Return the message set when the exception was raised
      inline string getWhat() const {return mWhat.c_str();}

	    //! Set from
	    Exception& setFrom(const string& inFrom);
	    //! Get from
	    inline string getFrom() const {return mFrom.c_str();}

      //! Set file
      Exception& setFile(const string& inFile);
      //! Get file string
      inline string getFile() const {return mFile.c_str();}
      //! Set line
      Exception& setLine(int inLine);
      //! Get line
      inline int getLine() const {return mLine;}
      //! Set file and line
      Exception& setFileLine(const string& inFile, int inLine);

      //! Get call stack
      inline CallStack getCallStack() const {return mCallStack;}
      //! Add entry on the call stack
      Exception& addEntry(const string& inEntry);

      //! Return formated exception message with all information available
      string toString() const;
      //! Return formated exception message with all information available
      inline operator string() const {return toString();}

      //! Output stream operator
      friend ostream& operator<<(ostream& ioStream, const Exception& inException);

	  private:

	    //! Exception code
      Code mCode;
      //! What string
      string mWhat;
	    //! From string
	    string mFrom;
	    //! File string
	    string mFile;
      //! Line
      int mLine;
      //! Call stack
      CallStack mCallStack;

	};

	/*! \todo
	 */
	inline ostream& operator<<(ostream& ioStream, const Exception& inException) {return ioStream<<inException.toString();}

};

#endif //VIPERS_EXCEPTION_HPP
