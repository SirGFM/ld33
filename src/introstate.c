/**
 * @file src/introstate.c
 * 
 * Game's title screen
 */
#include <GFraMe/gfmGenericArray.h>
#include <GFraMe/gfmGroup.h>
#include <GFraMe/gfmParser.h>
#include <GFraMe/gfmText.h>

#include <ld33/introstate.h>
#include <ld33/main.h>

#include <string.h>

struct stIntrostate {
    gfmSprite *pTitle[10];
    gfmText *pText;
};
typedef struct stIntrostate introstate;

#ifdef EMSCRIPT
static introstate isCtx;
#endif

/**
 * Initialize everything
 */
static gfmRV introstate_init(gameCtx *pGame) {
    gfmCamera *pCam;
    gfmRV rv;
    int i, y;
    introstate *pState;
    
    pState = (introstate*)pGame->pState;
    
    rv = gfm_getCamera(&pCam, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmCamera_centerAtPoint(pCam, 0/*x*/, 0/*y*/);
    ASSERT(rv == GFMRV_OK || rv == GFMRV_CAMERA_DIDNT_MOVE ||
            rv == GFMRV_CAMERA_MOVED, rv);
    
    i = 0;
    while (i < 10) {
        int frame, x, y;
        
        x = (160 - 5 * 32) / 2;
        x += 32 * (i % 5);
        y = 0;
        y += 32 * (i / 5);
        
        frame = 96 + (i % 5);
        frame += 16 * (i / 5);
        
        rv = gfmSprite_getNew(&(pState->pTitle[i]));
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmSprite_init(pState->pTitle[i], x, y, 32/*w*/, 32/*h*/,
                pGame->pSset32x32, 0/*offX*/, 0/*offY*/, 0/*child*/, 0/*type*/);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmSprite_setFrame(pState->pTitle[i], frame);
        
        i++;
    }
    
    y = 72;
    rv = gfmText_getNew(&(pState->pText));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmText_init(pState->pText, 0/*x*/, y, 160 / 8/*w*/,
            (120 - y) / 8 /*h*/, 66/*delay*/, 0/*bindToScreen!*/,
            pGame->pSset8x8, 0/*firstTile*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmText_setTextStatic(pState->pText, "    A GAME BY GFM\n\nMADE IN 48 "
            "HOURS FOR LD#33\n\n\n\nPRESS ANY KEY TO START...\n\nPRESS ESC TO "
            "EXIT", 1/*doCopy*/);
    ASSERT(rv == GFMRV_OK, rv);
    
    rv = GFMRV_OK;
__ret:
    
    return rv;
}

/**
 * Clean everything
 */
static void introstate_clean(gameCtx *pGame) {
    introstate *pState;
    int i;
    
    pState = (introstate*)pGame->pState;
    
    i = 0;
    while (i < 10) {
        gfmSprite_free(&(pState->pTitle[i]));
        i++;
    }
    gfmText_free(&(pState->pText));
}

/**
 * Updates the introstate
 */
static gfmRV introstate_update(gameCtx *pGame) {
    gfmInputIface iface;
    gfmRV rv;
    int i;
    introstate *pState;
    
    pState = (introstate*)pGame->pState;
    
    rv = gfm_getLastPressed(&iface, pGame->pCtx);
    ASSERT(rv == GFMRV_OK || rv == GFMRV_WAITING, rv);
    if (rv == GFMRV_OK) {
        // switch state
        pGame->quitState = 1;
        if (pGame->didWin || pGame->didLose) {
            pGame->state = state_playstate;
        }
        else {
            pGame->state = state_blastate;
        }
    }
    
    i = 0;
    while (i < 10) {
        gfmSprite *pSpr;
        
        pSpr = pState->pTitle[i];
        
        rv = gfmSprite_update(pSpr, pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
        
        i++;
    }
    
    rv = gfmText_update(pState->pText, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Draws the introstate
 */
static gfmRV introstate_draw(gameCtx *pGame) {
    gfmRV rv;
    int i;
    introstate *pState;
    
    pState = (introstate*)pGame->pState;
    
    i = 0;
    while (i < 10) {
        gfmSprite *pSpr;
        
        pSpr = pState->pTitle[i];
        
        rv = gfmSprite_draw(pSpr, pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
        
        i++;
    }
    rv = gfmText_draw(pState->pText, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Initialize the introstate and loop it
 */
gfmRV introstate_loop(gameCtx *pGame) {
#ifdef EMSCRIPT
    gfmRV rv;
    
    // Initialize the state, if needed
    if (!pGame->isInit) {
        memset(&isCtx, 0x0, sizeof(introstate));
        pGame->pState = &isCtx;
        
        rv = introstate_init(pGame);
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
        
        rv = introstate_update(pGame);
        ASSERT(rv == GFMRV_OK, rv);
        
        rv = gfm_fpsCounterUpdateEnd(pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    while (gfm_isDrawing(pGame->pCtx) == GFMRV_TRUE) {
        rv = gfm_drawBegin(pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
        
        rv = introstate_draw(pGame);
        ASSERT(rv == GFMRV_OK, rv);
        
        rv = gfm_drawEnd(pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
__ret:
    if (pGame->quitState || rv != GFMRV_OK) {
        introstate_clean(pGame);
        pGame->isInit = 0;
        pGame->quitState = 0;
    }
    
    return rv;
#else
    gfmRV rv;
    introstate isCtx;
    
    memset(&isCtx, 0x0, sizeof(introstate));
    pGame->pState = &isCtx;
    
    rv = introstate_init(pGame);
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
            
            rv = introstate_update(pGame);
            ASSERT(rv == GFMRV_OK, rv);
            
            rv = gfm_fpsCounterUpdateEnd(pGame->pCtx);
            ASSERT(rv == GFMRV_OK, rv);
        }
        
        while (gfm_isDrawing(pGame->pCtx) == GFMRV_TRUE) {
            rv = gfm_drawBegin(pGame->pCtx);
            ASSERT(rv == GFMRV_OK, rv);
            
            rv = introstate_draw(pGame);
            ASSERT(rv == GFMRV_OK, rv);
            
            rv = gfm_drawEnd(pGame->pCtx);
            ASSERT(rv == GFMRV_OK, rv);
        }
    }
    
    rv = GFMRV_OK;
__ret:
    introstate_clean(pGame);
    
    return rv;
#endif
}

