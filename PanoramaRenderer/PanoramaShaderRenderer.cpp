#include "PanoramaShaderRenderer.h"


using namespace std;
using namespace Eigen;


HDC dhdc;

void display(float*& vertex,unsigned int*& face,float*& reflectance,int meshNum,int vertNum);
void displayrgb(float*& vertex,unsigned int*& face,unsigned char*& rgba,int meshNum,int vertNum);
void display_points(float*& vertex,int*& face,float*& reflectance,int meshNum,int vertNum);
void sphericalTrans_renderer(Vector3d& ret_,Vector3d& pt,Vector3d& center,Matrix3d& rotMatrix);
void sphericalTrans_renderer(Vector3d& ret_,Vector3d& pt,Matrix4d& cameraParameter);
void Init(int viewSize);
static unsigned int vtxID, nrmID, uvID, idxID, colID;

void PanoramaRenderer::setData(float* vertex,unsigned int* face,float* reflectance,int vertNum,int meshnum){
	if(dataNum==0){
		vertexPointers=(float**)malloc(sizeof(float*)*1);
		facePointers=(unsigned int**)malloc(sizeof(float*)*1);
		reflectancePointers=(float**)malloc(sizeof(float*)*1);
		meshNumArray=(int*)malloc(sizeof(int)*1);
		vtNumArray=(int*)malloc(sizeof(int)*1);
		vertexPointers[0]=vertex;
		facePointers[0]=face;
		reflectancePointers[0]=reflectance;
		meshNumArray[0]=meshnum;
		vtNumArray[0]=vertNum;
		dataNum++;
	}else{
		dataNum++;
		vertexPointers=(float**)realloc(vertexPointers,sizeof(float*)*dataNum);
		facePointers=(unsigned int**)realloc(facePointers,sizeof(int*)*dataNum);
		reflectancePointers=(float**)realloc(reflectancePointers,sizeof(float*)*dataNum);
		meshNumArray=(int*)realloc(meshNumArray,sizeof(int)*dataNum);
		vtNumArray=(int*)realloc(vtNumArray,sizeof(int)*dataNum);
		vertexPointers[dataNum-1]=vertex;
		facePointers[dataNum-1]=face;
		reflectancePointers[dataNum-1]=reflectance;
		meshNumArray[dataNum-1]=meshnum;
		vtNumArray[dataNum-1]=vertNum;
	};
}

void PanoramaRenderer::setDataRGBfromReflectance(float* vertex,unsigned int* face,float* reflectance,int vertNum,int meshnum){

	vertexNumber=vertNum;
	meshNumber=meshnum;
	vertexArray=vertex;
	meshArray=(GLuint*)malloc(sizeof(GLuint)*meshNumber*3);
	for(int i=0;i<meshNumber;i++){
		meshArray[i*3]=face[i*3];
		meshArray[i*3+1]=face[i*3+1];
		meshArray[i*3+2]=face[i*3+2];
	}
	rgbaPointer=(unsigned char*)malloc(sizeof(unsigned char)*4*vertexNumber);
	for(int i=0;i<vertexNumber;i++){
		rgbaPointer[i*4]=rgbaPointer[i*4+1]=rgbaPointer[i*4+2]=(unsigned char)(reflectance[i]*256);
		rgbaPointer[i*4+3]=255;
	}

}

void PanoramaRenderer::setDataRGB(float* vertex,unsigned int* face,unsigned char* rgba,int vertNum,int meshnum){

	vertexNumber=vertNum;
	meshNumber=meshnum;
	vertexArray=vertex;
	meshArray=(GLuint*)malloc(sizeof(GLuint)*meshNumber*3);
	for(int i=0;i<meshNumber;i++)meshArray[i]=face[i];
	rgbaPointer=rgba;

}

void PanoramaRenderer::render_dot(Vector3d& center,Matrix3d& viewDirection,int viewSize){


	//	glGenRenderbuffersEXT(1,&uvID);
		   PIXELFORMATDESCRIPTOR _pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	//	Size of this struct
		1,	//	Versin of this structure
		PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL |
		PFD_GENERIC_ACCELERATED,
//		PFD_GENERIC_FORMAT,
		//	Pixel buffer flags
		PFD_TYPE_RGBA,	//	Type of pixel data
		24,	//	The number of color bitplanes
		0, 0, 0, 0, 0, 0,	//	Number of each color bitplanes and shift count
		0, 0,	//	Number of alpha bitplanes and shift count
		0, 0, 0, 0, 0,	//	Number of accumulation bits
		32,	//	Z depth
		0,	//	Stencil depth
		0,	//	Number of auxiliary buffers
		PFD_MAIN_PLANE,	//	Ignored
		0,	//	Reserved
		0,	//	Ignored
		0,	//	Transparent color value
		0,	//	Ignored
	};
		   GLint view[4];
	   HDC		_hdc_=CreateCompatibleDC(NULL);
	   viewHeight=viewSize;
	DWORD m_DIBWidth = viewSize*2;
	DWORD m_DIBHeight = viewSize;
	DWORD m_BPP = 24;

	// Create BITMAPINFOHEADER
	BITMAPINFOHEADER* m_PBIH = new BITMAPINFOHEADER;
	int iSize = sizeof(BITMAPINFOHEADER);
	::memset(m_PBIH, 0, iSize);

	m_PBIH->biSize = sizeof(BITMAPINFOHEADER);
	m_PBIH->biWidth = m_DIBWidth;
	m_PBIH->biHeight = m_DIBHeight;
	m_PBIH->biPlanes = 1;
	m_PBIH->biBitCount = m_BPP;
	m_PBIH->biCompression = BI_RGB;

	// Create DIB
	void* m_PBits;
	HBITMAP m_hbitmap_old;
	HBITMAP m_hbitmap = ::CreateDIBSection(
		_hdc_,
		(BITMAPINFO*)m_PBIH, DIB_RGB_COLORS,
		&m_PBits, NULL, 0
	);

	m_hbitmap_old = (HBITMAP)::SelectObject(_hdc_, m_hbitmap);
	DWORD dwLength;
	if ((m_DIBWidth*3) % 4==0) /* バッファの１ラインの長さを計算 */
          dwLength=m_DIBWidth*3;
      else
          dwLength=m_DIBWidth*3+(4-(m_DIBWidth*3) % 4);
	
	int		_pfid = ChoosePixelFormat(_hdc_, &_pfd);
	::SetPixelFormat(_hdc_, _pfid, &_pfd);
	HGLRC	_hrc = ::wglCreateContext(_hdc_);
		int argc=0;
	
	

	::wglMakeCurrent(_hdc_,_hrc);
	//glutInit(&argc,NULL);
	GLenum error;
	//glutInitWindowSize(viewSize*2, viewSize);
	//glutCreateWindow("dammy");
	
/*	if((error=glewInit())!=GLEW_OK){
		cout<<error<<endl;
		return;
	};*/
	ShaderPolarCoordinate*  spc_;
    spc_ = new ShaderPolarCoordinate(40, 500, 0.0f, 0.0f);
	spc_->InitShader();
	float points[] ={
        0,0,0,  //v0
        1,0,0,  //v1
        1,1,0,  //v2
        0,1,0,  //v3

        0,0,-1, //v4
        1,0,-1, //v5
        1,1,-1, //v6
        0,1,-1, //v7
};
//頂点番号を知らせる
GLuint index[]={
        0,1,2,3,
        1,5,6,2,
        5,4,7,6,
        4,0,3,7,
        4,5,1,0,
        3,2,6,7
}; 



/*	glGenBuffers(1, &vtxID);
	glBindBuffer(GL_ARRAY_BUFFER, vtxID);
	glBufferData(GL_ARRAY_BUFFER, vertexNumber, vertexArray, GL_STATIC_DRAW);*/
	
//	glGenBuffers(1, &vtxID);
//	glBindBuffer(GL_ARRAY_BUFFER, vtxID);
//	glBufferData(GL_ARRAY_BUFFER, 8, points, GL_STATIC_DRAW);

	/* 法線の登録 */
/*	glGenBuffers(1, &nrmID);
	glBindBuffer(GL_ARRAY_BUFFER, nrmID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(torusNrm), torusNrm, GL_STATIC_DRAW);*/

	/* インデックスの登録 */
