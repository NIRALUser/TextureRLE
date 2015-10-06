
#include "DMDCalib.h"

//////////////////////////////////////////////////////////////////////////
void DMDCalib::intensityRescale ( DMDData::OrientedSeriesReaderType::Pointer reader, DMDData::OrientedWriterType::Pointer & writer )
{

    typedef itk::RescaleIntensityImageFilter< DMDData::OrientedImageType, DMDData::OrientedImageType >  RescaleFilterType;
    typedef itk::ThresholdImageFilter< DMDData::OrientedImageType >  ThreshFilterType;
    typedef float MeasurementType ;
    typedef itk::Statistics::ScalarImageToHistogramGenerator< DMDData::OrientedImageType >   HistogramGeneratorType;

    HistogramGeneratorType::Pointer histogramGenerator = HistogramGeneratorType::New();
    const unsigned int numberOfHistogramBins = 10000 ;
    histogramGenerator->SetNumberOfBins( numberOfHistogramBins );
    histogramGenerator->SetMarginalScale( 1.0 );
    RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
    ThreshFilterType::Pointer threshFilter = ThreshFilterType::New();

    threshFilter->SetInput( reader->GetOutput() );

    // saturate pixel values  higher than 10000
    threshFilter->SetOutsideValue( 10000 );
    threshFilter->ThresholdAbove( 10000 );
    threshFilter->Update();
    // calculate histogram of the images
    histogramGenerator->SetInput( threshFilter->GetOutput() );
    histogramGenerator->Compute();
    typedef HistogramGeneratorType::HistogramType  HistogramType;
    const HistogramType * histogram = histogramGenerator->GetOutput();
    //  const unsigned int histogramSize = histogram->Size();

    HistogramType::ConstIterator itr = histogram->Begin();
    HistogramType::ConstIterator end = histogram->End();
    // remove the dark background
    double Sum = histogram->GetTotalFrequency() - itr.GetFrequency() ;
    ++itr;
    Sum = Sum - itr.GetFrequency();
    ++itr;
    Sum = Sum - itr.GetFrequency();
    ++itr;
    float accumProb = 0;
    while( itr != end )    {
      const double probability = itr.GetFrequency() / Sum  ;
        accumProb += probability ;
    // 	std::cout << itr.GetFrequency() << std::endl;
    if (accumProb >= 0.995){
      //    std::cout << itr.GetInstanceIdentifier() << std::endl;
        break;
    }
        ++itr ;
    }
    //  getchar();
    threshFilter->SetOutsideValue( itr.GetInstanceIdentifier() );
    threshFilter->ThresholdAbove( itr.GetInstanceIdentifier() );
    threshFilter->Update();
    rescaleFilter->SetInput( threshFilter->GetOutput() );    // setup the minmum and maxmum of the rescaled images
    rescaleFilter->SetOutputMinimum( 0 );
    rescaleFilter->SetOutputMaximum( 1024 );
    // run the pipeline
    rescaleFilter->Update();
    //   return filterZ;
    writer->SetInput( rescaleFilter->GetOutput() );

}
//////////////////////////////////////////////////////////////////////////
void DMDCalib::linearCalib ( DMDData::OrientedImageType::Pointer &data, DMDData::OrientedImageType::Pointer &output, DMDData::OrientedImageReaderType::Pointer maskPtr, float meanT2, float meanT2FS, float fk, float fb, float integrate )
{

    float tmp = 0;
    DMDData::ConstIteratorType dataIterator( data, data->GetRequestedRegion() );
    DMDData::IteratorType outputIterator( output, output->GetRequestedRegion() );
    DMDData::OrientedImageType::Pointer mask = maskPtr->GetOutput();
    DMDData::ConstIteratorType maskIterator( mask, mask->GetRequestedRegion() );

    if ( data->GetLargestPossibleRegion().GetSize()[0] == mask->GetLargestPossibleRegion().GetSize()[0] && data->GetLargestPossibleRegion().GetSize()[1] == mask->GetLargestPossibleRegion().GetSize()[1] && data->GetLargestPossibleRegion().GetSize()[2] == mask->GetLargestPossibleRegion().GetSize()[2] ) {
        for ( dataIterator.GoToBegin(), outputIterator.GoToBegin(); !dataIterator.IsAtEnd(); ++dataIterator, ++outputIterator ) {
            //  if ( constLeftMaskIterator.Get() > 0 ) {
      //   float tmp = dataIterator.Get();
       tmp = dataIterator.Get() - integrate ;
       outputIterator.Set( tmp * fk + fb );

        //  std::cout << dataIterator.Get() << "  " << outputIterator.Get() << "  " << fk << "  " << fb << std::endl;
      //outputIterator.Set( (dataIterator.Get() - meanT2FS) / (meanT2 - meanT2FS) * 900 + 100 );		    //    }
        //   std::cout << dataIterator.Get() << std::endl;
        //   getchar();
    }
    }

}
