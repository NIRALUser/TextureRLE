#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include <itkDenseFrequencyContainer2.h>
#include <itkHistogramToTextureFeaturesFilter.h>
#include "itkScalarImageToCooccurrenceMatrixFilter.h"
#include "itkVectorContainer.h"
#include "itkAddImageFilter.h"
#include "itkMultiplyImageFilter.h"
#include "itkRegionOfInterestImageFilter.h"
#include "math.h"

#include <itksys/SystemTools.hxx>
#include <itksys/Process.h>
#include <itksys/stl/vector>
#include <itksys/stl/string>

//definitions of used types
typedef itk::Image<float, 3> InternalImageType;
typedef itk::Image<unsigned char, 3> VisualizingImageType;
typedef itk::Neighborhood<float, 3> NeighborhoodType;
typedef itk::Statistics::ScalarImageToCooccurrenceMatrixFilter<InternalImageType>
    Image2CoOccuranceType;
typedef Image2CoOccuranceType::HistogramType HistogramType;
typedef itk::Statistics::HistogramToTextureFeaturesFilter<HistogramType> Hist2FeaturesType;
typedef InternalImageType::OffsetType OffsetType;
typedef itk::AddImageFilter <InternalImageType> AddImageFilterType;
typedef itk::MultiplyImageFilter<InternalImageType> MultiplyImageFilterType;
//calculate features for one offset
void calcTextureFeatureImage (OffsetType offset,
    InternalImageType::Pointer inputImage, InternalImageType::Pointer outInertia,
    InternalImageType::Pointer outCorrelation, InternalImageType::Pointer outEnergy,
    InternalImageType::Pointer outEntropy, InternalImageType::Pointer outIDM,
    InternalImageType::Pointer outClusterShade, InternalImageType::Pointer outClusterProminence,
    InternalImageType::Pointer outHaralickCorr)
{
    //allocate output images
    outInertia->CopyInformation(inputImage);
    outInertia->SetRegions(inputImage->GetLargestPossibleRegion());
    outInertia->Allocate();
    outInertia->FillBuffer(0);

    outCorrelation->CopyInformation(inputImage);
    outCorrelation->SetRegions(inputImage->GetLargestPossibleRegion());
    outCorrelation->Allocate();
    outCorrelation->FillBuffer(0);

    outEnergy->CopyInformation(inputImage);
    outEnergy->SetRegions(inputImage->GetLargestPossibleRegion());
    outEnergy->Allocate();
    outEnergy->FillBuffer(0);

    outEntropy->CopyInformation(inputImage);
    outEntropy->SetRegions(inputImage->GetLargestPossibleRegion());
    outEntropy->Allocate();
    outEntropy->FillBuffer(0);

    outIDM->CopyInformation(inputImage);
    outIDM->SetRegions(inputImage->GetLargestPossibleRegion());
    outIDM->Allocate();
    outIDM->FillBuffer(0);

    outClusterShade->CopyInformation(inputImage);
    outClusterShade->SetRegions(inputImage->GetLargestPossibleRegion());
    outClusterShade->Allocate();
    outClusterShade->FillBuffer(0);

    outClusterProminence->CopyInformation(inputImage);
    outClusterProminence->SetRegions(inputImage->GetLargestPossibleRegion());
    outClusterProminence->Allocate();
    outClusterProminence->FillBuffer(0);

    outHaralickCorr->CopyInformation(inputImage);
    outHaralickCorr->SetRegions(inputImage->GetLargestPossibleRegion());
    outHaralickCorr->Allocate();
    outHaralickCorr->FillBuffer(0);

    Image2CoOccuranceType::Pointer glcmGenerator=Image2CoOccuranceType::New();
    glcmGenerator->SetOffset(offset);
    glcmGenerator->SetNumberOfBinsPerAxis(16); //reasonable number of bins
    glcmGenerator->SetPixelValueMinMax(0, 255); //for input UCHAR pixel type
    Hist2FeaturesType::Pointer featureCalc=Hist2FeaturesType::New();
    typedef itk::RegionOfInterestImageFilter<InternalImageType,InternalImageType> roiType;
    roiType::Pointer roi=roiType::New();
    roi->SetInput(inputImage);
    InternalImageType::RegionType window;
    InternalImageType::RegionType::SizeType size;
    size.Fill(21); //window size=3x3x3
    window.SetSize(size);
    int half_window = 10;
    InternalImageType::IndexType pi; //pixel index

    //slide window over the entire image
    for (unsigned x=half_window; x<inputImage->GetLargestPossibleRegion().GetSize(0)-half_window; x++)
    {
       // std::cout << "x " << x << std::endl;
        pi.SetElement(0,x);
        window.SetIndex(0,x-half_window);
        for (unsigned y=half_window; y<inputImage->GetLargestPossibleRegion().GetSize(1)-half_window; y++)
        {
           // std::cout << "y " << y << std::endl;
            pi.SetElement(1,y);
            window.SetIndex(1,y-half_window);
            for (unsigned z=half_window; z<inputImage->GetLargestPossibleRegion().GetSize(2)-half_window; z++)
            {
                //std::cout << "z " << z << std::endl;
                pi.SetElement(2,z);
                window.SetIndex(2,z-half_window);
                roi->SetRegionOfInterest(window);
                roi->Update();
                glcmGenerator->SetInput(roi->GetOutput());
                glcmGenerator->Update();
                featureCalc->SetInput( glcmGenerator->GetOutput() );
                featureCalc->Update();

                float aux = featureCalc->GetFeature(Hist2FeaturesType::Inertia);
                if (isnan(aux))
                    aux = 0;
                outInertia->SetPixel(pi, aux);

                aux = featureCalc->GetFeature(Hist2FeaturesType::Correlation);
                if (isnan(aux))
                    aux = 0;
                outCorrelation->SetPixel(pi, aux);

                aux = featureCalc->GetFeature(Hist2FeaturesType::Energy);
                if (isnan(aux))
                    aux = 0;
                outEnergy->SetPixel(pi, aux);

                aux = featureCalc->GetFeature(Hist2FeaturesType::Entropy);
                if (isnan(aux))
                    aux = 0;
                outEntropy->SetPixel(pi, aux);

                aux = featureCalc->GetFeature(Hist2FeaturesType::InverseDifferenceMoment);
                if (isnan(aux))
                    aux = 0;
                outIDM->SetPixel(pi, aux);

                aux = featureCalc->GetFeature(Hist2FeaturesType::ClusterShade);
                if (isnan(aux))
                    aux = 0;
                outClusterShade->SetPixel(pi, aux);

                aux = featureCalc->GetFeature(Hist2FeaturesType::ClusterProminence);
                if (isnan(aux))
                    aux = 0;
                outClusterProminence->SetPixel(pi, aux);

                aux = featureCalc->GetFeature(Hist2FeaturesType::HaralickCorrelation);
                if (isnan(aux))
                    aux = 0;
                outHaralickCorr->SetPixel(pi, aux);
               // std::cout << "End z " << z << std::endl;
            }
        }
        std::cout<<'.';
    }
}

