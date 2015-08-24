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
 * \file VIPERS/SequentialKernel.cpp
 * \brief SequentialKernel class functions definition.
 * \author Frederic Jean
 * $Revision: 299 $
 * $Date: 2009-11-02 22:05:40 -0500 (Mon, 02 Nov 2009) $
 */

#include "SequentialKernel.hpp"
#include <iostream>

using namespace VIPERS;
using namespace std;

/*! \todo
*/
SequentialKernel::SequentialKernel()
{
	mThreadCommandChanged = false;
	mThreadCommand = eThreadCommandNone;
	run();
}

/*! \todo
*/
SequentialKernel::~SequentialKernel()
{
	clear();
}

/*! \todo
*/
void SequentialKernel::init()
{
	KernelState lState = getState();

	if(lState==KernelState::eStateStarted || lState==KernelState::eStatePaused)
		throw(Exception(Exception::eCodeInvalidOperationKernelState, "Cannot initialize modules while they are started or paused"));
	if(mModuleSet.size()==0)
		throw(Exception(Exception::eCodeInvalidOperationKernelState, "There is no modules in the kernel"));

	sendCommand(eThreadCommandInit);

}

/*! \todo
*/
void SequentialKernel::start()
{
	KernelState lState = getState();

	if(mModuleSet.size()==0)
		throw(Exception(Exception::eCodeInvalidOperationKernelState, "There is no modules in the kernel"));
	if(lState==KernelState::eStateUninitialized)
		throw(Exception(Exception::eCodeInvalidOperationKernelState, "Cannot start the modules since they are not initialized"));
	if(lState==KernelState::eStateStarted)
		throw(Exception(Exception::eCodeInvalidOperationKernelState, "Modules are already started"));

  sendCommand(eThreadCommandStart);

}

/*! \todo
*/
void SequentialKernel::pause()
{
  KernelState lState = getState();

  if(lState!=KernelState::eStateStarted)
    throw(Exception(Exception::eCodeInvalidOperationKernelState, "Cannot pause the modules since they are not started"));

  sendCommand(eThreadCommandPause);

}

/*! \todo
*/
void SequentialKernel::stop()
{
	KernelState lState = getState();

	if(lState!=KernelState::eStateStarted && lState!=KernelState::eStatePaused)
		throw(Exception(Exception::eCodeInvalidOperationKernelState, "Modules cannot be stopped since they are not started or paused"));

	sendCommand(eThreadCommandStop);

}

/*! \todo
*/
void SequentialKernel::refresh()
{
	KernelState lState = getState();

	if(lState!=KernelState::eStatePaused)
		throw(Exception(Exception::eCodeInvalidOperationKernelState, "Cannot refresh current frame since the modules are not paused"));

	sendCommand(eThreadCommandRefresh);

}

/*! \todo
*/
void SequentialKernel::step()
{
  KernelState lState = getState();

  if(lState!=KernelState::eStatePaused)
    throw(Exception(Exception::eCodeInvalidOperationKernelState, "Cannot process just one frame since the modules are not paused"));

  sendCommand(eThreadCommandStep);

}

/*! \todo
*/
void SequentialKernel::reset()
{
	KernelState lState = getState();

	if(lState==KernelState::eStateStarted || lState==KernelState::eStatePaused)
		throw(Exception(Exception::eCodeInvalidOperationKernelState, "Modules cannot be reseted since they are started or paused"));
	if(mModuleSet.size()==0)
		throw(Exception(Exception::eCodeInvalidOperationKernelState, "There is no modules in the kernel"));

	sendCommand(eThreadCommandReset);

}

/*! \todo
*/
void SequentialKernel::clear()
{
  /*
  KernelState lState = getState();
	if(lState==KernelState::eStateRunning || lState==KernelState::eStatePaused)
	{
		stop();
		//Wait for thread termination
		wait();
	}
	*/

  // Wait for the thread to stop
  sendCommand(eThreadCommandExit);
  wait();

	Kernel::clear();
}

