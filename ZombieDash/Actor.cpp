#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>

// ------------------------Actor implementations--------------------------------

Actor::Actor(StudentWorld* world, int imageID, double x, double y, int dir, int depth):GraphObject(imageID, x, y, dir, depth), m_world(world)
{}
bool Actor:: isDead() const
{
    return dead;
}
void Actor::setDead()
{
    dead= true;
}
StudentWorld* Actor::getWorld() const
{
    return m_world;
}
bool Actor::blocksMovement() const
{
    return false;     // default is that actors do not block movement and can overlap
}
bool Actor::blocksFlame() const
{
    return false;
}
bool Actor::triggersOnlyActiveLandmines() const
{
    return false;
}
bool Actor::triggersZombieVomit() const
{
    return false;
}
bool Actor::threatensCitizens() const
{
    return false;
}
bool Actor::triggersCitizens() const
{
    return false;
}
void Actor::activateIfAppropriate(Actor* a) {return;}
void Actor::useExitIfAppropriate() {return;}
void Actor::dieByFallOrBurnIfAppropriate() {return;}
void Actor::beVomitedOnIfAppropriate() {return;}
void Actor::pickUpGoodieIfAppropriate(Goodie* g) {return;}

//------------------------Wall implementations--------------------------------
Wall::Wall(StudentWorld* world, double x, double y):Actor(world, IID_WALL, x,y, right, 0)
{}
void Wall::doSomething()        // the wall does nothing per tick so this function is empty
{}
bool Wall::blocksMovement() const
{
    return true;
}
bool Wall::blocksFlame() const
{
    return true;
}
//------------------------Activiating Object implementations--------------------------------
ActivatingObject::ActivatingObject(StudentWorld* world, int imageID, double x, double y, int depth, int dir): Actor(world,imageID,x,y,dir,depth)
{}
//------------------------Exit implementations--------------------------------
Exit::Exit(StudentWorld* world, double x, double y):ActivatingObject(world, IID_EXIT, x, y, 1, right)
{}
void Exit::doSomething()
{
    getWorld()->activateOnAppropriateActors(this);
}
void Exit::activateIfAppropriate(Actor* a)
{
    a->useExitIfAppropriate();
}
bool Exit::blocksFlame() const
{
    return true;
}

//------------------------Pit implementations--------------------------------
Pit::Pit(StudentWorld* world, double x, double y):ActivatingObject(world, IID_PIT, x, y, 0, right)
{}
void Pit::doSomething()
{
    getWorld()->activateOnAppropriateActors(this);
}
void Pit::activateIfAppropriate(Actor* a)
{
    a -> dieByFallOrBurnIfAppropriate();
}
//------------------------Flame implementations--------------------------------
Flame::Flame(StudentWorld* world, double x, double y, int dir):ActivatingObject(world, IID_FLAME, x, y, 0, dir), ticks(2)
{}
void Flame::doSomething()
{
    if(isDead())
        return;
    if (ticks==0)
    {
        setDead();
        return;
    }
    else
    {
        getWorld()->activateOnAppropriateActors(this);
        ticks--;
    }
}
void Flame::activateIfAppropriate(Actor* a)
{
    if (!a->isDead())
    {
        a -> dieByFallOrBurnIfAppropriate();
    }
}
//------------------------Vomit implementations--------------------------------

Vomit::Vomit(StudentWorld* world, double x, double y, int dir): ActivatingObject(world, IID_VOMIT, x, y, 0, dir), ticks(2)
{}

