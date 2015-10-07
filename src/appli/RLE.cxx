
#include "RLECLP.h"


#include <itkImageFileReader.h>

#include <itkScalarImageToRunLengthFeaturesFilter.h>
#include <itkNeighborhood.h>

using namespace std;

 int main (int argc, char * argv[])
  {
  PARSE_ARGS;

  if(inputVolume.compare("") == 0){
      commandLine.getOutput()->usage(commandLine);
      return EXIT_FAILURE;
  }

  cout << "The input volume is: " << inputVolume << endl;

  if(inputMask.compare("") != 0){
      cout << "The input mask is: " << inputMask << endl;
      cout << "The foreground voxel is: " << foreGroundValue << endl;
  }

  cout << "Using min max intensity: " << useMinMaxIntensity <<endl;
  if(useMinMaxIntensity){
      cout << "\t Min intensity: " <<minIntensity<<endl;
      cout << "\t Max intensity: " <<maxIntensity<<endl;
  }

  cout << "Number of intensity bins: " << numberOfIntensityBins <<endl;

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

  typedef itk::Statistics::ScalarImageToRunLengthFeaturesFilter< InputImageType > ScalarImageToRunLengthFeaturesFilterType;
  ScalarImageToRunLengthFeaturesFilterType::Pointer imgtorunlength = ScalarImageToRunLengthFeaturesFilterType::New();
  imgtorunlength->SetInput(imgin);
  if(maskin){
      imgtorunlength->SetMaskImage(maskin);
      imgtorunlength->SetInsidePixelValue(foreGroundValue);
  }

  imgtorunlength->SetNumberOfBinsPerAxis(numberOfIntensityBins);

  if(useMinMaxIntensity){
      imgtorunlength->SetPixelValueMinMax(minIntensity, maxIntensity);
  }


  typedef itk::Neighborhood< InputImageType::PixelType, dimension> NeighborhoodType;
  NeighborhoodType hood;
  hood.SetRadius( 1 );
  unsigned int centerIndex = hood.GetCenterNeighborhoodIndex();
  ScalarImageToRunLengthFeaturesFilterType::OffsetVectorPointer offsets = ScalarImageToRunLengthFeaturesFilterType::OffsetVector::New();

  int ind[] = {0, 1, 2, 3, 4, 5, 7, 8, 9, 10, 11, 12};

  vector<int> offsetindex(ind, ind + (sizeof(ind)/sizeof(*ind)));

  if(connected6){
      int ind[] = {4, 10, 12};
      offsetindex = vector<int>(ind, ind + (sizeof(ind)/sizeof(*ind)));
  }

  if(connected18){
      int ind[] = {1, 3, 4, 5, 7, 9, 10, 11, 12};
      offsetindex = vector<int>(ind, ind + (sizeof(ind)/sizeof(*ind)));
  }

  for( unsigned int d = 0; d < offsetindex.size(); d++ ){
      offsets->push_back( hood.GetOffset( offsetindex[d] ) );
  }
  imgtorunlength->SetOffsets( offsets );

  imgtorunlength->Update();

  typedef itk::VectorContainer<unsigned char, double> VectorContainer;
  VectorContainer::Pointer featuremeans = imgtorunlength->GetFeatureMeans();

  const ScalarImageToRunLengthFeaturesFilterType::FeatureNameVector* featurenames = imgtorunlength->GetRequestedFeatures();

  ostringstream os;

  const string OutputNames[] = {"ShortRunEmphasis",
          "LongRunEmphasis",
          "GreyLevelNonuniformity",
          "RunLengthNonuniformity",
          "LowGreyLevelRunEmphasis",
          "HighGreyLevelRunEmphasis",
          "ShortRunLowGreyLevelEmphasis",
          "ShortRunHighGreyLevelEmphasis",
          "LongRunLowGreyLevelEmphasis",
          "LongRunHighGreyLevelEmphasis"};

  os<<"MetricName, ";
  for(ScalarImageToRunLengthFeaturesFilterType::FeatureNameVector::ConstIterator namesit = featurenames->Begin(); namesit != featurenames->End(); ++namesit){
      os<<OutputNames[namesit->Value()]<<", ";
  }
  os<<endl;

  os<<"Value, "<<endl;
  for(VectorContainer::Iterator fmiterator = featuremeans->Begin(); fmiterator != featuremeans->End(); ++fmiterator){
      os<<fmiterator->Value()<<", ";
  }
  os<<endl;

  os<<"Stdev, "<<endl;
  VectorContainer::Pointer featurestdev = imgtorunlength->GetFeatureStandardDeviations();
  for(VectorContainer::Iterator fmiterator = featurestdev->Begin(); fmiterator != featurestdev->End(); ++fmiterator){
      os<<fmiterator->Value()<<", ";
  }
  os<<endl;

  if(outputRLE.compare("") != 0){
      ofstream outputrlefile;
      outputrlefile.open(outputRLE.c_str());
      if(outputrlefile.is_open()){
          outputrlefile<< os.str()<<endl;
      }else{
          cout<<"Could not create file: "<<outputRLE<<endl;
      }

  }else{
      cout<<os.str()<<endl;
  }

  return EXIT_SUCCESS;

  }
