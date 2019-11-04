#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Pacman : public Game
{
private:

	//Methods
	void Input(int elapsedTime, Input::KeyboardState* state);
	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey);
	void CheckStart(Input::KeyboardState* state);
	void CheckViewportCollision();
	void UpdatePacman(int elapsedTime, Input::KeyboardState* state);
	void UpdateMunchie(int elapsedTime);


	// Data to represent Pacman
	Vector2* _pacmanPosition[4];
	Rect* _pacmanSourceRect[4];
	Texture2D* _pacmanTexture[4];
	Vector2* _arrowPos;
	Rect* _arrowSourceRect;
	Texture2D* _arrowTexture;

	//Pacman data
	int _pacmanDir[4];	//0 = right, 1 = down, 2 = left, 3 = up
	const float _cPacmanSpeed;
	int _pacmanFrame[4];
	int _pacmanCurrentFrameTime[4];
	const int _cPacmanFrameTime;

	//Munchie data
	const int _cMunchieFrameTime;
	int _munchieCurrentFrameTime;
	int _munchieFrame;

	Vector2* _munchiePosition;
	Rect* _munchieSourceRect;
	Texture2D* _munchieTexture;

	//Menu data
	bool _paused;
	bool _startmenu;
	bool _helpmenu;
	Texture2D* _menuBackground;
	Texture2D* _startBackground;

	Texture2D* _c1Texture;
	Rect* _c1Rect;
	Texture2D* _c2Texture;
	Rect* _c2Rect;
	Texture2D* _c3Texture;
	Rect* _c3Rect;
	Texture2D* _c4Texture;
	Rect* _c4Rect;

	Rect* _menuRectangle;
	Vector2* _menuStringPosition;
	Vector2* _titleStringPosition;
	Vector2* _startStringPosition;
	Vector2* _helpStringPosition;
	Vector2* _quitStringPosition;

	Vector2* _p1Pos;
	Vector2* _p2Pos;
	Vector2* _p3Pos;
	Vector2* _p4Pos;
	Vector2* _spacePos;

	bool _pKeyDown;
	bool _dirKeyDown;
	bool _retKeyDown;
	signed int _arrowPlace;	//0 = start, 1 = help, 2 = quit
	
	// Position for String
	Vector2* _stringPosition;

public:
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
};