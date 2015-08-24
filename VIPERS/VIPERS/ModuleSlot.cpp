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
 * \file VIPERS/ModuleSlot.cpp
 * \brief ModuleSlot class functions definition.
 * \author Frederic Jean
 * $Revision: 252 $
 * $Date: 2009-10-14 21:52:34 -0400 (Wed, 14 Oct 2009) $
 */

#include "ModuleSlot.hpp"
#include "Module.hpp"

using namespace VIPERS;
using namespace std;

/*! \todo
*/
ModuleSlot::ModuleSlot(const Module* inModule, const string& inName, const string& inDisplayName, const string& inDescription)
{
	mName = inName.c_str();
	mDisplayName = inDisplayName.c_str();
	mType = eSlotTypeInput;
	mDescription = inDescription.c_str();
    mImagePtr = NULL;
    mConnected = false;
    if(!inModule)
		throw(Exception(Exception::eCodeBuggyModule, "Input slot \"" + mName + "\" created with a NULL module pointer"));
	mModule = inModule;
	mUseCount = 0;
}

/*! \todo
*/
ModuleSlot::ModuleSlot(const Module* inModule, const string& inName, const string& inDisplayName, const string& inDescription, Image **inImagePtr)
{
	mName = inName.c_str();
	mDisplayName = inDisplayName.c_str();
	mType = eSlotTypeOutput;
	mDescription = inDescription.c_str();
	mConnected = false;
	if(inImagePtr)
	{
        mImagePtr = inImagePtr;
	}
	else
    {
        throw(Exception(Exception::eCodeBuggyModule, "Output slot \"" + mName + "\" created with a NULL image pointer"));
    }

    if(!inModule)
		throw(Exception(Exception::eCodeBuggyModule, "Output slot \"" + mName + "\" created with a NULL module pointer"));
	mModule = inModule;

    mMutexPtr = new Threading::Mutex();
    mUseCount = 0;
}

/*! \todo
*/
ModuleSlot::~ModuleSlot()
{
	disconnectAll();
    if(mType == eSlotTypeOutput)
        delete mMutexPtr;
}

/*! \todo
*/
void ModuleSlot::connect(ModuleSlot* ioModuleSlot)
{
	if(ioModuleSlot)
	{
		if(ioModuleSlot->mType==mType)
			throw(Exception(Exception::eCodeInvalidSlot, "Slots " + string(*this) + " and " + string(*ioModuleSlot) + " are of the same type"));

		if(mType==eSlotTypeInput && mModuleSlots.size())
			throw(Exception(Exception::eCodeAlreadyConnectedSlot, "Input slot \"" + string(*this) + "\" is already connected to another slot"));
		if(ioModuleSlot->mType==eSlotTypeInput && ioModuleSlot->mModuleSlots.size())
			throw(Exception(Exception::eCodeAlreadyConnectedSlot, "Input slot \"" + string(*ioModuleSlot) + "\" is already connected to another slot"));

		if(mType==eSlotTypeInput)
		{
			mImagePtr = ioModuleSlot->mImagePtr;
			mMutexPtr = ioModuleSlot->mMutexPtr;
			mModuleSlots.insert(ioModuleSlot);
			mConnected = true;
			incrementUseCount();

			ioModuleSlot->mModuleSlots.insert(this);
			ioModuleSlot->mConnected = true;
			ioModuleSlot->incrementUseCount();
		}
		else
		{
			ioModuleSlot->mImagePtr = mImagePtr;
			ioModuleSlot->mMutexPtr = mMutexPtr;
			ioModuleSlot->mModuleSlots.insert(this);
			ioModuleSlot->mConnected = true;
			ioModuleSlot->incrementUseCount();

			mModuleSlots.insert(ioModuleSlot);
			mConnected = true;
			incrementUseCount();
		}
	}
	else
	{
		throw(Exception(Exception::eCodeInvalidSlot, "Slot " + string(*this) + " received a NULL slot pointer"));
	}
}

/*! \todo
*/
void ModuleSlot::disconnect(ModuleSlot* ioModuleSlot)
{
	if(mModuleSlots.erase(ioModuleSlot))
	{
		if(mType==eSlotTypeInput)
		{
			mConnected = false;
			mImagePtr = NULL;
			mMutexPtr = NULL;
			decrementUseCount();

			ioModuleSlot->mModuleSlots.erase(this);
			ioModuleSlot->mConnected = ioModuleSlot->mModuleSlots.size() ? true : false;
			ioModuleSlot->decrementUseCount();
		}
		else
		{
			ioModuleSlot->mImagePtr = NULL;
			ioModuleSlot->mMutexPtr = NULL;
			ioModuleSlot->mModuleSlots.clear();
			ioModuleSlot->mConnected = false;
			ioModuleSlot->decrementUseCount();

			mConnected = mModuleSlots.size() ? true : false;
			decrementUseCount();
		}
	}
	else
		throw(Exception(Exception::eCodeNotConnectedSlot, "Slots " + string(*this) + " and " + string(*ioModuleSlot) + " are not connected together"));
}

