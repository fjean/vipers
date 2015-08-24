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
 * \file VIPERS/XMLStreamer.cpp
 * \brief XMLStreamer class functions definition.
 * \author Frederic Jean
 * $Revision: 252 $
 * $Date: 2009-10-14 21:52:34 -0400 (Wed, 14 Oct 2009) $
 */

#include "XMLStreamer.hpp"
#include "Exception.hpp"
#include "VIPERSConfig.hpp"
#include "VIPERS.hpp"
#include <fstream>

using namespace VIPERS;
using namespace std;

#define XML_ROOT_NAME                 "VIPERS"
#define XML_ROOT_ATTR_FILEVERSION     "fileversion"
#define XML_ROOT_ATTR_LIBVERSION      "libraryversion"
#define XML_ROOT_ATTR_LIBNAME         "library"

#define XML_NAME                      "Name"
#define XML_DESCRIPTION               "Description"

#define XML_MODULES                   "Modules"
#define XML_MODULE                    "Module"
#define XML_MODULE_ATTR_LABEL         "label"
#define XML_MODULE_ATTR_NAME          "name"
#define XML_MODULE_ATTR_VERSION       "version"

#define XML_INPUTSLOTS                "InputSlots"
#define XML_OUTPUTSLOTS               "OutputSlots"
#define XML_SLOT                      "Slot"
#define XML_SLOT_ATTR_NAME            "name"
#define XML_SLOT_CONNECT              "Connection"
#define XML_SLOT_CONNECT_ATTR_MODULE  "modulelabel"
#define XML_SLOT_CONNECT_ATTR_SLOT    "slotname"

#define XML_PROPERTIES                "Properties"
#define XML_PROPERTY                  "Property"
#define XML_PROPERTY_ATTR_NAME        "name"
#define XML_PROPERTY_ATTR_DISPLAYNAME "displayname"
#define XML_PROPERTY_ATTR_TYPE        "type"
#define XML_PROPERTY_DESCRIPTION      "Description"
#define XML_PROPERTY_VALUE            "Value"

#define XML_PARAMETERS                "Parameters"
#define XML_PARAMETER                 "Parameter"
#define XML_PARAMETER_ATTR_NAME       "name"

#define XML_MONITORS                  "Monitors"
#define XML_MONITOR                   "Monitor"
#define XML_MONITOR_ATTR_MODULE_LABEL "modulelabel"
#define XML_MONITOR_ATTR_SLOT_NAME    "slotname"
#define XML_MONITOR_ATTR_SLOT_TYPE    "slottype"

#define XPATH_NAME                    "Name"
#define XPATH_DESCRIPTION             "Description"
#define XPATH_MODULES                 "Modules/Module"
#define XPATH_MODULE_INPUTSLOT        "InputSlots/Slot"
#define XPATH_MODULE_OUTPUTSLOT       "OutputSlots/Slot"
#define XPATH_MODULE_SLOTCONNECTION   "Connection"
#define XPATH_PROPERTIES              "Properties/Property"
#define XPATH_PROPERTIES_VALUE        "Value"
#define XPATH_PROPERTIES_DESCRIPTION  "Description"
#define XPATH_PARAMETERS              "Parameters/Parameter"
#define XPATH_MONITORS                "Monitors/Monitor"

/*! \todo
*/
MonitorInfo::MonitorInfo() throw()
{

}

/*! \todo
*/
MonitorInfo::~MonitorInfo()
{

}

/*! \todo
*/
string MonitorInfo::getModuleLabel() const throw()
{
	return mModuleLabel.c_str();
}

/*! \todo
*/
string MonitorInfo::getModuleSlotName() const throw()
{
	return mModuleSlotName.c_str();
}

/*! \todo
*/
ModuleSlot::SlotType MonitorInfo::getModuleSlotType() const throw()
{
	return mModuleSlotType;
}

/*! \todo
*/
void MonitorInfo::setModuleLabel(const string& inLabel) throw()
{
	mModuleLabel = inLabel.c_str();
}

