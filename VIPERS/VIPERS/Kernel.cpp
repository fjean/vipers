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
 * \file VIPERS/Kernel.cpp
 * \brief Kernel class functions definition.
 * \author Frederic Jean
 * $Revision: 275 $
 * $Date: 2009-10-23 22:00:34 -0400 (Fri, 23 Oct 2009) $
 */

#include "Kernel.hpp"
#include "VIPERS.hpp"

#include <iostream>
#include <sstream>
#include <vector>

using namespace VIPERS;
using namespace std;

/*! \todo
*/
Kernel::Kernel()
{
  mKernelStateNotifier = NULL;
}

/*! \todo
*/
Kernel::~Kernel()
{
	clearModules();
	mModulesManager.clear();
}

/*! \todo
*/
void Kernel::clear()
{
	clearModules();
	mModulesManager.clear();
}

/*! \todo
*/
void Kernel::clearModules() throw()
{
	ModuleSet::iterator lModuleItr = mModuleSet.begin();
	string lModuleName;
	for(lModuleItr; lModuleItr!=mModuleSet.end(); lModuleItr++)
	{
	  try
	  {
	    lModuleName = (*lModuleItr)->getName();
	    (*lModuleItr)->disconnectAll();
	    mModulesManager.deleteModule(*lModuleItr);
	  }
	  catch(Exception inException)
	  {
	    cerr << inException << endl;
	  }
	  catch(...)
	  {
	    cerr << "ERROR while deleting module \"" << lModuleName.c_str() << "\"" << endl;
	  }
	}
	mModuleSet.clear();
}

/*! \todo
*/
Module* Kernel::newModule(const string& inName)
{
	Module* lModule = NULL;
	KernelState lState = getState();

	if(lState==KernelState::eStateStarted || lState==KernelState::eStatePaused)
		throw(Exception(Exception::eCodeInvalidOperationKernelState, "Cannot add new module while kernel is running").setFrom("Kernel::newModule").setFileLine(__FILE__, __LINE__));

	try
	{
		lModule = mModulesManager.newModule(inName.c_str());
	}
	catch(...)
	{
		throw;
	}

	if(!lModule)
		throw(Exception(Exception::eCodeInvalidModule, string("There is no module named \"") + inName.c_str() + string("\".")).setFrom("Kernel::newModule").setFileLine(__FILE__, __LINE__));

	mModuleSet.insert(lModule);

	//lState = Kernel::State::eProcessStateInitial;
	//setState(lState);

	return lModule;
}

/*! \todo
*/
void Kernel::deleteModule(Module* inModule)
{
	KernelState lState = getState();

	if(lState==KernelState::eStateStarted || lState==KernelState::eStatePaused)
		throw(Exception(Exception::eCodeInvalidOperationKernelState, "Cannot delete module while kernel is running").setFrom("Kernel::deleteModule").setFileLine(__FILE__, __LINE__));

	try
	{
		mModulesManager.deleteModule(inModule);
		mModuleSet.erase(inModule);
	}
	catch(...)
	{
		throw;
	}

	//lState = Kernel::State::eProcessStateInitial;
	//setState(lState);

}

/*! \todo
*/
void Kernel::loadModulePathList() throw()
{
	mModulesManager.loadModulePathList();
}

/*! \todo
*/
void Kernel::loadModulePath(const string& inPath) throw()
{
	mModulesManager.loadModulePath(inPath.c_str());
}

/*! \todo
*/
bool Kernel::loadModuleFile(const string& inFile) throw()
{
	mModulesManager.loadModuleFile(inFile.c_str());
}

/*! \todo
*/
void Kernel::addPath(const string& inPath) throw()
{
	mModulesManager.addPath(inPath.c_str());
}

/*! \todo
*/
void Kernel::addPaths(const PathList& inPathList) throw()
{
	mModulesManager.addPaths(inPathList);
}

/*! \todo
*/
const ModulesManager& Kernel::getModulesManager() const throw()
{
	return mModulesManager;
}

