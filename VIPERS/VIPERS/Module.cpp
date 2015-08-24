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
 * \file VIPERS/Module.cpp
 * \brief Module class functions definition.
 * \author Frederic Jean
 * $Revision: 296 $
 * $Date: 2009-11-02 20:52:05 -0500 (Mon, 02 Nov 2009) $
 */

#include "Module.hpp"

#include <memory>
#include <iostream>

using namespace VIPERS;
using namespace std;

/*! \todo
*/
Module::Module(const string& inName, const string& inDisplayName, const string& inVersion)
{
	mName = inName.c_str();
	mDisplayName = inDisplayName.c_str();
	mVersion = inVersion.c_str();
	mMaxNumberFrame = 0;
	mFrameRate = 0.0;
	mState = eStateUninitialized;
}

/*! \todo
*/
Module::~Module()
{
	detachAllMonitor();
	disconnectAll();

	ModuleSlotMap::iterator lInputSlotItr = mInputSlots.begin();
	ModuleSlotMap::iterator lOutputSlotItr = mOutputSlots.begin();

	for(lInputSlotItr; lInputSlotItr!=mInputSlots.end();lInputSlotItr++)
		delete lInputSlotItr->second;
	for(lOutputSlotItr; lOutputSlotItr!=mOutputSlots.end();lOutputSlotItr++)
		delete lOutputSlotItr->second;

	mInputSlots.clear();
	mOutputSlots.clear();
	mParameters.clear();
}

/*! \todo
*/
void Module::init()
{
  State lState = getState();

  if(lState!=eStateUninitialized && lState!=eStateInitialized && lState!=eStateStopped)
    throw(Exception(Exception::eCodeInvalidOperationModuleState, string("Module \"") + getLabel().c_str() + string("\" cannot be initialized since it is not stopped or uninitialized")));

	try
	{
		initFunction();
	}
	catch(...)
	{
		throw;
	}

	setState(eStateInitialized);
	notifyMonitors();
}

/*! \todo
*/
void Module::start()
{
  State lState = getState();

  if(lState!=eStateInitialized && lState!=eStateStopped && lState!=eStatePaused)
    throw(Exception(Exception::eCodeInvalidOperationModuleState, string("Module \"") + getLabel().c_str() + string("\" cannot be started since it is not stopped, paused, or initialized")));

	try
	{
		startFunction();
	}
	catch(...)
	{
		throw;
	}
	setState(eStateStarted);
}

/*! \todo
*/
void Module::process(unsigned int inFrameNumber)
{
  State lState = getState();
  if(lState!=eStateStarted && lState!=eStatePaused)
    throw(Exception(Exception::eCodeInvalidOperationModuleState, string("Module \"") + getLabel().c_str() + string("\" cannot process a frame since it is not started")));

	try
	{
		processFunction(inFrameNumber);
	}
	catch(...)
	{
		throw;
	}
	notifyMonitors();
}

/*! \todo
*/
void Module::stop()
{
  State lState = getState();

  if(lState!=eStateStarted && lState!=eStatePaused)
    throw(Exception(Exception::eCodeInvalidOperationModuleState, string("Module \"") + getLabel().c_str() + string("\" cannot be stopped since it is not started or paused")));

	try
	{
		stopFunction();
	}
	catch(...)
	{
		throw;
	}
	setState(Module::eStateStopped);
}

/*! \todo
*/
void Module::pause()
{
  State lState = getState();
  if(lState!=eStateStarted)
    throw(Exception(Exception::eCodeInvalidOperationModuleState, string("Module \"") + getLabel().c_str() + string("\" cannot be paused since it is not started")));

  try
  {
    pauseFunction();
  }
  catch(...)
  {
    throw;
  }
  setState(eStatePaused);
}

/*! \todo
*/
void Module::reset()
{
	try
	{
		resetFunction();
	}
	catch(...)
	{
		throw;
	}
	setState(eStateUninitialized);
}

/*! \todo
*/
ParameterList Module::updateParameters()
{
	try
	{
		updateParametersFunction();
	}
	catch(...)
	{
		throw;
	}
	return getParameterList();
}

