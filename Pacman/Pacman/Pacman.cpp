#include "Pacman.h"

#include <sstream>
#include <math.h>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <iostream>

using namespace std;

const int Tile::Width = 32;
const int Tile::Height = 32;
const Vector2* Tile::Size = new Vector2((float)Width, (float)Height);


Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanFrameTime(250), _cMunchieFrameTime(500)
{
	for (int i = 0; i < 4; i++)
	{
		_pacman[i] = new Player();
		_pacman[i]->direction = -1;
		_pacman[i]->speedMultiplier = 0.2f;
		_pacman[i]->canAnimate = false;
		_pacman[i]->speedCurrentFrameTime = 0;
		for (int j = 0; j < 4; j++)
		{
			_pacman[i]->canInput[j] = true;
		}
		_controls[i] = new Graphic();
	}

	_arrow = new Graphic();
	_menu = new Graphic();
	_start = new Graphic();
	_target = new Graphic();
	_paused = false;
	_startmenu = true;
	_helpmenu = false;
	_pKeyDown = false;
	_dirKeyDown = false;
	_retKeyDown = false;
	_teamScores[0] = 0;
	_teamScores[1] = 0;
	frozenTime = 0;
	pacmanWithGreen = -1;

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 1024, 992, false, 25, 25, "Pacman", 60);
	Input::Initialise();
	Audio::Initialise();

	LoadTiles(0);

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	// remember, DO CLEAN UP!

	for (int i = 0; i < 4; i++)
	{
		delete _pacman[i]->texture;
		delete _pacman[i]->sourceRect;
		delete _pacman[i]->position;
		delete _controls[i]->texture;
		delete _controls[i]->sourceRect;
		delete _controls[i]->position;
	}
	delete[] _pacman;
	delete[] _controls;

	for (vector<Collectable*>::iterator it = _munchie.begin(); it != _munchie.end(); it++)
	{
		delete* it;
	}

	for (vector<Collectable*>::iterator it = _powerup.begin(); it != _powerup.end(); it++)
	{
		delete* it;
	}
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

	delete _menuStringPosition;
	delete _titleStringPosition;
	delete _startStringPosition;
	delete _helpStringPosition;
	delete _quitStringPosition;

	delete _p1Pos;
	delete _p2Pos;
	delete _p3Pos;
	delete _p4Pos;
	delete _spacePos;

	delete _stringPosition;
	delete _tiles;

	delete _pop;
}

