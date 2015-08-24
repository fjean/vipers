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
 * \file VIPERS/KernelStateNotifier.hpp
 * \brief KernelStateNotifier class header.
 * \author Frederic Jean
 * $Revision: 273 $
 * $Date: 2009-10-22 22:28:46 -0400 (Thu, 22 Oct 2009) $
 */

#ifndef VIPERS_KERNELSTATENOTIFIER_HPP
#define VIPERS_KERNELSTATENOTIFIER_HPP

#include "KernelState.hpp"
#include "PACC/Threading/Condition.hpp"

namespace VIPERS
{

  using namespace PACC;
  using namespace std;

  /*! \brief %Notifier class.
    \author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada

    \todo
  */
  class KernelStateNotifier:protected Threading::Condition
  {
    public:

      //! Default constructor
      KernelStateNotifier();
      //! Destructor
      virtual ~KernelStateNotifier();

      //! Send kernel state notification
      virtual void notify(const KernelState& inKernelState);
      //! Wait for a notification
      virtual KernelState waitNotification();

      //! Send kernel state notification (must lock and unlock mutex manually)
      virtual void notifyLocked(const KernelState& inKernelState);
      //! Wait for a notification (must lock and unlock mutex manually)
      virtual KernelState waitNotificationLocked();

      //! Set kernel state
      void setState(const KernelState& inKernelState);
      //! Get kernel state
      KernelState getState();

      //! Set kernel state (must lock and unlock mutex manually)
      void setStateLocked(const KernelState& inKernelState);
      //! Get kernel state (must lock and unlock mutex manually)
      KernelState getStateLocked();

      //! Lock mutex
      inline void lock() const {Threading::Condition::lock();}
      //! Unlock mutex
      inline void unlock() const {Threading::Condition::unlock();}

    private:

      KernelState mKernelState;
      bool mNotification;

  };

}

#endif //VIPERS_KERNELSTATENOTIFIER_HPP