/*! \todo
*/
string Module::verifyParameter(const Parameter& inParameter) const throw()
{
	ParameterMap::const_iterator lParameterMapItr = mParameters.find(inParameter.getName().c_str());
	if(lParameterMapItr==mParameters.end())
		return string("There is no parameter named \"") + inParameter.getName().c_str() + string("\"");
	else
		return verifyParameterFunction(inParameter);
}

/*! \todo
*/
void Module::connect(ModuleSlot::SlotType inSlotType, const string& inSlotName, Module* ioModule, const string& inModuleSlotName)
{
	if(!ioModule)
		throw(Exception(Exception::eCodeUndefined, "Module \"" + getLabel() + "\" received a NULL module pointer in Module::connect function"));

	//Search for the slot by its name and type
	if(inSlotType==ModuleSlot::eSlotTypeInput)
	{
		ModuleSlotMap::iterator lInputSlotItr = mInputSlots.find(inSlotName);
		if(lInputSlotItr!=mInputSlots.end())
		{
			ModuleSlotMap::iterator lOutputSlotItr;
			try
			{
				lOutputSlotItr = ioModule->mOutputSlots.find(inModuleSlotName);
				if(lOutputSlotItr==ioModule->mOutputSlots.end())
					throw(Exception(Exception::eCodeInvalidSlot, "Module \"" + ioModule->getLabel() + "\" does not have an output slot named \"" + inModuleSlotName + "\""));

				//Deconnect input slot if already connected
				if(lInputSlotItr->second->isConnected())
					lInputSlotItr->second->disconnectAll();
				lInputSlotItr->second->connect(lOutputSlotItr->second);
			}
			catch(...)
			{
				throw;
			}
		}
		else
		{
			throw(Exception(Exception::eCodeInvalidSlot, "Module \"" + getLabel() + "\" does not have an input slot named \"" + inSlotName + "\""));
		}
	}
	else if(inSlotType==ModuleSlot::eSlotTypeOutput)
	{
		ModuleSlotMap::iterator lOutputSlotItr = mOutputSlots.find(inSlotName);
		if(lOutputSlotItr!=mInputSlots.end())
		{
			ModuleSlotMap::iterator lInputSlotItr;
			try
			{
				lInputSlotItr = ioModule->mInputSlots.find(inModuleSlotName);
				if(lInputSlotItr==ioModule->mInputSlots.end())
					throw(Exception(Exception::eCodeInvalidSlot, "Module \"" + ioModule->getLabel() + "\" does not have an input slot named \"" + inModuleSlotName + "\""));

				//Deconnect input slot if already connected
				if(lInputSlotItr->second->isConnected())
					lInputSlotItr->second->disconnectAll();
				lInputSlotItr->second->connect(lOutputSlotItr->second);
			}
			catch(...)
			{
				throw;
			}
		}
		else
		{
			throw(Exception(Exception::eCodeInvalidSlot, "Module \"" + getLabel() + "\" does not have an output slot named \"" + inSlotName + "\""));
		}
	}
	else
	{
		throw(Exception(Exception::eCodeInvalidSlot, "Module \"" + getLabel() + "\" received an invalid slot type in Module::connect"));
	}
}

/*! \todo
*/
void Module::disconnect(const string& inInputSlotName)
{
	ModuleSlotMap::iterator lInputSlotItr = mInputSlots.find(inInputSlotName);
	if(lInputSlotItr!=mInputSlots.end())
	{
		lInputSlotItr->second->disconnectAll();
	}
	else
	{
		throw(Exception(Exception::eCodeInvalidSlot, "Module \"" + getLabel() + string("\" does not have an input slot named \"") + inInputSlotName.c_str() + string("\".")));
	}
}

/*! \todo
*/
void Module::disconnectAll(ModuleSlot::SlotType inSlotType) throw()
{
	if(inSlotType==ModuleSlot::eSlotTypeInput)
	{
		ModuleSlotMap::iterator lSlotItr = mInputSlots.begin();
		for(lSlotItr; lSlotItr!=mInputSlots.end(); lSlotItr++)
			lSlotItr->second->disconnectAll();
	}
	else
	{
		ModuleSlotMap::iterator lSlotItr = mOutputSlots.begin();
		for(lSlotItr; lSlotItr!=mOutputSlots.end(); lSlotItr++)
			lSlotItr->second->disconnectAll();
	}
}

