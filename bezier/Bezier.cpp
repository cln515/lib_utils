
#include "Bezier.h"
using namespace std;
//file format
//x,y\nx2,y2\nx3,y3 ...
void Bezier::readBezierFile(string FileName){
	vector<float> point;
	ifstream ifs(FileName,ios::binary);
	string line_str;
	while(getline(ifs,line_str)){
		float f[5];
		for(int i=0;i<2;i++){
			if(i!=1)f[i]=stof(line_str.substr(0,line_str.find_first_of(",")));
			else f[i]=stof(line_str);
			line_str.erase(0,line_str.find_first_of(",")+1);
		}
		point.push_back(f[0]);
		point.push_back(f[1]);
	}
	section=(point.size()-2)/6;
	pointx=(float*)malloc(sizeof(float)*(section*100+1));
	pointy=(float*)malloc(sizeof(float)*(section*100+1));
	for(int sec=0;sec<section;sec++){
		float bx[4]={point.at(sec*6),point.at(sec*6+2),point.at(sec*6+4),point.at(sec*6+6)};
		float by[4]={point.at(sec*6+1),point.at(sec*6+3),point.at(sec*6+5),point.at(sec*6+7)};
		for(int t=0;t<100;t++){
			float tf=t*0.01;
			pointx[t+sec*100]=bx[0]*(1-tf)*(1-tf)*(1-tf)+3*bx[1]*tf*(1-tf)*(1-tf)+3*bx[2]*tf*tf*(1-tf)+bx[3]*tf*tf*tf;
			pointy[t+sec*100]=by[0]*(1-tf)*(1-tf)*(1-tf)+3*by[1]*tf*(1-tf)*(1-tf)+3*by[2]*tf*tf*(1-tf)+by[3]*tf*tf*tf;
		}
	}
	pointx[section*100]=point.at(section*6);
	pointy[section*100]=point.at(section*6+1);
	coefficient=1.0;
}

float Bezier::getyFromx(float x){
	x*=100;
	for(int t=0;t<section*100;t++){
		if(pointx[t]<=x&&pointx[t+1]>=x){
			double y0,y1;
			y0=pointy[t];
			y1=pointy[t+1];
			return (y0+(y1-y0)*(x-pointx[t])/(pointx[t+1]-pointx[t]))*coefficient;
		}
	}
	return -100000000;
}

void Bezier::setCoefficient(double t){coefficient=t;}