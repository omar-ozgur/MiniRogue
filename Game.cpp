// Game.cpp

#include "Game.h"
#include "Actor.h"
#include "GameObject.h"
#include "utilities.h"
#include <iostream>
using namespace std;

Game::~Game(){
    delete m_floor;
    delete m_player;
}

void Game::play()
{
    // Create a new player with a pointer to this game
    setPlayer(new Player);
    m_player->setGame(this);
    
    // Create a new floor with a pointer to this game
    m_floor = new Floor;
    m_floor->setGame(this);
    
    // Generate a new floor
    m_floor->regenerate();
    
    // Start the main game loop
    while(1){
        
        // Print the floor and player stats
        m_floor->print();
        m_player->printStats();
        cout << endl;
        
        // Output action text
        if(m_text != ""){
            cout << m_text;
            m_text = "";
        }
        
        // End the game if the player is dead
        if(m_player->getHp() <= 0){
            while(getCharacter() != 'q');
            exit(0);
        }
        
        // Randomly give the player extra health
        m_player->randomHealth();
        
        // Execute player command
        char c = getCharacter();
        int y = m_player->getY();
        int x = m_player->getX();
        switch (c) {
                
            // End the game
            case 'q':
                exit(0);
                
            // Move/Attack left
            case ARROW_LEFT:
                movePlayer(y, x, y, x - 1);
                break;
                
            // Move/Attack right
            case ARROW_RIGHT:
                movePlayer(y, x, y, x + 1);
                break;
                
            // Move/Attack up
            case ARROW_UP:
                movePlayer(y, x, y - 1, x);
                break;
            
            // Move/Attack down
            case ARROW_DOWN:
                movePlayer(y, x, y + 1, x);
                break;
                
            // Pick up the object at the player's position (if there is one)
            case 'g':
                
                // End the game if the idol is picked up
                if(y == m_floor->getIdolY() && x == m_floor->getIdolX()){
                    cout << "You pick up the golden idol\nCongratulations, you won!" << endl;
                    cout << "Press q to exit game." << endl;
                    while(1){
                        if(getCharacter() == 'q')
                            exit(0);
                    }
                }
                for(int i = 0; i < m_floor->getObj().size(); i++){
                    
                    // Check if there is an object at the player's position and the inventory is not full
                    if(m_player->getInventory().size() <= 25 && y == m_floor->getObj()[i]->getY() && x == m_floor->getObj()[i]->getX()){
                        
                        // Output different text if the object is a weapon or a scroll
                        Weapon* w = dynamic_cast<Weapon*>(m_floor->getObj()[i]);
                        if (w != nullptr){
                            setText("You pick up " + m_floor->getObj()[i]->getName());
                        }
                        else{
                            setText("You pick up a scroll called " + m_floor->getObj()[i]->getName());
                        }
                        
                        // Add the object to the player's inventory, and remove it from the floor
                        m_player->addItem(m_floor->getObj()[i]);
                        m_floor->removeObj(i);
                        break;
                    }
                    // Check if there is an object at the player's position and the inventory is full
                    else if(y == m_floor->getObj()[i]->getY() && x == m_floor->getObj()[i]->getX()){
                        setText("Your knapsack is full; you can't pick that up.");
                    }
                }
                break;
                
            // Wield a weapon
            case 'w':
                m_player->wield();
                break;
                
            // Read a scroll
            case 'r':
                m_player->read();
                break;
                
            // Check the inventory
            case 'i':
                m_player->viewInventory();
                break;
                
            // Go to the next floor
            case '>':
                if(getFloor()->getExitY() == m_player->getY() && getFloor()->getExitX() == m_player->getX()){
                    
                    // Increase the floor count and regenerate the floor
                    m_player->setFloor(m_player->getFloor() + 1);
                    m_floor->regenerate();
                }
                break;
                
            // Activate cheat mode
            case 'c':
                m_player->cheat();
                break;
                
            // If any other key is pressed, do nothing
            default:
                break;
        }
        
        // Have each monster take it's turn
        for(int i = 0; i < m_floor->getMonster().size(); i++){
            m_floor->getMonster()[i]->takeTurn();
        }
        
        // Set action test if the player is dead
        if(m_player->getHp() <= 0)
            setText("Press q to exit game.");
    }
}