/*	glGenBuffers(1, &idxID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshNumber, meshArray, GL_STATIC_DRAW);*/
	/*	glGenBuffers(1, &idxID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24, index, GL_STATIC_DRAW);*/
	
//	glGetIntegerv(GL_VIEWPORT,view);

	
	/*	for(int i=0;i<dataNum;i++){
		float* trsVert=(float*)malloc(sizeof(float)*vtNumArray[i]*3);
		float* vertexp=vertexPointers[i];
		for(int j=0;j<vtNumArray[i];j++){
			Vector3d tp;
			Vector3d pp;
			pp<<vertexp[j*3],vertexp[j*3+1],vertexp[j*3+2];
			
			sphericalTrans_renderer(tp,pp,center,viewDirection);
			trsVert[j*3]=(float)tp(0,0);
			trsVert[j*3+1]=(float)tp(1,0);
			trsVert[j*3+2]=(float)tp(2,0);
//			cout<<tp<<endl;
		}
		display_points(trsVert,facePointers[i],reflectancePointers[i],meshNumArray[i],vtNumArray[i]);
		free(trsVert);
	}*/
		

      


       
		glGetIntegerv(GL_VIEWPORT,view);

				/* 位置座標の設定 */
				glEnableClientState(GL_VERTEX_ARRAY);
				//glBindBuffer(GL_ARRAY_BUFFER, vtxID);
				glVertexPointer(3, GL_FLOAT, 0, vertexArray);
				glEnableClientState(GL_COLOR_ARRAY);
				glColorPointer(4, GL_UNSIGNED_BYTE, 0, rgbaPointer);
				

				/* 法線の設定 */
/*				glEnableClientState(GL_NORMAL_ARRAY);
				glBindBuffer(GL_ARRAY_BUFFER, nrmID);
				glNormalPointer(GL_FLOAT, 0, 0);*/

//レンダリング                _scene->render(0, 0, width_, height_);
				int offset = 0; 
				
				glViewport(0,0,viewSize*2,viewSize);
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				spc_->EnableShader(1);
					glClearColor(1,1,1,1);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glEnable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);
				//glEnable(GL_CULL_FACE);
				//glCullFace(GL_BACK);
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
			//    glOrtho(0, 2*PI_VAL, 0, PI_VAL,0, 500);
				::glOrtho(0.0, M_PI*2, 0.0,M_PI , 1, 500);

				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
				

				float RTforRender[16]={1,0,0,0
					,0,1,0,0
					,0,0,1,0
					,center(0),center(1),center(2),1
				};

				 glMultMatrixf(RTforRender);
				
				glDrawElements(GL_TRIANGLES, meshNumber*3, 
						GL_UNSIGNED_INT, meshArray);
				spc_->DisableShader();
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_COLOR_ARRAY);	
                
	
//	BitBlt(dhdc,0,0,m_DIBWidth,m_DIBHeight,_hdc_,0,0,SRCCOPY);
//	SelectObject(dhdc,m_hbitmap_old);
//	GetDIBits(_hdc_,m_hbitmap,0,m_DIBHeight,lpPixel,(LPBITMAPINFO)m_PBIH,DIB_RGB_COLORS);
	colorImage=(GLubyte*)malloc(sizeof(GLubyte)*view[2]*view[3]*3);
	glReadPixels(view[0],view[1],view[2],view[3],GL_RGB,GL_UNSIGNED_BYTE,colorImage);
	depthArray=(GLfloat*)malloc(sizeof(GLfloat)*view[2]*view[3]);
	glReadPixels(view[0],view[1],view[2],view[3],GL_DEPTH_COMPONENT,GL_FLOAT,depthArray);
	normArray=(GLfloat*)malloc(sizeof(GLfloat)*view[2]*view[3]);
	for(int x=0;x<viewHeight*2;x++){
		for(int y=0;y<viewHeight;y++){
			int idx=y*viewHeight*2+x;
			if(y==0||y==viewHeight-1){normArray[idx]=0;continue;}
			int prexidx=x!=0?y*viewHeight*2+x-1:y*viewHeight*2+viewHeight*2-1;
			int postxidx=x!=viewHeight*2-1?y*viewHeight*2+x+1:y*viewHeight*2;
			int preyidx=idx-viewHeight*2;
			int postyidx=idx+viewHeight*2;
			if(depthArray[idx]==1.0||depthArray[prexidx]==1.0||depthArray[postxidx]==1.0||depthArray[preyidx]==1.0||depthArray[postyidx]==1.0){
				normArray[idx]=0;
				continue;
			}
			double t=M_PI*(depthArray[postxidx]+depthArray[prexidx])*125/viewHeight;//125= viewOrtho far/4
			double d=(depthArray[postxidx]-depthArray[prexidx])*500;
			double t2=M_PI*(depthArray[postyidx]+depthArray[preyidx])*125/viewHeight;//125= viewOrtho far/4
			double d2=(depthArray[postyidx]-depthArray[preyidx])*500;
			normArray[idx]=(t/sqrt(t*t+d*d)+t2/sqrt(t2*t2+d2*d2))/2;//max...2.0
		}
	}

//	GetObject(m_hbitmap,
//	::wglMakeCurrent(0,0);
//	GlobalFree(lpBuf);
//	GlobalFree(lpBuf);
//	free(buffer);
// return 0;
}


void PanoramaRenderer::render_shader(Matrix4d& cameraParameter,int viewSize){


	//	glGenRenderbuffersEXT(1,&uvID);
		   PIXELFORMATDESCRIPTOR _pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	//	Size of this struct
		1,	//	Versin of this structure
		PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL |
		PFD_GENERIC_ACCELERATED,
//		PFD_GENERIC_FORMAT,
		//	Pixel buffer flags
		PFD_TYPE_RGBA,	//	Type of pixel data
		24,	//	The number of color bitplanes
		0, 0, 0, 0, 0, 0,	//	Number of each color bitplanes and shift count
		0, 0,	//	Number of alpha bitplanes and shift count
		0, 0, 0, 0, 0,	//	Number of accumulation bits
		32,	//	Z depth
		0,	//	Stencil depth
		0,	//	Number of auxiliary buffers
		PFD_MAIN_PLANE,	//	Ignored
		0,	//	Reserved
		0,	//	Ignored
		0,	//	Transparent color value
		0,	//	Ignored
	};
		   GLint view[4];
	   HDC		_hdc_=CreateCompatibleDC(NULL);
	   viewHeight=viewSize;
	DWORD m_DIBWidth = viewSize*2;
	DWORD m_DIBHeight = viewSize;
	DWORD m_BPP = 24;

	// Create BITMAPINFOHEADER
	BITMAPINFOHEADER* m_PBIH = new BITMAPINFOHEADER;
	int iSize = sizeof(BITMAPINFOHEADER);
	::memset(m_PBIH, 0, iSize);

	m_PBIH->biSize = sizeof(BITMAPINFOHEADER);
	m_PBIH->biWidth = m_DIBWidth;
	m_PBIH->biHeight = m_DIBHeight;
	m_PBIH->biPlanes = 1;
	m_PBIH->biBitCount = m_BPP;
	m_PBIH->biCompression = BI_RGB;

	// Create DIB
	void* m_PBits;
	HBITMAP m_hbitmap_old;
	HBITMAP m_hbitmap = ::CreateDIBSection(
		_hdc_,
		(BITMAPINFO*)m_PBIH, DIB_RGB_COLORS,
		&m_PBits, NULL, 0
	);

	m_hbitmap_old = (HBITMAP)::SelectObject(_hdc_, m_hbitmap);
	DWORD dwLength;
	if ((m_DIBWidth*3) % 4==0) /* バッファの１ラインの長さを計算 */
          dwLength=m_DIBWidth*3;
      else
          dwLength=m_DIBWidth*3+(4-(m_DIBWidth*3) % 4);
	
	int		_pfid = ChoosePixelFormat(_hdc_, &_pfd);
	::SetPixelFormat(_hdc_, _pfid, &_pfd);
	HGLRC	_hrc = ::wglCreateContext(_hdc_);
		int argc=0;
	
	

	::wglMakeCurrent(_hdc_,_hrc);
	//glutInit(&argc,NULL);
	GLenum error;
	//glutInitWindowSize(viewSize*2, viewSize);
	//glutCreateWindow("dammy");
	
