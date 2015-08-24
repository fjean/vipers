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
 * \file VIPERS/Image.hpp
 * \brief Image class header.
 * \author Frederic Jean
 * $Revision: 253 $
 * $Date: 2009-10-15 22:30:33 -0400 (Thu, 15 Oct 2009) $
 */

#ifndef VIPERS_IMAGE_HPP
#define VIPERS_IMAGE_HPP

namespace VIPERS
{

  /*! \brief %Image class.
    \author Fr&eacute;d&eacute;ric Jean, Computer Vision and Systems Laboratory, Laval University, QC, Canada

    \todo
  */
  class Image
  {
    public:

      //! Image depth
      enum Depth
      {
        eDepthUndefined, //!> Undefined
        eDepth8U,  //!> 8 bits unsigned
        eDepth8S,  //!> 8 bits signed
        eDepth16U, //!> 16 bits unsigned
        eDepth16S, //!> 16 bits signed
        eDepth32S, //!> 32 bits signed
        eDepth32F, //!> 32 bits float
        eDepth64F,  //!> 64 bits float
        eDepthInvalid = eDepth64F + 1 //!< Invalid value for this enumeration
      };

      //! Image channel
      enum Channel
      {
        eChannel0, //!< No channel
        eChannel1, //!< Channel 1 or one channel
        eChannel2, //!< Channel 2 or two channels
        eChannel3, //!< Channel 3 or three channels
        eChannel4  //!< Channel 4 or four channels
      };

      //! Image color model
      enum Model
      {
        eModelUndefined, //!< Undefined
        eModelGray,      //!< Gray scale
        eModelRGB,       //!< Red Green Blue
        eModelRGBA,      //!< Red Green Blue Alpha
        eModelBGR,       //!< Blue Green Red
        eModelBGRA,      //!< Blue Green Red Alpha
        eModelBGR555,    //!< Blue Green Red 555
        eModelBGR565,    //!< Blue Green Red 565
        eModelYCrCb,     //!< Luminance (Y) Chroma Red (Cr) Chroma Blue (Cb)
        eModelHSV,       //!< Hue Saturation Value
        eModelHLS,       //!< Hue Luminance Saturation
        eModelLab,       //!< CIE L* a* b*
        eModelLuv,       //!< CIE L* u* v*
        eModelXYZ,       //!< CIE XYZ
        eModelBayerBG,   //!< Bayer Pattern Blue Green
        eModelBayerGB,   //!< Bayer Pattern Green Blue
        eModelBayerRG,   //!< Bayer Pattern Red Green
        eModelBayerGR,   //!< Bayer Pattern Green Red
        eModelInvalid = eModelBayerGR + 1 //!< Invalid value for this enumeration
      };

      //! Image origin
      enum Origin
      {
        eOriginTopLeft,    //!< Top left corner
        eOriginBottomLeft  //!< Bottom left corner
      };

      //! Default constructor
      Image(bool inManaged = true);
      //! Create an image with the specified attribute
      Image(unsigned int inWidth, unsigned int inHeight, Depth inDepth, Channel inNbChannels, bool inManaged = true, char* inData = 0);
      //! Create image with specified size and color model
      Image(unsigned int inWidth, unsigned int inHeight, Model inModel, bool inManaged = true, char* inData = 0);
      //! Copy constructor
      Image(const Image& inImage);
      //! Destructor
      ~Image();

      //! Clear image data
      void clear();

      //! Assignment operator
      Image& operator=(const Image& inImage);

      //! Create image with specified attributes
      bool create(unsigned int inWidth, unsigned int inHeight, Depth inDepth, Channel inNbChannels, bool inManaged = true, char* inData = 0);
      //! Create image with specified size and color model
      bool create(unsigned int inWidth, unsigned int inHeight, Model inModel, bool inManaged = true, char* inData = 0);

      //! Set image origin
      inline void setOrigin(Origin inOrigin) {mOrigin = inOrigin;}
      //! Set image model
      inline void setModel(Model inModel) {mModel = inModel;}

      //! Get image width
      inline int getWidth() const {return mWidth;}
      //! Get image height
      inline int getHeight() const {return mHeight;}
      //! Get image depth
      inline Depth getDepth() const {return mDepth;}
      //! Get image origin
      inline Origin getOrigin() const {return mOrigin;}
      //! Get number of channels
      inline Channel getNbChannels() const {return mNbChannels;}
      //! Get image color model
      inline Model getModel() const {return mModel;}
      //! Get image size in bytes
      inline unsigned int getSizeBytes() const {return mSizeBytes;}
      //! Get image row length in bytes
      inline unsigned int getRowLengthBytes() const {return mRowLengthBytes;}
      //! Get bytes per pixel
      unsigned int getBytesPerPixel() const;

      //! Get model number of channels
      Channel getModelNbChannels(Model inModel) const;
      //! Get model number of components
      unsigned int getModelNbComponents(Model inModel) const;
      //! Get model bytes per pixel
      unsigned int getModelBytesPerPixel(Model inModel) const;
      //! Get model depth
      Depth getModelDepth(Model inModel) const;

      //! Check if image data is managed by this instance
      inline bool isManaged() const {return mManaged;}
      //! Check if image data is valid
      inline bool isValid() const {return mData!=0;}

      //! Get pointer to image data
      inline char* getData() const {return mData;}

      //! Set image data pointer (no effect if image is managed)
      void operator=(char* inImage);
      //! Set image data pointer (no effect if image is managed)
      void operator=(short* inImage);
      //! Set image data pointer (no effect if image is managed)
      void operator=(int* inImage);
      //! Set image data pointer (no effect if image is managed)
      void operator=(unsigned char* inImage);
      //! Set image data pointer (no effect if image is managed)
      void operator=(unsigned short* inImage);
      //! Set image data pointer (no effect if image is managed)
      void operator=(unsigned int* inImage);
      //! Set image data pointer (no effect if image is managed)
      void operator=(float* inImage);
      //! Set image data pointer (no effect if image is managed)
      void operator=(double* inImage);

      //! Pointer to image data
      inline operator char*() const {return mData;}
      //! Pointer to image data
      inline operator short*() const {return reinterpret_cast<short*>(mData);}
      //! Pointer to image data
      inline operator int*() const {return reinterpret_cast<int*>(mData);}
      //! Pointer to image data
      inline operator unsigned char*() const {return reinterpret_cast<unsigned char*>(mData);}
      //! Pointer to image data
      inline operator unsigned short*() const {return reinterpret_cast<unsigned short*>(mData);}
      //! Pointer to image data
      inline operator unsigned int*() const {return reinterpret_cast<unsigned int*>(mData);}
      //! Pointer to image data
      inline operator float*() const {return reinterpret_cast<float*>(mData);}
      //! Pointer to image data
      inline operator double*() const {return reinterpret_cast<double*>(mData);}

    private:

      bool mManaged; //!< Is the image data is managed by this instance

      char* mData; //!< Pointer to image data

      unsigned int mWidth; //!< Image width
      unsigned int mHeight; //!< Image height

      unsigned int mRowLengthBytes; //!< Length of a row in bytes
      unsigned int mSizeBytes; //!< Image size in bytes

      Channel mNbChannels; //!< Number of channels
      Depth mDepth; //!< Image depth
      Model mModel; //!< Image color model
      Origin mOrigin; //!< Image origin

  };

}

#endif //VIPERS_IMAGE_HPP
