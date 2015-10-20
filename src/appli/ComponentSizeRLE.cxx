
#include "ComponentSizeRLECLP.h"


#include <itkImageFileReader.h>
#include <itkLabelImageToLabelMapFilter.h>
#include <itkLabelMapMaskImageFilter.h>
#include <itkMaskImageFilter.h>

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
  if(inputMask.compare("") != 0){
      cout << "The input mask is: " << inputMask << endl;
  }
  if(inputLabelMap.compare("") != 0){
      cout << "Input label map: "<< inputLabelMap << endl;
      cout << "Compute minimum and maximum intensity for each label: " << computeMinMaxIntensityPerLabel <<endl;
      cout << "Compute minimum and maximum component size for each label: " << computeMinMaxSizePerLabel <<endl;
  }

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

  typedef itk::ImageRegionIterator< InputImageType > ImageRegionIteratorType;

  typedef itk::ImageFileReader< InputImageType > InputImageFileReaderType;
  typedef InputImageFileReaderType::Pointer InputImageFileReaderPointerType;
  InputImageFileReaderPointerType reader = InputImageFileReaderType::New();
  reader->SetFileName(inputVolume.c_str());
  InputImagePointerType imgin = reader->GetOutput();

  InputImagePointerType maskin = 0;

  typedef itk::LabelObject< InputPixelType, dimension > LabelObjectType;
  typedef itk::LabelMap< LabelObjectType > LabelImageType;
  typedef itk::LabelImageToLabelMapFilter< InputImageType, LabelImageType > LabelImageMapFilterType;
  LabelImageMapFilterType::Pointer labelmapfilter = LabelImageMapFilterType::New();
  LabelImageType::Pointer labelmap;
  LabelImageType::LabelVectorType labels;

  typedef map<InputPixelType, InputImagePointerType> MapInputImagePointerType;

  MapInputImagePointerType mapimages;

  if(!useMinMaxIntensity){
      minIntensity = numeric_limits< double >::max();
      maxIntensity = numeric_limits< double >::min();
  }

  if(inputMask.compare("")!=0){
      typedef itk::ImageFileReader< InputImageType > InputImageFileReaderType;
      typedef InputImageFileReaderType::Pointer InputImageFileReaderPointerType;
      InputImageFileReaderPointerType readermask = InputImageFileReaderType::New();

      readermask->SetFileName(inputMask);
      readermask->Update();
      maskin = readermask->GetOutput();

      typedef itk::MaskImageFilter< InputImageType, InputImageType > MaskImageFilterType;
      typedef MaskImageFilterType::Pointer MaskImageFilterPointerType;

      MaskImageFilterPointerType maskfilter = MaskImageFilterType::New();
      maskfilter->SetInput(imgin);
      maskfilter->SetMaskImage(maskin);
      maskfilter->SetMaskingValue(backGroundValue);
      maskfilter->SetOutsideValue(backGroundValue);
      maskfilter->Update();

      mapimages[1] = maskfilter->GetOutput();

  }else if(inputLabelMap.compare("") != 0){
      typedef itk::ImageFileReader< InputImageType > InputImageFileReaderType;
      typedef InputImageFileReaderType::Pointer InputImageFileReaderPointerType;
      typedef itk::LabelMapMaskImageFilter< LabelImageType, InputImageType > LabelMapMaskImageFilterType;

      InputImageFileReaderPointerType readerlm = InputImageFileReaderType::New();

      InputImagePointerType labelimage = 0;

      readerlm->SetFileName(inputLabelMap);
      readerlm->Update();
      labelimage = readerlm->GetOutput();

      labelmapfilter->SetInput(labelimage);
      labelmapfilter->Update();
      labelmap = labelmapfilter->GetOutput();

      labels = labelmap->GetLabels();
      for(unsigned i = 0; i < labels.size(); i++){
          InputPixelType currentlabel = labels[i];

          if(currentlabel != backGroundValue){

              LabelMapMaskImageFilterType::Pointer labelmaskfilter = LabelMapMaskImageFilterType::New();
              labelmaskfilter->SetInput( labelmap );
              labelmaskfilter->SetFeatureImage( imgin );
              labelmaskfilter->SetBackgroundValue( backGroundValue );
              labelmaskfilter->SetLabel(currentlabel);
              labelmaskfilter->CropOn();
              labelmaskfilter->Update();

              mapimages[currentlabel] = labelmaskfilter->GetOutput();
          }
      }
  }else{
    mapimages[1] = imgin;
  }


  ostringstream outhisto;

  ostringstream os;

  const string OutputNames[] = {
          "Input",
          "Label",
          "MinIntensity",
          "MaxIntensity",
          "NumberOfIntensityBins",
          "MinSize",
          "MaxSize",
          "NumberOfSizeBins",
          "ShortRunEmphasis",
          "LongRunEmphasis",
          "GreyLevelNonuniformity",
          "RunLengthNonuniformity",
          "LowGreyLevelRunEmphasis",
          "HighGreyLevelRunEmphasis",
          "ShortRunLowGreyLevelEmphasis",
          "ShortRunHighGreyLevelEmphasis",
          "LongRunLowGreyLevelEmphasis",
          "LongRunHighGreyLevelEmphasis"
  };

  vector<string> OutputNamesVector(OutputNames, OutputNames + (sizeof(OutputNames)/sizeof(*OutputNames)));
  for(unsigned i = 0; i < OutputNamesVector.size(); i++){
      os<<OutputNames[i]<<", ";
  }
  os<<endl;

  typedef itk::Statistics::ScalarImageToIntensitySizeRunLengthFeaturesFilter< InputImageType > ScalarImageToRunLengthIntensitySizeFilterType;
  typedef ScalarImageToRunLengthIntensitySizeFilterType::SamplePointerType SamplePointerType;

  typedef map< InputPixelType, SamplePointerType > MapListSamplePointerType;

  MapListSamplePointerType mapsample;

  if((!computeMinMaxIntensityPerLabel || !computeMinMaxSizePerLabel) && mapimages.size() > 1){

      double tempMinIntensity = numeric_limits<double>::max();
      double tempMaxIntensity = numeric_limits<double>::min();
      int tempMinSize = numeric_limits<int>::max();
      int tempMaxSize = numeric_limits<int>::min();

      for(MapInputImagePointerType::iterator iter = mapimages.begin(); iter != mapimages.end(); ++iter){

          cout<<endl<<"Label: "<<iter->first<<endl;

          ScalarImageToRunLengthIntensitySizeFilterType::Pointer imgtorunlegth = ScalarImageToRunLengthIntensitySizeFilterType::New();
          imgtorunlegth->SetBackgroundValue(backGroundValue);
          imgtorunlegth->SetInput(iter->second);
          imgtorunlegth->SetNumberOfIntensityBins(numberOfIntensityBins);
          imgtorunlegth->SetNumberOfSizeBins(numberOfSizeBins);
          imgtorunlegth->SetUseDynamicThreshold(useDynamicThreshold);
          imgtorunlegth->Update();

          mapsample[iter->first] = imgtorunlegth->GetListSample();

          tempMinIntensity = min(imgtorunlegth->GetMinIntensity(), tempMinIntensity);
          tempMaxIntensity = max(imgtorunlegth->GetMaxIntensity(), tempMaxIntensity);

          tempMinSize = min(imgtorunlegth->GetMinSize(), tempMinSize);
          tempMaxSize = max(imgtorunlegth->GetMaxSize(), tempMaxSize);
      }

      if(!computeMinMaxIntensityPerLabel && !useMinMaxIntensity){
          useMinMaxIntensity = true;
          minIntensity = tempMinIntensity;
          maxIntensity = tempMaxIntensity;

          cout<<"Using minIntensity and maxIntensity from all regions..."<<endl;
          cout<<"\t minIntensity: "<<minIntensity<<endl;
          cout<<"\t maxIntensity: "<<maxIntensity<<endl;
      }

      if(!computeMinMaxSizePerLabel && !useMinMaxSize){
          useMinMaxSize = true;
          minSize = tempMinSize;
          maxSize = tempMaxSize;

          cout<<"Using minSize and maxSize from all regions..."<<endl;
          cout<<"\t minSize: "<<minSize<<endl;
          cout<<"\t maxSize: "<<maxSize<<endl;
      }
  }

  for(MapInputImagePointerType::iterator iter = mapimages.begin(); iter != mapimages.end(); ++iter){

      cout<<endl<<"Label: "<<iter->first<<endl;

      ScalarImageToRunLengthIntensitySizeFilterType::Pointer imgtorunlegth = ScalarImageToRunLengthIntensitySizeFilterType::New();
      imgtorunlegth->SetBackgroundValue(backGroundValue);
      imgtorunlegth->SetInput(iter->second);

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

      if(useDynamicThreshold && mapsample.find(iter->first) != mapsample.end()){
          //if the dynamic threshold is enabled, and compute the global intensity or component size is enabled then it is not necesary to recompute the sample.  if((!computeMinMaxIntensityPerLabel || !computeMinMaxSizePerLabel) && mapimages.size() > 1) then the list sample exists.
          imgtorunlegth->SetListSample(mapsample[iter->first]);
      }

      imgtorunlegth->SetUseDynamicThreshold(useDynamicThreshold);
      imgtorunlegth->Update();

      outhisto << ((ostringstream*)imgtorunlegth->GetHistogramOutput())->str();

      os<<inputVolume<<", ";
      os<<iter->first<<", ";
      os<<imgtorunlegth->GetMinIntensity()<<", ";
      os<<imgtorunlegth->GetMaxIntensity()<<", ";
      os<<imgtorunlegth->GetNumberOfIntensityBins()<<", ";
      os<<imgtorunlegth->GetMinSize()<<", ";
      os<<imgtorunlegth->GetMaxSize()<<", ";
      os<<imgtorunlegth->GetNumberOfSizeBins()<<", ";
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
  }

  if(outputHistogram.compare("") != 0){
      ofstream outhistofile(outputHistogram.c_str());
      if(outhistofile.is_open()){
          outhistofile << outhisto.str();
          outhistofile.close();
      }else{
          cerr<<"Could not create file: "<<outputHistogram<<endl;
      }
  }else if(outputHistogramPrint){
      cout<<outhisto.str()<<endl;
  }


  if(outputRLE.compare("") != 0){
      ofstream outputrlefile;
      if(outputRLE.compare(outputHistogram) == 0){
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
