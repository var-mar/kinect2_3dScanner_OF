#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	kinect.open();
	kinect.initDepthSource();
	kinect.initColorSource();

	cam.setDistance(10);
	cam.disableMouseInput();

	ofSetWindowShape(1920, 1080);
	scannedMesh = new ofMesh();
	scannedMesh->setMode(OF_PRIMITIVE_POINTS);
	photoMesh = new ofMesh();
	photoMesh->setMode(OF_PRIMITIVE_POINTS);

	setupGui();

	scanningArea = new ofBoxPrimitive();
	scanningArea->set(DEFAULT_AREA_X, DEFAULT_AREA_Y, DEFAULT_AREA_Z);
	scanningArea->setPosition(0, 0, (float)MAX_DEPTH*.5f);
}

//--------------------------------------------------------------
void ofApp::update(){
	ofSetWindowTitle("3D Scanning Tool for Kinect v2 FPS: "+ ofToString(ofGetFrameRate()));
	kinect.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(75, 95, 115);

	ofSetColor(255);
	kinect.getColorSource()->draw(0, 0, 320, 180);
	
	cam.begin();
		ofPushStyle();
			ofScale(10, 10, -10);
			scanningArea->drawWireframe();
			scannedMesh->drawVertices();
			photoMesh->drawVertices();
			drawGuideLines();
		ofPopStyle();
	cam.end();
}


void ofApp::rotateMesh(ofMesh* _mesh, float _angle){
	ofMatrix4x4 matrix, matrixTrans, matrixRot, matrixTrans2;
	matrixTrans.makeTranslationMatrix(ofVec3f(0, 0, -distanceToObject));
	matrixRot.makeRotationMatrix(_angle, ofVec3f(0, 1, 0));
	matrixTrans2.makeTranslationMatrix(ofVec3f(0, 0, distanceToObject));
	matrix = matrixTrans*matrixRot*matrixTrans2;
	vector<ofVec3f>* vertices = &_mesh->getVertices();

	for (int i = 0; i < vertices->size(); i++) 
		vertices->at(i) = ofVec3f(vertices->at(i).x, vertices->at(i).y, vertices->at(i).z) * matrix;
}

void ofApp::translateMesh(ofMesh* _mesh, ofVec3f _pos) {
	ofMatrix4x4 matrix;
	matrix.makeTranslationMatrix(_pos);
	vector<ofVec3f>* vertices = &_mesh->getVertices();
	for (int i = 0; i < vertices->size(); i++)
		vertices->at(i) = ofVec3f(vertices->at(i).x, vertices->at(i).y, vertices->at(i).z) * matrix;
}

void ofApp::addVerticesFromDepth(ofMesh* _mesh) {

	vector<ofPoint> vertices = kinect.getDepthSource()->getMesh(false, ofxKinectForWindows2::Source::Depth::PointCloudOptions::TextureCoordinates::ColorCamera).getVertices();

	for (int i = 0; i < vertices.size(); i++) {
		if (vertices[i].z > scanningArea->getPosition().z-scanningArea->getDepth()*.5f && vertices[i].z < scanningArea->getPosition().z + scanningArea->getDepth()*.5f) {
			if (vertices[i].x > scanningArea->getPosition().x - scanningArea->getWidth()*.5f && vertices[i].x < scanningArea->getPosition().x + scanningArea->getWidth()*.5f) {
				if (vertices[i].y > scanningArea->getPosition().y - scanningArea->getHeight()*.5f && vertices[i].y < scanningArea->getPosition().y + scanningArea->getHeight()*.5f) {
					if (vertices[i].z != -std::numeric_limits<double>::infinity()) {
						_mesh->addVertex(vertices[i]);
						_mesh->addColor(ofColor::red);
					}
				}
			}
		}
	}
}

void ofApp::addVerticesFromMesh(ofMesh* _targetMesh, ofMesh* _sourceMesh) {
	vector<ofPoint> vertices = _sourceMesh->getVertices();
	for (int i = 0; i < vertices.size(); i++) {
		_targetMesh->addVertex(vertices[i]);
		_targetMesh->addColor(ofColor::blue);
	}
}

void ofApp::clearMesh(ofMesh* _mesh) {
	_mesh->clear();
}

void ofApp::saveMesh(ofMesh* _mesh, string _filename) {
	_mesh->save(_filename);
}