void Vomit::doSomething()
{
    if (isDead())
        return;
    if (ticks == 0)
        setDead();
    else
    {
        getWorld()->activateOnAppropriateActors(this);
        ticks--;
    }
    
}
void Vomit::activateIfAppropriate(Actor* a)
{
    if (a->triggersZombieVomit())
        a-> beVomitedOnIfAppropriate();
}
//------------------------Landmine implementations--------------------------------
Landmine::Landmine(StudentWorld* world, double x, double y):ActivatingObject(world, IID_LANDMINE, x,y, 1, right), ticks(30)
{}
void Landmine::doSomething()
{
    if(isDead())
        return;
    if (ticks>0)
    {
        ticks--;
        return;
    }
    else
    {
        getWorld()->activateOnAppropriateActors(this);
    }
    
}
void Landmine::activateIfAppropriate(Actor* a)
{
        if(a->triggersOnlyActiveLandmines())
        {
            a->dieByFallOrBurnIfAppropriate();
            blowup();
        }
}
void Landmine:: dieByFallOrBurnIfAppropriate()
{
    blowup();
}
void Landmine::blowup()
{
    setDead();
    getWorld()-> playSound(SOUND_LANDMINE_EXPLODE);
    if (!getWorld()-> isFlameBlockedAt(getX()+SPRITE_WIDTH, getY()))
        getWorld()->addActor(new Flame(getWorld(), getX()+SPRITE_WIDTH, getY(), up));
    if (!getWorld()-> isFlameBlockedAt(getX()+SPRITE_WIDTH, getY()))
        getWorld()->addActor(new Flame(getWorld(), getX()+SPRITE_WIDTH, getY(), up));
    if (!getWorld()-> isFlameBlockedAt(getX(), getY()))
        getWorld()->addActor(new Flame(getWorld(), getX(), getY(), up));
     getWorld()->addActor(new Flame(getWorld(), getX()+SPRITE_WIDTH, getY()-SPRITE_HEIGHT, up));
     getWorld()->addActor(new Flame(getWorld(), getX()-SPRITE_WIDTH, getY()-SPRITE_HEIGHT, up));
     getWorld()->addActor(new Flame(getWorld(), getX(), getY()-SPRITE_HEIGHT, up));
     getWorld()->addActor(new Flame(getWorld(), getX()+SPRITE_WIDTH, getY()+SPRITE_HEIGHT, up));
     getWorld()->addActor(new Flame(getWorld(), getX()-SPRITE_WIDTH, getY()+SPRITE_HEIGHT, up));
     getWorld()->addActor(new Flame(getWorld(), getX(), getY()+SPRITE_HEIGHT, up));
     getWorld()->addActor(new Pit(getWorld(), getX(), getY()));
}
//------------------------Goodie implementations--------------------------------
Goodie::Goodie(StudentWorld* world, int imageID, double x, double y): ActivatingObject(world, imageID, x, y, 1, right)
{}
void Goodie::activateIfAppropriate(Actor* a)
{
    a -> pickUpGoodieIfAppropriate(this);
}
    
void Goodie::dieByFallOrBurnIfAppropriate()
{
    setDead();
}
//------------------------VaccineGoodie implementations--------------------------------
VaccineGoodie::VaccineGoodie(StudentWorld* world, double x, double y): Goodie(world, IID_VACCINE_GOODIE, x, y), ticks(2)
{}
void VaccineGoodie::doSomething()
{
    getWorld() -> activateOnAppropriateActors(this);
    ticks--;
}
void VaccineGoodie::pickUp(Penelope* p)
{
    p->increaseVaccines();
}
void VaccineGoodie::dieByFallOrBurnIfAppropriate()
{
    if (ticks<=0)
        setDead();
}
//------------------------GasCanGoodie implementations--------------------------------
GasCanGoodie::GasCanGoodie(StudentWorld* world, double x, double y): Goodie(world, IID_GAS_CAN_GOODIE, x, y)
{}
void GasCanGoodie::doSomething()
{
    getWorld() -> activateOnAppropriateActors(this);
}
void GasCanGoodie::pickUp(Penelope* p)
{
    p->increaseFlameCharges();
}
//------------------------LandmineGoodie implementations--------------------------------
LandmineGoodie::LandmineGoodie(StudentWorld* world, double x, double y): Goodie(world, IID_LANDMINE_GOODIE, x, y)
{}
void LandmineGoodie::doSomething()
{
    getWorld() -> activateOnAppropriateActors(this);
}
void LandmineGoodie::pickUp(Penelope* p)
{
    p->increaseLandmines();
}

//------------------------Alive Object implementations--------------------------------
AliveObjects::AliveObjects(StudentWorld* world, int imageID, double x, double y, int dir):Actor(world,imageID,x,y,dir,0)
{}

bool AliveObjects::blocksMovement() const
{
    return true;
}
bool AliveObjects::triggersOnlyActiveLandmines() const
{
    return true;
}
//------------------------Person implementations--------------------------------
Person::Person(StudentWorld* world, int imageID, double x, double y):AliveObjects(world,imageID,x,y,right)
{}
void Person::beVomitedOnIfAppropriate()
{
    infected = true;
    
}
bool Person::triggersZombieVomit() const
{
    return true;
}

