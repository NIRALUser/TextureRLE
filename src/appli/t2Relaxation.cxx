
#include "t2RelaxationCLP.h"


#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkThresholdImageFilter.h>
#include <itkOtsuThresholdImageFilter.h>
#include <itkMaskImageFilter.h>

#include "itkT2RelaxationMapFilter.h"

using namespace std;

typedef unsigned short InputPixelType;
static const int Dimension = 3;
typedef itk::VectorImage< InputPixelType, Dimension> InputImageType;
typedef InputImageType::Pointer InputImagePointerType;

typedef itk::ImageFileReader< InputImageType > InputImageFileReaderType;
typedef InputImageFileReaderType::Pointer InputImageFileReaderPointerType;

typedef double OutputPixelType;
typedef itk::Image< OutputPixelType, Dimension> OutputImageType;
typedef OutputImageType::Pointer OutputImagePointerType;
typedef itk::ImageFileWriter< OutputImageType > OutputImageFileWriterType;
typedef OutputImageFileWriterType::Pointer OutputImageFileWriterPointerType;

typedef unsigned short RescalePixelType;
typedef itk::Image< OutputPixelType, Dimension> RescaleImageType;
typedef RescaleImageType::Pointer RescaleImagePointerType;
typedef itk::ImageFileWriter< RescaleImageType > RescaleImageFileWriterType;
typedef RescaleImageFileWriterType::Pointer RescaleImageFileWriterPointerType;

typedef itk::ThresholdImageFilter<RescaleImageType> ThresholdImageFilterType;
typedef ThresholdImageFilterType::Pointer ThresholdImageFilterPointerType;

typedef itk::RescaleIntensityImageFilter<OutputImageType, RescaleImageType> RescaleIntensityImageFilterType;
typedef RescaleIntensityImageFilterType::Pointer RescaleIntensityImageFilterPointerType;


int main (int argc, char * argv[]){
  PARSE_ARGS;

  if(inputVolume.compare("") == 0){
      commandLine.getOutput()->usage(commandLine);
      return EXIT_FAILURE;
  }

  cout << "The input volume is: " << inputVolume << endl;
  cout << "The echo time is: " << echoTime <<endl;
  cout << "The M0 value is: " << m0 <<endl;

  
  InputImageFileReaderPointerType reader = InputImageFileReaderType::New();
  reader->SetFileName(inputVolume.c_str());
  InputImagePointerType imgin = reader->GetOutput();
  cout << "The number of components per pixel is: " << imgin->GetNumberOfComponentsPerPixel() <<endl;


  typedef itk::T2RelaxationMapFilter<InputImageType, OutputImageType> T2RelaxationMapFilterType;
  T2RelaxationMapFilterType::Pointer t2relaxation = T2RelaxationMapFilterType::New();
  t2relaxation->SetInput(imgin);
  t2relaxation->SetEchoTime(echoTime);
  t2relaxation->SetM0(m0);
  t2relaxation->Update();

  OutputImagePointerType outimg = t2relaxation->GetOutput();

  if(notRescaleIntensity){
    cout << "The output volume is: " << outputRelaxationMap << endl;  
    OutputImageFileWriterPointerType writer = OutputImageFileWriterType::New();
    writer->SetFileName(outputRelaxationMap.c_str());
    writer->SetInput(outimg);
    writer->Update();
  }else{
    cout << "Rescaling intensity and threshold nan values..."<< endl;  
    RescaleIntensityImageFilterPointerType rescaleFilter = RescaleIntensityImageFilterType::New();

    rescaleFilter->SetInput(outimg);
    rescaleFilter->SetOutputMinimum(numeric_limits<RescalePixelType>::min());
    rescaleFilter->SetOutputMaximum(numeric_limits<RescalePixelType>::max());
    rescaleFilter->Update();

    ThresholdImageFilterPointerType threshold = ThresholdImageFilterType::New();
    threshold->SetInput(rescaleFilter->GetOutput());
    threshold->SetOutsideValue(0);
    threshold->ThresholdAbove(numeric_limits<RescalePixelType>::max() - 1);
    threshold->Update();
    RescaleImagePointerType outthresholdimg = threshold->GetOutput();

    if(otsuThreshold){
      cout << "Computing foreground and background using otsu threshold filter."<< endl;
      cout << "The masking value for the otsu filter is: "<< otsuMaskingValue<<endl;
      typedef itk::OtsuThresholdImageFilter <RescaleImageType, RescaleImageType> OtsuThresholdFilterType;
      typedef OtsuThresholdFilterType::Pointer OtsuThresholdFilterPointerType;

      OtsuThresholdFilterPointerType otsuFilter = OtsuThresholdFilterType::New();
      otsuFilter->SetInput(outthresholdimg);
      otsuFilter->SetInsideValue(1);    
      otsuFilter->Update();    

      typedef itk::MaskImageFilter <RescaleImageType, RescaleImageType> MaskImageFilterType;
      typedef MaskImageFilterType::Pointer MaskImageFilterPointerType;


      MaskImageFilterPointerType maskimagefilter = MaskImageFilterType::New();
      maskimagefilter->SetInput(outthresholdimg);
      maskimagefilter->SetMaskImage(otsuFilter->GetOutput());
      maskimagefilter->SetMaskingValue(otsuMaskingValue);
      maskimagefilter->Update();
      outthresholdimg = maskimagefilter->GetOutput();
    }


    cout << "The output volume is: " << outputRelaxationMap << endl;  
    RescaleImageFileWriterPointerType writer = RescaleImageFileWriterType::New();
    writer->SetFileName(outputRelaxationMap.c_str());
    writer->SetInput(outthresholdimg);
    writer->Update();
  }

  


  return EXIT_SUCCESS;

}
