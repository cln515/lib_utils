#include "PanoramaRenderer.h"


using namespace std;
using namespace Eigen;

#if defined(WIN32) || defined(WIN64)
HDC dhdc;
#elif defined(__unix__)
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
typedef Bool (*glXMakeContextCurrentARBProc)(Display*, GLXDrawable, GLXDrawable, GLXContext);
static glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;
static glXMakeContextCurrentARBProc   glXMakeContextCurrentARB   = NULL;
#endif

void display(float*& vertex, unsigned int*& face, float*& reflectance, int meshNum, int vertNum);
void displayrgb(float*& vertex, unsigned int*& face, unsigned char*& rgba, int meshNum, int vertNum);
void display_points(float*& vertex, unsigned int*& face, float*& reflectance, int meshNum, int vertNum);
void sphericalTrans_renderer(Vector3d& ret_, Vector3d& pt, Vector3d& center, Matrix3d& rotMatrix);
void fisheyeTrans_renderer(Vector3d& ret_, Vector3d& pt, Matrix4d& cameraParameter);
void sphericalTrans_renderer(Vector3d& ret_, Vector3d& pt, Matrix4d& cameraParameter);
void Init(int viewWidth, int viewHeight, double depthResolution);
void InitPers(int viewWidth, int viewHeight, double znear, double depthResolution, double* intrinsic);
void InitFE(int viewWidth, int viewHeight, double depthResolution);

void PanoramaRenderer::setData(float* vertex, unsigned int* face, float* reflectance, int vertNum, int meshnum) {
	if (dataNum == 0) {
		vertexPointers = (float**)malloc(sizeof(float*) * 1);
		facePointers = (unsigned int**)malloc(sizeof(float*) * 1);
		reflectancePointers = (float**)malloc(sizeof(float*) * 1);
		meshNumArray = (int*)malloc(sizeof(int) * 1);
		vtNumArray = (int*)malloc(sizeof(int) * 1);
		vertexPointers[0] = vertex;
		facePointers[0] = face;
		reflectancePointers[0] = reflectance;
		meshNumArray[0] = meshnum;
		vtNumArray[0] = vertNum;
		dataNum++;
	}
	else {
		dataNum++;
		vertexPointers = (float**)realloc(vertexPointers, sizeof(float*)*dataNum);
		facePointers = (unsigned int**)realloc(facePointers, sizeof(int*)*dataNum);
		reflectancePointers = (float**)realloc(reflectancePointers, sizeof(float*)*dataNum);
		meshNumArray = (int*)realloc(meshNumArray, sizeof(int)*dataNum);
		vtNumArray = (int*)realloc(vtNumArray, sizeof(int)*dataNum);
		vertexPointers[dataNum - 1] = vertex;
		facePointers[dataNum - 1] = face;
		reflectancePointers[dataNum - 1] = reflectance;
		meshNumArray[dataNum - 1] = meshnum;
		vtNumArray[dataNum - 1] = vertNum;
	};
}

void PanoramaRenderer::setDataRGB(float* vertex, unsigned int* face, unsigned char* rgba, int vertNum, int meshnum) {
	if (dataNum == 0) {
		vertexPointers = (float**)malloc(sizeof(float*) * 1);
		facePointers = (unsigned int**)malloc(sizeof(float*) * 1);
		rgbaPointers = (unsigned char**)malloc(sizeof(char*) * 1);
		meshNumArray = (int*)malloc(sizeof(int) * 1);
		vtNumArray = (int*)malloc(sizeof(int) * 1);
		vertexPointers[0] = vertex;
		facePointers[0] = face;
		rgbaPointers[0] = rgba;
		meshNumArray[0] = meshnum;
		vtNumArray[0] = vertNum;
		dataNum++;
	}
	else {
		dataNum++;
		vertexPointers = (float**)realloc(vertexPointers, sizeof(float*)*dataNum);
		facePointers = (unsigned int**)realloc(facePointers, sizeof(int*)*dataNum);
		rgbaPointers = (unsigned char**)realloc(rgbaPointers, sizeof(char*)*dataNum);
		meshNumArray = (int*)realloc(meshNumArray, sizeof(int)*dataNum);
		vtNumArray = (int*)realloc(vtNumArray, sizeof(int)*dataNum);
		vertexPointers[dataNum - 1] = vertex;
		facePointers[dataNum - 1] = face;
		rgbaPointers[dataNum - 1] = rgba;
		meshNumArray[dataNum - 1] = meshnum;
		vtNumArray[dataNum - 1] = vertNum;
	};
}

// void PanoramaRenderer::render_dot(Vector3d& center, Matrix3d& viewDirection, int viewSize) {
// 	PIXELFORMATDESCRIPTOR _pfd = {
//  sizeof(PIXELFORMATDESCRIPTOR),	//	Size of this struct
//  1,	//	Versin of this structure
//  PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL |
//  PFD_GENERIC_ACCELERATED | PFD_DOUBLEBUFFER,
// 		//		PFD_GENERIC_FORMAT,
// 				//	Pixel buffer flags
// 				PFD_TYPE_RGBA,	//	Type of pixel data
// 				24,	//	The number of color bitplanes
// 				0, 0, 0, 0, 0, 0,	//	Number of each color bitplanes and shift count
// 				0, 0,	//	Number of alpha bitplanes and shift count
// 				0, 0, 0, 0, 0,	//	Number of accumulation bits
// 				32,	//	Z depth
// 				0,	//	Stencil depth
// 				0,	//	Number of auxiliary buffers
// 				PFD_MAIN_PLANE,	//	Ignored
// 				0,	//	Reserved
// 				0,	//	Ignored
// 				0,	//	Transparent color value
// 				0,	//	Ignored
// 	};
// 	GLint view[4];
// 	HDC		_hdc_ = CreateCompatibleDC(NULL);
// 	viewHeight = viewSize;
// 	DWORD m_DIBWidth = viewSize * 2;
// 	DWORD m_DIBHeight = viewSize;
// 	DWORD m_BPP = 24;

// 	// Create BITMAPINFOHEADER
// 	BITMAPINFOHEADER* m_PBIH = new BITMAPINFOHEADER;
// 	int iSize = sizeof(BITMAPINFOHEADER);
// 	::memset(m_PBIH, 0, iSize);

// 	m_PBIH->biSize = sizeof(BITMAPINFOHEADER);
// 	m_PBIH->biWidth = m_DIBWidth;
// 	m_PBIH->biHeight = m_DIBHeight;
// 	m_PBIH->biPlanes = 1;
// 	m_PBIH->biBitCount = m_BPP;
// 	m_PBIH->biCompression = BI_RGB;

// 	// Create DIB
// 	void* m_PBits;
// 	HBITMAP m_hbitmap_old;
// 	HBITMAP m_hbitmap = ::CreateDIBSection(
// 		_hdc_,
// 		(BITMAPINFO*)m_PBIH, DIB_RGB_COLORS,
// 		&m_PBits, NULL, 0
// 	);

// 	m_hbitmap_old = (HBITMAP)::SelectObject(_hdc_, m_hbitmap);
// 	DWORD dwLength;
// 	if ((m_DIBWidth * 3) % 4 == 0) /* �o�b�t�@�̂P���C���̒������v�Z */
// 		dwLength = m_DIBWidth * 3;
// 	else
// 		dwLength = m_DIBWidth * 3 + (4 - (m_DIBWidth * 3) % 4);
// 	//	LPBYTE lpPixel,lpBuf;
// 	//	lpBuf=(LPBYTE)GlobalAlloc(GPTR,sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwLength*m_DIBHeight);
// 	//	lpPixel=lpBuf+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

// 	int		_pfid = ChoosePixelFormat(_hdc_, &_pfd);
// 	::SetPixelFormat(_hdc_, _pfid, &_pfd);
// 	HGLRC	_hrc = ::wglCreateContext(_hdc_);
// 	::wglMakeCurrent(_hdc_, _hrc);
// 	Init(viewWidth_, viewHeight_, depthResolution);
// 	glGetIntegerv(GL_VIEWPORT, view);
// 	for (int i = 0; i < dataNum; i++) {
// 		float* trsVert = (float*)malloc(sizeof(float)*vtNumArray[i] * 3);
// 		float* vertexp = vertexPointers[i];
// 		for (int j = 0; j < vtNumArray[i]; j++) {
// 			Vector3d tp;
// 			Vector3d pp;
// 			pp << vertexp[j * 3], vertexp[j * 3 + 1], vertexp[j * 3 + 2];

// 			sphericalTrans_renderer(tp, pp, center, viewDirection);
// 			trsVert[j * 3] = (float)tp(0, 0);
// 			trsVert[j * 3 + 1] = (float)tp(1, 0);
// 			trsVert[j * 3 + 2] = (float)tp(2, 0);
// 			//			cout<<tp<<endl;
// 		}
// 		display_points(trsVert, facePointers[i], reflectancePointers[i], meshNumArray[i], vtNumArray[i]);
// 		free(trsVert);
// 	}

// 	//	BitBlt(dhdc,0,0,m_DIBWidth,m_DIBHeight,_hdc_,0,0,SRCCOPY);
// 	//	SelectObject(dhdc,m_hbitmap_old);
// 	//	GetDIBits(_hdc_,m_hbitmap,0,m_DIBHeight,lpPixel,(LPBITMAPINFO)m_PBIH,DIB_RGB_COLORS);
// 	reflectanceImage = (GLfloat*)malloc(sizeof(GLfloat)*view[2] * view[3]);
// 	glReadPixels(view[0], view[1], view[2], view[3], GL_RED, GL_FLOAT, reflectanceImage);
// 	depthArray = (GLfloat*)malloc(sizeof(GLfloat)*view[2] * view[3]);
// 	glReadPixels(view[0], view[1], view[2], view[3], GL_DEPTH_COMPONENT, GL_FLOAT, depthArray);
// 	normArray = (GLfloat*)malloc(sizeof(GLfloat)*view[2] * view[3]);
// 	for (int x = 0; x < viewHeight * 2; x++) {
// 		for (int y = 0; y < viewHeight; y++) {
// 			int idx = y * viewHeight * 2 + x;
// 			if (y == 0 || y == viewHeight - 1) { normArray[idx] = 0; continue; }
// 			int prexidx = x != 0 ? y * viewHeight * 2 + x - 1 : y * viewHeight * 2 + viewHeight * 2 - 1;
// 			int postxidx = x != viewHeight * 2 - 1 ? y * viewHeight * 2 + x + 1 : y * viewHeight * 2;
// 			int preyidx = idx - viewHeight * 2;
// 			int postyidx = idx + viewHeight * 2;
// 			if (depthArray[idx] == 1.0 || depthArray[prexidx] == 1.0 || depthArray[postxidx] == 1.0 || depthArray[preyidx] == 1.0 || depthArray[postyidx] == 1.0) {
// 				normArray[idx] = 0;
// 				continue;
// 			}
// 			double t = M_PI * (depthArray[postxidx] + depthArray[prexidx]) * 125 / viewHeight;//125= viewOrtho far/4
// 			double d = (depthArray[postxidx] - depthArray[prexidx]) * 500;
// 			double t2 = M_PI * (depthArray[postyidx] + depthArray[preyidx]) * 125 / viewHeight;//125= viewOrtho far/4
// 			double d2 = (depthArray[postyidx] - depthArray[preyidx]) * 500;
// 			normArray[idx] = (t / sqrt(t*t + d * d) + t2 / sqrt(t2*t2 + d2 * d2)) / 2;//max...2.0
// 		}
// 	}

// 	//	GetObject(m_hbitmap,
// 	::wglMakeCurrent(0, 0);
// 	//	GlobalFree(lpBuf);
// 	//	GlobalFree(lpBuf);
// 	//	free(buffer);
// 	// return 0;
// }

// void PanoramaRenderer::render(Vector3d& center, Matrix3d& viewDirection, int viewSize) {
// 	PIXELFORMATDESCRIPTOR _pfd = {
//  sizeof(PIXELFORMATDESCRIPTOR),	//	Size of this struct
//  1,	//	Versin of this structure
//  PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL |
//  PFD_GENERIC_ACCELERATED,
// 		//		PFD_GENERIC_FORMAT,
// 				//	Pixel buffer flags
// 				PFD_TYPE_RGBA,	//	Type of pixel data
// 				24,	//	The number of color bitplanes
// 				0, 0, 0, 0, 0, 0,	//	Number of each color bitplanes and shift count
// 				0, 0,	//	Number of alpha bitplanes and shift count
// 				0, 0, 0, 0, 0,	//	Number of accumulation bits
// 				32,	//	Z depth
// 				0,	//	Stencil depth
// 				0,	//	Number of auxiliary buffers
// 				PFD_MAIN_PLANE,	//	Ignored
// 				0,	//	Reserved
// 				0,	//	Ignored
// 				0,	//	Transparent color value
// 				0,	//	Ignored
// 	};






