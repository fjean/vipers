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
 * \file VIPERS/Monitor.hpp
 * \brief Monitor class header.
 * \author Frederic Jean
 * $Revision: 266 $
 * $Date: 2009-10-20 23:55:59 -0400 (Tue, 20 Oct 2009) $
 */

#ifndef VIPERS_MONITOR_HPP
#define VIPERS_MONITOR_HPP

#include "ModuleSlot.hpp"
#include "Property.hpp"
#include "Notifier.hpp"

namespace VIPERS
{

	using namespace std;
	using namespace PACC;

	// Forward declaration
	class Module;

	/*! \brief %Monitor class.
		\author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada

		\todo
	 */
	class Monitor: public Notifier
	{
	  public:

	    //! Default constructor
	    Monitor();
	    //! Virtual destructor
	    virtual ~Monitor();

	    //! Attach to module
	    void attach(Module* ioModule, bool inAddToModule = true) throw();
	    //! Detach from attached module
	    void detach(bool inRemoveFromModule = true) throw();
	    //! Get module attached to (mutex lock and unlock is performed)
	    Module* getModule() const throw();

	    //! Set the module slot that is going to be monitored
	    void setModuleSlot(ModuleSlot::SlotType inSlotType, const string& inName);
	    //! Unset the module slot that is monitored
	    void unsetModuleSlot() throw();
      //! Get currently monitored module slot (mutex lock and unlock is performed)
      const ModuleSlot* getModuleSlot() const throw();

	    //! Lock module slot
	    void lockModuleSlot();
	    //! Try lock module slot
	    bool tryLockModuleSlot();
	    //! Get locked module slot image
	    const Image* getModuleSlotImage() const;
	    //! Unlock module slot
	    void unlockModuleSlot();

	    //! Is the monitor monitoring
	    bool isMonitoring() const throw();

	    //! Set property
	    void setProperty(const Property& inProperty) throw();
	    //! Get property
	    Property getProperty(const string& inName) const throw();

	    //! Get property list
	    PropertyList getPropertyList() const throw();
	    //! Get property list
	    void setPropertyList(const PropertyList& inPropertyList) throw();

	    //! Erase property
	    bool eraseProperty(const string& inName) throw();
	    //! Clear property list
	    void clearPropertyList() throw();

	  protected:

	    //! Get module attached to (mutex must have been locked)
	    inline Module* getModuleMutexLocked() const {return mModule;}
      //! Get currently monitored module slot (mutex must have been locked)
      inline const ModuleSlot* getModuleSlotMutexLocked() const {return mModuleSlot;}

	  private:

      Module* mModule; //!< Pointer to the monitored %Module
      const ModuleSlot* mModuleSlot; //!< Const pointer to the monitored %ModuleSlot

	    PropertyMap mPropertyMap; //!< List of user defined properties

	};

}

#endif // VIPERS_MONITOR_HPP
