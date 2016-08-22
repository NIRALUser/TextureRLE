/*=========================================================================

  Program:   T2 Relaxation Map
  Module:    Compute T2 Relaxation Map Filter
  Language:  C++  

  Copyright (c) University of North Carolina at Chapel Hill (UNC-CH) All Rights Reserved.

  See License.txt for details.

==========================================================================*/
#ifndef __itkT2RelaxationMapFilter_h
#define __itkT2RelaxationMapFilter_h


#include <itkImageToImageFilter.h>
#include <itkImageRegionIterator.h>

#include "t2Fitting.h"
#include <vnl/algo/vnl_levenberg_marquardt.h>

namespace itk
{
/** \class T2RelaxationMapFilter
 *
 * Resample diffusion tensor images
 * A transformation and a interpolation have to be set
 */
template <class TInputImage, class TOutputImage>
class T2RelaxationMapFilter
  : public ImageToImageFilter
  <TInputImage, TOutputImage>
{
public:

  typedef T2RelaxationMapFilter           Self;
  typedef ImageToImageFilter <TInputImage, TOutputImage>             Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(T2RelaxationMapFilter, ImageToImageFilter)

  /** Method for creation through the object factory. */
  itkNewMacro(Self)

  typedef TInputImage                           InputImageType;
  typedef typename InputImageType::Pointer      InputImagePointer;
  typedef typename InputImageType::ConstPointer InputImageConstPointer;
  typedef typename InputImageType::RegionType   InputImageRegionType;
  typedef typename InputImageType::PixelType    InputImagePixelType;
  typedef typename InputImageType::IndexType    IndexType;

  typedef itk::ImageRegionConstIterator<InputImageType> InputImageIteratorType;

  typedef TOutputImage                          OutputImageType;
  typedef typename OutputImageType::Pointer      OutputImagePointerType;
  typedef typename OutputImageType::ConstPointer OutputImageConstPointer;
  typedef typename OutputImageType::RegionType   OutputImageRegionType;
  typedef typename OutputImageType::PixelType    OutputImagePixelType;
  typedef typename OutputImageType::IndexType    OutputType;

  typedef itk::ImageRegionIterator<OutputImageType> OutputImageIteratorType;

  itkSetMacro( EchoTime, double );
  itkGetMacro( EchoTime, double );

  itkSetMacro( M0, double );
  itkGetMacro( M0, double );

protected:
  T2RelaxationMapFilter();  
  virtual void BeforeThreadedGenerateData();
  virtual void ThreadedGenerateData( const OutputImageRegionType & outputRegionForThread, ThreadIdType threadId );  

private: 
  T2RelaxationMapFilter(const Self &); //purposely not implemented
  void operator=(const Self &);                //purposely not implemented

  double m_EchoTime;
  vnl_vector<double> m_EchoTimes;

  double m_M0;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkT2RelaxationMapFilter.hxx"
#endif

#endif
