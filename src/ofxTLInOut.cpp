/**
 * ofxTimeline
 * openFrameworks graphical timeline addon
 *
 * Copyright (c) 2011-2012 James George
 * Development Supported by YCAM InterLab http://interlab.ycam.jp/en/
 * http://jamesgeorge.org + http://flightphase.com
 * http://github.com/obviousjim + http://github.com/flightphase
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "ofxTLInOut.h"
#include "ofxTimeline.h"

ofxTLInOut::ofxTLInOut()
 : ofxTLTrack(),
	dragOffset(0),
	draggingIn(false),
    draggingOut(false),
	hoveringIn(false),
	hoveringOut(false),
    txtLoopName()
{
    setRange(ofFloatRange(0.2, ofRandom(0.4, 0.8)));
    setLoopName("loop");
    txtLoopName.setup(); // enable txt box
}

void ofxTLInOut::draw(){
    ofPushStyle();

	ofRange screenXRange(bounds.getMinX(),bounds.getMaxX());
    if(bounds.height > 2){
        ofSetLineWidth(3);
        int inScreenX = normalizedXtoScreenX( range.min );
        int outScreenX = normalizedXtoScreenX( range.max );
        if(screenXRange.contains(inScreenX)){
            if(hoveringIn){
                ofSetColor(timeline->getColors().highlightColor);
            }
            else{
                ofSetColor(timeline->getColors().keyColor);
            }
            ofLine(inScreenX, bounds.y, inScreenX, bounds.y+bounds.height);
        }

        if(screenXRange.contains(outScreenX)){
            if(hoveringOut){
                ofSetColor(timeline->getColors().highlightColor);
            }
            else{
                ofSetColor(timeline->getColors().keyColor);
            }
            ofLine(outScreenX, bounds.y, outScreenX, bounds.y+bounds.height);
        }
    }


    //draw inout over the whole thing
    ofFill();
    ofSetLineWidth(1);
	//draw in/out point
    float inPointX = ofClamp(normalizedXtoScreenX(range.min), screenXRange.min, screenXRange.max);
    float outPointX = ofClamp(normalizedXtoScreenX(range.max),screenXRange.min, screenXRange.max);

	if(bounds.x < inPointX){
		ofSetColor(timeline->getColors().disabledColor,120/timeline->getLoops().size());
		ofRect(bounds.x, pageRect.y, inPointX - bounds.x, pageRect.height);
		ofSetColor(timeline->getColors().highlightColor);
		ofLine(inPointX, pageRect.y, inPointX, pageRect.y+pageRect.height);
	}

	if(bounds.x+bounds.width > outPointX){
		ofSetColor(timeline->getColors().disabledColor,120/timeline->getLoops().size());
		ofRect(outPointX, pageRect.y, (bounds.x+bounds.width) - outPointX, pageRect.height);
		ofSetColor(timeline->getColors().highlightColor);
		ofLine(outPointX, pageRect.y, outPointX, pageRect.y+pageRect.height);
	}

    // draw play icon
    ofDrawTriangle(inPointX, bounds.y + 7, inPointX, bounds.y + 17, inPointX + 10, bounds.y + 12);

    // draw txt box
    txtLoopName.bounds.x = inPointX + 22;
    txtLoopName.bounds.setWidth( abs(outPointX - inPointX - 34.0 ));
    txtLoopName.bounds.y = bounds.y + 5;
    ofSetColor(timeline->getColors().keyColor);
    ofNoFill();
    ofRect(txtLoopName.bounds);
    if(hoveringIn || hoveringOut){
        ofSetColor(timeline->getColors().highlightColor);
    }
    else{
        ofSetColor(timeline->getColors().keyColor);
    }
    txtLoopName.draw();

	ofPopStyle();
}

void ofxTLInOut::setPageRectangle(ofRectangle pageRectangle){
    pageRect = pageRectangle;
}

void ofxTLInOut::mouseMoved(ofMouseEventArgs& args){

    if(!bounds.inside(args.x,args.y)) return;

    hoveringIn  = abs( normalizedXtoScreenX( range.min ) - args.x) < 17;
    hoveringOut = abs( normalizedXtoScreenX( range.max ) - args.x) < 7;
}

void ofxTLInOut::mousePressed(ofMouseEventArgs& args){

    if(!bounds.inside(args.x,args.y)) return;


	float distToIn = normalizedXtoScreenX( range.min ) - args.x;
    if(abs(distToIn) < 17){
        draggingIn = true;
        dragOffset = distToIn;

        if (args.button == 0) { // set current loop to this
            timeline->setCurrentLoop(this);
        }
        else { // right click remove it
            vector<ofxTLInOut*>::iterator it = timeline->getLoops().begin();
            while(it != timeline->getLoops().end()){
                if ((*it) == this) {
                    if (next(it) != timeline->getLoops().end()) {
                        timeline->setCurrentLoop((*next(it)));
                        timeline->getLoops().erase(it);
                    }
                }
                it++;
            }
        }
    }

    float distToOut = normalizedXtoScreenX( range.max ) - args.x;
    if(!draggingIn && abs(distToOut) < 7){
        draggingOut = true;
        dragOffset = distToOut;
    }

    /*
    disabled setting in/out on click, was good for one single loop.
    */
    /*
    if(!draggingIn && !draggingOut){
        if(abs(distToOut) > abs(distToIn)){
            draggingIn = true;
            dragOffset = 0;
            timeline->setInPointAtPercent(screenXtoNormalizedX(args.x));
        }
        else{
            draggingOut = true;
            dragOffset = 0;
            timeline->setOutPointAtPercent(screenXtoNormalizedX(args.x));
        }
    }
    */

    //cout << "dist to in " << abs(distToIn) << " out " << abs(distToOut) << " " << draggingIn << " " << draggingOut << endl;
}

void ofxTLInOut::mouseDragged(ofMouseEventArgs& args){
    if(draggingIn){
        range.setMin(screenXtoNormalizedX(args.x - dragOffset));
    }
    if(draggingOut){
        range.setMax(screenXtoNormalizedX(args.x - dragOffset));
    }
}

void ofxTLInOut::mouseReleased(ofMouseEventArgs& args){
    if(draggingIn || draggingOut){
	    draggingIn = draggingOut = false;
        save();
    }
}

void ofxTLInOut::load(){

	ofxXmlSettings settings;
	if(!settings.loadFile(xmlFileName)){
		ofLog(OF_LOG_VERBOSE, "ofxTLInOut -- couldn't load in/out settings file " + xmlFileName);
        timeline->setInOutRange(ofRange(0,1.0));
		return;
	}
	settings.pushTag("inout");
	timeline->setInOutRange(ofRange(settings.getValue("in", 0.0),settings.getValue("out", 1.0)));
	settings.popTag();

}

void ofxTLInOut::save(){
	ofxXmlSettings savedSettings;
	savedSettings.addTag("inout");
	savedSettings.pushTag("inout");
	savedSettings.addValue("in", range.min);
	savedSettings.addValue("out", range.max);
	savedSettings.popTag();//zoom
	savedSettings.saveFile(xmlFileName);
}

