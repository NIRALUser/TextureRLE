#include "replaceImageHeaderCLP.h"

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkVectorRescaleIntensityImageFilter.h>
#include <itkMinimumMaximumImageFilter.h>
#include <itkImageRegionIterator.h>

#include <iostream>
#include <limits> 

using namespace std;


static const int Dimension = 3;
typedef unsigned short InPixelType;
typedef itk::Image<InPixelType, Dimension> InputImageType;
typedef InputImageType::Pointer InputImagePointerType;
typedef InputImageType::PointType InputImagePointType;
typedef InputImageType::SpacingType InputImageSpacingType;
typedef itk::ImageFileReader<InputImageType> InputImageFileReaderType;
typedef itk::ImageRegionIterator<InputImageType> InputImageRegionIteratorType;

typedef itk::ImageFileWriter<InputImageType> ImageFileWriterType;

bool compareHeader(InputImagePointerType img, InputImagePointerType refimg){
	const double coordinateTol = 1.0e-6 * img->GetSpacing()[0]; // use first dimension spacing

	if ( !img->GetOrigin().GetVnlVector().is_equal(refimg->GetOrigin().GetVnlVector(), coordinateTol) ||
           !img->GetSpacing().GetVnlVector().is_equal(refimg->GetSpacing().GetVnlVector(), coordinateTol) ||
           !img->GetDirection().GetVnlMatrix().as_ref().is_equal(refimg->GetDirection().GetVnlMatrix(), 1.0e-6) ){
        
        return false;
    }

    return true;

}

int main (int argc, char * argv[]){


	PARSE_ARGS;

	if(inputFilename.compare("") == 0 || referenceFilename.compare("") == 0 ){
		cout<<"Type "<<argv[0]<<" --help to find out how to use this program."<<endl;
		return 1;
	}

	if(outputFilename.compare("") == 0){
		outputFilename = "out.nrrd";
	}

	cout<<"Reading: "<<inputFilename<<endl;

	InputImageFileReaderType::Pointer reader = InputImageFileReaderType::New();
	reader->SetFileName(inputFilename);
	reader->Update();
	InputImageType::Pointer inputimage = reader->GetOutput();

	cout<<"Reading: "<<referenceFilename<<endl;

	InputImageFileReaderType::Pointer reader2 = InputImageFileReaderType::New();
	reader2->SetFileName(referenceFilename);
	reader2->Update();
	InputImageType::Pointer refimage = reader2->GetOutput();
	

	if(compareHeaders){
		if(!compareHeader(inputimage, refimage)){
			cerr<<"Image space is not the same: "<<endl;
			cerr<<inputFilename<<endl;
			cerr<<referenceFilename<<endl;
			return EXIT_FAILURE;
		}
		
	}else{

		inputimage->SetSpacing(refimage->GetSpacing());
		inputimage->SetOrigin(refimage->GetOrigin());
		inputimage->SetDirection(refimage->GetDirection());
		
		cout<<"Writing: "<<outputFilename<<endl;
		ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
		writer->SetFileName(outputFilename);
		writer->SetInput(inputimage);
		writer->Update();
	}

	



	return EXIT_SUCCESS;
}