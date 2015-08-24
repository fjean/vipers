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

#include "AboutDialog.hpp"
#include "VisualVipersConfig.hpp"

#include <VIPERSConfig.hpp>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QFont>
#include <QScrollArea>
#include <QFile>
#include <QTextStream>
#include <QTextEdit>
#include <QPalette>

//-------------------------------------------------------------------------------

AboutDialog::AboutDialog(QWidget* inParent)
  : QDialog(inParent, Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
  createLayout();
  setWindowTitle(tr("About Visual VIPERS"));
  resize(550,350);
}

//-------------------------------------------------------------------------------

AboutDialog::~AboutDialog()
{

}

//-------------------------------------------------------------------------------

void AboutDialog::clickedCloseButton(bool inChecked)
{
  close();
}

//-------------------------------------------------------------------------------

void AboutDialog::createLayout()
{
  QVBoxLayout* lMainVLayout = new QVBoxLayout();
  setLayout(lMainVLayout);

  //Icon, Title and versions
  QHBoxLayout* lIconTitleHLayout = new QHBoxLayout();
  lMainVLayout->addLayout(lIconTitleHLayout, 0);

  QLabel* lIcon = new QLabel();
  lIcon->setPixmap(QPixmap(":/images/vipers"));
  lIconTitleHLayout->addWidget(lIcon, 0, Qt::AlignLeft);

  QVBoxLayout* lTitleVLayout = new QVBoxLayout();
  lIconTitleHLayout->addSpacing(15);
  lIconTitleHLayout->addLayout(lTitleVLayout, 0);
  lIconTitleHLayout->addStretch(1);
  lTitleVLayout->setSpacing(1);

  QLabel* lSoftwareName = new QLabel(tr("Visual VIPERS"));
  QFont lSoftwareNameFont = lSoftwareName->font();
  lSoftwareNameFont.setPointSize(16);
  lSoftwareName->setFont(lSoftwareNameFont);
  lTitleVLayout->addWidget(lSoftwareName, 0, Qt::AlignLeft | Qt::AlignTop);

  QLabel* lSoftwareVersion = new QLabel();
  QFont lSoftwareVersionFont = lSoftwareVersion->font();
  lSoftwareVersionFont.setBold(true);
  lSoftwareVersion->setFont(lSoftwareVersionFont);
  lTitleVLayout->addWidget(lSoftwareVersion, 0, Qt::AlignLeft | Qt::AlignTop);

  QString lTmpSoftVer = QString(tr("Version %1")).arg(VISUALVIPERS_VERSION);
  if(VISUALVIPERS_SVN_REVISION>0)
    lTmpSoftVer = lTmpSoftVer + " (" + QString(tr("revision %1")).arg(VISUALVIPERS_SVN_REVISION) + ")";
  lSoftwareVersion->setText(lTmpSoftVer);

  QLabel* lLibraryVersion = new QLabel();
  lTitleVLayout->addWidget(lLibraryVersion, 0, Qt::AlignLeft | Qt::AlignTop);

  QString lTmpLibVer = QString(tr("Using VIPERS library version %1")).arg(VIPERS::getVersion().c_str());
  if(VIPERS::getRevisionNumber()>0)
    lTmpLibVer = lTmpLibVer + " (" + QString(tr("revision %1")).arg(VIPERS::getRevisionNumber()) + ")";
  lLibraryVersion->setText(lTmpLibVer);

  lTitleVLayout->addStretch(1);

  //Tabs
  QTabWidget* lTabs = new QTabWidget();
  lMainVLayout->addSpacing(15);
  lMainVLayout->addWidget(lTabs, 1);

  lTabs->addTab(createAboutTab(), tr("About"));
  lTabs->addTab(createAuthorsTab(), tr("Authors"));
  lTabs->addTab(createLicenseTab(), tr("License"));

  //Close button
  QPushButton* lCloseButton = new QPushButton(QIcon(":/images/button-cancel"), tr("Close"));
  lMainVLayout->addWidget(lCloseButton, 0, Qt::AlignRight);
  connect(lCloseButton, SIGNAL(clicked(bool)), this, SLOT(clickedCloseButton(bool)));

}

