/*=========================================================================

  Program:   T2 fitting
  Module:    Compute T2 Relaxation Map Filter
  Language:  C++  

  Copyright (c) University of North Carolina at Chapel Hill (UNC-CH) All Rights Reserved.

  See License.txt for details.

==========================================================================*/
#ifndef __t2Fitting_h
#define __t2Fitting_h

#include <vnl/vnl_least_squares_function.h>

class t2Fitting: public vnl_least_squares_function
{
public: 
	t2Fitting(int vectorsize = 10);
	~t2Fitting();

	virtual void f(vnl_vector<double> const &x, vnl_vector<double> &fx);

	void SetX(vnl_vector<double> X){
		m_X = X;
	}
	void SetY(vnl_vector<double> Y){
		m_Y = Y;
	}	

	void SetM0(double m0){
		m_M0 = m0;
	}

private:
	vnl_vector<double> m_X;
	vnl_vector<double> m_Y;	
	double m_M0;
};

#endif