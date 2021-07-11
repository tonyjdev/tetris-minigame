#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>

using namespace sf;

struct Point { int x; int y; } coord[4], coord_copy[4];

Point figures[7][4] = {
	{ {1, 0}, {1, 1}, {1, 2}, {1, 3} },
	{ {1, 0}, {1, 1}, {2, 1}, {2, 2} },
	{ {2, 0}, {2, 1}, {2, 2}, {1, 2} },
	{ {1, 1}, {2, 1}, {1, 2}, {2, 2} },
	{ {1, 1}, {0, 2}, {1, 2}, {2, 2} },
	{ {2, 0}, {1, 1}, {2, 1}, {1, 2} },
	{ {1, 0}, {1, 1}, {1, 2}, {2, 2} }
};

int mapWidth = 10;
int mapHeight = 20;

int map[10][20] = {0};

Texture txBlock, txBG;
Sprite block, background;

int figColor;
int figID = 0;

int dir = 0;
bool rotate = false;

float timer = 0;
float delay = 0.1;

float timerDown = 0;
float defaultDelayDown = 0.4;
float minDelayDown = 0.1;
float delayDown = defaultDelayDown;


bool checkPosition()
{
	for (int i = 0; i < 4; i++) {
		if (coord_copy[i].x < 0 || coord_copy[i].x >= mapWidth) {
			return false;
		}

		if (coord_copy[i].y >= mapHeight) {
			return false;
		}

		if (map[coord_copy[i].x][coord_copy[i].y] != 0) {
			return false;
		}
	}

	return true;
}

void updateMap()
{
	for (int i = 0; i < 4; i++) {
		map[coord[i].x][coord[i].y] = figColor;
	}
}

void spawnFigure()
{
	figColor = rand() % 7;
	figID = rand() % 6;

	for (int i = 0; i < 4; i++) {
		coord[i].x = figures[figID][i].x;
		coord[i].y = figures[figID][i].y;
	}
}

void checkLine()
{
	for (int j = mapHeight - 1; j > 1; j--) {
		int line = 0;

		for (int i = 0; i < mapWidth; i++) {
			if (map[i][j] != 0) {
				line++;
			}
		}

		if (line == 10) {
			for (int py = j; py > 2; py--) {
				for (int i = 0; i < mapWidth; i++) {
					map[i][py] = map[i][py-1];
				}
			}
		}
	}
}


int main()
{
	RenderWindow window(VideoMode(400, 600), "The Game: Tetris!");
	Clock clock;


	txBlock.loadFromFile("./images/sprites.png");
	txBG.loadFromFile("./images/background.png");

	block.setTexture(txBlock);
	background.setTexture(txBG);

	spawnFigure();


	while (window.isOpen()) {
		float t = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer += t;
		timerDown += t;

		// Reset variables
		dir = 0;
		rotate = false;
		delayDown = defaultDelayDown;


		Event e;

		while (window.pollEvent(e)) {
			if (e.type == Event::Closed) {
				window.close();
			}

			if (e.type == Event::KeyPressed) {
				if (e.key.code == Keyboard::Left) { dir = -1; }
				if (e.key.code == Keyboard::Right) { dir = 1; }
				if (e.key.code == Keyboard::Up) { rotate = true; }
			}
		}

		// Keyboard Events
		if (Keyboard::isKeyPressed(Keyboard::Down)) { delayDown = minDelayDown; }



		// Movement
		for (int i = 0; i < 4; i++) {
			coord_copy[i].x = coord[i].x + dir;
		}
		if (checkPosition() == true) {
			for (int i = 0; i < 4; i++) {
				coord[i].x = coord_copy[i].x;
			}
		}

		// Rotate
		if (rotate) {
			for (int i = 0; i < 4; i++) {
				coord_copy[i].x = coord[i].x;
				coord_copy[i].y = coord[i].y;
			}

			Point pt = coord_copy[1]; // Point rotation

			for (int i = 0; i < 4; i++) {
				int x = coord_copy[i].y - pt.y;
				int y = coord_copy[i].x - pt.x;

				coord_copy[i].x = pt.x - x;
				coord_copy[i].y = pt.y + y;
			}

			if (checkPosition() == true) {
				for (int i = 0; i < 4; i++) {
					coord[i].x = coord_copy[i].x;
					coord[i].y = coord_copy[i].y;
				}
			}
		}


		if (timer > delay) {
			timer = 0;


			// Down
			if (timerDown > delayDown) {
				timerDown = 0;

				for (int i = 0; i < 4; i++) {
					coord_copy[i].x = coord[i].x;
					coord_copy[i].y = coord[i].y + 1;
				}


				if (!checkPosition()) {
					updateMap();
					spawnFigure();
				}
				else {
					for (int i = 0; i < 4; i++) {
						coord[i].x = coord_copy[i].x;
						coord[i].y = coord_copy[i].y;
					}
				}
			}

			// Check Line
			checkLine();


			// Draw
			window.clear(Color::Black);
			window.draw(background);

			// Draw map
			for (int i = 0; i < mapWidth; i++) {
				for (int j = 0; j < mapHeight; j++) {
					if (map[i][j] != 0) {
						block.setTextureRect(IntRect(map[i][j] * 18, 0, 18, 18));
						block.setPosition(i * 18, j * 18);
						block.move(24, 24);
						window.draw(block);
					}
				}
			}

			// Draw figure
			for (int i = 0; i < 4; i++) {
				block.setTextureRect(IntRect(figColor * 18, 0, 18, 18));
				block.setPosition(coord[i].x * 18, coord[i].y * 18);
				block.move(24, 24);
				window.draw(block);
			}

			window.display();
		}

	}

	return 0;
}
