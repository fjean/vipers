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

#ifndef VIPERSGUI_MODULEINFO_HPP
#define VIPERSGUI_MODULEINFO_HPP

#include <QString>
#include <QIcon>

class ModuleInfo
{
  public:

    ModuleInfo() {}
    ModuleInfo(const QString& inName, const QString& inDisplayName);

    inline void setName(const QString& inName) {mName = inName;}
    inline const QString& getName() const {return mName;}
    inline void setDisplayName(const QString& inDisplayName) {mDisplayName = inDisplayName;}
    inline const QString& getDisplayName() const {return mDisplayName;}
    inline void setVersion(const QString& inVersion) {mVersion = inVersion;}
    inline const QString& getVersion() const {return mVersion;}
    inline void setFileName(const QString& inFileName) {mFileName = inFileName;}
    inline const QString& getFileName() const {return mFileName;}
    inline void setFilePath(const QString& inFilePath) {mFilePath = inFilePath;}
    inline const QString& getFilePath() const {return mFilePath;}
    inline void setShortDescription(const QString& inShortDescription) {mShortDescription = inShortDescription;}
    inline const QString& getShortDescription() const {return mShortDescription;}
    inline void setLongDescription(const QString& inLongDescription) {mLongDescription = inLongDescription;}
    inline const QString& getLongDescription() const {return mLongDescription;}
    inline void setIcon(const QIcon& inIcon) {mIcon = inIcon;}
    inline const QIcon& getIcon() const {return mIcon;}

  protected:

    QString mName;
    QString mDisplayName;
    QString mVersion;
    QString mFileName;
    QString mFilePath;
    QString mShortDescription;
    QString mLongDescription;
    QIcon mIcon;

};

#endif //VIPERSGUI_MODULEINFO_HPP