/*! \todo
*/
void SequentialKernel::main()
{
  ThreadCommand lThreadCommand;

  // Thread command loop: loop until command is "exit"
  while((lThreadCommand = waitCommand()) != eThreadCommandExit)
  {

    if(lThreadCommand==eThreadCommandInit)
    {
      initFunction();
    }
    else if(lThreadCommand==eThreadCommandStart)
    {
      startFunction();
    }
    else if(lThreadCommand==eThreadCommandPause)
    {
      pauseFunction();
    }
    else if(lThreadCommand==eThreadCommandStop)
    {
      stopFunction();
    }
    else if(lThreadCommand==eThreadCommandRefresh)
    {
      refreshFunction();
    }
    else if(lThreadCommand==eThreadCommandStep)
    {
      stepFunction();
    }
    else if(lThreadCommand==eThreadCommandReset)
    {
      resetFunction();
    }

  }

}

/*! \todo
*/
void SequentialKernel::sendCommand(ThreadCommand inThreadCommand)
{
  mThreadCommandCondition.lock();

  mThreadCommand = inThreadCommand;
  mThreadCommandChanged = true;
  mThreadCommandCondition.signal();

  mThreadCommandCondition.unlock();
}

/*! \todo
*/
SequentialKernel::ThreadCommand SequentialKernel::waitCommand()
{
  ThreadCommand lThreadCommand;

  mThreadCommandCondition.lock();

  while(!mThreadCommandChanged)
    mThreadCommandCondition.wait();

  lThreadCommand = mThreadCommand;
  mThreadCommand = eThreadCommandNone;
  mThreadCommandChanged = false;

  mThreadCommandCondition.unlock();

  return lThreadCommand;
}

/*! \todo
*/
bool SequentialKernel::isCommandChanged()
{
  bool lIsCommandChanged;
  mThreadCommandCondition.lock();
  lIsCommandChanged = mThreadCommandChanged;
  mThreadCommandCondition.unlock();
  return lIsCommandChanged;
}

/*! \todo
*/
void SequentialKernel::initFunction()
{
  KernelState lState;
  SortedLevelModuleMap::iterator lSortedLevelModuleMapItr;

  mSortedLevelModuleMap.clear();
  lState.setFrame(0);

  // Get module levels
  try
  {
    mSortedLevelModuleMap = computeModuleLevel();

    //Loop on modules (sorted by level)
    for(lSortedLevelModuleMapItr = mSortedLevelModuleMap.begin(); lSortedLevelModuleMapItr != mSortedLevelModuleMap.end(); lSortedLevelModuleMapItr++)
      lSortedLevelModuleMapItr->second->init();

    // Set state to frame 0 and process state initialized
    lState.setState(KernelState::eStateInitialized);
  }
  catch(Exception inException)
  {
    lState.setException(inException);
    lState.setState(KernelState::eStateUninitialized);
  }
  catch(...)
  {
    lState.setException(Exception(Exception::eCodeUndefined, "An undefined error while initializing modules"));
    lState.setState(KernelState::eStateUninitialized);
  }

  setState(lState);
}

