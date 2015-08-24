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
 * \file VIPERS/ModulesManager.hpp
 * \brief ModulesManager class header.
 * \author Frederic Jean
 * $Revision: 252 $
 * $Date: 2009-10-14 21:52:34 -0400 (Wed, 14 Oct 2009) $
 */

#ifndef VIPERS_MODULES_MANAGER_HPP
#define VIPERS_MODULES_MANAGER_HPP

#include "ModuleFactory.hpp"

#include <vector>
#include <map>
#include <string>

namespace VIPERS
{

	using namespace std;

	//! Map of %ModuleFactory
	typedef map<string, ModuleFactory*>	ModuleFactoryMap;

	//! List of paths
	typedef vector<string>	PathList;


	/*! \brief %ModulesManager class.
		\author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada

		\todo
	*/
	class ModulesManager
	{
		public:

		//! Default constructor
		ModulesManager() throw();
		//! Default constructor with a list of paths
		ModulesManager(const PathList& inPathList) throw();
		//! Destructor
		~ModulesManager();
		//! Clear all loaded modules
		void clear();

		//! Get an instance to a module with given name
		Module* newModule(const string& inName);
		//! Delete an instance of a module
		void deleteModule(Module* inModule);
		//! Get a map of %ModuleFactory for loaded modules
		const ModuleFactoryMap& getModuleFactoryMap() const throw();
		//! Get number of allocated modules
		unsigned int getAllocatedModulesCount() const throw();

		//! Load modules in path list
		void loadModulePathList() throw();
		//! Load modules in the specified path
		void loadModulePath(const string& inPath) throw();
        //! Load module from the specified file
        bool loadModuleFile(const string& inFile) throw();

		//! Add a path to the paths list
		void addPath(const string& inPath) throw();
		//! Add a paths to the paths list
		void addPaths(const PathList& inPathList) throw();
		//! Get paths list
		PathList getPathList() const throw();

		private:

		//! Restrict (disable) copy constructor
		ModulesManager(const ModulesManager&);
		//! Restrict (disable) assignment operator
		void operator=(const ModulesManager&);

		//! Add module to the list of loaded modules
		void addModuleFactory(ModuleFactory* inModuleFactory);
		//! Load a module in the specified file
		void loadModule(const string& inFile);
		//! Add trailing file separator ("\\" or "/") to string if not present
		string convertPath(const string& inString);

		unsigned int mAllocatedModulesCount; //!< Count of allocated modules
		ModuleFactoryMap mModuleFactoryMap; //!< %ModuleFactory map
		PathList mPathList; //!< List of path that are scanned for modules

	};

}

#endif //VIPERS_MODULES_MANAGER_HPP
