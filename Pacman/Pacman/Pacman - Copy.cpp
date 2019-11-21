#include "Pacman.h"

#include <sstream>
#include <math.h>


Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanFrameTime(250), _cMunchieFrameTime(500)
{
	for (int i = 0; i < 4; i++)
	{
		_pacman[i] = new Player();
		_pacman[i]->direction = 0;
		_pacman[i]->speedMultiplier = 0.2f;
		for (int j = 0; j < 4; j++)
		{
			canInput[i][j] = true;
		}
		_controls[i] = new Graphic();
	}

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchie[i] = new Collectable();
	}
	
	_arrow = new Graphic();
	_menu = new Graphic();
	_start = new Graphic();
	_paused = false;
	_startmenu = true;
	_helpmenu = false;
	_pKeyDown = false;
	_dirKeyDown = false;
	_retKeyDown = false;
	_teamScores[0] = 0;
	_teamScores[1] = 0;

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

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
		delete _controls[i]->texture;
		delete _controls[i]->sourceRect;
		delete _controls[i]->pos;
	}
	delete[] _pacman;

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		delete _munchie[i]->texture;
		delete _munchie[i]->sourceRect;
		delete _munchie[i]->position;
	}
	delete[] _munchie;

	delete _menu->texture;
	delete _menu->sourceRect;

	delete _start->texture;

	delete _arrow->texture;
	delete _arrow->pos;
	delete _arrow->sourceRect;

	delete _menuStringPosition;
	delete _titleStringPosition;
	delete _startStringPosition;
	delete _helpStringPosition;
	delete _quitStringPosition;
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

/*signed int Pacman::PacmanCollisionDetection(float x1, float y1, float width1, float height1, float x2, float y2, float width2, float height2, int dir1, int dir2)
{
	float left1 = x1;
	float left2 = x2;
	float right1 = x1 + width1;
	float right2 = x2 + width2;
	float top1 = y1;
	float top2 = y2;
	float bottom1 = y1 + height1;
	float bottom2 = y2 + height2;

	//1 is being blocked, 2 is blocker
	//-1 = none,  0 = right, 1 = down, 2 = left, 3 = up, 4 = kill

	if (bottom1 < top2)
		return -1;
	if (top1 > bottom2)
		return -1;
	if (right1 < left2)
		return -1;
	if (left1 > right2)
		return -1;

	if (bottom1 > top2)
		return 3;
	if (top1 < bottom2)
		return 1;
	if (right1 > left2)
		return 2;
	if (left1 < right2)
		return 0;
}*/

void Pacman::LoadContent()
{
	//Help menu graphics
	for (int i = 0; i < 4; i++)
	{
		_controls[i]->texture = new Texture2D();
		_pacman[i]->currentFrameTime = 0;
		_pacman[i]->frame = 0;
	}
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
	_arrow->pos = new Vector2((Graphics::GetViewportWidth() / 2.0f) + 128, (Graphics::GetViewportHeight() / 2.0f) + 48);
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

	//Text for help screen
	_p1Pos = new Vector2(260.0f, 200.0f);
	_p2Pos = new Vector2(Graphics::GetViewportWidth() - 340.0f, 200.0f);
	_p3Pos = new Vector2(260.0f, Graphics::GetViewportHeight() - 250.0f);
	_p4Pos = new Vector2(Graphics::GetViewportWidth() - 340.0f, Graphics::GetViewportHeight() - 250.0f);
	_spacePos = new Vector2((Graphics::GetViewportWidth() / 2) - 100.0f, Graphics::GetViewportHeight() - 50.0f);

	// Load Pacmans
	_pacman[0]->texture = new Texture2D();
	_pacman[0]->texture->Load("Textures/Pacman_Red.tga", false);
	_pacman[0]->position = new Vector2(350.0f, 350.0f);
	_pacman[0]->sourceRect = new Rect(0.0f, 0.0f, 27, 27);

	_pacman[1]->texture = new Texture2D();
	_pacman[1]->texture->Load("Textures/Pacman_Red.tga", false);
	_pacman[1]->position = new Vector2(Graphics::GetViewportWidth() - 350.0f, 350.0f);
	_pacman[1]->sourceRect = new Rect(0.0f, 0.0f, 27, 27);

	_pacman[2]->texture = new Texture2D();
	_pacman[2]->texture->Load("Textures/Pacman_Blue.tga", false);
	_pacman[2]->position = new Vector2(350.0f, Graphics::GetViewportHeight() - 350.0f);
	_pacman[2]->sourceRect = new Rect(0.0f, 0.0f, 27, 27);

	_pacman[3]->texture = new Texture2D();
	_pacman[3]->texture->Load("Textures/Pacman_Blue.tga", false);
	_pacman[3]->position = new Vector2(Graphics::GetViewportWidth() - 350.0f, Graphics::GetViewportHeight() - 350.0f);
	_pacman[3]->sourceRect = new Rect(0.0f, 0.0f, 27, 27);
	
	// Load Munchie

	Texture2D* _munchieTexture = new Texture2D;
	_munchieTexture->Load("Textures/Munchies.png", true);
	Rect* _munchieRect = new Rect(0.0f, 0.0f, 12, 12);
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchie[i]->texture = _munchieTexture;
		_munchie[i]->position = new Vector2(100.0f, 450.0f);
		_munchie[i]->sourceRect = _munchieRect;
		_munchie[i]->currentFrameTime = 0;
		_munchie[i]->frame = 0;
	}

	_munchie[0]->position = new Vector2(100.0f, 450.0f);

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);
}

