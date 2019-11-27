#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

//For scores
#define RED 0
#define BLUE 1

//For Directions
#define _RIGHT 0
#define _DOWN 1
#define _LEFT 2
#define _UP 3

// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

struct Player
{
	float speedMultiplier; 
	int currentFrameTime,
		direction,			//0=right, 1=down, 2=left, 3=up
		frame,
		speedCurrentFrameTime;
	Rect* sourceRect; 
	Texture2D* texture; 
	Vector2* position;
	bool canInput[4],				//1st = pacman, 2nd = direction.
		 canAnimate;
};

struct Collectable
{
	//int currentFrameTime,
	    //frame;
	int type;
	Vector2* position;
	Rect* sourceRect;
	Texture2D* texture;
	bool isCollected;
	Collectable(float x, float y, int isPowerup);
};

struct Graphic
{
	Vector2* pos;
	Rect* sourceRect;
	Texture2D* texture;
};

struct Tile
{
public:
	Texture2D* Texture;
	bool isSolid;

	static const int Width;
	static const int Height;

	static const Vector2* Size;

	Tile(Texture2D* texture, bool isSolid);
	~Tile(void);
};

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Pacman : public Game
{
private:

	//Methods
	void Input(int elapsedTime, Input::KeyboardState* state);
	void InputSet(int elapsedTime, Input::KeyboardState* state, Input::Keys upKey, Input::Keys leftKey, Input::Keys downKey, Input::Keys rightKey, int pacNum);
	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey);
	void CheckStart(Input::KeyboardState* state);
	void CheckViewportCollision();
	void UpdatePacman(int elapsedTime, Input::KeyboardState* state);
	void UpdateMunchie(int elapsedTime);
	bool MunchieCollisionDetection(float pacx, float pacy, float pacwidth, float pacheight, float munchx, float munchy, float munchwidth, float munchheight);
	void RedCherry(int i, int elaspedTime);
	void BlueCherry(int i, int elaspedTime);

	//Tile* temp_func();

	//Pacman data
	Player *_pacman[4];
	const int _cPacmanFrameTime;
	int frozenTime;

	//Team Data
	int _teamScores[2];		//0 = red, 1 = blue

	//Munchie data
	std::vector<Collectable*> _munchie;
	const int _cMunchieFrameTime;

	int munchieCurrentFrameTime,
		munchieFrame; 

	//Powerup data
	std::vector<Collectable*> _powerup;

	//Menu data
	bool _paused,
	_startmenu,
	_helpmenu;

	Graphic *_arrow;
	signed int _arrowPlace;	//0 = start, 1 = help, 2 = quit
	Graphic *_menu;
	Graphic *_start;
	Graphic *_controls[4];

	Rect* _menuRectangle;
	Vector2* _menuStringPosition,
		*_titleStringPosition,
		*_startStringPosition,
		*_helpStringPosition,
		*_quitStringPosition;

	Vector2* _p1Pos,
	*_p2Pos,
	*_p3Pos,
	*_p4Pos,
	*_spacePos;

	bool _pKeyDown,
	_dirKeyDown,
	_retKeyDown;
	
	// Position for String
	Vector2* _stringPosition;

	//Tiles
	
	std::vector<std::vector<Tile*>>* _tiles;
	static const int Width;
	static const int Height;
	Vector2* Size;

	int GetWidth();
	int GetHeight();

public:
	void LoadTiles(int levelIndex);
	Tile* LoadTile(const char* name, bool isSolid);
	Tile* LoadTile(const char tileType, float x, float y);
	Tile* LoadStartTile(int player, float x, float y);
	Tile* LoadPowerupTile(int type, float x, float y);
	Tile* LoadMunchieTile(float x, float y);

	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);
	void DrawTiles();
};