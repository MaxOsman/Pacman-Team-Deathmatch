#include "Pacman.h"
#include <sstream>
#include <math.h>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <ctime>

using namespace std;

const int Tile::Width = 32;
const int Tile::Height = 32;
const Vector2* Tile::Size = new Vector2((float)Width, (float)Height);

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanFrameTime(250), _cMunchieFrameTime(500), gameWidth(864), gameHeight(932)
{
	_pacman = new Player[4];
	_controls = new Graphic[4];
	for (int i = 0; i < 4; i++)
	{
		_pacman[i].direction = -1;
		_pacman[i].previousDir = -1;
		_pacman[i].speedMultiplier = 0.2f;
		_pacman[i].canAnimate = false;
		_pacman[i].speedCurrentFrameTime = 0;
		_pacman[i].currentFrameTime = 0;
		_pacman[i].frame = 0;
		_pacman[i].isCPU = false;
		_pacman[i].isDying = false;
		deathDifference[i] = 0;
		_pacman[i].cpuDir = -1;
		cpuDirectionChangeTime[i] = 0;
		for (int j = 0; j < 4; j++)
			_pacman[i].canInput[j] = true;
	}

	_arrow = new Graphic();
	_menu = new Graphic();
	_start = new Graphic();
	_target = new Graphic();
	_playerHelp = new Graphic[2];
	_paused = false;
	_endmenu = false;
	_startmenu = true;
	_playermenu = true;
	_helpmenu = false;
	_pKeyDown = false;
	_dirKeyDown = false;
	_retKeyDown = false;
	_teamScores[RED] = 0;
	_teamScores[BLUE] = 0;
	frozenTime = 0;
	pacmanWithGreen = -1;
	powerupMessageState = 0;
	powerupMessageCount = 0;
	globalTime = 0;
	powerupToSpawn = -1;
	munchieCurrentFrameTime = 0;
	powerupCurrentFrameTime = 0;
	munchieFrame = 0;
	powerupFrame = 0;
	finalTemp = 0;

	//Initialise important Game aspects
	Audio::Initialise();
	Graphics::Initialise(argc, argv, this, gameWidth, gameHeight, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	int markerCount = 0;
	_marker = new Marker[75];

	LoadTiles(1);
	LoadMarkers();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	// remember, DO CLEAN UP!

	for (int i = 0; i < 4; i++)
	{
		delete _pacman[i].texture;
		delete _pacman[i].sourceRect;
		delete _pacman[i].position;
		delete _pacman[i].originalPos;
		delete _controls[i].texture;
		delete _controls[i].sourceRect;
		delete _controls[i].position;
		delete _cpuHumanPositions[i];
	}
	for (int i = 0; i < 2; i++)
	{
		delete _playerHelp[i].texture;
		delete _playerHelp[i].sourceRect;
		delete _playerHelp[i].
			position;
	}
	for (int i = 0; i < 72; i++)
		delete _marker->position;

	for (vector<Collectable*>::iterator it = _munchie.begin(); it != _munchie.end(); it++)
		delete* it;

	for (vector<Collectable*>::iterator it = _powerup.begin(); it != _powerup.end(); it++)
		delete* it;

	for (int y = 0; y < GetHeight(); y++)
	{
		for (int x = 0; x < GetWidth(); x++)
		{
			delete (*_tiles)[x][y];
		}
	}
	delete _tiles;

	delete _menu->texture;
	delete _menu->sourceRect;
	delete _menu->position;
	delete _menu;
	delete _start->texture;
	delete _start->sourceRect;
	delete _start->position;
	delete _start;
	delete _arrow->texture;
	delete _arrow->position;
	delete _arrow->sourceRect;
	delete _arrow;
	delete _target->texture;
	delete _target->sourceRect;
	delete _target->position;
	delete _target;

	delete[] _pacman;
	delete[] _controls;
	delete[] _playerHelp;
	delete[] _marker;
	delete[] _cpuHumanPositions;

	delete _pacHelpPos;
	delete _ghostHelpPos;
	delete _p1Pos;
	delete _p2Pos;
	delete _p3Pos;
	delete _p4Pos;
	delete _spacePos;
	delete _redScore;
	delete _blueScore;
	delete _timePos;
	delete _powerupInfoPos;
	delete _stringPosition;

	delete _pop;
	delete _death;
	delete _power;
	delete _laser;

	for (int i = 0; i < 6; i++)
		delete _menuPositions[i];

	delete[] _menuPositions;
	delete Size;

	delete _munchieFullTexture;
	delete _munchieTransTexture;
	delete _munchieEmptyTexture;
	delete _powerupFullTexture;
	delete _powerupTransTexture;
	delete _powerupEmptyTexture;
}

Tile* Pacman::LoadTile(char tileType, float x, float y)
{
	switch (tileType)
	{
		case ' ':	
			return new Tile(nullptr, false);

		case '|':
			return LoadTile("Vertical", true);		
		case '-':
			return LoadTile("Horizontal", true);		
		case '{':
			return LoadTile("TopLeft", true);			
		case '}':
			return LoadTile("TopRight", true);		
		case '[':
			return LoadTile("BottomLeft", true);		
		case ']':
			return LoadTile("BottomRight", true);		
		case 'J':
			return LoadTile("LeftJoint", true);
		case 'L':
			return LoadTile("RightJoint", true);
		case 'I':
			return LoadTile("UpJoint", true);
		case 'K':
			return LoadTile("DownJoint", true);
		case 'F':
			return LoadTile("EndLeft", true);
		case 'H':
			return LoadTile("EndRight", true);
		case 'T':
			return LoadTile("EndUp", true);
		case 'G':
			return LoadTile("EndDown", true);
		case '(':
			return LoadTile("PassWallLeft", false);
		case ')':
			return LoadTile("PassWallRight", false);


		case '.':
			return LoadMunchieTile(x, y);		// Munchie
		case 'A':
			return LoadPowerupTile(0, x, y);		// Powerups
		case 'B':
			return LoadPowerupTile(1, x, y);
		case 'C':
			return LoadPowerupTile(2, x, y);
		case 'D':
			return LoadPowerupTile(3, x, y);

		case '0':
			return LoadStartTile(0, x, y);			// Player 1 start point
		case '1':
			return LoadStartTile(1, x, y);			// Player 2 start point
		case '2':
			return LoadStartTile(2, x, y);			// Player 3 start point
		case '3':
			return LoadStartTile(3, x, y);			// Player 4 start point

		case '~':
			return LoadMarkerTile(_CLEAR, x, y);
		case '#':
			return LoadMarkerTile(_MUNCH, x, y);

		return nullptr;
	}
}

int Pacman::GetWidth()
{
	return _tiles->size();
}

int Pacman::GetHeight()
{
	return _tiles->at(0).size();
}

void Pacman::LoadTiles(int levelIndex)
{
	// Load the level and ensure all of the lines are the same length.
	int width;
	vector<string>* lines = new vector<string>();
	fstream stream;
	stringstream ss;
	ss << "Maps/" << levelIndex << ".txt";
	stream.open(ss.str(), fstream::in);

	char* line = new char[256];
	stream.getline(line, 256);
	string* sline = new string(line);
	width = sline->size();
	while (!stream.eof())
	{
		lines->push_back(*sline);
		stream.getline(line, 256);
		delete sline;
		sline = new string(line);
	}

	delete[] line;
	delete sline;

	// Allocate the tile grid.
	_tiles = new vector<vector<Tile*>>(width, vector<Tile*>(lines->size()));

	// Loop over every tile position,
	for (int y = 0; y < GetHeight(); y++)
	{
		for (int x = 0; x < GetWidth(); x++)
		{
			// to load each tile.
			char tileType = lines->at(y)[x];
			(*_tiles)[x][y] = LoadTile(tileType, x, y);
		}
	}

	delete lines;
}

Tile* Pacman::LoadTile(const char* name, bool isSolid)
{
	stringstream ss;
	ss << "Tiles/" << name << ".png";

	Texture2D* tex = new Texture2D();
	tex->Load(ss.str().c_str(), true);

	return new Tile(tex, isSolid);
}

Tile* Pacman::LoadStartTile(int player, float x, float y)
{
	_pacman[player].position = new Vector2((x * 32.0f) + 2, (y * 32.0f) + 102);
	_pacman[player].originalPos = new Vector2((x * 32.0f) + 2, (y * 32.0f) + 102);

	_marker[markerCount].position = new Vector2((x * 32.0f), (y * 32.0f) + 100);
	markerCount++;

	return new Tile(nullptr, false);
}

Tile* Pacman::LoadPowerupTile(int type, float x, float y)
{
	_powerup.push_back(new Collectable(x, y, type+1));

	_marker[markerCount].position = new Vector2((x * 32.0f), (y * 32.0f) + 100);
	markerCount++;
	
	return new Tile(nullptr, false);
}

Tile* Pacman::LoadMunchieTile(float x, float y)
{
	_munchie.push_back(new Collectable(x, y, 0));

	return new Tile(nullptr, false);
}

Tile* Pacman::LoadMarkerTile(int type, int x, int y)
{
	_marker[markerCount].position = new Vector2(x * 32.0f, (y * 32.0f) + 100);
	markerCount++;

	if (type == _MUNCH)
		return LoadMunchieTile(x, y);
	else
		return new Tile(nullptr, false);
}

void Pacman::MarkerSet(int c, bool canRight, bool canDown, bool canLeft, bool canUp)
{
	_marker[c].direction[_RIGHT] = canRight;
	_marker[c].direction[_DOWN] = canDown;
	_marker[c].direction[_LEFT] = canLeft;
	_marker[c].direction[_FORW] = canUp;
}

void Pacman::LoadMarkers()
{	
	//Incomplete AI marker system
	MarkerSet(0, 1, 1, 0, 0);
	MarkerSet(1, 1, 1, 1, 0);
	MarkerSet(2, 0, 1, 1, 0);
	MarkerSet(3, 1, 1, 0, 0);
	MarkerSet(4, 1, 1, 1, 0);
	MarkerSet(5, 0, 1, 1, 0);

	MarkerSet(6, 1, 1, 0, 1);

	MarkerSet(7, 1, 1, 1, 1);
	MarkerSet(8, 1, 0, 1, 1);
	MarkerSet(9, 1, 0, 1, 1);
	MarkerSet(10, 1, 1, 1, 1);

	MarkerSet(11, 0, 1, 1, 1);

	MarkerSet(12, 1, 0, 0, 1);
	MarkerSet(13, 0, 1, 1, 0);
	MarkerSet(14, 1, 0, 0, 1);
	MarkerSet(15, 0, 1, 1, 0);
	MarkerSet(16, 1, 1, 0, 0);
	MarkerSet(17, 0, 0, 1, 1);
	MarkerSet(18, 1, 1, 0, 0);
	MarkerSet(19, 0, 0, 1, 1);
	MarkerSet(20, 1, 1, 0, 0);
	MarkerSet(21, 1, 0, 1, 1);
	MarkerSet(22, 1, 1, 1, 0);
	MarkerSet(23, 1, 0, 1, 1);
	MarkerSet(24, 0, 1, 1, 0);
	MarkerSet(25, 0, 1, 0, 1);

	MarkerSet(26, 0, 0, 1, 0);
	MarkerSet(27, 1, 0, 0, 0);

	MarkerSet(28, 0, 1, 0, 1);
	MarkerSet(29, 1, 1, 1, 1);
	MarkerSet(30, 1, 1, 1, 1);
	MarkerSet(31, 1, 1, 1, 1);
	MarkerSet(32, 1, 1, 1, 1);
	MarkerSet(33, 1, 1, 1, 1);
	MarkerSet(34, 0, 1, 0, 1);

	MarkerSet(35, 0, 0, 1, 0);
	MarkerSet(36, 1, 0, 0, 0);

	MarkerSet(37, 0, 1, 0, 1);
	MarkerSet(38, 1, 1, 0, 1);
	MarkerSet(39, 1, 0, 1, 1);
	MarkerSet(40, 0, 1, 1, 1);
	MarkerSet(41, 1, 1, 0, 0);
	MarkerSet(42, 0, 1, 1, 1);
	MarkerSet(43, 1, 0, 0, 1);
	MarkerSet(44, 0, 1, 1, 0);
	MarkerSet(45, 1, 1, 0, 0);
	MarkerSet(46, 0, 0, 1, 1);
	MarkerSet(47, 1, 1, 0, 1);
	MarkerSet(48, 0, 1, 1, 0);
	MarkerSet(49, 1, 0, 0, 1);
	MarkerSet(50, 0, 1, 1, 0);
	MarkerSet(51, 1, 0, 0, 1);
	MarkerSet(52, 1, 1, 1, 0);
	MarkerSet(53, 1, 0, 1, 1);
	MarkerSet(54, 1, 0, 1, 1);
	MarkerSet(55, 1, 1, 1, 0);
	MarkerSet(56, 0, 0, 1, 1);
	MarkerSet(57, 1, 1, 0, 0);
	MarkerSet(58, 0, 0, 1, 1);
	MarkerSet(59, 1, 1, 0, 0);
	MarkerSet(60, 0, 0, 1, 1);

	MarkerSet(61, 0, 1, 0, 0);

	MarkerSet(62, 1, 0, 0, 1);
	MarkerSet(63, 0, 1, 1, 0);
	MarkerSet(64, 1, 1, 0, 0);
	MarkerSet(65, 0, 0, 1, 1);

	MarkerSet(66, 0, 1, 0, 0);

	MarkerSet(67, 1, 0, 0, 1);
	MarkerSet(68, 0, 1, 1, 0);
	MarkerSet(69, 1, 0, 0, 1);
	MarkerSet(70, 1, 0, 1, 1);
	MarkerSet(71, 1, 0, 1, 1);
	MarkerSet(72, 1, 0, 1, 1);
	MarkerSet(73, 1, 0, 1, 1);
	MarkerSet(74, 0, 0, 1, 1);
}

bool Pacman::MunchieCollisionDetection(float pacx, float pacy, float pacwidth, float pacheight, float munchx, float munchy, float munchwidth, float munchheight)
{
	float left1 = pacx;
	float left2 = munchx;
	float right1 = pacx + pacwidth;
	float right2 = munchx + munchwidth;
	float top1 = pacy;
	float top2 = munchy;
	float bottom1 = pacy + pacheight;
	float bottom2 = munchy + munchheight;

	if (bottom1 < top2)
		return false;
	if(top1 > bottom2)
		return false;
	if(right1 < left2)
		return false;
	if(left1 > right2)
		return false;

	return true;
}

void Pacman::LoadContent()
{
	//Help menu graphics
	for (int i = 0; i < 4; i++)
	{
		_controls[i].texture = new Texture2D();
		_pacman[i].sourceRect = new Rect(0.0f, 0.0f, 28, 28);
	}
	_playerHelp[0].texture = new Texture2D();
	_playerHelp[1].texture = new Texture2D();
	
	_controls[0].texture->Load("Textures/controls1.tga", false);
	_controls[1].texture->Load("Textures/controls2.tga", false);
	_controls[2].texture->Load("Textures/controls3.tga", false);
	_controls[3].texture->Load("Textures/controls4.tga", false);
	_playerHelp[0].texture->Load("Textures/PacHelp.tga", false);
	_playerHelp[1].texture->Load("Textures/GhostHelp.png", false);
	_controls[0].sourceRect = new Rect(50.0f, 50.0f, 192, 128);
	_controls[1].sourceRect = new Rect(gameWidth - 250.0f, 50.0f, 192, 128);
	_controls[2].sourceRect = new Rect(50.0f, gameHeight - 400.0f, 192, 128);
	_controls[3].sourceRect = new Rect(gameWidth - 250.0f, gameHeight - 400.0f, 192, 128);
	_playerHelp[0].sourceRect = new Rect(250.0f, gameHeight / 2 - 150.0f, 28, 28);
	_playerHelp[1].sourceRect = new Rect(250.0f, gameHeight / 2 - 50.0f, 28, 28);

	//Start menu assets
	_arrow->texture = new Texture2D();
	_arrow->texture->Load("Textures/arrow.png", false);
	_arrow->position = new Vector2((gameWidth / 2.0f) + 128, (gameHeight / 2.0f) + 48);
	_arrow->sourceRect = new Rect(0.0f, 0.0f, 16, 16);
	_arrowPlace = 0;
	_menu->texture = new Texture2D();
	_menu->texture->Load("Textures/Transparency.png", false);
	_menu->sourceRect = new Rect(0.0f, 0.0f, gameWidth, gameHeight);
	_start->texture = new Texture2D();
	_start->texture->Load("Textures/Start.png", false);
	_start->sourceRect = new Rect(0.0f, 0.0f, gameWidth, gameHeight);
	_menuPositions[0] = new Vector2((gameWidth / 2.0f) - 128, (gameHeight / 2.0f) - 128);
	_menuPositions[1] = new Vector2((gameWidth / 2.0f) - 128, (gameHeight / 2.0f) - 64);
	_menuPositions[2] = new Vector2((gameWidth / 2.0f) - 128, gameHeight / 2.0f);
	_menuPositions[3] = new Vector2((gameWidth / 2.0f) - 128, (gameHeight / 2.0f) + 64);
	_menuPositions[4] = new Vector2((gameWidth / 2.0f) - 128, (gameHeight / 2.0f) + 128);
	_menuPositions[5] = new Vector2((gameWidth / 2.0f) - 128, (gameHeight / 2.0f) + 192);
	_cpuHumanPositions[0] = new Vector2((gameWidth / 2.0f) + 70, (gameHeight / 2.0f) - 64);
	_cpuHumanPositions[1] = new Vector2((gameWidth / 2.0f) + 70, (gameHeight / 2.0f));
	_cpuHumanPositions[2] = new Vector2((gameWidth / 2.0f) + 70, (gameHeight / 2.0f) + 64);
	_cpuHumanPositions[3] = new Vector2((gameWidth / 2.0f) + 70, (gameHeight / 2.0f) + 128);
	_redScore = new Vector2(20.0f, 40.0f);
	_blueScore = new Vector2(gameWidth - 200.0f, 40.0f);
	_timePos = new Vector2(gameWidth / 2 - 100.0f, 40.0f);
	_powerupInfoPos = new Vector2(gameWidth / 2 - 100.0f, 80.0f);

	//Target Data
	_target->texture = new Texture2D();
	_target->texture->Load("Textures/GreenTarget.png", false);
	_target->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
	_target->position = new Vector2(-200.0f, -200.0f);

	//Text for help screen
	_p1Pos = new Vector2(110.0f, 200.0f);
	_p2Pos = new Vector2(gameWidth - 190.0f, 200.0f);
	_p3Pos = new Vector2(110.0f, gameHeight - 250.0f);
	_p4Pos = new Vector2(gameWidth - 190.0f, gameHeight - 250.0f);
	_spacePos = new Vector2((gameWidth / 2) - 100.0f, gameHeight - 100.0f);
	_pacHelpPos = new Vector2((gameWidth / 2) - 110.0f, gameHeight / 2 - 130.0f);
	_ghostHelpPos = new Vector2((gameWidth / 2) - 110.0f, gameHeight / 2 - 30.0f);

	for (int i = 0; i < 4; i++)
	{
		_baseTexture[i] = new Texture2D;
		_dieTexture[i] = new Texture2D;
	}

	// Load Pacmans
	_baseTexture[0]->Load("Textures/Pacman_Red.tga", false);
	_baseTexture[1]->Load("Textures/GhostRed.png", false);
	_baseTexture[2]->Load("Textures/Pacman_Blue.tga", false);
	_baseTexture[3]->Load("Textures/GhostBlue.png", false);
	_dieTexture[0]->Load("Textures/RedDeath.tga", false);
	_dieTexture[1]->Load("Textures/RedGhostDeath.png", false);
	_dieTexture[2]->Load("Textures/BlueDeath.tga", false);
	_dieTexture[3]->Load("Textures/BlueGhostDeath.png", false);
	
	for(int i = 0; i < 4; i++)
		_pacman[i].texture = _baseTexture[i];

	//Collectable data
	_munchieEmptyTexture = new Texture2D;
	_munchieTransTexture = new Texture2D;
	_munchieFullTexture = new Texture2D;
	_powerupEmptyTexture = new Texture2D;
	_powerupTransTexture = new Texture2D;
	_powerupFullTexture = new Texture2D;
	_munchieFullTexture->Load("Textures/Munchies.png", false);
	_munchieTransTexture->Load("Textures/MunchiesTransparent.png", false);
	_munchieEmptyTexture->Load("Textures/MunchiesEmpty.png", false);
	_powerupFullTexture->Load("Textures/allcherries.png", false);
	_powerupTransTexture->Load("Textures/allcherriesTransparent.png", false);
	_powerupEmptyTexture->Load("Textures/allcherriesEmpty.png", false);

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	//Sounds
	_pop = new SoundEffect();
	_pop->Load("Sounds/pop.wav");
	_death = new SoundEffect();
	_death->Load("Sounds/death.wav");
	_power = new SoundEffect();
	_power->Load("Sounds/powerup.wav");
	_laser = new SoundEffect();
	_laser->Load("Sounds/laser.wav");
}

void Pacman::CanInputSet(int pacNum, Dir dir, Input::Keys key, Input::KeyboardState* state)
{
	if (_pacman[pacNum].isCPU == false)
	{
		if (state->IsKeyDown(key) && _pacman[pacNum].canInput[dir] == true)		//If key is pressed, and if this pacman can input...
		{
			_pacman[pacNum].previousDir = _pacman[pacNum].direction;
			_pacman[pacNum].direction = dir;
		}
		else if (state->IsKeyDown(key) && !_pacman[pacNum].canInput[dir] && _pacman[pacNum].canMove[dir])
			_pacman[pacNum].canInput[dir] = true;
	}
	else
	{
		if (_pacman[pacNum].cpuDir == dir && _pacman[pacNum].canInput[dir] == true)		//If key is pressed, and if this pacman can input...
		{
			_pacman[pacNum].previousDir = _pacman[pacNum].direction;
			_pacman[pacNum].direction = dir;
		}
		else if (_pacman[pacNum].cpuDir == dir && !_pacman[pacNum].canInput[dir] && _pacman[pacNum].canMove[dir])
			_pacman[pacNum].canInput[dir] = true;
	}
}

void Pacman::KillPacman(int j)
{
	if (_pacman[j].isDying && !_paused)		//Folding-in death animation
	{
		int time;
		for (int k = 0; k < 4; k++)
		{
			_pacman[j].canInput[k] = false;
			_pacman[j].canMove[k] = false;
		}
		if (_pacman[j].dyingProgress < 6)
		{
			time = globalTime;
			deathDifference[j] = (time - _pacman[j].collectedTime) / 15;
			if (deathDifference[j] > 0.25f)
			{
				_pacman[j].sourceRect->X += _pacman[j].sourceRect->Width;
				_pacman[j].dyingProgress++;
				_pacman[j].collectedTime = globalTime;
			}
		}
		else if (_pacman[j].dyingProgress >= 6)		//Wait for respawn
		{
			_pacman[j].position->X = -200;
			time = globalTime;
			deathDifference[j] = (time - _pacman[j].collectedTime) / 60;
			if (deathDifference[j] >= 10.0f)
			{
				//Respawn
				_pacman[j].position->X = _pacman[j].originalPos->X;
				_pacman[j].position->Y = _pacman[j].originalPos->Y;
				_pacman[j].isDying = false;
				_pacman[j].texture = _baseTexture[j];
			
				Audio::Play(_power);
				_pacman[j].dyingProgress = 0;
			}
		}
	}
}

void Pacman::KillPacmanSet(int j)
{
	//Set to dead
	_pacman[j].isDying = true;
	_pacman[j].collectedTime = globalTime;
	for (int i = 0; i < 4; i++)
	{
		if (j == i)
			_pacman[j].texture = _dieTexture[i];
	}
	_pacman[j].dyingProgress = 0;
	_pacman[j].canAnimate = false;
	Audio::Play(_death);
}

void Pacman::InputSet(int elapsedTime, Input::KeyboardState* state, Input::Keys upKey, Input::Keys leftKey, Input::Keys downKey, Input::Keys rightKey, int pacNum)
{
	CanInputSet(pacNum, _FORW, upKey, state);
	CanInputSet(pacNum, _LEFT, leftKey, state);
	CanInputSet(pacNum, _DOWN, downKey, state);
	CanInputSet(pacNum, _RIGHT, rightKey, state);
	
	for (int j = 0; j < 4; j++)
	{
		for (int i = 0; i < 4; i++)
		{
			if (MunchieCollisionDetection(_pacman[i].position->X, _pacman[i].position->Y, 28, 28, _pacman[j].position->X, _pacman[j].position->Y, 28, 28))
			{
				//"i" is the killer, "j" gets killed.
				//only ghosts (1 or 3) can kill pacmen (0 or 2)
				if (((i == 1 && j == 2) || (i == 3 && j == 0)) && !_pacman[j].isDying)
					KillPacmanSet(j);
			}
		}
	}
}

void Pacman::Input(int elapsedTime, Input::KeyboardState* state) 
{
	for (int i = 0; i < 4; i++)
	{
		if (_pacman[i].position->X < 33 || _pacman[i].position->X > gameWidth - 33)
		{
			_pacman[i].canInput[_DOWN] = false;
			_pacman[i].canInput[_FORW] = false;
		}
		if (_pacman[i].position->Y < 33 || _pacman[i].position->Y > gameHeight - 33)
		{
			_pacman[i].canInput[_LEFT] = false;
			_pacman[i].canInput[_RIGHT] = false;
		}
	}

	// Checks if key is pressed, Pacs 1 to 4
	InputSet(elapsedTime, state, Input::Keys::W, Input::Keys::A, Input::Keys::S, Input::Keys::D, 0);
	InputSet(elapsedTime, state, Input::Keys::I, Input::Keys::J, Input::Keys::K, Input::Keys::L, 1);
	InputSet(elapsedTime, state, Input::Keys::UP, Input::Keys::LEFT, Input::Keys::DOWN, Input::Keys::RIGHT, 2);
	InputSet(elapsedTime, state, Input::Keys::NUMPAD5, Input::Keys::NUMPAD1, Input::Keys::NUMPAD2, Input::Keys::NUMPAD3, 3);
	for (int i = 0; i < 4; i++)
	{
		_pacman[i].canAnimate = true;		//Put here once to save space

		if (_pacman[i].direction == _RIGHT && _pacman[i].canInput[_RIGHT] == true && _pacman[i].canMove[_RIGHT])
			_pacman[i].position->X += _pacman[i].speedMultiplier * elapsedTime; //Moves Pacman forward across X axis
		else if (_pacman[i].direction == _DOWN && _pacman[i].canInput[_DOWN] == true && _pacman[i].canMove[_DOWN])
			_pacman[i].position->Y += _pacman[i].speedMultiplier * elapsedTime; //Moves Pacman forward across Y axis
		else if (_pacman[i].direction == _LEFT && _pacman[i].canInput[_LEFT] == true && _pacman[i].canMove[_LEFT])
			_pacman[i].position->X -= _pacman[i].speedMultiplier * elapsedTime; //Moves Pacman back across X axis
		else if (_pacman[i].direction == _FORW && _pacman[i].canInput[_FORW] == true && _pacman[i].canMove[_FORW])
			_pacman[i].position->Y -= _pacman[i].speedMultiplier * elapsedTime; //Moves Pacman back across Y axis
		else
			_pacman[i].canAnimate = false;
	}
}

void Pacman::UpdateCPU(int i)
{
	if (cpuDirectionChangeTime[i] >= 200)
	{
		srand(time(0)+i);
		_pacman[i].cpuDir = rand() % 4;
		cpuDirectionChangeTime[i] = 0;
	}
	srand(time(0)+i);
	cpuDirectionChangeTime[i] += rand() % 8;
}

void Pacman::CheckViewportCollision()
{
	for (int i = 0; i < 4; i++)
	{
		// Screen loop
		if (_pacman[i].position->X > gameWidth-4)
		{
			_pacman[i].position->X = -static_cast<float>(_pacman[i].sourceRect->Width) + 4;	//One is float, other is int
		}
		if (_pacman[i].position->X < -_pacman[i].sourceRect->Width + 4)
		{
			_pacman[i].position->X = static_cast<float>(gameWidth-4);
		}
		if (_pacman[i].position->Y > gameHeight-4)
		{
			_pacman[i].position->Y = -static_cast<float>(_pacman[i].sourceRect->Height) + 4;
		}
		if (_pacman[i].position->Y < -_pacman[i].sourceRect->Height + 4)
		{
			_pacman[i].position->Y = static_cast<float>(gameHeight-4);
		}
	}
}

void Pacman::UpdatePacmanSet(int i, int elapsedTime)
{
	if (_pacman[i].canAnimate && !_paused)
	{
		_pacman[i].currentFrameTime += elapsedTime;
		if (_pacman[i].currentFrameTime > _cPacmanFrameTime)
		{
			_pacman[i].frame++;
			if (_pacman[i].frame >= 2)
				_pacman[i].frame = 0;

			if (_pacman[i].speedMultiplier == 0.2f)
				_pacman[i].currentFrameTime = 0;
			else if (_pacman[i].speedMultiplier == 0.4f)
				_pacman[i].currentFrameTime = 125;
			else
				_pacman[i].currentFrameTime = 250;
		}
	}
	if (!_pacman[i].isDying)
	{
		_pacman[i].sourceRect->Y = _pacman[i].sourceRect->Height * static_cast<float>(_pacman[i].direction);
		_pacman[i].sourceRect->X = _pacman[i].sourceRect->Width * _pacman[i].frame;
	}
}

void Pacman::UpdatePacman(int elapsedTime, Input::KeyboardState* state)
{
	for (int i = 0; i < 4; i++)
		UpdatePacmanSet(i, elapsedTime);
}

void Pacman::UpdateCollectables(int elapsedTime)
{
	if (!_paused)
	{
		//Munchies
		munchieCurrentFrameTime += elapsedTime;
		if (munchieCurrentFrameTime > _cMunchieFrameTime)
		{
			if (munchieFrame == 0)
				munchieFrame = 1;
			else if (munchieFrame == 1)
				munchieFrame = 0;

			munchieCurrentFrameTime = 1;
		}
		for (int j = 0; j < (int)_munchie.size(); ++j)
			_munchie[j]->sourceRect->X = _munchie[j]->sourceRect->Width * munchieFrame;

		//Powerups
		powerupCurrentFrameTime += elapsedTime;
		if (powerupCurrentFrameTime > _cMunchieFrameTime)
		{
			if (powerupFrame < 3)
				powerupFrame++;
			else if (powerupFrame == 3)
				powerupFrame = 0;

			powerupCurrentFrameTime = 1;
		}
		for (int j = 0; j < (int)_powerup.size(); ++j)
			_powerup[j]->sourceRect->X = _powerup[j]->sourceRect->Width * powerupFrame;
	}
}

void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey)
{
	if (state->IsKeyDown(pauseKey) && !_pKeyDown && !_helpmenu && _paused)		//Unpause
	{
		_pKeyDown = true;
		_paused = !_paused;
	}
	else if (state->IsKeyDown(pauseKey) && !_pKeyDown && !_helpmenu && !_paused)		//Pause
	{
		_pKeyDown = true;
		_paused = !_paused;
	}
	else if (state->IsKeyUp(pauseKey) && !_helpmenu)
	{
		_pKeyDown = false;
		CheckStart(state);
	}
	if (state->IsKeyDown(Input::Keys::SPACE) && _helpmenu)
		_helpmenu = false;
}

