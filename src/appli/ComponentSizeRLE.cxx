
#include "ComponentSizeRLECLP.h"


#include <itkImageFileReader.h>

#include "itkScalarImageToIntensitySizeRunLengthFeaturesFilter.h"

using namespace std;

 int main (int argc, char * argv[])
  {
  PARSE_ARGS;

  if(inputVolume.compare("") == 0){
      commandLine.getOutput()->usage(commandLine);
      return EXIT_FAILURE;
  }

  cout << "The input volume is: " << inputVolume << endl;
  cout << "The input mask is: " << inputMask << endl;
  cout << "The background voxel is: " << backGroundValue << endl;
  cout << "Using min max intensity: " << useMinMaxIntensity <<endl;
  if(useMinMaxIntensity){
      cout << "\t Min intensity: " <<minIntensity<<endl;
      cout << "\t Max intensity: " <<maxIntensity<<endl;
  }

  cout << "Number of intensity bins: " << numberOfIntensityBins <<endl;

  cout << "Using min max size: " << useMinMaxSize <<endl;
  if(useMinMaxSize){
      cout << "\t Minimum size: " <<minSize<<endl;
      cout << "\t Maximum size: " <<maxSize<<endl;
  }
  cout << "Number of size bins: "<< numberOfSizeBins <<endl;
  cout << "Using dynamic threshold: "<< useDynamicThreshold <<endl;



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

  imgtorunlegth->SetNumberOfIntensityBins(numberOfIntensityBins);
  if(useMinMaxIntensity){
      imgtorunlegth->SetUseMinMaxIntensity(true);
      imgtorunlegth->SetMinIntensity(minIntensity);
      imgtorunlegth->SetMaxIntensity(maxIntensity);
  }

  imgtorunlegth->SetNumberOfSizeBins(numberOfSizeBins);
  if(useMinMaxSize){
      imgtorunlegth->SetUseMinMaxSize(true);
      imgtorunlegth->SetMinSize(minSize);
      if(maxSize != -1){
          imgtorunlegth->SetMaxSize(maxSize);
      }
  }

  imgtorunlegth->SetUseDynamicThreshold(useDynamicThreshold);
  imgtorunlegth->Update();

  ostringstream* outhisto = (ostringstream*)imgtorunlegth->GetHistogramOutput();

  if(outputHistogramCSV.compare("") != 0){
      ofstream outhistofile(outputHistogramCSV.c_str());
      if(outhistofile.is_open()){
          outhistofile << outhisto->str();
          outhistofile.close();
      }else{
          cerr<<"Could not create file: "<<outputHistogramCSV<<endl;
      }
  }else{
      cout<<outhisto->str()<<endl;
  }

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

  vector<string> OutputNamesVector(OutputNames, OutputNames + (sizeof(OutputNames)/sizeof(*OutputNames)));
  for(unsigned i = 0; i < OutputNamesVector.size(); i++){
      os<<OutputNames[i]<<", ";
  }
  os<<endl;

  os<<"Value, ";
  os<<imgtorunlegth->GetShortRunEmphasis()<<", ";
  os<<imgtorunlegth->GetLongRunEmphasis()<<", ";
  os<<imgtorunlegth->GetGreyLevelNonuniformity()<<", ";
  os<<imgtorunlegth->GetRunLengthNonuniformity()<<", ";
  os<<imgtorunlegth->GetLowGreyLevelRunEmphasis()<<", ";
  os<<imgtorunlegth->GetHighGreyLevelRunEmphasis()<<", ";
  os<<imgtorunlegth->GetShortRunLowGreyLevelEmphasis()<<", ";
  os<<imgtorunlegth->GetShortRunHighGreyLevelEmphasis()<<", ";
  os<<imgtorunlegth->GetLongRunLowGreyLevelEmphasis()<<", ";
  os<<imgtorunlegth->GetLongRunHighGreyLevelEmphasis()<<endl;


  if(outputRLE.compare("") != 0){
      ofstream outputrlefile;
      if(outputRLE.compare(outputHistogramCSV) == 0){
          outputrlefile.open(outputRLE.c_str(), ofstream::out|ofstream::app);
      }else{
          outputrlefile.open(outputRLE.c_str());
      }
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
