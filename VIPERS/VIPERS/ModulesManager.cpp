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
 * \file VIPERS/ModulesManager.cpp
 * \brief ModulesManager class functions definition.
 * \author Frederic Jean
 * $Revision: 252 $
 * $Date: 2009-10-14 21:52:34 -0400 (Wed, 14 Oct 2009) $
 */

#include "ModulesManager.hpp"
#include "VIPERSConfig.hpp"

#include <algorithm>
#include <iostream>

// Functions header for scanning directories
#ifdef WIN32
  // On Windows
  #include <windows.h>
#else
  // On Unixes
  #include <sys/types.h>
  #include <dirent.h>
#endif

using namespace VIPERS;
using namespace std;

/*! \todo
*/
ModulesManager::ModulesManager() throw()
{
	mAllocatedModulesCount = 0;
	mPathList.push_back(getModulesDefaultPath().c_str());
}

/*! \todo
*/
ModulesManager::ModulesManager(const PathList& inPathList) throw()
{
	mAllocatedModulesCount = 0;
	mPathList.push_back(getModulesDefaultPath().c_str());
	addPaths(inPathList);
}

/*! \todo
*/
ModulesManager::~ModulesManager()
{
	clear();
	mPathList.clear();
}

/*! \todo
*/
void ModulesManager::clear()
{
    ModuleFactoryMap::iterator lModuleFactoryPtr = mModuleFactoryMap.begin();
    for(lModuleFactoryPtr; lModuleFactoryPtr!=mModuleFactoryMap.end(); lModuleFactoryPtr++)
    {
        try
        {
            delete lModuleFactoryPtr->second;
        }
        catch(runtime_error lErrorRunTime)
		{
            cerr << "ERROR: An runtime error occured while freeing the factory for module \"" << lModuleFactoryPtr->second->getName().c_str() << "\": "<< lErrorRunTime.what() << endl;
        }
        catch(...)
        {
            cerr << "ERROR: An unknown error occured while freeing the factory for module \"" << lModuleFactoryPtr->second->getName().c_str() << "\"" << endl;
        }
    }
    mModuleFactoryMap.clear();
}

/*! \todo
*/
Module* ModulesManager::newModule(const string& inName)
{
	Module* lTmpModule = NULL;
    ModuleFactoryMap::const_iterator lModuleFactoryPtr = mModuleFactoryMap.find(inName.c_str());

	if(lModuleFactoryPtr != mModuleFactoryMap.end())
	{
	    try
	    {
            lTmpModule = lModuleFactoryPtr->second->newModuleInstance();
	    }
	    catch(...)
	    {
	        throw;
	    }
		mAllocatedModulesCount++;
	}

	return lTmpModule;
}

/*! \todo
*/
void ModulesManager::deleteModule(Module* inModule)
{
    if(inModule)
    {
        ModuleFactoryMap::const_iterator lModuleFactoryPtr = mModuleFactoryMap.find(inModule->getName().c_str());
        if(lModuleFactoryPtr != mModuleFactoryMap.end())
        {
            try
            {
                lModuleFactoryPtr->second->deleteModuleInstance(inModule);
            }
            catch(...)
            {
                throw;
            }
            mAllocatedModulesCount--;
        }
        else
        {
            throw(Exception(Exception::eCodeDeleteModule, string("Module \"") + inModule->getName().c_str() + string("\" does not have a corresponding factory")).setFrom("ModulesManager::deleteModule").setFileLine(__FILE__,__LINE__));
        }
    }
}

/*! \todo
*/
const ModuleFactoryMap& ModulesManager::getModuleFactoryMap() const throw()
{
	return mModuleFactoryMap;
}

/*! \todo
*/
unsigned int ModulesManager::getAllocatedModulesCount() const throw()
{
	return mAllocatedModulesCount;
}

/*! \todo
*/
void ModulesManager::loadModulePathList() throw()
{
	PathList::const_iterator lPathItr;
	string lTmpPaths;

	for(lPathItr=mPathList.begin(); lPathItr!=mPathList.end(); lPathItr++)
		loadModulePath(*lPathItr);
}

