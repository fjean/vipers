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
 * \file VIPERS/SequentialKernel.hpp
 * \brief SequentialKernel class header.
 * \author Frederic Jean
 * $Revision: 274 $
 * $Date: 2009-10-23 20:34:51 -0400 (Fri, 23 Oct 2009) $
 */

#ifndef VIPERS_SEQUENTIAL_KERNEL_HPP
#define VIPERS_SEQUENTIAL_KERNEL_HPP

#include "Kernel.hpp"
#include "PACC/Threading/Thread.hpp"
#include "PACC/Threading/Condition.hpp"

namespace VIPERS
{

  using namespace std;
  using namespace PACC;

  /*! \brief %SequentialKernel virtual base class.
		\author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada

		\todo
   */
  class SequentialKernel: public Kernel, protected Threading::Thread
  {
    public:

    //! Default explicit constructor
    SequentialKernel();
    //! Virtual destructor
    virtual ~SequentialKernel();

    //! Initialize all modules
    void init();
    //! Start modules processing
    void start();
    //! Pause modules processing
    void pause();
    //! Stop modules processing
    void stop();
    //! Refresh all modules for current frame
    void refresh();
    //! Process one frame and pause
    void step();
    //! Reset all modules
    void reset();
    //! Clear all modules and other data
    void clear();

    protected:

    //! Main thread function
    void main();

    private:

    /*! \brief Thread command
    \author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada
     */
    enum ThreadCommand
    {
      eThreadCommandNone,
      eThreadCommandInit,
      eThreadCommandStart,
      eThreadCommandStop,
      eThreadCommandPause,
      eThreadCommandReset,
      eThreadCommandRefresh,
      eThreadCommandStep,
      eThreadCommandExit
    };

    //! Set command to thread
    void sendCommand(ThreadCommand inThreadCommand);
    //! Wait for command
    ThreadCommand waitCommand();
    //! Check if command has changed
    bool isCommandChanged();

    //! Thread initialize modules function
    void initFunction();
    //! Thread start module processing function
    void startFunction();
    //! Thread stop module processing function
    void stopFunction();
    //! Thread pause module processing function
    void pauseFunction();
    //! Thread refresh current frame function
    void refreshFunction();
    //! Thread process one frame and pause
    void stepFunction();
    //! Thread reset modules function
    void resetFunction();

    ThreadCommand mThreadCommand; //!< Command
    bool mThreadCommandChanged;
    Threading::Condition mThreadCommandCondition; //!< Condition to notify thread of new commands

    SortedLevelModuleMap mSortedLevelModuleMap;

  };

}

#endif //VIPERS_SEQUENTIAL_KERNEL_HPP