Tile* Pacman::LoadTile(char tileType, float x, float y)
{
	switch (tileType)
	{
		case '.':	
			return new Tile(nullptr, false);

		case '|':
			return LoadTile("Vertical", true);		// Vertical
		
		case '-':
			return LoadTile("Horizontal", true);		// Horizontal

		case '[':
			return LoadTile("TopLeft", true);			// TopLeft

		case ']':
			return LoadTile("TopRight", true);		// TopRight

		case '{':
			return LoadTile("BottomLeft", true);		// BottomLeft

		case '}':
			return LoadTile("BottomRight", true);		// BottomLeft

		case '#':
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
	_pacman[player]->position = new Vector2(x * 32.0f, y * 32.0f);

	return new Tile(nullptr, false);
}

Tile* Pacman::LoadPowerupTile(int type, float x, float y)
{
	if (type == 0)
		_powerup.push_back(new Collectable(x, y, 1));
	if (type == 1)
		_powerup.push_back(new Collectable(x, y, 2));
	if (type == 2)
		_powerup.push_back(new Collectable(x, y, 3));
	if (type == 3)
		_powerup.push_back(new Collectable(x, y, 4));
	
	return new Tile(nullptr, false);
}

Tile* Pacman::LoadMunchieTile(float x, float y)
{
	_munchie.push_back(new Collectable(x, y, 0));

	return new Tile(nullptr, false);
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
		_controls[i]->texture = new Texture2D();
		_pacman[i]->currentFrameTime = 0;
		_pacman[i]->frame = 0;
		_pacman[i]->position = new Vector2(0.0f, 0.0f);
	}

	munchieCurrentFrameTime = 0;
	munchieFrame = 0;

	_controls[0]->texture->Load("Textures/controls1.tga", false);
	_controls[1]->texture->Load("Textures/controls2.tga", false);
	_controls[2]->texture->Load("Textures/controls3.tga", false);
	_controls[3]->texture->Load("Textures/controls4.tga", false);

	_controls[0]->sourceRect = new Rect(200.0f, 50.0f, 192, 128);
	_controls[1]->sourceRect = new Rect(Graphics::GetViewportWidth() - 400.0f, 50.0f, 192, 128);
	_controls[2]->sourceRect = new Rect(200.0f, Graphics::GetViewportHeight() - 400.0f, 192, 128);
	_controls[3]->sourceRect = new Rect(Graphics::GetViewportWidth() - 400.0f, Graphics::GetViewportHeight() - 400.0f, 192, 128);

	//Start menu assets
	_arrow->texture = new Texture2D();
	_arrow->texture->Load("Textures/arrow.png", false);
	_arrow->position = new Vector2((Graphics::GetViewportWidth() / 2.0f) + 128, (Graphics::GetViewportHeight() / 2.0f) + 48);
	_arrow->sourceRect = new Rect(0.0f, 0.0f, 16, 16);
	_arrowPlace = 0;

	_menu->texture = new Texture2D();
	_menu->texture->Load("Textures/Transparency.png", false);
	_menu->sourceRect = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());

	_start->texture = new Texture2D();
	_start->texture->Load("Textures/Start.png", false);
	_start->sourceRect = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());

	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);
	_titleStringPosition = new Vector2((Graphics::GetViewportWidth() / 2.0f)-64, (Graphics::GetViewportHeight() / 2.0f)-128);
	_startStringPosition = new Vector2((Graphics::GetViewportWidth() / 2.0f)-128, (Graphics::GetViewportHeight() / 2.0f)+64);
	_helpStringPosition = new Vector2((Graphics::GetViewportWidth() / 2.0f) - 128, (Graphics::GetViewportHeight() / 2.0f) + 128);
	_quitStringPosition = new Vector2((Graphics::GetViewportWidth() / 2.0f) - 128, (Graphics::GetViewportHeight() / 2.0f) + 192);

	//Target Data
	_target->texture = new Texture2D();
	_target->texture->Load("Textures/GreenTarget.png", false);
	_target->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
	_target->position = new Vector2(-200.0f, -200.0f);

	//Text for help screen
	_p1Pos = new Vector2(260.0f, 200.0f);
	_p2Pos = new Vector2(Graphics::GetViewportWidth() - 340.0f, 200.0f);
	_p3Pos = new Vector2(260.0f, Graphics::GetViewportHeight() - 250.0f);
	_p4Pos = new Vector2(Graphics::GetViewportWidth() - 340.0f, Graphics::GetViewportHeight() - 250.0f);
	_spacePos = new Vector2((Graphics::GetViewportWidth() / 2) - 100.0f, Graphics::GetViewportHeight() - 50.0f);

	// Load Pacmans
	_pacman[0]->texture = new Texture2D();
	_pacman[0]->texture->Load("Textures/Pacman_Red.tga", false);
	_pacman[1]->texture = new Texture2D();
	_pacman[1]->texture->Load("Textures/Pacman_Red.tga", false);
	_pacman[2]->texture = new Texture2D();
	_pacman[2]->texture->Load("Textures/Pacman_Blue.tga", false);
	_pacman[3]->texture = new Texture2D();
	_pacman[3]->texture->Load("Textures/Pacman_Blue.tga", false);

	for (int i = 0; i < 4; i++)
	{
		_pacman[i]->sourceRect = new Rect(0.0f, 0.0f, 27, 27);
		_pacman[i]->position->X = 0;
		_pacman[i]->position->Y = 0;
	}

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	//Sounds
	_pop = new SoundEffect();
	_pop->Load("Sounds/pop.wav");
}

void Pacman::CanMoveSet(int pacNum, Dir dir, Input::Keys key, Input::KeyboardState* state)
{
	if (state->IsKeyDown(key) && _pacman[pacNum]->canInput[dir] == true)		//If up key is pressed, and if this pacman can input...
	{
		_pacman[pacNum]->direction = dir;
	}
	else if (state->IsKeyDown(key) && _pacman[pacNum]->canInput[dir] == false)
	{
		_pacman[pacNum]->canInput[dir] = true;
	}
}

