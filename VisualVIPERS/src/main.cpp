/*
 *  Visual Video and Image Processing Environment for Real-time Systems (Visual VIPERS)
 *  Copyright (C) 2009 by Frederic Jean
 *
 *  Visual VIPERS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, either version 3 of the License,
 *  or (at your option) any later version.
 *
 *  Visual VIPERS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Visual VIPERS.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Contact:
 *  Computer Vision and Systems Laboratory
 *  Department of Electrical and Computer Engineering
 *  Universite Laval, Quebec, Canada, G1V 0A6
 *  http://vision.gel.ulaval.ca
 *
 */

/*!
 * \file main.cpp
 * \brief Main function that create the GUI using QT4.
 * \author Frederic Jean
 * $Revision: 313 $
 * $Date: 2009-11-10 22:10:44 -0500 (Tue, 10 Nov 2009) $
 */

#include <QApplication>

#include "MainWindow.hpp"

int main(int argc, char *argv[])
{
  QApplication lApp(argc, argv);
  MainWindow lMainWindow;

  lMainWindow.setMinimumSize(1000, 600);
  lMainWindow.show();

  return lApp.exec();
}
