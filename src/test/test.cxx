#include "itkSampleToHistogramFilter.h"
#include "itkListSample.h"
#include "itkHistogram.h"
 
typedef itk::Vector<unsigned char, 1> MeasurementVectorType;
typedef itk::Statistics::ListSample< MeasurementVectorType > SampleType ;
 
typedef itk::Statistics::Histogram< float,
        itk::Statistics::DenseFrequencyContainer2 > HistogramType;
 
void CreateSample(SampleType::Pointer image);
 
int main(int, char *[])
{
  SampleType::Pointer sample = SampleType::New();
  CreateSample(sample);
 
  typedef itk::Statistics::SampleToHistogramFilter<SampleType, HistogramType> SampleToHistogramFilterType;
  SampleToHistogramFilterType::Pointer sampleToHistogramFilter =
    SampleToHistogramFilterType::New();
  sampleToHistogramFilter->SetInput(sample);
 
  SampleToHistogramFilterType::HistogramSizeType histogramSize(1);
  histogramSize.Fill(10);
  sampleToHistogramFilter->SetHistogramSize(histogramSize);
 
  sampleToHistogramFilter->Update();
 
  const HistogramType* histogram = sampleToHistogramFilter->GetOutput();
  std::cout << "Histogram vector size: " << histogram->GetMeasurementVectorSize() << std::endl;
 
 
  for(unsigned int i = 0; i < histogram->GetSize()[0]; i++)
    {
    std::cout << "Frequency of " << i << " : (" << histogram->GetBinMin(0, i) << " to " << histogram->GetBinMax(0, i) << ") = " << histogram->GetFrequency(i) << std::endl;
    }
 
  std::cout << "Total count " << histogram->GetTotalFrequency() << std::endl;
 
  return EXIT_SUCCESS;
}
 
void CreateSample(SampleType::Pointer sample)
{
  MeasurementVectorType mv ;
  mv[0] = 1.0 ;
  sample->PushBack(mv) ;
 
  mv[0] = 1.0 ;
  sample->PushBack(mv) ;
 
  mv[0] = 2.0 ;
  sample->PushBack(mv) ;
 
}