/*	if((error=glewInit())!=GLEW_OK){
		cout<<error<<endl;
		return;
	};*/
	ShaderPolarCoordinate*  spc_;
    spc_ = new ShaderPolarCoordinate(40, 500, 0.0f, 0.0f);
	spc_->InitShader();
		glGetIntegerv(GL_VIEWPORT,view);

				/* 位置座標の設定 */
				glEnableClientState(GL_VERTEX_ARRAY);
				//glBindBuffer(GL_ARRAY_BUFFER, vtxID);
				glVertexPointer(3, GL_FLOAT, 0, vertexArray);
				glEnableClientState(GL_COLOR_ARRAY);
				glColorPointer(4, GL_UNSIGNED_BYTE, 0, rgbaPointer);
				

				int offset = 0; 
				
				glViewport(0,0,viewSize*2,viewSize);
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				spc_->EnableShader(1);
					glClearColor(1,1,1,1);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glEnable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);
				//glEnable(GL_CULL_FACE);
				//glCullFace(GL_BACK);
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
			//    glOrtho(0, 2*PI_VAL, 0, PI_VAL,0, 500);
				::glOrtho(0.0, M_PI*2, 0.0,M_PI , 1, 500);

				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
				

				float RTforRender[16]={cameraParameter(0,0),cameraParameter(1,0),cameraParameter(2,0),cameraParameter(3,0)
					,cameraParameter(0,1),cameraParameter(1,1),cameraParameter(2,1),cameraParameter(3,1)
					,cameraParameter(0,2),cameraParameter(1,2),cameraParameter(2,2),cameraParameter(3,2)
					,cameraParameter(0,3),cameraParameter(1,3),cameraParameter(2,3),cameraParameter(3,3)
				};

				 glMultMatrixf(RTforRender);
				
				glDrawElements(GL_TRIANGLES, meshNumber*3, 
						GL_UNSIGNED_INT, meshArray);
				spc_->DisableShader();
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_COLOR_ARRAY);	
                
	colorImage=(GLubyte*)malloc(sizeof(GLubyte)*view[2]*view[3]*3);
	glReadPixels(view[0],view[1],view[2],view[3],GL_RGB,GL_UNSIGNED_BYTE,colorImage);
	depthArray=(GLfloat*)malloc(sizeof(GLfloat)*view[2]*view[3]);
	glReadPixels(view[0],view[1],view[2],view[3],GL_DEPTH_COMPONENT,GL_FLOAT,depthArray);
	normArray=(GLfloat*)malloc(sizeof(GLfloat)*view[2]*view[3]);
	for(int x=0;x<viewHeight*2;x++){
		for(int y=0;y<viewHeight;y++){
			int idx=y*viewHeight*2+x;
			if(y==0||y==viewHeight-1){normArray[idx]=0;continue;}
			int prexidx=x!=0?y*viewHeight*2+x-1:y*viewHeight*2+viewHeight*2-1;
			int postxidx=x!=viewHeight*2-1?y*viewHeight*2+x+1:y*viewHeight*2;
			int preyidx=idx-viewHeight*2;
			int postyidx=idx+viewHeight*2;
			if(depthArray[idx]==1.0||depthArray[prexidx]==1.0||depthArray[postxidx]==1.0||depthArray[preyidx]==1.0||depthArray[postyidx]==1.0){
				normArray[idx]=0;
				continue;
			}
			double t=M_PI*(depthArray[postxidx]+depthArray[prexidx])*125/viewHeight;//125= viewOrtho far/4
			double d=(depthArray[postxidx]-depthArray[prexidx])*500;
			double t2=M_PI*(depthArray[postyidx]+depthArray[preyidx])*125/viewHeight;//125= viewOrtho far/4
			double d2=(depthArray[postyidx]-depthArray[preyidx])*500;
			normArray[idx]=(t/sqrt(t*t+d*d)+t2/sqrt(t2*t2+d2*d2))/2;//max...2.0
		}
	}
}

void PanoramaRenderer::render_shader_main(Matrix4d& cameraParameter,int viewSize){

		glGetIntegerv(GL_VIEWPORT,view);
		/* 位置座標の設定 */
		glEnableClientState(GL_VERTEX_ARRAY);
		//glBindBuffer(GL_ARRAY_BUFFER, vtxID);
		glVertexPointer(3, GL_FLOAT, 0, vertexArray);
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, rgbaPointer);
				

		int offset = 0; 
				
		glViewport(0,0,viewSize*2,viewSize);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		spc_->EnableShader(1);
			glClearColor(1,1,1,1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
	//    glOrtho(0, 2*PI_VAL, 0, PI_VAL,0, 500);
		::glOrtho(0.0, M_PI*2, 0.0,M_PI , 1, 500);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		float RTforRender[16]={cameraParameter(0,0),cameraParameter(1,0),cameraParameter(2,0),cameraParameter(3,0)
			,cameraParameter(0,1),cameraParameter(1,1),cameraParameter(2,1),cameraParameter(3,1)
			,cameraParameter(0,2),cameraParameter(1,2),cameraParameter(2,2),cameraParameter(3,2)
			,cameraParameter(0,3),cameraParameter(1,3),cameraParameter(2,3),cameraParameter(3,3)
		};
			glMultMatrixf(RTforRender);
		glDrawElements(GL_TRIANGLES, meshNumber*3, 
				GL_UNSIGNED_INT, meshArray);
		spc_->DisableShader();
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);	
                
	/*colorImage=(GLubyte*)malloc(sizeof(GLubyte)*view[2]*view[3]*3);
	glReadPixels(view[0],view[1],view[2],view[3],GL_RGB,GL_UNSIGNED_BYTE,colorImage);
	depthArray=(GLfloat*)malloc(sizeof(GLfloat)*view[2]*view[3]);
	glReadPixels(view[0],view[1],view[2],view[3],GL_DEPTH_COMPONENT,GL_FLOAT,depthArray);*/
	//normArray=(GLfloat*)malloc(sizeof(GLfloat)*view[2]*view[3]);
	//for(int x=0;x<viewHeight*2;x++){
	//	for(int y=0;y<viewHeight;y++){
	//		int idx=y*viewHeight*2+x;
	//		if(y==0||y==viewHeight-1){normArray[idx]=0;continue;}
	//		int prexidx=x!=0?y*viewHeight*2+x-1:y*viewHeight*2+viewHeight*2-1;
	//		int postxidx=x!=viewHeight*2-1?y*viewHeight*2+x+1:y*viewHeight*2;
	//		int preyidx=idx-viewHeight*2;
	//		int postyidx=idx+viewHeight*2;
	//		if(depthArray[idx]==1.0||depthArray[prexidx]==1.0||depthArray[postxidx]==1.0||depthArray[preyidx]==1.0||depthArray[postyidx]==1.0){
	//			normArray[idx]=0;
	//			continue;
	//		}
	//		double t=M_PI*(depthArray[postxidx]+depthArray[prexidx])*125/viewHeight;//125= viewOrtho far/4
	//		double d=(depthArray[postxidx]-depthArray[prexidx])*500;
	//		double t2=M_PI*(depthArray[postyidx]+depthArray[preyidx])*125/viewHeight;//125= viewOrtho far/4
	//		double d2=(depthArray[postyidx]-depthArray[preyidx])*500;
	//		normArray[idx]=(t/sqrt(t*t+d*d)+t2/sqrt(t2*t2+d2*d2))/2;//max...2.0
	//	}
	//}
}

void PanoramaRenderer::render_shader_viewset(Matrix4d& cameraParameter,int viewSize){

		glGetIntegerv(GL_VIEWPORT,view);
		/* 位置座標の設定 */
		int offset = 0; 
				
		glViewport(0,0,viewSize*2,viewSize);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

			glClearColor(0,0,0,1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		::glOrtho(0.0, M_PI*2, 0,M_PI , NEAR_, FAR_);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		float RTforRender[16]={cameraParameter(0,0),cameraParameter(1,0),cameraParameter(2,0),cameraParameter(3,0)
			,cameraParameter(0,1),cameraParameter(1,1),cameraParameter(2,1),cameraParameter(3,1)
			,cameraParameter(0,2),cameraParameter(1,2),cameraParameter(2,2),cameraParameter(3,2)
			,cameraParameter(0,3),cameraParameter(1,3),cameraParameter(2,3),cameraParameter(3,3)
		};
			glMultMatrixf(RTforRender);
		
                
}

void PanoramaRenderer::render_shader_main(){

		spc_->EnableShader(2);
		glGetIntegerv(GL_VIEWPORT,view);
		/* 位置座標の設定 */
		glEnableClientState(GL_VERTEX_ARRAY);
		//glBindBuffer(GL_ARRAY_BUFFER, vtxID);
		glVertexPointer(3, GL_FLOAT, 0, vertexArray);
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, rgbaPointer);
		
		if(meshNumber>0&&vertexNumber>0)glDrawElements(GL_TRIANGLES, meshNumber*3, 
				GL_UNSIGNED_INT, meshArray);

		//glDrawElements(GL_POINTS,vertexNumber,
		//	GL_UNSIGNED_INT, 0);
		spc_->DisableShader();
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);	
                
}

