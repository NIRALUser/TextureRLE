/*=========================================================================

  Program:   T2 Relaxation Map
  Module:    Compute T2 Relaxation Map Filter
  Language:  C++  

  Copyright (c) University of North Carolina at Chapel Hill (UNC-CH) All Rights Reserved.

  See License.txt for details.

==========================================================================*/

#ifndef __itkT2RelaxationMapFilter_hxx
#define __itkT2RelaxationMapFilter_hxx

#include "itkT2RelaxationMapFilter.h"

using namespace std;

namespace itk
{

  template <class TInputImage, class TOutputImage>
  T2RelaxationMapFilter<TInputImage, TOutputImage>
  ::T2RelaxationMapFilter()
  {
    this->SetNumberOfRequiredInputs( 1 );
    this->SetNumberOfRequiredOutputs( 1 );

    m_M0 = 1;
  }

  template <class TInput, class TOutput>
  void
  T2RelaxationMapFilter<TInput, TOutput>
  ::BeforeThreadedGenerateData(){
    InputImageConstPointer inputImage = this->GetInput();
    int numComponents = inputImage->GetNumberOfComponentsPerPixel();

    m_EchoTimes = vnl_vector<double>(numComponents);
    double echotime = this->GetEchoTime();

    for(int i = 0; i < numComponents; i++){
      m_EchoTimes[i] = echotime*(i+1);
    }    
  }
  

  template <class TInput, class TOutput>
  void
  T2RelaxationMapFilter<TInput, TOutput>
  ::ThreadedGenerateData( const OutputImageRegionType & outputRegionForThread,
                          ThreadIdType itkNotUsed(threadId)){

    InputImageConstPointer inputImage = this->GetInput();
    InputImageIteratorType  init( inputImage, outputRegionForThread );

    OutputImagePointerType  outputImagePtr = this->GetOutput( 0 );
    OutputImageIteratorType  outit( outputImagePtr, outputRegionForThread );

    init.GoToBegin();
    outit.GoToBegin();

    int numComponents = inputImage->GetNumberOfComponentsPerPixel();
    vnl_vector<double> Y = vnl_vector<double>(numComponents);

    t2Fitting t2fit(numComponents);
    t2fit.SetX(m_EchoTimes);
    t2fit.SetM0(m_M0);

    while(!init.IsAtEnd()){

      InputImagePixelType t2values = init.Get();
      for(int i = 0; i < numComponents; i++){
        Y[i] = t2values[i] + 1;
      }
      
      t2fit.SetY(Y);

      vnl_vector<double> b(1, 1);

      vnl_levenberg_marquardt optimizer(t2fit);
      optimizer.minimize(b);

      if(b[0] != 0 && !isnan(b[0])){
        outit.Set(-1.0/b[0]);
      }

      ++init;
      ++outit;
    }

    
  }

}

#endif