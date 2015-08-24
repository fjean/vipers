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
 * \file VIPERS/ModuleFactory.hpp
 * \brief ModuleFactory class header.
 * \author Frederic Jean
 * $Revision: 252 $
 * $Date: 2009-10-14 21:52:34 -0400 (Wed, 14 Oct 2009) $
 */

#ifndef VIPERS_MODULE_FACTORY_HPP
#define VIPERS_MODULE_FACTORY_HPP

#include "Module.hpp"
#include <string>

namespace VIPERS
{

	using namespace std;

	//! Define a pointer to the function that creates Module
	typedef Module * (*ModuleNewFunctionPtr)();

	//! Define a pointer to the function that deletes Module
	typedef void (*ModuleDeleteFunctionPtr)(Module*);


	/*! \brief %ModuleFactory class.
		\author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada

		\todo
	*/
	class ModuleFactory
	{
		public:

		friend class ModulesManager;

		//! Default constructor
		ModuleFactory() throw();
		//! Constructor that loads a %Module
		ModuleFactory(const string& inFile);
		//! Destructor
		~ModuleFactory();

		//! Init factory with possible %Module contained in a file
		bool init(const string& inFile);
		//! Clear the factory to be ready for use with another %Module
		void clear() throw();
		//! Verify if the factory is initialized
		bool isInit() const throw();

		//! Create a new instance for the %Module handled by the factory
		Module* newModuleInstance();
		//! Delete an instance of a %Module handled by the factory
		void deleteModuleInstance(Module* inModule) const;

		//! Get module name
		string getName() const throw();
		//! Get module display name
		string getDisplayName() const throw();
		//! Get module file name
		string getFileName() const throw();
		//! Get module file path
		string getFilePath() const throw();
		//! Get module short description
		string getShortDescription() const throw();
		//! Get module long description
		string getLongDescription() const throw();
		//! Get module documentation
		string getDocumentation() const throw();
		//! Get %Module version
		string getVersion() const throw();

		private:

        //! Restrict (disable) copy constructor
        ModuleFactory(const ModuleFactory&);
        //! Restrict (disable) assignment operator
        void operator=(const ModuleFactory&);

		//! Set file name and file path from string
		void setFile(const string& inFilePath) throw();

		bool mIsInit; //!< Is factory initialized?
		string mName; //!< Module unique name
		string mDisplayName; //!< Module display name
		string mVersion; //!< Module version
		string mFileName; //!< Module file name
		string mFilePath; //!< Module file path
		string mShortDescription; //!< Module short description
		string mLongDescription; //!< Module long description
		string mDocumentation; //!< Module documentation

        unsigned int mIDCounter; //!< Module ID counter used to generate unique label

		void* mDynamicLibraryInstance; //!< Opaque pointer for a %Module library instance object (OS dependent)
		ModuleNewFunctionPtr newModuleInstanceFunction; //!< Pointer to the function in the library that creates a new Module instance
		ModuleDeleteFunctionPtr deleteModuleInstanceFunction; //! Pointer to the function in the library that deletes a Module instance

	};

}


#endif //VIPERS_MODULE_FACTORY_HPP