void Game::movePlayer(int iY, int iX, int fY, int fX){
    for(int i = 0; i < m_floor->getMonster().size(); i++){
        
        // If the player tries to move into a monster, attack the monster
        if(m_floor->getMonster()[i]->getY() == fY && m_floor->getMonster()[i]->getX() == fX){
            m_player->attack(m_player, m_floor->getMonster()[i], m_player->getWeapon());
            
            // If the monster dies, have it possibly drop items, and remove it from the floor
            if(m_floor->getMonster()[i]->getHp() <= 0){
                m_floor->setTile(m_floor->getMonster()[i]->getY(), m_floor->getMonster()[i]->getX(), ' ');
                m_floor->getMonster()[i]->drop();
                delete m_floor->getMonster()[i];
                m_floor->removeMonster(i);
            }
            return;
        }
    }
    
    // If the player is not asleep, and the specified position is valid, move to that position
    if(m_player->getSleep() <= 0){
        if(!m_floor->isWall(fY, fX)){
            m_floor->setTile(iY, iX, ' ');
            m_floor->setTile(fY, fX, '@');
            m_player->setPos(fY, fX);
        }
    }
    
    // If the player is asleep, do nothing, and decrement the sleep counter
    else if(m_player->getSleep() > 0){
        m_player->setSleep(m_player->getSleep() - 1);
    }
}

Floor::~Floor(){
    delete m_game;
    for(int i = 0; i < m_monster.size(); i++){
        delete m_monster[i];
    }
    for(int i = 0; i < m_obj.size(); i++){
        delete m_obj[i];
    }
}

void Floor::regenerate(){
    
    // Delete all objects
    for(int i = 0; i < m_obj.size(); i++){
        delete m_obj[i];
    }
    
    // Delete all monsters
    for(int i = 0; i < m_monster.size(); i++){
        delete m_monster[i];
    }
    
    // Clear the object and monster vectors
    m_obj.clear();
    m_monster.clear();
    
    // Place a wall at every dungeon position
    for(int i = 0; i < DUNGEON_HEIGHT; i++){
        for(int j = 0; j < DUNGEON_WIDTH; j++){
            m_tiles[i][j] = '#';
            m_walls[i][j] = true;
        }
    }
    
    // Build the first room
    buildFirstRoom();
    
    // Set the stairs at a random location
    if(m_game->getPlayer()->getFloor() != 4)
        randomStairs();
    else{
        m_exitY = -1;
        m_exitX = -1;
    }
    
    // Set the idol at a random location
    if(m_game->getPlayer()->getFloor() == 4)
        randomIdol();
    
    // Add random objects
    randomObjects();
    
    // Set the player in a random location
    randomPlayer();
    
    // Add random enemies
    randomMonsters();
}

void Floor::print(){
    clearScreen();
    
    // Set object tiles
    for(int x = 0; x < m_obj.size(); x++){
        setTile(m_obj[x]->getY(), m_obj[x]->getX(), m_obj[x]->getC());
    }
    
    // Set the stairs tile
    setTile(m_exitY, m_exitX, '>');
    for(int x = 0; x < m_monster.size(); x++){
        setTile(m_monster[x]->getY(), m_monster[x]->getX(), m_monster[x]->getC());
    }
    
    // Set the idol tile if the player is on the last floor
    if(m_game->getPlayer()->getFloor() == 4)
        setTile(m_idolY, m_idolX, '&');
    
    // Set the player tile
    setTile(m_game->getPlayer()->getY(), m_game->getPlayer()->getX(), '@');
    
    // Print out all tiles
    for(int i = 0; i < DUNGEON_HEIGHT; i++){
        for(int j = 0; j < DUNGEON_WIDTH; j++){
            cout << m_tiles[i][j];
        }
        cout << endl;
    }
}

void Floor::randomStairs(){
    
    // Keep trying to place the stairs at random positions until a valid position is found
    while(1){
        int randX = 1 + randInt(DUNGEON_WIDTH - 2);
        int randY = 1 + randInt(DUNGEON_HEIGHT - 2);
        if(getTile(randY, randX) == ' '){
            m_exitY = randY;
            m_exitX = randX;
            break;
        }
    }
}

void Floor::randomIdol(){
    
    // Keep trying to place the idol at random positions until a valid position is found
    while(1){
        int randX = 1 + randInt(DUNGEON_WIDTH - 2);
        int randY = 1 + randInt(DUNGEON_HEIGHT - 2);
        if(getTile(randY, randX) == ' '){
            m_idolY = randY;
            m_idolX = randX;
            break;
        }
    }
}

