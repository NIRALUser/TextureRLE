/*=========================================================================

  Program:   Texture Analysis
  Module:    $$
  Language:  C++
  Date:      $Date: xxx-xx-xx $
  Version:   $Revision: xxx $

  Copyright (c) University of North Carolina at Chapel Hill (UNC-CH) All Rights Reserved.

  See License.txt or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/
#ifndef __itkTexture3DFilter_h
#define __itkTexture3DFilter_h

#include <itkObject.h>
#include <itkImageToImageFilter.h>
#include <itkImage.h>
//#include <itkCastImageFilter.h>
#include <itkImageRegionIterator.h>
#include <itkPoint.h>
//#include "define.h"

namespace itk
{
/** \class Texture3DFilter
 *
 * Resample diffusion tensor images
 * A transformation and a interpolation have to be set
 */
template <class TInput, class TOutput>
class Texture3DFilter
  : public ImageToImageFilter
  <TInput,
   TOutput>
{
public:
  typedef TInput  InputDataType;
  typedef TOutput OutputDataType;
  typedef ImageToImageFilter <TInput, TOutput>                     Superclass;
  typedef Image<float, 3>                                          InputImageType;
  typedef Image<float, 3>                                          OutputImageType;
  typedef Texture3DFilter                                          Self;
  typedef SmartPointer<Self>                                       Pointer;
  typedef SmartPointer<const Self>                                 ConstPointer;
  typedef Image<unsigned int, 3>                                   InputROIImageType;
  typedef typename InputImageType::Pointer                         InputImagePointerType;
  typedef typename InputROIImageType::Pointer                      InputROIImagePointerType;
  typedef typename OutputImageType::Pointer                        OutputImagePointerType;
  typedef itk::ImageRegionIterator<OutputImageType>                IteratorType;
  typedef typename OutputImageType::RegionType                     OutputImageRegionType;

  itkNewMacro( Self );
///Set the input image
  void SetInput( InputImagePointerType inputImage );

///Set the roi input image
  void SetInputROI( InputROIImagePointerType inputROIImage );

///Set the output parameters (size, spacing, origin, orientation) from a reference image
  void SetOutputParametersFromImage( InputImagePointerType Image );

 // itkSetMacro( DefaultPixelValue, OutputDataType );
 // itkGetMacro( DefaultPixelValue, OutputDataType );

  itkSetMacro( OutputOrigin, typename OutputImageType::PointType );
  itkSetMacro( OutputSpacing, typename OutputImageType::SpacingType );
  itkSetMacro( OutputSize, typename OutputImageType::SizeType );
  itkSetMacro( OutputDirection, typename OutputImageType::DirectionType );
  Matrix<double, 3, 3> GetOutputMeasurementFrame();

  itkGetMacro( OutputOrigin, typename OutputImageType::PointType );
  itkGetMacro( OutputSpacing, typename OutputImageType::SpacingType );
  itkGetMacro( OutputSize, typename OutputImageType::SizeType );
  itkGetMacro( OutputDirection, typename OutputImageType::DirectionType );

  TOutput * GetOutput (unsigned int idx);   // idx 00-04 Run length, idx 05-08 Coocurrence

protected:
  Texture3DFilter();
  void ThreadedGenerateData( const OutputImageRegionType & outputRegionForThread, ThreadIdType threadId );

  void BeforeThreadedGenerateData();

  void GenerateOutputInformation(unsigned int idx);

  void AfterThreadedGenerateData();

  void GenerateInputRequestedRegion();

 // DataObject::Pointer MakeOutput(unsigned int idx);

private:
  typename OutputImageType::PointType m_OutputOrigin;
  typename OutputImageType::SpacingType m_OutputSpacing;
  typename OutputImageType::SizeType m_OutputSize;
  typename OutputImageType::DirectionType m_OutputDirection;
  //OutputDataType       m_DefaultPixelValue;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTexture3DFilter.txx"
#endif

#endif