void PanoramaRenderer::render(Vector3d& center,Matrix3d& viewDirection,int viewSize){
		   PIXELFORMATDESCRIPTOR _pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	//	Size of this struct
		1,	//	Versin of this structure
		PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL |
		PFD_GENERIC_ACCELERATED,
//		PFD_GENERIC_FORMAT,
		//	Pixel buffer flags
		PFD_TYPE_RGBA,	//	Type of pixel data
		24,	//	The number of color bitplanes
		0, 0, 0, 0, 0, 0,	//	Number of each color bitplanes and shift count
		0, 0,	//	Number of alpha bitplanes and shift count
		0, 0, 0, 0, 0,	//	Number of accumulation bits
		32,	//	Z depth
		0,	//	Stencil depth
		0,	//	Number of auxiliary buffers
		PFD_MAIN_PLANE,	//	Ignored
		0,	//	Reserved
		0,	//	Ignored
		0,	//	Transparent color value
		0,	//	Ignored
	};

	




		   GLint view[4];
	   HDC		_hdc_=CreateCompatibleDC(NULL);
	   viewHeight=viewSize;
	DWORD m_DIBWidth = viewSize*2;
	DWORD m_DIBHeight = viewSize;
	DWORD m_BPP = 24;

	// Create BITMAPINFOHEADER
	BITMAPINFOHEADER* m_PBIH = new BITMAPINFOHEADER;
	int iSize = sizeof(BITMAPINFOHEADER);
	::memset(m_PBIH, 0, iSize);

	m_PBIH->biSize = sizeof(BITMAPINFOHEADER);
	m_PBIH->biWidth = m_DIBWidth;
	m_PBIH->biHeight = m_DIBHeight;
	m_PBIH->biPlanes = 1;
	m_PBIH->biBitCount = m_BPP;
	m_PBIH->biCompression = BI_RGB;

	// Create DIB
	void* m_PBits;
	HBITMAP m_hbitmap_old;
	HBITMAP m_hbitmap = ::CreateDIBSection(
		_hdc_,
		(BITMAPINFO*)m_PBIH, DIB_RGB_COLORS,
		&m_PBits, NULL, 0
	);

	m_hbitmap_old = (HBITMAP)::SelectObject(_hdc_, m_hbitmap);
	DWORD dwLength;
	if ((m_DIBWidth*3) % 4==0) /* バッファの１ラインの長さを計算 */
          dwLength=m_DIBWidth*3;
      else
          dwLength=m_DIBWidth*3+(4-(m_DIBWidth*3) % 4);
//	LPBYTE lpPixel,lpBuf;
//	lpBuf=(LPBYTE)GlobalAlloc(GPTR,sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwLength*m_DIBHeight);
//	lpPixel=lpBuf+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	
	int		_pfid = ChoosePixelFormat(_hdc_, &_pfd);
	::SetPixelFormat(_hdc_, _pfid, &_pfd);
	HGLRC	_hrc = ::wglCreateContext(_hdc_);
	::wglMakeCurrent(_hdc_,_hrc);
	Init(viewSize);
	glGetIntegerv(GL_VIEWPORT,view);
	for(int i=0;i<dataNum;i++){
		float* trsVert=(float*)malloc(sizeof(float)*vtNumArray[i]*3);
		float* vertexp=vertexPointers[i];
		for(int j=0;j<vtNumArray[i];j++){
			Vector3d tp;
			Vector3d pp;
			pp<<vertexp[j*3],vertexp[j*3+1],vertexp[j*3+2];
			
			sphericalTrans_renderer(tp,pp,center,viewDirection);
			trsVert[j*3]=(float)tp(0,0);
			trsVert[j*3+1]=(float)tp(1,0);
			trsVert[j*3+2]=(float)tp(2,0);
//			cout<<tp<<endl;
		}
		display(trsVert,facePointers[i],reflectancePointers[i],meshNumArray[i],vtNumArray[i]);
		free(trsVert);
	}
	
//	BitBlt(dhdc,0,0,m_DIBWidth,m_DIBHeight,_hdc_,0,0,SRCCOPY);
//	SelectObject(dhdc,m_hbitmap_old);
//	GetDIBits(_hdc_,m_hbitmap,0,m_DIBHeight,lpPixel,(LPBITMAPINFO)m_PBIH,DIB_RGB_COLORS);
	reflectanceImage=(GLfloat*)malloc(sizeof(GLfloat)*view[2]*view[3]);
	glReadPixels(view[0],view[1],view[2],view[3],GL_RED,GL_FLOAT,reflectanceImage);
	depthArray=(GLfloat*)malloc(sizeof(GLfloat)*view[2]*view[3]);
	glReadPixels(view[0],view[1],view[2],view[3],GL_DEPTH_COMPONENT,GL_FLOAT,depthArray);
	normArray=(GLfloat*)malloc(sizeof(GLfloat)*view[2]*view[3]);

	double th=M_PI/viewHeight;
	Vector3d zdrc;

	for(int x=0;x<viewHeight*2;x++){
		for(int y=0;y<viewHeight;y++){
			int idx=y*viewHeight*2+x;
			if(y==0||y==viewHeight-1){normArray[idx]=0;continue;}
			int prexidx=x!=0?y*viewHeight*2+x-1:y*viewHeight*2+viewHeight*2-1;
			int postxidx=x!=viewHeight*2-1?y*viewHeight*2+x+1:y*viewHeight*2;
			int preyidx=idx-viewHeight*2;
			int postyidx=idx+viewHeight*2;
			if(depthArray[idx]==1.0||depthArray[prexidx]==1.0||depthArray[postxidx]==1.0||depthArray[preyidx]==1.0||depthArray[postyidx]==1.0){
				normArray[idx]=0;
				continue;
			}

			/*double t=M_PI*(depthArray[postxidx]+depthArray[prexidx])*125/viewHeight;//125= viewOrtho far/4
			
			double d=(depthArray[postxidx]-depthArray[prexidx])*500;
			double t2=M_PI*(depthArray[postyidx]+depthArray[preyidx])*125/viewHeight;//125= viewOrtho far/4
			double d2=(depthArray[postyidx]-depthArray[preyidx])*500;*/
			Vector3d v1,v2;
			v1<<(depthArray[prexidx]+depthArray[postxidx])*sin(th)*500,0,(depthArray[prexidx]-depthArray[postxidx])*cos(th)*500;
			v2<<0,(depthArray[preyidx]+depthArray[postyidx])*sin(th)*500,(depthArray[preyidx]-depthArray[postyidx])*cos(th)*500;
			Vector3d n;
			n=v1.cross(v2);
			if(n(2)<0)n=-n;
			n.normalize();
			normArray[idx]=n(2);//max...2.0
		}
	}
//	GetObject(m_hbitmap,
	::wglMakeCurrent(0,0);
//	GlobalFree(lpBuf);
//	GlobalFree(lpBuf);
//	free(buffer);
// return 0;
}



void PanoramaRenderer::shaderSetup(){
	spc_=new ShaderPolarCoordinate(5,1500,0.0f,0.0f);
	spc_->InitShader();
}