// 	GLint view[4];
// 	HDC		_hdc_ = CreateCompatibleDC(NULL);
// 	viewHeight = viewSize;
// 	DWORD m_DIBWidth = viewSize * 2;
// 	DWORD m_DIBHeight = viewSize;
// 	DWORD m_BPP = 24;

// 	// Create BITMAPINFOHEADER
// 	BITMAPINFOHEADER* m_PBIH = new BITMAPINFOHEADER;
// 	int iSize = sizeof(BITMAPINFOHEADER);
// 	::memset(m_PBIH, 0, iSize);

// 	m_PBIH->biSize = sizeof(BITMAPINFOHEADER);
// 	m_PBIH->biWidth = m_DIBWidth;
// 	m_PBIH->biHeight = m_DIBHeight;
// 	m_PBIH->biPlanes = 1;
// 	m_PBIH->biBitCount = m_BPP;
// 	m_PBIH->biCompression = BI_RGB;

// 	// Create DIB
// 	void* m_PBits;
// 	HBITMAP m_hbitmap_old;
// 	HBITMAP m_hbitmap = ::CreateDIBSection(
// 		_hdc_,
// 		(BITMAPINFO*)m_PBIH, DIB_RGB_COLORS,
// 		&m_PBits, NULL, 0
// 	);

// 	m_hbitmap_old = (HBITMAP)::SelectObject(_hdc_, m_hbitmap);
// 	DWORD dwLength;
// 	if ((m_DIBWidth * 3) % 4 == 0) /* �o�b�t�@�̂P���C���̒������v�Z */
// 		dwLength = m_DIBWidth * 3;
// 	else
// 		dwLength = m_DIBWidth * 3 + (4 - (m_DIBWidth * 3) % 4);
// 	//	LPBYTE lpPixel,lpBuf;
// 	//	lpBuf=(LPBYTE)GlobalAlloc(GPTR,sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwLength*m_DIBHeight);
// 	//	lpPixel=lpBuf+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

// 	int		_pfid = ChoosePixelFormat(_hdc_, &_pfd);
// 	::SetPixelFormat(_hdc_, _pfid, &_pfd);
// 	HGLRC	_hrc = ::wglCreateContext(_hdc_);
// 	::wglMakeCurrent(_hdc_, _hrc);
// 	Init(viewWidth_,viewHeight_, depthResolution);
// 	glGetIntegerv(GL_VIEWPORT, view);
// 	for (int i = 0; i < dataNum; i++) {
// 		float* trsVert = (float*)malloc(sizeof(float)*vtNumArray[i] * 3);
// 		float* vertexp = vertexPointers[i];
// 		for (int j = 0; j < vtNumArray[i]; j++) {
// 			Vector3d tp;
// 			Vector3d pp;
// 			pp << vertexp[j * 3], vertexp[j * 3 + 1], vertexp[j * 3 + 2];

// 			sphericalTrans_renderer(tp, pp, center, viewDirection);
// 			trsVert[j * 3] = (float)tp(0, 0);
// 			trsVert[j * 3 + 1] = (float)tp(1, 0);
// 			trsVert[j * 3 + 2] = (float)tp(2, 0);
// 			//			cout<<tp<<endl;
// 		}
// 		display(trsVert, facePointers[i], reflectancePointers[i], meshNumArray[i], vtNumArray[i]);
// 		free(trsVert);
// 	}

// 	//	BitBlt(dhdc,0,0,m_DIBWidth,m_DIBHeight,_hdc_,0,0,SRCCOPY);
// 	//	SelectObject(dhdc,m_hbitmap_old);
// 	//	GetDIBits(_hdc_,m_hbitmap,0,m_DIBHeight,lpPixel,(LPBITMAPINFO)m_PBIH,DIB_RGB_COLORS);
// 	reflectanceImage = (GLfloat*)malloc(sizeof(GLfloat)*view[2] * view[3]);
// 	glReadPixels(view[0], view[1], view[2], view[3], GL_RED, GL_FLOAT, reflectanceImage);
// 	depthArray = (GLfloat*)malloc(sizeof(GLfloat)*view[2] * view[3]);
// 	glReadPixels(view[0], view[1], view[2], view[3], GL_DEPTH_COMPONENT, GL_FLOAT, depthArray);
// 	normArray = (GLfloat*)malloc(sizeof(GLfloat)*view[2] * view[3]);

// 	double th = M_PI / viewHeight;
// 	Vector3d zdrc;

// 	for (int x = 0; x < viewHeight * 2; x++) {
// 		for (int y = 0; y < viewHeight; y++) {
// 			int idx = y * viewHeight * 2 + x;
// 			if (y == 0 || y == viewHeight - 1) { normArray[idx] = 0; continue; }
// 			int prexidx = x != 0 ? y * viewHeight * 2 + x - 1 : y * viewHeight * 2 + viewHeight * 2 - 1;
// 			int postxidx = x != viewHeight * 2 - 1 ? y * viewHeight * 2 + x + 1 : y * viewHeight * 2;
// 			int preyidx = idx - viewHeight * 2;
// 			int postyidx = idx + viewHeight * 2;
// 			if (depthArray[idx] == 1.0 || depthArray[prexidx] == 1.0 || depthArray[postxidx] == 1.0 || depthArray[preyidx] == 1.0 || depthArray[postyidx] == 1.0) {
// 				normArray[idx] = 0;
// 				continue;
// 			}

// 			/*double t=M_PI*(depthArray[postxidx]+depthArray[prexidx])*125/viewHeight;//125= viewOrtho far/4

// 			double d=(depthArray[postxidx]-depthArray[prexidx])*500;
// 			double t2=M_PI*(depthArray[postyidx]+depthArray[preyidx])*125/viewHeight;//125= viewOrtho far/4
// 			double d2=(depthArray[postyidx]-depthArray[preyidx])*500;*/
// 			Vector3d v1, v2;
// 			v1 << (depthArray[prexidx] + depthArray[postxidx])*sin(th) * 500, 0, (depthArray[prexidx] - depthArray[postxidx])*cos(th) * 500;
// 			v2 << 0, (depthArray[preyidx] + depthArray[postyidx])*sin(th) * 500, (depthArray[preyidx] - depthArray[postyidx])*cos(th) * 500;
// 			Vector3d n;
// 			n = v1.cross(v2);
// 			if (n(2) < 0)n = -n;
// 			n.normalize();
// 			normArray[idx] = n(2);//max...2.0
// 		}
// 	}
// 	//	GetObject(m_hbitmap,
// 	::wglMakeCurrent(0, 0);
// 	//	GlobalFree(lpBuf);
// 	//	GlobalFree(lpBuf);
// 	//	free(buffer);
// 	// return 0;
// }

void PanoramaRenderer::render(Matrix4d cameraParam) {
	GLint view[4];

	if (type==PERSPECTIVE) {
		InitPers(viewWidth_,viewHeight_,znear, depthResolution,intrinsic);
		glGetIntegerv(GL_VIEWPORT, view);
		for (int i = 0; i < dataNum; i++) {
			float* trsVert = (float*)malloc(sizeof(float)*vtNumArray[i] * 3);
			float* vertexp = vertexPointers[i];
			for (int j = 0; j < vtNumArray[i]; j++) {
				Vector3d tp;
				Vector3d pp;
				pp << vertexp[j * 3], vertexp[j * 3 + 1], vertexp[j * 3 + 2];

				Vector4d pt_;
				pt_ << pp(0), pp(1), pp(2), 1;
				Vector4d tp_ = cameraParam * pt_;

				trsVert[j * 3] = (float)tp_(0);
				trsVert[j * 3 + 1] = (float)tp_(1);
				trsVert[j * 3 + 2] = (float)tp_(2);

			}

			const GLfloat lightPos[] = { 0 , 0 , 0 , 1.0 };
			const GLfloat lightCol[] = { 1 , 0 , 0 , 1.0 };


				//float* trsVert = (float*)malloc(sizeof(float)*vtNumArray[i] * 3);
			glBegin(GL_TRIANGLES);
			for (int j = 0; j < meshNumArray[i]; j++) {
				int index1 = facePointers[i][j * 3];
				int index2 = facePointers[i][j * 3 + 1];
				int index3 = facePointers[i][j * 3 + 2];

				glColor3f(reflectancePointers[i][index1], reflectancePointers[i][index1], reflectancePointers[i][index1]);
				glVertex3f(trsVert[index1 * 3], trsVert[index1 * 3 + 1], trsVert[index1 * 3 + 2]);
				glColor3f(reflectancePointers[i][index2], reflectancePointers[i][index2], reflectancePointers[i][index2]);
				glVertex3f(trsVert[index2 * 3], trsVert[index2 * 3 + 1], trsVert[index2 * 3 + 2]);
				glColor3f(reflectancePointers[i][index3], reflectancePointers[i][index3], reflectancePointers[i][index3]);
				glVertex3f(trsVert[index3 * 3], trsVert[index3 * 3 + 1], trsVert[index3 * 3 + 2]);
				//cout << trsVert[index3 * 3] << "," << trsVert[index3 * 3 + 1] << "," << trsVert[index3 * 3 + 2] << "," << endl;
			}
			glEnd();
			
			free(trsVert);
		}
	}
	else if (type == FISHEYE) {
		InitFE(viewWidth_, viewHeight_, depthResolution);
		glGetIntegerv(GL_VIEWPORT, view);
		for (int i = 0; i < dataNum; i++) {
			float* trsVert = (float*)malloc(sizeof(float)*vtNumArray[i] * 3);
			float* vertexp = vertexPointers[i];

			for (int j = 0; j < vtNumArray[i]; j++) {
				Vector3d tp;
				Vector3d pp;
				pp << vertexp[j * 3], vertexp[j * 3 + 1], vertexp[j * 3 + 2];

				fisheyeTrans_renderer(tp, pp, cameraParam);
				trsVert[j * 3] = (float)tp(0, 0);
				trsVert[j * 3 + 1] = (float)tp(1, 0);
				trsVert[j * 3 + 2] = (float)tp(2, 0);
	//	cout<<tp<<","<<endl;
			}
			glBegin(GL_TRIANGLES);
			double thresh = (M_PI * 3 / 4)*(M_PI * 3 / 4);
			for (int j = 0; j < meshNumArray[i]; j++) {
				
				int index1 = facePointers[i][j * 3];
				int index2 = facePointers[i][j * 3 + 1];
				int index3 = facePointers[i][j * 3 + 2];

				double r1, r2, r3;
				r1 = trsVert[index1 * 3] * trsVert[index1 * 3] + trsVert[index1 * 3 + 1] * trsVert[index1 * 3 + 1];
				r2 = trsVert[index2 * 3] * trsVert[index2 * 3] + trsVert[index2 * 3 + 1] * trsVert[index2 * 3 + 1];
				r3 = trsVert[index3 * 3] * trsVert[index3 * 3] + trsVert[index3 * 3 + 1] * trsVert[index3 * 3 + 1];
				if (r1 > thresh || r2 > thresh && r3 > thresh)continue;

				glColor3f(reflectancePointers[i][index1], reflectancePointers[i][index1], reflectancePointers[i][index1]);
				glVertex3f(trsVert[index1 * 3], trsVert[index1 * 3 + 1], trsVert[index1 * 3 + 2]);
				glColor3f(reflectancePointers[i][index2], reflectancePointers[i][index2], reflectancePointers[i][index2]);
				glVertex3f(trsVert[index2 * 3], trsVert[index2 * 3 + 1], trsVert[index2 * 3 + 2]);
				glColor3f(reflectancePointers[i][index3], reflectancePointers[i][index3], reflectancePointers[i][index3]);
				glVertex3f(trsVert[index3 * 3], trsVert[index3 * 3 + 1], trsVert[index3 * 3 + 2]);

			}
			glEnd();

			free(trsVert);
		}

	}
	else {
		Init(viewWidth_, viewHeight_, depthResolution);
		glGetIntegerv(GL_VIEWPORT, view);
		for (int i = 0; i < dataNum; i++) {
			float* trsVert = (float*)malloc(sizeof(float)*vtNumArray[i] * 3);
			float* vertexp = vertexPointers[i];
			for (int j = 0; j < vtNumArray[i]; j++) {
				Vector3d tp;
				Vector3d pp;
				pp << vertexp[j * 3], vertexp[j * 3 + 1], vertexp[j * 3 + 2];

				sphericalTrans_renderer(tp, pp, cameraParam);
				trsVert[j * 3] = (float)tp(0, 0);
				trsVert[j * 3 + 1] = (float)tp(1, 0);
				trsVert[j * 3 + 2] = (float)tp(2, 0);
				//			cout<<tp<<endl;
			}
			display(trsVert, facePointers[i], reflectancePointers[i], meshNumArray[i], vtNumArray[i]);
			free(trsVert);
		}
	}
	
	


	//	BitBlt(dhdc,0,0,m_DIBWidth,m_DIBHeight,_hdc_,0,0,SRCCOPY);
	//	SelectObject(dhdc,m_hbitmap_old);
	//	GetDIBits(_hdc_,m_hbitmap,0,m_DIBHeight,lpPixel,(LPBITMAPINFO)m_PBIH,DIB_RGB_COLORS);
	reflectanceImage = (GLfloat*)malloc(sizeof(GLfloat)*view[2] * view[3]);
	glReadPixels(view[0], view[1], view[2], view[3], GL_RED, GL_FLOAT, reflectanceImage);
	depthArray = (GLfloat*)malloc(sizeof(GLfloat)*view[2] * view[3]);
	glReadPixels(view[0], view[1], view[2], view[3], GL_DEPTH_COMPONENT, GL_FLOAT, depthArray);
	cout << view[2] << "," << view[3] << endl;
	//::wglMakeCurrent(0, 0);
	//	GlobalFree(lpBuf);
	//	GlobalFree(lpBuf);
	//	free(buffer);
	// return 0;
}


