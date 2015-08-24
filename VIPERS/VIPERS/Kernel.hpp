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
 * \file VIPERS/Kernel.hpp
 * \brief Kernel class header.
 * \author Frederic Jean
 * $Revision: 275 $
 * $Date: 2009-10-23 22:00:34 -0400 (Fri, 23 Oct 2009) $
 */

#ifndef VIPERS_KERNEL_HPP
#define VIPERS_KERNEL_HPP

#include "Exception.hpp"
#include "Module.hpp"
#include "ModulesManager.hpp"
#include "KernelState.hpp"
#include "KernelStateNotifier.hpp"
#include "PACC/Threading/Mutex.hpp"
#include <set>
#include <map>
#include <list>
#include <vector>

namespace VIPERS
{

  using namespace std;
  using namespace PACC;

  //! Typedef for a map of slots
  typedef set<Module*> ModuleSet;
  //! A set of const %Module pointer
  typedef set<const Module*> ModuleSetConst;
  //! Module map sorted by level
  typedef multimap<unsigned int, Module*> SortedLevelModuleMap;
  //! Module level map
  typedef map<Module*, unsigned int> ModuleLevelMap;
  //! A pair of const %Module pointer and const %ModuleSlot pointer
  typedef pair<const Module*, const ModuleSlot*> ModuleSlotPair;
  //! A vector of %ModuleSlotPair used as a stack
  typedef vector<ModuleSlotPair> ModuleSlotPairStack;

  /*! \brief %Kernel virtual base class.
		\author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada

		\todo
   */
  class Kernel
	{
	  public:

	    //! Default explicit constructor
	    Kernel();
	    //! Virtual destructor
	    virtual ~Kernel();

	    //! Initialize all modules
	    virtual void init() = 0;
	    //! Start modules processing
	    virtual void start() = 0;
      //! Pause modules processing
      virtual void pause() = 0;
	    //! Stop modules processing
	    virtual void stop() = 0;
	    //! Refresh all modules for current frame
	    virtual void refresh() = 0;
      //! Process one frame and pause
      virtual void step() = 0;
	    //! Reset all modules
	    virtual void reset() = 0;
	    //! Clear all modules and other data
	    virtual void clear();

	    //! Clear modules
	    void clearModules() throw();

	    //! Get an instance to a module with given name
	    Module* newModule(const string& inName);
	    //! Delete an instance of a module
	    void deleteModule(Module* inModule);

	    //! Load modules in path list
	    void loadModulePathList() throw();
	    //! Load modules in the specified path
	    void loadModulePath(const string& inPath) throw();
	    //! Load module from the specified file
	    bool loadModuleFile(const string& inFile) throw();
	    //! Add a path to the paths list
	    void addPath(const string& inPath) throw();
	    //! Add a paths to the paths list
	    void addPaths(const PathList& inPathList) throw();

	    //! Access %ModulesManager
	    const ModulesManager& getModulesManager() const throw();
	    //! Get instantiated modules
	    ModuleSet getModules() const throw();
	    //! Get instantiated module by its label
	    Module* getModule(const string& inLabel) const throw();
	    //! Get number of modules instantiated
	    unsigned int getModuleCount() const throw();

	    //! Get kernel state
	    KernelState getState() const throw();

	    //! Set kernel state notifier that will be used to notify kernel state changes
	    void setKernelStateNotifier(KernelStateNotifier* inKernelStateNotifier) throw();
	    //! Get the kernel state notifier that has been set
	    KernelStateNotifier* getKernelStateNotifier() const throw();

	    //! Detect cycles in the graph representing the connected modules
	    void detectModuleGraphCycle() const;
	    //!Compute module levels
	    SortedLevelModuleMap computeModuleLevel();

	  protected:

	    //! Set the state and broadcast
	    void setState(KernelState inState) throw();

	    ModulesManager mModulesManager; //!< The %ModulesManager
	    ModuleSet mModuleSet; //!< Module instances

	  private:

	    //! Restrict (disable) copy constructor
	    Kernel(const Kernel&);
	    //! Restrict (disable) assignment operator
	    void operator=(const Kernel&);

	    KernelState mState; //!< Current state of the kernel
	    Threading::Mutex mStateMutex; //!< Mutex for state

	    KernelStateNotifier* mKernelStateNotifier;
	    Threading::Mutex mKernelStateNotifierMutex; //!< Mutex for notifier access
	};

}

#endif //VIPERS_KERNEL_HPP
