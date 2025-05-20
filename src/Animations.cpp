#include "Animations.h"

// Simple ASCII animation frames of the pet blinking or moving
static const char* petFrames[] = {
    R"(
  (o_o)
  /| |\
   / \
)",
    R"(
  (O_O)
  /| |\
   / \
)",
    R"(
  (-_-)
  /| |\
   / \
)"
};

const int NUM_FRAMES = sizeof(petFrames) / sizeof(petFrames[0]);

const char* getPetFrame(int frameIndex) {
    return petFrames[frameIndex % NUM_FRAMES];
}