void PanoramaRenderer::renderColor(Matrix4d& cameraParam) {
	#if defined(__unix__)
	// XEvent xev;
	// XNextEvent(display_, &xev);
	#endif
	GLint view[4];
	GLint maj_v,min_v;
	glGetIntegerv(GL_MAJOR_VERSION, &maj_v);
	glGetIntegerv(GL_MINOR_VERSION, &min_v);
    std::cout << maj_v<<","<<min_v << std::endl;
	if (type == PERSPECTIVE) {
		InitPers(viewWidth_, viewHeight_, znear, depthResolution, intrinsic);
		glGetIntegerv(GL_VIEWPORT, view);
		for (int i = 0; i < dataNum; i++) {
			float* trsVert = (float*)malloc(sizeof(float)*vtNumArray[i] * 3);
			float* vertexp = vertexPointers[i];
			for (int j = 0; j < vtNumArray[i]; j++) {
				Vector3d tp;
				Vector3d pp;
				pp << vertexp[j * 3], vertexp[j * 3 + 1], vertexp[j * 3 + 2];

				Vector4d pt_;
				pt_ << pp(0), pp(1), pp(2), 1;
				Vector4d tp_ = cameraParam * pt_;

				trsVert[j * 3] = (float)tp_(0);
				trsVert[j * 3 + 1] = (float)tp_(1);
				trsVert[j * 3 + 2] = (float)tp_(2);

			}

			const GLfloat lightPos[] = { 0 , 0 , 0 , 1.0 };
			const GLfloat lightCol[] = { 1 , 0 , 0 , 1.0 };

			//float* trsVert = (float*)malloc(sizeof(float)*vtNumArray[i] * 3);
			glBegin(GL_TRIANGLES);

			for (int j = 0; j < meshNumArray[i]; j++) {
				int index1 = facePointers[i][j * 3];
				int index2 = facePointers[i][j * 3 + 1];
				int index3 = facePointers[i][j * 3 + 2];


				glColor3ub(rgbaPointers[i][index1 * 4], rgbaPointers[i][index1 * 4 + 1], rgbaPointers[i][index1 * 4 + 2]);
				glVertex3f(trsVert[index1 * 3], trsVert[index1 * 3 + 1], trsVert[index1 * 3 + 2]);
				glColor3ub(rgbaPointers[i][index2 * 4], rgbaPointers[i][index2 * 4 + 1], rgbaPointers[i][index2 * 4 + 2]);
				glVertex3f(trsVert[index2 * 3], trsVert[index2 * 3 + 1], trsVert[index2 * 3 + 2]);
				glColor3ub(rgbaPointers[i][index3 * 4], rgbaPointers[i][index3 * 4 + 1], rgbaPointers[i][index3 * 4 + 2]);
				glVertex3f(trsVert[index3 * 3], trsVert[index3 * 3 + 1], trsVert[index3 * 3 + 2]);
				//cout << trsVert[index3 * 3] << "," << trsVert[index3 * 3 + 1] << "," << trsVert[index3 * 3 + 2] << "," << endl;
			}
			glEnd();

			free(trsVert);
		}
	}
	else if (type == FISHEYE) {
		InitFE(viewWidth_, viewHeight_, depthResolution);
		glGetIntegerv(GL_VIEWPORT, view);
		for (int i = 0; i < dataNum; i++) {
			float* trsVert = (float*)malloc(sizeof(float)*vtNumArray[i] * 3);
			float* vertexp = vertexPointers[i];

			for (int j = 0; j < vtNumArray[i]; j++) {
				Vector3d tp;
				Vector3d pp;
				pp << vertexp[j * 3], vertexp[j * 3 + 1], vertexp[j * 3 + 2];

				fisheyeTrans_renderer(tp, pp, cameraParam);
				trsVert[j * 3] = (float)tp(0, 0);
				trsVert[j * 3 + 1] = (float)tp(1, 0);
				trsVert[j * 3 + 2] = (float)tp(2, 0);
				//	cout<<tp<<","<<endl;
			}
			glBegin(GL_TRIANGLES);
			double thresh = (M_PI * 3 / 4)*(M_PI * 3 / 4);
			for (int j = 0; j < meshNumArray[i]; j++) {

				int index1 = facePointers[i][j * 3];
				int index2 = facePointers[i][j * 3 + 1];
				int index3 = facePointers[i][j * 3 + 2];

				double r1, r2, r3;
				r1 = trsVert[index1 * 3] * trsVert[index1 * 3] + trsVert[index1 * 3 + 1] * trsVert[index1 * 3 + 1];
				r2 = trsVert[index2 * 3] * trsVert[index2 * 3] + trsVert[index2 * 3 + 1] * trsVert[index2 * 3 + 1];
				r3 = trsVert[index3 * 3] * trsVert[index3 * 3] + trsVert[index3 * 3 + 1] * trsVert[index3 * 3 + 1];
				if (r1 > thresh || r2 > thresh && r3 > thresh)continue;
				glColor3ub(rgbaPointers[i][index1 * 4], rgbaPointers[i][index1 * 4 + 1], rgbaPointers[i][index1 * 4 + 2]);
				glVertex3f(trsVert[index1 * 3], trsVert[index1 * 3 + 1], trsVert[index1 * 3 + 2]);
				glColor3ub(rgbaPointers[i][index2 * 4], rgbaPointers[i][index2 * 4 + 1], rgbaPointers[i][index2 * 4 + 2]);
				glVertex3f(trsVert[index2 * 3], trsVert[index2 * 3 + 1], trsVert[index2 * 3 + 2]);
				glColor3ub(rgbaPointers[i][index3 * 4], rgbaPointers[i][index3 * 4 + 1], rgbaPointers[i][index3 * 4 + 2]);
				glVertex3f(trsVert[index3 * 3], trsVert[index3 * 3 + 1], trsVert[index3 * 3 + 2]);
			}
			glEnd();


//test
// static const GLfloat g_vertex_buffer_data[] = {
//    -1.0f, -1.0f, 3.0f,
//    1.0f, -1.0f, 3.0f,
//    0.0f,  1.0f, 3.0f,
// };
//  // これが頂点バッファを指し示すものとなります。
// GLuint vertexbuffer;
// // バッファを1つ作り、vertexbufferに結果IDを入れます。
// glGenBuffers(1, &vertexbuffer);
// // 次のコマンドは'vertexbuffer'バッファについてです。
// glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
// // 頂点をOpenGLに渡します。
// glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

// 	glEnableVertexAttribArray(0);
// 	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
// 	glVertexAttribPointer(
// 	0,                  // 属性0：0に特に理由はありません。しかし、シェーダ内のlayoutとあわせないといけません。
// 	3,                  // サイズ
// 	GL_FLOAT,           // タイプ
// 	GL_FALSE,           // 正規化？
// 	0,                  // ストライド
// 	(void*)0            // 配列バッファオフセット
// 	);

// 	// 三角形を描きます！
// 	glDrawArrays(GL_TRIANGLES, 0, 3); // 頂点0から始まります。合計3つの頂点です。&rarr;1つの三角形です。

// 	glDisableVertexAttribArray(0);
// 	std::cout<<"pikumin!!"<<std::endl;
	glFlush();
//test end

			free(trsVert);
		}
	}
	else {
		Init(viewWidth_, viewHeight_, depthResolution);
		glGetIntegerv(GL_VIEWPORT, view);
		for (int i = 0; i < dataNum; i++) {
			float* trsVert = (float*)malloc(sizeof(float)*vtNumArray[i] * 3);
			float* vertexp = vertexPointers[i];
			for (int j = 0; j < vtNumArray[i]; j++) {
				Vector3d tp;
				Vector3d pp;
				pp << vertexp[j * 3], vertexp[j * 3 + 1], vertexp[j * 3 + 2];

				sphericalTrans_renderer(tp, pp, cameraParam);
				trsVert[j * 3] = (float)tp(0, 0);
				trsVert[j * 3 + 1] = (float)tp(1, 0);
				trsVert[j * 3 + 2] = (float)tp(2, 0);
				//			cout<<tp<<endl;
			}
			displayrgb(trsVert, facePointers[i], rgbaPointers[i], meshNumArray[i], vtNumArray[i]);
			free(trsVert);
		}
	}
	//	BitBlt(dhdc,0,0,m_DIBWidth,m_DIBHeight,_hdc_,0,0,SRCCOPY);
	//	SelectObject(dhdc,m_hbitmap_old);
	//	GetDIBits(_hdc_,m_hbitmap,0,m_DIBHeight,lpPixel,(LPBITMAPINFO)m_PBIH,DIB_RGB_COLORS);

	colorImage = (GLubyte*)malloc(sizeof(GLubyte)*view[2] * view[3] * 3);
	glReadPixels(view[0], view[1], view[2], view[3], GL_RGB, GL_UNSIGNED_BYTE, colorImage);
	depthArray = (GLfloat*)malloc(sizeof(GLfloat)*view[2] * view[3]);
	glReadPixels(view[0], view[1], view[2], view[3], GL_DEPTH_COMPONENT, GL_FLOAT, depthArray);
	//normArray = (GLfloat*)malloc(sizeof(GLfloat)*view[2] * view[3]);
#if defined(__unix__)
//std::cout <<"swap"<<std::endl;
//	glXSwapBuffers(display_, pbuffer);
//	glReadBuffer(GL_BACK);
#endif
	//::wglMakeCurrent(0, 0);
	//	GlobalFree(lpBuf);
	//	GlobalFree(lpBuf);
	//	free(buffer);
	// return 0;
}


// void PanoramaRenderer::render_dot(Matrix4d& cameraParameter, int viewSize) {
// 	PIXELFORMATDESCRIPTOR _pfd = {
//  sizeof(PIXELFORMATDESCRIPTOR),	//	Size of this struct
//  1,	//	Versin of this structure
//  PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL |
//  PFD_GENERIC_ACCELERATED | PFD_DOUBLEBUFFER,
// 		//		PFD_GENERIC_FORMAT,
// 				//	Pixel buffer flags
// 				PFD_TYPE_RGBA,	//	Type of pixel data
// 				24,	//	The number of color bitplanes
// 				0, 0, 0, 0, 0, 0,	//	Number of each color bitplanes and shift count
// 				0, 0,	//	Number of alpha bitplanes and shift count
// 				0, 0, 0, 0, 0,	//	Number of accumulation bits
// 				32,	//	Z depth
// 				0,	//	Stencil depth
// 				0,	//	Number of auxiliary buffers
// 				PFD_MAIN_PLANE,	//	Ignored
// 				0,	//	Reserved
// 				0,	//	Ignored
// 				0,	//	Transparent color value
// 				0,	//	Ignored
// 	};
// 	GLint view[4];
// 	HDC		_hdc_ = CreateCompatibleDC(NULL);
// 	viewHeight = viewSize;
// 	DWORD m_DIBWidth = viewSize * 2;
// 	DWORD m_DIBHeight = viewSize;
// 	DWORD m_BPP = 24;