/*! \todo
*/
void SequentialKernel::startFunction()
{
  SortedLevelModuleMap::iterator lSortedLevelModuleMapItr;
  SortedLevelModuleMap::iterator lTmpSortedLevelModuleMapItr;
  KernelState lState = getState();
  unsigned int lMaxNumberFrames = 0; // 0 = infinity
  unsigned int lTmpNumberFrames;
  unsigned int lCurrentFrameNumber = lState.getFrame();
  bool lDone = false;


  if(lState.getState()!=KernelState::eStatePaused)
  {
    // Find maximum number of frame that can be processed
    // This will be the module with the lower reported number of frame (0 means infinity)
    // Also call the start function of each module
    try
    {
      //Loop on modules (sorted by level)
      for(lSortedLevelModuleMapItr = mSortedLevelModuleMap.begin(); lSortedLevelModuleMapItr != mSortedLevelModuleMap.end(); lSortedLevelModuleMapItr++)
      {
        lTmpNumberFrames = lSortedLevelModuleMapItr->second->getMaxNumberFrames();
        if( lTmpNumberFrames !=0 && (lMaxNumberFrames == 0 || lTmpNumberFrames < lMaxNumberFrames) )
          lMaxNumberFrames = lTmpNumberFrames;
        lSortedLevelModuleMapItr->second->start();
      }
      lState.setMaximumFrame(lMaxNumberFrames);
    }
    catch(Exception inException)
    {
      //Stop started modules
      if(lSortedLevelModuleMapItr!= mSortedLevelModuleMap.begin())
      {
        lTmpSortedLevelModuleMapItr = --lSortedLevelModuleMapItr;
        for(lTmpSortedLevelModuleMapItr; lTmpSortedLevelModuleMapItr == mSortedLevelModuleMap.begin(); lTmpSortedLevelModuleMapItr--)
        {
          try
          {
            lTmpSortedLevelModuleMapItr->second->stop();
          }
          catch(...)
          {

          }
        }
      }

      // Put state back to initialized and raise exception
      lState.setState(KernelState::eStateStopped);
      lState.setException(inException);
      setState(lState);
      return;
    }
    catch(...)
    {
      //Stop started modules
      if(lSortedLevelModuleMapItr!= mSortedLevelModuleMap.begin())
      {
        lTmpSortedLevelModuleMapItr = --lSortedLevelModuleMapItr;
        for(lTmpSortedLevelModuleMapItr; lTmpSortedLevelModuleMapItr == mSortedLevelModuleMap.begin(); lTmpSortedLevelModuleMapItr--)
        {
          try
          {
            lTmpSortedLevelModuleMapItr->second->stop();
          }
          catch(...)
          {
            cerr << "ERROR: Module \"" << lTmpSortedLevelModuleMapItr->second->getLabel().c_str() << "\" could not be stopped after being started" << endl;
          }
        }
      }

      // Put state back to initialized and raise exception
      lState.setState(KernelState::eStateStopped);
      lState.setException(Exception(Exception::eCodeUndefined, "Happened while starting modules"));
      setState(lState);
      return;
    }
  }
  // Was paused
  else
  {
    try
    {
      //Loop on modules (sorted by level)
      for(lSortedLevelModuleMapItr = mSortedLevelModuleMap.begin(); lSortedLevelModuleMapItr != mSortedLevelModuleMap.end(); lSortedLevelModuleMapItr++)
        lSortedLevelModuleMapItr->second->start();
    }
    catch(Exception inException)
    {
      //Stop started modules
      if(lSortedLevelModuleMapItr!= mSortedLevelModuleMap.begin())
      {
        lTmpSortedLevelModuleMapItr = --lSortedLevelModuleMapItr;
        for(lTmpSortedLevelModuleMapItr; lTmpSortedLevelModuleMapItr == mSortedLevelModuleMap.begin(); lTmpSortedLevelModuleMapItr--)
        {
          try
          {
            lTmpSortedLevelModuleMapItr->second->stop();
          }
          catch(...)
          {

          }
        }
      }

      // Put state back to initialized and raise exception
      lState.setState(KernelState::eStateStopped);
      lState.setException(inException);
      setState(lState);
      return;
    }
    catch(...)
    {
      //Stop started modules
      if(lSortedLevelModuleMapItr!= mSortedLevelModuleMap.begin())
      {
        lTmpSortedLevelModuleMapItr = --lSortedLevelModuleMapItr;
        for(lTmpSortedLevelModuleMapItr; lTmpSortedLevelModuleMapItr == mSortedLevelModuleMap.begin(); lTmpSortedLevelModuleMapItr--)
        {
          try
          {
            lTmpSortedLevelModuleMapItr->second->stop();
          }
          catch(...)
          {
            cerr << "ERROR: Module \"" << lTmpSortedLevelModuleMapItr->second->getLabel().c_str() << "\" could not be stopped after being started" << endl;
          }
        }
      }

      // Put state back to initialized and raise exception
      lState.setState(KernelState::eStateStopped);
      lState.setException(Exception(Exception::eCodeUndefined, "Happened while starting modules"));
      setState(lState);
      return;
    }
  }

  lState.setState(KernelState::eStateStarted);

  // Loop until max number of frames is reached, or new command has arrived
  while(true)
  {
    // Check for max number of frame
    if(lMaxNumberFrames > 0 && lCurrentFrameNumber >= lMaxNumberFrames)
    {
      lDone = true;
      break;
    }

    // Check for new command
    if(isCommandChanged())
      break;

    // Call the process function of each module
    try
    {

      //Loop on modules (sorted by level)
      for(lSortedLevelModuleMapItr = mSortedLevelModuleMap.begin(); lSortedLevelModuleMapItr != mSortedLevelModuleMap.end(); lSortedLevelModuleMapItr++)
      {
        lSortedLevelModuleMapItr->second->process(lCurrentFrameNumber);
      }

    }
    catch(Exception& inException)
    {

      //Stop started modules
      if(lSortedLevelModuleMapItr!= mSortedLevelModuleMap.begin())
      {
        lTmpSortedLevelModuleMapItr = --lSortedLevelModuleMapItr;
        for(lTmpSortedLevelModuleMapItr; lTmpSortedLevelModuleMapItr == mSortedLevelModuleMap.begin(); lTmpSortedLevelModuleMapItr--)
        {
          try
          {
            lTmpSortedLevelModuleMapItr->second->stop();
          }
          catch(...)
          {
            cerr << "ERROR: Module \"" << lTmpSortedLevelModuleMapItr->second->getLabel().c_str() << "\" could not be stopped after being started" << endl;
          }
        }
      }

      // Put state back to stopped and raise exception
      lState.setState(KernelState::eStateStopped);
      lState.setException(inException);
      setState(lState);
      return;
    }
    catch(...)
    {

      //Stop started modules
      if(lSortedLevelModuleMapItr!= mSortedLevelModuleMap.begin())
      {
        lTmpSortedLevelModuleMapItr = --lSortedLevelModuleMapItr;
        for(lTmpSortedLevelModuleMapItr; lTmpSortedLevelModuleMapItr == mSortedLevelModuleMap.begin(); lTmpSortedLevelModuleMapItr--)
        {
          try
          {
            lTmpSortedLevelModuleMapItr->second->stop();
          }
          catch(...)
          {
            cerr << "ERROR: Module \"" << lTmpSortedLevelModuleMapItr->second->getLabel().c_str() << "\" could not be stopped after being started" << endl;
          }
        }
      }

      // Put state back to stopped and raise exception
      lState.setState(KernelState::eStateStopped);
      lState.setException(Exception(Exception::eCodeUndefined, "Happened while looping on modules (processing)"));
      setState(lState);
      return;
    }

    // Increment current frame, and set state
    lState.setFrame(lCurrentFrameNumber);
    setState(lState);
    lCurrentFrameNumber++;

  }

  // Call the stop function
  if(lDone)
    stopFunction();

}

