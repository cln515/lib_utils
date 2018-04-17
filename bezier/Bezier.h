#include "Eigen\Eigen"
#include "Eigen\Core"
#include <fstream>
#include <iostream>
#include <vector>
#include <utility\utility.h>


using namespace std;
class Bezier{
private:
	float* startPoint;
	float* pointx;
	float* pointy;
	int section;
	double coefficient;//Ç∆Ç¢Ç§Ç©ÉXÉPÅ[Éã
public:
	void readBezierFile(string);
	float getyFromx(float);
	void setCoefficient(double);
};