void Pacman::InputSet(int elapsedTime, Input::KeyboardState* state, Input::Keys upKey, Input::Keys leftKey, Input::Keys downKey, Input::Keys rightKey, int pacNum)
{
	for (int j = 0; j < 4; j++)
	{
		for (int i = 0; i < 4; i++)
		{
			if (sqrtf((_pacman[j]->position->X - _pacman[i]->position->X) * (_pacman[j]->position->X - _pacman[i]->position->X) + (_pacman[j]->position->Y - _pacman[i]->position->Y) * (_pacman[j]->position->Y - _pacman[i]->position->Y)) < 27 && i != j)
			{
				//Neither can move if hit face on
				if (_pacman[j]->direction == _RIGHT && _pacman[i]->direction == _LEFT)
				{
					_pacman[j]->canInput[_RIGHT] = false;
					_pacman[i]->canInput[_LEFT] = false;
				}
				else if (_pacman[j]->direction == _LEFT && _pacman[i]->direction == _RIGHT)
				{
					_pacman[j]->canInput[_LEFT] = false;
					_pacman[i]->canInput[_RIGHT] = false;
				}
				else if (_pacman[j]->direction == _DOWN && _pacman[i]->direction == _FORW)
				{
					_pacman[j]->canInput[_DOWN] = false;
					_pacman[i]->canInput[_FORW] = false;
				}
				else if (_pacman[j]->direction == _FORW && _pacman[i]->direction == _DOWN)
				{
					_pacman[j]->canInput[_FORW] = false;
					_pacman[i]->canInput[_DOWN] = false;
				}

				//"i" is the killer, "j" gets killed.
				//Gets killed if killer is walking towards its back or sides
				if (_pacman[j]->direction == _RIGHT && _pacman[i]->direction == _RIGHT && _pacman[i]->position->X < _pacman[j]->position->X)
				{
					_pacman[j]->position->X = -666;
				}
				else if (_pacman[j]->direction == _RIGHT && _pacman[i]->direction == _DOWN && _pacman[i]->position->Y < _pacman[j]->position->Y)
				{
					_pacman[j]->position->X = -666;
				}
				else if (_pacman[j]->direction == _RIGHT && _pacman[i]->direction == _FORW && _pacman[i]->position->Y > _pacman[j]->position->Y)
				{
					_pacman[j]->position->X = -666;
				}

				if (_pacman[j]->direction == _DOWN && _pacman[i]->direction == _RIGHT && _pacman[i]->position->X < _pacman[j]->position->X)
				{
					_pacman[j]->position->X = -666;
				}
				else if (_pacman[j]->direction == _DOWN && _pacman[i]->direction == _DOWN && _pacman[i]->position->Y < _pacman[j]->position->Y)
				{
					_pacman[j]->position->X = -666;
				}
				else if (_pacman[j]->direction == _DOWN && _pacman[i]->direction == _LEFT && _pacman[i]->position->X > _pacman[j]->position->X)
				{
					_pacman[j]->position->X = -666;
				}

				if (_pacman[j]->direction == _LEFT && _pacman[i]->direction == _LEFT && _pacman[i]->position->X > _pacman[j]->position->X)
				{
					_pacman[j]->position->X = -666;
				}
				else if (_pacman[j]->direction == _LEFT && _pacman[i]->direction == _DOWN && _pacman[i]->position->Y < _pacman[j]->position->Y)
				{
					_pacman[j]->position->X = -666;
				}
				else if (_pacman[j]->direction == _LEFT && _pacman[i]->direction == _FORW && _pacman[i]->position->Y > _pacman[j]->position->Y)
				{
					_pacman[j]->position->X = -666;
				}

				if (_pacman[j]->direction == _FORW && _pacman[i]->direction == _RIGHT && _pacman[i]->position->X < _pacman[j]->position->X)
				{
					_pacman[j]->position->X = -666;
				}
				else if (_pacman[j]->direction == _FORW && _pacman[i]->direction == _LEFT && _pacman[i]->position->X > _pacman[j]->position->X)
				{
					_pacman[j]->position->X = -666;
				}
				else if (_pacman[j]->direction == _FORW && _pacman[i]->direction == _FORW && _pacman[i]->position->Y > _pacman[j]->position->Y)
				{
					_pacman[j]->position->X = -666;
				}
			}
		}
	}

	CanMoveSet(pacNum, _FORW, upKey, state);
	CanMoveSet(pacNum, _LEFT, leftKey, state);
	CanMoveSet(pacNum, _DOWN, downKey, state);
	CanMoveSet(pacNum, _RIGHT, rightKey, state);
}

