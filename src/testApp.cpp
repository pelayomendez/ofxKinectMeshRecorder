#include "testApp.h"

string _timestamp = "default";
int step = 4;

//--------------------------------------------------------------
void testApp::setup() {
	
    ofSetLogLevel(OF_LOG_VERBOSE);
	
	// enable depth->video image calibration
	kinect.setRegistration(true);
    
	kinect.init();
	//kinect.init(true); // shows infrared instead of RGB video image
	//kinect.init(false, false); // disable video image (faster fps)
	kinect.open();
	
#ifdef USE_TWO_KINECTS
	kinect2.init();
	kinect2.open();
#endif
	
	colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	
	nearThreshold = 255;
	farThreshold = 0;
	bThreshWithOpenCV = true;
	
	ofSetFrameRate(30);
	
	// zero the tilt on startup
	angle = 0;
	kinect.setCameraTiltAngle(angle);
	
	// start from the front
	bDrawPointCloud = true;
    
    //offsets to center the particle son screen
    int xOffset = (ofGetWidth() - 640 ) /2 ; 
    int yOffset = (ofGetHeight() - 480 ) /2 ;
    
    //////////////////////////////////////////////////////
    // Recording / Playing:
    //////////////////////////////////////////////////////
    
    recording = false;
    playing = false;
    colorMode = true;
    
    frame = 0;
    distanciaMinima = 200;
    distanciaMaxima = 1500;
    
    //////////////////////////////////////////////////////
    // Configuracion de la APP/GUI
    //////////////////////////////////////////////////////
    
    myFont.loadFont("fonts/profaisal-elitetahreerv1-0/ProfaisalEliteTahreer.ttf",9);
    
}

//--------------------------------------------------------------
void testApp::update() {
    
    //////////////////////////////////////////////////////
    // General
    //////////////////////////////////////////////////////
    
    ofBackground(0, 0, 0);
    kinect.update();
    
    distanciaMinima = farThreshold*15;
    distanciaMaxima = nearThreshold*15;
    
    //////////////////////////////////////////////////////
    // Grabando
    //////////////////////////////////////////////////////
    
    // Guardamos la info del Mesh:
    
    if(recording) {
        savePointCloud();
    }
    
    //////////////////////////////////////////////////////
    // Reproduciendo
    ////////////////////////////////////////////////////// 
    
    if(playing) {
        frameToPlay += 1;
        if(frameToPlay >= meshRecorder.TotalFrames) frameToPlay = 0;
    }
    
    //////////////////////////////////////////////////////
    // Tracking / Playing
    ////////////////////////////////////////////////////// 
    
	// there is a new frame and we are connected
	if(kinect.isFrameNew()) {
		// load grayscale depth image from the kinect source
        if (!playing) 
            grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        
        // we do two thresholds - one for the far plane and one for the near plane
		// we then do a cvAnd to get the pixels which are a union of the two thresholds
		if(bThreshWithOpenCV) {
			grayThreshNear = grayImage;
			grayThreshFar = grayImage;
			grayThreshNear.threshold(nearThreshold, true);
			grayThreshFar.threshold(farThreshold);
			cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
		} else {
			
			// or we do it ourselves - show people how they can work with the pixels
			unsigned char * pix = grayImage.getPixels();
			
			int numPixels = grayImage.getWidth() * grayImage.getHeight();
			for(int i = 0; i < numPixels; i++) {
				if(pix[i] < nearThreshold && pix[i] > farThreshold) {
					pix[i] = 255;
				} else {
					pix[i] = 0;
				}
			}
		}
		
		// update the cv images
		grayImage.flagImageChanged();
		
		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		//contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/2, 20, false);
	}
	
#ifdef USE_TWO_KINECTS
	kinect2.update();
#endif
    
}