// 	// Create BITMAPINFOHEADER
// 	BITMAPINFOHEADER* m_PBIH = new BITMAPINFOHEADER;
// 	int iSize = sizeof(BITMAPINFOHEADER);
// 	::memset(m_PBIH, 0, iSize);

// 	m_PBIH->biSize = sizeof(BITMAPINFOHEADER);
// 	m_PBIH->biWidth = m_DIBWidth;
// 	m_PBIH->biHeight = m_DIBHeight;
// 	m_PBIH->biPlanes = 1;
// 	m_PBIH->biBitCount = m_BPP;
// 	m_PBIH->biCompression = BI_RGB;

// 	// Create DIB
// 	void* m_PBits;
// 	HBITMAP m_hbitmap_old;
// 	HBITMAP m_hbitmap = ::CreateDIBSection(
// 		_hdc_,
// 		(BITMAPINFO*)m_PBIH, DIB_RGB_COLORS,
// 		&m_PBits, NULL, 0
// 	);

// 	m_hbitmap_old = (HBITMAP)::SelectObject(_hdc_, m_hbitmap);
// 	DWORD dwLength;
// 	if ((m_DIBWidth * 3) % 4 == 0) /* �o�b�t�@�̂P���C���̒������v�Z */
// 		dwLength = m_DIBWidth * 3;
// 	else
// 		dwLength = m_DIBWidth * 3 + (4 - (m_DIBWidth * 3) % 4);
// 	//	LPBYTE lpPixel,lpBuf;
// 	//	lpBuf=(LPBYTE)GlobalAlloc(GPTR,sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwLength*m_DIBHeight);
// 	//	lpPixel=lpBuf+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

// 	int		_pfid = ChoosePixelFormat(_hdc_, &_pfd);
// 	::SetPixelFormat(_hdc_, _pfid, &_pfd);
// 	HGLRC	_hrc = ::wglCreateContext(_hdc_);
// 	::wglMakeCurrent(_hdc_, _hrc);
// 	Init(viewWidth_, viewHeight_, depthResolution);
// 	glGetIntegerv(GL_VIEWPORT, view);
// 	for (int i = 0; i < dataNum; i++) {
// 		float* trsVert = (float*)malloc(sizeof(float)*vtNumArray[i] * 3);
// 		float* vertexp = vertexPointers[i];
// 		for (int j = 0; j < vtNumArray[i]; j++) {
// 			Vector3d tp;
// 			Vector3d pp;
// 			pp << vertexp[j * 3], vertexp[j * 3 + 1], vertexp[j * 3 + 2];

// 			//sphericalTrans_renderer(tp, pp, center, viewDirection);
// 			sphericalTrans_renderer(tp, pp, cameraParameter);
// 			trsVert[j * 3] = (float)tp(0, 0);
// 			trsVert[j * 3 + 1] = (float)tp(1, 0);
// 			trsVert[j * 3 + 2] = (float)tp(2, 0);
// 			//			cout<<tp<<endl;
// 		}
// 		display_points(trsVert, facePointers[i], reflectancePointers[i], meshNumArray[i], vtNumArray[i]);
// 		free(trsVert);
// 	}

// 	//	BitBlt(dhdc,0,0,m_DIBWidth,m_DIBHeight,_hdc_,0,0,SRCCOPY);
// 	//	SelectObject(dhdc,m_hbitmap_old);
// 	//	GetDIBits(_hdc_,m_hbitmap,0,m_DIBHeight,lpPixel,(LPBITMAPINFO)m_PBIH,DIB_RGB_COLORS);
// 	reflectanceImage = (GLfloat*)malloc(sizeof(GLfloat)*view[2] * view[3]);
// 	glReadPixels(view[0], view[1], view[2], view[3], GL_RED, GL_FLOAT, reflectanceImage);
// 	depthArray = (GLfloat*)malloc(sizeof(GLfloat)*view[2] * view[3]);
// 	glReadPixels(view[0], view[1], view[2], view[3], GL_DEPTH_COMPONENT, GL_FLOAT, depthArray);
// 	normArray = (GLfloat*)malloc(sizeof(GLfloat)*view[2] * view[3]);
// 	//for (int x = 0;x < viewHeight * 2;x++) {
// 	//	for (int y = 0;y < viewHeight;y++) {
// 	//		int idx = y * viewHeight * 2 + x;
// 	//		if (y == 0 || y == viewHeight - 1) { normArray[idx] = 0;continue; }
// 	//		int prexidx = x != 0 ? y * viewHeight * 2 + x - 1 : y * viewHeight * 2 + viewHeight * 2 - 1;
// 	//		int postxidx = x != viewHeight * 2 - 1 ? y * viewHeight * 2 + x + 1 : y * viewHeight * 2;
// 	//		int preyidx = idx - viewHeight * 2;
// 	//		int postyidx = idx + viewHeight * 2;
// 	//		if (depthArray[idx] == 1.0 || depthArray[prexidx] == 1.0 || depthArray[postxidx] == 1.0 || depthArray[preyidx] == 1.0 || depthArray[postyidx] == 1.0) {
// 	//			normArray[idx] = 0;
// 	//			continue;
// 	//		}
// 	//		double t = M_PI * (depthArray[postxidx] + depthArray[prexidx]) * 125 / viewHeight;//125= viewOrtho far/4
// 	//		double d = (depthArray[postxidx] - depthArray[prexidx]) * 500;
// 	//		double t2 = M_PI * (depthArray[postyidx] + depthArray[preyidx]) * 125 / viewHeight;//125= viewOrtho far/4
// 	//		double d2 = (depthArray[postyidx] - depthArray[preyidx]) * 500;
// 	//		normArray[idx] = (t / sqrt(t*t + d * d) + t2 / sqrt(t2*t2 + d2 * d2)) / 2;//max...2.0
// 	//	}
// 	//}

// 	//	GetObject(m_hbitmap,
// 	::wglMakeCurrent(0, 0);
// 	//	GlobalFree(lpBuf);
// 	//	GlobalFree(lpBuf);
// 	//	free(buffer);
// 	// return 0;
// }




// void PanoramaRenderer::renderColor(Vector3d& center, Matrix3d& viewDirection, int viewSize) {
// 	PIXELFORMATDESCRIPTOR _pfd = {
//  sizeof(PIXELFORMATDESCRIPTOR),	//	Size of this struct
//  1,	//	Versin of this structure
//  PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL |
//  PFD_GENERIC_ACCELERATED,
// 		//		PFD_GENERIC_FORMAT,
// 				//	Pixel buffer flags
// 				PFD_TYPE_RGBA,	//	Type of pixel data
// 				24,	//	The number of color bitplanes
// 				0, 0, 0, 0, 0, 0,	//	Number of each color bitplanes and shift count
// 				0, 0,	//	Number of alpha bitplanes and shift count
// 				0, 0, 0, 0, 0,	//	Number of accumulation bits
// 				32,	//	Z depth
// 				0,	//	Stencil depth
// 				0,	//	Number of auxiliary buffers
// 				PFD_MAIN_PLANE,	//	Ignored
// 				0,	//	Reserved
// 				0,	//	Ignored
// 				0,	//	Transparent color value
// 				0,	//	Ignored
// 	};
// 	GLint view[4];
// 	HDC		_hdc_ = CreateCompatibleDC(NULL);
// 	viewHeight = viewSize;
// 	DWORD m_DIBWidth = viewSize * 2;
// 	DWORD m_DIBHeight = viewSize;
// 	DWORD m_BPP = 24;

// 	// Create BITMAPINFOHEADER
// 	BITMAPINFOHEADER* m_PBIH = new BITMAPINFOHEADER;
// 	int iSize = sizeof(BITMAPINFOHEADER);
// 	::memset(m_PBIH, 0, iSize);

// 	m_PBIH->biSize = sizeof(BITMAPINFOHEADER);
// 	m_PBIH->biWidth = m_DIBWidth;
// 	m_PBIH->biHeight = m_DIBHeight;
// 	m_PBIH->biPlanes = 1;
// 	m_PBIH->biBitCount = m_BPP;
// 	m_PBIH->biCompression = BI_RGB;

// 	// Create DIB
// 	void* m_PBits;
// 	HBITMAP m_hbitmap_old;
// 	HBITMAP m_hbitmap = ::CreateDIBSection(
// 		_hdc_,
// 		(BITMAPINFO*)m_PBIH, DIB_RGB_COLORS,
// 		&m_PBits, NULL, 0
// 	);

// 	m_hbitmap_old = (HBITMAP)::SelectObject(_hdc_, m_hbitmap);
// 	DWORD dwLength;
// 	if ((m_DIBWidth * 3) % 4 == 0) /* �o�b�t�@�̂P���C���̒������v�Z */
// 		dwLength = m_DIBWidth * 3;
// 	else
// 		dwLength = m_DIBWidth * 3 + (4 - (m_DIBWidth * 3) % 4);
// 	//	LPBYTE lpPixel,lpBuf;
// 	//	lpBuf=(LPBYTE)GlobalAlloc(GPTR,sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwLength*m_DIBHeight);
// 	//	lpPixel=lpBuf+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

// 	int		_pfid = ChoosePixelFormat(_hdc_, &_pfd);
// 	::SetPixelFormat(_hdc_, _pfid, &_pfd);
// 	HGLRC	_hrc = ::wglCreateContext(_hdc_);
// 	::wglMakeCurrent(_hdc_, _hrc);
// 	Init(viewWidth_, viewHeight_, depthResolution);
// 	glGetIntegerv(GL_VIEWPORT, view);
// 	for (int i = 0; i < dataNum; i++) {
// 		float* trsVert = (float*)malloc(sizeof(float)*vtNumArray[i] * 3);
// 		float* vertexp = vertexPointers[i];
// 		for (int j = 0; j < vtNumArray[i]; j++) {
// 			Vector3d tp;
// 			Vector3d pp;
// 			pp << vertexp[j * 3], vertexp[j * 3 + 1], vertexp[j * 3 + 2];

// 			sphericalTrans_renderer(tp, pp, center, viewDirection);
// 			trsVert[j * 3] = (float)tp(0, 0);
// 			trsVert[j * 3 + 1] = (float)tp(1, 0);
// 			trsVert[j * 3 + 2] = (float)tp(2, 0);
// 			//			cout<<tp<<endl;
// 		}
// 		displayrgb(trsVert, facePointers[i], rgbaPointers[i], meshNumArray[i], vtNumArray[i]);
// 		free(trsVert);
// 	}

// 	//	BitBlt(dhdc,0,0,m_DIBWidth,m_DIBHeight,_hdc_,0,0,SRCCOPY);
// 	//	SelectObject(dhdc,m_hbitmap_old);
// 	//	GetDIBits(_hdc_,m_hbitmap,0,m_DIBHeight,lpPixel,(LPBITMAPINFO)m_PBIH,DIB_RGB_COLORS);
// 	colorImage = (GLubyte*)malloc(sizeof(GLubyte)*view[2] * view[3] * 3);
// 	glReadPixels(view[0], view[1], view[2], view[3], GL_RGB, GL_UNSIGNED_BYTE, colorImage);
// 	depthArray = (GLfloat*)malloc(sizeof(GLfloat)*view[2] * view[3]);
// 	glReadPixels(view[0], view[1], view[2], view[3], GL_DEPTH_COMPONENT, GL_FLOAT, depthArray);
// 	normArray = (GLfloat*)malloc(sizeof(GLfloat)*view[2] * view[3]);

// 	double th = M_PI / viewHeight;
// 	Vector3d zdrc;

// 	for (int x = 0; x < viewHeight * 2; x++) {
// 		for (int y = 0; y < viewHeight; y++) {
// 			int idx = y * viewHeight * 2 + x;
// 			if (y == 0 || y == viewHeight - 1) { normArray[idx] = 0; continue; }
// 			int prexidx = x != 0 ? y * viewHeight * 2 + x - 1 : y * viewHeight * 2 + viewHeight * 2 - 1;
// 			int postxidx = x != viewHeight * 2 - 1 ? y * viewHeight * 2 + x + 1 : y * viewHeight * 2;
// 			int preyidx = idx - viewHeight * 2;
// 			int postyidx = idx + viewHeight * 2;
// 			if (depthArray[idx] == 1.0 || depthArray[prexidx] == 1.0 || depthArray[postxidx] == 1.0 || depthArray[preyidx] == 1.0 || depthArray[postyidx] == 1.0) {
// 				normArray[idx] = 0;
// 				continue;
// 			}