/*! \todo
*/
void Module::disconnectAll() throw()
{
	ModuleSlotMap::iterator lInputSlotItr = mInputSlots.begin();
	for(lInputSlotItr; lInputSlotItr!=mInputSlots.end(); lInputSlotItr++)
		lInputSlotItr->second->disconnectAll();
	ModuleSlotMap::iterator lOutputSlotItr = mOutputSlots.begin();
	for(lOutputSlotItr; lOutputSlotItr!=mOutputSlots.end(); lOutputSlotItr++)
		lOutputSlotItr->second->disconnectAll();
}

/*! \todo
*/
string Module::getLabel() const throw()
{
	string lTmpStr;
	mLabelMutex.lock();
	lTmpStr = mLabel.c_str();
	mLabelMutex.unlock();
	return lTmpStr;
}

/*! \todo
*/
string Module::getName() const throw()
{
	return mName.c_str();
}

/*! \todo
*/
string Module::getDisplayName() const throw()
{
	return mDisplayName.c_str();
}

/*! \todo
*/
string Module::getShortDescription() const throw()
{
	return mShortDescription.c_str();
}

/*! \todo
*/
string Module::getLongDescription() const throw()
{
	return mLongDescription.c_str();
}

/*! \todo
*/
string Module::getDocumentation() const throw()
{
	return mDocumentation.c_str();
}

/*! \todo
*/
string Module::getVersion() const throw()
{
	return mVersion.c_str();
}

/*! \todo
*/
Module::State Module::getState() const throw()
{
	State lState;
	mStateMutex.lock();
	lState = mState;
	mStateMutex.unlock();
	return lState;
}

/*! \todo
*/
unsigned int Module::getMaxNumberFrames() throw()
{
	unsigned int lTmpMaxNumberFrames;

	mMaxNumberFramesMutex.lock();
	lTmpMaxNumberFrames = mMaxNumberFrame;
	mMaxNumberFramesMutex.unlock();

	return lTmpMaxNumberFrames;
}

/*! \todo
*/
double Module::getFrameRate() throw()
{
	double lTmpFrameRate;

	mFrameRateMutex.lock();
	lTmpFrameRate = mFrameRate;
	mFrameRateMutex.unlock();

	return lTmpFrameRate;
}

/*! \todo
*/
void Module::setLabel(string inLabel) throw()
{
	string lTmpLabel = inLabel.c_str();
	mLabelMutex.lock();
	mLabel = lTmpLabel.c_str();
	mLabelMutex.unlock();
}

/*! \todo
*/
ParameterList Module::getParameterList() const throw()
{
	ParameterList lTmpParameters;

	mParametersMutex.lock();

	lTmpParameters.reserve(mParameters.size());
	for(ParameterMap::const_iterator lParameterItr = mParameters.begin(); lParameterItr != mParameters.end(); lParameterItr++)
		lTmpParameters.push_back(lParameterItr->second);

	mParametersMutex.unlock();

	return lTmpParameters;
}

/*! \todo
*/
ParameterMap Module::getParameterMap() const throw()
{
	ParameterMap lTmpParameters;

	mParametersMutex.lock();

  lTmpParameters = mParameters;

	mParametersMutex.unlock();

	return lTmpParameters;
}

/*! \todo
*/
Parameter Module::getParameter(const string& inName) const
{
	auto_ptr<Parameter> lTmpParameterPtr;

	mParametersMutex.lock();

	ParameterMap::const_iterator lParameterMapItr = mParameters.find(inName.c_str());
	if(lParameterMapItr!=mParameters.end())
		lTmpParameterPtr.reset( new Parameter(lParameterMapItr->second) );
	else
	{
		mParametersMutex.unlock();
		throw(Exception(Exception::eCodeInvalidParameter, string("Module \"") + getLabel() + string("\" does not have a parameter named \"") + inName.c_str() + string("\".")));
	}

	mParametersMutex.unlock();

	return *lTmpParameterPtr;
}

