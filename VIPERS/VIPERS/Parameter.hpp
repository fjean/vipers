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
 * \file VIPERS/Parameter.hpp
 * \brief Parameter class header.
 * \author Frederic Jean
 * $Revision: 287 $
 * $Date: 2009-10-30 17:36:18 -0400 (Fri, 30 Oct 2009) $
 */

#ifndef VIPERS_PARAMETER_HPP
#define VIPERS_PARAMETER_HPP

#include "Variable.hpp"
#include <set>

namespace VIPERS
{

	using namespace std;

  /*! \brief %Value class.
    \author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada

    \todo
  */
	class Value
	{
	  public:

	    //! Default constructor
	    Value();
      //! Constructor with value initialization
      Value(const string& inValue);
	    //! Constructor members initialization
	    Value(const string& inValue, const string& inDisplayName, const string& inDescription);
	    //! Copy constructor
	    Value(const Value& inValue);
	    //! Destructor
	    ~Value();

	    //! Assignment operator
	    Value& operator=(const Value& inValue);

	    //! Comparison operator ==
	    bool operator==(const Value& inValue) const;
      //! Comparison operator == with string
      bool operator==(const string& inValue) const;
      //! Comparison operator <
      bool operator<(const Value& inValue) const;
      //! Comparison operator < with string
      bool operator<(const string& inValue) const;

      //! Set value
      Value& operator=(const string& inValue);
      //! Get value
      operator string() const;

	    //! Get value
	    string getValue() const;
      //! Set value
      void setValue(const string& inValue);
      //! Get display name for the value
      string getDisplayName() const;
      //! Set display name for the value
      void setDisplayName(const string& inDisplayName);
      //! Get description name for the value
      string getDescription() const;
      //! Set description name for the value
      void setDescription(const string& inVDescription);

	  private:

	    string mValue; //!< The value
	    string mDisplayName; //!< The value display name
	    string mDescription; //!< The value description

	};

	//! Typedef for a set of value
	typedef set<Value> ValueSet;
  //! Typedef for a set of name
  typedef set<string> NameSet;

	/*! \brief %Parameter class.
		\author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada

		\todo
	*/
	class Parameter : public Variable
	{
		public:

		//! Default explicit constructor
		explicit Parameter(const string& inName, VariableType inVarType, const string& inDisplayName, const string& inDescription, bool inRunTimeChange=false) throw();
    //! Copy constructor
    Parameter(const Parameter& inParameter);
    //! Assignment operator
		Parameter& operator=(const Parameter& inParameter);
		//! Destructor
		virtual ~Parameter();

		//! Get runtime change
		bool getRunTimeChange() const throw();

		//! Get enabled
		bool getEnabled() const throw();
		//! Get possible values
		ValueSet getPossibleValues() const throw();
		//! Get the number of possible value
		unsigned int getPossibleValuesCount() const throw();
		//! Get minimum value
		string getMinValue() const throw();
		//! Get maximum value
		string getMaxValue() const throw();
		//! Get step value
		string getStepValue() const throw();
    //! Get dependent parameter name set
		NameSet getDependentParameterNameSet() const throw();

		//! Set enabled
		void setEnabled(bool inEnabled) throw();
		//! Set possible values
		void setPossibleValues(const ValueSet& inValues) throw();
		//! Set minimum value
		void setMinValue(const string& inValue) throw();
		//! Set maximum value
		void setMaxValue(const string& inValue) throw();
		//! Set step value
		void setStepValue(const string& inValue) throw();
    //! Set dependent parameter name set
    void setDependentParameterNameSet(const NameSet& inNameSet) throw();

		protected:

		bool mEnabled; //!< Is the parameter enabled
		string mMinValue; //!< Minimum value
		string mMaxValue; //!< Maximum value
		string mStepValue; //!< Value step

    ValueSet mPossibleValues; //!< List of possible values
		NameSet mDependentParameterNameSet; //!< List of dependent parameter name

		private:

		bool mMandatory; //!< Is the parameter mandatory
		bool mRunTimeChange; //!< Can the parameter be changed in runtime

	};

}

#endif //VIPERS_PARAMETER_HPP
