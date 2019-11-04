#include "Pacman.h"

#include <sstream>


Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.2f), _cPacmanFrameTime(250), _cMunchieFrameTime(500)
{
	_pacmanDir[0] = 0;
	_pacmanDir[1] = 0;
	_pacmanDir[2] = 0;
	_pacmanDir[3] = 0;
	_paused = false;
	_startmenu = true;
	_helpmenu = false;
	_pKeyDown = false;
	_dirKeyDown = false;
	_retKeyDown = false;

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	// remember, DO CLEAN UP

	delete _pacmanTexture[0];
	delete _pacmanTexture[1];
	delete _pacmanTexture[2];
	delete _pacmanTexture[3];
	delete _pacmanSourceRect[0];
	delete _pacmanSourceRect[1];
	delete _pacmanSourceRect[2];
	delete _pacmanSourceRect[3];
	delete _munchieTexture;
	delete _munchieSourceRect;
	delete _menuBackground;
	delete _startBackground;
	delete _arrowTexture;
	delete _arrowPos;
	delete _menuRectangle;
	delete _arrowSourceRect;
	delete _menuStringPosition;
	delete _titleStringPosition;
	delete _startStringPosition;
	delete _helpStringPosition;
	delete _quitStringPosition;
}

void Pacman::LoadContent()
{
	_menuBackground = new Texture2D();
	_startBackground = new Texture2D();
	_arrowTexture = new Texture2D();

	//Help menu graphics
	_c1Texture = new Texture2D();
	_c2Texture = new Texture2D();
	_c3Texture = new Texture2D();
	_c4Texture = new Texture2D();

	_c1Texture->Load("Textures/controls1.tga", false);
	_c2Texture->Load("Textures/controls2.tga", false);
	_c3Texture->Load("Textures/controls3.tga", false);
	_c4Texture->Load("Textures/controls4.tga", false);

	_c1Rect = new Rect(200.0f, 50.0f, 192, 128);
	_c2Rect = new Rect(Graphics::GetViewportWidth() - 400.0f, 50.0f, 192, 128);
	_c3Rect = new Rect(200.0f, Graphics::GetViewportHeight() - 400.0f, 192, 128);
	_c4Rect = new Rect(Graphics::GetViewportWidth() - 400.0f, Graphics::GetViewportHeight() - 400.0f, 192, 128);

	//Start menu assets
	_arrowPos = new Vector2((Graphics::GetViewportWidth() / 2.0f) + 128, (Graphics::GetViewportHeight() / 2.0f) + 48);
	_menuBackground->Load("Textures/Transparency.png", false);
	_startBackground->Load("Textures/Start.png", false);
	_arrowTexture->Load("Textures/arrow.png", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_arrowSourceRect = new Rect(0.0f, 0.0f, 16, 16);
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

	_arrowPlace = 0;
	_arrowOffset = 0;

	// Load Pacman
	_pacmanTexture[0] = new Texture2D();
	_pacmanTexture[0]->Load("Textures/Pacman_Red.tga", false);
	_pacmanPosition[0] = new Vector2(350.0f, 350.0f);
	_pacmanSourceRect[0] = new Rect(0.0f, 0.0f, 32, 32);

	_pacmanTexture[1] = new Texture2D();
	_pacmanTexture[1]->Load("Textures/Pacman_Red.tga", false);
	_pacmanPosition[1] = new Vector2(Graphics::GetViewportWidth() - 350.0f, 350.0f);
	_pacmanSourceRect[1] = new Rect(0.0f, 0.0f, 32, 32);

	_pacmanTexture[2] = new Texture2D();
	_pacmanTexture[2]->Load("Textures/Pacman_Blue.tga", false);
	_pacmanPosition[2] = new Vector2(350.0f, Graphics::GetViewportHeight() - 350.0f);
	_pacmanSourceRect[2] = new Rect(0.0f, 0.0f, 32, 32);

	_pacmanTexture[3] = new Texture2D();
	_pacmanTexture[3]->Load("Textures/Pacman_Blue.tga", false);
	_pacmanPosition[3] = new Vector2(Graphics::GetViewportWidth() - 350.0f, Graphics::GetViewportHeight() - 350.0f);
	_pacmanSourceRect[3] = new Rect(0.0f, 0.0f, 32, 32);

	for (int i = 0; i < 4; i++)
	{
		_pacmanCurrentFrameTime[i] = 0;
		_pacmanFrame[i] = 0;
	}
	
	// Load Munchie
	_munchieTexture = new Texture2D();
	_munchieTexture->Load("Textures/Munchies.png", true);
	_munchiePosition = new Vector2(100.0f, 450.0f);
	_munchieSourceRect = new Rect(0.0f, 0.0f, 12, 12);
	_munchieCurrentFrameTime = 0;
	_munchieFrame = 0;

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);
}