/*! \todo
*/
void SequentialKernel::stopFunction()
{
  SortedLevelModuleMap::iterator lSortedLevelModuleMapItr;
  KernelState lState = getState();

  // Stop all modules
  for(lSortedLevelModuleMapItr = mSortedLevelModuleMap.begin(); lSortedLevelModuleMapItr != mSortedLevelModuleMap.end(); lSortedLevelModuleMapItr++)
  {
    try
    {
      lSortedLevelModuleMapItr->second->stop();
    }
    catch(Exception inException)
    {
      cerr << inException << endl;
    }
    catch(...)
    {
      cerr << "ERROR: Module \"" << lSortedLevelModuleMapItr->second->getLabel().c_str() << "\" could not be stopped" << endl;
    }
  }

  // Processing done, put state back to initialized
  lState.setState(KernelState::eStateStopped);
  setState(lState);

}

/*! \todo
*/
void SequentialKernel::pauseFunction()
{
  SortedLevelModuleMap::iterator lSortedLevelModuleMapItr;
  KernelState lState = getState();
  bool lException = false;

  // Pause all modules
  for(lSortedLevelModuleMapItr = mSortedLevelModuleMap.begin(); lSortedLevelModuleMapItr != mSortedLevelModuleMap.end(); lSortedLevelModuleMapItr++)
  {
    try
    {
      lSortedLevelModuleMapItr->second->pause();
    }
    catch(...)
    {
      lException = true;
      cerr << "ERROR: Module \"" << lSortedLevelModuleMapItr->second->getLabel().c_str() << "\" could not be paused" << endl;
    }
  }

  // Put state to pauses
  if(!lException)
    lState.setState(KernelState::eStatePaused);
  setState(lState);
}

