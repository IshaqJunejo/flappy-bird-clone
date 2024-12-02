#include "raylib.h"

// Class for the Obstacles
class Obstacle {
	private:
		// Loading the Images of the Obstacles
		Texture2D image = LoadTexture("assets/pipe.png");
		Texture2D imageFlipped = LoadTexture("assets/pipe-flipped.png");
		// Initial Position and Scale
		float initialPosX;
		float scale = 2;
		// Position and Size
		float posX;
		float posY;
		float width = 52 * this->scale;
		float height = 320 * this->scale;

	public:
		// Score Counter, it will be used for Counting Score
		bool scoreCounter = true;

		// Constructor
		Obstacle(float x) {
			this->posX = x;
			this->posY = GetRandomValue(120, 560);

			this->initialPosX = x;
		}

		// Getter Functions
		float getPosX() { return this->posX; }
		float getPosY() { return this->posY; }
		float getWidth() { return this->width; }
		float getHeight() { return this->height; }

		// Function for Updating the Obstacle
		void update(float speed) {
			if (this->posX <= this->width * -1) {
				// If too much to the left, reposition it
				this->reposition();
			}

			// move it the left
			this->posX -= speed;
		}

		// Function to Reposition the Obstacle
		void reposition() {
			this->posX = this->initialPosX;
			this->posY = GetRandomValue(120, 560);

			this->scoreCounter = true;
		}

		// Function to Draw the Obstacle
		void draw() {
			// Drawing the Bottom Pipe
			DrawTextureEx(this->image, {this->posX, this->posY}, 0.0, this->scale, WHITE);
			// Drawing the Upper Pipe
			DrawTextureEx(this->imageFlipped, {this->posX, this->posY - this->height - 120}, 0.0, this->scale, WHITE);
		}
};

// Class for the Base
class Base {
	private:
		// Loading the Image of the Base
		Texture2D image = LoadTexture("assets/base.png");
		// Scale and Offset of the Base
		float scale = 1.5;
		float offsetX = 0.0;

	public:
		// Function to Move the Base to the Left
		void update(float limit, float speed) {
			if (this->offsetX <= limit * -1) {
				// If Offset is smaller than the limit, turn it back to zero
				this->offsetX = 0.0;
			} else {
				// Else, keep reducing the Offset
				this->offsetX -= speed;
			}
		}

		// Function to Draw the Base (twice)
		void draw(float limit) {
			// Once here
			DrawTextureEx(this->image, {this->offsetX, 600}, 0.0, this->scale, WHITE);
			// And also here
			DrawTextureEx(this->image, {this->offsetX + limit, 600}, 0.0, this->scale, WHITE);
		}
};

// Class for the Player
class Player {
	private:
		// Loading the Image of the Bird
		Texture2D image = LoadTexture("assets/bird.png");
		// Scale and velocity
		float scale = 1.5;
		float velY = 0.0;
		// Position and Size
		float posX = 168;
		float posY = 320;
		float width = 34 * this->scale;
		float height = 24 * this->scale;

	public:
		// Getter Functions
		float getPosX() { return this->posX; }
		float getPosY() { return this->posY; }
		float getWidth() { return this->width; }
		float getHeight() { return this->height; }

		// Function to Update the Player
		void update() {
			if (IsKeyDown(KEY_SPACE)) {
				// If Space Bar is pressed, JUMP!
				this->jump();
			} else {
				// Else, Fall Down
				this->velY += 0.3;
			}

			// Adding velY to posY
			this->posY += this->velY;
		}

		// Function to make Player Jump
		void jump() {
			this->velY = -3.5;
		}

		// Function to Reposition the Payer
		void reposition() {
			this->posY = 320;
		}

		// Function to check for Collision between an Obstacle and the Player
		bool collideObstacle(Obstacle obs) {
			// Bounding Box of the Player
			Rectangle playerRect = {this->posX, this->posY, this->width, this->height};
			// Bounding Box of the Bottom Pipe
			Rectangle bottomPipeRect = {obs.getPosX(), obs.getPosY(), obs.getWidth(), obs.getHeight()};
			// Bounding Box of the Upper Pipe
			Rectangle upperPipeRect = {obs.getPosX(), obs.getPosY() - obs.getHeight() - 120, obs.getWidth(), obs.getHeight()};

			if (CheckCollisionRecs(playerRect, bottomPipeRect)) {
				// Player is colliding with the bottom pipe
				return true;
			} else if (CheckCollisionRecs(playerRect, upperPipeRect)) {
				// Player is colliding with the upper pipe
				return true;
			}

			// Player is not colliding with either of the pipes
			return false;
		}