void Floor::randomObjects(){
    
    // Keep trying to place objects at random positions until valid positions are found
    int obj = 0;
    while(obj < MIN_ITEMS + randInt(MAX_ITEMS - MIN_ITEMS + 1)){
        int randX = 1 + randInt(DUNGEON_WIDTH - 2);
        int randY = 1 + randInt(DUNGEON_HEIGHT - 2);
        if(getTile(randY, randX) == ' '){
            
            // If a valid position is found, choose to place a weapon or a scroll
            int category = randInt(2);
            
            // Choose a random weapon
            if(category == 0){
                int choice = randInt(3);
                switch (choice) {
                    case 0:
                        setObj(new Mace(randY, randX));
                        setTile(randY, randX, ')');
                        break;
                    case 1:
                        setObj(new ShortSword(randY, randX));
                        setTile(randY, randX, ')');
                        break;
                    case 2:
                        setObj(new LongSword(randY, randX));
                        setTile(randY, randX, ')');
                        break;
                }
            }
            
            // Choose a random scroll
            else if(category == 1){
                int choice = randInt(4);
                switch (choice) {
                    case 0:
                        setObj(new ArmorScroll(m_game, randY, randX));
                        setTile(randY, randX, '?');
                        break;
                    case 1:
                        setObj(new HealthScroll(m_game, randY, randX));
                        setTile(randY, randX, '?');
                        break;
                    case 2:
                        setObj(new DexterityScroll(m_game, randY, randX));
                        setTile(randY, randX, '?');
                        break;
                    case 3:
                        setObj(new StrengthScroll(m_game, randY, randX));
                        setTile(randY, randX, '?');
                        break;
                }
            }
            obj++;
        }
    }
}

void Floor::randomPlayer(){
    
    // Keep trying to place the player at random positions until a valid position is found
    while(1){
        int randX = 1 + randInt(DUNGEON_WIDTH - 2);
        int randY = 1 + randInt(DUNGEON_HEIGHT - 2);
        if(getTile(randY, randX) != '#'){
            setTile(randY, randX, '@');
            m_game->getPlayer()->setPos(randY, randX);
            break;
        }
    }
}

void Floor::randomMonsters(){
    
    // Keep trying to place monsters at random positions until valid positions are found
    int enemies = 0;
    while(enemies < 2 + randInt(5*(m_game->getPlayer()->getFloor()+1))){
        int randX = 1 + randInt(DUNGEON_WIDTH - 2);
        int randY = 1 + randInt(DUNGEON_HEIGHT - 2);
        
        // Check if there is already a monster at the random position
        for(int i = 0; i < m_monster.size(); i++){
            if(m_monster[i]->getY() == randY && m_monster[i]->getX() == randX){
                randY = 0;
                randX = 0;
            }
        }
        
        // Check if there is a wall or a player at the random position
        if(!isWall(randY, randX) && getTile(randY, randX) != '@'){
            Monster* a;
            
            // Choose a random monster based on the current floor
            int choice;
            if(m_game->getPlayer()->getFloor() < 2)
                choice = randInt(2);
            else if(m_game->getPlayer()->getFloor() < 3)
                choice = randInt(3);
            else
                choice = randInt(4);
            
            // Create the new monster
            switch (choice) {
                case 0:
                    a = new Snakewoman(randY, randX);
                    a->setGame(m_game);
                    setMonster(a);
                    break;
                case 1:
                    a = new Goblin(randY, randX);
                    a->setGame(m_game);
                    setMonster(a);
                    break;
                case 2:
                    a = new Bogeyman(randY, randX);
                    a->setGame(m_game);
                    setMonster(a);
                    break;
                case 3:
                    a = new Dragon(randY, randX);
                    a->setGame(m_game);
                    setMonster(a);
                    break;
            }
            enemies++;
        }
    }
}

void Floor::buildFirstRoom(){
    
    // Pick random valid sizes for an initial room
    int top = 1 + randInt(DUNGEON_HEIGHT - DUNGEON_HEIGHT/2);
    int left = 1 + randInt(DUNGEON_WIDTH - DUNGEON_WIDTH/5);
    int width = 7 + randInt((DUNGEON_WIDTH - left)/2);
    int height = 6 + randInt((DUNGEON_HEIGHT - top)/3);
    
    // Set tiles of the first room
    for(int i = top; i < top + height; i++){
        for(int j = left; j < left + width; j++){
            m_tiles[i][j] = ' ';
            m_walls[i][j] = false;
        }
    }
    
    // Randomly add rooms on all sides of the inital room
    addRooms(top, left, height, width);
}

