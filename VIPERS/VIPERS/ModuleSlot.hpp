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
 * \file VIPERS/ModuleSlot.hpp
 * \brief ModuleSlot class header.
 * \author Frederic Jean
 * $Revision: 252 $
 * $Date: 2009-10-14 21:52:34 -0400 (Wed, 14 Oct 2009) $
 */

#ifndef VIPERS_MODULE_SLOT_HPP
#define VIPERS_MODULE_SLOT_HPP

#include "Exception.hpp"
#include "Image.hpp"
#include "PACC/Threading/Mutex.hpp"
#include <string>
#include <set>


namespace VIPERS
{

  using namespace std;
  using namespace PACC;

  //Forward declaration
  class ModuleSlot;
  class Module;

  //!Typedef for a list of pointers to module slots
  typedef set<ModuleSlot*> ModuleSlotSet;

  /*! \brief %ModuleSlot class.
		\author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada

		\todo
   */
  class ModuleSlot
  {
    public:

      /*! \brief Type of errors
		\author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada
       */
      enum SlotType
      {
        eSlotTypeInput,	//!< Input slot
        eSlotTypeOutput //!< Output slot
      };

      //! Explicit constructor for input slot
      explicit ModuleSlot(const Module* inModule, const string& inName, const string& inDisplayName, const string& inDescription);
      //! Explicit constructor for output slot
      explicit ModuleSlot(const Module* inModule, const string& inName, const string& inDisplayName, const string& inDescription, Image **inImagePtr);
      //! Destructor
      ~ModuleSlot();

      //! Connect two slot together
      void connect(ModuleSlot* ioModuleSlot);
      //! Disconnect from a slot
      void disconnect(ModuleSlot* ioModuleSlot);
      //! Disnonnect from all slots
      void disconnectAll();

      //! Get reference on image pointer
      const Image* operator()() const;
      //! Get reference on image pointer
      operator const Image* () const throw();
      //! Get reference on image pointer
      const Image* getImage() const throw();
      //! Lock slot mutex
      void lock() const;
      //! Unlock slot mutex
      void unlock() const;
      //! Try lock slot mutex
      bool tryLock() const;

      //! Get module slot name
      string getName() const throw();
      //! Get module slot name
      string getDisplayName() const throw();
      //! Get module slot description
      string getDescription() const throw();
      //! Get full name
      string getFullName() const throw();
      //! Get full name
      operator string () const throw();
      //! Get module slot type
      SlotType getType() const throw();
      //! Return connection status
      bool isConnected() const throw();
      //! Get connected slots list
      ModuleSlotSet getConnectedSlots() const throw();

      //! Increment use count for the slot
      void incrementUseCount() throw();
      //! Decrement use count for the slot
      void decrementUseCount() throw();
      //! Get use count for the slot
      unsigned int getUseCount() const throw();

      //! Get a pointer to the %Module owning the %ModuleSlot
      const Module* getModule() const throw();

    private:

      //! Restrict (disable) copy constructor
      ModuleSlot(const ModuleSlot&);
      //! Restrict (disable) assignment operator
      void operator=(const ModuleSlot&);

      bool mConnected; //!< Connection status
      string mName; //!< %ModuleSlot unique name
      string mDisplayName; //!< %ModuleSlot display name
      string mDescription; //!< %ModuleSlot description
      SlotType mType; //!< Type of slot

      Image** mImagePtr; //!< Image structure pointer reference for the slot
      Threading::Mutex* mMutexPtr; //!< Slot %Mutex pointer

      unsigned int mUseCount; //!< Hold the count of the slot's use
      Threading::Mutex mUseCountMutex; //!< %Mutex for use count

      ModuleSlotSet mModuleSlots; //!< List of connected slots

      const Module* mModule; //!< Pointer to the %Module that owns the %ModuleSlot
  };

}

#endif //VIPERS_MODULE_SLOT_HPP
