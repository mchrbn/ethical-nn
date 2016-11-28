#include "testApp.h"

#define PATH_CAPITALIST "/Users/mchrbn/Documents/openframworks0080/apps/myApps/NeuralNetwork/bin/data/capitalist.data"
#define PATH_LIBERTARIAN "/Users/mchrbn/Documents/openframworks0080/apps/myApps/NeuralNetwork/bin/data/libertarian.data"
#define PATH_UTILITARIAN ""
#define PATH_CONFUCIANISM ""



//--------------------------------------------------------------
void testApp::setup(){
    
    //General
    ofBackground(0, 95, 107);
    
    consoleLog = "";
    
    //Setup GUI
    setGUIA();
    setGUIP();
    setConsole();
    
}

//--------------------------------------------------------------
void testApp::update(){
    
}

//--------------------------------------------------------------
void testApp::draw(){
}


void testApp::guiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if(name == "calculate"){
        ofxUIButton *button = (ofxUIButton *) e.widget;
		if(button->getValue())
            testNetwork();
    }
    else if(name == "train"){
        ofxUIButton *button = (ofxUIButton *) e.widget;
        if(button->getValue())
            trainNetwork();
    }
}


void testApp::testNetwork(){
    
    
    ofxUIIntSlider *sRev = (ofxUIIntSlider*) guiP->getWidget("revenue");
    ofxUIIntSlider *sLater = (ofxUIIntSlider*) guiP->getWidget("later");
    ofxUIIntSlider *sRel = (ofxUIIntSlider*) guiP->getWidget("relative");
    ofxUIToggle *tSkill = (ofxUIToggle*) guiP->getWidget("skill");
    ofxUIIntSlider *sJail = (ofxUIIntSlider*) guiP->getWidget("jail");
    ofxUIIntSlider *sAge = (ofxUIIntSlider*) guiP->getWidget("age");
    ofxUIToggle *tIll = (ofxUIToggle*) guiP->getWidget("terminally ill");
    ofxUIToggle *tWantDie = (ofxUIToggle*) guiP->getWidget("wants to die");
    ofxUIToggle *tFamilyAgree = (ofxUIToggle*) guiP->getWidget("family agrees");
    ofxUIToggle *tConscious = (ofxUIToggle*) guiP->getWidget("unconscious");
    ofxUIToggle *tWrittenProof = (ofxUIToggle*) guiP->getWidget("written proof");
    ofxUIIntSlider *sYear = (ofxUIIntSlider*) guiP->getWidget("year");
    ofxUIToggle *tFemale = (ofxUIToggle*) guiP->getWidget("female");
    
    vector<float> test;
    if(algorithmType == 1){
        test.push_back(sRev->getValue()/100.f);
        test.push_back(sLater->getValue()/100.f);
        test.push_back(sRel->getValue()/100.f);
        test.push_back(tSkill->getValue());
        test.push_back(sJail->getValue()/100.f);
        test.push_back(tWantDie->getValue());
        test.push_back(tFamilyAgree->getValue());
    }
    else if(algorithmType == 2){
        test.push_back(sAge->getValue()/100.f);
        test.push_back(tIll->getValue());
        test.push_back(tWantDie->getValue());
        test.push_back(tFamilyAgree->getValue());
        test.push_back(tConscious->getValue());
        test.push_back(tWrittenProof->getValue());
    }
    else if(algorithmType == 3){
    }
    
    float *output = fann_run(ann, &test[0]);
    
    stringstream ss1, ss2;
    ss1 << output[0];
    ss2 << output[1];
    
    ofxUITextArea *txtA = (ofxUITextArea*) console->getWidget("console");
    string strResult = "\n\nLive: " + ss1.str() + "\n\nPass Away: " + ss2.str();
    txtA->setTextString(consoleLog + strResult);
    
}


