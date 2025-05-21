#include "Animations.h"
#include "raylib.h"

// Enhanced Idle (default) frames
const char* idleFrames[] = {
    R"(
    /\_/\  
   ( o.o ) 
    > ^ <  
   /     \
)",
    R"(
    /\_/\  
   ( o.o ) 
    > ~ <  
   /     \
)"
};

// Enhanced Eating frames with better food
const char* eatingFrames[] = {
    R"(
    /\_/\  
   ( o.o ) 
    > ^ <  
   (  🍔  )
)",
    R"(
    /\_/\  
   ( ^.^ ) 
    > o <  
   ( nom! )
)"
};

// Enhanced Cleaning frames with better effects
const char* cleaningFrames[] = {
    R"(
    /\_/\  
   ( o.o )✨
    > ^ < ✨
   /     \✨
)",
    R"(
   ✨/\_/\  
  ✨( ^.^ ) 
   ✨> ^ <  
    /     \
)"
};

// Enhanced Sleeping frames with better effects
const char* sleepingFrames[] = {
    R"(
    /\_/\  
   (-.- )  zzZ
    (   ) Z
   /     \
)",
    R"(
    /\_/\  
   ( -.-) Zzz
   z(   )  
   /     \
)"
};

// New Playing frames
const char* playingFrames[] = {
    R"(
    /\_/\  
   ( ^o^ ) 
   ~> v <~ 
   /  |  \
)",
    R"(
    /\_/\  
   ( >w< ) 
   ~> ^ <~ 
   / \|/ \
)"
};

// New Happy frames
const char* happyFrames[] = {
    R"(
    /\_/\  
   ( ^▽^ ) 
    > ♥ <  
   /     \
)",
    R"(
    /\_/\  
   ( >ᴗ< ) 
    > ♪ <  
   /     \
)"
};

// New Sad frames
const char* sadFrames[] = {
    R"(
    /\_/\  
   ( ×_× ) 
    > . <  
   /     \
)",
    R"(
    /\_/\  
   ( ;_; ) 
    > ╥ <  
   /     \
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
    case PLAYING:
        return playingFrames[frame % 2];
    case HAPPY:
        return happyFrames[frame % 2];
    case SAD:
        return sadFrames[frame % 2];
    case IDLE:
    default:
        return idleFrames[frame % 2];
    }
}

// Get background color based on time of day for day/night cycle
Color getBackgroundColor(int hour) {
    // Night (dark blue)
    if (hour >= 22 || hour < 5) {
        return (Color) { 10, 10, 35, 255 };
    }
    // Dawn/Dusk (purple/orange tint)
    else if (hour >= 5 && hour < 7) {
        return (Color) { 155, 140, 180, 255 }; // Dawn purple
    }
    else if (hour >= 19 && hour < 22) {
        return (Color) { 180, 120, 100, 255 }; // Dusk orange
    }
    // Day (light blue/white)
    else {
        return (Color) { 245, 245, 250, 255 };
    }
}

// Get a description for the current time of day
const char* getTimeDescription(int hour) {
    if (hour >= 22 || hour < 5) {
        return "Night";
    }
    else if (hour >= 5 && hour < 7) {
        return "Dawn";
    }
    else if (hour >= 7 && hour < 12) {
        return "Morning";
    }
    else if (hour >= 12 && hour < 17) {
        return "Afternoon";
    }
    else if (hour >= 17 && hour < 19) {
        return "Evening";
    }
    else { // 19-22
        return "Dusk";
    }
}