/*! \todo
*/
ModuleSet Kernel::getModules() const throw()
{
	return mModuleSet;
}

/*! \todo
*/
Module* Kernel::getModule(const string& inLabel) const throw()
{
	Module* lModule = NULL;
	for(ModuleSet::iterator lModuleItr = mModuleSet.begin(); lModuleItr != mModuleSet.end(); lModuleItr++)
	{
		if((*lModuleItr)->getLabel()==inLabel)
		{
			lModule = (*lModuleItr);
			break;
		}
	}
	return lModule;
}

/*! \todo
*/
unsigned int Kernel::getModuleCount() const throw()
{
  return mModuleSet.size();
}

/*! \todo
*/
KernelState Kernel::getState() const throw()
{
	KernelState lState;

	mStateMutex.lock();
	lState = mState;
	mStateMutex.unlock();

	return lState;
}

/*! \todo
*/
void Kernel::setKernelStateNotifier(KernelStateNotifier* inKernelStateNotifier) throw()
{
  mKernelStateNotifierMutex.lock();
  mKernelStateNotifier = inKernelStateNotifier;
  mKernelStateNotifierMutex.unlock();
}

/*! \todo
*/
KernelStateNotifier* Kernel::getKernelStateNotifier() const throw()
{
  KernelStateNotifier* lKernelStateNotifier;
  mKernelStateNotifierMutex.lock();
  lKernelStateNotifier=mKernelStateNotifier;
  mKernelStateNotifierMutex.unlock();
  return lKernelStateNotifier;
}

