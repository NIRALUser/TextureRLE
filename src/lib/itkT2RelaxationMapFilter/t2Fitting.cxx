#include "t2Fitting.h"

t2Fitting::t2Fitting(int vectorsize)
: vnl_least_squares_function(1, vectorsize, no_gradient){
	m_M0 = 1;
}

t2Fitting::~t2Fitting(){

}

//Here we optmize for x -> -1/T2. Remember to do -1/x when optimization is done
// Yi  =  M0*e^(x*Xi)
// ln(Yi) = ln(M0) + ln(e^(x*Xi))
// This function miminize for x where:
// 0 = ln(M0) + x*Xi - ln(Yi)
void t2Fitting::f(vnl_vector<double> const &x, vnl_vector<double> &fx){
	for(unsigned i = 0; i < m_X.size(); i++){
		fx[i] = log(m_M0) + m_X[i]*x[0] - log(m_Y[i]);
	}
}