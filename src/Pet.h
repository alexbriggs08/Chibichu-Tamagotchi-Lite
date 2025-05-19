#ifndef PET_H
#define PET_H

struct Pet {
    int hunger = 50;
    int happiness = 50;
    int health = 100;
    int age = 0;
    bool asleep = false;

    // You could add methods here if needed
    // void updateAge() { age++; }
    // bool isHealthy() const { return health > 50; }
};

#endif // PET_H