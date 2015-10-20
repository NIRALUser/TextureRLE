/*=========================================================================

  Program:   TextureRLE
  Module:    $$
  Language:  C++
  Date:      $Date: xxx-xx-xx $
  Version:   $Revision: xxx $

  Copyright (c) University of North Carolina at Chapel Hill (UNC-CH) All Rights Reserved.

  See License.txt or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/
#ifndef __itkScalarImageToIntensitySizeRunLengthFeaturesFilter_h
#define __itkScalarImageToIntensitySizeRunLengthFeaturesFilter_h

#include <itkListSample.h>
#include <itkPixelTraits.h>
#include <itkProcessObject.h>

#include <itkImage.h>
#include <itkImageRegionConstIterator.h>
#include <itkHistogram.h>
#include <itkHistogramToRunLengthFeaturesFilter.h>
#include <itkMapContainer.h>

#include "itkScalarImageToIntensitySizeListSampleFilter.h"
#include "itkScalarImageToConnectedIntensitySizeListSampleFilter.h"
#include <itkSampleToHistogramFilter.h>

using namespace std;

namespace itk
{
namespace  Statistics {

/** \class ScalarImageToIntensitySizeRunLengthFeaturesFilter
 *
 *
 */
template <class TInputImage >
class ScalarImageToIntensitySizeRunLengthFeaturesFilter
  : public ProcessObject
{
public:
    typedef ScalarImageToIntensitySizeRunLengthFeaturesFilter           Self;
    typedef ProcessObject              Superclass;
    typedef SmartPointer<Self>             Pointer;
    typedef SmartPointer<const Self>       ConstPointer;

    /** Run-time type information (and related methods). */
    itkTypeMacro(ScalarImageToIntensitySizeRunLengthFeaturesFilter, ProcessObject)

    /** Method for creation through the object factory. */
    itkNewMacro(Self)

    typedef TInputImage                           InputImageType;
    typedef typename InputImageType::Pointer      InputImagePointer;
    typedef typename InputImageType::ConstPointer InputImageConstPointer;
    typedef typename InputImageType::RegionType   InputImageRegionType;
    typedef typename InputImageType::PixelType    InputImagePixelType;
    typedef typename InputImageType::IndexType    IndexType;
    typedef typename std::vector<IndexType>       SeedContainerType;
    typedef typename InputImageType::SizeType     SizeType;

    typedef itk::ImageRegionConstIterator< InputImageType > InputImageRegionIteratorType;

    typedef itk::Statistics::Histogram< double > HistogramType;
    typedef itk::Statistics::HistogramToRunLengthFeaturesFilter< HistogramType > HistogramToRunLengthFeaturesFilterType;
    typedef typename HistogramToRunLengthFeaturesFilterType::Pointer HistogramToRunLengthFeaturesFilterPointerType;

    typedef typename HistogramToRunLengthFeaturesFilterType::MeasurementType MeasurementType;
    typedef typename HistogramToRunLengthFeaturesFilterType::MeasurementVectorType MeasurementVectorType;


    typedef itk::Statistics::ScalarImageToIntensitySizeListSampleFilter< InputImageType > ScalarImageToIntensitySizeListSampleType;
    typedef typename ScalarImageToIntensitySizeListSampleType::Pointer ScalarImageToIntensitySizeListSamplePointerType;
    typedef typename  ScalarImageToIntensitySizeListSampleType::SampleType SampleType;
    typedef typename SampleType::Pointer SamplePointerType;

    typedef itk::Statistics::ScalarImageToConnectedIntensitySizeListSampleFilter< InputImageType > ScalarImageToConnectedIntensitySizeListSampleType;
    typedef typename ScalarImageToConnectedIntensitySizeListSampleType::Pointer ScalarImageToConnectedIntensitySizeListSamplePointerType;


    void PrintSelf(std::ostream & os, Indent indent) const ITK_OVERRIDE;

    /** Set/Get the image input of this process object.  */
    using Superclass::SetInput;
    virtual void SetInput(const InputImageType *image);

    virtual void SetInput(unsigned int, const InputImageType *image);

    const InputImageType * GetInput() const;
    const InputImageType * GetInput(unsigned int idx) const;

    itkSetMacro(BackgroundValue, InputImagePixelType)
    itkGetMacro(BackgroundValue, InputImagePixelType)

    itkSetMacro(UseMinMaxIntensity, bool)
    itkGetMacro(UseMinMaxIntensity, bool)
    itkSetMacro(MinIntensity, InputImagePixelType)
    itkGetMacro(MinIntensity, InputImagePixelType)
    itkSetMacro(MaxIntensity, InputImagePixelType)
    itkGetMacro(MaxIntensity, InputImagePixelType)
    itkSetMacro(NumberOfIntensityBins, int)
    itkGetMacro(NumberOfIntensityBins, int)

    itkSetMacro(UseMinMaxSize, bool)
    itkGetMacro(UseMinMaxSize, bool)
    itkSetMacro(MinSize, int)
    itkGetMacro(MinSize, int)
    itkSetMacro(MaxSize, int)
    itkGetMacro(MaxSize, int)
    itkSetMacro(NumberOfSizeBins, int)
    itkGetMacro(NumberOfSizeBins, int)

    itkSetMacro(UseDynamicThreshold, bool)
    itkGetMacro(UseDynamicThreshold, bool)

    itkSetMacro(FullConnectivity, bool)
    itkGetMacro(FullConnectivity, bool)

    itkSetMacro(ListSample, SamplePointerType)
    itkGetMacro(ListSample, SamplePointerType)

    //OUTPUTS

    itkGetMacro(ShortRunEmphasis, MeasurementType)
    itkSetMacro(ShortRunEmphasis, MeasurementType)

    itkGetMacro(LongRunEmphasis, MeasurementType)
    itkSetMacro(LongRunEmphasis, MeasurementType)

    itkGetMacro(GreyLevelNonuniformity, MeasurementType)
    itkSetMacro(GreyLevelNonuniformity, MeasurementType)

    itkGetMacro(RunLengthNonuniformity, MeasurementType)
    itkSetMacro(RunLengthNonuniformity, MeasurementType)

    itkGetMacro(LowGreyLevelRunEmphasis, MeasurementType)
    itkSetMacro(LowGreyLevelRunEmphasis, MeasurementType)

    itkGetMacro(HighGreyLevelRunEmphasis, MeasurementType)
    itkSetMacro(HighGreyLevelRunEmphasis, MeasurementType)

    itkGetMacro(ShortRunLowGreyLevelEmphasis, MeasurementType)
    itkSetMacro(ShortRunLowGreyLevelEmphasis, MeasurementType)

    itkGetMacro(ShortRunHighGreyLevelEmphasis, MeasurementType)
    itkSetMacro(ShortRunHighGreyLevelEmphasis, MeasurementType)

    itkGetMacro(LongRunLowGreyLevelEmphasis, MeasurementType)
    itkSetMacro(LongRunLowGreyLevelEmphasis, MeasurementType)

    itkGetMacro(LongRunHighGreyLevelEmphasis, MeasurementType)
    itkSetMacro(LongRunHighGreyLevelEmphasis, MeasurementType)

    ostream* GetHistogramOutput(){
        return &m_HistogramOutput;
    }

    void SetHistogramOutput(ostream* os){
        m_HistogramOutput = *os;
    }

protected:
    ScalarImageToIntensitySizeRunLengthFeaturesFilter();
    ~ScalarImageToIntensitySizeRunLengthFeaturesFilter(){};


    /** Standard itk::ProcessObject subclass method. */
    typedef DataObject::Pointer                           DataObjectPointer;
    typedef ProcessObject::DataObjectPointerArraySizeType DataObjectPointerArraySizeType;
    using Superclass::MakeOutput;
    virtual DataObjectPointer MakeOutput(DataObjectPointerArraySizeType idx) ITK_OVERRIDE;

    /** This method causes the filter to generate its output. */
    virtual void GenerateData() ITK_OVERRIDE;
    virtual void GenerateInputRequestedRegion() ITK_OVERRIDE;

private:
  ScalarImageToIntensitySizeRunLengthFeaturesFilter(const Self &); //purposely not implemented
  void operator=(const Self &);                //purposely not implemented

  InputImagePixelType m_MinIntensity;
  InputImagePixelType m_MaxIntensity;
  bool m_UseMinMaxIntensity;

  int m_MinSize;
  int m_MaxSize;
  bool m_UseMinMaxSize;
  bool m_UseDynamicThreshold;
  bool m_FullConnectivity;

  InputImagePixelType m_BackgroundValue;
  int m_NumberOfIntensityBins;
  int m_NumberOfSizeBins;

  MeasurementType m_ShortRunEmphasis;
  MeasurementType m_LongRunEmphasis;
  MeasurementType m_GreyLevelNonuniformity;
  MeasurementType m_RunLengthNonuniformity;
  MeasurementType m_LowGreyLevelRunEmphasis;
  MeasurementType m_HighGreyLevelRunEmphasis;
  MeasurementType m_ShortRunLowGreyLevelEmphasis;
  MeasurementType m_ShortRunHighGreyLevelEmphasis;
  MeasurementType m_LongRunLowGreyLevelEmphasis;
  MeasurementType m_LongRunHighGreyLevelEmphasis;

  ostringstream m_HistogramOutput;

  SamplePointerType m_ListSample;

};

} //end namespace statistics
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkScalarImageToIntensitySizeRunLengthFeaturesFilter.hxx"
#endif

#endif
