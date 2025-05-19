#ifndef PET_H
#define PET_H

struct Pet {
    int hunger{ 50 };      // 0-100, where 100 = very hungry, 0 = full
    int happiness{ 50 };   // 0-100
    int health{ 100 };     // 0-100, hidden from player
    int age{ 0 };
    bool asleep{ false };
    bool isDead{ false };  // New death state

    // Helper methods
    bool isStarving() const { return hunger >= 25; }
    bool isHealthy() const { return health > 50; }
    bool isDying() const { return health <= 10; }
};

#endif // PET_H