#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

#include "GraphObject.h"
class StudentWorld;
class Goodie;
class Penelope;
class Actor : public GraphObject
{
public:
    Actor(StudentWorld* world, int imageID, double x, double y, int dir, int depth);
    // Action to perform for each tick.
    virtual void doSomething() = 0;
    // Is this actor dead?
    bool isDead() const;
    // Mark this actor as dead.
    void setDead();
    // Get this actor's world
    StudentWorld* getWorld() const;
    // If this is an activated object, perform its effect on a (e.g., for an
    // Exit have a use the exit).
    virtual void activateIfAppropriate(Actor* a);
    // If this object uses exits, use the exit.
    virtual void useExitIfAppropriate();
    // If this object can die by falling into a pit or burning, die.
    virtual void dieByFallOrBurnIfAppropriate();
    // If this object can be infected by vomit, get infected.
    virtual void beVomitedOnIfAppropriate();
    // If this object can pick up goodies, pick up g
    virtual void pickUpGoodieIfAppropriate(Goodie* g);
    // Does this object block agent movement?
    virtual bool blocksMovement() const;
    // Does this object block flames?
    virtual bool blocksFlame() const;
    // Does this object trigger landmines only when they're active?
    virtual bool triggersOnlyActiveLandmines() const;
    // Can this object cause a zombie to vomit?
    virtual bool triggersZombieVomit() const;
    // Is this object a threat to citizens?
    virtual bool threatensCitizens() const;
    // Does this object trigger citizens to follow it or flee it?
    virtual bool triggersCitizens() const;
private:
    StudentWorld* m_world;
    bool dead = false;      //all actors start out alive
};
class Wall : public Actor
{
public:
    Wall(StudentWorld* world, double x, double y);
    virtual void doSomething();
    virtual bool blocksMovement() const;
    virtual bool blocksFlame() const;
};
class ActivatingObject : public Actor
{
public:
    ActivatingObject(StudentWorld* world, int imageID, double x, double y, int depth, int dir);
};
class Exit : public ActivatingObject
{
public:
    Exit(StudentWorld* world, double x, double y);
    virtual void doSomething();
    virtual void activateIfAppropriate(Actor* a);
    virtual bool blocksFlame() const;
};
class Pit : public ActivatingObject
{
public:
    Pit(StudentWorld* world, double x, double y);
    virtual void doSomething();
    virtual void activateIfAppropriate(Actor* a);
};
class Flame : public ActivatingObject
{
public:
    Flame(StudentWorld* world, double x, double y, int dir);
    virtual void doSomething();
    virtual void activateIfAppropriate(Actor* a);
private:
    int ticks; //ticks counts number of times it goes through before it leaves the level
};
class Vomit : public ActivatingObject
{
public:
    Vomit(StudentWorld* world, double x, double y, int dir);
    virtual void doSomething();
    virtual void activateIfAppropriate(Actor* a);
private:
    int ticks;
};
class Landmine : public ActivatingObject
{
public:
    Landmine(StudentWorld* world, double x, double y);
    virtual void doSomething();
    virtual void activateIfAppropriate(Actor* a);
    virtual void dieByFallOrBurnIfAppropriate();
    void blowup();
private:
    int ticks;
};
class Goodie : public ActivatingObject
{
public:
    Goodie(StudentWorld* world, int imageID, double x, double y);
    virtual void activateIfAppropriate(Actor* a);
    
    virtual void dieByFallOrBurnIfAppropriate();
    // Have p pick up this goodie.
    virtual void pickUp(Penelope* p) = 0;
};
class VaccineGoodie : public Goodie
{
public:
    VaccineGoodie(StudentWorld* world, double x, double y);
    virtual void doSomething();
    virtual void pickUp(Penelope* p);
    virtual void dieByFallOrBurnIfAppropriate();
private:
    int ticks;
};
class GasCanGoodie : public Goodie
{
public:
    GasCanGoodie(StudentWorld* world, double x, double y);
    virtual void doSomething();
    virtual void pickUp(Penelope* p);
};
class LandmineGoodie : public Goodie
{
public:
    LandmineGoodie(StudentWorld* world, double x, double y);
    virtual void doSomething();
    virtual void pickUp(Penelope* p);
};
class AliveObjects : public Actor
{
public:
    AliveObjects(StudentWorld* world, int imageID, double x, double y, int dir);
    virtual bool blocksMovement() const;
    virtual bool triggersOnlyActiveLandmines() const;
};
class Person : public AliveObjects
{
public:
    Person(StudentWorld* world, int imageID, double x, double y);
    virtual void beVomitedOnIfAppropriate();
    virtual bool triggersZombieVomit() const;
    // Make this human uninfected by vomit.
    void clearInfection();
    // How many ticks since this human was infected by vomit?
    int getInfectionDuration() const;
    bool isInfected() const;
    void increaseInfectionDuration();
private:
    bool infected = false; 
    int infectionDuration= 0;
};
class Penelope : public Person
{
public:
    Penelope(StudentWorld* world, double x, double y);
    virtual void doSomething();
    virtual void useExitIfAppropriate();
    virtual void dieByFallOrBurnIfAppropriate();
    virtual void pickUpGoodieIfAppropriate(Goodie* g);
    virtual bool triggersCitizens();
    // Increase the number of vaccines the object has.
    void increaseVaccines();
    // Increase the number of flame charges the object has.
    void increaseFlameCharges();
    // Increase the number of landmines the object has.
    void increaseLandmines();
    // How many vaccines does the object have?
    int getNumVaccines() const;
    // How many flame charges does the object have?
    int getNumFlameCharges() const;
    // How many landmines does the object have?
    int getNumLandmines() const;
private:
    int vaccines;
    int flames;
    int mines;
};
class Citizen : public Person
{
public:
    Citizen(StudentWorld* world,  double x, double y);
    virtual void doSomething();
    virtual void useExitIfAppropriate();
    virtual void dieByFallOrBurnIfAppropriate();
};
class Zombie : public AliveObjects
{
public:
    Zombie(StudentWorld* world,  double x, double y);
    int getMovementPlan();
    void setMovementPlan(int move);
    void tryToVomit();
    void moveOneStep();
    bool triggersCitizens() const;
    bool threatensCitizens() const;
private:
    int movementPlan;
};
class DumbZombie : public Zombie
{
public:
    DumbZombie(StudentWorld* world,  double x, double y);
    virtual void doSomething();
    virtual void dieByFallOrBurnIfAppropriate();
private:
    int ticks;
};
class SmartZombie : public Zombie
{
public:
    SmartZombie(StudentWorld* world,  double x, double y);
    virtual void doSomething();
    virtual void dieByFallOrBurnIfAppropriate();
private:
    int ticks;
};

#endif // ACTOR_H_