/*! \todo
*/
void MonitorInfo::setModuleSlotName(const string& inSlotName) throw()
{
	mModuleSlotName = inSlotName.c_str();
}

/*! \todo
*/
void MonitorInfo::setModuleSlotType(ModuleSlot::SlotType inSlotType) throw()
{
	mModuleSlotType = inSlotType;
}

/*! \todo
*/
PropertyList MonitorInfo::getPropertyList() const throw()
{
	return mPropertyList;
}

/*! \todo
*/
void MonitorInfo::setPropertyList(const PropertyList& inPropertyList) throw()
{
	mPropertyList.clear();
	mPropertyList = inPropertyList;
}

/*! \todo
*/
Property MonitorInfo::getProperty(const string& inName) const throw()
{
	PropertyList::const_iterator lItr = mPropertyList.begin();
	for(lItr; lItr!=mPropertyList.end(); lItr++)
	{
		if(lItr->getName()==inName)
			return *lItr;
	}
	return Property("", Variable::eVariableTypeString, "", "");
}

/*! \todo
*/
XMLStreamer::XMLStreamer() throw()
{

}

/*! \todo
*/
XMLStreamer::~XMLStreamer()
{

}

/*! \todo
*/
void XMLStreamer::clear() throw()
{
	mName = "";
	mDescription = "";
	mStreamVersion = "";
	mLibraryName = "";
	mLibraryVersion = "";
	mMonitorInfoList.clear();
}

/*! \todo
*/
void XMLStreamer::readStream(const string& inFile, Kernel& ioKernel)
{
	ifstream lFileStream(inFile.c_str(), ios_base::in);

	if(!lFileStream)
		throw(Exception(Exception::eCodeIOXML, ""));

	try
	{
		readStream(lFileStream, ioKernel);
	}
	catch(...)
	{
		lFileStream.close();
		throw;
	}
	lFileStream.close();
}

/*! \todo
*/
void XMLStreamer::writeStream(const string& inFile, const Kernel& inKernel)
{
	ofstream lFileStream(inFile.c_str(), ios_base::out);

	if(!lFileStream)
		throw(Exception(Exception::eCodeIOXML, ""));

	try
	{
		writeStream(lFileStream, inKernel);
	}
	catch(...)
	{
		lFileStream.close();
		throw;
	}
	lFileStream.close();
}

