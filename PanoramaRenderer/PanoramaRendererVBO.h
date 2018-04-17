//#include<Windows.h>


#ifndef PRVBO
#define PRVBO

#define PI_VAL M_PI
#include <windows.h>
#include<fstream>
#include <iostream>
#include"Eigen/Eigen"
#include"Eigen/Core"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>



#if defined(WIN32)
//#  pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#  include "PanoramaRenderer\glew.h"
//#  include "PanoramaRenderer\glut.h"
//#  include "PanoramaRenderer\glext.h"
#pragma comment(lib,"glew32.lib");
#elif defined(__APPLE__) || defined(MACOSX)
#  include <GLUT/glut.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/glut.h>
#endif

#define CHECK_FRAMEBUFFER_STATUS() \
{ \
  GLenum status; \
  status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT); \
  switch(status) { \
  case GL_FRAMEBUFFER_COMPLETE_EXT: \
    break; \
  case GL_FRAMEBUFFER_UNSUPPORTED_EXT: \
    fprintf(stderr, "choose different formats\n"); \
    break; \
  default: \
    fprintf(stderr, "programming error; will fail on all hardware: %04x\n", status); \
  } \
}

//#include<gl\GL.h>
//#include<gl\GLU.h>

using namespace Eigen;
using namespace std;

class PanoramaRenderer{
private:
	float** vertexPointers;
	int**facePointers;
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
public:
	PanoramaRenderer(){dataNum=0;};
	void setData(float* vertex,int* face,float* reflectance,int vertNum,int meshNum);
	void setDataRGB(float* vertex,int* face,unsigned char* rgba,int vertNum,int meshNum);
	void render(Vector3d& center,Matrix3d& viewDirection,int viewSize);
	void renderColor(Vector3d& center,Matrix3d& viewDirection,int viewSize);
	void renderColor(Matrix4d& cameraParameter,int viewSize);
	void render_dot(Vector3d& center,Matrix3d& viewDirection,int viewSize);
	void outputReflectance(string fileName);
	void outputColor(string fileName);
	void outputDepth(string fileName,float rankWidth);
	void outputNorm(string fileName);
	void setImagePointer(float*& reflectanceImage_,float*& depthImage_,float*& normImage_){reflectanceImage_=(float*)reflectanceImage;depthImage_=(float*)depthArray;normImage_=(float*)normArray;};
	void clear(){free(vertexPointers);free(facePointers);free(reflectancePointers);free(meshNumArray);free(reflectanceImage);free(depthArray);};
	void clearImage(){if(normArray)free(normArray);if(reflectanceImage)free(reflectanceImage);if(depthArray)free(depthArray);if(colorImage)free(colorImage);}
};

#endif