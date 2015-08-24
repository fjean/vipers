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

#ifndef VIPERSGUI_ABOUTDIALOG_HPP
#define VIPERSGUI_ABOUTDIALOG_HPP

#include <QDialog>

class QWidget;

class AboutDialog: public QDialog
{
  Q_OBJECT

  public:

    AboutDialog(QWidget* inParent=0);
    ~AboutDialog();

  protected slots:

    void clickedCloseButton(bool inChecked);

  private:

    void createLayout();
    QWidget* createAboutTab();
    QWidget* createAuthorsTab();
    QWidget* createLicenseTab();

};

#endif //VIPERSGUI_ABOUTDIALOG_HPP