void Pacman::CheckStart(Input::KeyboardState* state)
{
	if (state->IsKeyDown(Input::Keys::S) && !_dirKeyDown && !_retKeyDown && (_paused || _startmenu || _playermenu))	//So that arrow only moves in menu, not while game is played
	{
		if (!_startmenu && _playermenu)
		{
			if (_arrowPlace >= 4)		//4 for settings menu
				_arrowPlace = 0;
			else
				_arrowPlace++;
		}
		else
		{
			if (_arrowPlace >= 2)
				_arrowPlace = 0;
			else
				_arrowPlace++;
		}
		_dirKeyDown = true;
	}

	if (state->IsKeyDown(Input::Keys::W) && !_dirKeyDown && (_paused || _startmenu || _playermenu))	//So that arrow only moves in menu, not while game is played
	{
		if (!_startmenu && _playermenu)
		{
			if (_arrowPlace <= 0)
				_arrowPlace = 4;
			else
				_arrowPlace--;
		}
		else
		{
			if (_arrowPlace <= 0)
				_arrowPlace = 2;
			else
				_arrowPlace--;
		}
		_dirKeyDown = true;
	}

	if (state->IsKeyUp(Input::Keys::W) && state->IsKeyUp(Input::Keys::S))
		_dirKeyDown = false;
	if (state->IsKeyUp(Input::Keys::RETURN))
		_retKeyDown = false;
	if (state->IsKeyDown(Input::Keys::RETURN) && !_retKeyDown)
	{
		if (!_startmenu && _playermenu)
		{
			for (int i = 0; i < 4; i++)
			{
				if (_arrowPlace == i)
					_pacman[i].isCPU = !_pacman[i].isCPU;
			}
			if (_arrowPlace == 4)
			{
				_playermenu = false;
				_arrowPlace = 0;
				for (int i = 0; i < 4; i++)		//Start timer for cherries
					_powerup[i]->collectedTime = globalTime;
			}
		}
		else
		{
			if (_arrowPlace == 0)
			{
				_startmenu = false;
				_paused = false;
			}
			else if (_arrowPlace == 1)
				_helpmenu = true;
			else if (_arrowPlace == 2)
				Graphics::Destroy();
		}
		_retKeyDown = true;
	}
}

