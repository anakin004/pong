#include <SDL.h>
#include <iostream>
#include <stdio.h>
#include <SDL_image.h>
#include <cmath>
#include <SDL_ttf.h>
#include "LTexture.h"
#include "ball.h"
#include <random>

const int WIDTH = 1500;

const int HEIGHT = 900;

bool init();
bool loadMedia();
void close();

std::random_device rd;  // Obtain a random number from hardware
std::mt19937 eng(rd()); // Seed the generator
std::uniform_int_distribution<> distrY(30, HEIGHT - 30); // Define the range for y-coordinate


//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;


//Globally used font
TTF_Font* gFont = NULL;

//Rendered texture
LTexture gTextTexture;
LTexture gTextTexture2;


//scene texture
LTexture gBackgroundTexture;

Ball ball_sprite;

const int FPS = 60;
const int FRAME_DELAY = 1000 / FPS;

int main( int argc, char* args[] )
{

    if( !init() )
    {
        printf( "Failed to initialize!\n" );
    }
    else
    {
        //Load media
        if( !loadMedia() )
        {
            printf( "Failed to load media!\n" );
        }
        else
        {
            int player1_x = WIDTH-30, player2_x = 20;
            int player1_y = HEIGHT/2, player2_y = HEIGHT/2;
            int ball_x = WIDTH/2, ball_y = distrY(eng);

            Uint32 start_time = SDL_GetTicks();
                    // Define the rectangle within the image

            SDL_Rect player1 = { player1_x, player1_y, 10, 122};

            SDL_Rect player2 = { player2_x, player2_y, 10, 122};

            SDL_Rect ball_clip = { 0, 0, 20, 20};


            //Hack to get window to stay up
            SDL_Event e;
            bool quit = false;

            while( !quit )
            {   

                Uint32 frameStart = SDL_GetTicks();
                    //User requests quit
                while( SDL_PollEvent(&e) != 0 ){

                        
                    const Uint8 *keyboard = SDL_GetKeyboardState( NULL );

                    if( e.type == SDL_QUIT ){
                        quit = true;
                        break;
                    }

                    if ( keyboard[SDL_SCANCODE_D] == 1){
                        if ( player1.y+ 15 <= HEIGHT-122 )
                            player1.y += 15;
                        break;
                    }
                    
                    if ( keyboard[SDL_SCANCODE_E] == 1){
                        if ( player1.y - 15 >= 5 )
                            player1.y -= 15;
                        break;
                    }
                    
                    if ( keyboard[SDL_SCANCODE_S] == 1){
                        if ( player2.y+ 15 <= HEIGHT-122 )
                            player2.y += 15;
                        break;
                    }
                    
                    if ( keyboard[SDL_SCANCODE_W] == 1){
                        if ( player2.y - 15 >= 5 )
                            player2.y -= 15;
                        break;
                    }

                    else{
                        break;
                    }

                    }

                    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                    SDL_RenderClear( gRenderer );

                    //Render background texture to screen

                    gBackgroundTexture.render( 0, 0 );  


                    // moves ball in x and y direction


                    std::vector<float> bounds_check = ball_sprite.check_bound(
                    ball_sprite.ball_x, ball_sprite.ball_y,
                    ball_sprite.v_x, ball_sprite.v_y,
                    player1.y, player2.y
                    );


                    ball_sprite.v_x = bounds_check[0];
                    ball_sprite.v_y = bounds_check[1];

                    ball_sprite.ball_x += ball_sprite.v_x;
                    ball_sprite.ball_y += ball_sprite.v_y;

                    std::tuple<int, int, bool> checking = ball_sprite.check_win( ball_sprite.ball_x );
                    
                    if ( std::get<2>(checking) == true ){
                        ball_sprite.pts_1 += std::get<0>(checking);
                        ball_sprite.pts_2 += std::get<1>(checking);
                        ball_sprite.ball_x = WIDTH/2;
                        ball_sprite.ball_y = distrY(eng);
                        ball_sprite.v_x = 2;
                        ball_sprite.v_y = 2;
                    }

                    std::string pts1 = std::to_string(ball_sprite.pts_1);
                    std::string pts2 = std::to_string(ball_sprite.pts_2);



                    ball_sprite.render( ball_sprite.ball_x, ball_sprite.ball_y, &ball_clip);                                                                                                

                    //bounds for ball is y = 10, y = height - 10
                    // also it cant pass the lines

                    SDL_RenderDrawRect( gRenderer, &player1);
                    SDL_RenderFillRect(gRenderer, &player1 );

                    SDL_RenderDrawRect( gRenderer, &player2);
                    SDL_RenderFillRect(gRenderer, &player2);

                    SDL_Rect clip = {0,0, 20, 20};

                    gTextTexture.loadFromRenderedText(pts1, {255,255,255});
                    gTextTexture.render(20, 10, &clip);

                    gTextTexture2.loadFromRenderedText(pts2, {255,255,255});
                    gTextTexture2.render(WIDTH-20, 10, &clip);

                    SDL_RenderPresent( gRenderer );

                    Uint32 frameTime = SDL_GetTicks() - frameStart;
                    if (frameTime < FRAME_DELAY) {
                        SDL_Delay(FRAME_DELAY - frameTime);
                    }

        
            }
        }
    }
    close();
    return EXIT_SUCCESS;
}


bool loadMedia()
{
    //Loading success flag
    bool success = true;

    //Load background texture
    if( !ball_sprite.loadFromFile( "ball.png" ) )
    {
        printf( "Failed to load ball texture image!\n" );
        success = false;
    }
    //Load background texture
    if( !gBackgroundTexture.loadFromFile( "gameboard.png" ) )
    {
        printf( "Failed to load background texture image!\n" );
        success = false;
    }
    //Open the font
    gFont = TTF_OpenFont( "lazy.ttf", 28 );
    if( gFont == NULL )
    {
        printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }
    else
    {
        //Render text
        SDL_Color textColor = { 0, 0, 0 };
        if( !gTextTexture.loadFromRenderedText( "0", textColor ) )
        {
            printf( "Failed to render text texture!\n" );
            success = false;
        }
        if( !gTextTexture2.loadFromRenderedText( "0", textColor ) )
        {
            printf( "Failed to render text texture!\n" );
            success = false;
        }
    }

    return success;
}

void close()
{
    //Free loaded images
    gBackgroundTexture.free();
    ball_sprite.free();

    //Free loaded images
    gTextTexture.free();

    //Free global font
    TTF_CloseFont( gFont );
    gFont = NULL;

    //Destroy window    
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;

    //Quit SDL subsystems
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

bool init(){

    bool success = true;

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
        success = false;
        printf("Couldnt init video");
    }

    else {
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

        gWindow = SDL_CreateWindow("Zesty", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

        if ( gWindow == NULL ){
            success = false;
            printf("Could not create window %s", SDL_GetError());
        }
        else{
            if ( IMG_Init( IMG_INIT_PNG) != IMG_INIT_PNG ){
                success = false;
                printf(" wrong goofy ");
            }
            else{
                if( TTF_Init() == -1 )
                {
                        printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
                        success = false;
                }
                else{
                    gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
                    if( gRenderer == NULL )
                    {
                        printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
                        success = false;
                    }
                    else
                    {
                        //Initialize renderer color
                        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

                    }
                }

            }

        }
    }

    return success;


}