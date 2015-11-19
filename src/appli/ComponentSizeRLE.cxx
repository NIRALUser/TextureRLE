
#include "ComponentSizeRLECLP.h"

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkLabelImageToLabelMapFilter.h>
#include <itkLabelMapMaskImageFilter.h>
#include <itkMaskImageFilter.h>

#include "itkScalarImageToIntensitySizeRunLengthFeaturesFilter.h"

extern "C" {

int executeRLE(int argc, char* argv[]);

}

#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <bind.h>

#include "itkImageJS.h"

using namespace std;
using namespace emscripten;

/*
* This function should only be used when executing in NODE.js in order to mount 
* a path in the filesystem to the NODEFS system. 
*
*/
extern "C" {
    void MountDirectory(const char* filename){
      
      EM_ASM_({

          var path = require('path');
          var fs = require('fs');
          var allpath = Pointer_stringify($0);
          var dir = path.dirname(allpath);

          var currentdir = path.sep;
          var sepdirs = dir.split(path.sep);

          for(var i = 0; i < sepdirs.length; i++){
            currentdir += sepdirs[i];
            try{
              FS.mkdir(currentdir);
            }catch(e){
              //console.error(e);
            }
            currentdir += path.sep;
          }
          try{
            FS.mount(NODEFS, { root: currentdir }, dir);
          }catch(e){
            //console.error(e);
          }

        }, filename
      );
      
    }
}

// Binding code
EMSCRIPTEN_BINDINGS(itk_image_j_s) {
  class_<itkImageJS>("itkImageJS")
    .constructor<>()
    .function("ReadImage", &itkImageJS::ReadImage)
    .function("WriteImage", &itkImageJS::WriteImage)
    .function("MountDirectory", &itkImageJS::MountDirectory)
    .function("GetBufferPointer", &itkImageJS::GetBufferPointer)
    .function("GetBufferSize", &itkImageJS::GetBufferSize)
    .function("GetSpacing", &itkImageJS::GetSpacing)
    .function("GetOrigin", &itkImageJS::GetOrigin)
    .function("GetDimensions", &itkImageJS::GetDimensions)
    .function("GetDirection", &itkImageJS::GetDirection)
    .function("GetPixel", &itkImageJS::GetPixel)
    .function("GetPixelWorld", &itkImageJS::GetPixelWorld)
    .function("SetPixel", &itkImageJS::SetPixel)
    .function("GetDataType", &itkImageJS::GetDataType)
    .function("GetFilename", &itkImageJS::GetFilename)
    .function("SetFilename", &itkImageJS::SetFilename)
    ;
}

#endif


int writeStringToMemory(string str, int ptr){
#ifdef EMSCRIPTEN
    return EM_ASM_({
        var string = Pointer_stringify($0);
        var variable = Pointer_stringify($1);
        if(Module.variable === undefined){
            Module.variable = {};
        }
        Module.variable[variable] = string;
        return 1;
    }, str.c_str(), ptr);
#else
    return ptr;
#endif

}

#ifndef EMSCRIPTEN
 int main (int argc, char * argv[]){
     return executeRLE(argc, argv);
 }
#endif

int executeRLE(int argc, char* argv[]){
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

    cout<<endl<<"Outputs: "<<endl;
    if(outputRLE.compare("") != 0){
        cout<<"\tThe output RLE analysis will be written to: "<<outputRLE<<endl;
    }

    if(outputHistogram.compare("") != 0){
        cout<<"\tThe output histogram will be written to: "<<outputHistogram<<endl;
    }

    if(outputRLEVar){
        cout<<"\tThe output RLE analysis will be written to pointer: "<<outputRLEVar<<endl;
    }

    if(outputHistogramVar){
        cout<<"\tThe output histogram will be written to pointer: "<<outputHistogramVar<<endl;
    }

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
        os<<OutputNames[i]<<",";
    }

    typedef itk::Statistics::ScalarImageToIntensitySizeRunLengthFeaturesFilter< InputImageType > ScalarImageToRunLengthIntensitySizeFilterType;
    typedef ScalarImageToRunLengthIntensitySizeFilterType::SamplePointerType SamplePointerType;

    typedef map< InputPixelType, SamplePointerType > MapListSamplePointerType;

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

        imgtorunlegth->SetUseDynamicThreshold(useDynamicThreshold);
        imgtorunlegth->SetPercentile(percentile);

        try{
            imgtorunlegth->Update();
            
            if(iter != mapimages.begin()){
                outhisto<<endl<<"\t";
            }
            outhisto <<"Label"<<endl<<iter->first<<"\t"<<endl;
            outhisto <<((ostringstream*)imgtorunlegth->GetHistogramOutput())->str();

            os<<endl;
            os<<inputVolume<<",";
            os<<iter->first<<",";
            os<<imgtorunlegth->GetMinIntensity()<<",";
            os<<imgtorunlegth->GetMaxIntensity()<<",";
            os<<imgtorunlegth->GetNumberOfIntensityBins()<<",";
            os<<imgtorunlegth->GetMinSize()<<",";
            os<<imgtorunlegth->GetMaxSize()<<",";
            os<<imgtorunlegth->GetNumberOfSizeBins()<<",";
            os<<imgtorunlegth->GetShortRunEmphasis()<<",";
            os<<imgtorunlegth->GetLongRunEmphasis()<<",";
            os<<imgtorunlegth->GetGreyLevelNonuniformity()<<",";
            os<<imgtorunlegth->GetRunLengthNonuniformity()<<",";
            os<<imgtorunlegth->GetLowGreyLevelRunEmphasis()<<",";
            os<<imgtorunlegth->GetHighGreyLevelRunEmphasis()<<",";
            os<<imgtorunlegth->GetShortRunLowGreyLevelEmphasis()<<",";
            os<<imgtorunlegth->GetShortRunHighGreyLevelEmphasis()<<",";
            os<<imgtorunlegth->GetLongRunLowGreyLevelEmphasis()<<",";
            os<<imgtorunlegth->GetLongRunHighGreyLevelEmphasis();

        }catch(exception& ex){
            cerr<<"Input volume: " << inputVolume << endl;
            if(inputMask.compare("") != 0){
                cerr << "Input mask: " << inputMask << endl;
            }
            if(inputLabelMap.compare("") != 0){
                cerr << "Input label map: "<< inputLabelMap << endl;
                cerr<<"Label: "<<iter->first<<endl;
            }
            cerr<<ex.what()<<endl;
        }

    }

    if(outputHistogram.compare("") != 0){
        ofstream outhistofile(outputHistogram.c_str());
        if(outhistofile.is_open()){
            outhistofile << outhisto.str();
            outhistofile.close();
        }else{
            cerr<<"Could not create file: "<<outputHistogram<<endl;
        }
    }else if(outputHistogramVar){
        writeStringToMemory(outhisto.str(), outputHistogramVar);
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
    }else if(outputRLEVar){
        writeStringToMemory(os.str(), outputRLEVar);
    }else{
        cout<<os.str()<<endl;
    }

    cout<<"Done!"<<endl;

    return EXIT_SUCCESS;
}