void PanoramaRenderer::render_shader(Matrix4d& cameraParameter){
	
				//glViewport(0,0,viewSize*2,viewSize);
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				spc_->EnableShader(1);
					glClearColor(1,1,1,1);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glEnable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);
				//glEnable(GL_CULL_FACE);
				//glCullFace(GL_BACK);
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
			//    glOrtho(0, 2*PI_VAL, 0, PI_VAL,0, 500);
				::glOrtho(0.0, M_PI*2, 0.0,M_PI , 1, 500);

				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
				

				float RTforRender[16]={cameraParameter(0,0),cameraParameter(1,0),cameraParameter(2,0),cameraParameter(3,0)
					,cameraParameter(0,1),cameraParameter(1,1),cameraParameter(2,1),cameraParameter(3,1)
					,cameraParameter(0,2),cameraParameter(1,2),cameraParameter(2,2),cameraParameter(3,2)
					,cameraParameter(0,3),cameraParameter(1,3),cameraParameter(2,3),cameraParameter(3,3)
				};

				 glMultMatrixf(RTforRender);
				
				glDrawElements(GL_TRIANGLES, meshNumber*3, 
						GL_UNSIGNED_INT, meshArray);
				spc_->DisableShader();
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_COLOR_ARRAY);	
                
	colorImage=(GLubyte*)malloc(sizeof(GLubyte)*view[2]*view[3]*3);
	glReadPixels(view[0],view[1],view[2],view[3],GL_RGB,GL_UNSIGNED_BYTE,colorImage);
	depthArray=(GLfloat*)malloc(sizeof(GLfloat)*view[2]*view[3]);
	glReadPixels(view[0],view[1],view[2],view[3],GL_DEPTH_COMPONENT,GL_FLOAT,depthArray);


}


void PanoramaRenderer::renderColor(Matrix4d& cameraParameter,int viewSize){
		   PIXELFORMATDESCRIPTOR _pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	//	Size of this struct
		1,	//	Versin of this structure
		PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL |
		PFD_GENERIC_ACCELERATED,
//		PFD_GENERIC_FORMAT,
		//	Pixel buffer flags
		PFD_TYPE_RGBA,	//	Type of pixel data
		24,	//	The number of color bitplanes
		0, 0, 0, 0, 0, 0,	//	Number of each color bitplanes and shift count
		0, 0,	//	Number of alpha bitplanes and shift count
		0, 0, 0, 0, 0,	//	Number of accumulation bits
		32,	//	Z depth
		0,	//	Stencil depth
		0,	//	Number of auxiliary buffers
		PFD_MAIN_PLANE,	//	Ignored
		0,	//	Reserved
		0,	//	Ignored
		0,	//	Transparent color value
		0,	//	Ignored
	};
		   GLint view[4];
	   HDC		_hdc_=CreateCompatibleDC(NULL);
	   viewHeight=viewSize;
	DWORD m_DIBWidth = viewSize*2;
	DWORD m_DIBHeight = viewSize;
	DWORD m_BPP = 24;

	// Create BITMAPINFOHEADER
	BITMAPINFOHEADER* m_PBIH = new BITMAPINFOHEADER;
	int iSize = sizeof(BITMAPINFOHEADER);
	::memset(m_PBIH, 0, iSize);

	m_PBIH->biSize = sizeof(BITMAPINFOHEADER);
	m_PBIH->biWidth = m_DIBWidth;
	m_PBIH->biHeight = m_DIBHeight;
	m_PBIH->biPlanes = 1;
	m_PBIH->biBitCount = m_BPP;
	m_PBIH->biCompression = BI_RGB;

	// Create DIB
	void* m_PBits;
	HBITMAP m_hbitmap_old;
	HBITMAP m_hbitmap = ::CreateDIBSection(
		_hdc_,
		(BITMAPINFO*)m_PBIH, DIB_RGB_COLORS,
		&m_PBits, NULL, 0
	);

	m_hbitmap_old = (HBITMAP)::SelectObject(_hdc_, m_hbitmap);
	DWORD dwLength;
	if ((m_DIBWidth*3) % 4==0) /* バッファの１ラインの長さを計算 */
          dwLength=m_DIBWidth*3;
      else
          dwLength=m_DIBWidth*3+(4-(m_DIBWidth*3) % 4);
	
	int		_pfid = ChoosePixelFormat(_hdc_, &_pfd);
	::SetPixelFormat(_hdc_, _pfid, &_pfd);
	HGLRC	_hrc = ::wglCreateContext(_hdc_);
	::wglMakeCurrent(_hdc_,_hrc);
	Init(viewSize);
	glGetIntegerv(GL_VIEWPORT,view);
	for(int i=0;i<dataNum;i++){
		float* trsVert=(float*)malloc(sizeof(float)*vtNumArray[i]*3);
		float* vertexp=vertexPointers[i];
		for(int j=0;j<vtNumArray[i];j++){
			Vector3d tp;
			Vector3d pp;
			pp<<vertexp[j*3],vertexp[j*3+1],vertexp[j*3+2];
			
			sphericalTrans_renderer(tp,pp,cameraParameter);
			trsVert[j*3]=(float)tp(0,0);
			trsVert[j*3+1]=(float)tp(1,0);
			trsVert[j*3+2]=(float)tp(2,0);
//			cout<<tp<<endl;
		}
		displayrgb(trsVert,facePointers[i],rgbaPointers[i],meshNumArray[i],vtNumArray[i]);
		free(trsVert);
	}
	
//	BitBlt(dhdc,0,0,m_DIBWidth,m_DIBHeight,_hdc_,0,0,SRCCOPY);
//	SelectObject(dhdc,m_hbitmap_old);
//	GetDIBits(_hdc_,m_hbitmap,0,m_DIBHeight,lpPixel,(LPBITMAPINFO)m_PBIH,DIB_RGB_COLORS);
	colorImage=(GLubyte*)malloc(sizeof(GLubyte)*view[2]*view[3]*3);
	glReadPixels(view[0],view[1],view[2],view[3],GL_RGB,GL_UNSIGNED_BYTE,colorImage);
	depthArray=(GLfloat*)malloc(sizeof(GLfloat)*view[2]*view[3]);
	glReadPixels(view[0],view[1],view[2],view[3],GL_DEPTH_COMPONENT,GL_FLOAT,depthArray);
	normArray=(GLfloat*)malloc(sizeof(GLfloat)*view[2]*view[3]);

	double th=M_PI/viewHeight;
	Vector3d zdrc;

	for(int x=0;x<viewHeight*2;x++){
		for(int y=0;y<viewHeight;y++){
			int idx=y*viewHeight*2+x;
			if(y==0||y==viewHeight-1){normArray[idx]=0;continue;}
			int prexidx=x!=0?y*viewHeight*2+x-1:y*viewHeight*2+viewHeight*2-1;
			int postxidx=x!=viewHeight*2-1?y*viewHeight*2+x+1:y*viewHeight*2;
			int preyidx=idx-viewHeight*2;
			int postyidx=idx+viewHeight*2;
			if(depthArray[idx]==1.0||depthArray[prexidx]==1.0||depthArray[postxidx]==1.0||depthArray[preyidx]==1.0||depthArray[postyidx]==1.0){
				normArray[idx]=0;
				continue;
			}

			/*double t=M_PI*(depthArray[postxidx]+depthArray[prexidx])*125/viewHeight;//125= viewOrtho far/4
			
			double d=(depthArray[postxidx]-depthArray[prexidx])*500;
			double t2=M_PI*(depthArray[postyidx]+depthArray[preyidx])*125/viewHeight;//125= viewOrtho far/4
			double d2=(depthArray[postyidx]-depthArray[preyidx])*500;*/
			Vector3d v1,v2;
			v1<<(depthArray[prexidx]+depthArray[postxidx])*sin(th)*500,0,(depthArray[prexidx]-depthArray[postxidx])*cos(th)*500;
			v2<<0,(depthArray[preyidx]+depthArray[postyidx])*sin(th)*500,(depthArray[preyidx]-depthArray[postyidx])*cos(th)*500;
			Vector3d n;
			n=v1.cross(v2);
			if(n(2)<0)n=-n;
			n.normalize();
			normArray[idx]=n(2);//max...2.0
		}
	}
//	GetObject(m_hbitmap,
	
		glFlush();
		glFinish();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if(FALSE==::wglMakeCurrent(0,0))exit(ERROR);
	if(FALSE==::wglDeleteContext(_hrc))exit(ERROR);
	//delete _hdc_;
	//free(m_PBits);
	DeleteDC(_hdc_);
	DeleteObject(m_hbitmap);

