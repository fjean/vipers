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
 * \file VIPERS/Module.hpp
 * \brief Module class header.
 * \author Frederic Jean
 * $Revision: 252 $
 * $Date: 2009-10-14 21:52:34 -0400 (Wed, 14 Oct 2009) $
 */

#ifndef VIPERS_MODULE_HPP
#define VIPERS_MODULE_HPP

#include "Exception.hpp"
#include "ModuleSlot.hpp"
#include "Monitor.hpp"
#include "Parameter.hpp"
#include "Property.hpp"
#include "PACC/Threading/Mutex.hpp"
#include <string>
#include <map>
#include <set>
#include <vector>

namespace VIPERS
{

	using namespace std;

	//! Typedef for a map of slots
	typedef map<string, ModuleSlot*> ModuleSlotMap;

	//! Typedef for a map of parameters
	typedef map<string, Parameter> ParameterMap;

	//! Typedef for a list of parameters
	typedef vector<Parameter> ParameterList;

	//! Typedef for a set of %Monitor
	typedef set<Monitor*> MonitorSet;

	/*! \brief %Module virtual base class.
		\author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada

		\todo
	*/
	class Module
	{
	  public:

	    enum State
	    {
	      eStateUninitialized, //!< Module is uninitialized (or reseted)
	      eStateInitialized, //!< Module is initialized
	      eStateStarted, //!< Module is started
	      eStatePaused, //!< Module is paused
	      eStateStopped //!< Module is stopped
	    };

	    //! Default explicit constructor
	    explicit Module(const string& inName, const string& inDisplayName, const string& inVersion);
	    //! Virtual destructor
	    virtual ~Module();

	    //! Verifies inputs and initializes outputs
	    void init();
	    //! Put the %Module in a ready state for processing
	    void start();
	    //! Put the %Module in a paused state
	    void pause();
	    //! Processes current inputs and/or generates outputs for one cycle
	    void process(unsigned int inFrameNumber);
	    //! Put the %Module in a stopped state (no more ready for processing)
	    void stop();
	    //! Resetting the %Module as it was when created
	    void reset();

	    //! Update %Module parameters
	    ParameterList updateParameters();
	    //! Verify if %Parameter value is valid without actually changing the value
	    string verifyParameter(const Parameter& inParameter) const throw();

	    //! Connect an input slot to another %Module output slot
	    void connect(ModuleSlot::SlotType inSlotType, const string& inSlotName, Module* ioModule, const string& inModuleSlotName);
	    //! Disconnect an input slot
	    void disconnect(const string& inInputSlotName);
	    //! Disconnect all slots of the given type
	    void disconnectAll(ModuleSlot::SlotType inSlotType) throw();
	    //! Disconnect all slots (input and output)
	    void disconnectAll() throw();

	    //! Get %Module label
	    string getLabel() const throw();
	    //! Get %Module name
	    string getName() const throw();
	    //! Get %Module display name
	    string getDisplayName() const throw();
	    //! Get %Module short description
	    string getShortDescription() const throw();
	    //! Get %Module long description
	    string getLongDescription() const throw();
	    //! Get %Module documentation
	    string getDocumentation() const throw();
	    //! Get %Module version
	    string getVersion() const throw();
	    //! Get %Module state
	    State getState() const throw();

	    //! Maximum number of frame that the %Module can process
	    unsigned int getMaxNumberFrames() throw();
	    //! Get the frame rate at which the %Module can process frames
	    double getFrameRate() throw();

	    //! Get input slots list
	    const ModuleSlotMap& getInputSlots() const throw();
	    //! Get output slots list
	    const ModuleSlotMap& getOutputSlots() const throw();
	    //! Get input slot by name
	    const ModuleSlot* getInputSlot(const string& inName) const throw();
	    //! Get output slot by name
	    const ModuleSlot* getOutputSlot(const string& inName) const throw();
	    //! Check if any input slot connected
	    bool isAnyInputSlotConnected() const throw();
	    //! Check if any output slot connected
	    bool isAnyOutputSlotConnected() const throw();

	    //! Set %Module label
	    void setLabel(string inLabel) throw();

	    //! Get a parameter list
	    ParameterList getParameterList() const throw();
	    //! Get a parameter map
	    ParameterMap getParameterMap() const throw();
	    //! Get a parameter by its name
	    Parameter getParameter(const string& inName) const;
	    //! Set a parameter value
	    void setParameterValue(const string& inName, const string& inValue);
	    //! Set a parameter value
	    void setParameterValue(const Parameter& inParameter);