// 			/*double t=M_PI*(depthArray[postxidx]+depthArray[prexidx])*125/viewHeight;//125= viewOrtho far/4

// 			double d=(depthArray[postxidx]-depthArray[prexidx])*500;
// 			double t2=M_PI*(depthArray[postyidx]+depthArray[preyidx])*125/viewHeight;//125= viewOrtho far/4
// 			double d2=(depthArray[postyidx]-depthArray[preyidx])*500;*/
// 			Vector3d v1, v2;
// 			v1 << (depthArray[prexidx] + depthArray[postxidx])*sin(th) * 500, 0, (depthArray[prexidx] - depthArray[postxidx])*cos(th) * 500;
// 			v2 << 0, (depthArray[preyidx] + depthArray[postyidx])*sin(th) * 500, (depthArray[preyidx] - depthArray[postyidx])*cos(th) * 500;
// 			Vector3d n;
// 			n = v1.cross(v2);
// 			if (n(2) < 0)n = -n;
// 			n.normalize();
// 			normArray[idx] = n(2);//max...2.0
// 		}
// 	}
// 	//	GetObject(m_hbitmap,
// 	::wglMakeCurrent(0, 0);
// 	//	GlobalFree(lpBuf);
// 	//	GlobalFree(lpBuf);
// 	//	free(buffer);
// 	// return 0;
// }




// void PanoramaRenderer::renderColor(Matrix4d& cameraParameter, int viewSize) {
// 	PIXELFORMATDESCRIPTOR _pfd = {
//  sizeof(PIXELFORMATDESCRIPTOR),	//	Size of this struct
//  1,	//	Versin of this structure
//  PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL |
//  PFD_GENERIC_ACCELERATED,
// 		//		PFD_GENERIC_FORMAT,
// 				//	Pixel buffer flags
// 				PFD_TYPE_RGBA,	//	Type of pixel data
// 				24,	//	The number of color bitplanes
// 				0, 0, 0, 0, 0, 0,	//	Number of each color bitplanes and shift count
// 				0, 0,	//	Number of alpha bitplanes and shift count
// 				0, 0, 0, 0, 0,	//	Number of accumulation bits
// 				32,	//	Z depth
// 				0,	//	Stencil depth
// 				0,	//	Number of auxiliary buffers
// 				PFD_MAIN_PLANE,	//	Ignored
// 				0,	//	Reserved
// 				0,	//	Ignored
// 				0,	//	Transparent color value
// 				0,	//	Ignored
// 	};
// 	GLint view[4];
// 	HDC		_hdc_ = CreateCompatibleDC(NULL);
// 	viewHeight = viewSize;
// 	DWORD m_DIBWidth = viewSize * 2;
// 	DWORD m_DIBHeight = viewSize;
// 	DWORD m_BPP = 24;

// 	// Create BITMAPINFOHEADER
// 	BITMAPINFOHEADER* m_PBIH = new BITMAPINFOHEADER;
// 	int iSize = sizeof(BITMAPINFOHEADER);
// 	::memset(m_PBIH, 0, iSize);

// 	m_PBIH->biSize = sizeof(BITMAPINFOHEADER);
// 	m_PBIH->biWidth = m_DIBWidth;
// 	m_PBIH->biHeight = m_DIBHeight;
// 	m_PBIH->biPlanes = 1;
// 	m_PBIH->biBitCount = m_BPP;
// 	m_PBIH->biCompression = BI_RGB;

// 	// Create DIB
// 	void* m_PBits;
// 	HBITMAP m_hbitmap_old;
// 	HBITMAP m_hbitmap = ::CreateDIBSection(
// 		_hdc_,
// 		(BITMAPINFO*)m_PBIH, DIB_RGB_COLORS,
// 		&m_PBits, NULL, 0
// 	);

// 	m_hbitmap_old = (HBITMAP)::SelectObject(_hdc_, m_hbitmap);
// 	DWORD dwLength;
// 	if ((m_DIBWidth * 3) % 4 == 0) /* �o�b�t�@�̂P���C���̒������v�Z */
// 		dwLength = m_DIBWidth * 3;
// 	else
// 		dwLength = m_DIBWidth * 3 + (4 - (m_DIBWidth * 3) % 4);

// 	int		_pfid = ChoosePixelFormat(_hdc_, &_pfd);
// 	::SetPixelFormat(_hdc_, _pfid, &_pfd);
// 	HGLRC	_hrc = ::wglCreateContext(_hdc_);
// 	::wglMakeCurrent(_hdc_, _hrc);
// 	Init(viewWidth_, viewHeight_, depthResolution);
// 	glGetIntegerv(GL_VIEWPORT, view);
// 	for (int i = 0; i < dataNum; i++) {
// 		float* trsVert = (float*)malloc(sizeof(float)*vtNumArray[i] * 3);
// 		float* vertexp = vertexPointers[i];
// 		for (int j = 0; j < vtNumArray[i]; j++) {
// 			Vector3d tp;
// 			Vector3d pp;
// 			pp << vertexp[j * 3], vertexp[j * 3 + 1], vertexp[j * 3 + 2];

// 			sphericalTrans_renderer(tp, pp, cameraParameter);
// 			trsVert[j * 3] = (float)tp(0, 0);
// 			trsVert[j * 3 + 1] = (float)tp(1, 0);
// 			trsVert[j * 3 + 2] = (float)tp(2, 0);
// 			//			cout<<tp<<endl;
// 		}
// 		displayrgb(trsVert, facePointers[i], rgbaPointers[i], meshNumArray[i], vtNumArray[i]);
// 		free(trsVert);
// 	}

// 	//	BitBlt(dhdc,0,0,m_DIBWidth,m_DIBHeight,_hdc_,0,0,SRCCOPY);
// 	//	SelectObject(dhdc,m_hbitmap_old);
// 	//	GetDIBits(_hdc_,m_hbitmap,0,m_DIBHeight,lpPixel,(LPBITMAPINFO)m_PBIH,DIB_RGB_COLORS);
// 	colorImage = (GLubyte*)malloc(sizeof(GLubyte)*view[2] * view[3] * 3);
// 	glReadPixels(view[0], view[1], view[2], view[3], GL_RGB, GL_UNSIGNED_BYTE, colorImage);
// 	depthArray = (GLfloat*)malloc(sizeof(GLfloat)*view[2] * view[3]);
// 	glReadPixels(view[0], view[1], view[2], view[3], GL_DEPTH_COMPONENT, GL_FLOAT, depthArray);
// 	normArray = (GLfloat*)malloc(sizeof(GLfloat)*view[2] * view[3]);

// 	double th = M_PI / viewHeight;
// 	Vector3d zdrc;
// 	if (bNormalImage) {
// 		for (int x = 0; x < viewHeight * 2; x++) {
// 			for (int y = 0; y < viewHeight; y++) {
// 				int idx = y * viewHeight * 2 + x;
// 				if (y == 0 || y == viewHeight - 1) { normArray[idx] = 0; continue; }
// 				int prexidx = x != 0 ? y * viewHeight * 2 + x - 1 : y * viewHeight * 2 + viewHeight * 2 - 1;
// 				int postxidx = x != viewHeight * 2 - 1 ? y * viewHeight * 2 + x + 1 : y * viewHeight * 2;
// 				int preyidx = idx - viewHeight * 2;
// 				int postyidx = idx + viewHeight * 2;
// 				if (depthArray[idx] == 1.0 || depthArray[prexidx] == 1.0 || depthArray[postxidx] == 1.0 || depthArray[preyidx] == 1.0 || depthArray[postyidx] == 1.0) {
// 					normArray[idx] = 0;
// 					continue;
// 				}

// 				/*double t=M_PI*(depthArray[postxidx]+depthArray[prexidx])*125/viewHeight;//125= viewOrtho far/4

// 				double d=(depthArray[postxidx]-depthArray[prexidx])*500;
// 				double t2=M_PI*(depthArray[postyidx]+depthArray[preyidx])*125/viewHeight;//125= viewOrtho far/4
// 				double d2=(depthArray[postyidx]-depthArray[preyidx])*500;*/
// 				Vector3d v1, v2;
// 				v1 << (depthArray[prexidx] + depthArray[postxidx])*sin(th) * 500, 0, (depthArray[prexidx] - depthArray[postxidx])*cos(th) * 500;
// 				v2 << 0, (depthArray[preyidx] + depthArray[postyidx])*sin(th) * 500, (depthArray[preyidx] - depthArray[postyidx])*cos(th) * 500;
// 				Vector3d n;
// 				n = v1.cross(v2);
// 				if (n(2) < 0)n = -n;
// 				n.normalize();
// 				normArray[idx] = n(2);//max...2.0
// 			}
// 		}
// 	}
// 	//	GetObject(m_hbitmap,

// 	glFlush();
// 	glFinish();
// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// 	if (FALSE == ::wglMakeCurrent(0, 0))exit(ERROR);
// 	if (FALSE == ::wglDeleteContext(_hrc))exit(ERROR);
// 	//delete _hdc_;
// 	//free(m_PBits);
// 	DeleteDC(_hdc_);
// 	DeleteObject(m_hbitmap);

// 	//	::ReleaseDC(NULL,_hdc_);
// 	//	GlobalFree(lpBuf);
// 	//	GlobalFree(lpBuf);
// 	//	free(buffer);
// 	// return 0;
// }

//void PanoramaRenderer::outputReflectance(string fileName){
//	ofstream	_os(fileName, ios::binary);
//	if(!_os.is_open())
//		return;
////	Save image as bitmap.
//	BITMAPFILEHEADER	_fh;
//	BITMAPINFOHEADER	_if;
//	int		_fh_size = sizeof(BITMAPFILEHEADER);
//	int		_if_size = sizeof(BITMAPINFOHEADER);
//	int		_size = viewHeight * viewHeight * 2*3;
//	::ZeroMemory(&_fh, _fh_size);
//	::ZeroMemory(&_if, _if_size);
////	Initialize BITMAPFILEHEADER.
//	_fh.bfSize = _fh_size + _if_size + _size;
//	_fh.bfOffBits = _fh_size + _if_size;
//	((char*)&_fh)[0] = 'B';
//	((char*)&_fh)[1] = 'M';
////	Initialize BITMAPINFOHEADER.
//	_if.biSize = _if_size;
//	_if.biWidth = viewHeight*2;
//	_if.biHeight = viewHeight;
//	_if.biPlanes = 1;
//	_if.biBitCount = 24;
//	_if.biCompression = BI_RGB;
//	_if.biSizeImage = _size;
////	Reverse byte.
//	unsigned char*		_buffer = new unsigned char[_size];
//	float*		_tdata = reinterpret_cast<float*>(reflectanceImage);
//	unsigned char*		_cdata = _buffer;
//	int			i;
//	for(i = 0; i < _size/3; ++i){
//		//modification
//		//_tdata[0]=1-(1-_tdata[0])*(1-_tdata[0])*(1-_tdata[0]);//
//		//_tdata[0]=_tdata[0]<1/3.0?_tdata[0]*3:1;
//
//		unsigned char gr=_tdata[0]*255<255?(unsigned char)(_tdata[0]*255):255;
//		//unsigned char gr=i<_size/8?(unsigned char)(_tdata[0]*255):255;
//		_cdata[0] = gr;
//		_cdata[1] = gr;
//		_cdata[2] = gr;
//		_tdata ++;
//		_cdata += 3;
//	}
////	Write to file.
//	_os.write((char*)&_fh, _fh_size);
//	_os.write((char*)&_if, _if_size);
//	_os.write(reinterpret_cast<char*>(_buffer), _size);
//	_os.close();
//
//	delete []	_buffer;
//
//	return;
//
//}

