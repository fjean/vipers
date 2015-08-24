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
 * \file VIPERS/ImageUtils.hpp
 * \brief Utilities function for the %Image class
 * \author Frederic Jean
 * $Revision: 266 $
 * $Date: 2009-10-20 23:55:59 -0400 (Tue, 20 Oct 2009) $
 */

#ifndef VIPERS_IMAGE_UTILS_HPP
#define VIPERS_IMAGE_UTILS_HPP

#include "Image.hpp"

#ifdef VIPERS_UTILS_OPENCV
  #include <cv.h>
  #include <cstdlib>
#endif

#ifdef VIPERS_UTILS_QT
  #include <QImage>
  #include <ctype.h>
#endif

namespace VIPERS
{

  #ifdef VIPERS_UTILS_OPENCV

  //! Convert IplImage depth to IMage depth
  inline Image::Depth convertDepthFromIpl(int inDepth)
  {
    Image::Depth lDepth;
    switch(inDepth)
    {
      case IPL_DEPTH_8U:
        lDepth = Image::eDepth8U;
        break;
      case IPL_DEPTH_8S:
        lDepth = Image::eDepth8S;
        break;
      case IPL_DEPTH_16U:
        lDepth = Image::eDepth16U;
        break;
      case IPL_DEPTH_16S:
        lDepth = Image::eDepth16S;
        break;
      case IPL_DEPTH_32S:
        lDepth = Image::eDepth32S;
        break;
      case IPL_DEPTH_32F:
        lDepth = Image::eDepth32F;
        break;
      case IPL_DEPTH_64F:
        lDepth = Image::eDepth64F;
        break;
      default:
        lDepth = Image::eDepthInvalid;
        break;
    }
    return lDepth;
  }

  //! Convert Image depth to IplImage depth
  inline int convertDepthToIpl(Image::Depth inDepth)
  {
    int lDepth;
    switch(inDepth)
    {
      case Image::eDepth8U:
        lDepth = IPL_DEPTH_8U;
        break;
      case Image::eDepth8S:
        lDepth = IPL_DEPTH_8S;
        break;
      case Image::eDepth16U:
        lDepth = IPL_DEPTH_16U;
        break;
      case Image::eDepth16S:
        lDepth = IPL_DEPTH_16S;
        break;
      case Image::eDepth32S:
        lDepth = IPL_DEPTH_32S;
        break;
      case Image::eDepth32F:
        lDepth = IPL_DEPTH_32F;
        break;
      case Image::eDepth64F:
        lDepth = IPL_DEPTH_64F;
        break;
      default:
        lDepth = 0;
        break;
    }
    return lDepth;
  }

  //! Copy IplImage to Image object (deep copy)
  inline bool copyFromIplImage(const IplImage* inIplImage, Image& outImage)
  {
    if(inIplImage)
      return outImage.create(inIplImage->width, inIplImage->height, convertDepthFromIpl(inIplImage->depth), static_cast<Image::Channel>(inIplImage->nChannels), true, inIplImage->imageData);
    else
      return false;
  }

  //! Copy Image to IplImage (deep copy)
  inline bool copyToIplImage(const Image& inImage, IplImage** outIplImage)
  {
    if(!outIplImage)
      return false;
    *outIplImage = ::cvCreateImage(cvSize(inImage.getWidth(), inImage.getHeight()), convertDepthToIpl(inImage.getDepth()), inImage.getNbChannels());
    ::memcpy((*outIplImage)->imageData, inImage.getData(), (*outIplImage)->imageSize);
  }

  //! Set Image from an IplImage (copy data pointer)
  inline bool setFromIplImage(const IplImage* inIplImage, Image& outImage)
  {
    if(inIplImage)
      return outImage.create(inIplImage->width, inIplImage->height, convertDepthFromIpl(inIplImage->depth), static_cast<Image::Channel>(inIplImage->nChannels), false, inIplImage->imageData);
    else
      return false;
  }

  //! Set IplImage to an Image (copy data pointer)
  inline bool setToIplImage(const Image& inImage, IplImage** outIplImage)
  {
    if(!outIplImage)
      return false;
    *outIplImage = ::cvCreateImageHeader(cvSize(inImage.getWidth(), inImage.getHeight()), convertDepthToIpl(inImage.getDepth()), inImage.getNbChannels());
    (*outIplImage)->imageData = inImage.getData();
  }

  #endif //VIPERS_UTILS_OPENCV

}

#endif //VIPERS_IMAGE_UTILS_HPP