/*! \todo
*/
void Module::setParameterValue(const string& inName, const string& inValue)
{
	Module::State lState = getState();
	string lVerifyStr;
	ValueSet lPossibleValues;

	mParametersMutex.lock();

	ParameterMap::iterator lParameterMapItr = mParameters.find(inName.c_str());
	if(lParameterMapItr!=mParameters.end())
	{
		Parameter lTmpParameter(lParameterMapItr->second);
		lTmpParameter.setValueStr(inValue.c_str());

		lVerifyStr = verifyParameterFunction(lTmpParameter);
		if(!lVerifyStr.empty())
		{
			mParametersMutex.unlock();
			throw(Exception(Exception::eCodeInvalidParameter, lVerifyStr.c_str()));
		}

		if((lState==Module::eStateStarted || lState==Module::eStatePaused) && !lParameterMapItr->second.getRunTimeChange())
		{
			mParametersMutex.unlock();
			throw(Exception(Exception::eCodeInvalidParameter, string("Parameter \"") + lTmpParameter.getDisplayName().c_str() + string("\" cannot be changed once the module is started") ));
		}

		lPossibleValues = lParameterMapItr->second.getPossibleValues();
		if(lPossibleValues.size() && lPossibleValues.find(lTmpParameter.getValue())==lPossibleValues.end())
		{
			mParametersMutex.unlock();
			throw(Exception(Exception::eCodeInvalidParameter, string("Value \"") + lTmpParameter.getValue().c_str() + string("\" is not valid for parameter ") + lTmpParameter.getDisplayName().c_str() + string("\"") ));
		}

		lParameterMapItr->second.setValueStr(inValue.c_str());
		mParametersMutex.unlock();
	}
	else
	{
		mParametersMutex.unlock();
		throw(Exception(Exception::eCodeInvalidParameter, string("Module \"") + getLabel() + string("\" does not have a parameter named \"") + inName.c_str() + string("\"")));
	}
}

/*! \todo
*/
void Module::setParameterValue(const Parameter& inParameter)
{
	Module::State lState = getState();
	string lVerifyStr;
	ValueSet lPossibleValues;

	mParametersMutex.lock();

	ParameterMap::iterator lParameterMapItr = mParameters.find(inParameter.getName().c_str());
	if(lParameterMapItr!=mParameters.end())
	{
		lVerifyStr = verifyParameterFunction(inParameter);
		if(!lVerifyStr.empty())
		{
			mParametersMutex.unlock();
			throw(Exception(Exception::eCodeInvalidParameter, lVerifyStr.c_str()));
		}

		if((lState==Module::eStateStarted || lState==Module::eStatePaused) && !lParameterMapItr->second.getRunTimeChange())
		{
			mParametersMutex.unlock();
			throw(Exception(Exception::eCodeInvalidParameter, string("Parameter \"") + inParameter.getDisplayName().c_str() + string("\" cannot be changed once the module is started") ));
		}

		lPossibleValues = lParameterMapItr->second.getPossibleValues();
		if(lPossibleValues.size() && lPossibleValues.find(inParameter.getValue())==lPossibleValues.end())
		{
			mParametersMutex.unlock();
			throw(Exception(Exception::eCodeInvalidParameter, string("Value \"") + inParameter.getValue().c_str() + string("\" is not valid for parameter ") + inParameter.getDisplayName().c_str() + string("\"") ));
		}

		lParameterMapItr->second.setValueStr(inParameter.getValue());
		mParametersMutex.unlock();
	}
	else
	{
		mParametersMutex.unlock();
		throw(Exception(Exception::eCodeInvalidParameter, string("Module \"") + getLabel() + string("\" does not have a parameter named \"") + inParameter.getName().c_str() + string("\"")));
	}
}

/*! \todo
*/
const ModuleSlotMap& Module::getInputSlots() const throw()
{
	return mInputSlots;
}

/*! \todo
*/
const ModuleSlotMap& Module::getOutputSlots() const throw()
{
	return mOutputSlots;
}

/*! \todo
*/
const ModuleSlot* Module::getInputSlot(const string& inName) const throw()
{
	ModuleSlotMap::const_iterator lSlotItr = mInputSlots.find(inName.c_str());
	if(lSlotItr!=mInputSlots.end())
		return lSlotItr->second;
	else
		return NULL;
}

