/*=========================================================================
 *
 * Program :   $Insight Segmentation & Registration Toolkit $
 * Module  :   $DMDBiomarkerTool: DMDCalib.h $
 * Purpose :   $The base class of Duchenne Muscle Dystrophy biomarker tools $
 * Language:   $C++ $
 * Date    :   $Date: 2010-06-08 11:24:41 $
 * Version :   $Revision: 0.10 $
 * Authors :   $Jiahui Wang, Martin Styner $
 * Update  :   1. Created the t2 calibration class (06-08-10) * Copyright (c) Neuro Image Research and Analysis Lab.@UNC All rights reserved.
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even 
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
 * PURPOSE.
 *
=========================================================================*/

#ifndef DMDCALIB    // prevent for redefine
#define DMDCALIB
#include "DMDData.h"
#include "itkCastImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkShiftScaleImageFilter.h"
#include "itkNormalizeImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "itkScalarImageToHistogramGenerator.h"
#include "itkImage.h"


class DMDCalib : public virtual DMDData
{
    private:       
    public:   
        DMDCalib () {                 
	      
	}
	void intensityRescale ( DMDData::OrientedSeriesReaderType::Pointer reader, DMDData::OrientedWriterType::Pointer & writer ) ;
	void linearCalib ( DMDData::OrientedImageType::Pointer &data, DMDData::OrientedImageType::Pointer &output, DMDData::OrientedImageReaderType::Pointer mask, float meanT2, float meanT2FS, float fk, float fb, float integrate ) ;
    
}; 
#endif