void PanoramaRenderer::outputColor(string fileName) {
#if defined(WIN32) || defined(WIN64)
	ofstream	_os(fileName, ios::binary);
	if (!_os.is_open())
		return;
	//	Save image as bitmap.
	BITMAPFILEHEADER	_fh;
	BITMAPINFOHEADER	_if;
	int		_fh_size = sizeof(BITMAPFILEHEADER);
	int		_if_size = sizeof(BITMAPINFOHEADER);
	int		_size = viewHeight * viewHeight * 2 * 3;
	::ZeroMemory(&_fh, _fh_size);
	::ZeroMemory(&_if, _if_size);
	//	Initialize BITMAPFILEHEADER.
	_fh.bfSize = _fh_size + _if_size + _size;
	_fh.bfOffBits = _fh_size + _if_size;
	((char*)&_fh)[0] = 'B';
	((char*)&_fh)[1] = 'M';
	//	Initialize BITMAPINFOHEADER.
	_if.biSize = _if_size;
	_if.biWidth = viewHeight * 2;
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
	for (i = 0; i < _size / 3; ++i) {
		//modification

		_cdata[0] = _tdata[2];
		_cdata[1] = _tdata[1];
		_cdata[2] = _tdata[0];
		_tdata += 3;
		_cdata += 3;
	}
	//	Write to file.
	_os.write((char*)&_fh, _fh_size);
	_os.write((char*)&_if, _if_size);
	_os.write(reinterpret_cast<char*>(_buffer), _size);
	_os.close();

	free(_buffer);
#endif
	return;

}

void PanoramaRenderer::outputReflectance(string fileName) {
	#if defined(WIN32) || defined(WIN64)
	ofstream	_os(fileName, ios::binary);
	if (!_os.is_open())
		return;
	//	Save image as bitmap.
	BITMAPFILEHEADER	_fh;
	BITMAPINFOHEADER	_if;
	int		_fh_size = sizeof(BITMAPFILEHEADER);
	int		_if_size = sizeof(BITMAPINFOHEADER);
	int		_size = viewHeight_ * viewWidth_ * 3;
	::ZeroMemory(&_fh, _fh_size);
	::ZeroMemory(&_if, _if_size);
	//	Initialize BITMAPFILEHEADER.
	_fh.bfSize = _fh_size + _if_size + _size;
	_fh.bfOffBits = _fh_size + _if_size;
	((char*)&_fh)[0] = 'B';
	((char*)&_fh)[1] = 'M';
	//	Initialize BITMAPINFOHEADER.
	_if.biSize = _if_size;
	_if.biWidth = viewWidth_;
	_if.biHeight = viewHeight_;
	_if.biPlanes = 1;
	_if.biBitCount = 24;
	_if.biCompression = BI_RGB;
	_if.biSizeImage = _size;
	//	Reverse byte.
	//	unsigned char*		_buffer = new unsigned char[_size];
	unsigned char*		_buffer = (unsigned char*)malloc(sizeof(unsigned char)*_size);
	float*		_tdata = reinterpret_cast<float*>(reflectanceImage);
	unsigned char*		_cdata = _buffer;
	int			i;
	for (i = 0; i < _size / 3; ++i) {
		//modification

		_cdata[0] = _tdata[0] * 255;
		_cdata[1] = _tdata[0] * 255;
		_cdata[2] = _tdata[0] * 255;
		_tdata += 1;
		_cdata += 3;
	}
	//	Write to file.
	_os.write((char*)&_fh, _fh_size);
	_os.write((char*)&_if, _if_size);
	_os.write(reinterpret_cast<char*>(_buffer), _size);
	_os.close();

	free(_buffer);
#endif
	return;

}


void PanoramaRenderer::outputDepth(string fileName) {
	#if defined(WIN32) || defined(WIN64)
	ofstream	_os(fileName, ios::binary);
	if (!_os.is_open())
		return;
	//	Save image as bitmap.
	BITMAPFILEHEADER	_fh;
	BITMAPINFOHEADER	_if;
	int		_fh_size = sizeof(BITMAPFILEHEADER);
	int		_if_size = sizeof(BITMAPINFOHEADER);
	int		_size = viewHeight_ * viewWidth_* 3;
	::ZeroMemory(&_fh, _fh_size);
	::ZeroMemory(&_if, _if_size);
	//	Initialize BITMAPFILEHEADER.
	_fh.bfSize = _fh_size + _if_size + _size;
	_fh.bfOffBits = _fh_size + _if_size;
	((char*)&_fh)[0] = 'B';
	((char*)&_fh)[1] = 'M';
	//	Initialize BITMAPINFOHEADER.
	_if.biSize = _if_size;
	_if.biWidth = viewWidth_;
	_if.biHeight = viewHeight_;
	_if.biPlanes = 1;
	_if.biBitCount = 24;
	_if.biCompression = BI_RGB;
	_if.biSizeImage = _size;
	//	Reverse byte.
	unsigned char*		_buffer = new unsigned char[_size];
	float*		_tdata = reinterpret_cast<float*>(depthArray);
	unsigned char*		_cdata = _buffer;
	int			i;
	for (i = 0; i < _size / 3; ++i) {
		unsigned char gr = _tdata[0] * 256 < 255 ? (unsigned char)(_tdata[0] * 256) : 255;

		_cdata[0] = gr;
		_cdata[1] = gr;
		_cdata[2] = gr;
				
		_tdata += 1;
		_cdata += 3;
	}
	//	Write to file.
	_os.write((char*)&_fh, _fh_size);
	_os.write((char*)&_if, _if_size);
	_os.write(reinterpret_cast<char*>(_buffer), _size);
	_os.close();

	delete[]	_buffer;
#endif
	return;



};

void PanoramaRenderer::outputNorm(string fileName) {
	#if defined(WIN32) || defined(WIN64)
	ofstream	_os(fileName, ios::binary);
	if (!_os.is_open())
		return;
	//	Save image as bitmap.
	BITMAPFILEHEADER	_fh;
	BITMAPINFOHEADER	_if;
	int		_fh_size = sizeof(BITMAPFILEHEADER);
	int		_if_size = sizeof(BITMAPINFOHEADER);
	int		_size = viewHeight * viewHeight * 2 * 3;
	::ZeroMemory(&_fh, _fh_size);
	::ZeroMemory(&_if, _if_size);
	//	Initialize BITMAPFILEHEADER.
	_fh.bfSize = _fh_size + _if_size + _size;
	_fh.bfOffBits = _fh_size + _if_size;
	((char*)&_fh)[0] = 'B';
	((char*)&_fh)[1] = 'M';
	//	Initialize BITMAPINFOHEADER.
	_if.biSize = _if_size;
	_if.biWidth = viewHeight * 2;
	_if.biHeight = viewHeight;
	_if.biPlanes = 1;
	_if.biBitCount = 24;
	_if.biCompression = BI_RGB;
	_if.biSizeImage = _size;
	float* normArray = (float*)malloc(sizeof(float)*viewHeight*viewHeight * 2);
	for (int x = 0; x < viewHeight * 2; x++) {
		for (int y = 0; y < viewHeight; y++) {
			int idx = y * viewHeight * 2 + x;
			if (y == 0 || y == viewHeight - 1) { normArray[idx] = 0; continue; }
			int prexidx = x != 0 ? y * viewHeight * 2 + x - 1 : y * viewHeight * 2 + viewHeight * 2 - 1;
			int postxidx = x != viewHeight * 2 - 1 ? y * viewHeight * 2 + x + 1 : y * viewHeight * 2;
			int preyidx = idx - viewHeight * 2;
			int postyidx = idx + viewHeight * 2;
			if (depthArray[idx] == 1.0 || depthArray[prexidx] == 1.0 || depthArray[postxidx] == 1.0 || depthArray[preyidx] == 1.0 || depthArray[postyidx] == 1.0) {
				normArray[idx] = 0;
				continue;
			}
			Vector3d n;
			double c = depthArray[idx] * M_PI * 4 / (viewHeight * 2);
			n << (depthArray[postxidx] - depthArray[prexidx]) / c, (depthArray[postyidx] - depthArray[preyidx]) / c, 1;
			n.normalize();

			normArray[idx] = n(2, 0);
		}
	}



	//	Reverse byte.
	unsigned char*		_buffer = new unsigned char[_size];
	float*		_tdata = reinterpret_cast<float*>(normArray);
	unsigned char*		_cdata = _buffer;
	int			i;
	for (i = 0; i < _size / 3; ++i) {



		unsigned char gr = _tdata[0] * 255 < 255 ? (unsigned char)(_tdata[0] * 255) : 255;

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

	delete[]	_buffer;
#endif
	return;



};

void display(float*& vertex, unsigned int*& face, float*& reflectance, int meshNum, int vertNum)
{
	GLint view[4];
	GLdouble ox, oy, oz;
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	glGetIntegerv(GL_VIEWPORT,view);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < meshNum; i++) {
		int index1 = face[i * 3];
		int index2 = face[i * 3 + 1];
		int index3 = face[i * 3 + 2];
		if (index1 >= vertNum) {
			continue;
		}if (index2 >= vertNum) {
			continue;
		}if (index3 >= vertNum) {
			continue;
		}
	//	if vertex[index1 * 3 + ] - vertex[index2 * 3 + 1]);
		double thres = 1.0;
		if (vertex[index1 * 3 + 2] < thres || vertex[index2 * 3 + 2] < thres || vertex[index3 * 3 + 2] < thres)continue;//�ߋ����̓����_�����O���Ȃ��i���悻ladybug�̔��a���j
		//if (vertex[index1 * 3 + 1] < PI_VAL / 179 || vertex[index2 * 3 + 1] < PI_VAL / 179 || vertex[index3 * 3 + 1] < PI_VAL / 179)continue;
		//if (vertex[index1 * 3 + 1] > PI_VAL - PI_VAL / 179 || vertex[index2 * 3 + 1] > PI_VAL - PI_VAL / 179 || vertex[index3 * 3 + 1] > PI_VAL - PI_VAL / 179)continue;
		GLfloat gr;
		//�p�m���}�[�_�̏���
		if (vertex[index1 * 3] * vertex[index2 * 3] < 0 && vertex[index1 * 3] * vertex[index3 * 3]>0 && abs(vertex[index1 * 3]) > PI_VAL / 2) {
			double val = vertex[index2 * 3] < 0 ? PI_VAL * 2 : -PI_VAL * 2;
			gr = reflectance[index1];
			glColor3f(gr, gr, gr);
			glVertex3f(vertex[index1 * 3], vertex[index1 * 3 + 1], vertex[index1 * 3 + 2]);
			gr = reflectance[index2];
			glColor3f(gr, gr, gr);
			glVertex3f(vertex[index2 * 3] + val, vertex[index2 * 3 + 1], vertex[index2 * 3 + 2]);
			gr = reflectance[index3];
			glColor3f(gr, gr, gr);
			glVertex3f(vertex[index3 * 3], vertex[index3 * 3 + 1], vertex[index3 * 3 + 2]);
			gr = reflectance[index1];
			glColor3f(gr, gr, gr);
			glVertex3f(vertex[index1 * 3] - val, vertex[index1 * 3 + 1], vertex[index1 * 3 + 2]);
			gr = reflectance[index2];
			glColor3f(gr, gr, gr);
			glVertex3f(vertex[index2 * 3], vertex[index2 * 3 + 1], vertex[index2 * 3 + 2]);
			gr = reflectance[index3];
			glColor3f(gr, gr, gr);
			glVertex3f(vertex[index3 * 3] - val, vertex[index3 * 3 + 1], vertex[index3 * 3 + 2]);
		}
		else if (vertex[index1 * 3] * vertex[index3 * 3] < 0 && vertex[index1 * 3] * vertex[index2 * 3]>0 && abs(vertex[index1 * 3]) > PI_VAL / 2) {
			double val = vertex[index3 * 3] < 0 ? PI_VAL * 2 : -PI_VAL * 2;
			gr = reflectance[index1];
			glColor3f(gr, gr, gr);
			glVertex3f(vertex[index1 * 3], vertex[index1 * 3 + 1], vertex[index1 * 3 + 2]);
			gr = reflectance[index2];
			glColor3f(gr, gr, gr);
			glVertex3f(vertex[index2 * 3], vertex[index2 * 3 + 1], vertex[index2 * 3 + 2]);
			gr = reflectance[index3];
			glColor3f(gr, gr, gr);
			glVertex3f(vertex[index3 * 3] + val, vertex[index3 * 3 + 1], vertex[index3 * 3 + 2]);
			gr = reflectance[index1];
			glColor3f(gr, gr, gr);
			glVertex3f(vertex[index1 * 3] - val, vertex[index1 * 3 + 1], vertex[index1 * 3 + 2]);
			gr = reflectance[index2];
			glColor3f(gr, gr, gr);
			glVertex3f(vertex[index2 * 3] - val, vertex[index2 * 3 + 1], vertex[index2 * 3 + 2]);
			gr = reflectance[index3];
			glColor3f(gr, gr, gr);
			glVertex3f(vertex[index3 * 3], vertex[index3 * 3 + 1], vertex[index3 * 3 + 2]);
		}
		else if (vertex[index1 * 3] * vertex[index3 * 3] < 0 && vertex[index2 * 3] * vertex[index3 * 3]>0 && abs(vertex[index1 * 3]) > PI_VAL / 2) {
			double val = vertex[index1 * 3] < 0 ? PI_VAL * 2 : -PI_VAL * 2;
			gr = reflectance[index1];
			glColor3f(gr, gr, gr);
			glVertex3f(vertex[index1 * 3] + val, vertex[index1 * 3 + 1], vertex[index1 * 3 + 2]);
			gr = reflectance[index2];
			glColor3f(gr, gr, gr);
			glVertex3f(vertex[index2 * 3], vertex[index2 * 3 + 1], vertex[index2 * 3 + 2]);
			gr = reflectance[index3];
			glColor3f(gr, gr, gr);
			glVertex3f(vertex[index3 * 3], vertex[index3 * 3 + 1], vertex[index3 * 3 + 2]);
			gr = reflectance[index1];
			glColor3f(gr, gr, gr);
			glVertex3f(vertex[index1 * 3], vertex[index1 * 3 + 1], vertex[index1 * 3 + 2]);
			gr = reflectance[index2];
			glColor3f(gr, gr, gr);
			glVertex3f(vertex[index2 * 3] - val, vertex[index2 * 3 + 1], vertex[index2 * 3 + 2]);
			gr = reflectance[index3];
			glColor3f(gr, gr, gr);
			glVertex3f(vertex[index3 * 3] - val, vertex[index3 * 3 + 1], vertex[index3 * 3 + 2]);
		}
		else {
			gr = reflectance[index1];
			glColor3f(gr, gr, gr);
			glVertex3f(vertex[index1 * 3], vertex[index1 * 3 + 1], vertex[index1 * 3 + 2]);
			gr = reflectance[index2];
			glColor3f(gr, gr, gr);
			glVertex3f(vertex[index2 * 3], vertex[index2 * 3 + 1], vertex[index2 * 3 + 2]);
			gr = reflectance[index3];
			glColor3f(gr, gr, gr);
			glVertex3f(vertex[index3 * 3], vertex[index3 * 3 + 1], vertex[index3 * 3 + 2]);
		}



	}
	glEnd();
	//	glFlush();
	cout << "check" << endl;

}

