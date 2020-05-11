#define GOLD 1.618034
#define CGOLD 0.3819660
#define ITMAX 100
#define GLIMIT 100.0
#define ZEPS 1.0e-10
#define TINY 1.0e-20
#define SIGN(a,b) ((b)>=0.0 ? fabs(a):-fabs(a))
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include "Eigen\Eigen"
#include "Eigen\Core"
#include "Eigen\Dense"

using namespace std;
using namespace Eigen;


class optimizedFunction{
public:
	virtual ~optimizedFunction(){};
	virtual bool evaluateFunction(const double*,double&)=0;

};

class optimizedVectorFunction{
public:
	virtual ~optimizedVectorFunction(){};
	virtual bool evaluateFunction(VectorXd&,double&)=0;

};

class optimizedVectorDescentFunction{
public:
	virtual ~optimizedVectorDescentFunction(){};
	virtual bool evaluateFunction(VectorXd&,VectorXd&,double&)=0;

};

class autoOptimizedVectorDescentFunction{
public:
	virtual ~autoOptimizedVectorDescentFunction(){};
	virtual bool evaluateFunction(VectorXd&,double&)=0;
	bool autoEvaluateFunctionAndDiffer(VectorXd& p,VectorXd& diff,VectorXd& interval,int size,double& ans){
		evaluateFunction(p,ans);
		VectorXd p_(diff);
		double d;
		double a1,a2;
		if(method==0){
			for(int i=0;i<size;i++){//’†S·•ª
				d=interval(i);
				p_=p;
				p_(i)+=d;
				evaluateFunction(p_,a1);
				p_(i)-=2*d;
				evaluateFunction(p_,a2);
				diff(i)=-(a2-a1)/(2*d);
			}
		}
		else if (method > 0) {
			for (int i = 0; i < size; i++) {//‘O•û·•ª
				d = interval(i);
				p_ = p;
				p_(i) += d;
				evaluateFunction(p_, a1);

				diff(i) =- (ans - a1) / (d);
			}
		}
		else {
			for (int i = 0; i < size; i++) {//Œã•û·•ª
				d = interval(i);
				p_ = p;
				p_(i) -= d;
				evaluateFunction(p_, a1);

				diff(i) = -(a1-ans) / (d);
			}
		}

		return true;
	};

	int method = 0;

};

void evaluation_mnbrak(optimizedFunction*,double*,double*,double*,double*,double*,double*);
double brent_method(optimizedFunction*,double ax,double bx,double cx,float tol,double& xmin);

void evaluation_mnbrak(optimizedVectorFunction*,VectorXd&,VectorXd&,VectorXd&,double*,double*,double*);
double brent_method(optimizedVectorFunction*,VectorXd&,VectorXd&,VectorXd&,float tol,VectorXd& xmin);

void powellOptimization(optimizedVectorFunction* func,VectorXd& p,int dim,double& ans,double dtol);
void steepestDescentOptimization(optimizedVectorDescentFunction* func,VectorXd& p,int dim,double& ans,double alpha,int maxIter);
void steepestDescentOptimization(optimizedVectorDescentFunction* func, VectorXd& p, int dim, double& ans, VectorXd alpha, int maxIter);
void automaticSteepestDescentOptimization(autoOptimizedVectorDescentFunction* func,VectorXd& p,VectorXd& intervals,int dim,double& ans,double alpha,int maxIter);
void automaticSteepestDescentOptimization(autoOptimizedVectorDescentFunction* func,VectorXd& p,VectorXd& intervals,int dim,double& ans,VectorXd&  alphas,int maxIter);