/*! \todo
*/
const ModuleSlot* Module::getOutputSlot(const string& inName) const throw()
{
	ModuleSlotMap::const_iterator lSlotItr = mOutputSlots.find(inName.c_str());
	if(lSlotItr!=mOutputSlots.end())
		return lSlotItr->second;
	else
		return NULL;
}

/*! \todo
*/
bool Module::isAnyInputSlotConnected() const throw()
{
	bool lResult = false;
	ModuleSlotMap::const_iterator lModuleSlotMapItr = mInputSlots.begin();
	for(lModuleSlotMapItr; lModuleSlotMapItr != mInputSlots.end(); lModuleSlotMapItr++)
	{
		if(lModuleSlotMapItr->second->isConnected())
		{
			lResult = true;
			break;
		}
	}
	return lResult;
}

/*! \todo
*/
bool Module::isAnyOutputSlotConnected() const throw()
{
	bool lResult = false;
	ModuleSlotMap::const_iterator lModuleSlotMapItr = mOutputSlots.begin();
	for(lModuleSlotMapItr; lModuleSlotMapItr != mOutputSlots.end(); lModuleSlotMapItr++)
	{
		if(lModuleSlotMapItr->second->isConnected())
		{
			lResult = true;
			break;
		}
	}
	return lResult;
}

/*! \todo
*/
void Module::attachMonitor(Monitor* ioMonitor, bool inAddToMonitor) throw()
{
	if(ioMonitor)
	{
		bool lNotAttached;

		mMonitorSetMutex.lock();

		lNotAttached = mMonitorSet.insert(ioMonitor).second;
		if(lNotAttached && inAddToMonitor)
			ioMonitor->attach(this, false);

    mMonitorSetMutex.unlock();
	}
}

/*! \todo
*/
void Module::detachMonitor(Monitor* ioMonitor, bool inRemoveFromMonitor) throw()
{
	if(ioMonitor)
	{
		size_t lNbErased;

		mMonitorSetMutex.lock();

		lNbErased = mMonitorSet.erase(ioMonitor);

    if(lNbErased && inRemoveFromMonitor)
      ioMonitor->detach(false);

		mMonitorSetMutex.unlock();
	}
}

/*! \todo
*/
void Module::detachAllMonitor() throw()
{
	MonitorSet lTmpMonitorSet;
	MonitorSet::iterator lMonitorItr;

	mMonitorSetMutex.lock();

  for(lMonitorItr = mMonitorSet.begin(); lMonitorItr != mMonitorSet.end(); lMonitorItr++)
    (*lMonitorItr)->detach(false);

  mMonitorSet.clear();

	mMonitorSetMutex.unlock();
}

/*! \todo
*/
MonitorSet Module::getMonitors() const throw()
{
	MonitorSet lTmpMonitorSet;
	mMonitorSetMutex.lock();
	lTmpMonitorSet = mMonitorSet;
	mMonitorSetMutex.unlock();
	return lTmpMonitorSet;
}

/*! \todo
*/
void Module::notifyMonitors() const throw()
{
	MonitorSet::iterator lMonitorItr;

	mMonitorSetMutex.lock();
	for(lMonitorItr = mMonitorSet.begin(); lMonitorItr != mMonitorSet.end(); lMonitorItr++)
	{
	  try
	  {
	    (*lMonitorItr)->notify();
	  }
	  catch(...)
	  {

	  }
	}
	mMonitorSetMutex.unlock();
}

/*! \todo
*/
bool Module::isMonitorAttached(Monitor* inMonitor) const throw()
{
	bool lResult;
	MonitorSet::const_iterator lMonitorItr;

	mMonitorSetMutex.lock();

	lMonitorItr = mMonitorSet.find(inMonitor);
	if(lMonitorItr!=mMonitorSet.end())
		lResult = true;
	else
		lResult = false;

	mMonitorSetMutex.unlock();

	return lResult;
}