void Pacman::RedCherry(int i, int elapsedTime)
{
	//Remove speed buff if timer reduced to zero
	if (_pacman[i].speedCurrentFrameTime > 0)
		_pacman[i].speedCurrentFrameTime -= elapsedTime;
	if (_pacman[i].speedCurrentFrameTime <= 0)
	{
		if (_pacman[i].speedMultiplier == 0.4f)
		{
			_pacman[i].speedMultiplier = 0.2f;
			_pacman[i].speedCurrentFrameTime = 0;
		}
	}
}

void Pacman::BlueCherry(int i, int elapsedTime)
{
	//freeze other team if one gets blue cherry
	if (frozenTime > 0)
		frozenTime -= elapsedTime;
	if (frozenTime <= 0)
	{
		if (_pacman[i].speedMultiplier == 0.0f)
		{
			_pacman[i].speedMultiplier = 0.2f;
			frozenTime = 0;
		}
	}
}

void Pacman::GreenCherry(int i, Input::MouseState* state)
{
	if (pacmanWithGreen != -1)
	{
		_target->position->X = state->X-16;
		_target->position->Y = state->Y-16;
		if (state->LeftButton == Input::ButtonState::PRESSED)	//Kill on click
		{
			if (pacmanWithGreen <= 1)
			{
				if (MunchieCollisionDetection(_target->position->X - 32, _target->position->Y - 32, 64, 64, _pacman[2].position->X, _pacman[2].position->Y, 28, 28))
					KillPacmanSet(2);
				if (MunchieCollisionDetection(_target->position->X - 32, _target->position->Y - 32, 64, 64, _pacman[3].position->X, _pacman[3].position->Y, 28, 28))
					KillPacmanSet(3);
			}
			else
			{
				if (MunchieCollisionDetection(_target->position->X - 32, _target->position->Y - 32, 64, 64, _pacman[0].position->X, _pacman[0].position->Y, 28, 28))
					KillPacmanSet(0);
				if (MunchieCollisionDetection(_target->position->X - 32, _target->position->Y - 32, 64, 64, _pacman[1].position->X, _pacman[1].position->Y, 28, 28))
					KillPacmanSet(1);
			}
			Audio::Play(_laser);
			pacmanWithGreen = -1;
		}
	}
	else
		_target->position->X = -200;
}

