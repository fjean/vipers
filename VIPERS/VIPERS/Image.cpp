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
 * \file VIPERS/Image.cpp
 * \brief Image class functions definition.
 * \author Frederic Jean
 * $Revision: 253 $
 * $Date: 2009-10-15 22:30:33 -0400 (Thu, 15 Oct 2009) $
 */

#include "Image.hpp"

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <new>
#include <iostream>

using namespace VIPERS;

/*! \todo
*/
Image::Image(bool inManaged)
{
  mManaged = inManaged;
  mData = NULL;
  mWidth = 0;
  mHeight = 0;
  mRowLengthBytes = 0;
  mSizeBytes = 0;
  mNbChannels = eChannel0;
  mDepth = eDepthUndefined;
  mModel = eModelUndefined;
  mOrigin = eOriginTopLeft;
}

/*! \todo
*/
Image::Image(unsigned int inWidth, unsigned int inHeight, Depth inDepth, Channel inNbChannels, bool inManaged, char* inData)
{
  create(inWidth, inHeight, inDepth, inNbChannels, inManaged, inData);
}

/*! \todo
*/
Image::Image(unsigned int inWidth, unsigned int inHeight, Model inModel, bool inManaged, char* inData)
{
  create(inWidth, inHeight, inModel, inManaged, inData);
}

/*! \todo
*/
Image::Image(const Image& inImage)
{
  mManaged = true;
  mWidth = inImage.mWidth;
  mHeight = inImage.mHeight;
  mRowLengthBytes = inImage.mRowLengthBytes;
  mSizeBytes = inImage.mSizeBytes;
  mNbChannels = inImage.mNbChannels;
  mDepth = inImage.mDepth;
  mModel = inImage.mModel;
  mOrigin = inImage.mOrigin;

  if(mSizeBytes>0 && inImage.mData)
  {
    try
    {
      mData = new char[mSizeBytes];
      ::memcpy(mData, inImage.mData, mSizeBytes);
    }
    catch(std::bad_alloc& inBadAlloc)
    {
      std::cerr << "VIPERS ERROR: Image Copy Constructor could not allocate " << mSizeBytes << " bytes of memory (" << inBadAlloc.what() << ")" << std::endl;
      mData = NULL;
      clear();
    }
    catch(std::runtime_error inException)
    {
      std::cerr << "ERROR: Image Copy Constructor : " << inException.what() << std::endl;
    }
    catch(...)
    {
      throw;
    }
  }
}

/*! \todo
*/
Image::~Image()
{
  clear();
}

/*! \todo
*/
void Image::clear()
{
  if(mManaged && mData)
    delete [] mData;
  mData = NULL;
  mWidth = 0;
  mHeight = 0;
  mRowLengthBytes = 0;
  mSizeBytes = 0;
  mNbChannels = eChannel0;
  mDepth = eDepthUndefined;
  mModel = eModelUndefined;
  mOrigin = eOriginTopLeft;
}

/*! \todo
*/
Image& Image::operator=(const Image& inImage)
{
  if(mManaged)
  {
    clear();
    mWidth = inImage.mWidth;
    mHeight = inImage.mHeight;
    mRowLengthBytes = inImage.mRowLengthBytes;
    mSizeBytes = inImage.mSizeBytes;
    mNbChannels = inImage.mNbChannels;
    mDepth = inImage.mDepth;
    mModel = inImage.mModel;
    mOrigin = inImage.mOrigin;

    if(mSizeBytes>0 && inImage.mData)
    {
      try
      {
        mData = new char[mSizeBytes];
        ::memcpy(mData, inImage.mData, mSizeBytes);
      }
      catch(std::bad_alloc& inBadAlloc)
      {
        std::cerr << "VIPERS ERROR: Image::operator= could not allocate " << mSizeBytes << " bytes of memory (" << inBadAlloc.what() << ")" << std::endl;
        mData = NULL;
        clear();
      }
      catch(std::runtime_error inException)
      {
        std::cerr << "VIPERS ERROR: Image::operator= : " << inException.what() << std::endl;
      }
      catch(...)
      {
        throw;
      }
    }
  }
  else
  {
    mData = inImage.mData;
    mWidth = inImage.mWidth;
    mHeight = inImage.mHeight;
    mRowLengthBytes = inImage.mRowLengthBytes;
    mSizeBytes = inImage.mSizeBytes;
    mNbChannels = inImage.mNbChannels;
    mDepth = inImage.mDepth;
    mModel = inImage.mModel;
    mOrigin = inImage.mOrigin;
  }
}