void displayrgb(float*& vertex, unsigned int*& face, unsigned char*& rgba, int meshNum, int vertNum)
{
	GLint view[4];
	GLdouble ox, oy, oz;
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < meshNum; i++) {

		int index1 = face[i * 3];
		int index2 = face[i * 3 + 1];
		int index3 = face[i * 3 + 2];
		if (index1 > vertNum) {
			continue;
		}if (index2 > vertNum) {
			continue;
		}if (index3 > vertNum) {
			continue;
		}



		double thres = 0.4;
		if (vertex[index1 * 3 + 2] < thres || vertex[index2 * 3 + 2] < thres || vertex[index3 * 3 + 2] < thres)continue;//�ߋ����̓����_�����O���Ȃ��i���悻ladybug�̔��a���j
		//if (vertex[index1 * 3 + 1] < PI_VAL / 359 || vertex[index2 * 3 + 1] < PI_VAL / 359 || vertex[index3 * 3 + 1] < PI_VAL / 359)continue;
		//if (vertex[index1 * 3 + 1] > PI_VAL - PI_VAL / 359 || vertex[index2 * 3 + 1] > PI_VAL - PI_VAL / 359 || vertex[index3 * 3 + 1] > PI_VAL - PI_VAL / 359)continue;
		//if (vertex[index1 * 3 + 1] == vertex[index1 * 3 + 2]||||)continue;
		GLfloat gr;
		//�p�m���}�[�_�̏���
		if (vertex[index1 * 3] * vertex[index2 * 3] < 0 && vertex[index1 * 3] * vertex[index3 * 3]>0 && fabs(vertex[index1 * 3]) > PI_VAL / 2) {
			double val = vertex[index2 * 3] < 0 ? PI_VAL * 2 : -PI_VAL * 2;
			//gr=reflectance[index1];

			glColor3ub(rgba[index1 * 4], rgba[index1 * 4 + 1], rgba[index1 * 4 + 2]);
			glVertex3f(vertex[index1 * 3], vertex[index1 * 3 + 1], vertex[index1 * 3 + 2]);
			//gr=reflectance[index2];
			glColor3ub(rgba[index2 * 4], rgba[index2 * 4 + 1], rgba[index2 * 4 + 2]);
			glVertex3f(vertex[index2 * 3] + val, vertex[index2 * 3 + 1], vertex[index2 * 3 + 2]);
			//gr=reflectance[index3];
			glColor3ub(rgba[index3 * 4], rgba[index3 * 4 + 1], rgba[index3 * 4 + 2]);
			glVertex3f(vertex[index3 * 3], vertex[index3 * 3 + 1], vertex[index3 * 3 + 2]);
			//gr=reflectance[index1];
			glColor3ub(rgba[index1 * 4], rgba[index1 * 4 + 1], rgba[index1 * 4 + 2]);
			glVertex3f(vertex[index1 * 3] - val, vertex[index1 * 3 + 1], vertex[index1 * 3 + 2]);
			//gr=reflectance[index2];
			glColor3ub(rgba[index2 * 4], rgba[index2 * 4 + 1], rgba[index2 * 4 + 2]);
			glVertex3f(vertex[index2 * 3], vertex[index2 * 3 + 1], vertex[index2 * 3 + 2]);
			//gr=reflectance[index3];
			glColor3ub(rgba[index3 * 4], rgba[index3 * 4 + 1], rgba[index3 * 4 + 2]);
			glVertex3f(vertex[index3 * 3] - val, vertex[index3 * 3 + 1], vertex[index3 * 3 + 2]);
			if (fabs(vertex[index1 * 3+1] - vertex[index2 * 3+1]) > PI_VAL / 2) {
				cout << vertex[index1 * 3] << endl;
				cout << vertex[index2 * 3] << endl;
				cout << vertex[index3 * 3] << endl;
				cout << vertex[index1 * 3 + 1] << endl;
				cout << vertex[index2 * 3 + 1] << endl;
				cout << vertex[index3 * 3 + 1] << endl;
			};


		}
		else if (vertex[index1 * 3] * vertex[index3 * 3] < 0 && vertex[index1 * 3] * vertex[index2 * 3]>0 && fabs(vertex[index1 * 3]) > PI_VAL / 2) {
			double val = vertex[index3 * 3] < 0 ? PI_VAL * 2 : -PI_VAL * 2;
			//gr=reflectance[index1];

			glColor3ub(rgba[index1 * 4], rgba[index1 * 4 + 1], rgba[index1 * 4 + 2]);
			glVertex3f(vertex[index1 * 3], vertex[index1 * 3 + 1], vertex[index1 * 3 + 2]);
			//gr=reflectance[index2];
			glColor3ub(rgba[index2 * 4], rgba[index2 * 4 + 1], rgba[index2 * 4 + 2]);
			glVertex3f(vertex[index2 * 3], vertex[index2 * 3 + 1], vertex[index2 * 3 + 2]);
			//gr=reflectance[index3];
			glColor3ub(rgba[index3 * 4], rgba[index3 * 4 + 1], rgba[index3 * 4 + 2]);
			glVertex3f(vertex[index3 * 3] + val, vertex[index3 * 3 + 1], vertex[index3 * 3 + 2]);
			//gr=reflectance[index1];
			glColor3ub(rgba[index1 * 4], rgba[index1 * 4 + 1], rgba[index1 * 4 + 2]);
			glVertex3f(vertex[index1 * 3] - val, vertex[index1 * 3 + 1], vertex[index1 * 3 + 2]);
			//gr=reflectance[index2];
			glColor3ub(rgba[index2 * 4], rgba[index2 * 4 + 1], rgba[index2 * 4 + 2]);
			glVertex3f(vertex[index2 * 3] - val, vertex[index2 * 3 + 1], vertex[index2 * 3 + 2]);
			//gr=reflectance[index3];
			glColor3ub(rgba[index3 * 4], rgba[index3 * 4 + 1], rgba[index3 * 4 + 2]);
			glVertex3f(vertex[index3 * 3], vertex[index3 * 3 + 1], vertex[index3 * 3 + 2]);
		}
		else if (vertex[index1 * 3] * vertex[index3 * 3] < 0 && vertex[index2 * 3] * vertex[index3 * 3]>0 && fabs(vertex[index1 * 3]) > PI_VAL / 2) {
			double val = vertex[index1 * 3] < 0 ? PI_VAL * 2 : -PI_VAL * 2;
			//gr=reflectance[index1];
			glColor3ub(rgba[index1 * 4], rgba[index1 * 4 + 1], rgba[index1 * 4 + 2]);
			glVertex3f(vertex[index1 * 3] + val, vertex[index1 * 3 + 1], vertex[index1 * 3 + 2]);
			//gr=reflectance[index2];
			glColor3ub(rgba[index2 * 4], rgba[index2 * 4 + 1], rgba[index2 * 4 + 2]);
			glVertex3f(vertex[index2 * 3], vertex[index2 * 3 + 1], vertex[index2 * 3 + 2]);
			//gr=reflectance[index3];
			glColor3ub(rgba[index3 * 4], rgba[index3 * 4 + 1], rgba[index3 * 4 + 2]);
			glVertex3f(vertex[index3 * 3], vertex[index3 * 3 + 1], vertex[index3 * 3 + 2]);
			//gr=reflectance[index1];
			glColor3ub(rgba[index1 * 4], rgba[index1 * 4 + 1], rgba[index1 * 4 + 2]);
			glVertex3f(vertex[index1 * 3], vertex[index1 * 3 + 1], vertex[index1 * 3 + 2]);
			//gr=reflectance[index2];
			glColor3ub(rgba[index2 * 4], rgba[index2 * 4 + 1], rgba[index2 * 4 + 2]);
			glVertex3f(vertex[index2 * 3] - val, vertex[index2 * 3 + 1], vertex[index2 * 3 + 2]);
			//gr=reflectance[index3];
			glColor3ub(rgba[index3 * 4], rgba[index3 * 4 + 1], rgba[index3 * 4 + 2]);
			glVertex3f(vertex[index3 * 3] - val, vertex[index3 * 3 + 1], vertex[index3 * 3 + 2]);
		}
		else {
			//gr=reflectance[index1];
			glColor3ub(rgba[index1 * 4], rgba[index1 * 4 + 1], rgba[index1 * 4 + 2]);
			glVertex3f(vertex[index1 * 3], vertex[index1 * 3 + 1], vertex[index1 * 3 + 2]);
			//gr=reflectance[index2];
			glColor3ub(rgba[index2 * 4], rgba[index2 * 4 + 1], rgba[index2 * 4 + 2]);
			glVertex3f(vertex[index2 * 3], vertex[index2 * 3 + 1], vertex[index2 * 3 + 2]);
			//gr=reflectance[index3];
			glColor3ub(rgba[index3 * 4], rgba[index3 * 4 + 1], rgba[index3 * 4 + 2]);
			glVertex3f(vertex[index3 * 3], vertex[index3 * 3 + 1], vertex[index3 * 3 + 2]);
		}
	}
	glEnd();


	cout << "check" << endl;

}

void display_points(float*& vertex, unsigned int*& face, float*& reflectance, int meshNum, int vertNum)
{
	GLint view[4];
	GLdouble ox, oy, oz;
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	glGetIntegerv(GL_VIEWPORT,view);
	glBegin(GL_POINTS);
	glPointSize(1);
	for (int i = 0; i < vertNum; i++) {
		float gr = reflectance[i];
		glColor3f(gr, gr, gr);
		glVertex3f(vertex[i * 3], vertex[i * 3 + 1], vertex[i * 3 + 2]);
	}
	glEnd();
	//	glFlush();
	cout << "check" << endl;

}

//���E���W�n�ɂ�����R,t�����ړ������J�����ɂ�����_p�̕ϊ�����
//R*(pt-t)

void sphericalTrans_renderer(Vector3d& ret_, Vector3d& pt, Vector3d& center, Matrix3d& rotMatrix) {
	Vector3d tp = rotMatrix * (pt - center);
	float r = sqrt(tp.dot(tp));
	float phi = acos(tp(2, 0) / r);//
	float theta = atan2(tp(1, 0), tp(0, 0));
	ret_ << -theta, phi, r;
}

