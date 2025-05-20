#include "Animations.h"

// Idle (default) frames
const char* idleFrames[] = {
    R"(
  /\_/\  
 ( o.o ) 
  > ^ <  
)",
    R"(
  /\_/\  
 ( o.o ) 
  > v <  
)"
};

// Eating frames (ASCII-style food)
const char* eatingFrames[] = {
    R"(
  /\_/\  
 ( o.o ) 
  > ^ <  
 (  *o* )
)",
    R"(
  /\_/\  
 ( ^.^ ) 
  > o <  
 (  mmm )
)"
};

// Cleaning frames (sparkles as ASCII)
const char* cleaningFrames[] = {
    R"(
  /\_/\  
 ( o.o ) 
  > ^ <  
  * * *  
)",
    R"(
  /\_/\  
 ( ^.^ ) 
  > ^ <  
   * *   
)"
};

// Sleeping frames
const char* sleepingFrames[] = {
    R"(
  /\_/\  
 (-.- )  
  (   ) z
)",
    R"(
  /\_/\  
 ( -.-)  
 z(   )  
)"
};

// Returns the frame based on state and frame index
const char* getPetFrame(PetState state, int frame) {
    switch (state) {
    case EATING:
        return eatingFrames[frame % 2];
    case CLEANING:
        return cleaningFrames[frame % 2];
    case SLEEPING:
        return sleepingFrames[frame % 2];
    case IDLE:
    default:
        return idleFrames[frame % 2];
    }
}
