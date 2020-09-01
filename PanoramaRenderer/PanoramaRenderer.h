//#include<Windows.h>

#define _USE_MATH_DEFINES
#include <math.h>
#define PI_VAL M_PI
#include<fstream>
#include <iostream>
#include"Eigen/Eigen"
#include"Eigen/Core"



#if defined(_WIN32) || defined(_WIN64)
// Windows 32-bit and 64-bit
#include <Windows.h>
#include<GL/gl.h>
#include<GL/glu.h>

#elif defined(MAC_OSX)
// Mac OSX

#else
// Linux and all others
// Using GCC 4 where hiding attributes is possible
//#include <GL/glew.h>
#include<GL/gl.h>
#include<GL/glu.h>
#include<GL/glx.h>

#endif
#include<GL/gl.h>
#include<GL/glu.h>





using namespace Eigen;
using namespace std;
#ifndef PANORAMA_RENDERER
#define  PANORAMA_RENDERER

enum render_type {
	PERSPECTIVE,
	PANORAMA,
	FISHEYE
};

class PanoramaRenderer {
protected:
	float** vertexPointers;
	unsigned int**facePointers;
	float**reflectancePointers;
	unsigned char**rgbaPointers;
	int* meshNumArray;
	int* vtNumArray;
	int dataNum;
	GLfloat* reflectanceImage = NULL;
	GLubyte* colorImage = NULL;
	int viewHeight;
	GLfloat* depthArray = NULL;
	GLfloat* normArray = NULL;
	bool bNormalImage;
	double depthResolution;
	static int viewHeight_stat;
	static int viewWidth_stat;
	int viewWidth_;
	int viewHeight_;

//	bool persRender = false;
	render_type type;

	double intrinsic[4];
	double znear;
public:


	PanoramaRenderer() { dataNum = 0; bNormalImage = false; depthResolution = 20.0; znear = 0.3; type = PANORAMA; };
	void setDepthFarClip(double depthResolution_) { depthResolution = depthResolution_; }
	void setData(float* vertex, unsigned int* face, float* reflectance, int vertNum, int meshNum);
	void setOutputNorm(bool bNormalImage_) { bNormalImage = bNormalImage_; };
	void setDataRGB(float* vertex, unsigned int* face, unsigned char* rgba, int vertNum, int meshNum);
	void render(Vector3d& center, Matrix3d& viewDirection, int viewSize);
	void render(Matrix4d& cameraParameter, int viewSize);//camera parameter--> tf:camera2world
	void render(Matrix4d cameraParam);
	void renderColor(Vector3d& center, Matrix3d& viewDirection, int viewSize);
	void renderColor(Matrix4d& cameraParameter, int viewSize);
	void renderColor(Matrix4d& cameraParameter);
	void render_dot(Vector3d& center, Matrix3d& viewDirection, int viewSize);
	void render_dot(Matrix4d& cameraParameter, int viewSize);
	void outputReflectance(string fileName);
	void outputColor(string fileName);
	//znear=0, zfer=500, depth value in array*500(m)=real depth value
	void outputDepth(string fileName);
	void outputNorm(string fileName);
	float* getDepthData() { return depthArray; };
	unsigned char* getColorData() { return colorImage; };
	float* getReflectanceData() { return reflectanceImage; };
	void setImagePointer(float*& reflectanceImage_, float*& depthImage_, float*& normImage_) { reflectanceImage_ = (float*)reflectanceImage; depthImage_ = (float*)depthArray; normImage_ = (float*)normArray; };
	void clear() { free(vertexPointers); free(facePointers); free(reflectancePointers); free(meshNumArray); free(reflectanceImage); free(depthArray); };
	void clearImage() {
		if (normArray != NULL)
			free(normArray);
		if (reflectanceImage != NULL)free(reflectanceImage);
		if (depthArray != NULL)free(depthArray);
		if (colorImage != NULL)free(colorImage);
	}
	double getDepthFarClip() {
		return depthResolution;
	};
	void createContext(int viewWidth_,int viewHeight_);
	void createContext();
	void discardContext();
	static void getViewSize(int& w, int &h) { w = viewWidth_stat; h = viewHeight_stat; };
	void setUniqueViewSize(int w,int h){
		viewWidth_ = w;
		viewHeight_ = w;
	}
	void getUniqueViewSize(int& w, int &h) { w = viewWidth_; h = viewHeight_; };
	
	void setPersRender(double cx,double cy,double fx,double fy) {
		type=PERSPECTIVE; 
		intrinsic[0] = cx;
		intrinsic[1] = cy;
		intrinsic[2] = fx;
		intrinsic[3] = fy;
	};
	void setFisheye() { type = FISHEYE; };
	void getPersRender(double& cx, double& cy, double& fx, double& fy, double& zn, double& zf) {
		cx = intrinsic[0];
		cy = intrinsic[1];
		fx = intrinsic[2];
		fy = intrinsic[3];
		zn = znear;
		zf = znear + depthResolution;
	};
	bool isPers() { return type==PERSPECTIVE; };
	render_type getType() { return type; }
private:
#if defined(_WIN32) || defined(_WIN64)
	HDC		_hdc_;
	HBITMAP m_hbitmap;
	HGLRC	_hrc;
#elif defined(__unix__)
	Display *display_;
	GLXPbuffer pbuffer;
	GLXContext openGLContext;
#endif
};

#endif