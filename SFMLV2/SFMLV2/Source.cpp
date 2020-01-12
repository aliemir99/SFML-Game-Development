//
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <sstream>

//make code easier to type with "using namespace"
using namespace sf;

//function declaration
void updateBranches(int seed);
const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];
//where is the player/branch?
//left or right ? 
enum class side {LEFT, RIGHT, NONE};
side branchPositions[NUM_BRANCHES];

int main()
{
	//create a video mode object
	VideoMode vm(1920, 1080);

	//create and open a window for the game
	RenderWindow window(vm, "Timber!!!", Style::Fullscreen);

	//create a texture to hold a graphic on the GPU
	Texture textureBackground;

	//Load a graphic into the texture
	textureBackground.loadFromFile("C:/Users/Unatro/Desktop/SFMLV2/SFMLV2/graphics/background.png");

	//create a sprite
	Sprite spriteBackground;

	//Attach the texture to the sprite
	spriteBackground.setTexture(textureBackground);

	//set the spriteBackground to cover the screen
	spriteBackground.setPosition(0, 0);

	//Make a tree sprite
	Texture textureTree;
	textureTree.loadFromFile("C:/Users/Unatro/Desktop/SFMLV2/SFMLV2/graphics/tree2.png");
	Sprite spriteTree;
	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(810, 0);

	//make a bee sprite
	Texture textureBee;
	textureBee.loadFromFile("C:/Users/Unatro/Desktop/SFMLV2/SFMLV2/graphics/bee.png");
	Sprite spriteBee;
	spriteBee.setTexture(textureBee);
	spriteBee.setPosition(0, 800);
	//is the bee currently moving ?
	bool beeActive = false;
	//how fast can the bee fly
	float beeSpeed = 0.0f;

	//make 3 cloud sprites from 1 texture
	Texture textureCloud;
	//load 1 new texture
	textureCloud.loadFromFile("C:/Users/Unatro/Desktop/SFMLV2/SFMLV2/graphics/cloud.png");
	//3 new sprites with the same texture
	Sprite spriteCloud1;
	Sprite spriteCloud2;
	Sprite spriteCloud3;
	spriteCloud1.setTexture(textureCloud);
	spriteCloud2.setTexture(textureCloud);
	spriteCloud3.setTexture(textureCloud);
	//position the clouds off screen
	spriteCloud1.setPosition(0, 0);
	spriteCloud2.setPosition(0, 250);
	spriteCloud3.setPosition(0, 500);
	//Are the clouds currently on screen?
	bool cloud1Active = false;
	bool cloud2Active = false;
	bool cloud3Active = false;
	//how fast is each cloud?
	float cloud1Speed= 0.0f;
	float cloud2Speed = 0.0f;
	float cloud3Speed = 0.0f;
	//variables to control time itself
	Clock clock;

	//Time bar
	RectangleShape timeBar;
	float timeBarStartWidth = 400;
	float timeBarHeight = 80;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);

	Time gameTimeTotal;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

	//Track wether the game is running
	bool paused = true;

	//draw some text
	int score = 0;
	sf::Text messageText;
	sf::Text scoreText;

	//we need to choose a font
	Font font;
	font.loadFromFile("C:/Users/Unatro/Desktop/SFMLV2/SFMLV2/fonts/KOMIKAP_.ttf");

	//set the font to our message
	messageText.setFont(font);
	scoreText.setFont(font);

	//assign the actual message
	messageText.setString("Press Enter to start!");
	scoreText.setString("Score = 0");

	//make it really big
	messageText.setCharacterSize(75);
	scoreText.setCharacterSize(100);

	//choose a color
	messageText.setFillColor(Color::White);
	scoreText.setFillColor(Color::White);

	//position the text
	FloatRect textRect = messageText.getLocalBounds();
	messageText.setOrigin(textRect.left +
		textRect.width / 2.0f,
		textRect.top +
		textRect.height / 2.0f);
	messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
	scoreText.setPosition(20, 20);

	//prepare 6 branches
	Texture textureBranch;
	textureBranch.loadFromFile("C:/Users/Unatro/Desktop/SFMLV2/SFMLV2/graphics/branch.png");
	//set the texture for each branch sprite
	for (int i = 0; i < NUM_BRANCHES; i++)
	{
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-2000, -2000);
		//set the sprite's origin to dead center
		//we can then spin it round without changing its position
		branches[i].setOrigin(220, 20);
	}

	//prepare the player
	Texture texturePlayer;
	texturePlayer.loadFromFile("C:/Users/Unatro/Desktop/SFMLV2/SFMLV2/graphics/player.png");
	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);

	//the player starts on the left
	side PlayerSide = side::LEFT;

	//prepare the gravestone
	Texture textureRIP;
	textureRIP.loadFromFile("C:/Users/Unatro/Desktop/SFMLV2/SFMLV2/graphics/rip.png");
	Sprite spriteRIP;
	spriteRIP.setTexture(textureRIP);
	spriteRIP.setPosition(700, 830);

	//prepare the axe
	Texture textureAxe;
	textureAxe.loadFromFile("C:/Users/Unatro/Desktop/SFMLV2/SFMLV2/graphics/axe.png");
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(700, 830);
	//line the axe up with the tree
	const float AXE_POSITION_LEFT = 700;
	const float AXE_POSITION_RIGHT = 1075;

	//prepare the flying log
	Texture textureLog;
	textureLog.loadFromFile("C:/Users/Unatro/Desktop/SFMLV2/SFMLV2/graphics/log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(810, 720);

	//some other useful log related variables
	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;

	//control the player input
	bool acceptInput = false;

	//prepare the sound
	SoundBuffer chopBuffer;
	chopBuffer.loadFromFile("C:/Users/Unatro/Desktop/SFMLV2/SFMLV2/sound/chop.wav");
	Sound chop;
	chop.setBuffer(chopBuffer);

	SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("C:/Users/Unatro/Desktop/SFMLV2/SFMLV2/sound/death.wav");
	Sound death;
	death.setBuffer(deathBuffer);

	SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("C:/Users/Unatro/Desktop/SFMLV2/SFMLV2/sound/out_of_time.wav");
	Sound outOfTime;
	outOfTime.setBuffer(ootBuffer);
	
	//MAIN GAME LOOP

	while (window.isOpen())
	{
		//******************************
		//Handle the player input
		//******************************
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::KeyReleased && !paused)
			{
				//listen for key presses again
				acceptInput = true;

				//hide the axe
				spriteAxe.setPosition(2000,
					spriteAxe.getPosition().y);
			}
		}
		
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}
		//start the game
		if (Keyboard::isKeyPressed(Keyboard::Return))
		{
			paused = false;

			//reset the time and the score
			score = 0;
			timeRemaining = 5;


			//Make all the branches disappear
			for(int j = 1; j < NUM_BRANCHES; j++)
			{
				branchPositions[j] = side::NONE;
			}

			//Make sure the gravestone is hidden
			spriteRIP.setPosition(675, 2000);

			//Move the player into position
			spritePlayer.setPosition(580, 720);
			acceptInput = true;
		}

		//Wrap the player controls to
		//Make sure we are accepting input
		if (acceptInput)
		{
			if (Keyboard::isKeyPressed(Keyboard::Right))
			{
				//Make surue the player is on the right
				PlayerSide = side::RIGHT;
				score++;

				//Add to the amount of time remaining
				timeRemaining += (2 / score) + .15;

				spriteAxe.setPosition(AXE_POSITION_RIGHT,
					spriteAxe.getPosition().y);

				spritePlayer.setPosition(1200, 720);

				//update the branches
				updateBranches(score);

				//set the log flying to the left
				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;

				acceptInput = false;

				//play a chop sound
				chop.play();
			}
			//handle left cursor key
			if (Keyboard::isKeyPressed(Keyboard::Left))
			{
				//Make surue the player is on the left
				PlayerSide = side::LEFT;
				score++;

				//Add to the amount of time remaining
				timeRemaining += (2 / score) + .15;

				spriteAxe.setPosition(AXE_POSITION_LEFT,
					spriteAxe.getPosition().y);

				spritePlayer.setPosition(580, 720);

				//update the branches
				updateBranches(score);

				//set the log flying to the left
				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;

				acceptInput = false;

				//play a chop sound
				chop.play();
			}
	
		}

		//***********************************
		//update the scene
		//***********************************
		if (!paused)
		{
			//measure time
			Time dt = clock.restart();

			//subtract from the amount of time remaining
			timeRemaining -= dt.asSeconds();
			//size up the time bar
			timeBar.setSize(Vector2f(timeBarWidthPerSecond *
				timeRemaining, timeBarHeight));

			if (timeRemaining <= 0.0f)
			{
				//pause the game
				paused = true;

				//change the message shown to the player
				messageText.setString("Out of time!!");

				//reposition the text based on its new size
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left +
					textRect.width / 2.0f,
					textRect.top +
					textRect.height / 2.0f);
				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

				//play the out of time sound
				outOfTime.play();
			}

			//setup the bee
			if (!beeActive)
			{
				//how fast is the bee
				srand((int)time(0) * 10);
				beeSpeed = (rand() % 200) + 200;

				//how high is the bee
				srand((int)time(0) * 10);
				float height = (rand() % 500) + 500;
				spriteBee.setPosition(2000, height);
				beeActive = true;
			}
			else
				//move the bee
			{
				spriteBee.setPosition(
					spriteBee.getPosition().x -
					(beeSpeed * dt.asSeconds()),
					spriteBee.getPosition().y);
				//Has the bee reached the right hand edge of the screen?
				if (spriteBee.getPosition().x < -100)
				{
					//set it up ready to be a whole new cloud next frame
					beeActive = false;
				}
			}

			//Manage the clouds
			//Cloud 1
			if (!cloud1Active)
			{
				//how fast is the cloud
				srand((int)time(0) * 10);
				cloud1Speed = (rand() % 200);

				//how high is the cloud
				srand((int)time(0) * 10);
				float height = (rand() % 150);
				spriteCloud1.setPosition(-200, height);
				cloud1Active = true;
			}
			else
			{
				spriteCloud1.setPosition(
					spriteCloud1.getPosition().x +
					(cloud1Speed * dt.asSeconds()),
					spriteCloud1.getPosition().y);
				//Has the bee reached the right hand edge of the screen?
				if (spriteCloud1.getPosition().x > 1920)
				{
					//set it up ready to be a whole new cloud next frame
					cloud1Active = false;
				}
			}
			//Cloud 2
			if (!cloud2Active)
			{
				//how fast is the cloud
				srand((int)time(0) * 20);
				cloud2Speed = (rand() % 200);

				//how high is the cloud
				srand((int)time(0) * 20);
				float height = (rand() % 300) - 150;
				spriteCloud2.setPosition(-200, height);
				cloud2Active = true;
			}
			else
			{
				spriteCloud2.setPosition(
					spriteCloud2.getPosition().x +
					(cloud2Speed * dt.asSeconds()),
					spriteCloud2.getPosition().y);
				//Has the bee reached the right hand edge of the screen?
				if (spriteCloud2.getPosition().x > 1920)
				{
					//set it up ready to be a whole new cloud next frame
					cloud1Active = false;
				}
			}

			//Cloud 3
			if (!cloud3Active)
			{
				//how fast is the cloud
				srand((int)time(0) * 30);
				cloud3Speed = (rand() % 200);

				//how high is the cloud
				srand((int)time(0) * 30);
				float height = (rand() % 450) - 150;
				spriteCloud3.setPosition(-200, height);
				cloud3Active = true;
			}
			else
			{
				spriteCloud3.setPosition(
					spriteCloud3.getPosition().x +
					(cloud3Speed * dt.asSeconds()),
					spriteCloud3.getPosition().y);
				//Has the bee reached the right hand edge of the screen?
				if (spriteCloud3.getPosition().x > 1920)
				{
					//set it up ready to be a whole new cloud next frame
					cloud3Active = false;
				}
			}
			//update the score text
			std::stringstream ss;
			ss << "Score = " << score;
			scoreText.setString(ss.str());

			//update the branch sprites
			for (int i = 0; i < NUM_BRANCHES; i++)
			{
				float height = i * 150;
				if (branchPositions[i] == side::LEFT)
				{
					//Move the sprite to the left side
					branches[i].setPosition(610, height);
					//flip the sprite round the other way
					branches[i].setRotation(180);
				}
				else if (branchPositions[i] == side::RIGHT)
				{
					//Move the sprite to the right side
					branches[i].setPosition(1330, height);
					//set the sprite rotation normal
					branches[i].setRotation(0);
				}
				else
				{
					//hide the branch
					branches[i].setPosition(3000, height);
				}
			}

			//handle a flying log
			if (logActive)
			{
				spriteLog.setPosition(
					spriteLog.getPosition().x +
					(logSpeedX * dt.asSeconds()),
					spriteLog.getPosition().y +
					(logSpeedY * dt.asSeconds()));

				//has the log reached the right hand edge?
				if (spriteLog.getPosition().x < -100 ||
					spriteLog.getPosition().x >2000)
				{
					//set it up ready to be a whole new log next frame
					logActive = false;
					spriteLog.setPosition(810, 720);
				}
			}

			//has the player been squshed by a branch?
			if (branchPositions[5] == PlayerSide)
			{
				//death
				paused = true;
				acceptInput = false;

				//draw the gravestone
				spriteRIP.setPosition(525, 760);

				//hide the player
				spritePlayer.setPosition(2000, 660);


				//change the text of the message
				messageText.setString("SQUISHED!!");

				//center it on the screen
				FloatRect textRect = messageText.getLocalBounds();

				messageText.setOrigin(textRect.left +
					textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f);

				messageText.setPosition(1920 / 2.0f,
					1080 / 2.0f);

				//play the death sound
				death.play();
			}

		}// end if (!paused)
		
		//**************************
		//draw the scene
		//**************************
		//clear everything from the last frame
		window.clear();

		//draw our game scene here
		window.draw(spriteBackground);

		//draw the clouds
		window.draw(spriteCloud1);
		window.draw(spriteCloud2);
		window.draw(spriteCloud3);

		//draw the branches
		for (int i = 0; i < NUM_BRANCHES; i++)
		{
			window.draw(branches[i]);
		}

		//draw the tree
		window.draw(spriteTree);

		//draw the player
		window.draw(spritePlayer);

		//draw the axe
		window.draw(spriteAxe);

		//draw the flying log
		window.draw(spriteLog);

		//draw the gravestone
		window.draw(spriteRIP);

		//draw the insect
		window.draw(spriteBee);

		//draw the score
		window.draw(scoreText);

		//draw the timebar
		window.draw(timeBar);
		if (paused)
		{
			//draw our message
			window.draw(messageText);
		}

		//show everything we just drew
		window.display();
	}
	return 0;
}

//function definition
void updateBranches(int seed)
{
	//move all the branches down one place
	for (int j = NUM_BRANCHES - 1; j > 0; j--)
	{
		branchPositions[j] = branchPositions[j - 1];
	}
	//spawn new branch at position 0
	//left, right or None
	srand((int)time(0) + seed);
	int r = (rand() % 5);
	switch (r)
	{
	case 0:
		branchPositions[0] = side::LEFT;
		break;
	case 1:
		branchPositions[0] = side::RIGHT;
		break;
	default:
		branchPositions[0] = side::NONE;
		break;
	}
}