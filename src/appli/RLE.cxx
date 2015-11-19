
#include "RLECLP.h"


#include <itkImageFileReader.h>
#include <itkMaskImageFilter.h>
#include <itkScalarImageToRunLengthFeaturesFilter.h>
#include <itkNeighborhood.h>
#include <itkLabelImageToLabelMapFilter.h>
#include <itkLabelMapMaskImageFilter.h>

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

  typedef itk::LabelObject< InputPixelType, dimension > LabelObjectType;
  typedef itk::LabelMap< LabelObjectType > LabelImageType;
  typedef itk::LabelImageToLabelMapFilter< InputImageType, LabelImageType > LabelImageMapFilterType;
  LabelImageMapFilterType::Pointer labelmapfilter = LabelImageMapFilterType::New();
  LabelImageType::Pointer labelmap;
  LabelImageType::LabelVectorType labels;

  typedef map<InputPixelType, InputImagePointerType> MapInputImagePointerType;

  MapInputImagePointerType mapimages;

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


  //WARNING! this should be the same size as the featuresnames number of elements. If you are reading this because the
  //program crashed, then something changed in the filter. Check the number of produced outputs.

  const string OutputNames[] = {
          "Input",
          "Label",
          "MinIntensity",
          "MaxIntensity",
          "NumberOfIntensityBins",          
          "ShortRunEmphasis",
          "LongRunEmphasis",
          "GreyLevelNonuniformity",
          "RunLengthNonuniformity",
          "LowGreyLevelRunEmphasis",
          "HighGreyLevelRunEmphasis",
          "ShortRunLowGreyLevelEmphasis",
          "ShortRunHighGreyLevelEmphasis",
          "LongRunLowGreyLevelEmphasis",
          "LongRunHighGreyLevelEmphasis",
          "ShortRunEmphasisStdev",
          "LongRunEmphasisStdev",
          "GreyLevelNonuniformityStdev",
          "RunLengthNonuniformityStdev",
          "LowGreyLevelRunEmphasisStdev",
          "HighGreyLevelRunEmphasisStdev",
          "ShortRunLowGreyLevelEmphasisStdev",
          "ShortRunHighGreyLevelEmphasisStdev",
          "LongRunLowGreyLevelEmphasisStdev",
          "LongRunHighGreyLevelEmphasisStdev"};

  ostringstream os;

  vector<string> OutputNamesVector(OutputNames, OutputNames + (sizeof(OutputNames)/sizeof(*OutputNames)));
  for(unsigned i = 0; i < OutputNamesVector.size(); i++){
      os<<OutputNames[i]<<",";
  }
  os<<endl;

  for(MapInputImagePointerType::iterator iter = mapimages.begin(); iter != mapimages.end(); ++iter){

     if(!useMinMaxIntensity){
      cout<<"Calculating the min max intensity..."<<endl;

      typedef itk::ImageRegionIterator<InputImageType> InputImageRegionIteratorType;
      InputImageRegionIteratorType it(iter->second, iter->second->GetLargestPossibleRegion());
      InputPixelType minvalue = numeric_limits< InputPixelType >::max();
      InputPixelType maxvalue = numeric_limits< InputPixelType >::min();
      while(!it.IsAtEnd()){
          InputPixelType pix = it.Get();
          if(pix != backGroundValue){
              if(minvalue > pix){
                  minvalue = pix;
              }
              if(maxvalue < pix){
                  maxvalue = pix;
              }
          }
          ++it;
      }
      minIntensity = minvalue + (maxvalue - minvalue)*.01;      
      cout<<"\t Min intensity + 1% = "<<minIntensity<<endl;
      maxIntensity = maxvalue - (maxvalue - minvalue)*.01;      
      cout<<"\t Max intensity - 1% = "<<maxIntensity<<endl;
  }

    typedef itk::Statistics::ScalarImageToRunLengthFeaturesFilter< InputImageType > ScalarImageToRunLengthFeaturesFilterType;
    ScalarImageToRunLengthFeaturesFilterType::Pointer imgtorunlength = ScalarImageToRunLengthFeaturesFilterType::New();
    imgtorunlength->SetInput(iter->second);    

    imgtorunlength->SetNumberOfBinsPerAxis(numberOfIntensityBins);    
    imgtorunlength->SetPixelValueMinMax(minIntensity, maxIntensity);
    

    typedef itk::Neighborhood< InputImageType::PixelType, dimension> NeighborhoodType;
    NeighborhoodType hood;
    hood.SetRadius( 1 );

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

    //const ScalarImageToRunLengthFeaturesFilterType::FeatureNameVector* featurenames = imgtorunlength->GetRequestedFeatures();

    os<<inputVolume<<", ";
    os<<iter->first<<", ";//label value
    os<<minIntensity<<",";
    os<<maxIntensity<<",";
    os<<numberOfIntensityBins<<",";
    
    for(VectorContainer::Iterator fmiterator = featuremeans->Begin(); fmiterator != featuremeans->End(); ++fmiterator){
        os<<fmiterator->Value()<<", ";
    }
    
    VectorContainer::Pointer featurestdev = imgtorunlength->GetFeatureStandardDeviations();
    for(VectorContainer::Iterator fmiterator = featurestdev->Begin(); fmiterator != featurestdev->End(); ++fmiterator){
        os<<fmiterator->Value()<<", ";
    }
    os<<endl;   
  }  

  

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
