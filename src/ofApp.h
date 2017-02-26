#pragma once

#include "ofMain.h"

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <queue>
#include <stdio.h>
#include <math.h>

#include "constants.h"
#include "findEyeCenter.h"
#include "findEyeCorner.h"
#include "Player.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    /** Function Headers */
    void detectAndDisplay( cv::Mat frame, cv::Mat debugImage, string face_window_name, Player* player);
    void findEyes(cv::Mat frame_gray, cv::Rect face,string face_window_name, Player* player);
    cv::Mat findSkin(cv::Mat &frame);
    
    private:
        /** Global variables */
        //-- Note, either copy these two files from opencv/data/haarscascades to your current folder, or change these locations
        cv::String face_cascade_name = "/Users/Bdon/workspace/eyeLike/res/haarcascade_frontalface_alt.xml";
        cv::CascadeClassifier face_cascade;
        std::string player1_debug_window_name = "Player1 Debug";
        std::string player1_face_window_name = "Player1 Face";
        std::string player2_debug_window_name = "Player2 Debug";
        std::string player2_face_window_name = "Player2 Face";

        cv::RNG rng = rng(12345);
        cv::Mat debugImageLeft, debugImageRight;
        cv::Mat skinCrCbHist = cv::Mat::zeros(cv::Size(256, 256), CV_8UC1);

        cv::Mat frame;
        cv::VideoCapture capture;

        Player* player1;
        Player* player2;
    
        bool gameIsPaused = TRUE;
        bool isCalibrating = FALSE;
    
        ofVec2f ballPosition = ofVec2f(200.0,400.0);
        ofVec2f ballDirection = ofVec2f(0.0,-5.0);
    
        ofTrueTypeFont font;
        int score = 0;
    
    float paddleWidth = 200;
    float ballSpeed = 5.0;
    
    ofSoundPlayer beepSound;
};
