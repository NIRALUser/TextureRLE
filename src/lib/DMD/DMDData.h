/*=========================================================================
 * * Program :   $Insight Segmentation & Registration Toolkit $ * Module  :   $DMDBiomarkerTool: DMDBase.h $ * Purpose :   $The base class of Duchenne Muscle Dystrophy biomarker tools $
 * Language:   $C++ $
 * Date    :   $Date: 2010-06-04 12:36:34 $
 * Version :   $Revision: 0.10 $
 * Authors :   $Jiahui Wang, Martin Styner $
 * Update  :   1. Created the data class (06-04-10)
 * Copyright (c) Nero Image Research and Analysis Lab.@UNC All rights reserved.
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even 
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
 * PURPOSE.
 *
=========================================================================*/

#include "TextureBioMarkerTool.h"

//#include "itkOrientedImage.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImage.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkConstShapedNeighborhoodIterator.h"
#include "itkGradientAnisotropicDiffusionImageFilter.h"
#include "itkImageDuplicator.h"
#include "itkResampleImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"

#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"

#include "itkGrayscaleErodeImageFilter.h"
#include "itkGrayscaleDilateImageFilter.h"
#include "itkScalarImageToHistogramGenerator.h"

#include "itkExtractImageFilter.h"
#include "itkSliceBySliceImageFilter.h"

#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"

#include "itkOtsuThresholdImageFilter.h"

#include "itkRecursiveGaussianImageFilter.h"
#include "itkImageMomentsCalculator.h"
#include "itkLabelStatisticsImageFilter.h"
#include "itkLabelGeometryImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"

#include <math.h>
#include <iostream>
#include <fstream>

//using namespace std;

#ifndef DMDDATA    // prevent for redefine
#define DMDDATA    
#define SEARCHED_PIXEL_VALUE -1
#define TEMP_PIXEL_VALUE -2


class DMDData
{
    private:
        
    public:
        typedef float                                                      PixelType;
        typedef long			                                   uncharPixelType;
	typedef std::string                                                StrITKType;
        typedef int                                                        IntITKType;
        typedef float                                                      FITKType;
        typedef double                                                     LfITKType;
        DMDData();
        DMDData( IntITKType d, StrITKType inputDirec );
        // declare the pixel type and image dimension
        typedef itk::Image< PixelType, 2 >                         OrientedImage2DType;
        typedef itk::Image< PixelType, 3 >                         OrientedImageType;
        typedef itk::Image< uncharPixelType, 3 >                   uncharOrientedImageType;
	// create image file reader and writer
        typedef itk::ImageSeriesReader< OrientedImageType >                OrientedSeriesReaderType;
        typedef itk::ImageFileWriter< OrientedImageType >                  OrientedWriterType;
	typedef itk::Image< PixelType, 3 >                                 ImageReaderInputType;
	typedef itk::ImageFileReader< ImageReaderInputType >               segMuscleReaderType;  
	//	typedef itk::Image< PixelType, 3 >                                 ImageReaderInputType;
        // dicomIO was created to connect GDCMImage (provide services for reading and writing DICOM files) and reader
        typedef itk::GDCMImageIO                                           ImageIOType;
        // to identify from a given directory the set of filenames that belong together to the same volumetric image
        typedef itk::RecursiveGaussianImageFilter<OrientedImageType, OrientedImageType>    FilterType;
	typedef itk::GDCMSeriesFileNames                                   NamesGeneratorType;    
        typedef itk::MetaDataDictionary                                    DictionaryType;            
        typedef std::vector< StrITKType >                                  FileNamesContainer;        
	typedef itk::ImageFileReader< OrientedImageType >                  OrientedImageReaderType;
        typedef itk::ImageRegionConstIterator< OrientedImageType >         ConstIteratorType;
        typedef itk::ImageRegionIterator< OrientedImageType >              IteratorType;
	typedef itk::ConstShapedNeighborhoodIterator< ImageReaderInputType > ShapedNeighborhoodIteratorType;	

