// Game.h

#ifndef GAME_INCLUDED
#define GAME_INCLUDED

#include <vector>
#include <iostream>
using namespace std;

class Game;
class GameObject;
class Player;
class Monster;

const int DUNGEON_WIDTH = 70;
const int DUNGEON_HEIGHT = 18;
const int MAX_ITEMS = 3;
const int MIN_ITEMS = 2;

class Floor{
public:
    ~Floor();
    
    // Regenerate the floor
    void regenerate();
    
    // Print the floor
    void print();
    
    // Check if there is a wall at a the specified position
    bool isWall(int y, int x) const{ return m_walls[y][x]; }
    
    // Get floor variables
    int getExitY() const{ return m_exitY; }
    int getExitX() const{ return m_exitX; }
    int getIdolY() const{ return m_idolY; }
    int getIdolX() const{ return m_idolX; }
    char getTile(int y, int x) const{ return m_tiles[y][x]; }
    vector<GameObject*> getObj() const{ return m_obj; }
    vector<Monster*> getMonster() const{ return m_monster; }
    
    // Set floor variables
    void setGame(Game* g){ m_game = g; }
    void setObj(GameObject* g){ m_obj.push_back(g); }
    void setMonster(Monster* a){ m_monster.push_back(a); }
    void setTile(int y, int x, char c){ m_tiles[y][x] = c; }
    
    // Manipulate objects
    void addObj(GameObject* g){ m_obj.push_back(g); }
    void removeObj(int i){ m_obj.erase(m_obj.begin() + i); }
    
    // Manipulate monsters
    void removeMonster(int i){ m_monster.erase(m_monster.begin() + i); }
    
    // Functions to generate rooms
    void buildFirstRoom();
    void addRooms(int top, int left, int height, int width);
    bool buildUp(int y, int x, int h, int w);
    bool buildRight(int y, int x, int h, int w);
    bool buildDown(int y, int x, int h, int w);
    bool buildLeft(int y, int x, int h, int w);
    
    // Functions to randomize placement of objects/actors
    void randomStairs();
    void randomIdol();
    void randomObjects();
    void randomMonsters();
    void randomPlayer();
private:
    int m_exitY, m_exitX;
    int m_idolY, m_idolX;
    Game* m_game;
    
    char m_tiles[DUNGEON_HEIGHT][DUNGEON_WIDTH];
    bool m_walls[DUNGEON_HEIGHT][DUNGEON_WIDTH];
    vector<GameObject*> m_obj;
    vector<Monster*> m_monster;
};

class Game
{
public:
    Game(int goblinSmellDistance):m_goblinSmellDistance(goblinSmellDistance){}
    ~Game();
    
    // The main game loop
	void play();
    
    // Move the player to a certain position if it is possible, or attack an enemy at the specified position
    void movePlayer(int iY, int iX, int fY, int fX);
    
    // Get game variables
    int getSmellDistance() const{ return m_goblinSmellDistance; }
    Floor* getFloor() const { return m_floor; }
    Player* getPlayer() const{ return m_player; }
    
    // Set game variables
    void setPlayer(Player* p){ m_player = p; }
    
    // Saves dialogue for many different actions during each turn
    void setText(string text){ m_text += text + "\n"; }
private:
    int m_goblinSmellDistance;
    string m_text;
    Player* m_player;
    Floor* m_floor;
};

#endif // GAME_INCLUDED