void ofApp::setupGui() {
	gui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	ofxDatGuiButton* button = gui->addButton("NEW PHOTOSHOOT");
	button->onButtonEvent(this, &ofApp::onButtonEvent);

	ofxDatGuiSlider* slider = gui->addSlider("NUMBER OF PHOTOS", 0, 36, MAX_PHOTOS);
	slider->onSliderEvent(this, &ofApp::onSliderEvent);
	slider = gui->addSlider("AREA X", 0, (float)MAX_DEPTH, DEFAULT_AREA_X);
	slider->onSliderEvent(this, &ofApp::onSliderEvent);
	slider = gui->addSlider("AREA Y", 0, (float)MAX_DEPTH, DEFAULT_AREA_Y);
	slider->onSliderEvent(this, &ofApp::onSliderEvent);
	slider = gui->addSlider("AREA Z", 0, (float)MAX_DEPTH, DEFAULT_AREA_Z);
	slider->onSliderEvent(this, &ofApp::onSliderEvent);
	slider = gui->addSlider("AREA POSITION", 0, (float)MAX_DEPTH, DEFAULT_AREA_POSITION);
	slider->onSliderEvent(this, &ofApp::onSliderEvent);
	slider = gui->addSlider("DISTANCE TO OBJECT", 0, MAX_DEPTH, DISTANCE_TO_OBJECT);
	slider->onSliderEvent(this, &ofApp::onSliderEvent);

	textInput = gui->addTextInput("PHOTOS TAKEN",ofToString(INITIAL_CURRENT_PHOTO)+"/"+ofToString(DEFAULT_NUMBER_OF_PHOTOS));
	textInput->setEnabled(false);
	button = gui->addButton("TAKE PHOTO");
	button->onButtonEvent(this, &ofApp::onButtonEvent);
	nextPhotoButton = gui->addButton("NEXT PHOTO");
	nextPhotoButton->onButtonEvent(this, &ofApp::onButtonEvent);
	exportButton = gui->addButton("EXPORT MODEL");
	exportButton->onButtonEvent(this, &ofApp::onButtonEvent);

	resetGuiValues();
}

void ofApp::resetGuiValues() {
	ofxDatGuiSlider* slider = gui->getSlider("NUMBER OF PHOTOS");
	slider->setValue(DEFAULT_NUMBER_OF_PHOTOS);
	maxPhotos = DEFAULT_NUMBER_OF_PHOTOS;
	currentPhoto = INITIAL_CURRENT_PHOTO;
	textInput->setText(ofToString(currentPhoto) + "/" + ofToString(maxPhotos));
	photoTaken = false;
	nextPhotoButton->setLabelColor(ofColor::grey);
	exportButton->setLabelColor(ofColor::grey);
}

void ofApp::drawGuideLines() {
	ofSetColor(255, 255, 0);
	ofLine(-10, 0, 0, 10, 0, 0);
	ofLine(0, 0, -10, 0, 0, 10);
	ofLine(0, -10, 0, 0, 10, 0);
}

void ofApp::onSliderEvent(ofxDatGuiSliderEvent e)
{
	if (e.target->is("NUMBER OF PHOTOS")) {
		maxPhotos = e.value;
		textInput->setText(ofToString(currentPhoto) + "/" + ofToString(maxPhotos));
	}
	else if (e.target->is("AREA X")) {
		scanningArea->setWidth(e.value);
	}
	else if (e.target->is("AREA Y")) {
		scanningArea->setHeight(e.value);
	}
	else if (e.target->is("AREA Z")) {
		scanningArea->setDepth(e.value);
	}
	else if (e.target->is("AREA POSITION")) {
		scanningArea->setPosition(ofVec3f(0,0,e.value));
	}
	else if (e.target->is("DISTANCE TO OBJECT")) {
		distanceToObject = e.value;
	}
}

void ofApp::onButtonEvent(ofxDatGuiButtonEvent e)
{
	if (e.target->is("NEW PHOTOSHOOT")) {
		clearMesh(scannedMesh);
		clearMesh(photoMesh);
		resetGuiValues();
	}
	else if (e.target->is("TAKE PHOTO")) {
		clearMesh(photoMesh);
		addVerticesFromDepth(photoMesh);
		if (currentPhoto < maxPhotos) {
			photoTaken = true;
			nextPhotoButton->setLabelColor(ofColor::white);
		}
		else {
			exportButton->setLabelColor(ofColor::white);
		}
	}
	else if (e.target->is("NEXT PHOTO")) {
		if (photoTaken) {
				photoTaken = false;
				nextPhotoButton->setLabelColor(ofColor::grey);
				currentPhoto++;
				textInput->setText(ofToString(currentPhoto) + "/" + ofToString(maxPhotos));
				rotateMesh(photoMesh, (float)360 / maxPhotos);
				rotateMesh(scannedMesh, (float)360 / maxPhotos);
				addVerticesFromMesh(scannedMesh, photoMesh);
		}
	}
	else if (e.target->is("EXPORT MODEL")) {
		addVerticesFromMesh(scannedMesh, photoMesh);
		translateMesh(scannedMesh, ofVec3f(0, 0, 0));
		saveMesh(scannedMesh, "scanned-mesh.ply");
		exportButton->setLabelColor(ofColor::grey);
	}
}