int main(int argc, char*argv[])
{
  if(argc < 2)
    {
    std::cerr << "Usage: " << argv[0] << " Required image.mha" << std::endl;
    return EXIT_FAILURE;
    }
  
  std::string fileName = argv[1];
  
  typedef itk::ImageFileReader<InternalImageType> ReaderType;
  ReaderType::Pointer reader=ReaderType::New();
  reader->SetFileName(fileName);
  reader->Update();
  InternalImageType::Pointer image=reader->GetOutput();

  std::cout << "Checkpoint 1: Image read" << std::endl;

  NeighborhoodType neighborhood;
  neighborhood.SetRadius(1);
  unsigned int centerIndex = neighborhood.GetCenterNeighborhoodIndex();


  OffsetType offset;
  typedef itk::ImageFileWriter<InternalImageType> WriterType;
  WriterType::Pointer writer=WriterType::New();
  for ( unsigned int d = 0; d < centerIndex; d++ )
  {
      std::cout << "Checkpoint 2: Before computing" << std::endl;
      offset = neighborhood.GetOffset(d);
      InternalImageType::Pointer inertia=InternalImageType::New();
      inertia->FillBuffer(0);
      InternalImageType::Pointer correlation=InternalImageType::New();
      correlation->FillBuffer(0);
      InternalImageType::Pointer energy=InternalImageType::New();
      energy->FillBuffer(0);
      InternalImageType::Pointer entropy=InternalImageType::New();
      entropy->FillBuffer(0);
      InternalImageType::Pointer inversedifferencemoment=InternalImageType::New();
      inversedifferencemoment->FillBuffer(0);
      InternalImageType::Pointer clustershade=InternalImageType::New();
      clustershade->FillBuffer(0);
      InternalImageType::Pointer clusterprominence=InternalImageType::New();
      clusterprominence->FillBuffer(0);
      InternalImageType::Pointer haralickcorrelation=InternalImageType::New();
      haralickcorrelation->FillBuffer(0);

      calcTextureFeatureImage(offset, image, inertia, correlation, energy, entropy, inversedifferencemoment,
                               clustershade, clusterprominence, haralickcorrelation);
      
      std::cout << "Checkpoint 3: After computing" << std::endl;

      writer->SetInput(inertia);
      std::stringstream ssInertia;
      ssInertia << "Inertia" << d << ".nrrd";
      writer->SetFileName(ssInertia.str());
      writer->Update();

      writer->SetInput(correlation);
      std::stringstream ssCorrelation;
      ssCorrelation << "Correlation" << d << ".nrrd";
      writer->SetFileName(ssCorrelation.str());
      writer->Update();

      writer->SetInput(energy);
      std::stringstream ssEnergy;
      ssEnergy << "Energy" << d << ".nrrd";
      writer->SetFileName(ssEnergy.str());
      writer->Update();
      std::cout<<'\n';

      writer->SetInput(entropy);
      std::stringstream ssEntropy;
      ssEntropy << "Entropy" << d << ".nrrd";
      writer->SetFileName(ssEntropy.str());
      writer->Update();
      std::cout<<'\n';

      writer->SetInput(inversedifferencemoment);
      std::stringstream ssIDM;
      ssIDM << "InverseDifferenceMoment" << d << ".nrrd";
      writer->SetFileName(ssIDM.str());
      writer->Update();
      std::cout<<'\n';

      writer->SetInput(clustershade);
      std::stringstream ssClusterShade;
      ssClusterShade << "ClusterShade" << d << ".nrrd";
      writer->SetFileName(ssClusterShade.str());
      writer->Update();
      std::cout<<'\n';

      writer->SetInput(clusterprominence);
      std::stringstream ssClusterProminence;
      ssClusterProminence << "ClusterProminence" << d << ".nrrd";
      writer->SetFileName(ssClusterProminence.str());
      writer->Update();
      std::cout<<'\n';

      writer->SetInput(haralickcorrelation);
      std::stringstream ssHaralickCorr;
      ssHaralickCorr << "HaralickCorrelation" << d << ".nrrd";
      writer->SetFileName(ssHaralickCorr.str());
      writer->Update();
      std::cout<<'\n';

  }

  return EXIT_SUCCESS;
}