//	::ReleaseDC(NULL,_hdc_);
//	GlobalFree(lpBuf);
//	GlobalFree(lpBuf);
//	free(buffer);
// return 0;
}

void PanoramaRenderer::outputReflectance(string fileName){
	ofstream	_os(fileName, ios::binary);
	if(!_os.is_open())
		return;
//	Save image as bitmap.
	BITMAPFILEHEADER	_fh;
	BITMAPINFOHEADER	_if;
	int		_fh_size = sizeof(BITMAPFILEHEADER);
	int		_if_size = sizeof(BITMAPINFOHEADER);
	int		_size = viewHeight * viewHeight * 2*3;
	::ZeroMemory(&_fh, _fh_size);
	::ZeroMemory(&_if, _if_size);
//	Initialize BITMAPFILEHEADER.
	_fh.bfSize = _fh_size + _if_size + _size;
	_fh.bfOffBits = _fh_size + _if_size;
	((char*)&_fh)[0] = 'B';
	((char*)&_fh)[1] = 'M';
//	Initialize BITMAPINFOHEADER.
	_if.biSize = _if_size;
	_if.biWidth = viewHeight*2;
	_if.biHeight = viewHeight;
	_if.biPlanes = 1;
	_if.biBitCount = 24;
	_if.biCompression = BI_RGB;
	_if.biSizeImage = _size;
//	Reverse byte.
	unsigned char*		_buffer = new unsigned char[_size];
	float*		_tdata = reinterpret_cast<float*>(reflectanceImage);
	unsigned char*		_cdata = _buffer;
	int			i;
	for(i = 0; i < _size/3; ++i){
		//modification
		//_tdata[0]=1-(1-_tdata[0])*(1-_tdata[0])*(1-_tdata[0]);//
		//_tdata[0]=_tdata[0]<1/3.0?_tdata[0]*3:1;

		unsigned char gr=_tdata[0]*255<255?(unsigned char)(_tdata[0]*255):255;
		//unsigned char gr=i<_size/8?(unsigned char)(_tdata[0]*255):255;
		_cdata[0] = gr;
		_cdata[1] = gr;
		_cdata[2] = gr;
		_tdata ++;
		_cdata += 3;
	}
//	Write to file.
	_os.write((char*)&_fh, _fh_size);
	_os.write((char*)&_if, _if_size);
	_os.write(reinterpret_cast<char*>(_buffer), _size);
	_os.close();

	delete []	_buffer;

	return;

}

void PanoramaRenderer::outputColor(string fileName){
	ofstream	_os(fileName, ios::binary);
	if(!_os.is_open())
		return;
//	Save image as bitmap.
	BITMAPFILEHEADER	_fh;
	BITMAPINFOHEADER	_if;
	int		_fh_size = sizeof(BITMAPFILEHEADER);
	int		_if_size = sizeof(BITMAPINFOHEADER);
	int		_size = viewHeight * viewHeight * 2*3;
	::ZeroMemory(&_fh, _fh_size);
	::ZeroMemory(&_if, _if_size);
//	Initialize BITMAPFILEHEADER.
	_fh.bfSize = _fh_size + _if_size + _size;
	_fh.bfOffBits = _fh_size + _if_size;
	((char*)&_fh)[0] = 'B';
	((char*)&_fh)[1] = 'M';
//	Initialize BITMAPINFOHEADER.
	_if.biSize = _if_size;
	_if.biWidth = viewHeight*2;
	_if.biHeight = viewHeight;
	_if.biPlanes = 1;
	_if.biBitCount = 24;
	_if.biCompression = BI_RGB;
	_if.biSizeImage = _size;
//	Reverse byte.
//	unsigned char*		_buffer = new unsigned char[_size];
	unsigned char*		_buffer = (unsigned char*)malloc(sizeof(unsigned char)*_size);
	unsigned char*		_tdata = reinterpret_cast<unsigned char*>(colorImage);
	unsigned char*		_cdata = _buffer;
	int			i;
	for(i = 0; i < _size/3; ++i){
		//modification
		
		_cdata[0] = _tdata[0];
		_cdata[1] = _tdata[1];
		_cdata[2] = _tdata[2];
		_tdata +=3;
		_cdata += 3;
	}
//	Write to file.
	_os.write((char*)&_fh, _fh_size);
	_os.write((char*)&_if, _if_size);
	_os.write(reinterpret_cast<char*>(_buffer), _size);
	_os.close();

	free(_buffer);

	return;

}



void PanoramaRenderer::outputDepth(string fileName,float rankWidth){
		ofstream	_os(fileName, ios::binary);
	if(!_os.is_open())
		return;
//	Save image as bitmap.
	BITMAPFILEHEADER	_fh;
	BITMAPINFOHEADER	_if;
	int		_fh_size = sizeof(BITMAPFILEHEADER);
	int		_if_size = sizeof(BITMAPINFOHEADER);
	int		_size = viewHeight * viewHeight * 2*3;
	::ZeroMemory(&_fh, _fh_size);
	::ZeroMemory(&_if, _if_size);
//	Initialize BITMAPFILEHEADER.
	_fh.bfSize = _fh_size + _if_size + _size;
	_fh.bfOffBits = _fh_size + _if_size;
	((char*)&_fh)[0] = 'B';
	((char*)&_fh)[1] = 'M';
//	Initialize BITMAPINFOHEADER.
	_if.biSize = _if_size;
	_if.biWidth = viewHeight*2;
	_if.biHeight = viewHeight;
	_if.biPlanes = 1;
	_if.biBitCount = 24;
	_if.biCompression = BI_RGB;
	_if.biSizeImage = _size;
//	Reverse byte.
	unsigned char*		_buffer = new unsigned char[_size];
	float*		_tdata = reinterpret_cast<float*>(depthArray);
	unsigned char*		_cdata = _buffer;
	int			i;
	for(i = 0; i < _size/3; ++i){
		unsigned char gr=_tdata[0]*rankWidth<255?(unsigned char)(_tdata[0]*rankWidth):255;

		_cdata[0] = gr;
		_cdata[1] = gr;
		_cdata[2] = gr;
//		cout<<(int)_tdata[0]<<endl;
		_tdata += 1;
		_cdata += 3;
	}
//	Write to file.
	_os.write((char*)&_fh, _fh_size);
	_os.write((char*)&_if, _if_size);
	_os.write(reinterpret_cast<char*>(_buffer), _size);
	_os.close();

	delete []	_buffer;

	return;



};

void PanoramaRenderer::outputNorm(string fileName){
		ofstream	_os(fileName, ios::binary);
	if(!_os.is_open())
		return;
//	Save image as bitmap.
	BITMAPFILEHEADER	_fh;
	BITMAPINFOHEADER	_if;
	int		_fh_size = sizeof(BITMAPFILEHEADER);
	int		_if_size = sizeof(BITMAPINFOHEADER);
	int		_size = viewHeight * viewHeight * 2*3;
	::ZeroMemory(&_fh, _fh_size);
	::ZeroMemory(&_if, _if_size);
//	Initialize BITMAPFILEHEADER.
	_fh.bfSize = _fh_size + _if_size + _size;
	_fh.bfOffBits = _fh_size + _if_size;
	((char*)&_fh)[0] = 'B';
	((char*)&_fh)[1] = 'M';
//	Initialize BITMAPINFOHEADER.
	_if.biSize = _if_size;
	_if.biWidth = viewHeight*2;
	_if.biHeight = viewHeight;
	_if.biPlanes = 1;
	_if.biBitCount = 24;
	_if.biCompression = BI_RGB;
	_if.biSizeImage = _size;
	float* normArray=(float*)malloc(sizeof(float)*viewHeight*viewHeight*2);
	for(int x=0;x<viewHeight*2;x++){
		for(int y=0;y<viewHeight;y++){
			int idx=y*viewHeight*2+x;
			if(y==0||y==viewHeight-1){normArray[idx]=0;continue;}
			int prexidx=x!=0?y*viewHeight*2+x-1:y*viewHeight*2+viewHeight*2-1;
			int postxidx=x!=viewHeight*2-1?y*viewHeight*2+x+1:y*viewHeight*2;
			int preyidx=idx-viewHeight*2;
			int postyidx=idx+viewHeight*2;
			if(depthArray[idx]==1.0||depthArray[prexidx]==1.0||depthArray[postxidx]==1.0||depthArray[preyidx]==1.0||depthArray[postyidx]==1.0){
				normArray[idx]=0;
				continue;
			}
			Vector3d n;
			double c=depthArray[idx]*M_PI*4/(viewHeight*2);
			n<<(depthArray[postxidx]-depthArray[prexidx])/c,(depthArray[postyidx]-depthArray[preyidx])/c,1;
			n.normalize();

			normArray[idx]=n(2,0);
		}
	}



//	Reverse byte.
	unsigned char*		_buffer = new unsigned char[_size];
	float*		_tdata = reinterpret_cast<float*>(normArray);
	unsigned char*		_cdata = _buffer;
	int			i;
	for(i = 0; i < _size/3; ++i){



		unsigned char gr=_tdata[0]*255<255?(unsigned char)(_tdata[0]*255):255;

		_cdata[0] = gr;
		_cdata[1] = gr;
		_cdata[2] = gr;
//		cout<<(int)_tdata[0]<<endl;
		_tdata += 1;
		_cdata += 3;
	}
//	Write to file.
	_os.write((char*)&_fh, _fh_size);
	_os.write((char*)&_if, _if_size);
	_os.write(reinterpret_cast<char*>(_buffer), _size);
	_os.close();

	delete []	_buffer;

	return;



};