void testApp::trainNetwork(){
    
    fann_destroy(ann);
    
    //DŽtermine l'algorithme ˆ utiliser
    ofxUIRadio *radio = (ofxUIRadio*) guiA->getWidget("ethic");
    string path = "";
    
    if(radio->getActive()->getName() == "capitalist"){
        path = PATH_CAPITALIST;
        algorithmType = 1;
    }
    else if(radio->getActive()->getName() == "libertarian"){
        path = PATH_LIBERTARIAN;
        algorithmType = 2;
    }
    else if(radio->getActive()->getName() == "kevorkian"){
    }
    
    
    //Load & Train ANN
    struct fann_train_data *trainData;
    trainData = fann_read_train_from_file(path.c_str());
    
    ann = fann_create_standard(3, trainData->num_input, 32, trainData->num_output);
    
    fann_train_on_data(ann, trainData, 400, 10, 0.0001);
    
    
    fann_destroy_train(trainData);
    
    
    //Test the network - reclassify cases
    struct fann_train_data *testData;
    testData = fann_read_train_from_file(path.c_str());
    
    int success = 0;
    for(int i=0; i<fann_length_train_data(testData); i++){
        float *o = fann_run(ann, testData->input[i]);
        
        if((o[0] >= testData->output[i][0]-0.1) || (o[1] == testData->output[i][1]-0.1)){
            success++;
        }
    }
    
    stringstream ss;
    ss << (success/(float)fann_length_train_data(testData))*100.f;
    
    ofxUITextArea *txtA = (ofxUITextArea*) console->getWidget("console");
    consoleLog = "\nReclassification success: " + ss.str() + "%\n";
    txtA->setTextString(consoleLog);
     
    fann_destroy_train(testData);
}



void testApp::exit(){
    fann_destroy(ann);
}


void testApp::setGUIA(){
    guiA = new ofxUICanvas();
    
    guiA->setFont("GUI/Akkurat-Mono.otf");
    guiA->setFontSize(OFX_UI_FONT_MEDIUM, 7);
    guiA->setColorFill(ofColor(0,223,252));
    guiA->setColorBack(ofColor(52,56,56,200));
    guiA->setWidth(200);
    guiA->setHeight(300);
    guiA->setPosition(20, 20);
    
    vector<string> ethics;
    ethics.push_back("capitalist");
    ethics.push_back("libertarian");
    ethics.push_back("matthieu");
    
    guiA->addWidgetRight(new ofxUILabel("ALGORITHM", OFX_UI_FONT_MEDIUM));
    guiA->addSpacer();
    guiA->addRadio("ethic", ethics);
    guiA->addSpacer();
    guiA->addLabelButton("train", false);
    
    ofAddListener(guiA->newGUIEvent, this, &testApp::guiEvent);
    
}


void testApp::setGUIP(){
    
    
    guiP = new ofxUIScrollableCanvas();
    
    guiP->setFont("GUI/Akkurat-Mono.otf");
    guiP->setFontSize(OFX_UI_FONT_MEDIUM, 7);
    guiP->setColorFill(ofColor(0,223,252));
    guiP->setColorBack(ofColor(52,56,56,200));
    guiP->setWidth(200);
    guiP->setHeight(600);
    guiP->setPosition(230, 20);
    
    guiP->addWidgetRight(new ofxUILabel("PERSON", OFX_UI_FONT_MEDIUM));
    guiP->addSpacer();
    guiP->addIntSlider("revenue", 0, 100, 50);
    guiP->addIntSlider("later", 0, 100, 50);
    guiP->addIntSlider("relative", 0, 100, 50);
    guiP->addToggle("skill", false);
    guiP->addIntSlider("jail", 0, 100, 50);
    guiP->addSpacer();
    guiP->addIntSlider("age", 0, 100, 30);
    guiP->addToggle("terminally ill", false);
    guiP->addToggle("wants to die", false);
    guiP->addToggle("family agrees", true);
    guiP->addToggle("unconscious", false);
    guiP->addToggle("written proof", false);
    guiP->addSpacer();
    guiP->addIntSlider("year", 1990, 1998, 1995);
    guiP->addToggle("female", false);
    guiP->addSpacer();
    guiP->addLabelButton("calculate", false);
    
    ofAddListener(guiP->newGUIEvent, this, &testApp::guiEvent);
    
}


void testApp::setConsole(){
    console = new ofxUICanvas();
    
    console->setFont("GUI/Akkurat-Mono.otf");
    console->setFontSize(OFX_UI_FONT_MEDIUM, 7);
    console->setColorFill(ofColor(0,223,252));
    console->setColorBack(ofColor(52,56,56,200));
    console->setWidth(400);
    console->setHeight(300);
    console->setPosition(440, 20);
    
    console->addWidgetRight(new ofxUILabel("OUTPUT", OFX_UI_FONT_MEDIUM));
    console->addSpacer();
    console->addTextArea("console", consoleLog);
    
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