void Pacman::YellowCherry(int i)
{
	for (int j = 0; j < (int)_munchie.size(); ++j)
	{
		int time = globalTime;
		float difference = (time - _munchie[j]->collectedTime) / 60;
		if (difference <= 30.0f && _munchie[j]->isCollected == true)		//Replenish munchies
		{
			_munchie[j]->texture = _munchieFullTexture;
			_munchie[j]->isCollected = false;
		}
	}
}

void Pacman::RefreshMunchie(Collectable* tempMunch)
{
	int time = globalTime;
	float difference = (time - tempMunch->collectedTime) / 60;
	if (difference > 25.0f && tempMunch->isCollected == true)		//Munchie Transparent
		tempMunch->texture = _munchieTransTexture;

	if (difference > 30.0f && tempMunch->isCollected == true)		//Munchie Full
	{
		tempMunch->texture = _munchieFullTexture;
		tempMunch->isCollected = false;
	}
}

void Pacman::RefreshPowerup(int i)
{
	int time = globalTime;
	float difference = (time - _powerup[i]->collectedTime) / 60;
	if (difference > 55.0f && _powerup[i]->isCollected == true)		//Powerup transparent
	{
		_powerup[i]->texture = _powerupTransTexture;
		powerupMessageState = 1;
	}
	if (difference > 50.0f && _powerup[i]->isCollected == true)		//Powerup Full
	{
		_powerup[i]->texture = _powerupFullTexture;
		_powerup[i]->isCollected = false;
		powerupMessageState = 2;
	}
}

