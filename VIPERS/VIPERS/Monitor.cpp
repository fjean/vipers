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
 * \file VIPERS/Monitor.cpp
 * \brief Monitor class functions definition.
 * \author Frederic Jean
 * $Revision: 252 $
 * $Date: 2009-10-14 21:52:34 -0400 (Wed, 14 Oct 2009) $
 */

#include "Monitor.hpp"
#include "Module.hpp"

#include <iostream>

using namespace VIPERS;
using namespace std;

/*! \todo
*/
Monitor::Monitor()
  : Notifier()
{
	mModule = NULL;
	mModuleSlot = NULL;
}

/*! \todo
*/
Monitor::~Monitor()
{
	detach();
}

/*! \todo
*/
void Monitor::attach(Module* ioModule, bool inAddToModule) throw()
{
	lock();

	if(ioModule==mModule)
	{
	  unlock();
	  return;
	}

	if(mModule)
	{
	  if(mModuleSlot)
	    mModule->decrementModuleSlotUseCount(mModuleSlot->getType(), mModuleSlot->getName());
	  mModule->detachMonitor(this, false);
	}

	mModule = ioModule;
	if(ioModule && inAddToModule)
		ioModule->attachMonitor(this, false);

  unlock();
}

/*! \todo
*/
void Monitor::detach(bool inRemoveFromModule) throw()
{
	lock();

	if(!mModule)
	{
	  unlock();
	  return;
	}

	if(mModuleSlot)
	{
		mModule->decrementModuleSlotUseCount(mModuleSlot->getType(), mModuleSlot->getName());
		mModuleSlot = NULL;
	}

  if(inRemoveFromModule)
    mModule->detachMonitor(this, false);

  mModule = NULL;

	unlock();
}

/*! \todo
*/
Module* Monitor::getModule() const throw()
{
	Module* lTmpModule;
	lock();
	lTmpModule = mModule;
	unlock();
	return lTmpModule;
}

/*! \todo
*/
void Monitor::setModuleSlot(ModuleSlot::SlotType inSlotType, const string& inName)
{
	lock();

	if(!mModule)
	{
		unlock();
		throw(Exception(Exception::eCodeNotAttachedMonitor, "Cannot set module slot in Monitor::setModuleSlot"));
	}

	if(mModuleSlot)
	{
		mModule->decrementModuleSlotUseCount(mModuleSlot->getType(), mModuleSlot->getName());
		mModuleSlot	= NULL;
	}

	if(inSlotType==ModuleSlot::eSlotTypeInput)
		mModuleSlot = static_cast<const Module*>(mModule)->getInputSlot(inName.c_str());
	else if(inSlotType==ModuleSlot::eSlotTypeOutput)
		mModuleSlot = static_cast<const Module*>(mModule)->getOutputSlot(inName.c_str());
	else
	{
		unlock();
		throw(Exception(Exception::eCodeUndefined, "Invalid module slot type requested by monitor in Monitor::lockModuleSlot"));
	}

	if(!mModuleSlot)
	{
		string lTmpLabel = mModule->getLabel().c_str();
		unlock();

		string lTmpSlotType;
		if(inSlotType==ModuleSlot::eSlotTypeInput)
			lTmpSlotType = "input";
		else
			lTmpSlotType = "output";

		throw(Exception(Exception::eCodeInvalidModuleSlotMonitor, string("Module \"") + lTmpLabel + string("\" does not have an ") + lTmpSlotType + string(" slot named \"") + inName.c_str() + string("\".")));
	}

	mModule->incrementModuleSlotUseCount(mModuleSlot->getType(), mModuleSlot->getName());

	unlock();
}

/*! \todo
*/
void Monitor::unsetModuleSlot() throw()
{
	lock();
	if(mModule && mModuleSlot)
	{
		mModule->decrementModuleSlotUseCount(mModuleSlot->getType(), mModuleSlot->getName());
		mModuleSlot	= NULL;
	}
	unlock();
}

