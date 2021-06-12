#include <stdio.h>
#include <SDL.h>
#include "constants.h"

//Define SDL_Window as a global so we can access from anywhere.
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int game_is_running = FALSE;

int last_frame_time = 0; //Keep track of last frame called.

struct ball {
	float x;
	float y;
	float width;
	float height;
	float velocityX;
	float velocityY;
} ball;

struct paddle {
	float x;
	float y;
	float width;
	float height;
	int move_direction;
} paddle;

int initialize_window(void)
{
	//Initialize the different components so SDL knows what to use for the program. 
	//Accepts a flag. 
	//SDL_INIT_EVERYTHING will literally allow SDL to use everything that SDL has access to hardware-wise. (Refer to the defines for more specific flag info)
	//If it doesn't work, throw error
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "Error Initializing SDL\n");
		return FALSE;
	}

	window = SDL_CreateWindow(
		NULL, //Don't name the window
		SDL_WINDOWPOS_CENTERED, //SDL define to get the center of screen
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, //Defined in constants as the pixel size of the minimized window
		WINDOW_HEIGHT,
		SDL_WINDOW_BORDERLESS //Flag to tell SDL to show window without borders
	);

	//If the window doesn't show, something's wrong.
	if (!window)
	{
		fprintf(stderr, "Error Creating SDL Window");
		return FALSE;
	}
	
	renderer = SDL_CreateRenderer(
		window, //render our created window
		-1, //Rendering driver (-1 renders the first available)
		0 //Flag to tell the renderer what to do (0 means Nothing special)
	);

	if (!renderer)
	{
		fprintf(stderr, "Error Creating SDL Renderer");
		return FALSE;
	}

	//Everything works!
	return TRUE;
}
//Get keyboard/controller inputs
void process_input()
{
	SDL_Event event; //Create the event
	SDL_PollEvent(&event); //Run the event

	//Since there'll be a shitload of events, use a switch.
	switch (event.type)
	{

	case SDL_QUIT: //The exit button event
		game_is_running = FALSE;
		break;
	case SDL_KEYDOWN: //get keydown events
		if (event.key.keysym.sym == SDLK_ESCAPE)
		{
			game_is_running = FALSE;
		}
		
		paddle.move_direction = (event.key.keysym.sym == SDLK_RIGHT) - (event.key.keysym.sym == SDLK_LEFT);
		break;
	case SDL_KEYUP: // Get the key up events
		if ((event.key.keysym.sym == SDLK_RIGHT) || (event.key.keysym.sym == SDLK_LEFT))
		{
			paddle.move_direction = 0;
		}
		break;
	default:
		break;
	}
}

void create_ball()
{
	ball.x = 20;
	ball.y = 20;
	ball.width = 15;
	ball.height = 15;
	ball.velocityX = 100;
	ball.velocityY = 100;
}

void create_paddle()
{
	paddle.x = 320;
	paddle.y = 500;
	paddle.width = 100;
	paddle.height = 20;
}

void update_ball(float dt)
{
	ball.x += ball.velocityX * dt;
	ball.y += ball.velocityY * dt;
}

void check_ball_collide()
{
	int ball_x_bounds_right = ball.x + ball.width;
	int ball_x_bounds_left = ball.x - (ball.width / 2);

	int ball_y_bounds_bottom = ball.y + ball.height;
	int ball_y_bounds_top = ball.y - (ball.height / 2);

	if (ball_x_bounds_right > WINDOW_WIDTH || ball_x_bounds_left <= 0) ball.velocityX = -ball.velocityX;
	if (
		(ball_y_bounds_bottom > WINDOW_HEIGHT || ball_y_bounds_top <= 0) //If it's the top or bottom
		|| //AABB COLLISION!!!
		(
			ball.x < paddle.x + paddle.width &&
			ball_x_bounds_right > paddle.x &&
			ball.y < paddle.y + paddle.height &&
			ball_y_bounds_bottom > paddle.y)
		)
	{
		ball.velocityY = -ball.velocityY;
	}
}

void setup() //Initialize the default values
{
	create_ball();
	create_paddle();
}

//Update characters/check collisions/move/whatever
void update()
{
	int useDelay = FALSE;
	//Get the time between when the program started (now), return true after the previous frame elapsed 33.333 seconds
	//Used to "waste" time between the ticks
	if (!useDelay)
	{
		while (!SDL_TICKS_PASSED(SDL_GetTicks(), last_frame_time + FRAME_TARGET_TIME));
	}
	else
	{
		//Use the operating system to delegate the "wait" to another core instead using the delay function.
		//WAY better performance
		int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);

		if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
		{
			SDL_Delay(time_to_wait);
		}
	}


	//Get the delta time to update the objects per-second instead of a fixed pixel rate per frame.
	float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;
	
	last_frame_time = SDL_GetTicks(); //Keep track of time SDL has been open for FPS tracking.
	
	//update ball positions
	check_ball_collide();
	update_ball(delta_time);

	//Move the paddle
	paddle.x += paddle.move_direction * 200 * delta_time;
}

//Draw the stuff to the screen
void render()
{
	SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
	SDL_RenderClear(renderer); //clear stuff from the renderer

	//Draw the ball rect
	SDL_Rect ball_rect = {
		ball.x,
		ball.y,
		ball.width,
		ball.height
	};

	//Set ball color
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	//Actually render the ball
	SDL_RenderFillRect(renderer, &ball_rect);
	
	//Draw the paddle rect
	SDL_Rect paddle_rect = {
		paddle.x,
		paddle.y,
		paddle.width,
		paddle.height
	};

	//Set paddle color
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	//Actually render the paddle
	SDL_RenderFillRect(renderer, &paddle_rect);

	SDL_RenderPresent(renderer); //Swap the buffers to avoid graphics glitches
}

//After the game runs. Close it.
destroy_window()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	game_is_running = initialize_window();

	setup(); //Setup environment, colors, etc.

	//Game loop basis
	//Only run 
	while (game_is_running)
	{
		process_input();
		update();
		render();
	}

	destroy_window();

	return 0;
}