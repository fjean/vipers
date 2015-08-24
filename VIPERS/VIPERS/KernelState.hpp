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
 * \file VIPERS/KernelState.hpp
 * \brief KernelState class header.
 * \author Frederic Jean
 * $Revision: 274 $
 * $Date: 2009-10-23 20:34:51 -0400 (Fri, 23 Oct 2009) $
 */

#ifndef VIPERS_KERNELSTATE_HPP
#define VIPERS_KERNELSTATE_HPP

#include "Exception.hpp"

namespace VIPERS
{

  using namespace std;

  /*! \brief %KernelState class.
    \author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada

    \todo
  */
  class KernelState
  {
    public:

    /*! \brief Kernel states
    \author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada
    */
    enum State
    {
      eStateUninitialized, //!< Kernel is uninitialized (or reseted)
      eStateInitialized, //!< Kernel is initialized
      eStateStarted, //!< Kernel is started
      eStatePaused, //!< Kernel is paused
      eStateStopped //!< Kernel is stopped
    };

    //! Default constructor
    KernelState();
    //! Destructor
    virtual ~KernelState();

    //! Get state
    State getState() const throw();
    //! Get current frame
    unsigned int getFrame() const throw();
    //! Get maximum frame number
    unsigned int getMaximumFrame() const throw();
    //! Get state
    operator State() const throw();
    //! Get current frame
    operator unsigned int() const throw();

    //! Set state
    void setState(State inState) throw();
    //! Set current frame
    void setFrame(unsigned int inFrame) throw();
    //! Set maximum frame number
    void setMaximumFrame(unsigned int inMaximumFrame) throw();

    //! Set state
    KernelState& operator=(const State& inState) throw();
    //! Set current frame
    KernelState& operator=(unsigned int inFrame) throw();

    //! Check if there was an Exception raised
    bool isExceptionRaised() const throw();
    //! Get raised exception and reset exception flag
    Exception getException() throw();
    //! Set exception and set exception flag
    void setException(const Exception& inException) throw();

    private:

    State mState; //!< Kernel processing state
    unsigned int mFrame; //!< Frame currently being processed
    unsigned int mMaximumFrame; //!< Maximum number of frame to process (<0 means no maximum)

    bool mIsExceptionRaised; //!< Did an Exception occurred while processing
    Exception mException; //!< Raised exception

  };

}

#endif //VIPERS_KERNELSTATE_HPP