void Pacman::MunchieCollInteraction(int i)
{
	if (i == 0 || i == 2)		//Only pacmen can eat
	{
		//collision between munchie and pacman
		for (int j = 0; j < (int)_munchie.size(); ++j)
		{
			RefreshMunchie(_munchie[j]);
			if (MunchieCollisionDetection(_pacman[i].position->X, _pacman[i].position->Y, 28, 28, _munchie[j]->position->X, _munchie[j]->position->Y, 12, 12) == true && _munchie[j]->isCollected == false)
			{
				_munchie[j]->texture = _munchieEmptyTexture;
				_munchie[j]->collectedTime = globalTime;
				_munchie[j]->isCollected = true;

				Audio::Play(_pop);
				if (i == 0 || i == 1)
					_teamScores[RED]++;
				else if (i == 2 || i == 3)
					_teamScores[BLUE]++;
			}
		}
	}
}

void Pacman::PowerupCollInteraction(int i)
{
	if (i == 0 || i == 2)
	{
		//collision between powerup and pacman
		for (int j = 0; j < (int)_powerup.size(); ++j)
		{
			if (MunchieCollisionDetection(_pacman[i].position->X, _pacman[i].position->Y, 28, 28, _powerup[j]->position->X, _powerup[j]->position->Y, 24, 24) == true && _powerup[j]->isCollected == false)
			{
				if (_powerup[j]->type == 1)		//red
				{
					_pacman[i].speedCurrentFrameTime = 6000;
					_pacman[i].speedMultiplier = 0.4f;
					_pacman[i].currentFrameTime += 125;			//Animation starts later, so moves faster
				}
				else if (_powerup[j]->type == 2)		//blue
				{
					//Stops movement
					if (i == 0 || i == 1)
					{
						_pacman[2].speedMultiplier = 0.0f;
						_pacman[3].speedMultiplier = 0.0f;
					}
					else if (i == 2 || i == 3)
					{
						_pacman[0].speedMultiplier = 0.0f;
						_pacman[1].speedMultiplier = 0.0f;
					}
					frozenTime = 15000;
				}
				else if (_powerup[j]->type == 3)		//green
					pacmanWithGreen = i;
				else if (_powerup[j]->type == 4)		//yellow
					YellowCherry(i);

				_powerup[j]->texture = _powerupEmptyTexture;
				for(int k = 0; k < 4; k++)
					_powerup[k]->collectedTime = globalTime;

				_powerup[j]->isCollected = true;
				powerupToSpawn = -1;
				Audio::Play(_power);
			}
		}
	}
}

