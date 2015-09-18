/*=========================================================================
 
 Program:   Texture Analysis Test
 Module:    $$
 Language:  C++
 Date:      $Date: xxx-xx-xx $
 Version:   $Revision: xxx $
 
 Copyright (c) University of North Carolina at Chapel Hill (UNC-CH) All Rights Reserved.
 
 See License.txt or http://www.slicer.org/copyright/copyright.txt for details.
 
 ==========================================================================*/
#include "itkTexture3DFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include <iostream>
#include <string>

/*TYPEDEFS HERE*/

/*EXTERNAL FUNCTIONS HERE*/

int main (int argc, char ** argv)
{
    
    std::cout << "lalalala" << std::endl;
    // Setup types
    typedef itk::Image<unsigned int, 3>   ROIImageType;
    typedef itk::Image<float, 3>           OutputImageType;
    typedef itk::Image<float, 3>           InputImageType;
    typedef itk::Texture3DFilter<InputImageType, OutputImageType>  FilterType;
    typedef itk::ImageFileReader<InputImageType> ReaderImageType;
    typedef itk::ImageFileReader<ROIImageType> ReaderROIImageType;

    ReaderImageType::Pointer readerImg = ReaderImageType::New();
    readerImg->SetFileName("/Users/bpaniagua/Work/Code/Texture/testing/Scan_MICRO_CT_6R_rs_pp.nrrd");

    ReaderROIImageType::Pointer readerROIImg = ReaderROIImageType::New();
    readerROIImg->SetFileName("/Users/bpaniagua/Work/Code/Texture/testing/SegmC_MICRO_CT_6R_e_pp.nrrd");

    // Create and the filter
    FilterType::Pointer filter = FilterType::New();
    filter->SetInputROI(readerROIImg->GetOutput());
    filter->SetInput(readerImg->GetOutput());
    filter->Update();

    {
          typedef  itk::ImageFileWriter< OutputImageType  > WriterType;
          WriterType::Pointer writer = WriterType::New();
          writer->SetFileName("/Users/bpaniagua/Work/Code/Texture/testing/testMap00.nrrd");
          writer->SetInput(filter->GetOutput(0));
          writer->Update();
    }

     return EXIT_SUCCESS;
}