//--------------------------------------------------------------
void testApp::draw() {
	
	ofSetColor(255, 255, 255);
    
    easyCam.begin();
    if(!playing) {
        drawPointCloud();
    } else {
        if(meshRecorder.readyToPlay) {
            drawRecordedPointCloud();
        }
    }
    easyCam.end();
    
    if(!playing) {
        kinect.draw(10, 10, 400/4, 300/4);
        kinect.drawDepth(10, 320/4+10, 400/4, 300/4);
        grayImage.draw(10, 320/2, 400/4, 300/4);
        #ifdef USE_TWO_KINECTS
		kinect2.draw(420, 320, 400, 300);
        #endif
    }
  
    // Loadinf info:
    
    if(!meshRecorder.readyToPlay) {
        string l = ofToString(meshRecorder.FramesLoaded);
        string t = ofToString(meshRecorder.TotalFrames);
        myFont.drawString("loading... " + l + "/" + t,10, 612);
    }
    
	// draw instructions
	ofSetColor(255, 255, 255);
	stringstream reportStream;
    
    reportStream << "ofxKinectMeshRecording 0.1 :: welovecode " << ", fps: " << ofGetFrameRate() << endl
    << " " << endl
    << "r: START RECORDING" << endl
    << "s: STOP RECORDING" << endl
    << "l: LAST RECORDING / LIVE MODE" << endl
    << " " << endl
	<< "set near threshold " << nearThreshold << " (press: + -)" << endl
	<< "set far threshold " << farThreshold << " (press: < >) " << endl
	<< "press c to close the connection and o to open it again, connection is: " << kinect.isConnected()
	<< "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
    << "SPACE Color Mode on/off" << endl;
	//ofDrawBitmapString(reportStream.str(),10,652);
    myFont.drawString(reportStream.str(), 10, 632);
    ofSetColor(255, 0, 0);
    myFont.drawString(saveTo,120, 20);

}

//--------------------------------------------------------------
void testApp::savePointCloud() {
    
    int w = 640;
	int h = 480;
    
    //char buff[200];
    //stringstream stringToSave;
    FILE* fout = fopen((saveTo + "frame" + ofToString(frame) + ".txt").c_str(), "w");
    

    int pIndex = 0;
    
    for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
            
            ofVec3f v2;
            v2.set(0,0,0);
            
            float distancia;
            distancia = kinect.getDistanceAt(x, y);
            
            if(distancia > distanciaMinima && distancia < distanciaMaxima) {
                
                v2 = kinect.getWorldCoordinateAt(x, y);
            
            }
            
            ofColor pColor;
            pColor = kinect.getColorAt(x, y);
            
            /*
            stringToSave << pIndex << ","
            << v2.x << ","
            << v2.y << ","
            << v2.z << ","
            << pColor.getHex()
             << "\n";
             */
            
            //sprintf(buff, "%i%s%f%s%f%s%f%s%i%s", pIndex, ",", v2.x, ",",  v2.y, ",",  v2.z, ",",  pColor.getHex(), "\n");
            fprintf(fout, "%i%s%f%s%f%s%f%s%i%s", pIndex, ",", v2.x, ",",  v2.y, ",",  v2.z, ",",  pColor.getHex(), "\n");
            //stringToSave << buff;
            pIndex++;
            
        }
    }
    
    
    
    fclose(fout);
    /*
    ofstream framesFile ((saveTo + "frame" + ofToString(frame) + ".txt").c_str());
    framesFile << stringToSave.str();
    //framesFile << buff;
    framesFile.close();
    */
    
    
    frame++;
    
}

//--------------------------------------------------------------
void testApp::drawPointCloud() {
    
	int w = 640;
	int h = 480;
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
    
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
            
            ofVec3f v2;
            v2.set(0,0,0);
            
            float distancia;
            
            distancia = kinect.getDistanceAt(x, y);
            
			if(distancia > distanciaMinima && distancia < distanciaMaxima) {
                v2 = kinect.getWorldCoordinateAt(x, y);
                //if(v2.z > 0)
                mesh.addVertex(v2);
                if(colorMode) mesh.addColor(kinect.getColorAt(x, y));
            }
        }
	}

	glPointSize(1);
    glEnable(GL_POINT_SMOOTH); // use circular points instead of square points
	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards' 
	ofScale(1, -1, -1);
	ofTranslate(0, 0, -750); // center the points a bit
	glEnable(GL_DEPTH_TEST);
	mesh.drawVertices();
	glDisable(GL_DEPTH_TEST);
    mesh.clear();
	ofPopMatrix();
 
}


