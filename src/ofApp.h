#pragma once

#include "ofMain.h"
#include "ofxKinectForWindows2.h"
#include "ofxDatGui.h"

#define DISTANCE_TO_OBJECT 1.7f
#define MAX_DEPTH 4
#define DEFAULT_AREA_X MAX_DEPTH*.5f
#define DEFAULT_AREA_Y MAX_DEPTH*.5f
#define DEFAULT_AREA_Z MAX_DEPTH*.5f
#define DEFAULT_AREA_POSITION MAX_DEPTH*.5f
#define DEFAULT_NUMBER_OF_PHOTOS 4
#define INITIAL_CURRENT_PHOTO 1
#define MAX_PHOTOS 36

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		ofxKFW2::Device kinect;
		ofEasyCam cam;

		//Gui methods & vars
		void setupGui();
		void resetGuiValues();
		void drawGuideLines();
		void onSliderEvent(ofxDatGuiSliderEvent e);
		void onButtonEvent(ofxDatGuiButtonEvent e);
		ofxDatGui* gui;
		ofxDatGuiTextInput* textInput;
		ofxDatGuiButton* exportButton;
		ofxDatGuiButton* nextPhotoButton;
		ofBoxPrimitive* scanningArea;
		int currentPhoto, maxPhotos;
		bool photoTaken = false;
		float distanceToObject = DISTANCE_TO_OBJECT;

		//Mesh methods & vars
		void addVerticesFromDepth(ofMesh* _mesh); //Copy depth vertices into another mesh
		void addVerticesFromMesh(ofMesh* _targetMesh, ofMesh* _sourceMesh); //Copy vertices from one mesh to another
		void rotateMesh(ofMesh* _mesh, float _angle); //Rotate mesh vertices a given angle
		void translateMesh(ofMesh* _mesh, ofVec3f _pos); //Translate mesh to a given point
		void clearMesh(ofMesh* _mesh); //Delete mesh vertices
		void saveMesh(ofMesh* _mesh, string _filename); //Export mesh to PLY file
		ofMesh* scannedMesh;
		ofMesh* photoMesh;
};
