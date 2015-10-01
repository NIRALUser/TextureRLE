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

#include <itkImageToListSampleFilter.h>
using namespace std;

namespace itk
{
namespace  Statistics {

/** \class ScalarImageToIntensitySizeRunLengthFeaturesFilter
 *
 *
 */
template <class TInputImage, class THistogram = itk::Statistics::Histogram< int > >
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

    typedef itk::Statistics::Histogram< int > HistogramType;
    typedef itk::Statistics::HistogramToRunLengthFeaturesFilter< HistogramType > HistogramToRunLengthFeaturesFilterType;
    typedef typename HistogramToRunLengthFeaturesFilterType::MeasurementType MeasurementType;
    typedef typename HistogramToRunLengthFeaturesFilterType::Pointer HistogramToRunLengthFeaturesFilterPointerType;


    typedef itk::MapContainer< string, MeasurementType > MapContainterType;
    typedef typename MapContainterType::Pointer MapContainterPointerType;

    void PrintSelf(std::ostream & os, Indent indent) const ITK_OVERRIDE;

    /** Set/Get the image input of this process object.  */
    using Superclass::SetInput;
    virtual void SetInput(const InputImageType *image);

    virtual void SetInput(unsigned int, const InputImageType *image);

    const InputImageType * GetInput() const;
    const InputImageType * GetInput(unsigned int idx) const;

    const MapContainterType* GetOutput();

    itkSetMacro(InputMask, InputImagePointer)
    itkGetMacro(InputMask, InputImageConstPointer)

    itkSetMacro(MinIntensity, InputImagePixelType)
    itkGetMacro(MinIntensity, InputImagePixelType)

    itkSetMacro(MaxIntensity, InputImagePixelType)
    itkGetMacro(MaxIntensity, InputImagePixelType)

    itkSetMacro(NumberOfIntensityBins, int)
    itkGetMacro(NumberOfIntensityBins, int)

    itkSetMacro(NumberOfSizeBins, int)
    itkGetMacro(NumberOfSizeBins, int)

    itkSetMacro(ComputeMinMaxIntensity, bool)
    itkGetMacro(ComputeMinMaxIntensity, bool)

    itkSetMacro(BackgroundValue, InputImagePixelType)
    itkGetMacro(BackgroundValue, InputImagePixelType)

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

  InputImageConstPointer m_InputMask;

  InputImagePixelType m_MinIntensity;
  InputImagePixelType m_MaxIntensity;
  InputImagePixelType m_BackgroundValue;
  int m_NumberOfIntensityBins;
  int m_NumberOfSizeBins;
  bool m_ComputeMinMaxIntensity;

};

} //end namespace statistics
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkScalarImageToIntensitySizeRunLengthFeaturesFilter.hxx"
#endif

#endif