/*! \todo
*/
void XMLStreamer::readStream(istream& inStream, Kernel& ioKernel)
{
	XML::Document lXMLDoc;
	XML::Iterator lRoot;
	XML::Iterator lName;
	XML::Iterator lDescription;
	XML::Iterator lModule;
	XML::Iterator lSlot;
	XML::Iterator lConnection;
	XML::Iterator lParameter;
	XML::Iterator lMonitor;
	XML::Iterator lProperty;

	Module* lTmpModule;
	Module* lTmpModuleOther;

	string lModuleLabel;
	string lSlotName;
	string lSlotNameOther;
	MonitorInfo lMonitorInfo;

	clear();

	try
	{
		ioKernel.clearModules();
		lXMLDoc.parse(inStream);
	}
	catch(runtime_error inError)
	{
		throw(Exception(Exception::eCodeIOXML, string("An error occured while parsing XML stream: ") + inError.what()));
	}

	lRoot = lXMLDoc.getFirstDataTag();

	XML::Finder lModuleFinder(lRoot);

	if(!lRoot || lRoot->getValue()!=XML_ROOT_NAME )
		throw(Exception(Exception::eCodeIOXML, "The stream is not a VIPERS XML stream"));

	mStreamVersion = lRoot->getAttribute(XML_ROOT_ATTR_FILEVERSION);
	mLibraryName = lRoot->getAttribute(XML_ROOT_ATTR_LIBNAME);
	mLibraryVersion = lRoot->getAttribute(XML_MODULE_ATTR_VERSION);

	lName = lModuleFinder.find(XPATH_NAME);
	if(lName && lName->getFirstChild() && lName->getFirstChild()->getType()==XML::eString)
		mName = lName->getFirstChild()->getValue();

	lDescription = lModuleFinder.find(XPATH_DESCRIPTION);
	if(lDescription && lDescription->getFirstChild() && lDescription->getFirstChild()->getType()==XML::eString)
		mDescription = lDescription->getFirstChild()->getValue();

	lModule = lModuleFinder.find(XPATH_MODULES);

	try
	{
		while(lModule)
		{
			lTmpModule = ioKernel.newModule(lModule->getAttribute(XML_MODULE_ATTR_NAME));
			lTmpModule->setLabel(lModule->getAttribute(XML_MODULE_ATTR_LABEL));

			//cout << "Module " << lTmpModule->getLabel() << endl;

			XML::Finder lParametersFinder(lModule);
			lParameter = lParametersFinder.find(XPATH_PARAMETERS);
			while(lParameter)
			{
				if(lParameter->getFirstChild() && lParameter->getFirstChild()->getType()==XML::eString)
				{
					lTmpModule->setParameterValue(lParameter->getAttribute(XML_PARAMETER_ATTR_NAME), lParameter->getFirstChild()->getValue());
				}
				lParameter = lParametersFinder.findNext();
			}

			XML::Finder lPropertiesFinder(lModule);
			lProperty = lPropertiesFinder.find(XPATH_PROPERTIES);
			lTmpModule->setPropertyList(readProperties(lPropertiesFinder, lProperty));

			lModule = lModuleFinder.findNext();
		}

		lModule = lModuleFinder.find(XPATH_MODULES);
		while(lModule)
		{
			lTmpModule = ioKernel.getModule(lModule->getAttribute(XML_MODULE_ATTR_LABEL));
			VIPERS_ASSERT(lTmpModule, "Cannot find module that as just been created");

			//cout << "Module " << lTmpModule->getLabel() << endl;

			XML::Finder lSlotFinder(lModule);
			lSlot = lSlotFinder.find(XPATH_MODULE_OUTPUTSLOT);
			while(lSlot)
			{
				lSlotName = lSlot->getAttribute(XML_SLOT_ATTR_NAME);

				XML::Finder lConnectionFinder(lSlot);
				lConnection = lConnectionFinder.find(XPATH_MODULE_SLOTCONNECTION);
				while(lConnection)
				{
					lSlotNameOther = lConnection->getAttribute(XML_SLOT_CONNECT_ATTR_SLOT);

					lTmpModuleOther = ioKernel.getModule(lConnection->getAttribute(XML_SLOT_CONNECT_ATTR_MODULE));
					VIPERS_ASSERT(lTmpModuleOther, "Cannot find module that as just been created");

					lTmpModuleOther->connect(ModuleSlot::eSlotTypeInput, lSlotNameOther.c_str(), lTmpModule, lSlotName.c_str());

					//cout << "    Connect slot " << lSlotName << " with slot " << lSlotNameOther << " of module " << lTmpModuleOther->getLabel() << endl;

					lConnection = lConnectionFinder.findNext();
				}

				lSlot = lSlotFinder.findNext();
			}

			lModule = lModuleFinder.findNext();
		}

		XML::Finder lMonitorFinder(lRoot);
		lMonitor = lMonitorFinder.find(XPATH_MONITORS);
		while(lMonitor)
		{
			lMonitorInfo.setModuleLabel(lMonitor->getAttribute(XML_MONITOR_ATTR_MODULE_LABEL));
			lMonitorInfo.setModuleSlotName(lMonitor->getAttribute(XML_MONITOR_ATTR_SLOT_NAME));

			if(lMonitor->getAttribute(XML_MONITOR_ATTR_MODULE_LABEL)=="input")
				lMonitorInfo.setModuleSlotType(ModuleSlot::eSlotTypeInput);
			else
				lMonitorInfo.setModuleSlotType(ModuleSlot::eSlotTypeOutput);

			XML::Finder lPropertiesFinder(lMonitor);
			lProperty = lPropertiesFinder.find(XPATH_PROPERTIES);
			lMonitorInfo.setPropertyList(readProperties(lPropertiesFinder, lProperty));

			mMonitorInfoList.push_back(lMonitorInfo);

			lMonitor = lMonitorFinder.findNext();
		}
	}
	catch(Exception inException)
	{
		throw(inException);
	}
	catch(runtime_error inError)
	{
		throw(Exception(Exception::eCodeIOXML, string("An error occured while parsing XML stream: ") + inError.what()));
	}

}