void Pacman::Input(int elapsedTime, Input::KeyboardState* state) 
{
	// Checks if key is pressed, Pacs 1 to 4
	InputSet(elapsedTime, state, Input::Keys::W, Input::Keys::A, Input::Keys::S, Input::Keys::D, 0);
	InputSet(elapsedTime, state, Input::Keys::I, Input::Keys::J, Input::Keys::K, Input::Keys::L, 1);
	InputSet(elapsedTime, state, Input::Keys::UP, Input::Keys::LEFT, Input::Keys::DOWN, Input::Keys::RIGHT, 2);
	InputSet(elapsedTime, state, Input::Keys::NUMPAD5, Input::Keys::NUMPAD1, Input::Keys::NUMPAD2, Input::Keys::NUMPAD3, 3);

	for (int i = 0; i < 4; i++)
	{
		_pacman[i]->canAnimate = true;		//Put here once to save space
		if (_pacman[i]->direction == _RIGHT && _pacman[i]->canInput[_RIGHT] == true)
		{
			_pacman[i]->position->X += _pacman[i]->speedMultiplier * elapsedTime; //Moves Pacman forward across X axis
		}
		else if (_pacman[i]->direction == _DOWN && _pacman[i]->canInput[_DOWN] == true)
		{
			_pacman[i]->position->Y += _pacman[i]->speedMultiplier * elapsedTime; //Moves Pacman forward across Y axis
		}
		else if (_pacman[i]->direction == _LEFT && _pacman[i]->canInput[_LEFT] == true)
		{
			_pacman[i]->position->X -= _pacman[i]->speedMultiplier * elapsedTime; //Moves Pacman back across X axis
		}
		else if (_pacman[i]->direction == _FORW && _pacman[i]->canInput[_FORW] == true)	
		{
			_pacman[i]->position->Y -= _pacman[i]->speedMultiplier * elapsedTime; //Moves Pacman back across Y axis
		}
		else
		{
			_pacman[i]->canAnimate = false;
		}
	}
}

void Pacman::CheckViewportCollision()
{
	for (int i = 0; i < 4; i++)
	{
		// Screen loop
		if (_pacman[i]->position->X > Graphics::GetViewportWidth())
		{
			_pacman[i]->position->X = -static_cast<float>(_pacman[i]->sourceRect->Width);	//One is float, other is int
		}
		if (_pacman[i]->position->X < -_pacman[i]->sourceRect->Width)
		{
			_pacman[i]->position->X = static_cast<float>(Graphics::GetViewportWidth());
		}
		if (_pacman[i]->position->Y > Graphics::GetViewportHeight())
		{
			_pacman[i]->position->Y = -static_cast<float>(_pacman[i]->sourceRect->Height);
		}
		if (_pacman[i]->position->Y < -_pacman[i]->sourceRect->Height)
		{
			_pacman[i]->position->Y = static_cast<float>(Graphics::GetViewportHeight());
		}
	}
}

