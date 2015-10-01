/*=========================================================================

  Program:   Texture Analysis
  Module:    $$
  Language:  C++
  Date:      $Date: xxx-xx-xx $
  Version:   $Revision: xxx $

  Copyright (c) University of North Carolina at Chapel Hill (UNC-CH) All Rights Reserved.

  See License.txt or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/

#ifndef __itkScalarImageToIntensitySizeRunLengthFeaturesFilter_hxx
#define __itkScalarImageToIntensitySizeRunLengthFeaturesFilter_hxx

#include "itkScalarImageToIntensitySizeRunLengthFeaturesFilter.h"

#include "itkScalarImageToIntensitySizeListSampleFilter.h"
#include <itkSampleToHistogramFilter.h>
#include <itkHistogramToRunLengthFeaturesFilter.h>

#include <itkDataObjectDecorator.h>

#include <itkMaskImageFilter.h>

namespace itk
{
namespace Statistics {
/**
 * Constructor
 */
template< typename TInputImage >
ScalarImageToIntensitySizeRunLengthFeaturesFilter< TInputImage >
::ScalarImageToIntensitySizeRunLengthFeaturesFilter()
{
    m_MinIntensity = 0;
    m_MaxIntensity = 0;
    m_NumberOfIntensityBins = 32;
    m_NumberOfSizeBins = 32;

    m_ComputeMinMaxIntensity = true;
    m_BackgroundValue = 0;
    m_InputMask = 0;

    this->SetNumberOfRequiredInputs(1);
    this->SetNumberOfRequiredOutputs(1);

    this->ProcessObject::SetNthOutput( 0, this->MakeOutput(0) );

}

template< typename TImage >
typename ScalarImageToIntensitySizeRunLengthFeaturesFilter< TImage >::DataObjectPointer
ScalarImageToIntensitySizeRunLengthFeaturesFilter< TImage >
::MakeOutput( DataObjectPointerArraySizeType itkNotUsed(idx) )
{
  typedef itk::DataObjectDecorator< MapContainterType > DataObjectDecoratorType;
  typename DataObjectDecoratorType::Pointer objdec = DataObjectDecoratorType::New();
    objdec->Set(MapContainterType::New());
  return objdec.GetPointer();
}

/**
 *
 */
template< typename TInputImage >
void
ScalarImageToIntensitySizeRunLengthFeaturesFilter< TInputImage >
::SetInput(const InputImageType *input)
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput( 0,
                                    const_cast< InputImageType * >( input ) );
}

/**
 * Connect one of the operands for pixel-wise addition
 */
template< typename TInputImage >
void
ScalarImageToIntensitySizeRunLengthFeaturesFilter< TInputImage >
::SetInput(unsigned int index, const TInputImage *image)
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput( index, const_cast< TInputImage * >( image ) );
}

/**
 *
 */
template< typename TInputImage >
const typename ScalarImageToIntensitySizeRunLengthFeaturesFilter< TInputImage >::InputImageType *
ScalarImageToIntensitySizeRunLengthFeaturesFilter< TInputImage >::GetInput(void) const
{
  return itkDynamicCastInDebugMode< const TInputImage * >( this->GetPrimaryInput() );
}

/**
 *
 */
template< typename TInputImage >
const typename ScalarImageToIntensitySizeRunLengthFeaturesFilter< TInputImage >::InputImageType *
ScalarImageToIntensitySizeRunLengthFeaturesFilter< TInputImage >
::GetInput(unsigned int idx) const
{
  const TInputImage *in = dynamic_cast< const TInputImage * >
    ( this->ProcessObject::GetInput(idx) );

  if ( in == ITK_NULLPTR && this->ProcessObject::GetInput(idx) != ITK_NULLPTR )
    {
    itkWarningMacro (<< "Unable to convert input number " << idx << " to type " <<  typeid( InputImageType ).name () );
    }
  return in;
}

template< typename TImage >
void
ScalarImageToIntensitySizeRunLengthFeaturesFilter< TImage >
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method. this should
  // copy the output requested region to the input requested region
  Superclass::GenerateInputRequestedRegion();
}

template< typename TInputImage >
const typename ScalarImageToIntensitySizeRunLengthFeaturesFilter< TInputImage >::MapContainterType*
ScalarImageToIntensitySizeRunLengthFeaturesFilter< TInputImage >::GetOutput(){

    typedef itk::DataObjectDecorator< MapContainterType > DataObjectDecoratorType;
    const DataObjectDecoratorType* objdecorator = static_cast<const DataObjectDecoratorType*>(this->ProcessObject::GetOutput(0));
    const MapContainterType* output = static_cast<const MapContainterType*>(objdecorator->Get());

    return output;

}