/*! \todo
*/
void XMLStreamer::writeStream(ostream& inStream, const Kernel& inKernel)
{
	ModuleSlotMap::const_iterator lSlotItr;
	ModuleSlotSet lConnectedSlots;
	ModuleSlotSet::const_iterator lConnectedSlotItr;
	ParameterList lParameterList;
	ParameterList::iterator lParameterItr;
	MonitorSet lMonitorSet;
	MonitorSet::const_iterator lMonitorItr;
	const Module* lTmpModule;
	const ModuleSlot* lTmpModuleSlot;

	XML::Document lXMLDoc;
	XML::Iterator lRoot;
	XML::Iterator lModules;
	XML::Iterator lModule;
	XML::Iterator lSlots;
	XML::Iterator lSlot;
	XML::Iterator lConnection;
	XML::Iterator lParameters;
	XML::Iterator lParameter;
	XML::Iterator lMonitors;
	XML::Iterator lMonitor;
	XML::Iterator lProperties;

	lRoot = lXMLDoc.addRoot(XML_ROOT_NAME);

	mStreamVersion = "1.0";
	mLibraryName = "libvipers";
	mLibraryVersion = getVersion().c_str();

	lRoot->setAttribute(XML_ROOT_ATTR_FILEVERSION, mStreamVersion);
	lRoot->setAttribute(XML_ROOT_ATTR_LIBNAME, mLibraryName);
	lRoot->setAttribute(XML_ROOT_ATTR_LIBVERSION, mLibraryVersion);

	lXMLDoc.addChild(lXMLDoc.addChild(lRoot, XML_NAME), mName, XML::eString);
	lXMLDoc.addChild(lXMLDoc.addChild(lRoot, XML_DESCRIPTION), mDescription, XML::eString);


	// ***** MODULES *****

	lModules = lXMLDoc.addChild(lRoot, XML_MODULES);

	ModuleSet lModuleSet = inKernel.getModules();
	ModuleSet::const_iterator lModuleItr = lModuleSet.begin();
	for(lModuleItr; lModuleItr!=lModuleSet.end(); lModuleItr++)
	{
		lModule = lXMLDoc.addChild(lModules, XML_MODULE);
		lModule->setAttribute(XML_MODULE_ATTR_NAME, (*lModuleItr)->getName().c_str());
		lModule->setAttribute(XML_MODULE_ATTR_LABEL, (*lModuleItr)->getLabel().c_str());
		lModule->setAttribute(XML_MODULE_ATTR_VERSION, (*lModuleItr)->getVersion().c_str());

		lSlots = lXMLDoc.addChild(lModule, XML_INPUTSLOTS);

		const ModuleSlotMap& lInputSlotsMap = (*lModuleItr)->getInputSlots();
		lSlotItr = lInputSlotsMap.begin();
		for(lSlotItr; lSlotItr!=lInputSlotsMap.end();lSlotItr++)
		{
			lSlot = lXMLDoc.addChild(lSlots, XML_SLOT);
			lSlot->setAttribute(XML_SLOT_ATTR_NAME, lSlotItr->first.c_str());

			lConnectedSlots.clear();
			lConnectedSlots = lSlotItr->second->getConnectedSlots();
			lConnectedSlotItr = lConnectedSlots.begin();
			for(lConnectedSlotItr; lConnectedSlotItr!=lConnectedSlots.end(); lConnectedSlotItr++)
			{
				lConnection = lXMLDoc.addChild(lSlot, XML_SLOT_CONNECT);
				lConnection->setAttribute(XML_SLOT_CONNECT_ATTR_MODULE, (*lConnectedSlotItr)->getModule()->getLabel().c_str());
				lConnection->setAttribute(XML_SLOT_CONNECT_ATTR_SLOT, (*lConnectedSlotItr)->getName().c_str());
			}
		}

		lSlots = lXMLDoc.addChild(lModule, XML_OUTPUTSLOTS);

		const ModuleSlotMap& lOutputSlotsMap = (*lModuleItr)->getOutputSlots();
		lSlotItr = lOutputSlotsMap.begin();
		for(lSlotItr; lSlotItr!=lOutputSlotsMap.end();lSlotItr++)
		{
			lSlot = lXMLDoc.addChild(lSlots, XML_SLOT);
			lSlot->setAttribute(XML_SLOT_ATTR_NAME, lSlotItr->first.c_str());

			lConnectedSlots.clear();
			lConnectedSlots = lSlotItr->second->getConnectedSlots();
			lConnectedSlotItr = lConnectedSlots.begin();
			for(lConnectedSlotItr; lConnectedSlotItr!=lConnectedSlots.end(); lConnectedSlotItr++)
			{
				lConnection = lXMLDoc.addChild(lSlot, XML_SLOT_CONNECT);
				lConnection->setAttribute(XML_SLOT_CONNECT_ATTR_MODULE, (*lConnectedSlotItr)->getModule()->getLabel().c_str());
				lConnection->setAttribute(XML_SLOT_CONNECT_ATTR_SLOT, (*lConnectedSlotItr)->getName().c_str());
			}
		}

		lParameters = lXMLDoc.addChild(lModule, XML_PARAMETERS);

		lParameterList.clear();
		lParameterList = (*lModuleItr)->getParameterList();
		lParameterItr = lParameterList.begin();
		for(lParameterItr; lParameterItr!=lParameterList.end(); lParameterItr++)
		{
			lParameter = lXMLDoc.addChild(lParameters, XML_PARAMETER);
			lParameter->setAttribute(XML_PARAMETER_ATTR_NAME, lParameterItr->getName().c_str());
			lXMLDoc.addChild(lParameter, lParameterItr->toString().c_str(), XML::eString);
		}


		lProperties = lXMLDoc.addChild(lModule, XML_PROPERTIES);
		writeProperties(lXMLDoc, lProperties, (*lModuleItr)->getPropertyList());

	}

	// ***** MONITORS *****

	lMonitors = lXMLDoc.addChild(lRoot, XML_MONITORS);

	for(lModuleItr=lModuleSet.begin(); lModuleItr!=lModuleSet.end(); lModuleItr++)
	{
		lMonitorSet.clear();
		lMonitorSet = (*lModuleItr)->getMonitors();
		lMonitorItr = lMonitorSet.begin();

		for(lMonitorItr; lMonitorItr!=lMonitorSet.end();lMonitorItr++)
		{
			lTmpModule = (*lMonitorItr)->getModule();
			lTmpModuleSlot = (*lMonitorItr)->getModuleSlot();

			if(lTmpModule && lTmpModuleSlot)
			{
				lMonitor = lXMLDoc.addChild(lMonitors, XML_MONITOR);
				lMonitor->setAttribute(XML_MONITOR_ATTR_MODULE_LABEL, lTmpModule->getLabel().c_str());
				lMonitor->setAttribute(XML_MONITOR_ATTR_SLOT_NAME, lTmpModuleSlot->getName().c_str());

				if(lTmpModuleSlot->getType()==ModuleSlot::eSlotTypeInput)
					lMonitor->setAttribute(XML_MONITOR_ATTR_SLOT_TYPE, "input");
				else
					lMonitor->setAttribute(XML_MONITOR_ATTR_SLOT_TYPE, "output");

				lProperties = lXMLDoc.addChild(lMonitor, XML_PROPERTIES);
				writeProperties(lXMLDoc, lProperties, (*lMonitorItr)->getPropertyList());
			}
		}
	}

	lXMLDoc.serialize(inStream);
}