void Pacman::UpdatePacman(int elapsedTime, Input::KeyboardState* state)
{
	if (_pacman[0]->canAnimate && !_paused)
	{
		_pacman[0]->currentFrameTime += elapsedTime;
		if (_pacman[0]->currentFrameTime > _cPacmanFrameTime)
		{
			_pacman[0]->frame++;
			if (_pacman[0]->frame >= 2)
				_pacman[0]->frame = 0;

			if(_pacman[0]->speedMultiplier == 0.2f)
				_pacman[0]->currentFrameTime = 0;
			else
				_pacman[0]->currentFrameTime = 125;
		}
	}

	if (_pacman[1]->canAnimate && !_paused)
	{
		_pacman[1]->currentFrameTime += elapsedTime;
		if (_pacman[1]->currentFrameTime > _cPacmanFrameTime)
		{
			_pacman[1]->frame++;
			if (_pacman[1]->frame >= 2)
				_pacman[1]->frame = 0;

			if (_pacman[1]->speedMultiplier == 0.2f)
				_pacman[1]->currentFrameTime = 0;
			else if(_pacman[1]->speedMultiplier == 0.4f)
				_pacman[1]->currentFrameTime = 125;
			else
				_pacman[1]->currentFrameTime = 250;
		}
	}

	if (_pacman[2]->canAnimate && !_paused)
	{
		_pacman[2]->currentFrameTime += elapsedTime;
		if (_pacman[2]->currentFrameTime > _cPacmanFrameTime)
		{
			_pacman[2]->frame++;
			if (_pacman[2]->frame >= 2)
				_pacman[2]->frame = 0;

			if (_pacman[2]->speedMultiplier == 0.2f)
				_pacman[2]->currentFrameTime = 0;
			else
				_pacman[2]->currentFrameTime = 125;
		}
	}

	if (_pacman[3]->canAnimate && !_paused)
	{
		_pacman[3]->currentFrameTime += elapsedTime;
		if (_pacman[3]->currentFrameTime > _cPacmanFrameTime)
		{
			_pacman[3]->frame++;
			if (_pacman[3]->frame >= 2)
				_pacman[3]->frame = 0;

			if (_pacman[3]->speedMultiplier == 0.2f)
				_pacman[3]->currentFrameTime = 0;
			else
				_pacman[3]->currentFrameTime = 125;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		//Change direction, animate
		_pacman[i]->sourceRect->Y = _pacman[i]->sourceRect->Height * static_cast<float>(_pacman[i]->direction);
		_pacman[i]->sourceRect->X = _pacman[i]->sourceRect->Width * _pacman[i]->frame;
	}
}

void Pacman::UpdateMunchie(int elapsedTime)
{
	if (!_paused)
	{
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
		{
			Collectable* tempMunch = _munchie[j];

			tempMunch->sourceRect->X = tempMunch->sourceRect->Width * munchieFrame;
		}
	}
}

void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey)
{
	if (state->IsKeyDown(pauseKey) && !_pKeyDown && !_helpmenu)
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
	{
		_helpmenu = false;
	}
}