        // connected component filter
	typedef itk::ConnectedComponentImageFilter<uncharOrientedImageType, uncharOrientedImageType> itkConnectedComponentFilterType;
        typedef itk::RelabelComponentImageFilter<uncharOrientedImageType, uncharOrientedImageType>    itkRelabelComponentFilterType;
 


	FileNamesContainer                                                 fileNames;
	StrITKType                                                         inputDirectory;
        itkConnectedComponentFilterType::Pointer labeller; //= itkConnectedComponentFilterType::New();
        itkRelabelComponentFilterType::Pointer relabeller;// = itkRelabelComponentFilterType::New();
	
        // tells the GDCMSereiesFileNames object to use additional DICOM information to distinguish unique volumes within the directory.
        // note that SetUseSeriesDetails(true) must be called before SetDirectory()	          
	void dataReader ( StrITKType fileName, OrientedImageReaderType::Pointer & reader );
	void seriesDataReader ( StrITKType seriesIdentifie, OrientedSeriesReaderType::Pointer & reader, ImageIOType::Pointer & dicomIO );
	void dataWriter ( OrientedWriterType::Pointer writer, StrITKType outputFilename );
	void readDicomTag ( ImageIOType::Pointer & dicomIO, std::vector<StrITKType> & tagvalue, std::vector<StrITKType> & tagkey );
	void imageErod ( );
	void smoothGradAnisoDiff( OrientedImageType::Pointer input, OrientedImageType::Pointer &output, int smoothItr, float smoothTimeStep, float smoothConductance );
	void smoothGaussian( OrientedImageType::Pointer input, OrientedImageType::Pointer &output, float sigma );
	//void imageInitialize (OrientedSeriesReaderType::Pointer input, OrientedImageType::Pointer &data, float &voxelVol );
        //void imageInitialize ( OrientedSeriesReaderType::Pointer input, OrientedImageType::Pointer &data );
	void imageInitialize (OrientedImageType::Pointer input, OrientedImageType::Pointer &data, float &voxelVol );
        void imageInitialize ( OrientedImageType::Pointer input, OrientedImageType::Pointer &data );
        float muscleMean ( OrientedSeriesReaderType::Pointer input, OrientedImageReaderType::Pointer mask, int &volume ) ;     
        void morphErod ( OrientedImageType::Pointer input, OrientedImageType::Pointer &output, IntITKType size ) ;
        void morphDilat ( OrientedImageType::Pointer input, OrientedImageType::Pointer &output, IntITKType size ) ;
        void morphMultiGrayErod2DIn3D ( OrientedImageType::Pointer input, OrientedImageType::Pointer &output, IntITKType size ) ;
        void outHistogram ( OrientedImageType::Pointer data, OrientedImageType::Pointer roi, OrientedImageType::Pointer mask, StrITKType Identify );
        //void interpolate3D (OrientedImageType::Pointer inputImage, OrientedImageType::Pointer outputImage);
        void interpolate3D (const OrientedImageType::Pointer inputImage, StrITKType);
        float overlap (OrientedImageType::Pointer, OrientedImageType::Pointer, int, bool, float );
        void connectedLabeling2D (const OrientedImage2DType::Pointer input );
        void connectedLabeling3D (const OrientedImageType::Pointer input );
        void waterTubeSegmentation (const OrientedImageType::Pointer input );
        void fatSuppressBiasIdentify (const OrientedImageType::Pointer inputT2, OrientedImageType::Pointer inputT2FS);
        void histogramMatchinMask (const OrientedImageType::Pointer input, OrientedImageType::Pointer inputRef, OrientedImageType::Pointer output, int bin, int point);
        int connectedComponentRegionGrowing (float lower, float upper, OrientedImageType::Pointer & input, OrientedImageType::IndexType index);
        int connectedComponentLabeling (OrientedImageType::Pointer & input);


        
};
#endif