//-------------------------------------------------------------------------------

QWidget* AboutDialog::createAboutTab()
{
  QWidget* lAboutWidget = new QWidget();
  QVBoxLayout* lLayout = new QVBoxLayout();
  lAboutWidget->setLayout(lLayout);

  lLayout->setContentsMargins(15,15,15,15);

  QLabel* lSoftwareName = new QLabel(tr("Visual VIPERS"));
  QLabel* lSoftwareDesc = new QLabel(tr("Visual Video and Image Processing Environment for Real-time Systems"));
  QLabel* lCopyright = new QLabel(tr("(C) Copyright 2009 Frederic Jean"));
  QLabel* lWebsite = new QLabel("<a href=\"http://vipers.sourceforge.net\">http://vipers.sourceforge.net</a>");
  lWebsite->setOpenExternalLinks(true);

  lLayout->addStretch(1);
  lLayout->addWidget(lSoftwareName, 0, Qt::AlignLeft);
  lLayout->addWidget(lSoftwareDesc, 0, Qt::AlignLeft);
  lLayout->addSpacing(10);
  lLayout->addWidget(lCopyright, 0, Qt::AlignLeft);
  lLayout->addSpacing(10);
  lLayout->addWidget(lWebsite, 0, Qt::AlignLeft);
  lLayout->addStretch(1);

  return lAboutWidget;
}

//-------------------------------------------------------------------------------

QWidget* AboutDialog::createAuthorsTab()
{
  QWidget* lAuthorsWidget = new QWidget();
  QVBoxLayout* lLayout = new QVBoxLayout();
  lAuthorsWidget->setLayout(lLayout);

  QScrollArea* lScrollArea = new QScrollArea();
  lScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  lScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  lScrollArea->setAlignment(Qt::AlignTop);
  lScrollArea->setFrameStyle(QFrame::NoFrame);

  QLabel* lText = new QLabel();
  lText->setWordWrap(true);
  lText->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  lText->setOpenExternalLinks(true);
  lText->setMargin(5);

  lScrollArea->setWidget(lText);
  lScrollArea->setWidgetResizable(true);

  QFile lFile(":/texts/authors");
  if(lFile.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QTextStream lTextStream(&lFile);
    QString lTmpFileContent;
    QString lTmpLine;

    while(!lTextStream.atEnd())
    {
      lTmpLine = lTextStream.readLine();
      if(lTmpLine.contains("@"))
      {
        lTmpLine = lTmpLine.trimmed();
        lTmpLine = "&nbsp;&nbsp;<a href=\"mailto:" + lTmpLine + "\">" + lTmpLine + "</a>";
      }
      else if(lTmpLine.size()>0 && lTmpLine[0].isSpace())
        lTmpLine = "&nbsp;&nbsp;" + lTmpLine.trimmed();

      lTmpFileContent += lTmpLine + "<br>";
    }

    lText->setText(lTmpFileContent);
  }
  else
  {
    lText->setText(tr("Error: Cannot find the AUTHORS file"));
  }

  lLayout->addWidget(lScrollArea, 1);

  return lAuthorsWidget;
}

//-------------------------------------------------------------------------------

QWidget* AboutDialog::createLicenseTab()
{
  QWidget* lLicenseWidget = new QWidget();

  QVBoxLayout* lLayout = new QVBoxLayout();
  lLicenseWidget->setLayout(lLayout);

  QTextEdit* lText = new QTextEdit();
  lText->setReadOnly(true);
  lText->setWordWrapMode(QTextOption::WordWrap);

  QFile lFile(":/texts/license");
  if(lFile.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QTextStream lTextStream(&lFile);
    QString lTmpFileContent;

    while(!lTextStream.atEnd())
      lTmpFileContent += lTextStream.readLine() + "\n";

    lText->setText(lTmpFileContent);
  }
  else
  {
    lText->setText(tr("Error: Cannot find the COPYING (license) file"));
  }

  lLayout->addWidget(lText, 1);

  return lLicenseWidget;
}

//-------------------------------------------------------------------------------