void Pacman::CheckStart(Input::KeyboardState* state)
{
	if (state->IsKeyDown(Input::Keys::S) && !_dirKeyDown && !_retKeyDown && (_paused || _startmenu))	//So that arrow only moves in menu, not while game is played
	{
		if (_arrowPlace >= 2)
		{
			_arrowPlace = 0;
		}
		else
		{
			_arrowPlace++;
		}
		_dirKeyDown = true;
	}

	if (state->IsKeyDown(Input::Keys::W) && !_dirKeyDown && (_paused || _startmenu))	//So that arrow only moves in menu, not while game is played
	{
		if (_arrowPlace <= 0)
		{
			_arrowPlace = 2;
		}
		else
		{
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
		if (_arrowPlace == 0)
		{
			_startmenu = false;
			_paused = false;
		}
		else if (_arrowPlace == 1)
			_helpmenu = true;
		else if (_arrowPlace == 2)
			Graphics::Destroy();

		_retKeyDown = true;
	}
}

void Pacman::RedCherry(int i, int elapsedTime)
{
	//Remove speed buff if timer reduced to zero
	if (_pacman[i]->speedCurrentFrameTime > 0)
		_pacman[i]->speedCurrentFrameTime -= elapsedTime;
	if (_pacman[i]->speedCurrentFrameTime <= 0)
	{
		if (_pacman[i]->speedMultiplier == 0.4f)
		{
			_pacman[i]->speedMultiplier = 0.2f;
			_pacman[i]->speedCurrentFrameTime = 0;
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
		if (_pacman[i]->speedMultiplier == 0.0f)
		{
			_pacman[i]->speedMultiplier = 0.2f;
			frozenTime = 0;
		}
	}
}

void Pacman::GreenCherry(int i, Input::MouseState* state)
{
	if (pacmanWithGreen != -1)
	{
		_target->position->X = state->X;
		_target->position->Y = state->Y;
	}
	else
	{
		_target->position->X = -200;
	}
}

void Pacman::MunchieCollInteraction(int i)
{
	//collision between munchie and pacman
	for (int j = 0; j < (int)_munchie.size(); ++j)
	{
		Collectable* tempMunch = _munchie[j];
		if (MunchieCollisionDetection(_pacman[i]->position->X, _pacman[i]->position->Y, 27, 27, tempMunch->position->X, tempMunch->position->Y, 12, 12) == true && tempMunch->collectedTime <= 0)
		{
			tempMunch->texture->Load("Textures/MunchiesEmpty.png", false);
			tempMunch->collectedTime = 60;

			/*std::cout << "Test." << std::endl;
			if (!Audio::IsInitialised())
			{
				std::cout << "Not init." << std::endl;
			}
			if (!_pop->IsLoaded())
			{
				std::cout << "Not load." << std::endl;
			}*/

			Audio::Play(_pop);
			if (i == 0 || i == 1)
			{
				_teamScores[RED]++;
			}
			else if (i == 2 || i == 3)
			{
				_teamScores[BLUE]++;
			}
		}
	}
}

void Pacman::PowerupCollInteraction(int i)
{
	//collision between powerup and pacman
	for (int j = 0; j < (int)_powerup.size(); ++j)
	{
		Collectable* tempPower = _powerup[j];
		if (MunchieCollisionDetection(_pacman[i]->position->X, _pacman[i]->position->Y, 27, 27, tempPower->position->X, tempPower->position->Y, 24, 24) == true)
		{
			if (tempPower->type == 1)
			{
				_pacman[i]->speedCurrentFrameTime = 5000;
				_pacman[i]->speedMultiplier = 0.4f;
				_pacman[i]->currentFrameTime += 125;
			}
			else if (tempPower->type == 2)
			{
				if (i == 0 || i == 1)
				{
					_pacman[2]->speedMultiplier = 0.0f;
					_pacman[3]->speedMultiplier = 0.0f;
				}
				else if (i == 2 || i == 3)
				{
					_pacman[0]->speedMultiplier = 0.0f;
					_pacman[1]->speedMultiplier = 0.0f;
				}
				frozenTime = 5000;
			}
			else if (tempPower->type == 3)
			{
				pacmanWithGreen = i;
			}
			else if (tempPower->type == 4)
			{

			}
			tempPower->position->X = -200;
		}
	}
}

void Pacman::Update(int elapsedTime)
{
	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	Input::MouseState* mouseState = Input::Mouse::GetState();

	if (!_startmenu)
	{
		if (!_paused)
		{
			Input(elapsedTime, keyboardState);
			CheckViewportCollision();
			
			for (int i = 0; i < 4; i++)		//Collectable detection + action
			{
				RedCherry(i, elapsedTime);
				BlueCherry(i, elapsedTime);
				GreenCherry(i, mouseState);
				MunchieCollInteraction(i);
				PowerupCollInteraction(i);
			}
		}

		UpdatePacman(elapsedTime, keyboardState);
		UpdateMunchie(elapsedTime);
		CheckPaused(keyboardState, Input::Keys::P);
	}
	else if(!_helpmenu)
	{
		CheckStart(keyboardState);
	}
	else if (_helpmenu)
	{
		if (keyboardState->IsKeyDown(Input::Keys::SPACE))
		{
			_helpmenu = false;
		}
	}
}

void Pacman::DrawTiles()
{
	for (int y = 0; y < GetHeight(); y++)
	{
		for (int x = 0; x < GetWidth(); x++)
		{
			// If there is a visible tile in that position
			Texture2D* texture = _tiles->at(x).at(y)->Texture;
			if (texture != nullptr)
			{
				// Draw it in screen space.
				Vector2 position((float)x, (float)y);
				position *= *Tile::Size;
				SpriteBatch::Draw(texture, &position);
				//delete texture;
			}
		}
	}
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	stringstream stream;
	
	SpriteBatch::BeginDraw(); // Starts Drawing
	stream << _pacman[0]->direction;

	DrawTiles();		//Draws level
	
	// Draw Munchie
	for (int i = 0; i < (int)_munchie.size(); ++i)
	{
		Collectable* tempMunch = _munchie[i];
		SpriteBatch::Draw(tempMunch->texture, tempMunch->position, tempMunch->sourceRect); // Draws Munchies
	}

	for (int i = 0; i < (int)_powerup.size(); ++i)
	{
		Collectable* tempPower = _powerup[i];
		SpriteBatch::Draw(tempPower->texture, tempPower->position, tempPower->sourceRect); // Draws Powerups
	}
	
	for (int i = 0; i < 4; i++)
	{
		SpriteBatch::Draw(_pacman[i]->texture, _pacman[i]->position, _pacman[i]->sourceRect); // Draws Pacman
	}

	SpriteBatch::Draw(_target->texture, _target->position, _target->sourceRect);
	
	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);

	stream.str(string());
	if (_paused)
	{
		SpriteBatch::Draw(_menu->texture, _menu->sourceRect, nullptr);

		SpriteBatch::Draw(_arrow->texture, _arrow->position, _arrow->sourceRect);
		_arrow->position->Y = Graphics::GetViewportHeight() / 2.0f + 48 + (64 * _arrowPlace);
		
		stream << "-PAUSED-";
		SpriteBatch::DrawString(stream.str().c_str(), _titleStringPosition, Color::White);
		stream.str(string());

		stream << "Resume Game";
		SpriteBatch::DrawString(stream.str().c_str(), _startStringPosition, Color::White);
		stream.str(string());

		stream << "How To Play";
		SpriteBatch::DrawString(stream.str().c_str(), _helpStringPosition, Color::White);
		stream.str(string());

		stream << "Quit";
		SpriteBatch::DrawString(stream.str().c_str(), _quitStringPosition, Color::White);
		stream.str(string());
	}

	if (_startmenu)
	{
		SpriteBatch::Draw(_start->texture, _start->sourceRect, nullptr);
		_arrow->position->Y = Graphics::GetViewportHeight() / 2.0f + 48 + (64 * _arrowPlace);

		if (!_helpmenu)
		{
			SpriteBatch::Draw(_arrow->texture, _arrow->position, _arrow->sourceRect);
			
			stream << "PACMAN";
			SpriteBatch::DrawString(stream.str().c_str(), _titleStringPosition, Color::White);
			stream.str(string());

			stream << "Start Game";
			SpriteBatch::DrawString(stream.str().c_str(), _startStringPosition, Color::White);
			stream.str(string());

			stream << "How To Play";
			SpriteBatch::DrawString(stream.str().c_str(), _helpStringPosition, Color::White);
			stream.str(string());

			stream << "Quit";
			SpriteBatch::DrawString(stream.str().c_str(), _quitStringPosition, Color::White);
			stream.str(string());
		}
	}

	// Help menu
	if (_helpmenu)
	{
		SpriteBatch::Draw(_start->texture, _start->sourceRect, nullptr);

		SpriteBatch::Draw(_controls[0]->texture, _controls[0]->sourceRect, nullptr);
		SpriteBatch::Draw(_controls[1]->texture, _controls[1]->sourceRect, nullptr);
		SpriteBatch::Draw(_controls[2]->texture, _controls[2]->sourceRect, nullptr);
		SpriteBatch::Draw(_controls[3]->texture, _controls[3]->sourceRect, nullptr);
		
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
	}

	SpriteBatch::EndDraw(); // Ends Drawing
}

Collectable::Collectable(float x, float y, int isPowerup)
{
	texture = new Texture2D;
	position = new Vector2(x * 32.0f, y * 32.0f);
	collectedTime = 0;
	if (isPowerup == 0)
	{
		sourceRect = new Rect(0.0f, 0.0f, 12, 12);
		texture->Load("Textures/Munchies.png", false);
		position->X += 10;
		position->Y += 10;
	}	
	else
	{
		sourceRect = new Rect(0.0f, 0.0f, 24, 24);
		position->X += 4;
		position->Y += 4;
	}

	if (isPowerup == 1)
	{
		texture->Load("Textures/cherryred.png", false);
		type = 1;
	}
	if (isPowerup == 2)
	{
		texture->Load("Textures/cherryblue.png", false);
		type = 2;
	}
	if (isPowerup == 3)
	{
		texture->Load("Textures/cherrygreen.png", false);
		type = 3;
	}
	if (isPowerup == 4)
	{
		texture->Load("Textures/cherryyellow.png", false);
		type = 4;
	}
}

Tile::Tile(Texture2D* texture, bool isSolid)
{
	Texture = texture;
	isSolid = isSolid;
}

Tile::~Tile(void)
{
	delete Texture;
}