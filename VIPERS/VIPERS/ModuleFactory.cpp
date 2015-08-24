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
 * \file VIPERS/ModuleFactory.cpp
 * \brief ModuleFactory class functions definition.
 * \author Frederic Jean
 * $Revision: 260 $
 * $Date: 2009-10-18 15:23:53 -0400 (Sun, 18 Oct 2009) $
 */

#include "ModuleFactory.hpp"
#include "VIPERSConfig.hpp"
#include "VIPERS.hpp"
#include <sstream>
#include <iostream>

// Header of functions for loading dynamic libraries
#ifdef VIPERS_OS_WINDOWS
	// On Windows
	#include <windows.h>
#else
	// On most Unixes
	#include <dlfcn.h>
#endif

using namespace VIPERS;
using namespace std;

/*! \todo
*/
ModuleFactory::ModuleFactory() throw()
{
	mIsInit = false;
	mDynamicLibraryInstance = NULL;
	newModuleInstanceFunction = NULL;
	deleteModuleInstanceFunction = NULL;
	mIDCounter = 0;
}

/*! \todo
*/
ModuleFactory::ModuleFactory(const string &inFile)
{
	mIsInit = false;
	mDynamicLibraryInstance = NULL;
	newModuleInstanceFunction = NULL;
	deleteModuleInstanceFunction = NULL;

	try
	{
		mIsInit = init(inFile.c_str());
	}
	catch(...)
	{
		clear();
		throw;
	}
}

/*! \todo
*/
ModuleFactory::~ModuleFactory()
{
	clear();
}

/*! \todo
*/
bool ModuleFactory::init(const string& inFile)
{
	Module *lTmpModule = NULL;
	clear();

#ifdef VIPERS_OS_WINDOWS
	mDynamicLibraryInstance = (HINSTANCE)LoadLibraryA((LPCSTR)inFile.c_str());
#else
	mDynamicLibraryInstance = dlopen(inFile.c_str(), RTLD_LAZY);
#endif

	if(!mDynamicLibraryInstance)
		return false;
	
#ifdef VIPERS_OS_WINDOWS
	newModuleInstanceFunction = (ModuleNewFunctionPtr) GetProcAddress((HINSTANCE)mDynamicLibraryInstance, NEW_VIPERS_MODULE_FUNC_NAME);
	deleteModuleInstanceFunction = (ModuleDeleteFunctionPtr) GetProcAddress((HINSTANCE)mDynamicLibraryInstance, DELETE_VIPERS_MODULE_FUNC_NAME);
#else
	newModuleInstanceFunction = (ModuleNewFunctionPtr) dlsym(mDynamicLibraryInstance, NEW_VIPERS_MODULE_FUNC_NAME);
	deleteModuleInstanceFunction = (ModuleDeleteFunctionPtr) dlsym(mDynamicLibraryInstance, DELETE_VIPERS_MODULE_FUNC_NAME);
#endif

	if(!newModuleInstanceFunction || !deleteModuleInstanceFunction)
	{
		clear();
		return false;
	}

	try
	{
		lTmpModule = newModuleInstanceFunction();
		if(!lTmpModule)
		{
			clear();
			return false;
		}
		mName = lTmpModule->getName().c_str();
		mDisplayName = lTmpModule->getDisplayName().c_str();
		mVersion = lTmpModule->getVersion().c_str();
		mShortDescription = lTmpModule->getShortDescription().c_str();
		mLongDescription = lTmpModule->getLongDescription().c_str();
		mDocumentation = lTmpModule->getDocumentation().c_str();		
		deleteModuleInstanceFunction(lTmpModule);
	}
	catch(runtime_error lErrorRunTime)
	{
		throw(Exception(Exception::eCodeLoadModule, lErrorRunTime.what() + string(" (File: ") + inFile.c_str() + string(")") ));
	}
	catch(...)
	{
		throw;
	}

	//Split file in file name and file path
	setFile(inFile.c_str());

	mIsInit = true;
	return true;
}

