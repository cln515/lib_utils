//#include<Windows.h>

#define _USE_MATH_DEFINES
#include <math.h>
#define PI_VAL M_PI
#include <windows.h>
//#include<gl/glew.h>
#include<fstream>
#include <iostream>
#include"Eigen/Eigen"
#include"Eigen/Core"

#include<gl\GL.h>
#include<gl\GLU.h>

using namespace Eigen;
using namespace std;

class PanoramaRenderer{
private:
	float** vertexPointers;
	unsigned int**facePointers;
	float**reflectancePointers;
	unsigned char**rgbaPointers;
	int* meshNumArray;
	int* vtNumArray;
	int dataNum;
	GLfloat* reflectanceImage;
	GLubyte* colorImage;
	int viewHeight;
	GLfloat* depthArray;
	GLfloat* normArray;
	bool bNormalImage;
	double depthResolution;
public:
	PanoramaRenderer(){dataNum=0;bNormalImage=false;depthResolution=0.1;};
	void setDepthResolution(double depthResolution_){depthResolution=depthResolution_;}
	void setData(float* vertex,unsigned int* face,float* reflectance,int vertNum,int meshNum);
	void setOutputNorm(bool bNormalImage_){bNormalImage=bNormalImage_;};
	void setDataRGB(float* vertex, unsigned int* face,unsigned char* rgba,int vertNum,int meshNum);
	void render(Vector3d& center,Matrix3d& viewDirection,int viewSize);
	void render(Matrix4d& cameraParameter,int viewSize);
	void renderColor(Vector3d& center,Matrix3d& viewDirection,int viewSize);
	void renderColor(Matrix4d& cameraParameter,int viewSize);
	void render_dot(Vector3d& center,Matrix3d& viewDirection,int viewSize);
	void outputReflectance(string fileName);
	void outputColor(string fileName);
	//znear=0, zfer=500, depth value in array*500(m)=real depth value
	void outputDepth(string fileName);
	void outputNorm(string fileName);
	float* getDepthData(){return depthArray;};
	unsigned char* getColorData() { return colorImage; };
	float* getReflectanceData() { return reflectanceImage; };
	void setImagePointer(float*& reflectanceImage_,float*& depthImage_,float*& normImage_){reflectanceImage_=(float*)reflectanceImage;depthImage_=(float*)depthArray;normImage_=(float*)normArray;};
	void clear(){free(vertexPointers);free(facePointers);free(reflectancePointers);free(meshNumArray);free(reflectanceImage);free(depthArray);};
	void clearImage(){if(normArray)free(normArray);if(reflectanceImage)free(reflectanceImage);if(depthArray)free(depthArray);if(colorImage)free(colorImage);}
};