void Pacman::WallCollision(int i, int elapsedTime)
{
	for (int h = 0; h < 4; h++)
		_pacman[i].canMove[h] = true;

	for (int y = 0; y < GetHeight(); ++y)
	{
		for (int x = 0; x < GetWidth(); ++x)
		{
			bool canContinue = true;
			if (_tiles->at(x).at(y)->isSolid > 0)			//If solid
			{
				Vector2* pos = new Vector2(x * 32, (y * 32) + 100);
				int left1 = _pacman[i].position->X;
				int left2 = pos->X;
				int right1 = _pacman[i].position->X + 27;
				int right2 = pos->X + 31;
				int top1 = _pacman[i].position->Y;
				int top2 = pos->Y;
				int bottom1 = _pacman[i].position->Y + 27;
				int bottom2 = pos->Y + 31;

				for (int h = 0; h < 4; h++)
					_pacman[i].canMove[h] = true;

				if ((bottom1 < top2) || (top1 > bottom2) || (right1 < left2) || (left1 > right2))
					canContinue = false;

				if (canContinue)
				{
					if (bottom1 >= top2 && _pacman[i].direction == _DOWN)
					{
						_pacman[i].canMove[_DOWN] = false;

						if(OppositeDir(_pacman[i].direction) != _pacman[i].previousDir)
							_pacman[i].direction = _pacman[i].previousDir;

						_pacman[i].position->Y = top2 - 30;
						if (_pacman[i].previousDir == _LEFT && _tiles->at(x - 1).at(y)->Texture == nullptr)
							_pacman[i].position->X -= _pacman[i].speedMultiplier * elapsedTime;
						if (_pacman[i].previousDir == _RIGHT && _tiles->at(x + 1).at(y)->Texture == nullptr)
							_pacman[i].position->X += _pacman[i].speedMultiplier * elapsedTime;
					}
					else if (top1 <= bottom2 && _pacman[i].direction == _FORW)
					{
						_pacman[i].canMove[_FORW] = false;

						if (OppositeDir(_pacman[i].direction) != _pacman[i].previousDir)
							_pacman[i].direction = _pacman[i].previousDir;

						_pacman[i].position->Y = bottom2 + 3;
						if (_pacman[i].previousDir == _LEFT && _tiles->at(x - 1).at(y)->Texture == nullptr)
							_pacman[i].position->X -= _pacman[i].speedMultiplier * elapsedTime;
						if (_pacman[i].previousDir == _RIGHT && _tiles->at(x + 1).at(y)->Texture == nullptr)
							_pacman[i].position->X += _pacman[i].speedMultiplier * elapsedTime;
					}
					else if (right1 >= left2 && _pacman[i].direction == _RIGHT)
					{
						_pacman[i].canMove[_RIGHT] = false;

						if (OppositeDir(_pacman[i].direction) != _pacman[i].previousDir)
							_pacman[i].direction = _pacman[i].previousDir;

						_pacman[i].position->X = left2 - 30;
						if (_pacman[i].previousDir == _FORW && _tiles->at(x).at(y - 1)->Texture == nullptr)
							_pacman[i].position->Y -= _pacman[i].speedMultiplier * elapsedTime;
						if (_pacman[i].previousDir == _DOWN && _tiles->at(x).at(y + 1)->Texture == nullptr)
							_pacman[i].position->Y += _pacman[i].speedMultiplier * elapsedTime;
					}
					else if (left1 <= right2 && _pacman[i].direction == _LEFT)
					{
						_pacman[i].canMove[_LEFT] = false;

						if (OppositeDir(_pacman[i].direction) != _pacman[i].previousDir)
							_pacman[i].direction = _pacman[i].previousDir;

						_pacman[i].position->X = right2 + 3;
						if (_pacman[i].previousDir == _FORW && _tiles->at(x).at(y - 1)->Texture == nullptr)
							_pacman[i].position->Y -= _pacman[i].speedMultiplier * elapsedTime;
						if (_pacman[i].previousDir == _DOWN && _tiles->at(x).at(y + 1)->Texture == nullptr)
							_pacman[i].position->Y += _pacman[i].speedMultiplier * elapsedTime;
					}
				}
				delete pos;
			}
		}
	}
}

