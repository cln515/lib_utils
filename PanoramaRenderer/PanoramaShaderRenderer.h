//#include<Windows.h>
#define PI_VAL M_PI

#define NEAR_ 0.4
#define FAR_ 250
#include <windows.h>
//#include<gl/glew.h>
#include<fstream>
#include <iostream>
#include"Eigen/Eigen"
#include"Eigen/Core"


#include<ShaderPolarCoordinate\ShaderPolarCoordinate.h>

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
	unsigned char* rgbaPointer;
	int dataNum;
	int vertexNumber;
	int meshNumber;
	GLfloat* vertexArray;
	GLuint* meshArray;
	GLfloat* reflectanceImage;
	GLubyte* colorImage;
	int viewHeight;
	GLfloat* depthArray;
	GLfloat* normArray;
	ShaderPolarCoordinate*  spc_;	
	GLint view[4];

public:
	PanoramaRenderer(){dataNum=0;
	
	    //spc_ = new ShaderPolarCoordinate(40, 500, 0.0f, 0.0f);
		//spc_->InitShader();
	};
	void setData(float* vertex,unsigned int* face,float* reflectance,int vertNum,int meshNum);
	void setDataRGB(float* vertex,unsigned int* face,unsigned char* rgba,int vertNum,int meshNum);
	void setDataRGBfromReflectance(float* vertex,unsigned int* face,float* reflectance,int vertNum,int meshnum);
	void render(Vector3d& center,Matrix3d& viewDirection,int viewSize);
	void renderColor(Vector3d& center,Matrix3d& viewDirection,int viewSize);
	void renderColor(Matrix4d& cameraParameter,int viewSize);
	void render_dot(Vector3d& center,Matrix3d& viewDirection,int viewSize);
	void render_shader(Matrix4d& cameraParameter,int viewSize);

	void render_shader_main(Matrix4d& cameraParameter,int viewSize);
	void render_shader_main();
	void render_shader_viewset(Matrix4d& cameraParameter,int viewSize);

	void shaderSetup();
	void render_shader(Matrix4d& cameraParameter);

	void outputReflectance(string fileName);
	void outputColor(string fileName);
	void outputDepth(string fileName,float rankWidth);
	void outputNorm(string fileName);
	void setImagePointer(float*& reflectanceImage_,float*& depthImage_,float*& normImage_){reflectanceImage_=(float*)reflectanceImage;depthImage_=(float*)depthArray;normImage_=(float*)normArray;};
	void clear(){free(vertexPointers);free(facePointers);free(reflectancePointers);free(meshNumArray);free(reflectanceImage);free(depthArray);};
	void clearImage(){if(normArray)free(normArray);if(reflectanceImage)free(reflectanceImage);if(depthArray)free(depthArray);if(colorImage)free(colorImage);}

	void shaderEnable(int mode=1){spc_->EnableShader(mode);};
	void shaderDisable(){spc_->DisableShader();};

};