/*! \todo
*/
bool Image::create(unsigned int inWidth, unsigned int inHeight, Depth inDepth, Channel inNbChannels, bool inManaged, char* inData)
{
  clear();

  if(inWidth==0 || inHeight==0 || inDepth<=eDepthUndefined || inDepth>=eDepthInvalid || inNbChannels<=eChannel0)
    return false;

  mManaged = inManaged;
  mWidth = inWidth;
  mHeight = inHeight;
  mDepth = inDepth;
  mNbChannels = inNbChannels;
  mModel = eModelUndefined;

  mRowLengthBytes = mWidth*getBytesPerPixel();
  mSizeBytes = mHeight*mRowLengthBytes;

  if(mManaged)
  {
    try
    {
      mData = new char[mSizeBytes];
      if(inData)
        ::memcpy(mData, inData, mSizeBytes);
      else
        ::memset(mData, 0, mSizeBytes);
    }
    catch(std::bad_alloc& inBadAlloc)
    {
      std::cerr << "VIPERS ERROR: Image::create could not allocate " << mSizeBytes << " bytes of memory (" << inBadAlloc.what() << ")" << std::endl;
      mData = NULL;
      clear();
      return false;
    }
    catch(std::runtime_error& inRunTimeErr)
    {
      std::cerr << "VIPERS ERROR: Image::create : " << inRunTimeErr.what() << std::endl;
      return false;
    }
    catch(...)
    {
      throw;
    }
  }
  else
  {
    if(inData)
      mData = inData;
  }

  return true;
}

/*! \todo
*/
bool Image::create(unsigned int inWidth, unsigned int inHeight, Model inModel, bool inManaged, char* inData)
{
  clear();

  if(inWidth==0 || inHeight==0 || inModel<=eModelUndefined || inModel>=eModelInvalid)
    return false;

  mManaged = inManaged;
  mWidth = inWidth;
  mHeight = inHeight;
  mModel = inModel;
  mDepth = getModelDepth(mModel);
  mNbChannels = getModelNbChannels(mModel);

  mRowLengthBytes = mWidth*getBytesPerPixel();
  mSizeBytes = mHeight*mRowLengthBytes;

  if(mManaged)
  {
    try
    {
      mData = new char[mSizeBytes];
      if(inData)
        ::memcpy(mData, inData, mSizeBytes);
      else
        ::memset(mData, 0, mSizeBytes);
    }
    catch(std::bad_alloc& inBadAlloc)
    {
      std::cerr << "VIPERS ERROR: Image::create could not allocate " << mSizeBytes << " bytes of memory (" << inBadAlloc.what() << ")" << std::endl;
      mData = NULL;
      clear();
      return false;
    }
    catch(std::runtime_error& inRunTimeErr)
    {
      std::cerr << "VIPERS ERROR: Image::create : " << inRunTimeErr.what() << std::endl;
      return false;
    }
    catch(...)
    {
      throw;
    }
  }
  else
  {
    if(inData)
      mData = inData;
  }

  return true;
}

/*! \todo
*/
unsigned int Image::getBytesPerPixel() const
{
  unsigned int lBytesDepth;

  switch(mDepth)
  {
    case eDepth8U:
      lBytesDepth = mNbChannels;
      break;
    case eDepth8S:
      lBytesDepth = mNbChannels;
      break;
    case eDepth16U:
      lBytesDepth = 2*mNbChannels;
      break;
    case eDepth16S:
      lBytesDepth = 2*mNbChannels;
      break;
    case eDepth32S:
      lBytesDepth = 4*mNbChannels;
      break;
    case eDepth32F:
      lBytesDepth = 4*mNbChannels;
      break;
    case eDepth64F:
      lBytesDepth = 8*mNbChannels;
      break;
    default:
      lBytesDepth = 0;
  }

  return lBytesDepth;
}

/*! \todo
*/
Image::Channel Image::getModelNbChannels(Model inModel) const
{
  if(mModel==eModelGray || mModel==eModelBayerBG || mModel==eModelBayerGB || mModel==eModelBayerRG || mModel==eModelBayerGR || inModel==eModelBGR555 || mModel==eModelBGR565)
    return eChannel1;
  else if(inModel==eModelRGB || mModel==eModelBGR || mModel==eModelYCrCb || mModel==eModelHSV || mModel==eModelHLS || mModel==eModelLab || mModel==eModelLuv  || mModel==eModelXYZ)
    return eChannel3;
  else if(inModel==eModelRGBA || mModel==eModelBGRA)
    return eChannel4;
  else
    return eChannel0;
}

/*! \todo
*/
unsigned int Image::getModelNbComponents(Model inModel) const
{
  if(mModel==eModelGray || mModel==eModelBayerBG || mModel==eModelBayerGB || mModel==eModelBayerRG || mModel==eModelBayerGR)
    return 1;
  else if(inModel==eModelRGB || mModel==eModelBGR || mModel==eModelYCrCb || mModel==eModelHSV || mModel==eModelHLS || mModel==eModelLab || mModel==eModelLuv  || mModel==eModelXYZ || inModel==eModelBGR555 || mModel==eModelBGR565)
    return 3;
  else if(inModel==eModelRGBA || mModel==eModelBGRA)
    return 4;
  else
    return 0;
}