		// Function to Draw the Player
		void draw() {
			DrawTextureEx(this->image, {posX, posY}, 0.0, this->scale, WHITE);
		}
};

// Main Function
int main() {
	// Creating a game window of 432 by 768 pixels with title "Flappy Bird"
	const int WIDTH = 432;
	const int HEIGHT = 768;
	InitWindow(WIDTH, HEIGHT, "Flappy Bird");
	// Setting the Target FPS to 60 frames per second
	SetTargetFPS(60);

	// Loading a Texture for using as a background
	Texture2D backgroundImg = LoadTexture("assets/background.png");

	// Game State, 0 for Beginning, 1 for Playing, 2 for Game Over
	int gameState = 0;
	// Score
	int score = 0;
	// Creating an object for our Base
	Base base;
	// Creating an object for our Player
	Player bird;
	// Creating an object for an Obstacle
	Obstacle pipe = Obstacle(WIDTH);

	// Main Loop
	while (!WindowShouldClose()) {
		// Game State Management
		if (gameState == 0) {
			// Beginning State
			if (IsKeyPressed(KEY_SPACE)) {
				// Press Space Bar to Play
				gameState = 1;
			}
		} else if (gameState == 1) {
			// Updating the base
			base.update(WIDTH, 1.2);
			// Updating the bird
			bird.update();
			// Updating the pipe
			pipe.update(1.2);

			if (pipe.getPosX() <= bird.getPosX() - pipe.getWidth() && pipe.scoreCounter == true) {
				// Score Increment (once) when a Pipe is crossed
				score++;
				pipe.scoreCounter = false;
			}

			if (bird.getPosY() >= 600 - bird.getHeight()) {
				// If the Bird touches the Base, Game State will be Game Over
				gameState = 2;
			}

			if (bird.collideObstacle(pipe)) {
				// If the Bird touches a Pipe, Game State will be Game Over
				gameState = 2;
			}
		} else if (gameState == 2) {
			// Game Over State
			if (IsKeyPressed(KEY_SPACE)) {
				// Restart when Space is Pressed
				gameState = 1;
				// Bird is Repositioned
				bird.reposition();
				// Pipe is Repositioned
				pipe.reposition();
				// Score is back to zero
				score = 0;
			}
		}

		// Drawing Section
		BeginDrawing();
			ClearBackground(RAYWHITE);
			DrawTextureEx(backgroundImg, {0, 0}, 0.0, 1.5, WHITE);
			// Drawing the pipe
			pipe.draw();
			// Drawing the Base
			base.draw(WIDTH);
			// Drawing the Bird
			bird.draw();
			if (gameState == 0) {
				// Drawing 'Press Space to Start'
				DrawRectangle(WIDTH / 2 - 150 - 2, HEIGHT / 2 - 15 - 2, 350, 34, WHITE);
				DrawText("Press Space to Start", WIDTH / 2 - 150, HEIGHT / 2 - 15, 30, GRAY);
			} else if (gameState == 1) {
				// Drawing the Score
				DrawRectangle(WIDTH / 2 - 5, 40 - 2, 30, 30, WHITE);
				DrawText(TextFormat("%i", score), WIDTH / 2, 40, 30, GRAY);
			} else if (gameState == 2) {
				// Drawing the Score
				DrawRectangle(WIDTH / 2 - 5, 40 - 2, 30, 30, WHITE);
				DrawText(TextFormat("%i", score), WIDTH / 2, 40, 30, GRAY);
				// Drawing 'Game Over'
				DrawRectangle(WIDTH / 2 - 75 - 2, HEIGHT / 2 - 100 - 2, 165, 34, WHITE);
				DrawText("Game Over", WIDTH / 2 - 75, HEIGHT / 2 - 100, 30, GRAY);
				// Drawing 'Press Space to Restart'
				DrawRectangle(WIDTH / 2 - 180 - 2, HEIGHT / 2 - 15 - 2, 380, 34, WHITE);
				DrawText("Press Space to Restart", WIDTH / 2 - 180, HEIGHT / 2 - 15, 30, GRAY);
			}
		EndDrawing();
	}

	// Close the Game Window after "WindowShouldClose()" returns true
	CloseWindow();

	return 0;
}
