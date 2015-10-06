/*=========================================================================
 *
 * Program :   $Insight Segmentation & Registration Toolkit $
 * Module  :   $DMDBiomarkerTool: TextureFeatureCal.h $
 * Purpose :   $The class of calculating features in segmented muscle regions $
 * Language:   $C++ $
 * Date    :   $Date: 2010-06-24 16:35:04 $
 * Version :   $Revision: 0.10 $
 * Authors :   $Jiahui Wang, Martin Styner $
 * Update  :   1. Created the class (06-24-10)
 * Copyright (c) Neuro Image Research and Analysis Lab.@UNC All rights reserved.
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even 
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
 * PURPOSE.
 *
=========================================================================*/

#ifndef DMDMUSCLEFEAT
#define DMDMUSCLEFEAT    // prevent for redefine

#include "DMDData.h"
#include <iostream>
#include <fstream>
//#include "itkScalarImageToCooccurrenceMatrixFilter.h"

#define HISTOGRAM_BIN_SIZE 4000
#define RUN_INTENSITY_LEVEL 64
#define RUN_LENGTH_LEVEL 64
#define INTENSITY_RESCALE_FACTOR 64
#define SEARCHED_PIXEL_VALUE -1
#define BACKGROUND 0
#define SIZE_LABELING_BIN 10000
#define HISTOGRAM_LIMIT 40000
#define INTENSITY_CUT_RATE 0.05
#define NUMBER_MUSCLE 11
#define ROI_SIZE 32
#define CO_OCCURRENCE_LEVEL 64

//////////////////////////////////////////////////////////////////////////
class TextureFeatureCal : public virtual DMDData
{
    private:           
//        float meanCran, meanRect, meanSemit, meanAdductor, meanGracilis, meanBic, meanSemimem, meanVaslat, meanVasmed, meanVasint, meanCaud, sdCran, sdRect, sdSemit, sdAdductor, sdGracilis, sdBic, sdSemimem, sdVaslat, sdVasmed, sdVasint, sdCaud, volCran, volRect, volSemit, volAdductor, volGracilis, volBic, volSemimem, volVaslat, volVasmed, volVasint, volCaud ;
        char **muscleName;
        float meanCran, meanRect, meanSemit, meanAdductor, meanGracilis, meanBic, meanSemimem, meanVaslat, meanVasmed, meanVasint, meanCaud, sdCran, sdRect, sdSemit, sdAdductor, sdGracilis, sdBic, sdSemimem, sdVaslat, sdVasmed, sdVasint, sdCaud, skewCran, skewRect, skewSemit, skewAdductor, skewGracilis, skewBic, skewSemimem, skewVaslat, skewVasmed, skewVasint, skewCaud, kortCran, kortRect, kortSemit, kortAdductor, kortGracilis, kortBic, kortSemimem, kortVaslat, kortVasmed, kortVasint, kortCaud, volCran, volRect, volSemit, volAdductor, volGracilis, volBic, volSemimem, volVaslat, volVasmed, volVasint, volCaud, entropyCran, entropyRect, entropySemit, entropyAdductor, entropyGracilis, entropyBic, entropySemimem, entropyVaslat, entropyVasmed, entropyVasint, entropyCaud;
        float muscleVolume[NUMBER_OF_MUSCLE];
        int labelMin, labelMax;
    public:   
       // TextureFeatureCal():meanCran(0), meanRect(0), meanSemit(0), meanAdductor(0), meanGracilis(0), meanBic(0), meanSemimem(0), meanVaslat(0), meanVasmed(0), meanVasint(0), meanCaud(0), sdCran(0), sdRect(0), sdSemit(0), sdAdductor(0), sdGracilis(0), sdBic(0), sdSemimem(0), sdVaslat(0), sdVasmed(0), sdVasint(0), sdCaud(0), volCran(0), volRect(0), volSemit(0), volAdductor(0), volGracilis(0), volBic(0), volSemimem(0), volVaslat(0), volVasmed(0), volVasint(0), volCaud(0) {                 
        TextureFeatureCal();

        typedef itk::ImageRegionConstIterator< DMDData::OrientedImageType > ConstIteratorType;
        void histogramFeat3DROI( DMDData::OrientedImageType::Pointer mask, DMDData::OrientedImageType::Pointer erodemask, DMDData::OrientedImageType::Pointer data, float voxelSize, std::string featurefilename, bool );
        void runlengthFeat( DMDData::OrientedImageType::Pointer erodemask, DMDData::OrientedImageType::Pointer data, std::string featurefilename);
        void runlengthFeat3D( DMDData::OrientedImageType::Pointer erodemask, DMDData::OrientedImageType::Pointer data, std::string featurefilename, FITKType);
        void runlengthFeat2DROI( DMDData::OrientedImageType::Pointer erodemask, DMDData::OrientedImageType::Pointer data, std::string featurefilename, FITKType);
        void runlengthFeat3DROI( DMDData::OrientedImageType::Pointer erodemask, DMDData::OrientedImageType::Pointer data, std::string featurefilename, FITKType);        
        void calRunLengthFeatures(DMDData::FITKType &SRE, DMDData::FITKType &LRE, DMDData::FITKType &GLN, DMDData::FITKType &RLN, DMDData::FITKType &RP, FITKType np, int runlengthmatrix[RUN_LENGTH_LEVEL][RUN_INTENSITY_LEVEL]);
        void cooccurrenceFeat3DROI( DMDData::OrientedImageType::Pointer erodemask, DMDData::OrientedImageType::Pointer data, std::string featurefilename, FITKType voxelSize);        
        void calCooccurrenceFeatures( DMDData::FITKType &EntropyTmp, DMDData::FITKType &EnergyTmp, DMDData::FITKType &ContrastTmp, DMDData::FITKType &HomoGeneityTmp, float  cooccurrencematrix[CO_OCCURRENCE_LEVEL][CO_OCCURRENCE_LEVEL]);  
 
};

#endif
