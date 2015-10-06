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
#include "itkScalarImageToConnectedIntensitySizeListSampleFilter.h"
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
    m_UseMinMaxIntensity = false;
    m_UseDynamicThreshold = false;

    m_MinSize = 1;
    m_MaxSize = -1;
    m_UseMinMaxSize = false;

    m_NumberOfIntensityBins = 16;
    m_NumberOfSizeBins = 16;

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
  typedef itk::DataObjectDecorator< HistogramType > DataObjectDecoratorType;
  return DataObjectDecoratorType::New().GetPointer();
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

  typedef itk::Statistics::ScalarImageToConnectedIntensitySizeListSampleFilter< InputImageType > ScalarImageToConnectedIntensitySizeListSampleType;
  typedef typename ScalarImageToConnectedIntensitySizeListSampleType::Pointer ScalarImageToConnectedIntensitySizeListSamplePointerType;

  //Compute min max intensity of the image
  if(!(this->GetUseMinMaxIntensity())){
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

  //Generate a sample vector.
  //The sample vector is generated using the intensity and the size of each component

  SampleType* sample = 0;
  if(!this->GetUseDynamicThreshold()){
      ScalarImageToIntensitySizeListSamplePointerType runlengthfilter = ScalarImageToIntensitySizeListSampleType::New();
      runlengthfilter->SetMinIntensity(this->GetMinIntensity());
      runlengthfilter->SetMaxIntensity(this->GetMaxIntensity());
      runlengthfilter->SetNumberOfIntensityBins(this->GetNumberOfIntensityBins());
      runlengthfilter->SetMinSize(this->GetMinSize());
      if(this->GetMaxSize() != -1){
          runlengthfilter->SetMaxSize(this->GetMaxSize());
      }
      runlengthfilter->SetBackgroundValue(this->GetBackgroundValue());
      runlengthfilter->SetInput(inputImage);
      runlengthfilter->Update();

      sample = const_cast< SampleType* >(runlengthfilter->GetOutput());
  }else{
      ScalarImageToConnectedIntensitySizeListSamplePointerType runlengthfilter = ScalarImageToConnectedIntensitySizeListSampleType::New();
      runlengthfilter->SetMinIntensity(this->GetMinIntensity());
      runlengthfilter->SetMaxIntensity(this->GetMaxIntensity());
      runlengthfilter->SetNumberOfIntensityBins(this->GetNumberOfIntensityBins());
      runlengthfilter->SetMinSize(this->GetMinSize());
      if(this->GetMaxSize() != -1){
          runlengthfilter->SetMaxSize(this->GetMaxSize());
      }
      runlengthfilter->SetBackgroundValue(this->GetBackgroundValue());
      runlengthfilter->SetInput(inputImage);
      runlengthfilter->Update();

      sample = const_cast< SampleType* >(runlengthfilter->GetOutput());
  }

  if(this->GetMaxSize() == -1){
      int max = numeric_limits<int>::min();
      cout<<"Calculating the max size of the components..."<<endl;
      for(int i = 0; i < sample->GetTotalFrequency(); i++){
          int val = sample->GetMeasurementVector(i)[1];
          if(max < val){
              max = val;
          }
      }
      this->SetMaxSize(max);
  }

  //Set the bin ranges for the histogram.
  MeasurementVectorType binmin;
  binmin.SetSize(2);
  binmin[0] = this->GetMinIntensity();
  binmin[1] = this->GetMinSize();

  MeasurementVectorType binmax;
  binmax.SetSize(2);
  binmax[0] = this->GetMaxIntensity();
  binmax[1] = this->GetMaxSize();

  //Generate a histogram from the samples. AutoMinMax is set to false in order to generate the correct bining using the values
  typedef itk::Statistics::SampleToHistogramFilter< SampleType, HistogramType > SampleToHistogramFilterType;
  typedef typename  SampleToHistogramFilterType::Pointer SampleToHistogramFilterPointerType;
  SampleToHistogramFilterPointerType sampleToHistogram = SampleToHistogramFilterType::New();
  typename SampleToHistogramFilterType::HistogramSizeType histogramSize(2);
  histogramSize[0] = this->GetNumberOfIntensityBins();
  histogramSize[1] = this->GetNumberOfSizeBins();
  sampleToHistogram->SetAutoMinimumMaximum(false);
  sampleToHistogram->SetHistogramBinMinimum(binmin);
  sampleToHistogram->SetHistogramBinMaximum(binmax);
  sampleToHistogram->SetHistogramSize(histogramSize);
  sampleToHistogram->SetInput(sample);
  sampleToHistogram->Update();
  const HistogramType* histogram = sampleToHistogram->GetOutput();

  HistogramType::SizeType size = histogram->GetSize();
  unsigned int totalBins = 1;
  for(unsigned int i = 0; i < size.GetNumberOfElements(); i++){
      totalBins *= histogram->GetSize()[i];
  }


  m_HistogramOutput.clear();

  m_HistogramOutput << "BinId, ";

  HistogramType::BinMinContainerType mins = histogram->GetMins();
  HistogramType::BinMaxContainerType maxs = histogram->GetMaxs();

  int count = 0;
  for(unsigned k = 0; k < mins[1].size(); k++){
      for(unsigned j = 0; j < mins[0].size(); j++){
          m_HistogramOutput << "[";
          m_HistogramOutput << mins[0][j];
          m_HistogramOutput << "-";
          m_HistogramOutput << maxs[0][j];
          m_HistogramOutput <<"]";

          m_HistogramOutput << "-";

          m_HistogramOutput << "[";
          m_HistogramOutput << mins[1][k];
          m_HistogramOutput << "-";
          m_HistogramOutput << maxs[1][k];
          m_HistogramOutput <<"]";
          m_HistogramOutput <<",";
          count++;
      }
  }

  m_HistogramOutput << endl;

  m_HistogramOutput << "Frequency, ";
  for(unsigned int i = 0; i < totalBins; ++i){
      m_HistogramOutput << histogram->GetFrequency(i) << ", ";
  }

  m_HistogramOutput << endl;


  //Calculate the RLE features of the histogram.
  HistogramToRunLengthFeaturesFilterPointerType histogramtorunlength = HistogramToRunLengthFeaturesFilterType::New();
  histogramtorunlength->SetInput(histogram);
  histogramtorunlength->Update();

  this->SetShortRunEmphasis(histogramtorunlength->GetShortRunEmphasis());
  this->SetLongRunEmphasis(histogramtorunlength->GetLongRunEmphasis());
  this->SetGreyLevelNonuniformity(histogramtorunlength->GetGreyLevelNonuniformity());
  this->SetRunLengthNonuniformity(histogramtorunlength->GetRunLengthNonuniformity());
  this->SetLongRunLowGreyLevelEmphasis(histogramtorunlength->GetLowGreyLevelRunEmphasis());
  this->SetHighGreyLevelRunEmphasis(histogramtorunlength->GetHighGreyLevelRunEmphasis());
  this->SetLowGreyLevelRunEmphasis(histogramtorunlength->GetShortRunLowGreyLevelEmphasis());
  this->SetGreyLevelNonuniformity(histogramtorunlength->GetShortRunHighGreyLevelEmphasis());
  this->SetLongRunLowGreyLevelEmphasis(histogramtorunlength->GetLongRunLowGreyLevelEmphasis());
  this->SetLongRunHighGreyLevelEmphasis(histogramtorunlength->GetLongRunHighGreyLevelEmphasis());


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
    os << indent << "ComputeMinMaxIntensity"<< m_UseMinMaxIntensity <<endl;
}

}// namespace statistics
}// namespace itk

#endif