// Make this human uninfected by vomit.
void Person::clearInfection()
{
    infected = false;
    infectionDuration = 0;
}
bool Person::isInfected() const
{
    return infected;
}
// How many ticks since this human was infected by vomit?
int Person::getInfectionDuration() const
{
    return infectionDuration;
}
void Person::increaseInfectionDuration()
{
    infectionDuration++;
}
//------------------------Penelope implementations--------------------------------
Penelope::Penelope(StudentWorld* world, double x, double y): Person(world, IID_PLAYER, x, y), vaccines(0), flames(0), mines(0)
{}
void Penelope::doSomething()
{
    if (isDead() == true)
    {
        return;
    }
    if (isInfected())
    {
        increaseInfectionDuration();
        if (getInfectionDuration()>=500)
        {
            setDead();
            getWorld()->addActor(new DumbZombie(getWorld(), getX(), getY()));
            getWorld()->playSound(SOUND_PLAYER_DIE);
            return;
        }
    }
    //GET input from user to determine move
    int ch;
    if (getWorld()->getKey(ch))
    {
        // user hit a key during this tick!
        switch (ch)
        {
            case KEY_PRESS_LEFT:
                setDirection(left);
                if (getWorld() -> isAliveObjectMovementBlockedAt(getX()-4, getY(), this))
                    break;
                moveTo(getX()-4, getY());
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                if (getWorld() -> isAliveObjectMovementBlockedAt(getX()+4, getY(), this))
                    break;
                moveTo(getX()+4, getY());
                break;
            case KEY_PRESS_UP:
                setDirection(up);
                if (getWorld() -> isAliveObjectMovementBlockedAt(getX(), getY()+4, this))
                    break;
                moveTo(getX(), getY()+4);
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                if (getWorld() -> isAliveObjectMovementBlockedAt(getX(), getY()-4, this))
                    break;
                moveTo(getX(), getY()-4);
                break;
                
            case KEY_PRESS_ENTER:
                if (vaccines>0)
                {
                    clearInfection();
                    vaccines--;
                }
                break;
            case KEY_PRESS_TAB:
                if (mines>0)
                {
                    getWorld()-> addActor(new Landmine(getWorld(),getX(), getY()));
                    mines--;
                }
                break;
                
            case KEY_PRESS_SPACE:
                if (flames<=0)
                    break;
                flames--;
                getWorld()->playSound(SOUND_PLAYER_FIRE);
                bool breakLoop = false;
                int i = 1; // counting the flames
                while (!breakLoop && i<4)
                {
                    if(getDirection()==up)
                    {
                        if (!getWorld()->isFlameBlockedAt(getX(), getY() + i *SPRITE_HEIGHT))
                        {
                            getWorld()->addActor(new Flame(getWorld(), getX(), getY()+ i *SPRITE_HEIGHT, getDirection()));
                            i++;
                        }
                        else
                            breakLoop = true;
                    }
                    if(getDirection()==down)
                    {
                        if (!getWorld()->isFlameBlockedAt(getX(), getY() - i *SPRITE_HEIGHT))
                        {
                            getWorld()->addActor(new Flame(getWorld(), getX(), getY()- i *SPRITE_HEIGHT, getDirection()));
                            i++;
                        }
                        else
                            breakLoop = true;
                    }
                    if(getDirection()==right)
                    {
                        if (!getWorld()->isFlameBlockedAt(getX()+ i*SPRITE_WIDTH, getY()))
                        {
                            getWorld()->addActor(new Flame(getWorld(), getX()+ i*SPRITE_WIDTH, getY(), getDirection()));
                            i++;
                        }
                        else
                            breakLoop = true;
                    }
                    if(getDirection()==left)
                    {
                        if (!getWorld()->isFlameBlockedAt(getX()- i*SPRITE_WIDTH, getY()))
                        {
                            getWorld()->addActor(new Flame(getWorld(), getX()- i*SPRITE_WIDTH, getY(), getDirection()));
                            i++;
                        }
                        else
                            breakLoop = true;
                    }
                }
                break;
            
                
            }
        
    }
}