/*! \todo
*/
void SequentialKernel::refreshFunction()
{
  SortedLevelModuleMap::iterator lSortedLevelModuleMapItr;
  KernelState lState = getState();

  // Get module levels
  try
  {
    //Loop on modules (sorted by level)
    for(lSortedLevelModuleMapItr = mSortedLevelModuleMap.begin(); lSortedLevelModuleMapItr != mSortedLevelModuleMap.end(); lSortedLevelModuleMapItr++)
      lSortedLevelModuleMapItr->second->process(lState.getFrame());
  }
  catch(Exception inException)
  {
    lState.setException(inException);
  }
  catch(...)
  {
    lState.setException(Exception(Exception::eCodeUseModule, "An error happened while refreshing current frame"));
  }

  setState(lState);
}

/*! \todo
*/
void SequentialKernel::stepFunction()
{
  SortedLevelModuleMap::iterator lSortedLevelModuleMapItr;
  KernelState lState = getState();

  unsigned int lMaxFrame = lState.getMaximumFrame();
  unsigned int lCurrentFrame = lState.getFrame();

  if(lMaxFrame==0 || (lMaxFrame>0 && lCurrentFrame<lMaxFrame))
  {
    try
    {
      //Loop on modules (sorted by level)
      for(lSortedLevelModuleMapItr = mSortedLevelModuleMap.begin(); lSortedLevelModuleMapItr != mSortedLevelModuleMap.end(); lSortedLevelModuleMapItr++)
      {
        lSortedLevelModuleMapItr->second->start();
        lSortedLevelModuleMapItr->second->process(lCurrentFrame+1);
        lSortedLevelModuleMapItr->second->pause();
      }
    }
    catch(Exception inException)
    {
      lState.setException(inException);
    }
    catch(...)
    {
      lState.setException(Exception(Exception::eCodeUseModule, "An error happened while processing one frame"));
    }
  }

  lState.setFrame(lCurrentFrame+1);
  setState(lState);

  if(lMaxFrame>0 && (lCurrentFrame+1)==lMaxFrame)
    stopFunction();

}

/*! \todo
*/
void SequentialKernel::resetFunction()
{
   SortedLevelModuleMap::iterator lSortedLevelModuleMapItr;
   KernelState lState = getState();

   // Get module levels
   try
   {
     //Loop on modules (sorted by level)
     for(lSortedLevelModuleMapItr = mSortedLevelModuleMap.begin(); lSortedLevelModuleMapItr != mSortedLevelModuleMap.end(); lSortedLevelModuleMapItr++)
       lSortedLevelModuleMapItr->second->reset();
   }
   catch(Exception inException)
   {
     lState.setException(inException);
     setState(lState);
     return;
   }
   catch(...)
   {
     lState.setException(Exception(Exception::eCodeUseModule, "An error happened while resetting modules"));
     setState(lState);
     return;
   }

   // Set state to frame 0 and state to uninitialized
   lState.setFrame(0);
   lState.setState(KernelState::eStateUninitialized);
   setState(lState);
}

