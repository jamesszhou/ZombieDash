#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    if (won)
        return GWSTATUS_PLAYER_WON;
    else
    {
        loadLevel();
        return GWSTATUS_CONTINUE_GAME;
    }
}

int StudentWorld::move()
{
    /*
    int StudentWorld::move()
    {
        // The term "actors" refers to all zombies, Penelope, goodies, // pits, flames, vomit, landmines, etc.
        // Give each actor a chance to do something, including Penelope for each of the actors in the game world
        {
            if (actor[i] is still alive) {
                // tell each actor to do something (e.g. move) actor[i]->doSomething();
                if (Penelope died during this tick) return GWSTATUS_PLAYER_DIED;
                if (Penelope completed the current level) return GWSTATUS_FINISHED_LEVEL;
            } }
        // Remove newly-dead actors after each tick Remove and delete dead game objects
        // Update the game status line
        Update Display Text // update the score/lives/level text at screen top
        // the player hasn’t completed the current level and hasn’t died, so // continue playing the current level
        return GWSTATUS_CONTINUE_GAME;
    }
     */

    m_Penelope-> doSomething();
    
    for (int i = 0; i <m_Actors.size(); i++)
    {
        if (!m_Actors[i]->isDead())
        {
            m_Actors[i] -> doSomething();
            if (m_Penelope->isDead())
            {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            if (levelAdvance)
            {
                playSound(SOUND_LEVEL_FINISHED);
                if (getLevel() == 6)
                {
                    won = true;
                }
                setLevelAdvance(false);
                return GWSTATUS_FINISHED_LEVEL;
                
            }
        }
    }
    if (levelAdvance)
    {
        playSound(SOUND_LEVEL_FINISHED);
        if (getLevel() == 6)
        {
            won = true;
        }
        setLevelAdvance(false);
        return GWSTATUS_FINISHED_LEVEL;
        
    }
    //remove Dead Actors
    vector<Actor*>::iterator it = m_Actors.begin();
    while ( it != m_Actors.end())
    {
        if ((*it)->isDead())
        {
            delete (*it);
            it= m_Actors.erase(it);
        }
        else
            it++;
    }
    //Update Score
    
    
//Score: 001200 Level: 4 Lives: 2 Vaccines: 4 Flames: 19 Mines: 3 Infected: 0
    
    ostringstream oss;
    if (getScore()<0)
        oss<<"Score: -"<< setw(5) << setfill('0')<<abs(getScore())<<" ";
    else
        oss<<"Score: "<< setw(6) << setfill('0')<<getScore()<<" ";
    oss<<"Level: "<< getLevel()<<" ";
    oss<<"Lives: "<< getLives()<<" ";
    oss<<"Vaccines: "<< m_Penelope->getNumVaccines()<<" ";
    oss<<"Flames: "<< m_Penelope->getNumFlameCharges()<<" ";
    oss<<"Mines: "<< m_Penelope->getNumLandmines()<<" ";
    oss<<"Infected: "<< m_Penelope-> getInfectionDuration();
    setGameStatText(oss.str());
    
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    while (!m_Actors.empty())
    {
        delete m_Actors[m_Actors.size()-1];
        m_Actors.pop_back();
    }
    if (!won)
    {
        delete m_Penelope;
        m_Penelope= nullptr;
    }
}

void StudentWorld::loadLevel() {
    Level lev(assetPath());
    ostringstream oss;
    oss<<"level0"<<getLevel()<<".txt";
    string levelFile = oss.str();
    Level::LoadResult result = lev.loadLevel(levelFile);
    if (result == Level::load_fail_file_not_found)
        won = true; // player wins the game
    else if (result == Level::load_fail_bad_format)
            cerr << "Your level was improperly formatted" << endl;
    else if (result == Level::load_success)
            {
                cerr << "Successfully loaded level" << endl;
                for(int i = 0; i<LEVEL_WIDTH; i++)
                {
                    for (int j = 0; j<LEVEL_HEIGHT; j++)
                    {
                        Level::MazeEntry ge = lev.getContentsOf(i,j); // level_x=5, level_y=10
                        switch (ge) // so x=80 and y=160
                        {
                            case Level::empty:
                                    break;
                            case Level::smart_zombie:
                                m_Actors.push_back(new SmartZombie(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                                break;
                            case Level::dumb_zombie:
                                m_Actors.push_back(new DumbZombie(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                                break;
                            
                            case Level::player:
                                m_Penelope = new Penelope(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j);
                                break;
                                
                            case Level::citizen:
                                m_Actors.push_back(new Citizen(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                                numCitizens++;
                                break;
                            
                            case Level::exit:
                                m_Actors.push_back(new Exit(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                                break;
                                
                            case Level::pit:
                                m_Actors.push_back(new Pit(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                                break;
                                
                            case Level::vaccine_goodie:
                                m_Actors.push_back(new VaccineGoodie(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                                break;
                                
                            case Level::gas_can_goodie: 
                                m_Actors.push_back(new GasCanGoodie(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                                break;
                                
                            case Level::landmine_goodie:
                                m_Actors.push_back(new LandmineGoodie(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                                break;
                                
                            case Level::wall:
                                m_Actors.push_back(new Wall(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                                break;
                                
                            default:
                                break;
                        }
                    }
                }
            }
}
void StudentWorld::addActor(Actor* a)
{
    m_Actors.push_back(a);
}

void StudentWorld::recordCitizenGone()
{
    numCitizens--;
}
void StudentWorld::recordLevelFinishedIfAllCitizensGone()
{
    setLevelAdvance(true);
}
void StudentWorld::activateOnAppropriateActors(Actor* a)
{
    for (int i=0; i< m_Actors.size(); i++)
    {
        if ( m_Actors[i] != a && !m_Actors[i]->isDead())
        {
            if((((m_Actors[i]->getX() - a->getX())*(m_Actors[i]->getX() - a->getX())) + (m_Actors[i]->getY() - a->getY())*(m_Actors[i]->getY() - a->getY()))<= 100)
            {
                
                a->activateIfAppropriate(m_Actors[i]);
            }
        }
    }
    if((((m_Penelope->getX() - a->getX())*(m_Penelope->getX() - a->getX())) + (m_Penelope->getY() - a->getY())*(m_Penelope->getY() - a->getY()))<= 100)
    {
        a->activateIfAppropriate(m_Penelope);
    }
     
}


bool StudentWorld::isAliveObjectMovementBlockedAt(double x, double y, Actor* self) const
{
    for (int i=0; i< m_Actors.size(); i++)
    {
        if (m_Actors[i]==self)
            continue;
        else if (((m_Actors[i]->getX() <= x && m_Actors[i]->getX()+(SPRITE_WIDTH-1) >= x &&
                   m_Actors[i]->getY() <= y && m_Actors[i]->getY()+(SPRITE_HEIGHT-1) >= y) ||
             // bottom left corner of alive object cannot enter the Actor's box
                  (m_Actors[i]->getX()  <= x+SPRITE_WIDTH-1 && m_Actors[i]->getX()+(SPRITE_WIDTH-1) >= x+SPRITE_WIDTH-1 &&
                   m_Actors[i]->getY() <= y && m_Actors[i]->getY()+(SPRITE_HEIGHT-1) >= y) ||
            //bottom right corner of alive object cannot enter the Actor's box
                  (m_Actors[i]->getX()  <= x && m_Actors[i]->getX()+(SPRITE_WIDTH-1) >= x &&
                   m_Actors[i]->getY() <= y+SPRITE_HEIGHT-1 && m_Actors[i]->getY()+(SPRITE_HEIGHT-1) >= y+SPRITE_HEIGHT-1) ||
             //top left corner of alive object cannot enter the Actor's box
                  (m_Actors[i]->getX()  <= x+SPRITE_WIDTH-1 && m_Actors[i]->getX()+(SPRITE_WIDTH-1) >= x+SPRITE_WIDTH-1 &&
                   m_Actors[i]->getY() <= y+SPRITE_HEIGHT-1 && m_Actors[i]->getY()+(SPRITE_HEIGHT-1) >= y+SPRITE_HEIGHT-1))
             //top right corner of alive object cannot enter the Actor's box
            && m_Actors[i]->blocksMovement() == true)
        {
            return true;
        }
    }
    
    if (((m_Penelope->getX() <= x && m_Penelope->getX()+(SPRITE_WIDTH-1) >= x &&
          m_Penelope->getY() <= y && m_Penelope->getY()+(SPRITE_HEIGHT-1) >= y) ||
         // bottom left corner of alive object cannot enter the Actor's box
         (m_Penelope->getX()  <= x+SPRITE_WIDTH-1 && m_Penelope->getX()+(SPRITE_WIDTH-1) >= x+SPRITE_WIDTH-1 &&
          m_Penelope->getY() <= y && m_Penelope->getY()+(SPRITE_HEIGHT-1) >= y) ||
         //bottom right corner of alive object cannot enter the Actor's box
         (m_Penelope->getX()  <= x && m_Penelope->getX()+(SPRITE_WIDTH-1) >= x &&
          m_Penelope->getY() <= y+SPRITE_HEIGHT-1 && m_Penelope->getY()+(SPRITE_HEIGHT-1) >= y+SPRITE_HEIGHT-1) ||
         //top left corner of alive object cannot enter the Actor's box
         (m_Penelope->getX()  <= x+SPRITE_WIDTH-1 && m_Penelope->getX()+(SPRITE_WIDTH-1) >= x+SPRITE_WIDTH-1 &&
          m_Penelope->getY() <= y+SPRITE_HEIGHT-1 && m_Penelope->getY()+(SPRITE_HEIGHT-1) >= y+SPRITE_HEIGHT-1))
        //top right corner of alive object cannot enter the Actor's box
        && m_Penelope->blocksMovement() == true && m_Penelope!=self)
    {
        return true;
    }
    return false;
}
bool StudentWorld::isFlameBlockedAt(double x, double y) const
{
    for (int i=0; i< m_Actors.size(); i++)
    {
        if (((m_Actors[i]->getX() <= x && m_Actors[i]->getX()+(SPRITE_WIDTH-1) >= x &&
              m_Actors[i]->getY() <= y && m_Actors[i]->getY()+(SPRITE_HEIGHT-1) >= y) ||
             // bottom left corner of alive object cannot enter the Actor's box
             (m_Actors[i]->getX()  <= x+SPRITE_WIDTH-1 && m_Actors[i]->getX()+(SPRITE_WIDTH-1) >= x+SPRITE_WIDTH-1 &&
              m_Actors[i]->getY() <= y && m_Actors[i]->getY()+(SPRITE_HEIGHT-1) >= y) ||
             //bottom right corner of alive object cannot enter the Actor's box
             (m_Actors[i]->getX()  <= x && m_Actors[i]->getX()+(SPRITE_WIDTH-1) >= x &&
              m_Actors[i]->getY() <= y+SPRITE_HEIGHT-1 && m_Actors[i]->getY()+(SPRITE_HEIGHT-1) >= y+SPRITE_HEIGHT-1) ||
             //top left corner of alive object cannot enter the Actor's box
             (m_Actors[i]->getX()  <= x+SPRITE_WIDTH-1 && m_Actors[i]->getX()+(SPRITE_WIDTH-1) >= x+SPRITE_WIDTH-1 &&
              m_Actors[i]->getY() <= y+SPRITE_HEIGHT-1 && m_Actors[i]->getY()+(SPRITE_HEIGHT-1) >= y+SPRITE_HEIGHT-1))
            //top right corner of alive object cannot enter the Actor's box
            && m_Actors[i]->blocksFlame() == true)
        {
            return true;
        }
    }
    return false;
}

bool StudentWorld::isZombieVomitTriggerAt(double x, double y) const
{
    for (int i=0; i< m_Actors.size(); i++)
    {
        if (((m_Actors[i]->getX() <= x && m_Actors[i]->getX()+(SPRITE_WIDTH-1) >= x &&
              m_Actors[i]->getY() <= y && m_Actors[i]->getY()+(SPRITE_HEIGHT-1) >= y) ||
             // bottom left corner of alive object cannot enter the Actor's box
             (m_Actors[i]->getX()  <= x+SPRITE_WIDTH-1 && m_Actors[i]->getX()+(SPRITE_WIDTH-1) >= x+SPRITE_WIDTH-1 &&
              m_Actors[i]->getY() <= y && m_Actors[i]->getY()+(SPRITE_HEIGHT-1) >= y) ||
             //bottom right corner of alive object cannot enter the Actor's box
             (m_Actors[i]->getX()  <= x && m_Actors[i]->getX()+(SPRITE_WIDTH-1) >= x &&
              m_Actors[i]->getY() <= y+SPRITE_HEIGHT-1 && m_Actors[i]->getY()+(SPRITE_HEIGHT-1) >= y+SPRITE_HEIGHT-1) ||
             //top left corner of alive object cannot enter the Actor's box
             (m_Actors[i]->getX()  <= x+SPRITE_WIDTH-1 && m_Actors[i]->getX()+(SPRITE_WIDTH-1) >= x+SPRITE_WIDTH-1 &&
              m_Actors[i]->getY() <= y+SPRITE_HEIGHT-1 && m_Actors[i]->getY()+(SPRITE_HEIGHT-1) >= y+SPRITE_HEIGHT-1))
            //top right corner of alive object cannot enter the Actor's box
            && m_Actors[i]->triggersZombieVomit() == true)
        {
            return true;
        }
    }
    if (((m_Penelope->getX() <= x && m_Penelope->getX()+(SPRITE_WIDTH-1) >= x &&
          m_Penelope->getY() <= y && m_Penelope->getY()+(SPRITE_HEIGHT-1) >= y) ||
         // bottom left corner of alive object cannot enter the Actor's box
         (m_Penelope->getX()  <= x+SPRITE_WIDTH-1 && m_Penelope->getX()+(SPRITE_WIDTH-1) >= x+SPRITE_WIDTH-1 &&
          m_Penelope->getY() <= y && m_Penelope->getY()+(SPRITE_HEIGHT-1) >= y) ||
         //bottom right corner of alive object cannot enter the Actor's box
         (m_Penelope->getX()  <= x && m_Penelope->getX()+(SPRITE_WIDTH-1) >= x &&
          m_Penelope->getY() <= y+SPRITE_HEIGHT-1 && m_Penelope->getY()+(SPRITE_HEIGHT-1) >= y+SPRITE_HEIGHT-1) ||
         //top left corner of alive object cannot enter the Actor's box
         (m_Penelope->getX()  <= x+SPRITE_WIDTH-1 && m_Penelope->getX()+(SPRITE_WIDTH-1) >= x+SPRITE_WIDTH-1 &&
          m_Penelope->getY() <= y+SPRITE_HEIGHT-1 && m_Penelope->getY()+(SPRITE_HEIGHT-1) >= y+SPRITE_HEIGHT-1))
        //top right corner of alive object cannot enter the Actor's box
        && m_Penelope->triggersZombieVomit() == true)
        return true;
    return false;
}
bool StudentWorld::locateNearestVomitTrigger(double x, double y, double& otherX, double& otherY, double& distance)
{
    double newDistance = sqrt(pow(x - m_Penelope->getX(),2) + pow(y - m_Penelope->getY(),2));
    otherX = m_Penelope->getX();
    otherY = m_Penelope->getY();
    for (int i=0; i< m_Actors.size(); i++)
    {
        if (m_Actors[i]->triggersZombieVomit() == true)
        {
            newDistance = sqrt(pow(x - m_Actors[i]->getX(),2) + pow(y - m_Actors[i]->getY(),2));
            if (newDistance<distance)
            {
                distance=newDistance;
                otherX = m_Actors[i]->getX();
                otherY = m_Actors[i]->getY();
            }
        }
    }
    return true;
}

// Return true if there is a living zombie or Penelope, otherwise false.
// If true, otherX, otherY, and distance will be set to the location and
// distance of the one nearest to (x,y), and isThreat will be set to true
// if it's a zombie, false if a Penelope.
bool StudentWorld::locateNearestCitizenTrigger(double x, double y, double& otherX, double& otherY,
                                 double& distance, bool& isThreat) const
{
    double newDistance = sqrt(pow(x - m_Penelope->getX(),2) + pow(y - m_Penelope->getY(),2));
    otherX = m_Penelope->getX();
    otherY = m_Penelope->getY();
    isThreat = false;
    for (int i=0; i< m_Actors.size(); i++)
    {
        if (m_Actors[i]->triggersCitizens() == true)
        {
            newDistance = sqrt(pow(x - m_Actors[i]->getX(),2) + pow(y - m_Actors[i]->getY(),2));
            if (newDistance<distance)
            {
                distance=newDistance;
                otherX = m_Actors[i]->getX();
                otherY = m_Actors[i]->getY();
                if (m_Actors[i]->threatensCitizens())
                    isThreat = true;
                else
                    isThreat = false;
            }
        }
    }
    return true;
}
// Return true if there is a living zombie, false otherwise.  If true,
// otherX, otherY and distance will be set to the location and distance
// of the one nearest to (x,y).
bool StudentWorld::locateNearestCitizenThreat(double x, double y, double& otherX, double& otherY,
                                double& distance) const
{
    bool isAnyoneAlive= false;
    double newDistance= 100000000;
    for (int i=0; i< m_Actors.size(); i++)
    {
        if (m_Actors[i]->threatensCitizens() == true)
        {
            if (!m_Actors[i]->isDead())
                isAnyoneAlive= true;
            newDistance = sqrt(pow(x - m_Actors[i]->getX(),2) + pow(y - m_Actors[i]->getY(),2));
            if (newDistance<distance)
            {
                distance=newDistance;
                otherX = m_Actors[i]->getX();
                otherY = m_Actors[i]->getY();
            }
        }
    }
    return isAnyoneAlive;
}
// make sure that after all citizens leave move understands when it needs to advance level
void StudentWorld::setLevelAdvance(bool advance)
{
    levelAdvance = advance;
}
Penelope* StudentWorld::getPenelope()
{
    return m_Penelope;
}
int StudentWorld::getCitizenNum()
{
    return numCitizens;
}