void Penelope::useExitIfAppropriate()
{
    if (getWorld()->getCitizenNum() <= 0)
        getWorld()-> recordLevelFinishedIfAllCitizensGone();
}
void Penelope::dieByFallOrBurnIfAppropriate()
{
    setDead();
    getWorld()->playSound(SOUND_PLAYER_DIE);
}
void Penelope::pickUpGoodieIfAppropriate(Goodie* g)
{
    getWorld()->playSound(SOUND_GOT_GOODIE);
    getWorld()->increaseScore(50);
    g->pickUp(this);
    g->setDead();
}
void Penelope::increaseVaccines()
{
    vaccines++;
}
void Penelope::increaseFlameCharges()
{
    flames+=5;
}
void Penelope::increaseLandmines()
{
    mines+=2;
}
int Penelope::getNumVaccines() const
{
    return vaccines;
}
int Penelope::getNumFlameCharges() const
{
    return flames;
}
int Penelope::getNumLandmines() const
{
    return mines;
}
bool Penelope::triggersCitizens()
{
    return true;
}
//------------------------Citizen implementations--------------------------------
Citizen::Citizen(StudentWorld* world, double x, double y): Person(world, IID_CITIZEN, x, y)
{}
void Citizen::doSomething()
{
    if (isDead() == true)
    {
        return;
    }
    if (isInfected())
    {
        increaseInfectionDuration();
        if (getInfectionDuration()>=500)
        {
            setDead();
            getWorld()->recordCitizenGone();
            getWorld()->playSound(SOUND_ZOMBIE_BORN);
            getWorld()-> increaseScore(-1000);
            int random10 = rand()%10;
            if (random10<3)
                getWorld()->addActor(new SmartZombie(getWorld(),getX(),getY()));
            else
                getWorld()->addActor(new DumbZombie(getWorld(),getX(),getY()));
            return;
        }
    }
    double dist_p = sqrt(pow(getWorld()->getPenelope()->getX() - getX(),2) + pow(getWorld()->getPenelope()->getY() - getY(),2));
    double dist_z;
    double zombieX;
    double zombieY;
    if(getWorld()-> locateNearestCitizenThreat(getX(), getX(), zombieX, zombieY, dist_z) == false||dist_p < dist_z || dist_p<=80)
    {
        if (getX() == getWorld()->getPenelope()->getX())
        {
            if (getY()<getWorld()->getPenelope()-> getY() && !getWorld() ->    isAliveObjectMovementBlockedAt(getX(), getY()+2, this))
            {
                setDirection(up);
                moveTo(getX(), getY()+2);
                return;
            }
            else if (getY()>getWorld()->getPenelope()->getY() && !getWorld() ->     isAliveObjectMovementBlockedAt(getX(), getY()-2, this))
            {
                setDirection(down);
                moveTo(getX(), getY()-2);
                return;
            }
        }
        else if (getY() == getWorld()->getPenelope()->getY())
        {
            if (getX()<getWorld()->getPenelope()->getX() && !getWorld() ->     isAliveObjectMovementBlockedAt(getX()+2, getY(), this))
            {
                setDirection(right);
                moveTo(getX()+2, getY());
                return;
            }
            else if (getX()>getWorld()->getPenelope()->getX() && !getWorld() ->  isAliveObjectMovementBlockedAt(getX()-2, getY(), this))
            {
                setDirection(left);
                moveTo(getX()-2, getY());
                return;
            }
        }
        if(getY() != getWorld()->getPenelope()->getY()&& getX() != getWorld()->getPenelope()->getX())
        {
            int random = rand()%2;
            if (random == 0) // case vertical
            {
                if ((abs(getY()+2-getWorld()->getPenelope()->getY()) < abs(getY()-2-getWorld()->getPenelope()->getY())) && !getWorld()-> isAliveObjectMovementBlockedAt(getX(), getY()+2, this))
                {
                    setDirection(up);
                    moveTo(getX(), getY()+2);
                    return;
                }
                else if ((abs(getY()+2-getWorld()->getPenelope()->getY()) > abs(getY()-2-getWorld()->getPenelope()->getY())) && !getWorld()-> isAliveObjectMovementBlockedAt(getX(), getY()-2, this))
                {
                    setDirection(down);
                    moveTo(getX(), getY()-2);
                    return;
                }
            }
            else if (random==1)
            {
                if ((abs(getX()+2-getWorld()->getPenelope()->getX()) < abs(getX()-2-getWorld()->getPenelope()->getX())) && !getWorld()-> isAliveObjectMovementBlockedAt(getX()+2, getY(), this))
                {
                    setDirection(right);
                    moveTo(getX()+2, getY());
                    return;
                }
                else if ((abs(getX()+2-getWorld()->getPenelope()->getX()) > abs(getX()-2-getWorld()->getPenelope()->getX())) && !getWorld()-> isAliveObjectMovementBlockedAt(getX()-2, getY(), this))
                {
                    setDirection(left);
                    moveTo(getX()-2, getY());
                    return;
                }
            }
        }
    }
    else if (dist_z<80)
    {
        double tempShortestDist;
        double shortestDist= getWorld()->locateNearestCitizenThreat(getX(), getY(), zombieX, zombieY, tempShortestDist);
        Direction moveToDir = 100;
        if (!getWorld() ->    isAliveObjectMovementBlockedAt(getX(), getY()+2, this))
        {
            getWorld()->locateNearestCitizenThreat(getX(), getY()+2, zombieX, zombieY, tempShortestDist);
            if (tempShortestDist>shortestDist)
            {
                shortestDist=tempShortestDist;
                moveToDir=up;
            }
        }
        if (!getWorld() ->    isAliveObjectMovementBlockedAt(getX(), getY()-2, this))
        {
            getWorld()->locateNearestCitizenThreat(getX(), getY()-2, zombieX, zombieY, tempShortestDist);
            if (tempShortestDist>shortestDist)
            {
                shortestDist=tempShortestDist;
                moveToDir=down;
            }
        }
        if (!getWorld() ->    isAliveObjectMovementBlockedAt(getX()+2, getY(), this))
        {
            getWorld()->locateNearestCitizenThreat(getX()+2, getY(), zombieX, zombieY, tempShortestDist);
            if (tempShortestDist>shortestDist)
            {
                shortestDist=tempShortestDist;
                moveToDir=right;
            }
        }
        if (!getWorld() ->    isAliveObjectMovementBlockedAt(getX()-2, getY(), this))
        {
            getWorld()->locateNearestCitizenThreat(getX()-2, getY(), zombieX, zombieY, tempShortestDist);
            if (tempShortestDist>shortestDist)
            {
                shortestDist=tempShortestDist;
                moveToDir=left;
            }
        }
            switch (moveToDir)
            {
                case up:
                    setDirection(up);
                    moveTo(getX(), getY()+2);
                    return;
                    break;
                case down:
                    setDirection(down);
                    moveTo(getX(), getY()-2);
                    return;
                    break;
                case right:
                    setDirection(right);
                    moveTo(getX()+2, getY());
                    return;
                    break;
                case left:
                    setDirection(left);
                    moveTo(getX()-2, getY());
                    return;
                    break;
                default:
                    return;
            }
   
    }
}
void Citizen::useExitIfAppropriate()
{
    getWorld()->increaseScore(500);
    getWorld()-> recordCitizenGone();
    getWorld()->playSound(SOUND_CITIZEN_SAVED);
    setDead();
}
void Citizen::dieByFallOrBurnIfAppropriate()
{
    setDead();
    getWorld()->increaseScore(-1000);
    getWorld()-> recordCitizenGone();
    getWorld()->playSound(SOUND_CITIZEN_DIE);

}