/*! \todo
*/
void Kernel::detectModuleGraphCycle() const
{
	ModuleSlotPair lTmpModuleSlotPair;
	ModuleSlotPairStack lModulesStack;
	ModuleSlotPairStack lModulesCyclePath;
	ModuleSetConst::const_iterator lSearchModuleSetItr;
	ModuleSet::iterator lModuleItr;
	ModuleSlotMap::const_iterator lModuleSlotMapItr;
	ModuleSlotSet lTmpModuleSlotSet;
	ModuleSlotSet::const_iterator lModuleSlotSetItr;
	ModuleSlotSet lTmpModuleSlotSet1;
	ModuleSlotSet::const_iterator lModuleSlotSetItr1;
	const Module* lTmpModulePtr;

	// Loop on each instanciated modules, verify for each module if there is a graph cycle
	// in the slots connections that would make the processing come back to this module
	for(lModuleItr = mModuleSet.begin(); lModuleItr != mModuleSet.end(); lModuleItr++)
	{
		//cout << "Checking cycles for Module " << (*lModuleItr)->getLabel() << endl;

		// Add all module's output slots on the stack
		const ModuleSlotMap& lTmpModuleSlotMap = (*lModuleItr)->getOutputSlots();
		for(lModuleSlotMapItr = lTmpModuleSlotMap.begin(); lModuleSlotMapItr != lTmpModuleSlotMap.end(); lModuleSlotMapItr++)
			if(lModuleSlotMapItr->second->isConnected())
				lModulesStack.push_back(ModuleSlotPair(*lModuleItr, lModuleSlotMapItr->second));

		// Loop on stack until it is empty
		while(!lModulesStack.empty())
		{
			// Get the element on the top of the stack and remove it
			lTmpModuleSlotPair = lModulesStack.back();
			lModulesStack.pop_back();

			// Add current module/slot element in the path
			lModulesCyclePath.push_back(lTmpModuleSlotPair);

			//cout << "    Checking " << lTmpModuleSlotPair.second->getFullName() << endl;

			// Get slots connected to the current module's output slot and loop on them
			lTmpModuleSlotSet = lTmpModuleSlotPair.second->getConnectedSlots();
			if(lTmpModuleSlotSet.size()>0)
			{
				for(lModuleSlotSetItr = lTmpModuleSlotSet.begin(); lModuleSlotSetItr != lTmpModuleSlotSet.end(); lModuleSlotSetItr++)
				{
					//cout << "       Connected to " << (*lModuleSlotSetItr)->getFullName() << endl;
					lTmpModulePtr = (*lModuleSlotSetItr)->getModule();

					if(lTmpModulePtr!=*lModuleItr)
					{
						// Add the module's output slots on the stack
						const ModuleSlotMap& lTmpModuleSlotMap1 = lTmpModulePtr->getOutputSlots();

						for(lModuleSlotMapItr = lTmpModuleSlotMap1.begin(); lModuleSlotMapItr != lTmpModuleSlotMap1.end(); lModuleSlotMapItr++)
						{
							if(lModuleSlotMapItr->second->isConnected())
							{
								//cout << "                Adding " << lModuleSlotMapItr->second->getFullName() << endl;
								lModulesStack.push_back(ModuleSlotPair(lTmpModulePtr, lModuleSlotMapItr->second));
							}
						}
					}
					else
					{
						// Module has seen before: cycle detected!!
						// Construct a string that explains the cycle for the exception that will be raised
						ostringstream lPathStr;
						const ModuleSlot* lTmpModuleSlot;
						ModuleSlotPairStack::iterator lModulesCyclePathItr = lModulesCyclePath.begin();
						ModuleSlotPairStack::iterator lModulesCyclePathItr1;

						for(lModulesCyclePathItr; lModulesCyclePathItr != lModulesCyclePath.end(); lModulesCyclePathItr++)
						{
							lPathStr << lModulesCyclePathItr->first->getLabel().c_str()
							         << "(" << lModulesCyclePathItr->second->getName().c_str() << ")->";

							lModulesCyclePathItr1 = lModulesCyclePathItr + 1;
							if(lModulesCyclePathItr1==lModulesCyclePath.end())
								lModulesCyclePathItr1 = lModulesCyclePath.begin();

							//Search the input slot connected to the current output slot in the next module in the path
							lTmpModuleSlot = NULL;
							const ModuleSlotMap& lTmpModuleSlotMap2 = lModulesCyclePathItr1->first->getInputSlots();
							for(lModuleSlotMapItr = lTmpModuleSlotMap2.begin(); lModuleSlotMapItr != lTmpModuleSlotMap2.end(); lModuleSlotMapItr++)
							{
								if(lModuleSlotMapItr->second->isConnected())
								{
									lTmpModuleSlotSet1 = lModuleSlotMapItr->second->getConnectedSlots();
									VIPERS_ASSERT(lTmpModuleSlotSet1.size()!=0, "Input slot reported as connected but the set of connected slots is empty");

									if((*lTmpModuleSlotSet1.begin())->getModule()==lModulesCyclePathItr->first)
									{
										lTmpModuleSlot = lModuleSlotMapItr->second;
										break;
									}
								}
							}

							// Assert that the input slot has been found (it should, otherwise this is a bug)
							VIPERS_ASSERT(lTmpModuleSlot!=NULL, "Cannot find input slot connected to output slot based on the module pointer.");

							lPathStr << lTmpModuleSlot->getModule()->getLabel().c_str()
									 << "(" << lTmpModuleSlot->getName().c_str() << ")";
							if(lModulesCyclePathItr1!=lModulesCyclePath.begin())
								lPathStr << " : ";
						}

						throw(Exception(Exception::eCodeModuleCycleDetected, string("Cycle detected in module graph: ") + lPathStr.str().c_str()).setFrom("Kernel::detectModuleGraphCycle").setFileLine(__FILE__,__LINE__));
					}
				}
			}
			else
			{
				// Remove current module/slot element in the path
				// The current module/slot element is a dead end (the output slot is not connected)
				lModulesCyclePath.pop_back();
			}
		}
	}
}

