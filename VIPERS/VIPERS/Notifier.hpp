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
 * \file VIPERS/Notifier.hpp
 * \brief Notifier class header.
 * \author Frederic Jean
 * $Revision: 252 $
 * $Date: 2009-10-14 21:52:34 -0400 (Wed, 14 Oct 2009) $
 */

#ifndef VIPERS_NOTIFIER_HPP
#define VIPERS_NOTIFIER_HPP

#include "PACC/Threading/Condition.hpp"

namespace VIPERS
{

  using namespace PACC;
  using namespace std;

  /*! \brief %Notifier class.
    \author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada

    \todo
  */
  class Notifier: protected Threading::Condition
  {
    public:

      //! Default constructor
      Notifier();
      //! Destructor
      virtual ~Notifier();

      //! Send a notification
      virtual void notify();
      //! Wait for a notification
      virtual void waitNotification();

    protected:

      bool mNotification;

  };

}

#endif //VIPERS_NOTIFIER_HPP
