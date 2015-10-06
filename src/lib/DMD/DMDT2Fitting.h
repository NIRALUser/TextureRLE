/*=========================================================================
 *
 * Program :   $Insight Segmentation & Registration Toolkit $
 * Module  :   $DMDBiomarkerTool: DMDBase.h $
 * Purpose :   $The base class of Duchenne Muscle Dystrophy biomarker tools $
 * Language:   $C++ $
 * Date    :   $Date: 2010-06-09 19:07:21 $
 * Version :   $Revision: 0.10 $
 * Authors :   $Jiahui Wang, Martin Styner $
 * Update  :   1. Created the t2 fitting class (06-07-10)
 * Copyright (c) Neuro Image Research and Analysis Lab.@UNC All rights reserved.
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even 
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
 * PURPOSE.
 *
=========================================================================*/

#ifndef DMDT2FITTING
#define DMDT2FITTING    // prevent for redefine
#include "DMDData.h"

class DMDT2Fitting : public virtual DMDData
{
    private:       
    public:   
        DMDT2Fitting() {                 
                  
	}	
	typedef itk::RecursiveGaussianImageFilter< DMDData::OrientedImageType, DMDData::OrientedImageType >  FilterType;	
	void smooth( DMDData::OrientedSeriesReaderType::Pointer reader, DMDData::OrientedWriterType::Pointer & writer );
 
}; 

#endif