void Pacman::Input(int elapsedTime, Input::KeyboardState* state) 
{
	// Checks if key is pressed, Pac1
	if (state->IsKeyDown(Input::Keys::W))
	{
		_pacmanPosition[0]->Y -= _cPacmanSpeed * elapsedTime; //Moves Pacman across Y axis
		_pacmanDir[0] = 3;
	}
	else if (state->IsKeyDown(Input::Keys::A))
	{
		_pacmanPosition[0]->X -= _cPacmanSpeed * elapsedTime; //Moves Pacman across X axis
		_pacmanDir[0] = 2;
	}
	else if (state->IsKeyDown(Input::Keys::S))
	{
		_pacmanPosition[0]->Y += _cPacmanSpeed * elapsedTime; //Moves Pacman across Y axis
		_pacmanDir[0] = 1;
	}
	else if (state->IsKeyDown(Input::Keys::D))
	{
		_pacmanPosition[0]->X += _cPacmanSpeed * elapsedTime; //Moves Pacman across X axis
		_pacmanDir[0] = 0;
	}

	// Checks if key is pressed, Pac2
	if (state->IsKeyDown(Input::Keys::I))
	{
		_pacmanPosition[1]->Y -= _cPacmanSpeed * elapsedTime;
		_pacmanDir[1] = 3;
	}
	else if (state->IsKeyDown(Input::Keys::J))
	{
		_pacmanPosition[1]->X -= _cPacmanSpeed * elapsedTime;
		_pacmanDir[1] = 2;
	}
	else if (state->IsKeyDown(Input::Keys::K))
	{
		_pacmanPosition[1]->Y += _cPacmanSpeed * elapsedTime;
		_pacmanDir[1] = 1;
	}
	else if (state->IsKeyDown(Input::Keys::L))
	{
		_pacmanPosition[1]->X += _cPacmanSpeed * elapsedTime;
		_pacmanDir[1] = 0;
	}

	// Checks if key is pressed, Pac3
	if (state->IsKeyDown(Input::Keys::UP))
	{
		_pacmanPosition[2]->Y -= _cPacmanSpeed * elapsedTime;
		_pacmanDir[2] = 3;
	}
	else if (state->IsKeyDown(Input::Keys::LEFT))
	{
		_pacmanPosition[2]->X -= _cPacmanSpeed * elapsedTime;
		_pacmanDir[2] = 2;
	}
	else if (state->IsKeyDown(Input::Keys::DOWN))
	{
		_pacmanPosition[2]->Y += _cPacmanSpeed * elapsedTime;
		_pacmanDir[2] = 1;
	}
	else if (state->IsKeyDown(Input::Keys::RIGHT))
	{
		_pacmanPosition[2]->X += _cPacmanSpeed * elapsedTime;
		_pacmanDir[2] = 0;
	}

	// Checks if key is pressed, Pac4
	if (state->IsKeyDown(Input::Keys::NUMPAD5))
	{
		_pacmanPosition[3]->Y -= _cPacmanSpeed * elapsedTime;
		_pacmanDir[3] = 3;
	}
	else if (state->IsKeyDown(Input::Keys::NUMPAD1))
	{
		_pacmanPosition[3]->X -= _cPacmanSpeed * elapsedTime;
		_pacmanDir[3] = 2;
	}
	else if (state->IsKeyDown(Input::Keys::NUMPAD2))
	{
		_pacmanPosition[3]->Y += _cPacmanSpeed * elapsedTime;
		_pacmanDir[3] = 1;
	}
	else if (state->IsKeyDown(Input::Keys::NUMPAD3))
	{
		_pacmanPosition[3]->X += _cPacmanSpeed * elapsedTime;
		_pacmanDir[3] = 0;
	}
}