void Pacman::InputSet(int elapsedTime, Input::KeyboardState* state, Input::Keys upKey, Input::Keys leftKey, Input::Keys downKey, Input::Keys rightKey, int pacNum)
{
	for (int j = 0; j < 4; j++)
	{
		for (int i = 0; i < 4; i++)
		{
			if (sqrtf((_pacman[j]->position->X - _pacman[i]->position->X) * (_pacman[j]->position->X - _pacman[i]->position->X) + (_pacman[j]->position->Y - _pacman[i]->position->Y) * (_pacman[j]->position->Y - _pacman[i]->position->Y)) < 27 && i != j)
			{
				if (_pacman[j]->direction == 0 && _pacman[i]->direction == 2)
				{
					canInput[j][0] = false;
					canInput[i][2] = false;
				}
				else if (_pacman[j]->direction == 2 && _pacman[i]->direction == 0)
				{
					canInput[j][2] = false;
					canInput[i][0] = false;
				}
				else if (_pacman[j]->direction == 1 && _pacman[i]->direction == 3)
				{
					canInput[j][1] = false;
					canInput[i][3] = false;
				}
				else if (_pacman[j]->direction == 3 && _pacman[i]->direction == 1)
				{
					canInput[j][3] = false;
					canInput[i][1] = false;
				}

				//"i" is the killer, "j" gets killed.
				//IMPORTANT-COLLISION INCOMPLETE!!!
				if (_pacman[j]->direction == 0 && _pacman[i]->direction == 0 && _pacman[i]->position->X < _pacman[j]->position->X)
				{
					_pacman[j]->position->X = -666;
				}
				else if (_pacman[j]->direction == 0 && _pacman[i]->direction == 1 && _pacman[i]->position->Y < _pacman[j]->position->Y)
				{
					_pacman[j]->position->X = -666;
				}
				else if (_pacman[j]->direction == 0 && _pacman[i]->direction == 3 && _pacman[i]->position->Y > _pacman[j]->position->Y)
				{
					_pacman[j]->position->X = -666;
				}

				if (_pacman[j]->direction == 1 && _pacman[i]->direction == 0 && _pacman[i]->position->X < _pacman[j]->position->X)
				{
					_pacman[j]->position->X = -666;
				}
				else if (_pacman[j]->direction == 1 && _pacman[i]->direction == 1 && _pacman[i]->position->Y < _pacman[j]->position->Y)
				{
					_pacman[j]->position->X = -666;
				}
				else if (_pacman[j]->direction == 1 && _pacman[i]->direction == 2 && _pacman[i]->position->X > _pacman[j]->position->X)
				{
					_pacman[j]->position->X = -666;
				}

				if (_pacman[j]->direction == 2 && _pacman[i]->direction == 2 && _pacman[i]->position->X > _pacman[j]->position->X)
				{
					_pacman[j]->position->X = -666;
				}
				else if (_pacman[j]->direction == 2 && _pacman[i]->direction == 1 && _pacman[i]->position->Y < _pacman[j]->position->Y)
				{
					_pacman[j]->position->X = -666;
				}
				else if (_pacman[j]->direction == 2 && _pacman[i]->direction == 3 && _pacman[i]->position->Y > _pacman[j]->position->Y)
				{
					_pacman[j]->position->X = -666;
				}

				if (_pacman[j]->direction == 3 && _pacman[i]->direction == 0 && _pacman[i]->position->X < _pacman[j]->position->X)
				{
					_pacman[j]->position->X = -666;
				}
				else if (_pacman[j]->direction == 3 && _pacman[i]->direction == 2 && _pacman[i]->position->X > _pacman[j]->position->X)
				{
					_pacman[j]->position->X = -666;
				}
				else if (_pacman[j]->direction == 3 && _pacman[i]->direction == 3 && _pacman[i]->position->Y > _pacman[j]->position->Y)
				{
					_pacman[j]->position->X = -666;
				}
			}
		}
	}

	if (state->IsKeyDown(upKey) && canInput[pacNum][3] == true)		//If up key is pressed, and if this pacman can input...
	{
		_pacman[pacNum]->position->Y -= _pacman[pacNum]->speedMultiplier * elapsedTime; //Moves Pacman back across Y axis
		_pacman[pacNum]->direction = 3;
	}
	else if (state->IsKeyDown(upKey) && canInput[pacNum][3] == false)
	{
		canInput[pacNum][3] = true;
	}
	else if (state->IsKeyDown(leftKey) && canInput[pacNum][2] == true)
	{
		_pacman[pacNum]->position->X -= _pacman[pacNum]->speedMultiplier * elapsedTime; //Moves Pacman back across X axis
		_pacman[pacNum]->direction = 2;
	}
	else if (state->IsKeyDown(leftKey) && canInput[pacNum][2] == false)
	{
		canInput[pacNum][2] = true;
	}
	else if (state->IsKeyDown(downKey) && canInput[pacNum][1] == true)
	{
		_pacman[pacNum]->position->Y += _pacman[pacNum]->speedMultiplier * elapsedTime; //Moves Pacman forward across Y axis
		_pacman[pacNum]->direction = 1;
	}
	else if (state->IsKeyDown(downKey) && canInput[pacNum][1] == false)
	{
		canInput[pacNum][1] = true;
	}
	else if (state->IsKeyDown(rightKey) && canInput[pacNum][0] == true)
	{
		_pacman[pacNum]->position->X += _pacman[pacNum]->speedMultiplier * elapsedTime; //Moves Pacman forward across X axis
		_pacman[pacNum]->direction = 0;
	}
	else if (state->IsKeyDown(rightKey) && canInput[pacNum][0] == false)
	{
		canInput[pacNum][0] = true;
	}
}

