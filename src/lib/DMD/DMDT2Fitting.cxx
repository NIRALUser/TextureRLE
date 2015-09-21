#include "DMDT2Fitting.h"

//////////////////////////////////////////////////////////////////////////
void DMDT2Fitting::smooth( DMDData::OrientedSeriesReaderType::Pointer reader, DMDData::OrientedWriterType::Pointer & writer )
{
    // specify the filter type -- gaussian filter
    typedef itk::RecursiveGaussianImageFilter< DMDData::OrientedImageType, DMDData::OrientedImageType >  FilterType;
    FilterType::Pointer                 filterX = FilterType::New();
    FilterType::Pointer                 filterY = FilterType::New();
    FilterType::Pointer                 filterZ = FilterType::New();

    filterX->SetDirection( 0 );   // 0 --> X direction
    filterY->SetDirection( 1 );   // 1 --> Y direction
    filterZ->SetDirection( 2 );   // 1 --> Z direction

    filterX->SetOrder( FilterType::ZeroOrder );
    filterY->SetOrder( FilterType::ZeroOrder );
    filterZ->SetOrder( FilterType::ZeroOrder );

    filterX->SetNormalizeAcrossScale( false );
    filterY->SetNormalizeAcrossScale( false );
    filterZ->SetNormalizeAcrossScale( false );

    filterX->SetInput(reader->GetOutput());
    filterY->SetInput(filterX->GetOutput());
    filterZ->SetInput(filterY->GetOutput());

    // Set the Filter width
    const double                        sigma = 3;
    filterX->SetSigma( sigma );
    filterY->SetSigma( sigma );
    filterZ->SetSigma( sigma );

    // run the pipeline
    filterZ->Update();

    //   return filterZ;
    writer->SetInput( filterZ->GetOutput() );

}
