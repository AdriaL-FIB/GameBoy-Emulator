#include <iostream>
#include "GameBoy.h"

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>


static SDL_Window* window = nullptr;
static SDL_Renderer* renderer = nullptr;
static SDL_Texture* texture = nullptr;

GameBoy gb;

Uint32 colors[]{ 0x00FFFFFF, 0x00888888, 0x00444444, 0x00000000 };

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{

	//gb.load_rom("ROMs\\tetris.gb");

	//gb.game_loop();


	SDL_SetAppMetadata("GameBoy Emulator", "1.0", "gameboy.emulator");

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	if (!SDL_CreateWindowAndRenderer("GameBoy Emulator", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
		SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
	if (!texture) {
		SDL_Log("Couldn't create streaming texture: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "60");

	return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{

    if (event->type == SDL_EVENT_KEY_DOWN)
    {
        switch (event->key.key)
        {
        case SDLK_Z:
            gb.button_down(BTN_A);
            break;
        case SDLK_X:
            gb.button_down(BTN_B);
            break;
        case SDLK_RETURN:
            gb.button_down(BTN_START);
            break;
        case SDLK_BACKSPACE:
            gb.button_down(BTN_SELECT);
			break;
		case SDLK_RIGHT:
            gb.button_down(DPAD_RIGHT);
			break;
		case SDLK_LEFT:
            gb.button_down(DPAD_LEFT);
			break;
		case SDLK_UP:
            gb.button_down(DPAD_UP);
			break;
		case SDLK_DOWN:
            gb.button_down(DPAD_DOWN);
			break;
		default:
            break;
        }
    }
	else if (event->type == SDL_EVENT_KEY_UP)
	{
		switch (event->key.key)
		{
		case SDLK_Z:
			gb.button_up(BTN_A);
			break;
		case SDLK_X:
			gb.button_up(BTN_B);
			break;
		case SDLK_RETURN:
			gb.button_up(BTN_START);
			break;
		case SDLK_BACKSPACE:
			gb.button_up(BTN_SELECT);
			break;
		case SDLK_RIGHT:
			gb.button_up(DPAD_RIGHT);
			break;
		case SDLK_LEFT:
			gb.button_up(DPAD_LEFT);
			break;
		case SDLK_UP:
			gb.button_up(DPAD_UP);
			break;
		case SDLK_DOWN:
			gb.button_up(DPAD_DOWN);
			break;
		default:
			break;
		}
	}
    else if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
	else if (event->type == SDL_EVENT_DROP_FILE)
	{
		if (not gb.load_rom(event->drop.data))
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error loading ROM", "This ROM is not currently supported", window);
		}
	}
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

Uint64 last_time = 0;
int budget = 0;

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{
    const Uint64 now = SDL_GetTicks();

    uint32_t delta_time_ms = uint32_t(now - last_time);
    last_time = now;

	if (not gb.game_loaded()) return SDL_APP_CONTINUE;

    budget += int(delta_time_ms / 1000.0f * gb.get_freq());

    while (budget > 0)
    {
       budget -= int(gb.tick());
    }

    void* pixels;
    int pitch = 0;
    if (SDL_LockTexture(texture, nullptr, &pixels, &pitch)) {

        const std::array<u8, WINDOW_WIDTH * WINDOW_HEIGHT>& fb = gb.get_framebuffer();

        int pitch_pixels = pitch / sizeof(Uint32);
        Uint32* _pixels = (Uint32*)pixels;


        for (int i = 0; i < WINDOW_HEIGHT; ++i)
        {
            for (int j = 0; j < WINDOW_WIDTH; ++j)
            {
                int fb_idx = i * WINDOW_WIDTH + j;
                int tex_idx = i * pitch_pixels + j;
                _pixels[tex_idx] = colors[fb[fb_idx]];
            }
        }

        SDL_UnlockTexture(texture);  /* upload the changes (and frees the temporary surface)! */
    }


    //SDL_RenderClear(renderer);  /* start with a blank canvas. */

    SDL_RenderTexture(renderer, texture, nullptr, nullptr);

    SDL_RenderPresent(renderer);  /* put it all on the screen! */

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    SDL_DestroyTexture(texture);
    /* SDL will clean up the window/renderer for us. */
}