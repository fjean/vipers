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
 * \file main.cpp
 * \brief Main function for the CLI.
 * \author Frederic Jean
 * $Revision: 268 $
 * $Date: 2009-10-21 22:28:19 -0400 (Wed, 21 Oct 2009) $
 */

#include <iostream>
#include <string>
#include <cstdlib>

#include <VIPERSConfig.hpp>
#include <SequentialKernel.hpp>
#include <Converter.hpp>
#include <XMLStreamer.hpp>
#include <KernelStateNotifier.hpp>

#include <vector>
#include <cv.h>
#include <highgui.h>

#include "CLIConfig.hpp"
#include "OpenCVWindow.hpp"

using namespace std;
using namespace VIPERS;

typedef vector<OpenCVWindow*> OpenCVWindowList;

int main(int argc, char *argv[])
{
	KernelState lState;
	KernelStateNotifier lKernelStateNotifier;
	int lKey;
	XMLStreamer lXML;
	OpenCVWindowList lOpenCVWindowList;
	OpenCVWindowList::iterator lOpenCVWindowItr;
	OpenCVWindow* lOpenCVWindow;
	PropertyList lPropertyList;
	PropertyList::iterator lPropertyItr;
	Module* lTmpModule;
	double lTmpMaxValue;
	int lTmpSteps;
	string lParamName;
	string lFile;

	cout << "VIPERS Command Line Interface version " << VIPERSCLI_VERSION << endl;
	cout << "Using VIPERS library version " << VIPERS::getVersion() << endl;
	cout << "Copyright 2009 Frederic Jean <fjean@gel.ulaval.ca>" << endl << endl;

	if(argc!=2)
	{
		cerr << "ERROR: A VIPERS XML file must be provided" << endl;
		return EXIT_FAILURE;
	}

	lFile = string(argv[1]);

	Kernel* lKernel = new SequentialKernel();

	try
	{
		lKernel->loadModulePathList();
		lKernel->setKernelStateNotifier(&lKernelStateNotifier);

		lXML.readStream(lFile, *lKernel);

		// Create monitors;
		MonitorInfoList lMonitorInfoList = lXML.getMonitorInfoList();
		MonitorInfoList::iterator lMonitorInfoItr = lMonitorInfoList.begin();
		for(lMonitorInfoItr; lMonitorInfoItr!=lMonitorInfoList.end();lMonitorInfoItr++)
		{
			lOpenCVWindow = new OpenCVWindow();
			lTmpModule = lKernel->getModule(lMonitorInfoItr->getModuleLabel().c_str());
			lOpenCVWindow->attach(lTmpModule);
			lOpenCVWindow->setModuleSlot(lMonitorInfoItr->getModuleSlotType(), lMonitorInfoItr->getModuleSlotName());

			Property lOpenCVWindowAutosizeProp = lMonitorInfoItr->getProperty("opencv-window-autosize");
			if(lOpenCVWindowAutosizeProp)
				lOpenCVWindow->create(lOpenCVWindowAutosizeProp.toBool());
			else
				lOpenCVWindow->create();

			Property lOpenCVTrackbarProp = lMonitorInfoItr->getProperty("opencv-trackbars");
			if(lOpenCVTrackbarProp)
			{
				lPropertyList.clear();
				lPropertyList = lOpenCVTrackbarProp.getPropertyList();
				lPropertyItr = lPropertyList.begin();

				for(lPropertyItr; lPropertyItr!=lPropertyList.end(); lPropertyItr++)
				{
					lParamName = lPropertyItr->getName();
					Property lModuleName = lPropertyItr->getProperty("module-name");
					Property lParamMaxValue = lPropertyItr->getProperty("parameter-max-value");
					Property lParamSteps = lPropertyItr->getProperty("parameter-steps");

					if(lModuleName)
					{
						if(lParamMaxValue)
							lTmpMaxValue = lParamMaxValue.toDouble();
						else
							lTmpMaxValue = 0;
						if(lParamSteps)
							lTmpSteps = lParamSteps.toInt();
						else
							lTmpSteps = 0;

						lTmpModule = lKernel->getModule(lModuleName.toString().c_str());
						lOpenCVWindow->addTrackbar(lTmpModule, lParamName.c_str(), lTmpMaxValue, lTmpSteps);
					}
				}
			}

			lOpenCVWindowList.push_back(lOpenCVWindow);
		}

		cout << "Modules layout \"" << lXML.getName() << "\" successfully loaded" << endl << endl;

    // Init modules, and wait for initialization to complete
		lKernel->init();
		lState = lKernelStateNotifier.waitNotification();

		if(lState.isExceptionRaised())
		  throw(lState.getException());
		if(lState!=KernelState::eStateInitialized)
		  throw(VIPERS::Exception(VIPERS::Exception::eCodeUndefined, "Modules have not been initialized for an unknown reason"));

		// Update windows
		for(lOpenCVWindowItr = lOpenCVWindowList.begin(); lOpenCVWindowItr != lOpenCVWindowList.end(); lOpenCVWindowItr++)
			(*lOpenCVWindowItr)->update();

		cout << "Press a key to star processing; Press ESC to stop" << endl << endl;

		lKey = cvWaitKey(0);

		if(lKey!=27)
		{
			lKernel->start();

			while(true)
			{
			  lState = lKernelStateNotifier.waitNotification();

		    if(lState.isExceptionRaised())
		      throw(lState.getException());

		    if(lState==KernelState::eStateStarted)
		    {
		      cout << "\r" << flush << "Processing frame " << lState.getFrame();

		      for(lOpenCVWindowItr = lOpenCVWindowList.begin(); lOpenCVWindowItr != lOpenCVWindowList.end(); lOpenCVWindowItr++)
		        (*lOpenCVWindowItr)->update();
		    }
		    else if(lState==KernelState::eStateStopped)
		    {
		      break;
		    }

				lKey = cvWaitKey(5);
				if(lKey==27)
					lKernel->stop();
			}
		}

		cout << endl;

		for(lOpenCVWindowItr = lOpenCVWindowList.begin(); lOpenCVWindowItr != lOpenCVWindowList.end(); lOpenCVWindowItr++)
			delete (*lOpenCVWindowItr);

		lKernel->clear();
		delete lKernel;

	}
	catch(VIPERS::Exception inException)
	{
		cerr << inException << endl;
		return EXIT_FAILURE;
	}
	catch(std::runtime_error inError)
	{
		cerr << "RUNTIME EXCEPTION: " << inError.what() << endl;
		return EXIT_FAILURE;
	}
	catch(...)
	{
		cerr << "UNKNOWN EXCEPTION has occurred" <<endl;
	}

	return EXIT_SUCCESS;
}