/*! \todo
*/
void Module::incrementModuleSlotUseCount(ModuleSlot::SlotType inSlotType, const string& inSlotName)
{
	//Search for the slot by its name and type
	if(inSlotType==ModuleSlot::eSlotTypeInput)
	{
		ModuleSlotMap::iterator lInputSlotItr = mInputSlots.find(inSlotName);
		if(lInputSlotItr!=mInputSlots.end())
		{
			lInputSlotItr->second->incrementUseCount();
		}
		else
		{
			throw(Exception(Exception::eCodeInvalidSlot, "Module \"" + getLabel() + "\" does not have an input slot named \"" + inSlotName + "\""));
		}
	}
	else if(inSlotType==ModuleSlot::eSlotTypeOutput)
	{
		ModuleSlotMap::iterator lOutputSlotItr = mOutputSlots.find(inSlotName);
		if(lOutputSlotItr!=mInputSlots.end())
		{
			lOutputSlotItr->second->incrementUseCount();
		}
		else
		{
			throw(Exception(Exception::eCodeInvalidSlot, "Module \"" + getLabel() + "\" does not have an output slot named \"" + inSlotName + "\""));
		}
	}
	else
	{
		throw(Exception(Exception::eCodeInvalidSlot, "Module \"" + getLabel() + "\" received an invalid slot type in Module::incrementModuleSlotUseCount"));
	}
}

/*! \todo
*/
void Module::decrementModuleSlotUseCount(ModuleSlot::SlotType inSlotType, const string& inSlotName)
{
	//Search for the slot by its name and type
	if(inSlotType==ModuleSlot::eSlotTypeInput)
	{
		ModuleSlotMap::iterator lInputSlotItr = mInputSlots.find(inSlotName);
		if(lInputSlotItr!=mInputSlots.end())
		{
			lInputSlotItr->second->decrementUseCount();
		}
		else
		{
			throw(Exception(Exception::eCodeInvalidSlot, "Module \"" + getLabel() + "\" does not have an input slot named \"" + inSlotName + "\""));
		}
	}
	else if(inSlotType==ModuleSlot::eSlotTypeOutput)
	{
		ModuleSlotMap::iterator lOutputSlotItr = mOutputSlots.find(inSlotName);
		if(lOutputSlotItr!=mInputSlots.end())
		{
			lOutputSlotItr->second->decrementUseCount();
		}
		else
		{
			throw(Exception(Exception::eCodeInvalidSlot, "Module \"" + getLabel() + "\" does not have an output slot named \"" + inSlotName + "\""));
		}
	}
	else
	{
		throw(Exception(Exception::eCodeInvalidSlot, "Module \"" + getLabel() + "\" received an invalid slot type in Module::incrementModuleSlotUseCount"));
	}
}

/*! \todo
*/
void Module::setProperty(const Property& inProperty) throw()
{
	mPropertyMap.insert(pair<string,Property>(inProperty.getName().c_str(), inProperty));
}

/*! \todo
*/
Property Module::getProperty(const string& inName) const throw()
{
	PropertyMap::const_iterator lItr = mPropertyMap.find(inName.c_str());
	if(lItr!=mPropertyMap.end())
		return lItr->second;
	else
		return Property("", Variable::eVariableTypeString, "", "");
}

/*! \todo
*/
PropertyList Module::getPropertyList() const throw()
{
	PropertyList lPropertyList;
	PropertyMap::const_iterator lItr = mPropertyMap.begin();
	for(lItr; lItr!=mPropertyMap.end(); lItr++)
		lPropertyList.push_back(lItr->second);
	return lPropertyList;
}

/*! \todo
*/
void Module::setPropertyList(const PropertyList& inPropertyList) throw()
{
	mPropertyMap.clear();
	PropertyList::const_iterator lItr = inPropertyList.begin();
	for(lItr; lItr!=inPropertyList.end(); lItr++)
		mPropertyMap.insert(pair<string,Property>(lItr->getName().c_str(), *lItr));
}

/*! \todo
*/
bool Module::eraseProperty(const string& inName) throw()
{
	return mPropertyMap.erase(inName);
}

/*! \todo
*/
void Module::clearPropertyList() throw()
{
	mPropertyMap.clear();
}

