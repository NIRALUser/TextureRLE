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
#define false 0
#define true !false
#define progtitle    "POLYFIT"
#define progversion  "1.0.0"
#define progdate     "08.03.2003"
#define progauther   "K. Hough"
//#define MAXPAIRS 50
#define MAXPAIRS 9       // 9 or 4
#define MAXORDER 8      /* Can do higher, but who cares?   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>      /* need functions 'toupper' and tolower'   */
#include <float.h>      /* defines LDBL_MAX*/
#include <math.h>
#include <time.h>       /* for date marking of output data files   */

//using namespace std;

#ifndef DMDCURVEFIT    // prevent for redefine
#define DMDCURVEFIT    

#ifdef __cplusplus
extern "C" {
#endif

#include "f2c.h" 
void sgels_(char *trans, integer *m, integer *n, integer *nrhs, real *a, integer *lda, real *b, integer *ldb, real *work, integer *lwork, integer *info);

#ifdef __cplusplus
}
#endif


class DMDCurveFit
{
    private:
        int data_pairs, order;
        float correl_coef;
        long double xmin, xmax, ymin, ymax;
        long double polycoefs[MAXPAIRS];
        int debug;   /* enable display of results on screen -- reset via command line to true or false */

        int polynomfit( void );
        void datasort ( void );
        float poly_correlcoef( void );
        void get_xy_max_min ( void );
        void reporterror( char erstr[] );
        long double ldpow( long double n, unsigned p );
 
        double data_array[2][MAXPAIRS]; 

    public:
        DMDCurveFit( ) {
            
        }

        double polycurvefit( double data[2][MAXPAIRS] );
        double lslcurvefit( double data[2][MAXPAIRS] );

};


#endif