/*! \todo
*/
string XMLStreamer::getName() const throw()
{
	return mName.c_str();
}

/*! \todo
*/
string XMLStreamer::getDescription() const throw()
{
	return mDescription.c_str();
}

/*! \todo
*/
void XMLStreamer::setName(const string& inName) throw()
{
	mName = inName.c_str();
}

/*! \todo
*/
void XMLStreamer::setDescription(const string& inDescription) throw()
{
	mDescription = inDescription.c_str();
}

/*! \todo
*/
MonitorInfoList XMLStreamer::getMonitorInfoList() const throw()
{
	return mMonitorInfoList;
}

/*! \todo
*/
string XMLStreamer::getStreamVersion() const throw()
{
	return mStreamVersion.c_str();
}

/*! \todo
*/
string XMLStreamer::getLibraryName() const throw()
{
	return mLibraryName.c_str();
}

/*! \todo
*/
string XMLStreamer::getLibraryVersion() const throw()
{
	return mLibraryVersion.c_str();
}

/*! \todo
*/
PropertyList XMLStreamer::readProperties(XML::Finder& inFinder, XML::Iterator& inProperty)
{
	PropertyList lPropertyList;

	string lName;
	string lDisplayName;
	string lDescriptionStr;
	Variable::VariableType lType;
	string lTypeStr;
	string lValueStr;

	XML::Iterator lDescription;
	XML::Iterator lValue;
	XML::Iterator lProperties;

	while(inProperty)
	{
		XML::Finder lFinder(inProperty);

		lDescription = lFinder.find(XPATH_PROPERTIES_DESCRIPTION);
		if(lDescription && lDescription->getFirstChild() && lDescription->getFirstChild()->getType()==XML::eString)
			lDescriptionStr = lDescription->getFirstChild()->getValue();

		lValue = lFinder.find(XPATH_PROPERTIES_VALUE);
		if(lValue && lValue->getFirstChild() && lValue->getFirstChild()->getType()==XML::eString)
			lValueStr = lValue->getFirstChild()->getValue();

		lName = inProperty->getAttribute(XML_PROPERTY_ATTR_NAME);
		lDisplayName = inProperty->getAttribute(XML_PROPERTY_ATTR_DISPLAYNAME);
		lTypeStr = inProperty->getAttribute(XML_PROPERTY_ATTR_TYPE);

		if(lTypeStr=="bool")
			lType = Variable::eVariableTypeBool;
		else if(lTypeStr=="char")
			lType = Variable::eVariableTypeChar;
		else if(lTypeStr=="color")
			lType = Variable::eVariableTypeColor;
		else if(lTypeStr=="directory")
			lType = Variable::eVariableTypeDirectory;
		else if(lTypeStr=="double")
			lType = Variable::eVariableTypeDouble;
		else if(lTypeStr=="file")
			lType = Variable::eVariableTypeFile;
		else if(lTypeStr=="float")
			lType = Variable::eVariableTypeFloat;
		else if(lTypeStr=="int")
			lType = Variable::eVariableTypeInt;
		else if(lTypeStr=="longdouble")
			lType = Variable::eVariableTypeLDouble;
		else if(lTypeStr=="long")
			lType = Variable::eVariableTypeLong;
		else if(lTypeStr=="short")
			lType = Variable::eVariableTypeShort;
		else if(lTypeStr=="size")
			lType = Variable::eVariableTypeSize;
		else if(lTypeStr=="string")
			lType = Variable::eVariableTypeString;
		else if(lTypeStr=="uchar")
			lType = Variable::eVariableTypeUChar;
		else if(lTypeStr=="uint")
			lType = Variable::eVariableTypeUInt;
		else if(lTypeStr=="ulong")
			lType = Variable::eVariableTypeULong;
		else if(lTypeStr=="ushort")
			lType = Variable::eVariableTypeUShort;
		else
			throw(Exception(Exception::eCodeIOXML, string("Invalid property type ") + lTypeStr.c_str() + string(" in XML stream")));

		lPropertyList.push_back(Property(lName, lType, lDisplayName, lDescriptionStr));
		lPropertyList.back().setValueStr(lValueStr.c_str());

		lProperties = lFinder.find(XPATH_PROPERTIES);
		lPropertyList.back().setPropertyList(readProperties(lFinder, lProperties));

		inProperty = inFinder.findNext();
	}

	return lPropertyList;
}