void Pacman::Update(int elapsedTime)
{
	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	Input::MouseState* mouseState = Input::Mouse::GetState();

	if (!_startmenu && !_playermenu && !_endmenu)
	{
		if (!_paused && !_endmenu)
		{
			Input(elapsedTime, keyboardState);
			CheckViewportCollision();
			UpdatePacman(elapsedTime, keyboardState);
			UpdateCollectables(elapsedTime);
			for (int i = 0; i < 4; i++)		//Collectable detection + action
			{
				RedCherry(i, elapsedTime);
				BlueCherry(i, elapsedTime);
				GreenCherry(i, mouseState);
				MunchieCollInteraction(i);
				PowerupCollInteraction(i);
				WallCollision(i, elapsedTime);
				KillPacman(i);
				UpdateCPU(i);
			}
			if (powerupToSpawn == -1)
			{
				srand(time(0));
				powerupToSpawn = rand() % 4;
			}
			RefreshPowerup(powerupToSpawn);			//Pick a random powerup to spawn

			globalTime++;
			if (globalTime >= 18000)		//5 Mins
				_endmenu = true;
		}
		CheckPaused(keyboardState, Input::Keys::P);
	}
	else if ((_startmenu || _playermenu) && !_endmenu)
	{
		if (!_helpmenu)
			CheckStart(keyboardState);
		else if (_helpmenu)
		{
			if (keyboardState->IsKeyDown(Input::Keys::SPACE))
				_helpmenu = false;
		}
	}
	else if (_endmenu)
	{
		if (keyboardState->IsKeyDown(Input::Keys::SPACE))
			Graphics::Destroy();
	}
}