/*! \todo
*/
unsigned int Image::getModelBytesPerPixel(Model inModel) const
{
  if(mModel==eModelGray || mModel==eModelBayerBG || mModel==eModelBayerGB || mModel==eModelBayerRG || mModel==eModelBayerGR)
    return 1;
  else if(inModel==eModelRGB || mModel==eModelBGR || mModel==eModelYCrCb || mModel==eModelHSV || mModel==eModelHLS || mModel==eModelLab || mModel==eModelLuv  || mModel==eModelXYZ)
    return 3;
  else if(inModel==eModelRGBA || mModel==eModelBGRA)
    return 4;
  else if(inModel==eModelBGR555 || mModel==eModelBGR565)
    return 2;
  else
    return 0;
}

/*! \todo
*/
Image::Depth Image::getModelDepth(Model inModel) const
{
  if(mModel<=eModelUndefined || mModel>=eModelInvalid)
    return eDepthUndefined;
  else if(inModel==eModelBGR555 || mModel==eModelBGR565)
    return eDepth16U;
  else
    return eDepth8U;
}

/*! \todo
*/
void Image::operator=(char* inImage)
{
  if(mManaged)
  {
    mData = inImage;
  }
  else
  {
    try
    {
      ::memcpy(inImage, mData, mSizeBytes);
    }
    catch(std::runtime_error inException)
    {
      std::cerr << "VIPERS ERROR: Image::operator= : " << inException.what() << std::endl;
    }
    catch(...)
    {
      throw;
    }
  }
}

/*! \todo
*/
void Image::operator=(short* inImage)
{
  if(mManaged)
  {
    mData = reinterpret_cast<char*>(inImage);
  }
  else
  {
    try
    {
      ::memcpy(reinterpret_cast<char*>(inImage), mData, mSizeBytes);
    }
    catch(std::runtime_error inException)
    {
      std::cerr << "VIPERS ERROR: Image::operator= : " << inException.what() << std::endl;
    }
    catch(...)
    {
      throw;
    }
  }
}

/*! \todo
*/
void Image::operator=(int* inImage)
{
  if(mManaged)
  {
    mData = reinterpret_cast<char*>(inImage);
  }
  else
  {
    try
    {
      ::memcpy(reinterpret_cast<char*>(inImage), mData, mSizeBytes);
    }
    catch(std::runtime_error inException)
    {
      std::cerr << "VIPERS ERROR: Image::operator= : " << inException.what() << std::endl;
    }
    catch(...)
    {
      throw;
    }
  }
}

/*! \todo
*/
void Image::operator=(unsigned char* inImage)
{
  if(mManaged)
  {
    mData = reinterpret_cast<char*>(inImage);
  }
  else
  {
    try
    {
      ::memcpy(reinterpret_cast<char*>(inImage), mData, mSizeBytes);
    }
    catch(std::runtime_error inException)
    {
      std::cerr << "VIPERS ERROR: Image::operator= : " << inException.what() << std::endl;
    }
    catch(...)
    {
      throw;
    }
  }
}

/*! \todo
*/
void Image::operator=(unsigned short* inImage)
{
  if(mManaged)
  {
    mData = reinterpret_cast<char*>(inImage);
  }
  else
  {
    try
    {
      ::memcpy(reinterpret_cast<char*>(inImage), mData, mSizeBytes);
    }
    catch(std::runtime_error inException)
    {
      std::cerr << "VIPERS ERROR: Image::operator= : " << inException.what() << std::endl;
    }
    catch(...)
    {
      throw;
    }
  }
}

/*! \todo
*/
void Image::operator=(unsigned int* inImage)
{
  if(mManaged)
  {
    mData = reinterpret_cast<char*>(inImage);
  }
  else
  {
    try
    {
      ::memcpy(reinterpret_cast<char*>(inImage), mData, mSizeBytes);
    }
    catch(std::runtime_error inException)
    {
      std::cerr << "VIPERS ERROR: Image::operator= : " << inException.what() << std::endl;
    }
    catch(...)
    {
      throw;
    }
  }
}

/*! \todo
*/
void Image::operator=(float* inImage)
{
  if(mManaged)
  {
    mData = reinterpret_cast<char*>(inImage);
  }
  else
  {
    try
    {
      ::memcpy(reinterpret_cast<char*>(inImage), mData, mSizeBytes);
    }
    catch(std::runtime_error inException)
    {
      std::cerr << "VIPERS ERROR: Image::operator= : " << inException.what() << std::endl;
    }
    catch(...)
    {
      throw;
    }
  }
}

/*! \todo
*/
void Image::operator=(double* inImage)
{
  if(mManaged)
  {
    mData = reinterpret_cast<char*>(inImage);
  }
  else
  {
    try
    {
      ::memcpy(reinterpret_cast<char*>(inImage), mData, mSizeBytes);
    }
    catch(std::runtime_error inException)
    {
      std::cerr << "VIPERS ERROR: Image::operator= : " << inException.what() << std::endl;
    }
    catch(...)
    {
      throw;
    }
  }
}

