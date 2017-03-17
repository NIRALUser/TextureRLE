
#include "t2CalibrationCLP.h"

#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkLabelStatisticsImageFilter.h>
#include <itkLabelImageToLabelMapFilter.h>
#include <itkImageRegionIterator.h>
#include <itkRescaleIntensityImageFilter.h>

using namespace std;

typedef unsigned short InputPixelType;
static const int Dimension = 3;
typedef itk::Image< InputPixelType, Dimension> InputImageType;
typedef InputImageType::Pointer InputImagePointerType;

typedef itk::ImageFileReader< InputImageType > InputImageFileReaderType;
typedef InputImageFileReaderType::Pointer InputImageFileReaderPointerType;

typedef itk::LabelStatisticsImageFilter<InputImageType, InputImageType> LabelStatisticsImageFilterType;
typedef LabelStatisticsImageFilterType::Pointer LabelStatisticsImageFilterPointerType;

typedef itk::LabelImageToLabelMapFilter<InputImageType> LabelImageToLabelMapFilterType;
typedef LabelImageToLabelMapFilterType::Pointer LabelImageToLabelMapFilterPointerType;

typedef itk::ImageRegionIterator<InputImageType> ImageRegionIteratorType;

typedef double OutputPixelType;
typedef itk::Image< OutputPixelType, Dimension> OutputImageType;
typedef OutputImageType::Pointer OutputImagePointerType;
typedef itk::ImageFileWriter< OutputImageType > OutputImageFileWriterType;



typedef OutputImageFileWriterType::Pointer OutputImageFileWriterPointerType;
typedef itk::ImageRegionIterator<OutputImageType> OutputRegionIteratorType;

typedef itk::RescaleIntensityImageFilter< OutputImageType, OutputImageType > RescaleIntensityImageFilterType;
typedef RescaleIntensityImageFilterType::Pointer RescaleIntensityImageFilterPointerType;

/*Wang et. al. 2013 A computerized MRI biomarker quantification scheme for a canine model of Duchenne muscular dystrophy
Intensity calibration:

  Ic(x, y, z) = 900 / (m1 – m2) × I (x, y, z) + (100 × m1 – 1000 × m2) / (m1 – m2)

where I (x, y, z) is the intensity at voxel (x, y, z) in the non-calibrated T2w or T2fs 
and m1 and m2 are the mean intensities of the subcutaneous fat regions in non-calibrated T2w and T2fs, respectively.
*/