void Pacman::DrawTiles()
{
	for (int y = 0; y < GetHeight(); ++y)
	{
		for (int x = 0; x < GetWidth(); ++x)
		{
			// If there is a visible tile in that position
			Texture2D* texture = _tiles->at(x).at(y)->Texture;
			if (texture != nullptr)
			{
				// Draw it in screen space.
				Vector2 position((float)x, (float)y);
				position *= *Tile::Size;
				position.Y += 100;
				SpriteBatch::Draw(texture, &position);
			}
		}
	}
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	stringstream stream;
	SpriteBatch::BeginDraw(); // Starts Drawing
	//stream << _pacman[0].position->X << " " << _pacman[0].position->Y << " " << _pacman[0].originalPos->X << " " << _pacman[0].originalPos->Y;

	DrawTiles();		//Draws level
	
	// Draw Munchie
	for (int i = 0; i < (int)_munchie.size(); ++i)
		SpriteBatch::Draw(_munchie[i]->texture, _munchie[i]->position, _munchie[i]->sourceRect); // Draws Munchies

	for (int i = 0; i < (int)_powerup.size(); ++i)
		SpriteBatch::Draw(_powerup[i]->texture, _powerup[i]->position, _powerup[i]->sourceRect); // Draws Powerups
	
	for (int i = 0; i < 4; i++)
		SpriteBatch::Draw(_pacman[i].texture, _pacman[i].position, _pacman[i].sourceRect); // Draws Pacman

	SpriteBatch::Draw(_target->texture, _target->position, _target->sourceRect);
	
	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);
	stream.str(string());
	stream << "Red Team: " << _teamScores[RED];
	SpriteBatch::DrawString(stream.str().c_str(), _redScore, Color::Red);
	stream.str(string());
	stream << "Blue Team: " << _teamScores[BLUE];
	SpriteBatch::DrawString(stream.str().c_str(), _blueScore, Color::Cyan);
	stream.str(string());
	stream << "Time Remaining: " << 300 - (globalTime / 60);
	SpriteBatch::DrawString(stream.str().c_str(), _timePos, Color::Yellow);
	stream.str(string());

	if (powerupMessageState == 1)
	{
		stream << "Powerup is about to spawn...";
		SpriteBatch::DrawString(stream.str().c_str(), _powerupInfoPos, Color::Yellow);
		stream.str(string());
		powerupMessageCount = 100;
	}
	if (powerupMessageState == 2 && powerupMessageCount > 0)
	{
		stream << "Powerup has spawned!";
		SpriteBatch::DrawString(stream.str().c_str(), _powerupInfoPos, Color::Yellow);
		stream.str(string());
		powerupMessageCount--;
	}
	if (powerupMessageCount == 0)
		powerupMessageState = 0;

	for (int i = 0; i < 4; i++)
	{
		if (_pacman[i].dyingProgress >= 6)
		{
			_pacman[i].originalPos->Y += 20;
			_pacman[i].originalPos->X += 6;
			stream << 10 - deathDifference[i];
			if(i <= 1)
				SpriteBatch::DrawString(stream.str().c_str(), _pacman[i].originalPos, Color::Red);
			else
				SpriteBatch::DrawString(stream.str().c_str(), _pacman[i].originalPos, Color::Cyan);
			stream.str(string());
			_pacman[i].originalPos->Y -= 20;
			_pacman[i].originalPos->X -= 6;
		}
	}

	if (_paused)	//Game paused
	{
		SpriteBatch::Draw(_menu->texture, _menu->sourceRect, nullptr);

		SpriteBatch::Draw(_arrow->texture, _arrow->position, _arrow->sourceRect);
		_arrow->position->Y = gameHeight / 2.0f + 48 + (64 * _arrowPlace);
		
		stream << "-PAUSED-";
		SpriteBatch::DrawString(stream.str().c_str(), _menuPositions[0], Color::White);
		stream.str(string());
		stream << "Resume Game";
		SpriteBatch::DrawString(stream.str().c_str(), _menuPositions[3], Color::White);
		stream.str(string());
		stream << "How To Play";
		SpriteBatch::DrawString(stream.str().c_str(), _menuPositions[4], Color::White);
		stream.str(string());
		stream << "Quit";
		SpriteBatch::DrawString(stream.str().c_str(), _menuPositions[5], Color::White);
		stream.str(string());
	}

	if (_startmenu)		//Main menu
	{
		SpriteBatch::Draw(_start->texture, _start->sourceRect, nullptr);
		_arrow->position->Y = gameHeight / 2.0f + 48 + (64 * _arrowPlace);

		if (!_helpmenu)
		{
			SpriteBatch::Draw(_arrow->texture, _arrow->position, _arrow->sourceRect);
			
			stream << "PACMAN TEAM DEATHMATCH";
			SpriteBatch::DrawString(stream.str().c_str(), _menuPositions[0], Color::White);
			stream.str(string());
			stream << "Start Game";
			SpriteBatch::DrawString(stream.str().c_str(), _menuPositions[3], Color::White);
			stream.str(string());
			stream << "How To Play";
			SpriteBatch::DrawString(stream.str().c_str(), _menuPositions[4], Color::White);
			stream.str(string());
			stream << "Quit";
			SpriteBatch::DrawString(stream.str().c_str(), _menuPositions[5], Color::White);
			stream.str(string());
		}
	}

	if (_playermenu && !_startmenu)		//Pre-game options
	{
		SpriteBatch::Draw(_start->texture, _start->sourceRect, nullptr);

		SpriteBatch::Draw(_arrow->texture, _arrow->position, _arrow->sourceRect);
		_arrow->position->Y = gameHeight / 2.0f - 80 + (64 * _arrowPlace);

		stream << "-Round Options-";
		SpriteBatch::DrawString(stream.str().c_str(), _menuPositions[0], Color::White);
		stream.str(string());
		stream << "Player 1:";
		SpriteBatch::DrawString(stream.str().c_str(), _menuPositions[1], Color::Red);
		stream.str(string());
		stream << "Player 2:";
		SpriteBatch::DrawString(stream.str().c_str(), _menuPositions[2], Color::Red);
		stream.str(string());
		stream << "Player 3:";
		SpriteBatch::DrawString(stream.str().c_str(), _menuPositions[3], Color::Cyan);
		stream.str(string());
		stream << "Player 4:";
		SpriteBatch::DrawString(stream.str().c_str(), _menuPositions[4], Color::Cyan);
		stream.str(string());

		for (int i = 0; i < 4; i++)
		{
			if (_pacman[i].isCPU == false)
				stream << "Human";
			else
				stream << "CPU";

			SpriteBatch::DrawString(stream.str().c_str(), _cpuHumanPositions[i], Color::White);
			stream.str(string());
		}
		stream << "Begin Round!";
		SpriteBatch::DrawString(stream.str().c_str(), _menuPositions[5], Color::Yellow);
		stream.str(string());
	}

	// Help menu
	if (_helpmenu)		//Help menu
	{
		SpriteBatch::Draw(_start->texture, _start->sourceRect, nullptr);

		for (int i = 0; i < 4; i++)
			SpriteBatch::Draw(_controls[i].texture, _controls[i].sourceRect, nullptr);
		
		SpriteBatch::Draw(_playerHelp[0].texture, _playerHelp[0].sourceRect, nullptr);
		SpriteBatch::Draw(_playerHelp[1].texture, _playerHelp[1].sourceRect, nullptr);
		
		stream << "Player 1";
		SpriteBatch::DrawString(stream.str().c_str(), _p1Pos, Color::Red);
		stream.str(string());
		stream << "Player 2";
		SpriteBatch::DrawString(stream.str().c_str(), _p2Pos, Color::Red);
		stream.str(string());
		stream << "Player 3";
		SpriteBatch::DrawString(stream.str().c_str(), _p3Pos, Color::Cyan);
		stream.str(string());
		stream << "Player 4";
		SpriteBatch::DrawString(stream.str().c_str(), _p4Pos, Color::Cyan);
		stream.str(string());
		stream << "Press SPACE to go back";
		SpriteBatch::DrawString(stream.str().c_str(), _spacePos, Color::White);
		stream.str(string());
		stream << "Only Pacmans can eat Munchies and Powerups";
		SpriteBatch::DrawString(stream.str().c_str(), _pacHelpPos, Color::Yellow);
		stream.str(string());
		stream << "Only Ghosts can kill Pacmans";
		SpriteBatch::DrawString(stream.str().c_str(), _ghostHelpPos, Color::Yellow);
		stream.str(string());
	}

	if (_endmenu)		//End Screen
	{
		SpriteBatch::Draw(_start->texture, _start->sourceRect, nullptr);

		stream << "-Final Results-";
		SpriteBatch::DrawString(stream.str().c_str(), _menuPositions[0], Color::White);
		stream.str(string());
		stream << "Red Team: " << _teamScores[RED];
		SpriteBatch::DrawString(stream.str().c_str(), _menuPositions[1], Color::Red);
		stream.str(string());
		stream << "Blue Team: " << _teamScores[BLUE];
		SpriteBatch::DrawString(stream.str().c_str(), _menuPositions[2], Color::Cyan);
		stream.str(string());

		if (_teamScores[RED] > _teamScores[BLUE])
		{
			stream << "Red Team Wins!";
			SpriteBatch::DrawString(stream.str().c_str(), _menuPositions[3], Color::White);
			stream.str(string());
		}
		else if (_teamScores[RED] < _teamScores[BLUE])
		{
			stream << "Blue Team Wins!";
			SpriteBatch::DrawString(stream.str().c_str(), _menuPositions[3], Color::White);
			stream.str(string());
		}
		else
		{
			stream << "It's a Draw!";
			SpriteBatch::DrawString(stream.str().c_str(), _menuPositions[3], Color::White);
			stream.str(string());
		}

		stream << "Press SPACE to quit game";
		SpriteBatch::DrawString(stream.str().c_str(), _menuPositions[5], Color::White);
		stream.str(string());
	}

	SpriteBatch::EndDraw(); // Ends Drawing
}

Collectable::Collectable(float x, float y, int isPowerup)
{
	texture = new Texture2D;
	position = new Vector2(x * 32.0f, (y * 32.0f)+100);
	collectedTime = 0;
	if (isPowerup == 0)
	{
		isCollected = false;
		sourceRect = new Rect(0.0f, 0.0f, 12, 12);
		texture->Load("Textures/Munchies.png", false);
		position->X += 10;
		position->Y += 10;
	}	
	else
	{
		isCollected = true;
		sourceRect = new Rect(0.0f, 0.0f, 24, 24);
		texture->Load("Textures/allcherriesEmpty.png", false);
		position->X += 4;
		position->Y += 4;
	}

	if (isPowerup == 1)
		type = 1;
	if (isPowerup == 2)
	{
		sourceRect->Y += 24;
		type = 2;
	}
	if (isPowerup == 3)
	{
		sourceRect->Y += 48;
		type = 3;
	}
	if (isPowerup == 4)
	{
		sourceRect->Y += 72;
		type = 4;
	}
}

Tile::Tile(Texture2D* texture, bool solid)
{
	Texture = texture;
	isSolid = solid;
}

Tile::~Tile(void)
{
	delete Texture;
}

int Pacman::OppositeDir(int dir)
{
	if (dir == _FORW)
		return _DOWN;
	else if (dir == _DOWN)
		return _FORW;
	else if (dir == _LEFT)
		return _RIGHT;
	else if (dir == _RIGHT)
		return _LEFT;
	else
		return -1;
}