/*! \todo
*/
void Kernel::setState(KernelState inState) throw()
{
  mStateMutex.lock();
	mState = inState;
	mStateMutex.unlock();

	mKernelStateNotifierMutex.lock();
	if(mKernelStateNotifier)
	{
	  try
	  {
	    mKernelStateNotifier->notify(inState);
	  }
	  catch(...)
	  {
	    mKernelStateNotifier = NULL;
	    cerr << "ERROR: An exception occurred while using the provided kernel state notifier; it won't be used anymore" << endl;
	  }
	}
	mKernelStateNotifierMutex.unlock();
}

/*! \todo
*/
SortedLevelModuleMap Kernel::computeModuleLevel()
{
	SortedLevelModuleMap lSortedLevelModuleMap;
	ModuleLevelMap lModuleLevelMap;
	ModuleLevelMap::iterator lModuleLevelMapItr;
	ModuleSet::iterator lModuleSetItr;
	ModuleSet lLevelList;
	ModuleSet lTmpLevelList;
	ModuleSet::iterator lLevelListItr;
	ModuleSlotMap::const_iterator lModuleSlotMapItr;
	ModuleSlotSet lModuleSlotSet;
	ModuleSlotSet::const_iterator lModuleSlotSetItr;
	Module* lTmpModule;
	unsigned int lMaxLevels = 0;
	unsigned int lCurrentLevel = 0;
	bool lInputConnected;

	if(mModuleSet.empty())
		return lSortedLevelModuleMap;

	// First verify for cycles
	try
	{
		detectModuleGraphCycle();
	}
	catch(...)
	{
		throw;
	}

	// Level one consists of modules without any connected input slots
	// Those are considered as the sources, so they must first be identified
	// Initialize the module map level at the same time
	for(lModuleSetItr = mModuleSet.begin(); lModuleSetItr != mModuleSet.end(); lModuleSetItr++)
	{
		lModuleLevelMap[*lModuleSetItr] = 0;
		if(!(*lModuleSetItr)->isAnyInputSlotConnected())
			lLevelList.insert(*lModuleSetItr);
	}

	//Loop on all Modules of a level
	lCurrentLevel = 1;
	while(!lLevelList.empty())
	{
		// Loop on all Modules of current level
		for(lLevelListItr = lLevelList.begin(); lLevelListItr != lLevelList.end(); lLevelListItr++)
		{
			// Loop on all output slot of all Modules
			const ModuleSlotMap& lTmpModuleSlotMap = (*lLevelListItr)->getOutputSlots();
			for(lModuleSlotMapItr = lTmpModuleSlotMap.begin(); lModuleSlotMapItr != lTmpModuleSlotMap.end(); lModuleSlotMapItr++)
			{
				if(lModuleSlotMapItr->second->isConnected())
				{
					lModuleSlotSet = lModuleSlotMapItr->second->getConnectedSlots();

					for(lModuleSlotSetItr = lModuleSlotSet.begin(); lModuleSlotSetItr != lModuleSlotSet.end(); lModuleSlotSetItr++)
					{
						lTmpModule = const_cast<Module*>((*lModuleSlotSetItr)->getModule());
						// If The module is smaller than curent level, then update it
						if(lCurrentLevel > lModuleLevelMap[lTmpModule])
							lModuleLevelMap[lTmpModule] = lCurrentLevel;
						// If the module has any output slot connected, then add it to the next level list
						if(lTmpModule->isAnyOutputSlotConnected())
							lTmpLevelList.insert(lTmpModule);
					}
				}
			}
		}

		// Swap current level list for next level list
		lLevelList.clear();
		lLevelList = lTmpLevelList;
		lTmpLevelList.clear();
		lCurrentLevel += 1;
	}

	// Put each module in the module list for the right level
	for(lModuleLevelMapItr = lModuleLevelMap.begin(); lModuleLevelMapItr != lModuleLevelMap.end(); lModuleLevelMapItr++)
		lSortedLevelModuleMap.insert( pair<unsigned int, Module*>(lModuleLevelMapItr->second, lModuleLevelMapItr->first) );

	return lSortedLevelModuleMap;
}
