
#include "CoMatrixTextureToolCLP.h"


#include <itkImageFileReader.h>

#include <itkScalarImageToIntensitySizeRunLengthFeaturesFilter.h>

using namespace std;

 int main (int argc, char * argv[])
  {
  PARSE_ARGS;

  if(inputVolume.compare("") == 0){
      commandLine.getOutput()->usage(commandLine);
      return EXIT_FAILURE;
  }

  std::cout << "The input volume is: " << inputVolume << std::endl;
  std::cout << "The input mask is: " << inputMask << std::endl;
  std::cout << "The background voxel is: " << backGroundValue << std::endl;

  typedef double InputPixelType;
  static const int dimension = 3;
  typedef itk::Image< InputPixelType, dimension> InputImageType;
  typedef InputImageType::Pointer InputImagePointerType;

  typedef itk::ImageFileReader< InputImageType > InputImageFileReaderType;
  typedef InputImageFileReaderType::Pointer InputImageFileReaderPointerType;
  InputImageFileReaderPointerType reader = InputImageFileReaderType::New();
  reader->SetFileName(inputVolume.c_str());
  InputImagePointerType imgin = reader->GetOutput();

  InputImagePointerType maskin = 0;
  if(inputMask.compare("")!=0){
      typedef itk::ImageFileReader< InputImageType > InputImageFileReaderType;
      typedef InputImageFileReaderType::Pointer InputImageFileReaderPointerType;
      InputImageFileReaderPointerType readermask = InputImageFileReaderType::New();

      readermask->SetFileName(inputMask);
      readermask->Update();
      maskin = readermask->GetOutput();
  }

  typedef itk::Statistics::ScalarImageToIntensitySizeRunLengthFeaturesFilter< InputImageType > ScalarImageToRunLengthIntensitySizeFilterType;
  ScalarImageToRunLengthIntensitySizeFilterType::Pointer imgtorunlegth = ScalarImageToRunLengthIntensitySizeFilterType::New();
  imgtorunlegth->SetBackgroundValue(backGroundValue);
  imgtorunlegth->SetInput(imgin);
  imgtorunlegth->SetInputMask(maskin);
  imgtorunlegth->Update();

  const ScalarImageToRunLengthIntensitySizeFilterType::MapContainterType* runlengthfeatures = imgtorunlegth->GetOutput();

//  ScalarImageToRunLengthIntensitySizeFilterType::MapContainterType::Iterator it;

//  for(it = runlengthfeatures->Begin(); it != runlengthfeatures->End(); ++it){
//      cout<<it->first<<": "<<it->second<<endl;
//  }


//  ostringstream os;
//  os<<"ShortRunEmphasis: "<<runlengthfeatures->GetShortRunEmphasis()<<endl;
//  os<<"LongRunEmphasis: "<<runlengthfeatures->GetLongRunEmphasis()<<endl;
//  os<<"GreyLevelNonuniformity: "<<runlengthfeatures->GetGreyLevelNonuniformity()<<endl;
//  os<<"RunLengthNonuniformity: "<<runlengthfeatures->GetRunLengthNonuniformity()<<endl;
//  os<<"LowGreyLevelRunEmphasis: "<<runlengthfeatures->GetLowGreyLevelRunEmphasis()<<endl;
//  os<<"HighGreyLevelRunEmphasis: "<<runlengthfeatures->GetHighGreyLevelRunEmphasis()<<endl;
//  os<<"ShorRunLowGreyLevelEmphasis: "<<runlengthfeatures->GetShortRunLowGreyLevelEmphasis()<<endl;
//  os<<"ShorRunHighGreyLevelEmphasis: "<<runlengthfeatures->GetShortRunHighGreyLevelEmphasis()<<endl;
//  os<<"LongRunLowGreyLevelEmphasis: "<<runlengthfeatures->GetLongRunLowGreyLevelEmphasis()<<endl;
//  os<<"LongRunHighGreyLevelEmphasis"<<runlengthfeatures->GetLongRunHighGreyLevelEmphasis()<<endl;

//  cout<<os.str()<<endl;

  return EXIT_SUCCESS;

  }