/*! \todo
*/
void ModulesManager::loadModulePath(const string& inPath) throw()
{
	string lTmpPath = convertPath(inPath.c_str());
	string lTmpFileName;

//WINDOWS SPECIFIC
#ifdef WIN32

	//Search for all DLL file in the directory
	WIN32_FIND_DATAA lFindData;
	HANDLE lHdlFind = ::FindFirstFileA((LPCSTR)((lTmpPath + "*.dll").c_str()), &lFindData);

	if(lHdlFind==INVALID_HANDLE_VALUE)
	{
		cerr << "ERROR while scanning for modules in path " << lTmpPath.c_str() << " (invalid path?)" << endl;
		return;
	}

	//Loop on all DLL file in the path
	do
	{
		lTmpFileName = lTmpPath + lFindData.cFileName;

		if(!(lFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
		{
			try
			{
				loadModule(lTmpFileName.c_str());
			}
			catch(runtime_error lErrorRunTime)
			{
				cerr << "ERROR: " << lErrorRunTime.what() << endl;
				continue;
			}
			catch(...)
			{
				continue;
			}

		}

	} while(::FindNextFileA(lHdlFind, &lFindData));

//UNIXES (POSIX) SPECIFIC
#else

	//Search for all SO files in the directory
  DIR *lDirTmp = NULL;
  struct dirent *lEntryTmp = NULL;
  char lCurrentDirBackup[1024];
  char lCurrentDir[1024];

	//Backup the current directory location
  getcwd(lCurrentDirBackup, 1024);

  //Change dir to original directory (in case of relative directories)
  chdir(lCurrentDirBackup);

  //Open the directory
  lDirTmp = opendir(lTmpPath.c_str());

  //Change current directory to that directory
  if(lDirTmp && chdir(lTmpPath.c_str())==0)
  {
    //Try to load all .so files in this directory
    while( (lEntryTmp = readdir(lDirTmp)) !=NULL)
    {
	getcwd(lCurrentDir, 1024);
	lTmpFileName = lEntryTmp->d_name;

	if(lTmpFileName.size()>3 && lTmpFileName.substr(lTmpFileName.size()-3,lTmpFileName.size()-1)==".so")
	{
		try
		{
			loadModule(lCurrentDir + string("/") + lTmpFileName.c_str());
		}
		catch(runtime_error lErrorRunTime)
		{
			cerr << "ERROR: " << lErrorRunTime.what() << endl;
			continue;
		}
		catch(...)
		{
			continue;
		}
      }
    }

    //Close the directory
    closedir(lDirTmp);

	}
	else
	{
		cerr << "ERROR while scanning for modules in path " << lTmpPath << " (invalid path?)" << endl;
	}

  // Change for the original directory
  chdir(lCurrentDirBackup);

#endif

}

/*! \todo
*/
bool ModulesManager::loadModuleFile(const string& inFile) throw()
{
	try
	{
		loadModule(inFile.c_str());
	}
	catch(runtime_error lErrorRunTime)
	{
		cerr << "ERROR: " << lErrorRunTime.what() << endl;
		return false;
	}
	catch(...)
	{
		return false;
	}

	return true;
}

/*! \todo
*/
void ModulesManager::loadModule(const string& inFile)
{
	ModuleFactory* lTmpFactory = new ModuleFactory();
	
	try
	{
		if(lTmpFactory->init(inFile.c_str()))
			addModuleFactory(lTmpFactory);
		else
			delete lTmpFactory;
	}
	catch(...)
	{
		delete lTmpFactory;
		throw;
	}

}

/*! \todo
*/
void ModulesManager::addModuleFactory(ModuleFactory* inModuleFactory)
{
	ModuleFactoryMap::iterator lTmpFactoryItr = mModuleFactoryMap.find(inModuleFactory->getName().c_str());

	if(lTmpFactoryItr!=mModuleFactoryMap.end() && lTmpFactoryItr->second->getFileName()!=inModuleFactory->getFileName() &&  lTmpFactoryItr->second->getFilePath()!=inModuleFactory->getFilePath())
	{
		throw(Exception(Exception::eCodeAlreadyLoadedModule, string("Module \"") + inModuleFactory->getName().c_str() + string("\" is already loaded from \"") + lTmpFactoryItr->second->getFilePath().c_str() + lTmpFactoryItr->second->getFileName().c_str() + "\"").setFrom("ModulesManager::addModuleFactory").setFileLine(__FILE__,__LINE__));
	}

	mModuleFactoryMap.insert( pair<string, ModuleFactory*>(inModuleFactory->getName(), inModuleFactory) );
}

/*! \todo
*/
void ModulesManager::addPath(const string& inPath) throw()
{
	mPathList.push_back(inPath.c_str());
}

/*! \todo
*/
void ModulesManager::addPaths(const PathList& inPathList) throw()
{
	for(PathList::const_iterator lItr = inPathList.begin(); lItr != inPathList.end(); lItr++)
		mPathList.push_back(lItr->c_str());
}

/*! \todo
*/
PathList ModulesManager::getPathList() const throw()
{
	PathList lPathList;
	for(PathList::const_iterator lItr = mPathList.begin(); lItr != mPathList.end(); lItr++)
		lPathList.push_back(lItr->c_str());
	return lPathList;
}

/*! \todo
*/
string ModulesManager::convertPath(const string& inString)
{
	string lTmpStr = inString.c_str();

	if(lTmpStr.empty())
		return lTmpStr.c_str();

#ifdef WIN32

	if(lTmpStr[lTmpStr.size()-1]!='\\')
		lTmpStr += "\\";

#else

	if(lTmpStr[lTmpStr.size()-1]!='/')
		lTmpStr += "/";

#endif

	return lTmpStr.c_str();
}