void sphericalTrans_renderer(Vector3d& ret_, Vector3d& pt, Matrix4d& cameraParameter) {
	Vector4d pt_;
	pt_ << pt(0), pt(1), pt(2), 1;
	Vector4d tp_ = cameraParameter * pt_;
	Vector3d tp = tp_.block(0, 0, 3, 1);
	float r = sqrt(tp.dot(tp));
	float phi = acos(tp(2, 0) / r);//
	if (isnan(phi)) {
		if(tp(2, 0)>0)phi = 0;
		else phi = PI_VAL;
	}
	float theta = atan2(tp(1, 0), tp(0, 0));
	ret_ << -theta, phi, r;
}

#define FOCAL M_PI/2
void fisheyeTrans_renderer(Vector3d& ret_, Vector3d& pt, Matrix4d& cameraParameter) {
	
	Vector4d pt_;
	pt_ << pt(0), pt(1), pt(2), 1;
	Vector4d tp_ = cameraParameter * pt_;
	Vector3d tp = tp_.block(0, 0, 3, 1);
	float r = sqrt(tp.dot(tp));
	float phi = acos(tp(2, 0) / r);//
	if (isnan(phi)) {
		if (tp(2, 0) > 0)phi = 0;
		else phi = PI_VAL;
	}
	float x0 = tp(0);
	float y0 = tp(1);
	float r0 = sqrt(x0*x0 + y0 * y0);
	float x0_ = x0 / r0 * phi;
	float y0_ = y0 / r0 * phi;
	if (r0 == 0) {
		x0_ = 0;
		y0_ = 0;
	}
	ret_ <<x0_ ,-y0_  , r;
}

void Init(int viewWidth,int viewHeight, double depthResolution) {
	glViewport(0, 0, viewWidth, viewHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	//	  gluPerspective(90.0, (double)300/(double)300, 0.1, 100.0); //�������e�@�̎��̐�gluPerspactive(th, w/h, near, far);
	glOrtho(-PI_VAL, PI_VAL, -PI_VAL, 0, 0, depthResolution);
	//------------------------------------------------
	gluLookAt(
		0.0, 0.0, 0.0, // ���_�̈ʒux,y,z;
		0.0, 0.0, 1.0,   // ���E�̒��S�ʒu�̎Q�Ɠ_���Wx,y,z
		0.0, -1.0, 0.0);  //���E�̏�����̃x�N�g��x,y,z*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void InitFE(int viewWidth, int viewHeight, double depthResolution) {
	glViewport(0, 0, viewWidth, viewHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	//	  gluPerspective(90.0, (double)300/(double)300, 0.1, 100.0); //�������e�@�̎��̐�gluPerspactive(th, w/h, near, far);
	glOrtho(-PI_VAL/2, PI_VAL/2, -PI_VAL/2, PI_VAL / 2, 0.03, depthResolution);
	//------------------------------------------------
	gluLookAt(
		0.0, 0.0, 0.0, // ���_�̈ʒux,y,z;
		0.0, 0.0, 1.0,   // ���E�̒��S�ʒu�̎Q�Ɠ_���Wx,y,z
		0.0, -1.0, 0.0);  //���E�̏�����̃x�N�g��x,y,z*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    glFlush();
}


void InitPers(int viewWidth, int viewHeight,double znear ,double depthResolution, double* intrinsic) {

	glViewport(0, 0, viewWidth, viewHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);

	GLfloat m[16];

	Matrix4d m1_, r2l, rev;//projection
	double cx = intrinsic[0];
	double cy = intrinsic[1];
	double fx = intrinsic[2];
	double fy = intrinsic[3];
	double zfar = znear+depthResolution;
	//m1_ <<
	//	2 * fx / viewWidth, 0, (viewWidth - 2 * cx) / viewWidth, 0,
	//	0, -2 * fy / viewHeight, (viewHeight - 2 * cy) / viewHeight, 0,
	//	0, 0, -(zfar + znear) / (zfar - znear), - 2 * zfar*znear / (zfar - znear),
	//	0, 0, -1, 0;

	//m1_ <<
	//	2 * fx / viewWidth, 0, (viewWidth - 2 * cx) / viewWidth, 0,
	//	0, -2 * fy / viewHeight, (viewHeight - 2 * cy) / viewHeight, 0,
	//	0, 0, (zfar + znear) / (zfar - znear), 2 * zfar*znear / (zfar - znear),
	//	0, 0, -1, 0;

		//m1_ <<
		//2 * fx / viewWidth, 0, -(viewWidth - 2 * cx) / viewWidth, 0,
		//0, 2 * fy / viewHeight, -(viewHeight - 2 * cy) / viewHeight, 0,
		//0, 0, -(zfar + znear) / (zfar - znear), 2 * zfar*znear / (zfar - znear),
		//0, 0, 1, 0;

		m1_ <<
			2 * fx / viewWidth, 0, -(viewWidth - 2 * cx) / viewWidth, 0,
			0, 2 * fy / viewHeight, -(viewHeight - 2 * cy) / viewHeight, 0,
			0, 0, (zfar + znear) / (zfar - znear), -2 * zfar*znear / (zfar - znear),
			0, 0, 1, 0;

	Matrix4d m3 = m1_;

	GLdouble m2[16];
	memcpy(m2, m3.data(), sizeof(double) * 16);

	glMultMatrixd(m2);
	//gluPerspective(90.0, viewWidth/(double)viewHeight, 0.1, 100.0);
	//gluLookAt(
	//	0.0, 0.0, 0.0,
	//	1.0, 0.0, 0.0,
	//	0.0, 1.0, 0.0);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PanoramaRenderer::createContext(int view_w,int view_h) {
#if defined(WIN32) || defined(WIN64)

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
	HDC		_hdc_ = CreateCompatibleDC(NULL);
	viewWidth_ = viewWidth_stat = view_w;
	viewHeight_ = viewHeight_stat = view_h;



	DWORD m_DIBWidth = viewWidth_stat;
	DWORD m_DIBHeight = viewHeight_stat;
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
	if ((m_DIBWidth * 3) % 4 == 0) /* �o�b�t�@�̂P���C���̒������v�Z */
		dwLength = m_DIBWidth * 3;
	else
		dwLength = m_DIBWidth * 3 + (4 - (m_DIBWidth * 3) % 4);

	int		_pfid = ChoosePixelFormat(_hdc_, &_pfd);
	::SetPixelFormat(_hdc_, _pfid, &_pfd);
	HGLRC	_hrc = ::wglCreateContext(_hdc_);
	::wglMakeCurrent(_hdc_, _hrc);
#elif defined(__unix__)
	 glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc) glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );
    glXMakeContextCurrentARB   = (glXMakeContextCurrentARBProc)   glXGetProcAddressARB( (const GLubyte *) "glXMakeContextCurrent");

    display_ = XOpenDisplay(NULL);
    if (display_ == NULL){
        std::cout  << "error getting the X display";
    }

    static int visualAttribs[] = {
		GLX_X_RENDERABLE    , True,
		    GLX_RENDER_TYPE,    GLX_RGBA_BIT,
		    GLX_DRAWABLE_TYPE,  GLX_WINDOW_BIT,
    GLX_RED_SIZE,       8,
    GLX_GREEN_SIZE,     8,
    GLX_BLUE_SIZE,      8,
    GLX_ALPHA_SIZE,     8,
    GLX_DEPTH_SIZE,     24,
    GLX_STENCIL_SIZE,   8,
			GLX_DOUBLEBUFFER    , True,
		None};
    int numberOfFrameBufferConfigurations;
    GLXFBConfig *fbConfigs = glXChooseFBConfig(display_, DefaultScreen(display_), visualAttribs, &numberOfFrameBufferConfigurations);

    int context_attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB ,2,
        GLX_CONTEXT_MINOR_VERSION_ARB, 0,
   //     GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB,
   //     GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };

    std::cout << "initialising context...";
    this->openGLContext = glXCreateContextAttribsARB(display_, fbConfigs[0], 0, True, context_attribs);


	viewWidth_ = viewWidth_stat = view_w;
	viewHeight_ = viewHeight_stat = view_h;
    int pBufferAttribs[] = {
        GLX_PBUFFER_WIDTH, view_w,
        GLX_PBUFFER_HEIGHT, view_h,
        None
    };

    this->pbuffer = glXCreatePbuffer(display_, fbConfigs[0], pBufferAttribs);
    XFree(fbConfigs);
    XSync(display_, False);

	if(!glXMakeContextCurrent(display_, pbuffer, pbuffer, openGLContext)){
		std::cout << "error with content creation\n";
	}else{
		std::cout << "made a context the current context\n";
	}
	#endif
}

void PanoramaRenderer::createContext() {
	#if defined(WIN32) || defined(WIN64)
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
	_hdc_ = CreateCompatibleDC(NULL);

	DWORD m_DIBWidth = viewWidth_;
	DWORD m_DIBHeight = viewHeight_;
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
	m_hbitmap = ::CreateDIBSection(
		_hdc_,
		(BITMAPINFO*)m_PBIH, DIB_RGB_COLORS,
		&m_PBits, NULL, 0
	);

	m_hbitmap_old = (HBITMAP)::SelectObject(_hdc_, m_hbitmap);
	DWORD dwLength;
	if ((m_DIBWidth * 3) % 4 == 0) /* �o�b�t�@�̂P���C���̒������v�Z */
		dwLength = m_DIBWidth * 3;
	else
		dwLength = m_DIBWidth * 3 + (4 - (m_DIBWidth * 3) % 4);

	int		_pfid = ChoosePixelFormat(_hdc_, &_pfd);
	::SetPixelFormat(_hdc_, _pfid, &_pfd);
	_hrc = ::wglCreateContext(_hdc_);
	::wglMakeCurrent(_hdc_, _hrc);
	#elif (__unix__)

	 glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc) glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );
    glXMakeContextCurrentARB   = (glXMakeContextCurrentARBProc)   glXGetProcAddressARB( (const GLubyte *) "glXMakeContextCurrent");

    display_ = XOpenDisplay(NULL);
    if (display_ == NULL){
        std::cout  << "error getting the X display";
    }

    static int visualAttribs[] = {
		    GLX_RENDER_TYPE,    GLX_RGBA_BIT,
		    GLX_DRAWABLE_TYPE,  GLX_WINDOW_BIT,
			    GLX_DOUBLEBUFFER,   True,
    GLX_RED_SIZE,       8,
    GLX_GREEN_SIZE,     8,
    GLX_BLUE_SIZE,      8,
    GLX_ALPHA_SIZE,     8,
    GLX_DEPTH_SIZE,     24,
    GLX_STENCIL_SIZE,   8,
		None};
    int numberOfFrameBufferConfigurations;
    GLXFBConfig *fbConfigs = glXChooseFBConfig(display_, DefaultScreen(display_), visualAttribs, &numberOfFrameBufferConfigurations);

    int context_attribs[] = {
//        GLX_CONTEXT_MAJOR_VERSION_ARB ,3,
 //       GLX_CONTEXT_MINOR_VERSION_ARB, 2,
//		GLX_RENDER_TYPE, GLX_RGBA_BIT,
//		GLX_DRAWABLE_TYPE,GLX_PBUFFER_BIT,
    	GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };

    std::cout << "initialising context...";
    this->openGLContext = glXCreateContextAttribsARB(display_, fbConfigs[0], 0, True, context_attribs);


//	viewWidth_ = viewWidth_stat = view_w;
//	viewHeight_ = viewHeight_stat = view_h;
    int pBufferAttribs[] = {
        GLX_PBUFFER_WIDTH, viewWidth_,
        GLX_PBUFFER_HEIGHT, viewHeight_,
        None
    };

    this->pbuffer = glXCreatePbuffer(display_, fbConfigs[0], pBufferAttribs);
    XFree(fbConfigs);
    XSync(display_, False);

	if(!glXMakeContextCurrent(display_, pbuffer, pbuffer, openGLContext)){
		std::cout << "error with content creation\n";
	}else{
		std::cout << "made a context the current context\n";
	}
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
      std::cerr << glewGetErrorString(err) << std::endl;
      throw std::runtime_error("Failed to initialize GLEW.");
    }

    std::cout << "GLEW initialized." << std::endl;


	#endif
}

void  PanoramaRenderer::discardContext() {
#if defined(WIN32) || defined(WIN64)
	if (FALSE == ::wglMakeCurrent(0, 0))exit(ERROR);
	if (FALSE == ::wglDeleteContext(_hrc))exit(ERROR);
	DeleteDC(_hdc_);
	DeleteObject(m_hbitmap);
#elif (__unix__)
   


#endif
}

int PanoramaRenderer::viewWidth_stat;
int PanoramaRenderer::viewHeight_stat;
