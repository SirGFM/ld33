/**
 * @file src/main.c
 * 
 * The game's entry point
 */
#include <ld33/game.h>
#include <ld33/playstate.h>

#include <string.h>

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
    rv = gfm_createSpritesetCached(&(pGame->pSset8x8), pGame->pCtx, texIndex,
        8/*tileWidth*/, 8/*tileHeight*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_createSpritesetCached(&(pGame->pSset32x32), pGame->pCtx, texIndex,
        32/*tileWidth*/, 32/*tileHeight*/);
    ASSERT(rv == GFMRV_OK, rv);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

int main(int argc, char *argv[]) {
    gameCtx game;
    gfmAudioQuality audSettings;
    gfmRV rv;
    int bbufWidth, bbufHeight, dps, fps, isFullscreen, ups;
    
    // Clean everything before hand
    memset(&game, 0x0, sizeof(gameCtx));
    
    // Start the library
    rv = gfm_getNew(&(game.pCtx));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_initStatic(game.pCtx, "com.gfmgamecorner", "game");
    ASSERT(rv == GFMRV_OK, rv);
    
    // 'Parse' options
    isFullscreen = 0;
    audSettings = gfmAudio_defQuality;
    while (argc > 1) {
        #define GETARG(opt) strcmp(argv[argc - 1], opt) == 0
        
        if (GETARG("full")) {
            isFullscreen = 1;
        }
        else if (GETARG("noaudio")) {
            rv =  gfm_disableAudio(game.pCtx);
            ASSERT(rv == GFMRV_OK, rv);
        }
        else if (GETARG("badaudio")) {
            // TODO Test with lowQuality
            audSettings = gfmAudio_medQuality;
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
        rv = gfm_initGameWindow(game.pCtx, bbufWidth, bbufHeight, 640/*wnd w*/,
                480/*wnd h*/, 0/*dontResize*/);
    }
    ASSERT(rv == GFMRV_OK, rv);
    
    // Initialize the audio system
    rv = gfm_initAudio(game.pCtx, audSettings);
    ASSERT(rv == GFMRV_OK, rv);
    
    // TODO Bind keys
    
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