void Floor::addRooms(int top, int left, int height, int width){
    int times = 12;
    
    // Try creating rooms a specific number of times (higher number typically = more rooms)
    while(times > 0){
        
        // Try building room above
        if(top >= 5){
            int y = top;
            int x = left + randInt(width);
            int h = 4 + randInt(top - 4);
            int w;
            if(x > DUNGEON_WIDTH/2)
                w = 4 + randInt(DUNGEON_WIDTH - x);
            else if(x <= DUNGEON_WIDTH/2)
                w = 4 + randInt(x);
            buildUp(y, x, h, w);
        }
        
        // Try building room below
        if(DUNGEON_HEIGHT - top - height >= 5){
            int y = top + height;
            int x = left + randInt(width);
            int h = 4 + randInt(DUNGEON_HEIGHT - y - 4);
            int w;
            if(x > DUNGEON_WIDTH/2)
                w = 4 + randInt(DUNGEON_WIDTH - x);
            else if(x <= DUNGEON_WIDTH/2)
                w = 4 + randInt(x);
            buildDown(y, x, h, w);
        }
        
        // Try building room to the right
        if(DUNGEON_WIDTH - width - left >= 5){
            int y = top + randInt(height);
            int x = left + width;
            int w = 4 + randInt(DUNGEON_WIDTH - x - 4);
            int h;
            if(y > DUNGEON_HEIGHT/2)
                h = 4 + randInt(DUNGEON_HEIGHT - y);
            else if(y <= DUNGEON_HEIGHT/2)
                h = 4 + randInt(y);
            buildRight(y, x, h, w);
        }
        
        // Try building room to the right
        if(left >= 5){
            int y = top + randInt(height);
            int x = left;
            int w = 4 + randInt(left - 4);
            int h;
            if(y > DUNGEON_HEIGHT/2)
                h = 4 + randInt(DUNGEON_HEIGHT - y);
            else if(y <= DUNGEON_HEIGHT/2)
                h = 4 + randInt(y);
            buildLeft(y, x, h, w);
        }
        times--;
    }
}

bool Floor::buildUp(int y, int x, int h, int w){
    
    // Stop if the room is at an invalid position
    for(int i = y - h - 1; i < y; i++){
        for(int j = x - w - 1; j < x + w + 1; j++){
            if(j <= 0 || j >= DUNGEON_WIDTH - 1 || m_tiles[i][j] == ' '){
                return false;
            }
        }
    }
    
    // Create the upward corridor
    for(int i = y - h; i < y; i++){
        m_tiles[i][x] = ' ';
        m_walls[i][x] = false;
    }
    
    // Create the upward room
    int height = 3 + randInt(h - 3);
    for(int i = y - h; i < y - h + height; i++){
        for(int j = x - w; j < x + w; j++){
            m_tiles[i][j] = ' ';
            m_walls[i][j] = false;
        }
    }
    
    // Add random rooms on each side of the new room
    addRooms(y - h, x - w, height, w * 2);
    return true;
}

bool Floor::buildRight(int y, int x, int h, int w){
    
    // Stop if the room is at an invalid position
    for(int i = y - h - 1; i < y + h + 1; i++){
        for(int j = x; j < x + w + 1; j++){
            if(i <= 0 || i >= DUNGEON_HEIGHT - 1 || m_tiles[i][j] == ' ')
                return false;
        }
    }
    
    // Create a corridor to the right
    for(int i = x; i < x + w; i++){
        m_tiles[y][i] = ' ';
        m_walls[y][i] = false;
    }
    
    // Create the room on the right
    int width = 3 + randInt(w - 3);
    for(int i = y - h; i < y + h; i++){
        for(int j = x + w - width; j < x + w; j++){
            m_tiles[i][j] = ' ';
            m_walls[i][j] = false;
        }
    }
    
    // Add random rooms on each side of the new room
    addRooms(y - h, x + w - width, h * 2, width);
    return true;
}

bool Floor::buildLeft(int y, int x, int h, int w){
    
    // Stop if the room is at an invalid position
    for(int i = y - h - 1; i < y + h + 1; i++){
        for(int j = x - w - 1; j < x; j++){
            if(i <= 0 || i >= DUNGEON_HEIGHT - 1 || m_tiles[i][j] == ' ')
                return false;
        }
    }
    
    // Create a corridor on the left
    for(int i = x - w; i < x; i++){
        m_tiles[y][i] = ' ';
        m_walls[y][i] = false;
    }
    
    // Create the room on the left
    int width = 3 + randInt(w - 3);
    for(int i = y - h; i < y + h; i++){
        for(int j = x - w; j < x - w + width; j++){
            m_tiles[i][j] = ' ';
            m_walls[i][j] = false;
        }
    }
    
    // Add random rooms on each side of the new room
    addRooms(y - h, x - w, h * 2, width);
    return true;
}

bool Floor::buildDown(int y, int x, int h, int w){
    
    // Stop if the room is at an invalid position
    for(int i = y; i < y + h + 1; i++){
        for(int j = x - w - 1; j < x + w + 1; j++){
            if(j <= 0 || j >= DUNGEON_WIDTH - 1 || m_tiles[i][j] == ' '){
                return false;
            }
        }
    }
    
    // Create the downward corridor
    for(int i = y; i < y + h; i++){
        m_tiles[i][x] = ' ';
        m_walls[i][x] = false;
    }
    
    // Create the downward room
    int height = 3 + randInt(h - 3);
    for(int i = y + h - height; i < y + h; i++){
        for(int j = x - w; j < x + w; j++){
            m_tiles[i][j] = ' ';
            m_walls[i][j] = false;
        }
    }
    
    // Add random rooms on each side of the new room
    addRooms(y + h - height, x - w, height, w * 2);
    return true;
}