//--------------------------------------------------------------
void testApp::drawRecordedPointCloud() {
    
    int w = 640;
    int h = 480;
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_POINTS);
    
    int pCount = 0;
    
    for(int y = 0; y < h; y += step) {
        for(int x = 0; x < w; x += step) {
            
            ofVec3f v2;
            v2.set(0,0,0);

            //float distancia;
            //distancia = kinect.getDistanceAt(x, y);
            
            //if(distancia > distanciaMinima && distancia < distanciaMaxima) {
                v2 = meshRecorder.getVectorAt(frameToPlay, pCount);
                //if(v2.z > 0)
                mesh.addVertex(v2);
            
                ofColor c;
                c = meshRecorder.getColorAt(frameToPlay, pCount);
                if(colorMode) mesh.addColor(c);
            
            //}
   
            pCount ++;
            
        }
    }

    glPointSize(1);
    //glEnable(GL_POINT_SMOOTH); // use circular points instead of square points
    ofPushMatrix();
    // the projected points are 'upside down' and 'backwards' 
    ofScale(1, -1, -1);
    ofTranslate(0, 0, -750); // center the points a bit
    glEnable(GL_DEPTH_TEST);
    mesh.drawVertices();
    glDisable(GL_DEPTH_TEST);
    mesh.clear();
    ofPopMatrix();
         
}



//--------------------------------------------------------------

string testApp::generateFileName() {

	string _root = "";
    
	_timestamp = ofToString(ofGetDay()) +
	ofToString(ofGetMonth()) +
	ofToString(ofGetYear()) +
	ofToString(ofGetHours()) +
	ofToString(ofGetMinutes()) +
	ofToString(ofGetSeconds());
    
	string _filename = (_root + _timestamp + "/");
    
    dirHelper.createDirectory(_filename);
    
    ofFile file(ofToDataPath(_filename));
    //cout << file.getAbsolutePath();
    
	return file.getAbsolutePath() + "/";
    
}


//--------------------------------------------------------------
void testApp::exit() {
    
    // Todo: Esto dentro de la clase!!
    // No sabemos si funciona muy bien lo de detener el Thread :(
    meshRecorder.unlock();
    meshRecorder.stopThread(false);
    
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
#ifdef USE_TWO_KINECTS
	kinect2.close();
#endif
    
}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {
	switch (key) {
        
        case 'l':
            if(!meshRecorder.readyToPlay) return;
            // Todo: Cuando sepamos detener el Thread que pueda hacerse en cualquier
            // momento durante la carga.
            if(recording) return;
            if(!playing) {
                    playing = true;
                    frameToPlay = 0;
                    meshRecorder.startLoading(_timestamp);
                } else {
                    playing = false;
                }
            break; 
            
        case 'r':
            if(!meshRecorder.readyToPlay) return;
            if(recording) return;
            if(playing) return;
            saveTo = generateFileName();
            frame = 0;
            recording = true;
			break;   
        
        case 's':
            if(!meshRecorder.readyToPlay) return;
            if(!recording) return;
            if(playing) return;
            saveTo = "";
            recording = false;
			break; 
            
		case ' ':
			colorMode = !colorMode;
			break;
			
		case'p':
			bDrawPointCloud = !bDrawPointCloud;
			break;
			
		case '>':
		case '.':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;
			
		case '<':
		case ',':
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
			break;
			
		case '+':
		case '=':
			nearThreshold ++;
			if (nearThreshold > 255) nearThreshold = 255;
			break;
			
		case '-':
			nearThreshold --;
			if (nearThreshold < 0) nearThreshold = 0;
			break;
			
		case 'w':
			kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
			break;
			
		case 'o':
			kinect.setCameraTiltAngle(angle); // go back to prev tilt
			kinect.open();
			break;
			
		case 'c':
			kinect.setCameraTiltAngle(0); // zero the tilt
			kinect.close();
			break;
			
		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;
			
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
    }
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{}
