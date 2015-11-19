/*=========================================================================

  Program:   TextureRLE
  Module:    $$
  Language:  C++
  Date:      $Date: xxx-xx-xx $
  Version:   $Revision: xxx $

  Copyright (c) University of North Carolina at Chapel Hill (UNC-CH) All Rights Reserved.

  See License.txt or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/
#ifndef __itkScalarImageToConnectedIntensitySizeListSampleFilter_h
#define __itkScalarImageToConnectedIntensitySizeListSampleFilter_h

#include <itkListSample.h>
#include <itkPixelTraits.h>
#include <itkProcessObject.h>
#include <itkDataObjectDecorator.h>
#include <itkListSample.h>
#include <itkImage.h>
#include <itkImageRegionConstIterator.h>

#include <itkImageToListSampleFilter.h>

using namespace std;

namespace itk
{
namespace  Statistics {

/** \class ScalarImageToConnectedIntensitySizeListSampleFilter
 *
 *
 */
template <class TInputImage>
class ScalarImageToConnectedIntensitySizeListSampleFilter
  : public ProcessObject
{
public:
    typedef ScalarImageToConnectedIntensitySizeListSampleFilter           Self;
    typedef ProcessObject              Superclass;
    typedef SmartPointer<Self>             Pointer;
    typedef SmartPointer<const Self>       ConstPointer;

    /** Run-time type information (and related methods). */
    itkTypeMacro(ScalarImageToConnectedIntensitySizeListSampleFilter, ProcessObject)

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

    typedef vector< InputImagePixelType > VectorInputImagePixelType;

    typedef itk::ImageRegionConstIterator< InputImageType > InputImageRegionIteratorType;

    typedef unsigned int  OutputPixelType;
    typedef itk::Image<OutputPixelType, InputImageType::ImageDimension > OutputImageType;
    typedef typename OutputImageType::Pointer      OutputImagePointerType;

    typedef itk::Vector<double, 2> MeasurementVectorType;
    typedef itk::Statistics::ListSample<MeasurementVectorType> SampleType;
    typedef SampleType::Pointer                                SamplePointerType;

    void PrintSelf(std::ostream & os, Indent indent) const ITK_OVERRIDE;

    /** Set/Get the image input of this process object.  */
    using Superclass::SetInput;
    virtual void SetInput(const InputImageType *image);

    virtual void SetInput(unsigned int, const InputImageType *image);

    const InputImageType * GetInput() const;
    const InputImageType * GetInput(unsigned int idx) const;


    const SampleType* GetOutput() const;

    itkSetMacro(MinIntensity, InputImagePixelType)
    itkGetMacro(MinIntensity, InputImagePixelType)

    itkSetMacro(MaxIntensity, InputImagePixelType)
    itkGetMacro(MaxIntensity, InputImagePixelType)

    itkSetMacro(NumberOfIntensityBins, int)
    itkGetMacro(NumberOfIntensityBins, int)

    itkSetMacro(MinSize, double)
    itkGetMacro(MinSize, double)

    itkSetMacro(MaxSize, double)
    itkGetMacro(MaxSize, double)

    itkSetMacro(BackgroundValue, InputImagePixelType)
    itkGetMacro(BackgroundValue, InputImagePixelType)

    itkSetMacro(IntensityBinSize, int)
    itkGetMacro(IntensityBinSize, int)

    itkSetMacro(FullConnectivity, bool)
    itkGetMacro(FullConnectivity, bool)

protected:
    ScalarImageToConnectedIntensitySizeListSampleFilter();
    ~ScalarImageToConnectedIntensitySizeListSampleFilter(){};
    /** This method causes the filter to generate its output. */
    virtual void GenerateData() ITK_OVERRIDE;
    virtual void GenerateInputRequestedRegion() ITK_OVERRIDE;
    virtual void GenerateOutputInformation() ITK_OVERRIDE;


    using Superclass::MakeOutput;
    virtual DataObjectPointer MakeOutput(DataObjectPointerArraySizeType idx) ITK_OVERRIDE;

private:
  ScalarImageToConnectedIntensitySizeListSampleFilter(const Self &); //purposely not implemented
  void operator=(const Self &);                //purposely not implemented

  InputImagePixelType m_MinIntensity;
  InputImagePixelType m_MaxIntensity;
  InputImagePixelType m_BackgroundValue;
  int m_NumberOfIntensityBins;

  double m_MinSize;
  double m_MaxSize;
  int m_IntensityBinSize;

  bool m_FullConnectivity;

};

} //end namespace statistics
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkScalarImageToConnectedIntensitySizeListSampleFilter.hxx"
#endif

#endif
