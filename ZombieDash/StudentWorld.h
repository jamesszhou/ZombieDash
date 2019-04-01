#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_
#include <vector>
#include "GameWorld.h"
#include "Actor.h"
#include <string>
using namespace std;
class Actor;
class Penelope;
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    //loads in the level and creates all the Actors in their specified locations
    void loadLevel();
    // Add an actor to the world.
    void addActor(Actor* a);
    // Record that one more citizen on the current level is gone (exited,
    // died, or turned into a zombie).
    void recordCitizenGone();
    // Indicate that the player has finished the level if all citizens
    // are gone.
    void recordLevelFinishedIfAllCitizensGone();
    // For each actor overlapping a, activate a if appropriate.
    void activateOnAppropriateActors(Actor* a);
    // Is an agent blocked from moving to the indicated location?
    bool isAliveObjectMovementBlockedAt(double x, double y, Actor* self) const;
    // Is creation of a flame blocked at the indicated location?
    bool isFlameBlockedAt(double x, double y) const;
    // Is there something at the indicated location that might cause a
    // zombie to vomit (i.e., a human)?
    bool isZombieVomitTriggerAt(double x, double y) const;
    // Return true if there is a living human, otherwise false.  If true,
    // otherX, otherY, and distance will be set to the location and distance
    // of the human nearest to (x,y).
    bool locateNearestVomitTrigger(double x, double y, double& otherX, double& otherY,
                                   double& distance);
    // Return true if there is a living zombie or Penelope, otherwise false.
    // If true, otherX, otherY, and distance will be set to the location and
    // distance of the one nearest to (x,y), and isThreat will be set to true
    // if it's a zombie, false if a Penelope.
    bool locateNearestCitizenTrigger(double x, double y, double& otherX, double& otherY,
                                     double& distance, bool& isThreat) const;
    // Return true if there is a living zombie, false otherwise.  If true,
    // otherX, otherY and distance will be set to the location and distance
    // of the one nearest to (x,y).
    bool locateNearestCitizenThreat(double x, double y, double& otherX, double& otherY,
                                    double& distance) const;
    // make sure that after all citizens leave move understands when it needs to advance level
    void setLevelAdvance(bool advance);
    // get penelope from the world so that other actos can see Penelope's info
    Penelope* getPenelope();
    // Return number of citizens
    int getCitizenNum();
    
private:
    vector<Actor*> m_Actors;
    Penelope* m_Penelope;
    bool levelAdvance = false;
    bool won = false;
    int numCitizens = 0;
};

#endif // STUDENTWORLD_H_
