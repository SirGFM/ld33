/**
 * @file src/main.c
 * 
 * The game's entry point
 */
#include <ld33/game.h>
#include <ld33/main.h>
#include <ld33/playstate.h>

#include <string.h>
#include <time.h>

int main_getPRNG(gameCtx *pGame) {
    long int tmp = pGame->seed;
    
    tmp *= 0x19660d;
    tmp += 0x3c6ef35f;
    pGame->seed = tmp;
    
    return pGame->seed;
}

/**
 * Update all key's states (and the quit flag)
 */
gfmRV main_getKeyStates(gameCtx *pGame) {
    gfmRV rv;
    
#define GET_KEY_STATE(key) \
    rv = gfm_getKeyState(&(pGame->state_##key), &(pGame->num_##key), \
        pGame->pCtx, pGame->handle_##key); \
    ASSERT(rv == GFMRV_OK, rv)
    
    GET_KEY_STATE(down);
    GET_KEY_STATE(left);
    GET_KEY_STATE(right);
    GET_KEY_STATE(up);
    GET_KEY_STATE(atk);
    GET_KEY_STATE(quit);
    
#undef GET_KEY_STATE
    
    if ((pGame->state_quit & gfmInput_justPressed) == gfmInput_justPressed) {
        rv = gfm_setQuitFlag(pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Load all assets; It should run on another thread while the main plays an
 * animations, but I didn't get to implement that... yet... XD
 * 
 * @param  pGame  The game
 * @return        ...
 */
static gfmRV loadAssets(gameCtx *pGame) {
    gfmRV rv;
    int texIndex;
    
    // Load the texture and set it as default (since it will be the only one)
    rv = gfm_loadTextureStatic(&texIndex, pGame->pCtx, "atlas.bmp",
            0xff00ff/*keyColor*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_setDefaultTexture(pGame->pCtx, texIndex);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Create the texture's spritesets
    rv = gfm_createSpritesetCached(&(pGame->pSset4x4), pGame->pCtx, texIndex,
        4/*tileWidth*/, 4/*tileHeight*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_createSpritesetCached(&(pGame->pSset8x8), pGame->pCtx, texIndex,
        8/*tileWidth*/, 8/*tileHeight*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_createSpritesetCached(&(pGame->pSset32x32), pGame->pCtx, texIndex,
        32/*tileWidth*/, 32/*tileHeight*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_createSpritesetCached(&(pGame->pSset128x128), pGame->pCtx,
        texIndex, 128/*tileWidth*/, 128/*tileHeight*/);
    ASSERT(rv == GFMRV_OK, rv);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

int main(int argc, char *argv[]) {
    gameCtx game;
    gfmAudioQuality audSettings;
    gfmInput *pInput;
    gfmRV rv;
    int bbufWidth, bbufHeight, dps, fps, height, isFullscreen, ups, width;
    
    // Clean everything before hand
    memset(&game, 0x0, sizeof(gameCtx));
    
    // Start the library
    rv = gfm_getNew(&(game.pCtx));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_initStatic(game.pCtx, "com.gfmgamecorner", "game");
    ASSERT(rv == GFMRV_OK, rv);
    
    // Intialize the seed
    game.seed = (unsigned int)time(0);
    
    // 'Parse' options
    isFullscreen = 0;
    width = 640;
    height = 480;
    game.maxParts = 2048;
    audSettings = gfmAudio_defQuality;
    while (argc > 1) {
        #define GETARG(opt) strcmp(argv[argc - 1], opt) == 0
        
        if (GETARG("-full") || GETARG("-f")) {
            isFullscreen = 1;
        }
        else if (GETARG("-noaudio") || GETARG("-m")) {
            rv =  gfm_disableAudio(game.pCtx);
            ASSERT(rv == GFMRV_OK, rv);
        }
        else if (GETARG("-badaudio") || GETARG("-l")) {
            // TODO Test with lowQuality
            audSettings = gfmAudio_medQuality;
        }
        else if (GETARG("-width") || GETARG("-w")) {
            char *pTmp;
            
            width = 0;
            pTmp = argv[argc];
            while (*pTmp) {
                width = width * 10 + (*pTmp) - '0';
                
                pTmp++;
            }
        }
        else if (GETARG("-height") || GETARG("-h")) {
            char *pTmp;
            
            height = 0;
            pTmp = argv[argc];
            while (*pTmp) {
                height = height * 10 + (*pTmp) - '0';
                
                pTmp++;
            }
        }
        
        #undef GETARG
        argc--;
    }
    
    // Create the window
    bbufWidth = 160;
    bbufHeight = 120;
    if (isFullscreen) {
        rv = gfm_initGameFullScreen(game.pCtx, bbufWidth, bbufHeight,
                0/*defRes*/, 0/*dontResize*/);
    }
    else {
        rv = gfm_initGameWindow(game.pCtx, bbufWidth, bbufHeight, width, height,
                0/*dontResize*/);
    }
    ASSERT(rv == GFMRV_OK, rv);
    
    // Set the BG color
    rv = gfm_setBackground(game.pCtx, 0xff45283c);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Initialize the audio system
    rv = gfm_initAudio(game.pCtx, audSettings);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Shorten the double press window
    rv = gfm_getInput(&pInput, game.pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmInput_setMultiDelay(pInput, 150/*ms*/);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Bind keys
#define BIND_NEW_KEY(handle, key) \
    rv = gfm_addVirtualKey(&(game.handle_##handle), game.pCtx); \
    ASSERT(rv == GFMRV_OK, rv); \
    rv = gfm_bindInput(game.pCtx, game.handle_##handle, key); \
    ASSERT(rv == GFMRV_OK, rv)
#define BIND_KEY(handle, key) \
    rv = gfm_bindInput(game.pCtx, game.handle_##handle, key); \
    ASSERT(rv == GFMRV_OK, rv)
    
    BIND_NEW_KEY(down, gfmKey_down);
    BIND_KEY(down, gfmKey_s);
    BIND_NEW_KEY(left, gfmKey_left);
    BIND_KEY(left, gfmKey_a);
    BIND_NEW_KEY(right, gfmKey_right);
    BIND_KEY(right, gfmKey_d);
    BIND_NEW_KEY(up, gfmKey_up);
    BIND_KEY(up, gfmKey_w);
    BIND_NEW_KEY(atk, gfmKey_x);
    BIND_KEY(atk, gfmKey_space);
    BIND_NEW_KEY(quit, gfmKey_esc);
    
#undef BIND_NEW_KEY
#undef BIND_KEY
    
    // Load assets
    rv = loadAssets(&game);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Set FPS
    ups = 60;
    dps = 60;
    rv = gfm_setStateFrameRate(game.pCtx, ups, dps);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Set the timer resolution, in frames per seconds
    fps = 60;
    rv = gfm_setFPS(game.pCtx, fps);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Initialize the FPS counter (only visible in debug mode, though)
    rv = gfm_initFPSCounter(game.pCtx, game.pSset8x8, 0/*firstTile*/);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Initialize the quadtree
    rv = gfmQuadtree_getNew(&(game.pQt));
    ASSERT(rv == GFMRV_OK, rv);
    
    // Loop...
    game.state = state_playstate;
    while (gfm_didGetQuitFlag(game.pCtx) == GFMRV_FALSE) {
        // Run the current state
        switch (game.state) {
            case state_playstate: rv = playstate_loop(&game); break;
            default: rv = GFMRV_INTERNAL_ERROR;
        }
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    rv = GFMRV_OK;
__ret:
    // Clean all resources
    gfmGenArr_clean(game.pObjs, gfmObject_free);
    gfmGenArr_clean(game.pSprs, gfmSprite_free);
    gfmQuadtree_free(&(game.pQt));
    gfm_free(&(game.pCtx));
    
    return rv;
}