/*! \todo
*/
ModuleSlot* Module::newSlot(ModuleSlot* inSlot)
{
	ModuleSlotMap::iterator lSlotItr;
	ModuleSlotMap* lTmpModuleSlotsMapPtr = NULL;
	ModuleSlot::SlotType lSlotType;
	string lSlotName;

	if(inSlot)
	{
		try
		{
			lSlotType = inSlot->getType();
			lSlotName = inSlot->getName().c_str();
		}
		catch(...)
		{
			throw;
		}

		if(lSlotType==ModuleSlot::eSlotTypeInput)
			lTmpModuleSlotsMapPtr = &mInputSlots;
		else if(lSlotType==ModuleSlot::eSlotTypeOutput)
			lTmpModuleSlotsMapPtr = &mOutputSlots;
		else
			throw(Exception(Exception::eCodeBuggyModule, string("Module \"") + getLabel() + string("\" slot \"") + lSlotName.c_str() + string("\" has invalid type.  This is a module bug.")));

		lSlotItr = lTmpModuleSlotsMapPtr->find(lSlotName.c_str());
		if(lSlotItr!=lTmpModuleSlotsMapPtr->end())
			throw(Exception(Exception::eCodeBuggyModule, string("Module \"") + getLabel() + string("\" already has a slot named \"") + lSlotName.c_str() + string("\".  This is a module bug.")));

		lTmpModuleSlotsMapPtr->insert(pair<string, ModuleSlot*>(lSlotName.c_str(), inSlot));

	}
	else
	{
		throw(Exception(Exception::eCodeBuggyModule, string("Module \"") + getLabel() + string("\" has been passed a NULL slot pointer for function Module::newSlot.  This is a module bug.")));
	}

	return inSlot;
}

/*! \todo
*/
ModuleSlot* Module::getInputSlot(const string& inName) throw()
{
	ModuleSlotMap::iterator lSlotItr = mInputSlots.find(inName.c_str());
	if(lSlotItr!=mInputSlots.end())
		return lSlotItr->second;
	else
		return NULL;
}

/*! \todo
*/
ModuleSlot* Module::getOutputSlot(const string& inName) throw()
{
	ModuleSlotMap::iterator lSlotItr = mOutputSlots.find(inName.c_str());
	if(lSlotItr!=mOutputSlots.end())
		return lSlotItr->second;
	else
		return NULL;
}

/*! \todo
*/
void Module::setMaxNumberFrames(unsigned int inMaxNbFrames) throw()
{
	mMaxNumberFramesMutex.lock();
	mMaxNumberFrame = inMaxNbFrames;
	mMaxNumberFramesMutex.unlock();
}

/*! \todo
*/
void Module::setFrameRate(double inFrameRate) throw()
{
	mFrameRateMutex.lock();
	mFrameRate = inFrameRate;
	mFrameRateMutex.unlock();
}

/*! \todo
*/
void Module::newParameter(const Parameter& inParameter) throw()
{
	mParametersMutex.lock();
	mParameters.insert( pair<string, Parameter>(inParameter.getName().c_str(), inParameter) );
	mParametersMutex.unlock();
}

/*! \todo
*/
void Module::lockParameters() const
{
	mParametersMutex.lock();
}

/*! \todo
*/
void Module::unlockParameters() const
{
	mParametersMutex.unlock();
}

/*! \todo
*/
Parameter Module::getLockedParameter(const string& inName) const
{
	ParameterMap::const_iterator lParameterMapItr = mParameters.find(inName.c_str());
	if(lParameterMapItr!=mParameters.end())
		return lParameterMapItr->second;
	else
		throw(Exception(Exception::eCodeInvalidParameter, string("Module \"") + getLabel() + string("\" does not have a parameter named \"") + inName.c_str() + string("\".")));
}

/*! \todo
*/
void Module::setLockedParameter(const Parameter& inParameter)
{
	ParameterMap::iterator lParameterMapItr = mParameters.find(inParameter.getName().c_str());
	if(lParameterMapItr!=mParameters.end())
	{
		lParameterMapItr->second = inParameter;
	}
	else
		throw(Exception(Exception::eCodeBuggyModule, string("Module \"") + getLabel() + string(" does not have a parameter named \"") + inParameter.getName().c_str() + string("\".  This is a module bug.")));
}

/*! \todo
*/
void Module::setState(State inState) throw()
{
	mStateMutex.lock();
	mState = inState;
	mStateMutex.unlock();
}

