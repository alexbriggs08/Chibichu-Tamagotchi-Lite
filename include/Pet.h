#ifndef PET_H
#define PET_H

class Pet {
private:
    int hunger{ 90 };
    int happiness{ 80 };
    int health{ 100 };
    int age{ 0 };
    bool asleep{ false };

public:
    void feed();
    void play();
    void sleep();
    void wakeUp();
    void update();
    bool isAlive();
};

#endif