/*! \todo
*/
void ModuleSlot::disconnectAll()
{
	ModuleSlotSet::iterator lSlotItr = mModuleSlots.begin();

	if(mType==eSlotTypeInput)
	{
		for(lSlotItr; lSlotItr!=mModuleSlots.end();lSlotItr++)
		{
			(*lSlotItr)->mModuleSlots.erase(this);
			(*lSlotItr)->mConnected = (*lSlotItr)->mModuleSlots.size() ? true : false;
			(*lSlotItr)->decrementUseCount();
			decrementUseCount();
		}
		mModuleSlots.clear();
		mImagePtr = NULL;
		mMutexPtr = NULL;
		mConnected = false;
	}
	else
	{
		for(lSlotItr; lSlotItr!=mModuleSlots.end();lSlotItr++)
		{
			(*lSlotItr)->mImagePtr = NULL;
			(*lSlotItr)->mMutexPtr = NULL;
			(*lSlotItr)->mConnected = false;
			(*lSlotItr)->mModuleSlots.clear();
			(*lSlotItr)->decrementUseCount();
			decrementUseCount();
		}
		mModuleSlots.clear();
		mConnected = false;
	}
}

/*! \todo
*/
const Image* ModuleSlot::operator()() const
{
    if(mImagePtr)
    {
        try
        {
            return *mImagePtr;
        }
        catch(...)
        {
            throw(Exception(Exception::eCodeInvalidImageSlot, string("Slot ") +  string(*this) + string(" has an invalid image pointer")));
        }
    }
    return NULL;
}

/*! \todo
*/
ModuleSlot::operator const Image* () const throw()
{
    if(mImagePtr)
    {
        try
        {
            return *mImagePtr;
        }
        catch(...)
        {
            return NULL;
        }
    }
    return NULL;
}

/*! \todo
*/
const Image* ModuleSlot::getImage() const throw()
{
    if(mImagePtr)
    {
        try
        {
            return *mImagePtr;
        }
        catch(...)
        {
            return NULL;
        }
    }
    return NULL;
}

/*! \todo
*/
void ModuleSlot::lock() const
{
	if(!mConnected && mType==eSlotTypeInput)
		throw(Exception(Exception::eCodeNotConnectedSlot, "Input slot " + string(*this) + " cannot be locked since it is not connected to an output slot"));

	try
	{
		mMutexPtr->lock();
	}
	catch(...)
	{
		throw;
	}
}

/*! \todo
*/
void ModuleSlot::unlock() const
{
	if(!mConnected && mType==eSlotTypeInput)
		throw(Exception(Exception::eCodeNotConnectedSlot, "Input slot " + string(*this) + " cannot be unlocked since it is not connected to an output slot" ));

	try
	{
		mMutexPtr->unlock();
	}
	catch(...)
	{
		throw;
	}
}

/*! \todo
*/
bool ModuleSlot::tryLock() const
{
	bool mLocked;
	if(!mConnected && mType==eSlotTypeInput)
		throw(Exception(Exception::eCodeNotConnectedSlot, "Input slot " + string(*this) + " cannot be locked since it is not connected to an output slot" ));

	try
	{
		mLocked = mMutexPtr->tryLock();
	}
	catch(...)
	{
		throw;
	}
	return mLocked;
}

/*! \todo
*/
string ModuleSlot::getName() const throw()
{
    return mName.c_str();
}

/*! \todo
*/
string ModuleSlot::getDisplayName() const throw()
{
    return mDisplayName.c_str();
}

/*! \todo
*/
string ModuleSlot::getDescription() const throw()
{
    return mDescription.c_str();
}

/*! \todo
*/
string ModuleSlot::getFullName() const throw()
{
	string lTmpStr;
	lTmpStr = mModule->getLabel() + "(" + mName + ")";
	return lTmpStr.c_str();
}

/*! \todo
*/
ModuleSlot::operator string () const throw()
{
	string lTmpStr;
	lTmpStr = mModule->getLabel() + "(" + mName + ")";
	return lTmpStr.c_str();
}

/*! \todo
*/
ModuleSlot::SlotType ModuleSlot::getType() const throw()
{
    return mType;
}

/*! \todo
*/
bool ModuleSlot::isConnected() const throw()
{
	return mConnected;
}

/*! \todo
*/
ModuleSlotSet ModuleSlot::getConnectedSlots() const throw()
{
    return mModuleSlots;
}

/*! \todo
*/
void ModuleSlot::incrementUseCount() throw()
{
	mUseCountMutex.lock();
	mUseCount++;
	mUseCountMutex.unlock();
}

/*! \todo
*/
void ModuleSlot::decrementUseCount() throw()
{
	mUseCountMutex.lock();
	if(mUseCount)
		mUseCount--;
	mUseCountMutex.unlock();
}

/*! \todo
*/
unsigned int ModuleSlot::getUseCount() const throw()
{
	unsigned int lResult;
	mUseCountMutex.lock();
	lResult = mUseCount;
	mUseCountMutex.unlock();
	return lResult;
}

/*! \todo
*/
const Module* ModuleSlot::getModule() const throw()
{
	return mModule;
}