/*! \todo
*/
void XMLStreamer::writeProperties(XML::Document& inXMLDoc, XML::Iterator& inIterator, const PropertyList& inPropertyList)
{
	PropertyList::const_iterator lPropertyItr = inPropertyList.begin();
	XML::Iterator lProperty;
	XML::Iterator lProperties;

	for(lPropertyItr; lPropertyItr!=inPropertyList.end();lPropertyItr++)
	{
		lProperty = inXMLDoc.addChild(inIterator, XML_PROPERTY);
		lProperty->setAttribute(XML_PROPERTY_ATTR_NAME, lPropertyItr->getName().c_str());
		lProperty->setAttribute(XML_PROPERTY_ATTR_DISPLAYNAME, lPropertyItr->getDisplayName().c_str());

		switch(lPropertyItr->getVariableType())
		{
			case Variable::eVariableTypeBool:
				lProperty->setAttribute(XML_PROPERTY_ATTR_TYPE, "bool");
				break;
			case Variable::eVariableTypeChar:
				lProperty->setAttribute(XML_PROPERTY_ATTR_TYPE, "char");
				break;
			case Variable::eVariableTypeColor:
				lProperty->setAttribute(XML_PROPERTY_ATTR_TYPE, "color");
				break;
			case Variable::eVariableTypeDirectory:
				lProperty->setAttribute(XML_PROPERTY_ATTR_TYPE, "directory");
				break;
			case Variable::eVariableTypeDouble:
				lProperty->setAttribute(XML_PROPERTY_ATTR_TYPE, "double");
				break;
			case Variable::eVariableTypeFile:
				lProperty->setAttribute(XML_PROPERTY_ATTR_TYPE, "file");
				break;
			case Variable::eVariableTypeFloat:
				lProperty->setAttribute(XML_PROPERTY_ATTR_TYPE, "float");
				break;
			case Variable::eVariableTypeInt:
				lProperty->setAttribute(XML_PROPERTY_ATTR_TYPE, "int");
				break;
			case Variable::eVariableTypeLDouble:
				lProperty->setAttribute(XML_PROPERTY_ATTR_TYPE, "longdouble");
				break;
			case Variable::eVariableTypeLong:
				lProperty->setAttribute(XML_PROPERTY_ATTR_TYPE, "long");
				break;
			case Variable::eVariableTypeShort:
				lProperty->setAttribute(XML_PROPERTY_ATTR_TYPE, "short");
				break;
			case Variable::eVariableTypeSize:
				lProperty->setAttribute(XML_PROPERTY_ATTR_TYPE, "size");
				break;
			case Variable::eVariableTypeString:
				lProperty->setAttribute(XML_PROPERTY_ATTR_TYPE, "string");
				break;
			case Variable::eVariableTypeUChar:
				lProperty->setAttribute(XML_PROPERTY_ATTR_TYPE, "uchar");
				break;
			case Variable::eVariableTypeUInt:
				lProperty->setAttribute(XML_PROPERTY_ATTR_TYPE, "uint");
				break;
			case Variable::eVariableTypeULong:
				lProperty->setAttribute(XML_PROPERTY_ATTR_TYPE, "ulong");
				break;
			case Variable::eVariableTypeUShort:
				lProperty->setAttribute(XML_PROPERTY_ATTR_TYPE, "ushort");
				break;
			default:
				VIPERS_ASSERT(1, "Invalid variable type case");
				break;
		}

		inXMLDoc.addChild(inXMLDoc.addChild(lProperty, XML_PROPERTY_DESCRIPTION), lPropertyItr->getDescription().c_str(), XML::eString);
		inXMLDoc.addChild(inXMLDoc.addChild(lProperty, XML_PROPERTY_VALUE), lPropertyItr->toString().c_str(), XML::eString);

		lProperties = inXMLDoc.addChild(lProperty, XML_PROPERTIES);
		writeProperties(inXMLDoc, lProperties, lPropertyItr->getPropertyList());
	}
}