void display(float*& vertex,unsigned int*& face,float*& reflectance,int meshNum,int vertNum)
{
	GLint view[4];
	GLdouble ox,oy,oz;
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glGetIntegerv(GL_VIEWPORT,view);
	glBegin(GL_TRIANGLES);
    for(int i=0;i<meshNum;i++){
		int index1=face[i*3];
		int index2=face[i*3+1];
		int index3=face[i*3+2];
		if(index1>=vertNum){
			continue;
		}if(index2>=vertNum){
			continue;
		}if(index3>=vertNum){
			continue;
		}
		double thres=0.4;
		if(vertex[index1*3+2]<thres||vertex[index2*3+2]<thres||vertex[index3*3+2]<thres)continue;//近距離はレンダリングしない（およそladybugの半径分）
		
		GLfloat gr;
		//パノラマ端点の処理
		if(vertex[index1*3]*vertex[index2*3]<0&&vertex[index1*3]*vertex[index3*3]>0&&abs(vertex[index1*3])>PI_VAL/2){
			double val=vertex[index2*3]<0?PI_VAL*2:-PI_VAL*2;
			gr=reflectance[index1];
			glColor3f(gr,gr,gr);
			glVertex3f(vertex[index1*3],vertex[index1*3+1],vertex[index1*3+2]);
			gr=reflectance[index2];
			glColor3f(gr,gr,gr);
			glVertex3f(vertex[index2*3]+val,vertex[index2*3+1],vertex[index2*3+2]);
			gr=reflectance[index3];
			glColor3f(gr,gr,gr);
			glVertex3f(vertex[index3*3],vertex[index3*3+1],vertex[index3*3+2]);		
			gr=reflectance[index1];
			glColor3f(gr,gr,gr);
			glVertex3f(vertex[index1*3]-val,vertex[index1*3+1],vertex[index1*3+2]);
			gr=reflectance[index2];
			glColor3f(gr,gr,gr);
			glVertex3f(vertex[index2*3],vertex[index2*3+1],vertex[index2*3+2]);
			gr=reflectance[index3];
			glColor3f(gr,gr,gr);
			glVertex3f(vertex[index3*3]-val,vertex[index3*3+1],vertex[index3*3+2]);
		}else if(vertex[index1*3]*vertex[index3*3]<0&&vertex[index1*3]*vertex[index2*3]>0&&abs(vertex[index1*3])>PI_VAL/2){
			double val=vertex[index3*3]<0?PI_VAL*2:-PI_VAL*2;
			gr=reflectance[index1];
			glColor3f(gr,gr,gr);
			glVertex3f(vertex[index1*3],vertex[index1*3+1],vertex[index1*3+2]);
			gr=reflectance[index2];
			glColor3f(gr,gr,gr);
			glVertex3f(vertex[index2*3],vertex[index2*3+1],vertex[index2*3+2]);
			gr=reflectance[index3];
			glColor3f(gr,gr,gr);
			glVertex3f(vertex[index3*3]+val,vertex[index3*3+1],vertex[index3*3+2]);		
			gr=reflectance[index1];
			glColor3f(gr,gr,gr);
			glVertex3f(vertex[index1*3]-val,vertex[index1*3+1],vertex[index1*3+2]);
			gr=reflectance[index2];
			glColor3f(gr,gr,gr);
			glVertex3f(vertex[index2*3]-val,vertex[index2*3+1],vertex[index2*3+2]);
			gr=reflectance[index3];
			glColor3f(gr,gr,gr);
			glVertex3f(vertex[index3*3],vertex[index3*3+1],vertex[index3*3+2]);	
		}else if(vertex[index1*3]*vertex[index3*3]<0&&vertex[index2*3]*vertex[index3*3]>0&&abs(vertex[index1*3])>PI_VAL/2){
			double val=vertex[index1*3]<0?PI_VAL*2:-PI_VAL*2;
			gr=reflectance[index1];
			glColor3f(gr,gr,gr);
			glVertex3f(vertex[index1*3]+val,vertex[index1*3+1],vertex[index1*3+2]);
			gr=reflectance[index2];
			glColor3f(gr,gr,gr);
			glVertex3f(vertex[index2*3],vertex[index2*3+1],vertex[index2*3+2]);
			gr=reflectance[index3];
			glColor3f(gr,gr,gr);
			glVertex3f(vertex[index3*3],vertex[index3*3+1],vertex[index3*3+2]);		
			gr=reflectance[index1];
			glColor3f(gr,gr,gr);
			glVertex3f(vertex[index1*3],vertex[index1*3+1],vertex[index1*3+2]);
			gr=reflectance[index2];
			glColor3f(gr,gr,gr);
			glVertex3f(vertex[index2*3]-val,vertex[index2*3+1],vertex[index2*3+2]);
			gr=reflectance[index3];
			glColor3f(gr,gr,gr);
			glVertex3f(vertex[index3*3]-val,vertex[index3*3+1],vertex[index3*3+2]);	
		}else{
			gr=reflectance[index1];
			glColor3f(gr,gr,gr);
			glVertex3f(vertex[index1*3],vertex[index1*3+1],vertex[index1*3+2]);
			gr=reflectance[index2];
			glColor3f(gr,gr,gr);
			glVertex3f(vertex[index2*3],vertex[index2*3+1],vertex[index2*3+2]);
			gr=reflectance[index3];
			glColor3f(gr,gr,gr);
			glVertex3f(vertex[index3*3],vertex[index3*3+1],vertex[index3*3+2]);	
		}


	
    }
    glEnd();
//	glFlush();
	cout<<"check"<<endl;

}