int main (int argc, char * argv[]){
  PARSE_ARGS;

  if(inputM1Filename.compare("") == 0 || inputM2Filename.compare("") == 0 || inputMaskFilename.compare("") == 0){
      commandLine.getOutput()->usage(commandLine);
      return EXIT_FAILURE;
  }

  cout << "The input 1 is: " << inputM1Filename << endl;
  cout << "The input 2 is: " << inputM2Filename << endl;
  cout << "The mask is: " << inputMaskFilename << endl;
  

  
  InputImageFileReaderPointerType reader1 = InputImageFileReaderType::New();
  reader1->SetFileName(inputM1Filename.c_str());
  InputImagePointerType imageM1 = reader1->GetOutput();

  InputImageFileReaderPointerType reader2 = InputImageFileReaderType::New();
  reader2->SetFileName(inputM2Filename.c_str());
  InputImagePointerType imageM2 = reader2->GetOutput();

  InputImageFileReaderPointerType reader3 = InputImageFileReaderType::New();
  reader3->SetFileName(inputMaskFilename.c_str());
  InputImagePointerType imageMask = reader3->GetOutput();

  LabelImageToLabelMapFilterPointerType labelImageToLabelMapFilter = LabelImageToLabelMapFilterType::New();
  labelImageToLabelMapFilter->SetInput(imageMask);
  labelImageToLabelMapFilter->Update();

  LabelImageToLabelMapFilterType::OutputImageType::Pointer labelMap = labelImageToLabelMapFilter->GetOutput();

  cout<<"Number of labels in the image = "<<labelMap->GetNumberOfLabelObjects()<<endl;

  InputPixelType label = 0;
  if(labelValue == -1){
    if(labelMap->GetNumberOfLabelObjects() > 1){
      cout<<"WARNING!! the image mask provided has more than 2 label. Using the first label to compute m1 and m2..."<<endl;
    }
    label = labelMap->GetNthLabelObject(1)->GetLabel();
  }else{
    label = labelValue;
  }
  cout<<"Using label: "<<label<<endl;
   

  LabelStatisticsImageFilterPointerType image1stats = LabelStatisticsImageFilterType::New();
  image1stats->SetInput(imageM1);
  image1stats->SetLabelInput(imageMask);
  image1stats->Update();

  LabelStatisticsImageFilterPointerType image2stats = LabelStatisticsImageFilterType::New();
  image2stats->SetInput(imageM2);
  image2stats->SetLabelInput(imageMask);
  image2stats->Update();

  double m1 = image1stats->GetMean(label);
  double m2 = image2stats->GetMean(label);


  OutputImagePointerType imageM1Cali = OutputImageType::New();
  imageM1Cali->SetSpacing(imageM1->GetSpacing());
  imageM1Cali->SetOrigin(imageM1->GetOrigin());
  imageM1Cali->SetDirection(imageM1->GetDirection());
  imageM1Cali->SetRegions(imageM1->GetLargestPossibleRegion());
  imageM1Cali->Allocate(true);

  OutputImagePointerType imageM2Cali = OutputImageType::New();
  imageM2Cali->SetSpacing(imageM2->GetSpacing());
  imageM2Cali->SetOrigin(imageM2->GetOrigin());
  imageM2Cali->SetDirection(imageM2->GetDirection());
  imageM2Cali->SetRegions(imageM2->GetLargestPossibleRegion());
  imageM2Cali->Allocate(true);


  ImageRegionIteratorType m1it = ImageRegionIteratorType(imageM1, imageM1->GetLargestPossibleRegion());
  ImageRegionIteratorType m2it = ImageRegionIteratorType(imageM2, imageM2->GetLargestPossibleRegion());

  OutputRegionIteratorType m1outit = OutputRegionIteratorType(imageM1Cali, imageM1Cali->GetLargestPossibleRegion());
  OutputRegionIteratorType m2outit = OutputRegionIteratorType(imageM2Cali, imageM2Cali->GetLargestPossibleRegion());

  m1it.GoToBegin();
  m2it.GoToBegin();

  m1outit.GoToBegin();
  m2outit.GoToBegin();

  cout<<"m1= "<<m1<<endl;
  cout<<"m2= "<<m2<<endl;

  cout<<"Calibrating image 1"<<endl;

  while(!m1it.IsAtEnd()){
    InputPixelType pix = m1it.Get();

    double cali = 0;

    if(m1 - m2 != 0){
      cali = (900 * pix + 100 * m1 - 1000 * m2)/(m1 - m2);
    }

    m1outit.Set((OutputPixelType) cali);

    ++m1outit;
    ++m1it;
  }


  cout<<"Calibrating image 2"<<endl;
  while(!m2it.IsAtEnd()){
    InputPixelType pix = m2it.Get();

    double cali = 0;

    if(m1 - m2 != 0){
      cali = (900 * pix + 100 * m1 - 1000 * m2)/(m1 - m2);
    }

    m2outit.Set((OutputPixelType) cali);

    ++m2it;
    ++m2outit;
  }


  cout<<"Rescaling image: between: 0-"<<numeric_limits<InputPixelType>::max()<<endl;
  RescaleIntensityImageFilterPointerType rescaleimage1 = RescaleIntensityImageFilterType::New();
  rescaleimage1->SetOutputMinimum(0);
  rescaleimage1->SetOutputMaximum(numeric_limits<InputPixelType>::max());
  rescaleimage1->SetInput(imageM1Cali);
  rescaleimage1->Update();


  cout << "The output calibrated volume is: " << outputM1Filename << endl;  
  OutputImageFileWriterPointerType writer1 = OutputImageFileWriterType::New();
  writer1->SetFileName(outputM1Filename.c_str());
  writer1->SetInput(rescaleimage1->GetOutput());
  writer1->Update();

  cout<<"Rescaling image: between: 0-"<<numeric_limits<InputPixelType>::max()<<endl;
  RescaleIntensityImageFilterPointerType rescaleimage2 = RescaleIntensityImageFilterType::New();
  rescaleimage2->SetOutputMinimum(0);
  rescaleimage2->SetOutputMaximum(numeric_limits<InputPixelType>::max());
  rescaleimage2->SetInput(imageM2Cali);
  rescaleimage2->Update();

  cout << "The output calibrated volume is: " << outputM2Filename << endl;  
  OutputImageFileWriterPointerType writer2 = OutputImageFileWriterType::New();
  writer2->SetFileName(outputM2Filename.c_str());
  writer2->SetInput(rescaleimage2->GetOutput());
  writer2->Update();

  return EXIT_SUCCESS;

}
