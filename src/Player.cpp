//
//  Player.cpp
//  stupidApp
//
//  Created by Brandon Liu on 2/25/17.
//
//

#include "Player.h"
#include "ofMain.h"

void Player::updatePosition() {
    if (position < 0) position = 0;
    if (position > 400) position = 400;
    position = position + speed;
    speed = speed * 0.90;
}

float Player::getPosition() {
    return position;
}

float Player::getAverage() {
    return average;
}

void Player::addSample(float sample) {
    if (average == 50.0) {
        average = sample;
    } else {
        average = (average + sample) / 2;
    }
}

void Player::addSpeed(float sample) {
    speed += ofMap((sample-average),-10,10,-3.0,3.0);
}