void Pacman::Input(int elapsedTime, Input::KeyboardState* state) 
{
	// Checks if key is pressed, Pacs 1 to 4
	InputSet(elapsedTime, state, Input::Keys::W, Input::Keys::A, Input::Keys::S, Input::Keys::D, 0);
	InputSet(elapsedTime, state, Input::Keys::I, Input::Keys::J, Input::Keys::K, Input::Keys::L, 1);
	InputSet(elapsedTime, state, Input::Keys::UP, Input::Keys::LEFT, Input::Keys::DOWN, Input::Keys::RIGHT, 2);
	InputSet(elapsedTime, state, Input::Keys::NUMPAD5, Input::Keys::NUMPAD1, Input::Keys::NUMPAD2, Input::Keys::NUMPAD3, 3);
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
	if ((state->IsKeyDown(Input::Keys::W) || state->IsKeyDown(Input::Keys::A) || state->IsKeyDown(Input::Keys::S) || state->IsKeyDown(Input::Keys::D)) && !_paused)
	{
		_pacman[0]->currentFrameTime += elapsedTime;
		if (_pacman[0]->currentFrameTime > _cPacmanFrameTime)
		{
			_pacman[0]->frame++;
			if (_pacman[0]->frame >= 2)
				_pacman[0]->frame = 0;

			_pacman[0]->currentFrameTime = 0;
		}
	}

	if ((state->IsKeyDown(Input::Keys::I) || state->IsKeyDown(Input::Keys::J) || state->IsKeyDown(Input::Keys::K) || state->IsKeyDown(Input::Keys::L)) && !_paused)
	{
		_pacman[1]->currentFrameTime += elapsedTime;
		if (_pacman[1]->currentFrameTime > _cPacmanFrameTime)
		{
			_pacman[1]->frame++;
			if (_pacman[1]->frame >= 2)
				_pacman[1]->frame = 0;

			_pacman[1]->currentFrameTime = 0;
		}
	}

	if ((state->IsKeyDown(Input::Keys::UP) || state->IsKeyDown(Input::Keys::LEFT) || state->IsKeyDown(Input::Keys::DOWN) || state->IsKeyDown(Input::Keys::RIGHT)) && !_paused)
	{
		_pacman[2]->currentFrameTime += elapsedTime;
		if (_pacman[2]->currentFrameTime > _cPacmanFrameTime)
		{
			_pacman[2]->frame++;
			if (_pacman[2]->frame >= 2)
				_pacman[2]->frame = 0;

			_pacman[2]->currentFrameTime = 0;
		}
	}

	if ((state->IsKeyDown(Input::Keys::NUMPAD5) || state->IsKeyDown(Input::Keys::NUMPAD1) || state->IsKeyDown(Input::Keys::NUMPAD2) || state->IsKeyDown(Input::Keys::NUMPAD3)) && !_paused)
	{
		_pacman[3]->currentFrameTime += elapsedTime;
		if (_pacman[3]->currentFrameTime > _cPacmanFrameTime)
		{
			_pacman[3]->frame++;
			if (_pacman[3]->frame >= 2)
				_pacman[3]->frame = 0;

			_pacman[3]->currentFrameTime = 0;
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
		for (int i = 0; i < MUNCHIECOUNT; i++)
		{
			_munchie[i]->currentFrameTime += elapsedTime;
			if (_munchie[i]->currentFrameTime > _cMunchieFrameTime)
			{
				_munchie[i]->frame++;
				if (_munchie[i]->frame >= 2)
					_munchie[i]->frame = 0;

				_munchie[i]->currentFrameTime = 0;
			}
			_munchie[i]->sourceRect->X = _munchie[i]->sourceRect->Width * _munchie[i]->frame;
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

void Pacman::Update(int elapsedTime)
{
	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();

	if (!_startmenu)
	{
		if (!_paused)
		{
			Input(elapsedTime, keyboardState);
			CheckViewportCollision();
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < MUNCHIECOUNT; j++)
				{
					if (MunchieCollisionDetection(_pacman[i]->position->X, _pacman[i]->position->Y, 27, 27, _munchie[j]->position->X, _munchie[j]->position->Y, 12, 12) == true)
					{
						_munchie[j]->position->X = -200;
					}
				}
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

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	
	//stream << distanceFromPacToThis[1]

	SpriteBatch::BeginDraw(); // Starts Drawing
	for (int i = 0; i < 4; i++)
	{
		SpriteBatch::Draw(_pacman[i]->texture, _pacman[i]->position, _pacman[i]->sourceRect); // Draws Pacman
	}
	
	// Draw Munchie
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		SpriteBatch::Draw(_munchie[i]->texture, _munchie[i]->position, _munchie[i]->sourceRect); // Draws Munchies
	}
	
	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);

	stream.str(std::string());
	if (_paused)
	{
		SpriteBatch::Draw(_menu->texture, _menu->sourceRect, nullptr);

		SpriteBatch::Draw(_arrow->texture, _arrow->pos, _arrow->sourceRect);
		_arrow->pos->Y = Graphics::GetViewportHeight() / 2.0f + 48 + (64 * _arrowPlace);
		
		stream << "-PAUSED-";
		SpriteBatch::DrawString(stream.str().c_str(), _titleStringPosition, Color::Red);
		stream.str(std::string());

		stream << "Resume Game";
		SpriteBatch::DrawString(stream.str().c_str(), _startStringPosition, Color::Red);
		stream.str(std::string());

		stream << "How To Play";
		SpriteBatch::DrawString(stream.str().c_str(), _helpStringPosition, Color::Red);
		stream.str(std::string());

		stream << "Quit";
		SpriteBatch::DrawString(stream.str().c_str(), _quitStringPosition, Color::Red);
		stream.str(std::string());
	}

	if (_startmenu)
	{
		SpriteBatch::Draw(_start->texture, _start->sourceRect, nullptr);
		_arrow->pos->Y = Graphics::GetViewportHeight() / 2.0f + 48 + (64 * _arrowPlace);

		if (!_helpmenu)
		{
			SpriteBatch::Draw(_arrow->texture, _arrow->pos, _arrow->sourceRect);
			
			stream << "PACMAN";
			SpriteBatch::DrawString(stream.str().c_str(), _titleStringPosition, Color::Red);
			stream.str(std::string());

			stream << "Start Game";
			SpriteBatch::DrawString(stream.str().c_str(), _startStringPosition, Color::Red);
			stream.str(std::string());

			stream << "How To Play";
			SpriteBatch::DrawString(stream.str().c_str(), _helpStringPosition, Color::Red);
			stream.str(std::string());

			stream << "Quit";
			SpriteBatch::DrawString(stream.str().c_str(), _quitStringPosition, Color::Red); 
			stream.str(std::string());
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
		stream.str(std::string());

		stream << "Player 2";
		SpriteBatch::DrawString(stream.str().c_str(), _p2Pos, Color::Red);
		stream.str(std::string());

		stream << "Player 3";
		SpriteBatch::DrawString(stream.str().c_str(), _p3Pos, Color::Cyan);
		stream.str(std::string());

		stream << "Player 4";
		SpriteBatch::DrawString(stream.str().c_str(), _p4Pos, Color::Cyan);
		stream.str(std::string());

		stream << "Press SPACE to go back";
		SpriteBatch::DrawString(stream.str().c_str(), _spacePos, Color::Red); 
		stream.str(std::string());
	}

	SpriteBatch::EndDraw(); // Ends Drawing
}