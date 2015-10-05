/*=========================================================================

  Program:   Texture Analysis
  Module:    $$
  Language:  C++
  Date:      $Date: xxx-xx-xx $
  Version:   $Revision: xxx $

  Copyright (c) University of North Carolina at Chapel Hill (UNC-CH) All Rights Reserved.

  See License.txt or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/

#ifndef __itkScalarImageToIntensitySizeListSampleFilter_hxx
#define __itkScalarImageToIntensitySizeListSampleFilter_hxx


#include <itkLabelStatisticsImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkConnectedComponentImageFilter.h>

#include "itkScalarImageToIntensitySizeListSampleFilter.h"

#include "itkImageFileWriter.h"

namespace itk
{
namespace Statistics {
/**
 * Constructor
 */
template< typename TInputImage >
ScalarImageToIntensitySizeListSampleFilter< TInputImage >
::ScalarImageToIntensitySizeListSampleFilter()
{
    m_MinIntensity = 0;
    m_MaxIntensity = 0;
    m_NumberOfIntensityBins = 1;

    m_MinSize = 1;
    m_MaxSize = numeric_limits<int>::max();
    m_BackgroundValue = 0;

    this->SetNumberOfRequiredInputs(1);
    this->SetNumberOfRequiredOutputs(1);
    this->ProcessObject::SetNthOutput( 0, this->MakeOutput(0) );
}

/**
 *
 */
template< typename TInputImage >
void
ScalarImageToIntensitySizeListSampleFilter< TInputImage >
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
ScalarImageToIntensitySizeListSampleFilter< TInputImage >
::SetInput(unsigned int index, const TInputImage *image)
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput( index,
                                    const_cast< TInputImage * >( image ) );
}

/**
 *
 */
template< typename TInputImage >
const typename ScalarImageToIntensitySizeListSampleFilter< TInputImage >::InputImageType *
ScalarImageToIntensitySizeListSampleFilter< TInputImage >::GetInput(void) const
{
  return itkDynamicCastInDebugMode< const TInputImage * >( this->GetPrimaryInput() );
}

/**
 *
 */
template< typename TInputImage >
const typename ScalarImageToIntensitySizeListSampleFilter< TInputImage >::InputImageType *
ScalarImageToIntensitySizeListSampleFilter< TInputImage >
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

template< typename TInputImage >
typename ScalarImageToIntensitySizeListSampleFilter< TInputImage >::DataObjectPointer
ScalarImageToIntensitySizeListSampleFilter< TInputImage >::MakeOutput(DataObjectPointerArraySizeType itkNotUsed(idx)){
    return SampleType::New().GetPointer();
}

template< typename TImage >
void
ScalarImageToIntensitySizeListSampleFilter< TImage >
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method. this should
  // copy the output requested region to the input requested region
  Superclass::GenerateInputRequestedRegion();
}

template< typename TInputImage >
void
ScalarImageToIntensitySizeListSampleFilter< TInputImage >::GenerateOutputInformation(){
    Superclass::GenerateOutputInformation();

    SampleType *output =
      static_cast< SampleType * >( this->ProcessObject::GetOutput(0) );
    output->SetMeasurementVectorSize( 2 );
}

template< typename TImage >
const typename ScalarImageToIntensitySizeListSampleFilter< TImage >::SampleType *
ScalarImageToIntensitySizeListSampleFilter< TImage >
::GetOutput() const
{
  const SampleType *output =
    static_cast< const SampleType * >( this->ProcessObject::GetOutput(0) );

  return output;
}