/*! \todo
*/
void  Monitor::lockModuleSlot()
{
	lock();

	if(!mModule)
	{
		unlock();
		throw(Exception(Exception::eCodeNotAttachedMonitor, "Cannot lock module slot in Monitor::lockModuleSlot"));
	}

	if(!mModuleSlot)
	{
		unlock();
		throw(Exception(Exception::eCodeInvalidModuleSlotMonitor, "No monitored module slot"));
	}

	try
	{
		mModuleSlot->lock();
	}
	catch(...)
	{
		unlock();
		throw;
	}

}

/*! \todo
*/
bool Monitor::tryLockModuleSlot()
{
	bool lResult;

	lock();

	if(!mModule)
	{
		unlock();
		throw(Exception(Exception::eCodeNotAttachedMonitor, "Cannot lock module slot in Monitor::tryLockModuleSlot"));
	}

	if(!mModuleSlot)
	{
		unlock();
		throw(Exception(Exception::eCodeInvalidModuleSlotMonitor, "No monitored module slot"));
	}

	try
	{
		lResult = mModuleSlot->tryLock();
		if(!lResult)
			unlock();
	}
	catch(...)
	{
		unlock();
		throw;
	}

	return lResult;
}

/*! \todo
*/
const Image*  Monitor::getModuleSlotImage() const
{
	if(!mModuleSlot)
		throw(Exception(Exception::eCodeUnlockedModuleSlotMonitor, "Cannot get module slot image in Monitor::getModuleSlotImage"));
	return mModuleSlot->getImage();
}

/*! \todo
*/
void  Monitor::unlockModuleSlot()
{
	if(!mModuleSlot)
		throw(Exception(Exception::eCodeUnlockedModuleSlotMonitor, "Cannot unlock module slot in Monitor::unlockModuleSlot since no module slot is monitored"));

	try
	{
		mModuleSlot->unlock();
	}
	catch(...)
	{
		unlock();
		throw;
	}
	unlock();
}

/*! \todo
*/
const ModuleSlot* Monitor::getModuleSlot() const throw()
{
	const ModuleSlot* lTmpModuleSlot;

	lock();
	lTmpModuleSlot = mModuleSlot;
	unlock();

	return lTmpModuleSlot;
}

/*! \todo
*/
bool Monitor::isMonitoring() const throw()
{
	bool lResult = false;
	lock();
	if(mModule && mModuleSlot)
		lResult = true;
	unlock();
	return lResult;
}

/*! \todo
*/
void Monitor::setProperty(const Property& inProperty) throw()
{
	mPropertyMap.insert(pair<string,Property>(inProperty.getName().c_str(), inProperty));
}

/*! \todo
*/
Property Monitor::getProperty(const string& inName) const throw()
{
	PropertyMap::const_iterator lItr = mPropertyMap.find(inName.c_str());
	if(lItr!=mPropertyMap.end())
		return lItr->second;
	else
		return Property("", Variable::eVariableTypeString, "", "");
}

/*! \todo
*/
PropertyList Monitor::getPropertyList() const throw()
{
	PropertyList lPropertyList;
	PropertyMap::const_iterator lPropertyItr = mPropertyMap.begin();
	for(lPropertyItr; lPropertyItr!=mPropertyMap.end(); lPropertyItr++)
		lPropertyList.push_back(lPropertyItr->second);
	return lPropertyList;
}

/*! \todo
*/
void Monitor::setPropertyList(const PropertyList& inPropertyList) throw()
{
	mPropertyMap.clear();
	PropertyList::const_iterator lItr = inPropertyList.begin();
	for(lItr; lItr!=inPropertyList.end(); lItr++)
		mPropertyMap.insert(pair<string,Property>(lItr->getName().c_str(), *lItr));
}

/*! \todo
*/
bool Monitor::eraseProperty(const string& inName) throw()
{
	return mPropertyMap.erase(inName);
}

/*! \todo
*/
void Monitor::clearPropertyList() throw()
{
	mPropertyMap.clear();
}