template< typename TInputImage >
void
ScalarImageToIntensitySizeRunLengthFeaturesFilter< TInputImage >
::GenerateData()
{

  InputImageConstPointer inputImage = this->GetInput();
  InputImageConstPointer inputMask = this->GetInputMask();

  if(inputMask){//If there is a mask then mask all the values in the image
      typedef itk::MaskImageFilter< InputImageType, InputImageType > MaskImageFilterType;
      typedef typename MaskImageFilterType::Pointer MaskImageFilterPointerType;

      MaskImageFilterPointerType maskfilter = MaskImageFilterType::New();
      maskfilter->SetInput(inputImage);
      maskfilter->SetMaskImage(inputMask);
      maskfilter->SetMaskingValue(this->GetBackgroundValue());
      maskfilter->SetOutsideValue(this->GetBackgroundValue());
      maskfilter->Update();

      inputImage = maskfilter->GetOutput();

  }

  typedef itk::Statistics::ScalarImageToIntensitySizeListSampleFilter< InputImageType > ScalarImageToIntensitySizeListSampleType;
  typedef typename ScalarImageToIntensitySizeListSampleType::Pointer ScalarImageToIntensitySizeListSamplePointerType;
  typedef typename  ScalarImageToIntensitySizeListSampleType::SampleType SampleType;
  typedef typename  ScalarImageToIntensitySizeListSampleType::SamplePointerType SamplePointerType;

  //Compute min max intensity of the image
  if(this->GetComputeMinMaxIntensity()){
      InputImageRegionIteratorType it(inputImage, inputImage->GetLargestPossibleRegion());
      InputImagePixelType minvalue = numeric_limits< InputImagePixelType >::max();
      InputImagePixelType maxvalue = numeric_limits< InputImagePixelType >::min();
      while(!it.IsAtEnd()){
          InputImagePixelType pix = it.Get();
          if(pix != this->GetBackgroundValue()){
              if(minvalue > pix){
                  minvalue = pix;
              }
              if(maxvalue < pix){
                  maxvalue = pix;
              }
          }
          ++it;
      }
      this->SetMinIntensity(minvalue);
      this->SetMaxIntensity(maxvalue);
  }

  //Get the sample vector with the measurements of the size.
  ScalarImageToIntensitySizeListSamplePointerType runlengthfilter = ScalarImageToIntensitySizeListSampleType::New();
  runlengthfilter->SetMinIntensity(this->GetMinIntensity());
  runlengthfilter->SetMaxIntensity(this->GetMaxIntensity());
  runlengthfilter->SetNumberOfIntensityBins(this->GetNumberOfIntensityBins());
  runlengthfilter->SetBackgroundValue(this->GetBackgroundValue());
  runlengthfilter->SetInput(inputImage);
  runlengthfilter->Update();

  SampleType* sample = const_cast< SampleType* >(runlengthfilter->GetOutput());

  //Generate a histogram from the samples and use the bin sizes for intensity and size.
  typedef itk::Statistics::SampleToHistogramFilter< SampleType, HistogramType > SampleToHistogramFilterType;
  typedef typename  SampleToHistogramFilterType::Pointer SampleToHistogramFilterPointerType;
  SampleToHistogramFilterPointerType sampleToHistogram = SampleToHistogramFilterType::New();
  typename SampleToHistogramFilterType::HistogramSizeType histogramSize(2);
  histogramSize[0] = this->GetNumberOfIntensityBins();
  histogramSize[1] = this->GetNumberOfSizeBins();
  sampleToHistogram->SetHistogramSize(histogramSize);
  sampleToHistogram->SetInput(sample);
  sampleToHistogram->Update();
  const HistogramType* histogram = sampleToHistogram->GetOutput();

  //Calculate the RLE features of the histogram.
  HistogramToRunLengthFeaturesFilterPointerType histogramtorunlength = HistogramToRunLengthFeaturesFilterType::New();
  histogramtorunlength->SetInput(histogram);
  histogramtorunlength->Update();

  typename MapContainterType::STLContainerType output = this->GetOutput()->CastToSTLConstContainer();

    output["ShortRunEmphasis"] = histogramtorunlength->GetShortRunEmphasis();
    output["LongRunEmphasis"] = histogramtorunlength->GetLongRunEmphasis();
    output["GreyLevelNonuniformity"] = histogramtorunlength->GetGreyLevelNonuniformity();
    output["RunLengthNonuniformity"] = histogramtorunlength->GetRunLengthNonuniformity();
    output["LowGreyLevelRunEmphasis"] = histogramtorunlength->GetLowGreyLevelRunEmphasis();
    output["HighGreyLevelRunEmphasis"] = histogramtorunlength->GetHighGreyLevelRunEmphasis();
    output["ShorRunLowGreyLevelEmphasis"] = histogramtorunlength->GetShortRunLowGreyLevelEmphasis();
    output["ShorRunHighGreyLevelEmphasis"] = histogramtorunlength->GetShortRunHighGreyLevelEmphasis();
    output["LongRunLowGreyLevelEmphasis"] = histogramtorunlength->GetLongRunLowGreyLevelEmphasis();
    output["LongRunHighGreyLevelEmphasis"] = histogramtorunlength->GetLongRunHighGreyLevelEmphasis();


}

template< typename TInputImage >
void
ScalarImageToIntensitySizeRunLengthFeaturesFilter< TInputImage >
::PrintSelf(std::ostream & os, Indent indent) const
{
    Superclass::PrintSelf(os, indent);
    os << indent << "ScalarImageToIntensitySizeRunLengthFeaturesFilter: "<< std::endl;
    os << indent << "MinIntensity: "<<m_MinIntensity <<endl;
    os << indent << "MaxIntensity: "<<m_MaxIntensity<<endl;
    os << indent << "BackgroundValue: "<< m_BackgroundValue <<endl;
    os << indent << "NumberOfIntensityBins: "<< m_NumberOfIntensityBins <<endl;
    os << indent << "NumberOfSizeBins"<< m_NumberOfSizeBins <<endl;
    os << indent << "ComputeMinMaxIntensity"<< m_ComputeMinMaxIntensity <<endl; m_ComputeMinMaxIntensity;
}

}// namespace statistics
}// namespace itk

#endif