void Pacman::CheckViewportCollision()
{
	for (int i = 0; i < 4; i++)
	{
		// Screen loop
		if (_pacmanPosition[i]->X > Graphics::GetViewportWidth())
		{
			_pacmanPosition[i]->X = -static_cast<float>(_pacmanSourceRect[i]->Width);	//One is float, other is int
		}
		if (_pacmanPosition[i]->X < -_pacmanSourceRect[i]->Width)
		{
			_pacmanPosition[i]->X = static_cast<float>(Graphics::GetViewportWidth());
		}
		if (_pacmanPosition[i]->Y > Graphics::GetViewportHeight())
		{
			_pacmanPosition[i]->Y = -static_cast<float>(_pacmanSourceRect[i]->Height);
		}
		if (_pacmanPosition[i]->Y < -_pacmanSourceRect[i]->Height)
		{
			_pacmanPosition[i]->Y = static_cast<float>(Graphics::GetViewportHeight());
		}
	}
}

void Pacman::UpdatePacman(int elapsedTime, Input::KeyboardState* state)
{
	if (state->IsKeyDown(Input::Keys::W) || state->IsKeyDown(Input::Keys::A) || state->IsKeyDown(Input::Keys::S) || state->IsKeyDown(Input::Keys::D))
	{
		_pacmanCurrentFrameTime[0] += elapsedTime;
		if (_pacmanCurrentFrameTime[0] > _cPacmanFrameTime)
		{
			_pacmanFrame[0]++;
			if (_pacmanFrame[0] >= 2)
				_pacmanFrame[0] = 0;

			_pacmanCurrentFrameTime[0] = 0;
		}
	}

	if (state->IsKeyDown(Input::Keys::I) || state->IsKeyDown(Input::Keys::J) || state->IsKeyDown(Input::Keys::K) || state->IsKeyDown(Input::Keys::L))
	{
		_pacmanCurrentFrameTime[1] += elapsedTime;
		if (_pacmanCurrentFrameTime[1] > _cPacmanFrameTime)
		{
			_pacmanFrame[1]++;
			if (_pacmanFrame[1] >= 2)
				_pacmanFrame[1] = 0;

			_pacmanCurrentFrameTime[1] = 0;
		}
	}

	if (state->IsKeyDown(Input::Keys::UP) || state->IsKeyDown(Input::Keys::LEFT) || state->IsKeyDown(Input::Keys::DOWN) || state->IsKeyDown(Input::Keys::RIGHT))
	{
		_pacmanCurrentFrameTime[2] += elapsedTime;
		if (_pacmanCurrentFrameTime[2] > _cPacmanFrameTime)
		{
			_pacmanFrame[2]++;
			if (_pacmanFrame[2] >= 2)
				_pacmanFrame[2] = 0;

			_pacmanCurrentFrameTime[2] = 0;
		}
	}

	if (state->IsKeyDown(Input::Keys::NUMPAD5) || state->IsKeyDown(Input::Keys::NUMPAD1) || state->IsKeyDown(Input::Keys::NUMPAD2) || state->IsKeyDown(Input::Keys::NUMPAD3))
	{
		_pacmanCurrentFrameTime[3] += elapsedTime;
		if (_pacmanCurrentFrameTime[3] > _cPacmanFrameTime)
		{
			_pacmanFrame[3]++;
			if (_pacmanFrame[3] >= 2)
				_pacmanFrame[3] = 0;

			_pacmanCurrentFrameTime[3] = 0;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		//Change direction, animate
		_pacmanSourceRect[i]->Y = _pacmanSourceRect[i]->Height * static_cast<float>(_pacmanDir[i]);
		_pacmanSourceRect[i]->X = _pacmanSourceRect[i]->Width * _pacmanFrame[i];
	}
}

void Pacman::UpdateMunchie(int elapsedTime)
{
	_munchieCurrentFrameTime += elapsedTime;
	if (_munchieCurrentFrameTime > _cMunchieFrameTime)
	{
		_munchieFrame++;
		if (_munchieFrame >= 2)
			_munchieFrame = 0;

		_munchieCurrentFrameTime = 0;
	}
	_munchieSourceRect->X = _munchieSourceRect->Width * _munchieFrame;
}

void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey)
{
	if (state->IsKeyDown(pauseKey) && !_pKeyDown)
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
	if (state->IsKeyDown(Input::Keys::S) && !_dirKeyDown && !_retKeyDown)
	{
		_arrowPos->Y = (Graphics::GetViewportHeight() / 2.0f) + 48;
		if (_arrowPlace >= 2)
		{
			_arrowPlace = 0;
		}
		else
		{
			_arrowPlace++;
		}
		_dirKeyDown = true;
		_arrowOffset = 64 * _arrowPlace;
	}

	if (state->IsKeyDown(Input::Keys::W) && !_dirKeyDown)
	{
		_arrowPos->Y = (Graphics::GetViewportHeight() / 2.0f) + 48;
		if (_arrowPlace <= 0)
		{
			_arrowPlace = 2;
		}
		else
		{
			_arrowPlace--;
		}
		_dirKeyDown = true;
		_arrowOffset = 64 * _arrowPlace;
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
		if (_arrowPlace == 2)
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
	stream << "Pacman X: " << _pacmanPosition[0]->X << " Y: " << _pacmanPosition[0]->Y;

	SpriteBatch::BeginDraw(); // Starts Drawing
	for (int i = 0; i < 4; i++)
	{
		SpriteBatch::Draw(_pacmanTexture[i], _pacmanPosition[i], _pacmanSourceRect[i]); // Draws Pacman
	}
	
	// Draw Munchie
	SpriteBatch::Draw(_munchieTexture, _munchiePosition, _munchieSourceRect); // Draws Pacman
	
	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);

	if (_paused)
	{
		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);

		SpriteBatch::Draw(_arrowTexture, _arrowPos, _arrowSourceRect);
		_arrowPos->Y += _arrowOffset;
		_arrowOffset = 0;

		std::stringstream titleStream;
		titleStream << "-PAUSED-";
		SpriteBatch::DrawString(titleStream.str().c_str(), _titleStringPosition, Color::Red);

		std::stringstream startStream;
		startStream << "Resume Game";
		SpriteBatch::DrawString(startStream.str().c_str(), _startStringPosition, Color::Red);

		std::stringstream helpStream;
		helpStream << "How To Play";
		SpriteBatch::DrawString(helpStream.str().c_str(), _helpStringPosition, Color::Red);

		std::stringstream quitStream;
		quitStream << "Quit";
		SpriteBatch::DrawString(quitStream.str().c_str(), _quitStringPosition, Color::Red);

		
	}

	if (_startmenu)
	{
		SpriteBatch::Draw(_startBackground, _menuRectangle, nullptr);
		_arrowPos->Y += _arrowOffset;
		_arrowOffset = 0;

		if (!_helpmenu)
		{
			SpriteBatch::Draw(_arrowTexture, _arrowPos, _arrowSourceRect);

			std::stringstream titleStream;
			titleStream << "PACMAN";
			SpriteBatch::DrawString(titleStream.str().c_str(), _titleStringPosition, Color::Red);

			std::stringstream startStream;
			startStream << "Start Game";
			SpriteBatch::DrawString(startStream.str().c_str(), _startStringPosition, Color::Red);

			std::stringstream helpStream;
			helpStream << "How To Play";
			SpriteBatch::DrawString(helpStream.str().c_str(), _helpStringPosition, Color::Red);

			std::stringstream quitStream;
			quitStream << "Quit";
			SpriteBatch::DrawString(quitStream.str().c_str(), _quitStringPosition, Color::Red);
		}
	}

	// Help menu
	if (_helpmenu)
	{
		SpriteBatch::Draw(_startBackground, _menuRectangle, nullptr);

		SpriteBatch::Draw(_c1Texture, _c1Rect, nullptr);
		SpriteBatch::Draw(_c2Texture, _c2Rect, nullptr);
		SpriteBatch::Draw(_c3Texture, _c3Rect, nullptr);
		SpriteBatch::Draw(_c4Texture, _c4Rect, nullptr);

		std::stringstream p1;
		p1 << "Player 1";
		SpriteBatch::DrawString(p1.str().c_str(), _p1Pos, Color::Red);

		std::stringstream p2;
		p2 << "Player 2";
		SpriteBatch::DrawString(p2.str().c_str(), _p2Pos, Color::Red);

		std::stringstream p3;
		p3 << "Player 3";
		SpriteBatch::DrawString(p3.str().c_str(), _p3Pos, Color::Cyan);

		std::stringstream p4;
		p4 << "Player 4";
		SpriteBatch::DrawString(p4.str().c_str(), _p4Pos, Color::Cyan);

		std::stringstream space;
		space << "Press SPACE to go back";
		SpriteBatch::DrawString(space.str().c_str(), _spacePos, Color::Red);
	}

	SpriteBatch::EndDraw(); // Ends Drawing
}