/*! \todo
*/
void ModuleFactory::clear() throw()
{
	if(mDynamicLibraryInstance)
	{
#ifdef VIPERS_OS_WINDOWS
		FreeLibrary((HINSTANCE)mDynamicLibraryInstance);
#else
		dlclose(mDynamicLibraryInstance);
#endif
	}

	mDynamicLibraryInstance = NULL;
	newModuleInstanceFunction = NULL;
	deleteModuleInstanceFunction = NULL;
	mFileName = "";
	mFilePath = "";
	mName = "";
	mDisplayName = "";
	mVersion = "";
	mShortDescription = "";
	mLongDescription = "";
	mDocumentation = "";
	mIsInit = false;
	mIDCounter = 0;
}

/*! \todo
*/
bool ModuleFactory::isInit() const throw()
{
	return mIsInit;
}

/*! \todo
*/
Module* ModuleFactory::newModuleInstance()
{
	if(mIsInit && newModuleInstanceFunction)
	{
	    Module* lTmpModule = NULL;
	    ostringstream lLabelSStr;
		try
		{
		    mIDCounter++;
		    lTmpModule = newModuleInstanceFunction();
            lLabelSStr << mName.c_str() << mIDCounter;
            lTmpModule->setLabel(lLabelSStr.str().c_str());
			return lTmpModule;
		}
		catch(runtime_error lErrorRunTime)
		{
		     mIDCounter--;
			throw(Exception(Exception::eCodeCreateModule, lErrorRunTime.what()));
		}
		catch(...)
		{
		     mIDCounter--;
			throw;
		}
	}
	return NULL;
}

/*! \todo
*/
void ModuleFactory::deleteModuleInstance(Module* inModule) const
{
	if(mIsInit && deleteModuleInstanceFunction)
	{
		try
		{
			deleteModuleInstanceFunction(inModule);
		}
		catch(runtime_error lErrorRunTime)
		{
			throw(Exception(Exception::eCodeDeleteModule, lErrorRunTime.what()));
		}
		catch(...)
		{
			throw;
		}
	}
	else
	{
		throw(Exception(Exception::eCodeDeleteModule, "Factory has not been initialized"));
	}
}

/*! \todo
*/
string ModuleFactory::getName() const throw()
{
		return mName.c_str();
}

/*! \todo
*/
string ModuleFactory::getDisplayName() const throw()
{
		return mDisplayName.c_str();
}

/*! \todo
*/
string ModuleFactory::getFileName() const throw()
{
		return mFileName.c_str();
}

/*! \todo
*/
string ModuleFactory::getFilePath() const throw()
{
		return mFilePath.c_str();
}

/*! \todo
*/
string ModuleFactory::getShortDescription() const throw()
{
	return mShortDescription.c_str();
}

/*! \todo
*/
string ModuleFactory::getLongDescription() const throw()
{
	return mLongDescription.c_str();
}

/*! \todo
*/
string ModuleFactory::getDocumentation() const throw()
{
	return mDocumentation.c_str();
}

/*! \todo
*/
string ModuleFactory::getVersion() const throw()
{
	return mVersion.c_str();
}

/*! \todo
*/
void ModuleFactory::setFile(const string& inFilePath) throw()
{
	#ifdef VIPERS_OS_WINDOWS
		char lSep = '\\';
	#else
		char lSep = '/';
	#endif

	size_t lSepPos = inFilePath.rfind(lSep);
	if(lSepPos!=string::npos && lSepPos!=(inFilePath.size()-1))
	{
		mFilePath = inFilePath.substr(0, lSepPos+1).c_str();
		mFileName = inFilePath.substr(lSepPos+1, string::npos).c_str();
	}
	else
	{
		mFileName = inFilePath.c_str();
		mFilePath = "";
	}
}
