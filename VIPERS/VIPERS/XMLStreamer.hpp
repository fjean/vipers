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
 * \file VIPERS/XMLStreamer.hpp
 * \brief XMLStreamer class header.
 * \author Frederic Jean
 * $Revision: 252 $
 * $Date: 2009-10-14 21:52:34 -0400 (Wed, 14 Oct 2009) $
 */

#ifndef VIPERS_XMLSTREAMER_HPP
#define VIPERS_XMLSTREAMER_HPP

#include "Kernel.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <PACC/XML/Document.hpp>
#include <PACC/XML/Finder.hpp>

namespace VIPERS
{

	using namespace std;
	using namespace PACC;

	/*! \brief %MonitorInfo class.
		\author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada

		\todo
	*/
	class MonitorInfo
	{
		public:

		//! Default constructor
		MonitorInfo() throw();
		//! Destructor
		~MonitorInfo();

		//! Get the label of the monitored module
		string getModuleLabel() const throw();
		//! Get the name of the monitored module slot
		string getModuleSlotName() const throw();
		//! Get the type of the monitored module slot
		ModuleSlot::SlotType getModuleSlotType() const throw();

		//! Set the label of the monitored module
		void setModuleLabel(const string& inLabel) throw();
		//! Set the name of the monitored module slot
		void setModuleSlotName(const string& inSlotName) throw();
		//! Set the type of the monitored module slot
		void setModuleSlotType(ModuleSlot::SlotType inSlotType) throw();

		//! Get the monitor property list
		PropertyList getPropertyList() const throw();
		//! Get the monitor property list
		void setPropertyList(const PropertyList& inPropertyList) throw();
		//! Get a property by its name
		Property getProperty(const string& inName) const throw();

		private:

		string mModuleLabel; //!< Label of the module monitored
		string mModuleSlotName; //!< Name of the module slot monitored
		ModuleSlot::SlotType mModuleSlotType; //!< Type of the module slot monitored

		PropertyList mPropertyList; //!< Property list of the monitor

	};

	//! List of MonitorInfo
	typedef vector<MonitorInfo> MonitorInfoList;

	/*! \brief %XMLStreamer class.
		\author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada

		\todo
	*/
	class XMLStreamer
	{
		public:

		//! Default explicit constructor
		XMLStreamer() throw();

		//! Destructor
		virtual ~XMLStreamer();

		//! Clear the current content of the stream
		void clear() throw();

		//! Read XML from file
		void readStream(const string& inFile, Kernel& ioKernel);
		//! Write XML to stream
		void writeStream(const string& inFile, const Kernel& inKernel);

		//! Read XML from stream
		void readStream(istream& inStream, Kernel& ioKernel);
		//! Write XML to stream
		void writeStream(ostream& inStream, const Kernel& inKernel);

		//! Get stream name
		string getName() const throw();
		//! Get stream name
		string getDescription() const throw();
		//! Set stream name
		void setName(const string& inName) throw();
		//! Set stream name
		void setDescription(const string& inDescription) throw();

		//! Get stream version
		string getStreamVersion() const throw();
		//! Get name of the library that generated the stream
		string getLibraryName() const throw();
		//! Get version of the library that generated the stream
		string getLibraryVersion() const throw();


		//! Get list of minotor info
		MonitorInfoList getMonitorInfoList() const throw();

		private:

		//! Read XML properties
		PropertyList readProperties(XML::Finder& inFinder, XML::Iterator& inProperty);
		//! Write XML properties
		void writeProperties(XML::Document& inXMLDoc, XML::Iterator& inIterator, const PropertyList& inPropertyList);

		string mName; //!< Stream name
		string mDescription; //!< Description of the stream

		string mStreamVersion; //!< File version
		string mLibraryName; //!< Name of the library that generated the stream
		string mLibraryVersion; //!< Version of the library that generated the stream

		MonitorInfoList mMonitorInfoList; //!< List of monitor info

	};

}

#endif //VIPERS_XMLSTREAMER_HPP
