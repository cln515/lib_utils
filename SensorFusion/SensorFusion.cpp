#include "SensorFusion\SensorFusion.h"

SensorFusion::Inputs SensorFusion::setFolder(std::string workFolder) {
	return setFolder(workFolder, workFolder + "\\ext.cpara");
}


SensorFusion::Inputs SensorFusion::setFolder(std::string workFolder,std::string extCparaFile) {
	inputs.imgFileNameList.clear();
	
	std::string imageListBase = workFolder + "\\images\\";
	std::string pointFileBase = workFolder + "\\points\\binary";
	//	cout << "short" << endl;

	std::string calibFile = extCparaFile;

	//	cout << "short" << endl;
	std::string pointFileFirst = workFolder + "\\points\\binary.dat";
	//folder search
	{
		std::string imageListBase_ = workFolder + "\\images";
		DWORD ftyp = GetFileAttributesA(imageListBase_.c_str());
	
		bool fexist;
		fexist = false;    // this is not a directory!
		if (ftyp == INVALID_FILE_ATTRIBUTES) {
			fexist = false;  //something is wrong with your path!
		}else if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
			fexist = true;   // this is a directory!
		if (!fexist) {
			std::string imageListBase_shortcut = workFolder + "\\images.txt";
			ifstream ifs(imageListBase_shortcut);
			getline(ifs, imageListBase);			
			imageListBase = imageListBase + "\\";
			//getShortcut(imageListBase_shortcut);
			//imageListBase = getShortcut(imageListBase_shortcut) + "\\";

		}
		//cout << "short" << endl;
	}
	imageBase = imageListBase;
//	cout << "short" << endl;
	std::string imageList = imageListBase+ "img.lst";
	std::string imageTimeStamp = imageListBase + "timeStamp.dat";
//	cout << "short" << endl;

	if (!PathFileExistsA(imageList.c_str())) {
		std::string errorLog = imageList + " cannot be found!\n";
		cout << errorLog << endl;
		return inputs;
	}
	if (!PathFileExistsA(pointFileFirst.c_str())) {
		std::string errorLog = pointFileFirst + " cannot be found!\n";
		cout << errorLog << endl;
		return inputs;
	}
	if (!PathFileExistsA(calibFile.c_str())) {
		std::string errorLog = calibFile + " cannot be found!\n";
		cout << errorLog << endl;
		return inputs;
	}
	if (!PathFileExistsA(imageTimeStamp.c_str())) {
		std::string errorLog = imageTimeStamp + " cannot be found!\n";
		cout << errorLog << endl;
		return inputs;
	}

	//load image file name list

	std::ifstream ifs(imageList);
	std::string str;
	getline(ifs, str);

	int imageNumber = 0;
	imageNumber = stoi(str);
	while (getline(ifs, str)) {
		inputs.imgFileNameList.push_back(imageListBase+str);
	}
	ifs.close();

	//load time stamp (image)
	ifs.open(imageTimeStamp, std::ios::binary);
	inputs.imTimeStamp = (double*)malloc(imageNumber * sizeof(double));
	ifs.read((char*)inputs.imTimeStamp, imageNumber * sizeof(double));
	ifs.close();

	
	inputs.extCalib = readCPara(calibFile).inverse();
	/*if (config.profiler) {
		Matrix4d profRot;
		double horizAngle = config.horizAngle / 180 * M_PI;
		profRot << cos(horizAngle), sin(horizAngle), 0, 0,
			-sin(horizAngle), cos(horizAngle), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1;
		extCalib = extCalib*profRot;
	}*/
	/*console->PostLog("Testing Calib Shift...\n");
	Matrix4d shift;
	shift<<0,0,0,0,
	0,0,0,-0.03,
	0,0,0,0,
	0,0,0,0;
	extCalib=extCalib+shift;*/
	inputs.psl = new PointSequenceLoader();
	inputs.psl->loadPointStream(pointFileBase);

	return inputs;
};
SensorFusion::Status SensorFusion::initialize(Status stat_) {
	stat = stat_;
	if (initializeFunc != NULL) {
		initializeFunc(stat, inputs);
	}
	return stat;
};
SensorFusion::Status SensorFusion::update() {
	//current status
	//	image
	//	2d-3d correspondence
	//	idx

	//new input
	//	next image
	//	next point

	if (updateFunc != NULL) {
		updateFunc(stat,inputs);
	}
	return stat;
};

bool SensorFusion::setMask(string workMaskPath) {
	stat.mask = cv::imread(workMaskPath, 0);
	if (stat.mask.cols > 0) {
		stat.bMask = true;
		cv::resize(stat.mask, stat.mask, cv::Size(2048, 1024));
	}
	else stat.bMask = false;
	return stat.bMask;
};