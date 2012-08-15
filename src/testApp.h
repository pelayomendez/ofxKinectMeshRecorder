#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxKinectMeshRecorder.h"
// uncomment this to read from two kinects simultaneously
//#define USE_TWO_KINECTS

class testApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
	
	void drawPointCloud();
	
    
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
    
	
	ofxKinect kinect;
	
#ifdef USE_TWO_KINECTS
	ofxKinect kinect2;
#endif
	
	ofxCvColorImage colorImg;
	
	ofxCvGrayscaleImage grayImage; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
	
	ofxCvContourFinder contourFinder;
	
	bool bThreshWithOpenCV;
	bool bDrawPointCloud;
	
	int nearThreshold;
	int farThreshold;
	
	int angle;
	
	// used for viewing the point cloud
	ofEasyCam easyCam;

    // Estructura de datos:
    
    typedef struct {
        float x;
		float y;
		float z;
		float w;
    } pointData;
	
	typedef struct {
        int x;
		int y;
		int z;
    } colorData;
    
    // Guardar Datos:
    
    
    
    ofDirectory dirHelper;
    string generateFileName();
    int frame;
    string saveTo;
    void savePointCloud();
    void drawRecordedPointCloud();
   
    int distanciaMinima;
    int distanciaMaxima;
    
    //
    
    
     //////////////////////////////////////////////////////
    // Configuracion de la APP/GUI
    //////////////////////////////////////////////////////
    
    ofTrueTypeFont myFont;

    //////////////////////////////////////////////////////
    // Configuracion de la APP/GUI
    //////////////////////////////////////////////////////
    
    bool recording;
    bool playing;
    bool colorMode;
    
    //////////////////////////////////////////////////////
    // Reproduccion del PointCloud
    //////////////////////////////////////////////////////
    
    int frameToPlay;
    ofxKinectMeshRecorder meshRecorder;
    
};
