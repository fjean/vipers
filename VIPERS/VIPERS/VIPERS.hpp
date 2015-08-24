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
 * \file VIPERS/VIPERS.hpp
 * \brief Header with common definition.
 * \author Frederic Jean
 * $Revision: 252 $
 * $Date: 2009-10-14 21:52:34 -0400 (Wed, 14 Oct 2009) $
 */

/*! \mainpage

%Video and Image Processing Environment for Real-time Systems (VIPERS) is a framework used to perform video and image processing in real-time.  The framework basically consists in a collection of modules (plugins) linked together to perform a specific task.  Each module consist of a certain number of input and output slots on which processing will be performed.  Modules get connected together by connecting an output slot to one or more input slots. Once the module layout is done, processing is performed by making each module processes their inputs and produces their outputs.  The processing is performed one frame at a time until one of the module cannot produce any more outputs (ex. a module reading a video file and reaching the last frame), or until it is explicitely stopped (like in case of streaming).  A kernel object is responsible for coordinating modules execution flow.  How and when each module is executed depends on the kernel that as been choosen (sequential or multi-threaded) as well as the module layout (output-only modules are executed first for a frame, and input-only modules are executed last).

%VIPERS is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

It is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the <a href="http://www.gnu.org/copyleft/lesser.html">GNU Lesser General Public License</a> for more details.

 \author Fr&eacute;d&eacute;ric Jean<br>
<a href="http://vision.gel.ulaval.ca/en/index.php">Computer Vision and Systems Laboratory (CVSL)</a><br>
<a href="http://www.gel.ulaval.ca">Department of Electrical and Computer Engineering</a><br>
<a href="http://www.ulaval.ca/">Universit&eacute; Laval</a>, Qu&eacute;bec, QC, Canada, G1V 0A6

*/

#ifndef VIPERS_HPP
#define VIPERS_HPP

#include "VIPERSConfig.hpp"

/*!
 *	\namespace VIPERS
 *	\brief Video and Image Processing Environment for Real-time Systems (VIPERS)
 *	\author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada
 */
namespace VIPERS
{
	// Forward declaration of the Module class for the purpose of using a pointer
	// to a Module in the following macro
	class Module;
}

//Macros definition

//! Name of the C function in dynamic libraries (modules) used to create a new module instance
#define NEW_VIPERS_MODULE_FUNC_NAME "CreateNewVIPERSModule"
//! Name of the C function in dynamic libraries (modules) used to delete a module instance
#define DELETE_VIPERS_MODULE_FUNC_NAME "DeleteVIPERSModule"

#ifdef VIPERS_OS_WINDOWS

	#include <windows.h>

	//! Macro for module dynamic library entry point function definition
	#define VIPERS_MODULE_ENTRY_POINT	\
		BOOL WINAPI DllMain (HANDLE hModule, DWORD dwFunction, LPVOID lpNot) \
		{ \
			return TRUE; \
		} \

	//! Macro to faciliate the definition of the create and delete function for a module
	#define VIPERS_MODULE_FUNCTIONS(VIPERS_MODULE_CLASS) \
		extern "C" \
		{	\
			__declspec(dllexport) VIPERS::Module* CreateNewVIPERSModule() \
			{ \
				return new VIPERS_MODULE_CLASS(); \
			} \
			__declspec(dllexport) void DeleteVIPERSModule(VIPERS::Module* inPtr) \
			{ \
				delete reinterpret_cast<VIPERS_MODULE_CLASS*>(inPtr); \
			} \
		}	\

#else

	//! Macro for module dynamic library entry point function definition
	#define VIPERS_MODULE_ENTRY_POINT

	//! Macro to faciliate the definition of the create and delete function for a module
	#define VIPERS_MODULE_FUNCTIONS(VIPERS_MODULE_CLASS) \
		extern "C" \
		{	\
			void* CreateNewVIPERSModule() \
			{ \
				return reinterpret_cast<void*>(new VIPERS_MODULE_CLASS()); \
			} \
			void DeleteVIPERSModule(void* inPtr) \
			{ \
				delete reinterpret_cast<VIPERS_MODULE_CLASS*>(inPtr); \
			} \
		}	\

#endif


#ifdef VIPERS_DEBUG

	#include <iostream>
	#include <cstdlib>

	//! Macro for VIPERS assertion in debug code
	#define VIPERS_ASSERT(COND,MESSAGE) \
		if(!(COND)) { \
			std::cerr << "\n*************** VIPERS assert failed **************\nin "; \
			std::cerr << __FILE__ << ":" << __LINE__ << "\n" << MESSAGE; \
			std::cerr << "\n***************************************************" << std::endl; \
			exit(EXIT_FAILURE); \
		} \

#else

	//! Macro for VIPERS assertion in debug code
	#define VIPERS_ASSERT(COND,MESSAGE)

#endif


#endif //VIPERS_HPP
