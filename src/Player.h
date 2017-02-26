//
//  Player.hpp
//  stupidApp
//
//  Created by Brandon Liu on 2/25/17.
//
//

#ifndef Player_hpp
#define Player_hpp

#include <stdio.h>

#endif /* Player_hpp */

class Player {
    
public:
    void updatePosition();
    float getPosition();
    float getAverage();
    void addSample(float sample);
    void addSpeed(float sample);
    bool hasFace;
    
private:
    float position = 200.0;
    float speed = 0.0;
    float average = 50.0;
};
