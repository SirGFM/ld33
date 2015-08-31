/**
 * @file src/blastate.c
 * 
 * Game's title screen
 */
#include <GFraMe/gfmGenericArray.h>
#include <GFraMe/gfmGroup.h>
#include <GFraMe/gfmParser.h>
#include <GFraMe/gfmText.h>

#include <ld33/blastate.h>
#include <ld33/main.h>

#include <string.h>

struct stIntrostate {
    gfmSprite *pSpr;
    gfmText *pText;
    int time;
};
typedef struct stIntrostate blastate;

#ifdef EMSCRIPT
static blastate bsCtx;
#endif

/**
 * Initialize everything
 */
static gfmRV blastate_init(gameCtx *pGame) {
    gfmCamera *pCam;
    gfmRV rv;
    int x, y;
    blastate *pState;
    
    pState = (blastate*)pGame->pState;
    
    rv = gfm_getCamera(&pCam, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmCamera_centerAtPoint(pCam, 0/*x*/, 0/*y*/);
    ASSERT(rv == GFMRV_OK || rv == GFMRV_CAMERA_DIDNT_MOVE ||
            rv == GFMRV_CAMERA_MOVED, rv);
    
    x = (160 - 32) / 2;
    y = 32;
    
    rv = gfmSprite_getNew(&(pState->pSpr));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_init(pState->pSpr, x, y, 32/*w*/, 32/*h*/,
            pGame->pSset32x32, 0/*offX*/, 0/*offY*/, 0/*child*/, 0/*type*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setFrame(pState->pSpr, 16);
    
    y = 64;
    rv = gfmText_getNew(&(pState->pText));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmText_init(pState->pText, 0/*x*/, y, 160 / 8/*w*/,
            4 /*h*/, 100/*delay*/, 0/*bindToScreen!*/,
            pGame->pSset8x8, 0/*firstTile*/);
    ASSERT(rv == GFMRV_OK, rv);
    
    if (pGame->didWin) {
        rv = gfmSprite_setFrame(pState->pSpr, 25);
        ASSERT(rv == GFMRV_OK, rv);
        
        rv = gfmText_setTextStatic(pState->pText,
            "I DID COMPLETE MY MISSION AND KILLED ALL MONTERS, BUT...\n\n"
            "THEY WERE MERE SLIMES... WERE THEY REALLY A THREAT?\n\n"
            "WHY DID I FOLLOW THAT MISSION EVEN KNOWING THAT THEY WOULDN'T"
            "CAUSE ANY HARM?\n\n"
            "...\n\n"
            "BAD ENDING......?", 1/*doCopy*/);
        ASSERT(rv == GFMRV_OK, rv);
    }
    else if (pGame->didLose) {
        rv = gfmSprite_setFrame(pState->pSpr, 25);
        ASSERT(rv == GFMRV_OK, rv);
        
        rv = gfmText_setTextStatic(pState->pText,
            "IN THE END, I COULDN'T FULFILL MY MISSION...\n\n"
            "AT LEAST, THOSE SMALL CRITTERS WEREN'T SLAIN FOR NAUGHT...\n\n"
            "\n\n"
            "GOOD ENDING......?", 1/*doCopy*/);
        ASSERT(rv == GFMRV_OK, rv);
    }
    else {
        rv = gfmText_setTextStatic(pState->pText,
            "I'M THE LOCAL VILLAGE'S HERO\n\n"
            "WHENEVER THERE'S A LOCAL THREAT, I'M CALLED TO PUT AN END TO IT "
            "AND SAVE US ALL\n\n"
            "THIS TIME, I WAS TOLD THAT THERE'S AN OUTBREAK OF VERY DANGEROUS "
            "MONSTERS ON THE NEARBY FOREST\n\n"
            "I MUST GO AND CLEAN IT, OTHERWISE WE WILL BE IN DANGER...\n\n"
            "OR... SO I WAS TOLD...\n\n"
            "ANYWAY, I MUST OBEY! LET'S GO!\n\n"
            ".........", 1/*doCopy*/);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    rv = GFMRV_OK;
__ret:
    
    return rv;
}

/**
 * Clean everything
 */
static void blastate_clean(gameCtx *pGame) {
    blastate *pState;
    
    pState = (blastate*)pGame->pState;
    
    gfmSprite_free(&(pState->pSpr));
    gfmText_free(&(pState->pText));
}

/**
 * Updates the blastate
 */
static gfmRV blastate_update(gameCtx *pGame) {
    gfmRV rv;
    blastate *pState;
    
    pState = (blastate*)pGame->pState;
    
    rv = gfmText_didFinish(pState->pText);
    ASSERT(rv == GFMRV_TRUE || rv == GFMRV_FALSE, rv);
    if (rv == GFMRV_TRUE) {
        int elapsed;
        
        rv = gfm_getElapsedTime(&elapsed, pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
        
        pState->time += elapsed;
        
        if (pState->time >= 1250) {
            // switch state
            pGame->quitState = 1;
            pGame->state = state_playstate;
        }
    }
    
    rv = gfmText_update(pState->pText, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Draws the blastate
 */
static gfmRV blastate_draw(gameCtx *pGame) {
    gfmRV rv;
    blastate *pState;
    
    pState = (blastate*)pGame->pState;
    
    rv = gfmSprite_draw(pState->pSpr, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmText_draw(pState->pText, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Initialize the blastate and loop it
 */
gfmRV blastate_loop(gameCtx *pGame) {
#ifdef EMSCRIPT
    gfmRV rv;
    
    // Initialize the state, if needed
    if (!pGame->isInit) {
        memset(&bsCtx, 0x0, sizeof(blastate));
        pGame->pState = &bsCtx;
        
        rv = blastate_init(pGame);
        ASSERT(rv == GFMRV_OK, rv);
        
        pGame->isInit = 1;
    }
    
    // Run this loop
    
    // Sleep until there's a event
    rv = gfm_handleEvents(pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    
    while (gfm_isUpdating(pGame->pCtx) == GFMRV_TRUE) {
        rv = gfm_fpsCounterUpdateBegin(pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
        
        rv = main_getKeyStates(pGame);
        ASSERT(rv == GFMRV_OK, rv);
        
        rv = blastate_update(pGame);
        ASSERT(rv == GFMRV_OK, rv);
        
        rv = gfm_fpsCounterUpdateEnd(pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    while (gfm_isDrawing(pGame->pCtx) == GFMRV_TRUE) {
        rv = gfm_drawBegin(pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
        
        rv = blastate_draw(pGame);
        ASSERT(rv == GFMRV_OK, rv);
        
        rv = gfm_drawEnd(pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
__ret:
    if (pGame->quitState || rv != GFMRV_OK) {
        blastate_clean(pGame);
        pGame->isInit = 0;
        pGame->quitState = 0;
    }
    return rv;
#else
    gfmRV rv;
    blastate isCtx;
    
    memset(&isCtx, 0x0, sizeof(blastate));
    pGame->pState = &isCtx;
    
    rv = blastate_init(pGame);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Loop indefinitely....
    while (gfm_didGetQuitFlag(pGame->pCtx) == GFMRV_FALSE &&
            pGame->quitState == 0) {
        // Sleep until there's a event
        rv = gfm_handleEvents(pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
        
        while (gfm_isUpdating(pGame->pCtx) == GFMRV_TRUE) {
            rv = gfm_fpsCounterUpdateBegin(pGame->pCtx);
            ASSERT(rv == GFMRV_OK, rv);
            
            rv = main_getKeyStates(pGame);
            ASSERT(rv == GFMRV_OK, rv);
            
            rv = blastate_update(pGame);
            ASSERT(rv == GFMRV_OK, rv);
            
            rv = gfm_fpsCounterUpdateEnd(pGame->pCtx);
            ASSERT(rv == GFMRV_OK, rv);
        }
        
        while (gfm_isDrawing(pGame->pCtx) == GFMRV_TRUE) {
            rv = gfm_drawBegin(pGame->pCtx);
            ASSERT(rv == GFMRV_OK, rv);
            
            rv = blastate_draw(pGame);
            ASSERT(rv == GFMRV_OK, rv);
            
            rv = gfm_drawEnd(pGame->pCtx);
            ASSERT(rv == GFMRV_OK, rv);
        }
    }
    
    rv = GFMRV_OK;
__ret:
    blastate_clean(pGame);
    
    return rv;
#endif
}

