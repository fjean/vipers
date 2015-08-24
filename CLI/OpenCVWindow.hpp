/*
 *  Video and Image Processing Environment for Real-time Systems CLI (VIPERSCLI)
 *  Copyright (C) 2009 by Frederic Jean
 *
 *  VIPERSCLI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, either version 3 of the License,
 *  or (at your option) any later version.
 *
 *  VIPERSCLI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with VIPERSCLI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Contact:
 *  Computer Vision and Systems Laboratory
 *  Departement of Electrical and Computer Engineering
 *  Universite Laval, Quebec, Canada, G1V 0A6
 *  http://vision.gel.ulaval.ca
 *
 */

 /*!
 * \file OpenCVWindow.hpp
 * \brief OpenCVWindow class header.
 * \author Frederic Jean
 * $Revision: 261 $
 * $Date: 2009-10-18 16:02:27 -0400 (Sun, 18 Oct 2009) $
 */

#ifndef VIPERSCLI_OPENCV_WINDOW_HPP
#define VIPERSCLI_OPENCV_WINDOW_HPP

#include <ModuleSlot.hpp>
#include <Module.hpp>
#include <Monitor.hpp>
#include <Image.hpp>

#include <cv.h>
#include <string>
#include <vector>

namespace VIPERS
{

	using namespace std;

	/*! \brief %OpenCVMonitor class.
		\author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada

		\todo
	*/
	class OpenCVWindow : public Monitor
	{
		public:

		//! Class for holding information avout trackbar module parameters
		class TrackbarModuleParameter
		{
			public:

			TrackbarModuleParameter(Module* inModule, const string& inParamName, int inCurrentVal = 0, int inNbSteps=0, double inMaxValue=0.0);

			Module* mModule;
			string mParamName;
			int mParamLastValue;
			int mNbSteps;
			double mMaxValue;
		};

		//! List of module and parameter name
		typedef vector<TrackbarModuleParameter> TrackbarModuleParameterList;

		//! Default explicit constructor
		OpenCVWindow();
		//! Virtual destructor
		virtual ~OpenCVWindow();

		//! Create the window
		void create(bool inAutoSize=true);

		//! Update window
		void update();

		//! Get window name
		string getWindowName() const throw();

		//! Add trackbar to window for a module parameter
		void addTrackbar(Module* inModule, const string& inParamName, double inMaxValue = 100, int inSteps = 0);

		private:

		//! This function does not perform anything
		static void setTrackbarPosition(int inPos);

		string mModuleSlotFullName ; //!< Full name of the curent module slot
		string mWindowName; //!< Name of the open cv window

		IplImage* mImage; //!< Copy of the slot image

		TrackbarModuleParameterList mTrackbarModuleParameterList; //!< List of module parameter trackbar
		int mTrackBarPosition; //!< Trackbar position variable (unused)
		bool mAutosize;
	};

}

#endif // VIPERSCLI_OPENCV_WINDOW_HPP
