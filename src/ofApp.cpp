#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup(){
    
    
    
    // Load the cascades
    if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading face cascade, please change face_cascade_name in source code.\n"); return -1; };
    
    cv::namedWindow(player1_debug_window_name,CV_WINDOW_NORMAL);
    cv::moveWindow(player1_debug_window_name, 0, 0);
    cv::namedWindow(player1_face_window_name,CV_WINDOW_NORMAL);
    cv::moveWindow(player1_face_window_name, 300, 0);
    
    cv::namedWindow(player2_debug_window_name,CV_WINDOW_NORMAL);
    cv::moveWindow(player2_debug_window_name, 800, 0);
    cv::namedWindow(player2_face_window_name,CV_WINDOW_NORMAL);
    cv::moveWindow(player2_face_window_name, 900, 600);
    
    createCornerKernels();
    ellipse(skinCrCbHist, cv::Point(113, 155.6), cv::Size(23.4, 15.2),
            43.0, 0.0, 360.0, cv::Scalar(255, 255, 255), -1);
    capture = cv::VideoCapture(0);
    
    player1 = new Player();
    player2 = new Player();
    
    font.load("Karmatic Arcade.ttf",18);
    
    beepSound.load("beep.mp3");
}

//--------------------------------------------------------------
void ofApp::update(){
    if( capture.isOpened() ) {
        capture.read(frame);
        // mirror it
        cv::flip(frame, frame, 1);
        

        cv::Mat leftFrame = cv::Mat(frame, cv::Rect(0,0,frame.cols/2,frame.rows)).clone();
        cv::Mat rightFrame = cv::Mat(frame, cv::Rect(frame.cols/2,0,frame.cols/2,frame.rows)).clone();
        
        
        leftFrame.copyTo(debugImageLeft);
        rightFrame.copyTo(debugImageRight);
        
        // Apply the classifier to the frame
        if( !leftFrame.empty() ) {
            detectAndDisplay( leftFrame, debugImageLeft, player1_face_window_name, player1);
        }
        else {
            printf(" --(!) No captured frame -- Break!");
            return;
        }
        
        // Apply the classifier to the frame
        if( !rightFrame.empty() ) {
            detectAndDisplay( rightFrame, debugImageRight, player2_face_window_name, player2);
        }
        else {
            printf(" --(!) No captured frame -- Break!");
            return;
        }
        imshow(player1_debug_window_name,debugImageLeft);
        imshow(player2_debug_window_name,debugImageRight);
    }
    
    player1->updatePosition();
    player2->updatePosition();
    
    if (!gameIsPaused) {
        ballPosition += ballDirection * ballSpeed;
        ballSpeed = 5.0 + score;
        paddleWidth = 200 - score*10;
        
        // check player1 paddle
        if (ballPosition.y <= 120 && ballPosition.y >= 100) {
            if(abs(ballPosition.x - player1->getPosition()) <= paddleWidth/2) {
            //if (true) {
                ballPosition.y = 120;
                ballDirection.y = -ballDirection.y;
                score++;
                beepSound.play();
            }
        }
        if (ballPosition.y >= 680 && ballPosition.y <= 700) {
            if(abs(ballPosition.x - player2->getPosition()) <= paddleWidth/2) {
                ballPosition.y = 680;
                ballDirection.y = -ballDirection.y;
                score++;
                beepSound.play();
            }
        }
        if (ballPosition.x < 20) {
            ballDirection.x = -ballDirection.x;
        }
        if (ballPosition.x > 380) {
            ballDirection.x = -ballDirection.x;
        }
        
        if (ballPosition.y < 0 || ballPosition.y > 800) {
            // game over
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(ofColor(0,0,0));
    ofSetColor(ofColor(255,255,255));
    
    if (isCalibrating) {
            font.drawString("FINDING YOUR BALLS",40,400);
            ofDrawBitmapString(player1->getAverage(),10,20);
            ofDrawBitmapString(player2->getAverage(),10,700);
    } else {
        ofDrawRectangle(ballPosition.x-10,ballPosition.y-10,20,20);
    }
    
    if (player1->hasFace) {
        ofDrawRectangle(player1->getPosition()-paddleWidth/2,80,paddleWidth,20);
    } else {
        font.drawString("NO FACE DETECTED",40,100);
    }
    
    if (player2->hasFace) {
        ofDrawRectangle(player2->getPosition()-paddleWidth/2,700,paddleWidth,20);
    } else {
        font.drawString("NO FACE DETECTED",40,700);
    }
    
    ostringstream s;
    s << "SCORE = " << score;
    font.drawString(s.str(),10,30);
    
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key ==  ' ') {
        ofSetColor(ofColor(0,0,0));
        
        ballPosition = ofVec2f(200,400);
        ballDirection = ofVec2f(ofRandom(0,1),ofRandom(1,4)).normalize();
        ballSpeed = 5.0;
        score = 0;
        paddleWidth = 200;
        gameIsPaused = FALSE;
    } else if (key == 'c') {
        isCalibrating = !isCalibrating;
    }
    
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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

void ofApp::findEyes(cv::Mat frame_gray, cv::Rect face, string face_window_name, Player* player) {
    cv::Mat faceROI = frame_gray(face);
    cv::Mat debugFace = faceROI;
    
    if (kSmoothFaceImage) {
        double sigma = kSmoothFaceFactor * face.width;
        GaussianBlur( faceROI, faceROI, cv::Size( 0, 0 ), sigma);
    }
    //-- Find eye regions and draw them
    int eye_region_width = face.width * (kEyePercentWidth/100.0);
    int eye_region_height = face.width * (kEyePercentHeight/100.0);
    int eye_region_top = face.height * (kEyePercentTop/100.0);
    cv::Rect leftEyeRegion(face.width*(kEyePercentSide/100.0),
                           eye_region_top,eye_region_width,eye_region_height);
    cv::Rect rightEyeRegion(face.width - eye_region_width - face.width*(kEyePercentSide/100.0),
                            eye_region_top,eye_region_width,eye_region_height);
    
    //-- Find Eye Centers
    cv::Point leftPupil = findEyeCenter(faceROI,leftEyeRegion,"Left Eye");
    cv::Point rightPupil = findEyeCenter(faceROI,rightEyeRegion,"Right Eye");
    // get corner regions
    cv::Rect leftRightCornerRegion(leftEyeRegion);
    leftRightCornerRegion.width -= leftPupil.x;
    leftRightCornerRegion.x += leftPupil.x;
    leftRightCornerRegion.height /= 2;
    leftRightCornerRegion.y += leftRightCornerRegion.height / 2;
    cv::Rect leftLeftCornerRegion(leftEyeRegion);
    leftLeftCornerRegion.width = leftPupil.x;
    leftLeftCornerRegion.height /= 2;
    leftLeftCornerRegion.y += leftLeftCornerRegion.height / 2;
    cv::Rect rightLeftCornerRegion(rightEyeRegion);
    rightLeftCornerRegion.width = rightPupil.x;
    rightLeftCornerRegion.height /= 2;
    rightLeftCornerRegion.y += rightLeftCornerRegion.height / 2;
    cv::Rect rightRightCornerRegion(rightEyeRegion);
    rightRightCornerRegion.width -= rightPupil.x;
    rightRightCornerRegion.x += rightPupil.x;
    rightRightCornerRegion.height /= 2;
    rightRightCornerRegion.y += rightRightCornerRegion.height / 2;

    if (isCalibrating) {
        player->addSample(leftPupil.x);
    } else {
        player->addSpeed(leftPupil.x);
    }

    rightPupil.x += rightEyeRegion.x;
    rightPupil.y += rightEyeRegion.y;
    leftPupil.x += leftEyeRegion.x;
    leftPupil.y += leftEyeRegion.y;
    // draw eye centers
    circle(debugFace, rightPupil, 3, 1234);
    circle(debugFace, leftPupil, 3, 1234);
    

    
    imshow(face_window_name, faceROI);
}


cv::Mat ofApp::findSkin (cv::Mat &frame) {
    cv::Mat input;
    cv::Mat output = cv::Mat(frame.rows,frame.cols, CV_8U);
    
    cvtColor(frame, input, CV_BGR2YCrCb);
    
    for (int y = 0; y < input.rows; ++y) {
        const cv::Vec3b *Mr = input.ptr<cv::Vec3b>(y);
        //    uchar *Or = output.ptr<uchar>(y);
        cv::Vec3b *Or = frame.ptr<cv::Vec3b>(y);
        for (int x = 0; x < input.cols; ++x) {
            cv::Vec3b ycrcb = Mr[x];
            //      Or[x] = (skinCrCbHist.at<uchar>(ycrcb[1], ycrcb[2]) > 0) ? 255 : 0;
            if(skinCrCbHist.at<uchar>(ycrcb[1], ycrcb[2]) == 0) {
                Or[x] = cv::Vec3b(0,0,0);
            }
        }
    }
    return output;
}

/**
 * @function detectAndDisplay
 */
void ofApp::detectAndDisplay( cv::Mat frame, cv::Mat debugImage, string faceWindowName, Player* player) {
    std::vector<cv::Rect> faces;
    //cv::Mat frame_gray;
    
    std::vector<cv::Mat> rgbChannels(3);
    cv::split(frame, rgbChannels);
    cv::Mat frame_gray = rgbChannels[2];
    
    //cvtColor( frame, frame_gray, CV_BGR2GRAY );
    //equalizeHist( frame_gray, frame_gray );
    //cv::pow(frame_gray, CV_64F, frame_gray);
    //-- Detect faces
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(150, 150) );
    //  findSkin(debugImage);
    
    for( int i = 0; i < faces.size(); i++ )
    {
        rectangle(debugImage, faces[i], cv::Scalar(255,255,255), 3);
    }
    //-- Show what you got
    if (faces.size() > 0) {
        player->hasFace = true;
        findEyes(frame_gray, faces[0], faceWindowName, player);
    } else {
        player->hasFace = false;
    }
}
