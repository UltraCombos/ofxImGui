#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetLogLevel(OF_LOG_VERBOSE);
	
	m_background_color.setName("Background Color");
	m_background_color.set(ofColor(114, 144, 154));

	typedef std::shared_ptr < ofParameter<int> >			sp_param_int;
	typedef std::shared_ptr < ofParameter<bool> >			sp_param_bool;
	typedef std::shared_ptr < ofParameter<float> >			sp_param_float;
	typedef std::shared_ptr < ofParameter< ofxImGuiInt > >	sp_param_gui_int;
	typedef std::shared_ptr < ofParameter< ofxImGuiEnum > >	sp_param_enum;

	m_param_group.setName("RPI Camera Settings");
	m_param_group.add(*sp_param_int(new ofParameter<int>("Sharpness", 0, -100, 100)));
	m_param_group.add(*sp_param_int(new ofParameter<int>("Contrast", 0, -100, 100)));
	m_param_group.add(*sp_param_int(new ofParameter<int>("Brightness", 50, 0, 100)));
	m_param_group.add(*sp_param_int(new ofParameter<int>("Saturation", 0, -100, 100)));
	m_param_group.add(*sp_param_bool(new ofParameter<bool>("IsAutoISO", true)));
	m_param_group.add(*sp_param_int(new ofParameter<int>("ISO", 100, 100, 2000)));
	m_param_group.add(*sp_param_bool(new ofParameter<bool>("IsAutoShutter", true)));
	m_param_group.add(*sp_param_gui_int(new ofParameter< ofxImGuiInt >("ShutterSpeed_MicroSec", ofxImGuiInt(0, 10, 100))));
	m_param_group.add(*sp_param_int(new ofParameter<int>("DynRangeExp", 0, 0, 3)));
	m_param_group.add(*sp_param_int(new ofParameter<int>("Ev Compensation", 0, -4, 4)));

	{
		ofxImGuiEnum enum_val;
		enum_val.content.push_back("Off");
		enum_val.content.push_back("Auto");
		enum_val.content.push_back("SunLight");
		enum_val.content.push_back("Cloudy");
		enum_val.content.push_back("Shade");
		enum_val.content.push_back("Tungsten");
		enum_val.content.push_back("Fluorescent");
		enum_val.content.push_back("Incandescent");
		enum_val.content.push_back("Flash");
		enum_val.select = 0;

		m_param_group.add(*sp_param_enum(new ofParameter< ofxImGuiEnum >("WhiteBalance", enum_val)));
	}

	{
		ofxImGuiEnum enum_val;
		enum_val.content.push_back("None");
		enum_val.content.push_back("Noise");
		enum_val.content.push_back("Emboss");
		enum_val.content.push_back("Negative");
		enum_val.content.push_back("Sketch");
		enum_val.content.push_back("OilPaint");
		enum_val.content.push_back("Hatch");
		enum_val.content.push_back("Gpen");
		enum_val.content.push_back("Antialias");
		enum_val.content.push_back("DeRing");
		enum_val.content.push_back("Solarize");
		enum_val.select = 0;

		m_param_group.add(*sp_param_enum(new ofParameter< ofxImGuiEnum >("ImageFilter", enum_val)));
	}

	{
		ofxImGuiEnum enum_val;
		enum_val.content.push_back("Off");
		enum_val.content.push_back("Auto");
		enum_val.content.push_back("Night");
		enum_val.content.push_back("BackLight");
		enum_val.content.push_back("SpotLight");
		enum_val.content.push_back("Sports");
		enum_val.content.push_back("Snow");
		enum_val.content.push_back("Beach");
		enum_val.content.push_back("LargeAperture");
		enum_val.content.push_back("SmallApperture");
		enum_val.select = 0;

		m_param_group.add(*sp_param_enum(new ofParameter< ofxImGuiEnum >("Exposure", enum_val)));
	}

	{
		ofxImGuiEnum enum_val;
		enum_val.content.push_back("Average");
		enum_val.content.push_back("Spot");
		enum_val.content.push_back("Matrix");
		enum_val.select = 0;

		m_param_group.add(*sp_param_enum(new ofParameter< ofxImGuiEnum >("MeteringMode", enum_val)));
	}

	{
		ofxImGuiEnum enum_val;
		enum_val.content.push_back("None");
		enum_val.content.push_back("Vertical");
		enum_val.content.push_back("Horizontal");
		enum_val.content.push_back("Both");
		enum_val.select = 0;

		m_param_group.add(*sp_param_enum(new ofParameter< ofxImGuiEnum >("Mirror", enum_val)));
	}

	ofxImGuiParameter::Initialize();

	m_imgui_parameter.setup("ofxImGuiParameter", ofRectangle(10, 10, 480, 640));
	m_imgui_parameter.bind(m_background_color);
	m_imgui_parameter.bind(m_param_group);
	m_imgui_parameter.load();

	ofAddListener(m_param_group.parameterChangedE(), this, &ofApp::mf_on_parameter_changed);
	ofAddListener(ofxImGuiParameter::GetOnDrawEvent(), this, &ofApp::mf_on_ImGui_draw);
}

void ofApp::exit()
{
	ofxImGuiParameter::Finalize();
}

//--------------------------------------------------------------
void ofApp::update()
{

}

//--------------------------------------------------------------
void ofApp::draw()
{   
	ofSetBackgroundColor(m_background_color.get());
	ofxImGuiParameter::Draw();
}

void ofApp::mf_on_parameter_changed(ofAbstractParameter& e)
{

}

void ofApp::mf_on_ImGui_draw()
{
	//You can test any ImGui Code here.
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}


void ofApp::mouseScrolled(float x, float y){

}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}