template< typename TInputImage >
void
ScalarImageToIntensitySizeListSampleFilter< TInputImage >
::GenerateData()
{

    typedef itk::BinaryThresholdImageFilter< InputImageType, OutputImageType > ThresholdImageFilterType;
    typedef typename ThresholdImageFilterType::Pointer ThresholdImageFilterPointerType;

    typedef itk::ConnectedComponentImageFilter <OutputImageType, OutputImageType > ConnectedComponentImageFilterType;
    typedef typename ConnectedComponentImageFilterType::Pointer                           ConnectedComponentImageFilterPointerType;

    typedef itk::LabelStatisticsImageFilter< OutputImageType, OutputImageType >  LabelStatisticsImageFilterType;
    typedef typename LabelStatisticsImageFilterType::Pointer                    LabelStatisticsImageFilterPointerType;

  InputImageConstPointer inputImage = this->GetInput();

  SampleType *output = static_cast< SampleType * >( this->ProcessObject::GetOutput(0) );

  //Calculate the step size given the maximum intensity, minimum and the number of desired intensity bins.
  this->SetIntensityBinSize(round((this->GetMaxIntensity() - this->GetMinIntensity())/this->GetNumberOfIntensityBins()));
  int binsize = this->GetIntensityBinSize();

  if(binsize <= 0){
      cerr<<"MaxIntesity: "<<this->GetMaxIntensity()<<endl;
      cerr<<"MinIntesity: "<<this->GetMinIntensity()<<endl;
      cerr<<"NumberOfIntensityBins: "<<this->GetNumberOfIntensityBins()<<endl;
      cerr<<"Binsize = (maxIntensity - minIntensity)/numberOfBins"<<endl;
      itkExceptionMacro("ERROR: Intensity bin size is 0. Please reduce the number of intensity bins.")
  }

  InputImagePixelType minintensity = this->GetMinIntensity();
  InputImagePixelType maxintensity = this->GetMinIntensity();

  for(int i = 0; i < this->GetNumberOfIntensityBins() && maxintensity < this->GetMaxIntensity(); i++){

      minintensity = maxintensity;
      maxintensity = minintensity + binsize;

      if(minintensity == this->GetBackgroundValue()){
          minintensity = maxintensity;
      }

      //Threshold the image using the threshold values
      ThresholdImageFilterPointerType thresholdfilter = ThresholdImageFilterType::New();

      thresholdfilter->SetInput(inputImage);
      thresholdfilter->SetLowerThreshold(minintensity);
      thresholdfilter->SetUpperThreshold(maxintensity);
      thresholdfilter->SetOutsideValue(0);
      thresholdfilter->SetInsideValue(255);
      thresholdfilter->Update();
      OutputImagePointerType imgthresh = thresholdfilter->GetOutput();

      //Calculate the connected components and label each one of them with a unique value
      typename ConnectedComponentImageFilterType::Pointer connectedcomponents = ConnectedComponentImageFilterType::New();
      connectedcomponents->SetInput(imgthresh);

      OutputImagePointerType imglabel = connectedcomponents->GetOutput();

      typedef itk::ImageFileWriter< OutputImageType > ImageFileWriterType;
      typename ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
      writer->SetInput(imglabel);
      writer->SetFileName("temp.nrrd");
      writer->Update();

      //Compute statistics for each label
      LabelStatisticsImageFilterPointerType labelstatistics = LabelStatisticsImageFilterType::New();
      labelstatistics->SetLabelInput(imglabel);
      labelstatistics->SetInput(imglabel);
      labelstatistics->Update();

      typedef typename LabelStatisticsImageFilterType::ValidLabelValuesContainerType ValidLabelValuesType;
      typedef typename ValidLabelValuesType::const_iterator ValidLabelsIteratorType;

      typedef typename LabelStatisticsImageFilterType::LabelPixelType                LabelPixelType;

      ValidLabelValuesType validlabels = labelstatistics->GetValidLabelValues();

      //Get each label and insert it into the list sample
      for(ValidLabelsIteratorType vIt = validlabels.begin(); vIt != validlabels.end(); ++vIt){

          LabelPixelType labelValue = *vIt;
        if ( labelstatistics->HasLabel(labelValue) && labelValue != 0){

            double size = labelstatistics->GetCount( labelValue );
            if(this->GetMinSize() <= size && size <= this->GetMaxSize()){
                double bincenter = (minintensity + maxintensity)/2.0;

                MeasurementVectorType mv;
                mv[0] = bincenter;
                mv[1] = size;
                output->PushBack(mv);
            }

//            std::cout << "min: " << labelstatistics->GetMinimum( labelValue ) << std::endl;
//            std::cout << "max: " << labelstatistics->GetMaximum( labelValue ) << std::endl;
//            std::cout << "median: " << labelstatistics->GetMedian( labelValue ) << std::endl;
//            std::cout << "mean: " << labelstatistics->GetMean( labelValue ) << std::endl;
//            std::cout << "sigma: " << labelstatistics->GetSigma( labelValue ) << std::endl;
//            std::cout << "variance: " << labelstatistics->GetVariance( labelValue ) << std::endl;
//            std::cout << "sum: " << labelstatistics->GetSum( labelValue ) << std::endl;
//            std::cout << "count: " << labelstatistics->GetCount( labelValue ) << std::endl;
//            std::cout << "region: " << labelstatistics->GetRegion( labelValue ) << std::endl;
//            std::cout << std::endl << std::endl;
        }
      }

  }

}

template< typename TInputImage >
void
ScalarImageToIntensitySizeListSampleFilter< TInputImage >
::PrintSelf(std::ostream & os, Indent indent) const
{
    Superclass::PrintSelf(os, indent);
    os << indent << "ScalarImageToIntensityListSampleFilter: "<< std::endl;
    os << indent << "SamplePointerType: "<< this->GetOutput() <<endl;
    os << indent << "MinIntensity: "<<m_MinIntensity <<endl;
    os << indent << "MaxIntensity: "<<m_MaxIntensity<<endl;
    os << indent << "BackgroundValue: "<< m_BackgroundValue <<endl;
    os << indent << "NumberOfIntensityBins: "<< m_NumberOfIntensityBins <<endl;

    os << indent << "MinSize: "<< m_MinSize <<endl;
    os << indent << "MaxSize: "<< m_MaxSize <<endl;
}

}// namespace statistics
}// namespace itk

#endif
