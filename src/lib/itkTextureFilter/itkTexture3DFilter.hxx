/*=========================================================================
 
 Program:   Texture Analysis
 Module:    $$
 Language:  C++
 Date:      $Date: xxx-xx-xx $
 Version:   $Revision: xxx $
 
 Copyright (c) University of North Carolina at Chapel Hill (UNC-CH) All Rights Reserved.
 
 See License.txt or http://www.slicer.org/copyright/copyright.txt for details.
 
 ==========================================================================*/
#ifndef __itkTexture3DFilter_hxx
#define __itkTexture3DFilter_hxx

#include "itkTexture3DFilter.h"

namespace itk
{

template <class TInput, class TOutput>
Texture3DFilter<TInput, TOutput>
::Texture3DFilter()
{
  this->SetNumberOfRequiredInputs( 2 );
  this->SetNumberOfRequiredOutputs( 9 );
  m_OutputSpacing.Fill( 1.0 );
  m_OutputOrigin.Fill( 0.0 );
  m_OutputDirection.SetIdentity();
  m_OutputSize.Fill( 0 );
  //m_DefaultPixelValue = static_cast<OutputDataType>( ZERO );
}

template <class TInput, class TOutput>
void
Texture3DFilter<TInput, TOutput>
::BeforeThreadedGenerateData()
{

  // m_Interpolator->SetDefaultPixelValue( m_DefaultPixelValue ) ;
  //m_DefaultTensor.SetIdentity();
  //m_DefaultTensor *= this->m_DefaultPixelValue;
}

template <class TInput, class TOutput>
void
Texture3DFilter<TInput, TOutput>
::SetInput( InputImagePointerType inputImage )
{
  this->Superclass::SetNthInput( 0, inputImage );
}

template <class TInput, class TOutput>
void
Texture3DFilter<TInput, TOutput>
::SetInputROI( InputROIImagePointerType inputROIImage )
{
  this->Superclass::SetNthInput( 1, inputROIImage );
}

template <class TInput, class TOutput>
void
Texture3DFilter<TInput, TOutput>
::ThreadedGenerateData( const OutputImageRegionType & outputRegionForThread,
                        ThreadIdType itkNotUsed(threadId) )
{
  OutputImagePointerType outputImagePtr = this->GetOutput( 0 );
  IteratorType           it( outputImagePtr, outputRegionForThread );

  typename OutputImageType::IndexType index;
  Point<double, 3> point;
  for( it.GoToBegin(); !it.IsAtEnd(); ++it )
    {
    index = it.GetIndex();
    outputImagePtr->TransformIndexToPhysicalPoint( index, point );
 // ---  const Point<double, 3> pointTransform
 // ---    = m_Transform->EvaluateTensorPosition( point );
  //  if( m_Interpolator->IsInsideBuffer( pointTransform ) )
   //   {
   //   inputTensor = m_Interpolator->Evaluate( pointTransform );
    //  outputTensor = m_Transform->EvaluateTransformedTensor( inputTensor,
                                                       //      point );
      it.Set( 100 );
     // }
   // else
    //  {
      //it.Set( 0 );
      }
    //}
}

template <class TInput, class TOutput>
void
Texture3DFilter<TInput, TOutput>
::SetOutputParametersFromImage( InputImagePointerType Image )
{
  typename OutputImageType::RegionType region;
  region = Image->GetLargestPossibleRegion();
  m_OutputSize = region.GetSize();
  m_OutputSpacing = Image->GetSpacing();
  m_OutputDirection = Image->GetDirection();
  m_OutputOrigin = Image->GetOrigin();
}
/*
template <class TInput, class TOutput>
DataObject::Pointer
Texture3DFilter<TInput, TOutput>
::MakeOutput(unsigned int idx)
{
  DataObject::Pointer output;

  switch ( idx )
    {
    case 0:
      output = ( OutputImagePointerType::New() ).GetPointer();
      break;
    case 1:
      output = ( OutputImagePointerType::New() ).GetPointer();
      break;
    case 2:
      output = ( OutputImagePointerType::New() ).GetPointer();
      break;
    case 3:
      output = ( OutputImagePointerType::New() ).GetPointer();
      break;
    case 4:
      output = ( OutputImagePointerType::New() ).GetPointer();
      break;
    case 5:
      output = ( OutputImagePointerType::New() ).GetPointer();
      break;
    case 6:
      output = ( OutputImagePointerType::New() ).GetPointer();
      break;
    case 7:
      output = ( OutputImagePointerType::New() ).GetPointer();
      break;
    case 8:
      output = ( OutputImagePointerType::New() ).GetPointer();
      break;
    default:
      std::cerr << "No output " << idx << std::endl;
      output = NULL;
      break;
    }
  return output.GetPointer();
}*/

/**
 * Inform pipeline of required output region
 */
template <class TInput, class TOutput>
void
Texture3DFilter<TInput, TOutput>
::GenerateOutputInformation(unsigned int idx)
{
  // call the superclass' implementation of this method
  Superclass::GenerateOutputInformation();
  // get pointers to the input and output
  OutputImagePointerType outputPtr = this->GetOutput( idx );

  if( !outputPtr )
    {
    return;
    }
  outputPtr->SetSpacing( m_OutputSpacing );
  outputPtr->SetOrigin( m_OutputOrigin );
  outputPtr->SetDirection( m_OutputDirection );
  // Set the size of the output region
  typename OutputImageType::RegionType outputLargestPossibleRegion;
  outputLargestPossibleRegion.SetSize( m_OutputSize );
  typename OutputImageType::IndexType index;
  for( int i = 0; i < 3; i++ )
    {
    index[i] = 0;
    }
  outputLargestPossibleRegion.SetIndex( index );
  outputPtr->SetRegions( outputLargestPossibleRegion );
  return;
}

/**
 * Set up state of filter after multi-threading.
 */
template <class TInput, class TOutput>
void
Texture3DFilter<TInput, TOutput>
::AfterThreadedGenerateData()
{
  //m_Interpolator->SetInputImage( NULL );
}

/**
 * Inform pipeline of necessary input image region
 *
 * Determining the actual input region is non-trivial, especially
 * when we cannot assume anything about the transform being used.
 * So we do the easy thing and request the entire input image.
 */
template <class TInput, class TOutput>
void
Texture3DFilter<TInput, TOutput>
::GenerateInputRequestedRegion()
{
  // call the superclass's implementation of this method
  Superclass::GenerateInputRequestedRegion();

  if( !this->GetInput() )
    {
    return;
    }
  // get pointers to the input and output
  InputImagePointerType inputPtr  =
    const_cast<InputImageType *>( this->GetInput() );

  // Request the entire input image
  typename InputImageType::RegionType inputRegion;
  inputRegion = inputPtr->GetLargestPossibleRegion();
  inputPtr->SetRequestedRegion( inputRegion );
  return;
}

template <class TInput, class TOutput>
TOutput *
Texture3DFilter<TInput, TOutput>
::GetOutput(unsigned int idx)
{
  return dynamic_cast< TOutput * >(
           this->ProcessObject::GetOutput(idx) );
}

template <class TInput, class TOutput>
Matrix<double, 3, 3>
Texture3DFilter<TInput, TOutput>
::GetOutputMeasurementFrame()
{
  Matrix<double, 3, 3> mat;
  mat.SetIdentity();
  return mat;
}

} // end namespace itk
#endif