void displayrgb(float*& vertex,unsigned int*& face,unsigned char*& rgba,int meshNum,int vertNum)
{
	GLint view[4];
	GLdouble ox,oy,oz;
	glBegin(GL_TRIANGLES);
    for(int i=0;i<meshNum;i++){
		int index1=face[i*3];
		int index2=face[i*3+1];
		int index3=face[i*3+2];
		if(index1>vertNum){
			continue;
		}if(index2>vertNum){
			continue;
		}if(index3>vertNum){
			continue;
		}
		double thres=0.4;
		if(vertex[index1*3+2]<thres||vertex[index2*3+2]<thres||vertex[index3*3+2]<thres)continue;//近距離はレンダリングしない（およそladybugの半径分）
		if(vertex[index1*3+1]<PI_VAL/60||vertex[index2*3+1]<PI_VAL/60||vertex[index3*3+1]<PI_VAL/60)continue;
		if(vertex[index1*3+1]>PI_VAL-PI_VAL/60||vertex[index2*3+1]>PI_VAL-PI_VAL/60||vertex[index3*3+1]>PI_VAL-PI_VAL/60)continue;
		GLfloat gr;
		//パノラマ端点の処理
		if(vertex[index1*3]*vertex[index2*3]<0&&vertex[index1*3]*vertex[index3*3]>0&&abs(vertex[index1*3])>PI_VAL/2){
			double val=vertex[index2*3]<0?PI_VAL*2:-PI_VAL*2;
			//gr=reflectance[index1];
			
			glColor3ub(rgba[index1*4],rgba[index1*4+1],rgba[index1*4+2]);
			glVertex3f(vertex[index1*3],vertex[index1*3+1],vertex[index1*3+2]);
			//gr=reflectance[index2];
			glColor3ub(rgba[index2*4],rgba[index2*4+1],rgba[index2*4+2]);
			glVertex3f(vertex[index2*3]+val,vertex[index2*3+1],vertex[index2*3+2]);
			//gr=reflectance[index3];
			glColor3ub(rgba[index3*4],rgba[index3*4+1],rgba[index3*4+2]);
			glVertex3f(vertex[index3*3],vertex[index3*3+1],vertex[index3*3+2]);		
			//gr=reflectance[index1];
			glColor3ub(rgba[index1*4],rgba[index1*4+1],rgba[index1*4+2]);
			glVertex3f(vertex[index1*3]-val,vertex[index1*3+1],vertex[index1*3+2]);
			//gr=reflectance[index2];
			glColor3ub(rgba[index2*4],rgba[index2*4+1],rgba[index2*4+2]);
			glVertex3f(vertex[index2*3],vertex[index2*3+1],vertex[index2*3+2]);
			//gr=reflectance[index3];
			glColor3ub(rgba[index3*4],rgba[index3*4+1],rgba[index3*4+2]);
			glVertex3f(vertex[index3*3]-val,vertex[index3*3+1],vertex[index3*3+2]);
		}else if(vertex[index1*3]*vertex[index3*3]<0&&vertex[index1*3]*vertex[index2*3]>0&&abs(vertex[index1*3])>PI_VAL/2){
			double val=vertex[index3*3]<0?PI_VAL*2:-PI_VAL*2;
			//gr=reflectance[index1];
	
			glColor3ub(rgba[index1*4],rgba[index1*4+1],rgba[index1*4+2]);
			glVertex3f(vertex[index1*3],vertex[index1*3+1],vertex[index1*3+2]);
			//gr=reflectance[index2];
			glColor3ub(rgba[index2*4],rgba[index2*4+1],rgba[index2*4+2]);
			glVertex3f(vertex[index2*3],vertex[index2*3+1],vertex[index2*3+2]);
			//gr=reflectance[index3];
			glColor3ub(rgba[index3*4],rgba[index3*4+1],rgba[index3*4+2]);
			glVertex3f(vertex[index3*3]+val,vertex[index3*3+1],vertex[index3*3+2]);		
			//gr=reflectance[index1];
			glColor3ub(rgba[index1*4],rgba[index1*4+1],rgba[index1*4+2]);
			glVertex3f(vertex[index1*3]-val,vertex[index1*3+1],vertex[index1*3+2]);
			//gr=reflectance[index2];
			glColor3ub(rgba[index2*4],rgba[index2*4+1],rgba[index2*4+2]);
			glVertex3f(vertex[index2*3]-val,vertex[index2*3+1],vertex[index2*3+2]);
			//gr=reflectance[index3];
			glColor3ub(rgba[index3*4],rgba[index3*4+1],rgba[index3*4+2]);
			glVertex3f(vertex[index3*3],vertex[index3*3+1],vertex[index3*3+2]);	
		}else if(vertex[index1*3]*vertex[index3*3]<0&&vertex[index2*3]*vertex[index3*3]>0&&abs(vertex[index1*3])>PI_VAL/2){
			double val=vertex[index1*3]<0?PI_VAL*2:-PI_VAL*2;
			//gr=reflectance[index1];
			glColor3ub(rgba[index1*4],rgba[index1*4+1],rgba[index1*4+2]);
			glVertex3f(vertex[index1*3]+val,vertex[index1*3+1],vertex[index1*3+2]);
			//gr=reflectance[index2];
			glColor3ub(rgba[index2*4],rgba[index2*4+1],rgba[index2*4+2]);
			glVertex3f(vertex[index2*3],vertex[index2*3+1],vertex[index2*3+2]);
			//gr=reflectance[index3];
			glColor3ub(rgba[index3*4],rgba[index3*4+1],rgba[index3*4+2]);
			glVertex3f(vertex[index3*3],vertex[index3*3+1],vertex[index3*3+2]);		
			//gr=reflectance[index1];
			glColor3ub(rgba[index1*4],rgba[index1*4+1],rgba[index1*4+2]);
			glVertex3f(vertex[index1*3],vertex[index1*3+1],vertex[index1*3+2]);
			//gr=reflectance[index2];
			glColor3ub(rgba[index2*4],rgba[index2*4+1],rgba[index2*4+2]);
			glVertex3f(vertex[index2*3]-val,vertex[index2*3+1],vertex[index2*3+2]);
			//gr=reflectance[index3];
			glColor3ub(rgba[index3*4],rgba[index3*4+1],rgba[index3*4+2]);
			glVertex3f(vertex[index3*3]-val,vertex[index3*3+1],vertex[index3*3+2]);	
		}else{
			//gr=reflectance[index1];
			glColor3ub(rgba[index1*4],rgba[index1*4+1],rgba[index1*4+2]);
			glVertex3f(vertex[index1*3],vertex[index1*3+1],vertex[index1*3+2]);
			//gr=reflectance[index2];
			glColor3ub(rgba[index2*4],rgba[index2*4+1],rgba[index2*4+2]);
			glVertex3f(vertex[index2*3],vertex[index2*3+1],vertex[index2*3+2]);
			//gr=reflectance[index3];
			glColor3ub(rgba[index3*4],rgba[index3*4+1],rgba[index3*4+2]);
			glVertex3f(vertex[index3*3],vertex[index3*3+1],vertex[index3*3+2]);	
						
		}
		
    }
    glEnd();

	
	cout<<"check"<<endl;

}

void display_points(float*& vertex,int*& face,float*& reflectance,int meshNum,int vertNum)
{
	GLint view[4];
	GLdouble ox,oy,oz;
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glGetIntegerv(GL_VIEWPORT,view);
	glBegin(GL_POINTS);
	glPointSize(1);
	for(int i=0;i<vertNum;i++){
		float gr=reflectance[i];
			glColor3f(gr,gr,gr);
			glVertex3f(vertex[i*3],vertex[i*3+1],vertex[i*3+2]);
    }
    glEnd();
//	glFlush();
	cout<<"check"<<endl;

}

//世界座標系においてR,tだけ移動したカメラにおける点pの変換式は
//R*(pt-t)

void sphericalTrans_renderer(Vector3d& ret_,Vector3d& pt,Vector3d& center,Matrix3d& rotMatrix){
	Vector3d tp=rotMatrix*(pt-center);
	float r=sqrt(tp.dot(tp));
	float phi=acos(tp(2,0)/r);//
	float theta=atan2(tp(1,0),tp(0,0));
	ret_<<-theta,phi,r;
}

void sphericalTrans_renderer(Vector3d& ret_,Vector3d& pt,Matrix4d& cameraParameter){
	Vector4d pt_;
	pt_<<pt(0),pt(1),pt(2),1;
	Vector4d tp_=cameraParameter*pt_;
	Vector3d tp=tp_.block(0,0,3,1);
	float r=sqrt(tp.dot(tp));
	float phi=acos(tp(2,0)/r);//
	float theta=atan2(tp(1,0),tp(0,0));
	ret_<<-theta,phi,r;
}

void Init(int viewSize){
	glViewport(0,0,viewSize*2,viewSize);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	gluPerspective(90.0, (double)300/(double)300, 0.1, 100.0); //透視投影法の視体積gluPerspactive(th, w/h, near, far);

//	 ::glMatrixMode(GL_PROJECTION);
//	::glPushMatrix();
//	::glLoadIdentity();
//	::glOrtho(0.0, M_PI*2, 0.0,M_PI , 1, 500);
//	glOrtho(-PI_VAL, PI_VAL, -PI_VAL,0 ,0, 500);
  //------------------------------------------------
  gluLookAt(
       0.0, 0.0,0.0, // 視点の位置x,y,z;
       1.0, 0.0,0.0,   // 視界の中心位置の参照点座標x,y,z
       0.0, 0.0, 1.0);  //視界の上方向のベクトルx,y,z*/
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	GLfloat light_position[]
//	glLigntfv(GL_LIGHT0,GL_POSITION,);
	
}