	    //! Attach a %Monitor
	    void attachMonitor(Monitor* ioMonitor, bool inAddToMonitor=true) throw();
	    //! Attach a %Monitor
	    void detachMonitor(Monitor* ioMonitor, bool inRemoveFromMonitor=true) throw();
	    //! Attach all %Monitor
	    void detachAllMonitor() throw();
	    //! Get attached monitors
	    MonitorSet getMonitors() const throw();
	    //! Notify all monitors
	    void notifyMonitors() const throw();
	    //! Is monitor attached
	    bool isMonitorAttached(Monitor* inMonitor) const throw();

	    //! Increment ModuleSlot use count
	    void incrementModuleSlotUseCount(ModuleSlot::SlotType inSlotType, const string& inSlotName);
	    //! Decrement ModuleSlot use count
	    void decrementModuleSlotUseCount(ModuleSlot::SlotType inSlotType, const string& inSlotName);

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

	    //! Verifies inputs and initializes outputs (Module specific)
	    virtual void initFunction() = 0;
	    //! Put the %Module in a ready state for processing (Module specific)
	    virtual void startFunction() = 0;
	    //! Put the %Module in a paused state (Module specific)
	    virtual void pauseFunction() = 0;
	    //! Processes current inputs and/or generates outputs for one cycle (Module specific)
	    virtual void processFunction(unsigned int inFrameNumber) = 0;
	    //! Put the %Module in a stopped state (no more ready for processing) (Module specific)
	    virtual void stopFunction() = 0;
	    //! Resetting the %Module as it was when created (Module specific)
	    virtual void resetFunction() = 0;

	    //! Update %Module parameters (Module specific)
	    virtual void updateParametersFunction() = 0;
	    //! Verify if Parameter value is valid without actually changing the value (Module specific)
	    virtual string verifyParameterFunction(const Parameter& inParameter) const throw() = 0;

	    //! Define a new slot (for %Module development)
	    ModuleSlot* newSlot(ModuleSlot* inSlot);
	    //! Get input slot by name (for %Module development)
	    ModuleSlot* getInputSlot(const string& inName) throw();
	    //! Get input slot by name (for %Module development)
	    ModuleSlot* getOutputSlot(const string& inName) throw();

	    //! Set the maximum number of frame (for %Module development)
	    void setMaxNumberFrames(unsigned int inMaxNbFrames) throw();
	    //! Set the maximum number of frame (for %Module development)
	    void setFrameRate(double inFrameRate) throw();

	    //! Define a new parameter (for %Module development)
	    void newParameter(const Parameter& inParameter) throw();

	    //! Lock access to parameters (for %Module development)
	    void lockParameters() const;
	    //! Unlock access to parameters (for %Module development)
	    void unlockParameters() const;
	    //! Get a parameter (for %Module development)
	    Parameter getLockedParameter(const string& inName) const;
	    //! Get a parameter (for %Module development)
	    void setLockedParameter(const Parameter& inParameter);

	    string mShortDescription; //!< %Module short description
	    string mLongDescription; //!< %Module long description
	    string mDocumentation; //!< %Module documentation

	  private:

	    //! Restrict (disable) copy constructor
	    Module(const Module&);
	    //! Restrict (disable) assignment operator
	    void operator=(const Module&);
	    //! Set Module state
	    void setState(State inState) throw();


	    string mName; //!< %Module unique name
	    string mDisplayName; //!< %Module display name

	    Threading::Mutex mLabelMutex; //!< Mutex used to protect access to label
	    string mLabel; //!< %Module unique label

	    string mVersion; //!< %Module version string

	    ModuleSlotMap mInputSlots; //!< List of input slots
	    ModuleSlotMap mOutputSlots; //!< List of output slots

	    ParameterMap mParameters; //!< List of parameters for the %Module
	    Threading::Mutex mParametersMutex; //!< Mutex used to protect access to parameters

	    unsigned int mMaxNumberFrame; //!< Maximum number of frame that can be processed
	    Threading::Mutex mMaxNumberFramesMutex; //!< Mutex used to protect access to mMaxNumberFrame variable

	    double mFrameRate; //!< Frame rate at which the %Module can process frames
	    Threading::Mutex mFrameRateMutex; //!< Mutex used to protect access to mFrameRate variable

	    MonitorSet mMonitorSet; //!< Set of attached monitors
	    Threading::Mutex mMonitorSetMutex; //!< Monitor set mutex

	    State mState; //!< Module state
	    Threading::Mutex mStateMutex; //!< Module state mutex

	    PropertyMap mPropertyMap; //!< List of user defined properties

	};

}

#endif //VIPERS_MODULE_HPP