//------------------------Zombie implementations--------------------------------


Zombie::Zombie(StudentWorld* world, double x, double y): AliveObjects(world, IID_ZOMBIE, x, y, right), movementPlan(0)
{}
int Zombie::getMovementPlan()
{
    return movementPlan;
}
void Zombie::setMovementPlan(int move)
{
    movementPlan = move;
}
void Zombie::tryToVomit()   // if the location in front of it contains a vomit trigger add vomit in that location.
{
    switch (getDirection())
    {
        case right:
            if (getWorld()->isZombieVomitTriggerAt(getX()+SPRITE_WIDTH, getY()) && (rand()%3)==0)
                getWorld()->addActor(new Vomit(getWorld(), getX()+SPRITE_WIDTH, getY(), right));
            break;
        case left:
            if (getWorld()->isZombieVomitTriggerAt(getX()-SPRITE_WIDTH, getY()) && (rand()%3)==0)
                getWorld()->addActor(new Vomit(getWorld(), getX()-SPRITE_WIDTH, getY(), right));
            break;
        case up:
            if (getWorld()->isZombieVomitTriggerAt(getX(), getY()+SPRITE_HEIGHT) && (rand()%3)==0)
                getWorld()->addActor(new Vomit(getWorld(), getX(), getY()+SPRITE_HEIGHT, right));
            break;
        case down:
            if (getWorld()->isZombieVomitTriggerAt(getX(), getY()-SPRITE_HEIGHT) && (rand()%3)==0)
                getWorld()->addActor(new Vomit(getWorld(), getX(), getY()-SPRITE_HEIGHT, right));
            break;
    }
}
void Zombie::moveOneStep()  // gets direction of the zombie, checks if it can move. If yes then make the move, otherwise set movementPlan to 0
{
    if (getDirection()==left)
    {
        if (getWorld() -> isAliveObjectMovementBlockedAt(getX()-1, getY(), this))
            setMovementPlan(0);
        else
        {
            moveTo(getX()-1, getY());
            setMovementPlan(getMovementPlan()-1);
        }
    }
    if (getDirection()==right)
    {
        if (getWorld() -> isAliveObjectMovementBlockedAt(getX()+1, getY(), this))
            setMovementPlan(0);
        else
        {
            moveTo(getX()+1, getY());
            setMovementPlan(getMovementPlan()-1);
        }
    }
    if (getDirection()==up)
    {
        if (getWorld() -> isAliveObjectMovementBlockedAt(getX(), getY()+1, this))
            setMovementPlan(0);
        else
        {
            moveTo(getX(), getY()+1);
            setMovementPlan(getMovementPlan()-1);
        }
    }
    if (getDirection()==down)
    {
        if (getWorld() -> isAliveObjectMovementBlockedAt(getX(), getY()-1, this))
            setMovementPlan(0);
        else
        {
            moveTo(getX(), getY()-1);
            setMovementPlan(getMovementPlan()-1);
        }
        
    }
}
bool Zombie::triggersCitizens() const
{
    return true;
}
bool Zombie::threatensCitizens() const
{
    return true;
}
//------------------------DumbZombie implementations--------------------------------
DumbZombie::DumbZombie(StudentWorld* world,  double x, double y): Zombie(world,x,y), ticks(0)
{}
void DumbZombie::doSomething()
{
    
    if (isDead())
        return;
    if (ticks%2==0)
    {
        ticks++;
        return;
    }
    tryToVomit();
    if (getMovementPlan()<=0)
    {
        setMovementPlan((rand()%8)+3);
        setDirection((rand()%4)*90);
    }
    moveOneStep();
    ticks++;
}
void DumbZombie::dieByFallOrBurnIfAppropriate()
{
    setDead();
    getWorld()->increaseScore(1000);
    getWorld()->playSound(SOUND_ZOMBIE_DIE);
    if ((rand()%10)==0)
        getWorld()->addActor(new VaccineGoodie(getWorld(), getX(), getY()));
}
//------------------------SmartZombie implementations--------------------------------
SmartZombie::SmartZombie(StudentWorld* world,  double x, double y): Zombie(world,x,y), ticks(0)
{}
void SmartZombie::doSomething()
{
    if (isDead())
        return;
    if (ticks%2==0)
    {
        ticks++;
        return;
    }
    tryToVomit();
    moveOneStep();
    setMovementPlan(getMovementPlan()-1);
    double distance = 0;
    double otherX=0;
    double otherY=0;
    if (getMovementPlan()<=0)
    {
        setMovementPlan((rand()%8)+3);
        
        if(getWorld()-> locateNearestVomitTrigger(getX(), getY(), otherX, otherY, distance))
        {
            if (distance>80)
                setDirection((rand()%4)*90);
            else
            {
                if (getX() == otherX)
                {
                    if (getY()<otherY)
                    {
                        setDirection(up);
                    }
                    else if (getY()>otherY)
                    {
                        setDirection(down);
                    }
                }
                else if (getY() == otherY)
                {
                    if (getX()<otherX)
                    {
                        setDirection(right);
                    }
                    else if (getX()>otherX)
                    {
                        setDirection(left);
                    }
                }
                
            }
            if(getY() != otherY && getX() != otherX)
            {
                int random = rand()%2;
                if (random == 0) // case vertical
                {
                    if (getY()<otherY)
                    {
                        setDirection(up);
                    }
                    else if (getY()>otherY)
                    {
                        setDirection(down);
                    }
                }
                if (random==1)
                {
                    if (getX()<otherX)
                    {
                        setDirection(right);
                    }
                    else if (getX()>otherX)
                    {
                        setDirection(left);
                    }
                }
            }
        }
       
    }
    
    ticks++;
}
void SmartZombie::dieByFallOrBurnIfAppropriate()
{
    setDead();
    getWorld()->increaseScore(1000);
    getWorld()->playSound(SOUND_ZOMBIE_DIE);
}
