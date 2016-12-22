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

#include <itkHistogramToRunLengthFeaturesFilter.h>

#include <itkDataObjectDecorator.h>

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
    m_FullConnectivity = false;

    m_Percentile = 5;

    m_MinSize = 1;
    m_MaxSize = numeric_limits<int>::max();
    m_UseMinMaxSize = false;

    m_NumberOfIntensityBins = 16;
    m_NumberOfSizeBins = 16;

    m_BackgroundValue = 0;

    m_ListSample = 0;

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

  //Compute min max intensity of the labeled region
  if(!(this->GetUseMinMaxIntensity())){
      cout<<"Calculating the min max intensity to build the sample..."<<endl;
      InputImageRegionIteratorType it(inputImage, inputImage->GetLargestPossibleRegion());
      InputImagePixelType minvalue = numeric_limits< InputImagePixelType >::max();
      InputImagePixelType maxvalue = numeric_limits< InputImagePixelType >::min();
      while(!it.IsAtEnd()){
          InputImagePixelType pix = it.Get();
          if(pix != this->GetBackgroundValue()){
              minvalue = min(minvalue, pix);
              maxvalue = max(maxvalue, pix);
          }
          ++it;
      }
      this->SetMinIntensity(minvalue);
      cout<<"\t Min intensity = "<<this->GetMinIntensity()<<endl;
      this->SetMaxIntensity(maxvalue);
      cout<<"\t Max intensity = "<<this->GetMaxIntensity()<<endl;
  }

  //Generate a sample vector.
  //The sample vector is generated using the intensity and the size of each component

  SamplePointerType sample;

  if(this->GetListSample()){
      sample = this->GetListSample();
  }else{
      if(!this->GetUseDynamicThreshold()){
          ScalarImageToIntensitySizeListSamplePointerType runlengthfilter = ScalarImageToIntensitySizeListSampleType::New();
          runlengthfilter->SetMinIntensity(this->GetMinIntensity());
          runlengthfilter->SetMaxIntensity(this->GetMaxIntensity());
          runlengthfilter->SetNumberOfIntensityBins(this->GetNumberOfIntensityBins());
          runlengthfilter->SetMinSize(this->GetMinSize());
          runlengthfilter->SetMaxSize(this->GetMaxSize());
          runlengthfilter->SetBackgroundValue(this->GetBackgroundValue());
          runlengthfilter->SetInput(inputImage);
          runlengthfilter->SetFullConnectivity(this->GetFullConnectivity());
          runlengthfilter->Update();

          sample = const_cast< SampleType* >(runlengthfilter->GetOutput());
      }else{
          ScalarImageToConnectedIntensitySizeListSamplePointerType runlengthfilter = ScalarImageToConnectedIntensitySizeListSampleType::New();
          runlengthfilter->SetMinIntensity(this->GetMinIntensity());
          runlengthfilter->SetMaxIntensity(this->GetMaxIntensity());
          runlengthfilter->SetNumberOfIntensityBins(this->GetNumberOfIntensityBins());
          runlengthfilter->SetMinSize(this->GetMinSize());
          runlengthfilter->SetMaxSize(this->GetMaxSize());
          runlengthfilter->SetBackgroundValue(this->GetBackgroundValue());
          runlengthfilter->SetFullConnectivity(this->GetFullConnectivity());
          runlengthfilter->SetInput(inputImage);
          runlengthfilter->Update();

          sample = const_cast< SampleType* >(runlengthfilter->GetOutput());
      }

      this->SetListSample(sample);
  }

  typedef itk::Statistics::SampleToHistogramFilter< SampleType, HistogramType > SampleToHistogramFilterType;
  typedef typename  SampleToHistogramFilterType::Pointer SampleToHistogramFilterPointerType;
  typename SampleToHistogramFilterType::HistogramSizeType histogramSize(2);
  histogramSize[0] = this->GetNumberOfIntensityBins();
  histogramSize[1] = this->GetNumberOfSizeBins();

  if(0 <= this->GetPercentile() && this->GetPercentile() <= 100){

      double percentile = this->GetPercentile()/100.0;
      cout<<"Computing percentiles: ["<<percentile<<", "<<(1.0 - percentile)<<"]"<<endl;

      //Generate a histogram from the samples in order to compute the percentiles
      SampleToHistogramFilterPointerType sampleToHistogram = SampleToHistogramFilterType::New();
      sampleToHistogram->SetAutoMinimumMaximum(true);
      sampleToHistogram->SetHistogramSize(histogramSize);
      sampleToHistogram->SetInput(sample);
      sampleToHistogram->Update();
      const HistogramType* histogram = sampleToHistogram->GetOutput();

      this->SetMinIntensity(histogram->Quantile(0, percentile));
      this->SetMinSize(histogram->Quantile(1, percentile));
      this->SetMaxIntensity(histogram->Quantile(0, 1.0 - percentile));
      this->SetMaxSize(histogram->Quantile(1, 1.0 - percentile));

      cout<<"\t Min intensity: "<<this->GetMinIntensity()<<endl;
      cout<<"\t Max intensity: "<<this->GetMaxIntensity()<<endl;
      cout<<"\t Min size: "<<this->GetMinSize()<<endl;
      cout<<"\t Max size: "<<this->GetMaxSize()<<endl;

  }else{
      itkExceptionMacro("The percentile must be between [0, 100] -> " << this->GetPercentile())
  }

  cout<<"Computing histogram..."<<endl;
  //Set the bin ranges for the histogram.
  MeasurementVectorType binmin;
  binmin.SetSize(2);

  binmin[0] = this->GetMinIntensity();
  binmin[1] = this->GetMinSize();

  MeasurementVectorType binmax;
  binmax.SetSize(2);

  binmax[0] = this->GetMaxIntensity();
  binmax[1] = this->GetMaxSize();

  const typename SampleType::InstanceIdentifier measurementVectorSize = sample->GetMeasurementVectorSize();

  HistogramPointerType histogram = HistogramType::New();
  histogram->SetMeasurementVectorSize(measurementVectorSize);
  histogram->SetClipBinsAtEnds(false);
  histogram->Initialize(histogramSize, binmin, binmax);

  typename SampleType::ConstIterator iter = sample->Begin();
  typename SampleType::ConstIterator last = sample->End();

  typename SampleType::MeasurementVectorType lvector;
  typename HistogramType::MeasurementVectorType hvector(measurementVectorSize);

  while ( iter != last ){
      lvector = iter.GetMeasurementVector();
      for (unsigned i = 0; i < measurementVectorSize; i++ ){
          hvector[i] = lvector[i];
      }
      histogram->IncreaseFrequencyOfMeasurement(hvector, 1);
      ++iter;
  }

  HistogramType::SizeType size = histogram->GetSize();
  unsigned int totalBins = 1;
  for(unsigned int i = 0; i < size.GetNumberOfElements(); i++){
      totalBins *= histogram->GetSize()[i];
  }

  m_HistogramOutput.clear();

  HistogramType::BinMinContainerType mins = histogram->GetMins();
  HistogramType::BinMaxContainerType maxs = histogram->GetMaxs();

  //Intensity bins
  //m_HistogramOutput << "Total frequency, "<<histogram->GetTotalFrequency()<<endl;
  m_HistogramOutput << "size";
  for(unsigned i = 0; i < mins[0].size(); i++){
      m_HistogramOutput << ",[";
      m_HistogramOutput << mins[0][i];
      m_HistogramOutput << "-";
      m_HistogramOutput << maxs[0][i];
      m_HistogramOutput <<"]";
  }

  unsigned k = 0;
  for(unsigned int i = 0; i < totalBins; ++i){
      if(i % mins[0].size() == 0){
          m_HistogramOutput <<endl;
          m_HistogramOutput << "[";
          m_HistogramOutput << mins[1][k];
          m_HistogramOutput << "-";
          m_HistogramOutput << maxs[1][k];
          m_HistogramOutput <<"]";
          k++;
      }
      m_HistogramOutput <<","<<histogram->GetFrequency(i);
  }
  //Calculate the RLE features of the histogram.
  cout<<"Computing RLE analysis..."<<endl;
  HistogramToRunLengthFeaturesFilterPointerType histogramtorunlength = HistogramToRunLengthFeaturesFilterType::New();
  histogramtorunlength->SetInput(histogram);
  histogramtorunlength->Update();

  this->SetShortRunEmphasis(histogramtorunlength->GetShortRunEmphasis());
  this->SetLongRunEmphasis(histogramtorunlength->GetLongRunEmphasis());
  this->SetGreyLevelNonuniformity(histogramtorunlength->GetGreyLevelNonuniformity());
  this->SetRunLengthNonuniformity(histogramtorunlength->GetRunLengthNonuniformity());
  this->SetLowGreyLevelRunEmphasis(histogramtorunlength->GetLowGreyLevelRunEmphasis());
  this->SetHighGreyLevelRunEmphasis(histogramtorunlength->GetHighGreyLevelRunEmphasis());
  this->SetShortRunLowGreyLevelEmphasis(histogramtorunlength->GetShortRunLowGreyLevelEmphasis());
  this->SetShortRunHighGreyLevelEmphasis(histogramtorunlength->GetShortRunHighGreyLevelEmphasis());
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
