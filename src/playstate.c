/**
 * @file src/playstate.c
 * 
 * Game's main state, where all the fun should happen
 */
#include <GFraMe/gfmGenericArray.h>

#include <ld33/playstate.h>
#include <ld33/main.h>
#include <ld33/mob.h>

gfmGenArr_define(mob);

struct stPlaystate {
    /** Array of objects */
    gfmGenArr_var(mob, pMobs);
    /** World's height */
    int height;
    /** State to be set when exiting */
    int nextState;
    /** World's width */
    int width;
    /** Player's pointer */
    mob *pPlayer;
};
typedef struct stPlaystate playstate;

/**
 * Initialize everything
 */
static gfmRV playstate_init(gameCtx *pGame) {
    gfmCamera *pCam;
    gfmRV rv;
    playstate *pState;
    mob *pMob;
    
    pState = (playstate*)pGame->pState;
    
    // TODO Parse all objects
    // TODO Parse the world
    
    // TODO Get the world's dimensions
    pState->width = 160;
    pState->height = 120;
    
    // Set camera's dimensions
    rv = gfm_getCamera(&pCam, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmCamera_setWorldDimensions(pCam, pState->width, pState->height);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Initialize the player
    gfmGenArr_getNextRef(mob, pState->pMobs, 1, pMob, mob_getNew);
    gfmGenArr_push(pState->pMobs);
    
    rv = mob_init(pMob, pGame, player, 1/*level*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = mob_setAnimations(pMob, 0/*unused*/);
    ASSERT(rv == GFMRV_OK, rv);
    pState->pPlayer = pMob;
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Clean everything
 */
static void playstate_clean(gameCtx *pGame) {
    playstate *pState;
    
    pState = (playstate*)pGame->pState;
    
    gfmGenArr_clean(pState->pMobs, mob_free);
}

/**
 * Updates the playstate
 */
static gfmRV playstate_update(gameCtx *pGame) {
    gfmRV rv;
    int i;
    playstate *pState;
    
    pState = (playstate*)pGame->pState;
    
    // Initialize the qt
    rv = gfmQuadtree_initRoot(pGame->pQt, 0/*x*/, 0/*y*/, pState->width,
            pState->height, 6/*maxDepth*/, 10/*maxNodes*/);
    ASSERT(rv == GFMRV_OK, rv);
    
    // TODO Add world to quadtree?
    
    i = 0;
    while (i < gfmGenArr_getUsed(pState->pMobs)) {
        mob *pMob;
        
        pMob = gfmGenArr_getObject(pState->pMobs, i);
        
        rv = mob_update(pMob, pGame);
        ASSERT(rv == GFMRV_OK, rv);
        
        i++;
    }
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Draws the playstate
 */
static gfmRV playstate_draw(gameCtx *pGame) {
    gfmRV rv;
    int i;
    playstate *pState;
    
    pState = (playstate*)pGame->pState;
    
    // TODO Draw the world?
    
    i = 0;
    while (i < gfmGenArr_getUsed(pState->pMobs)) {
        mob *pMob;
        
        pMob = gfmGenArr_getObject(pState->pMobs, i);
        
        rv = mob_draw(pMob, pGame);
        ASSERT(rv == GFMRV_OK, rv);
        
        i++;
    }
    
#ifdef DEBUG
    rv = gfmQuadtree_drawBounds(pGame->pQt, pGame->pCtx, 0/*colors*/);
    ASSERT(rv == GFMRV_OK, rv);
#endif
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Initialize the playstate and loop it
 */
gfmRV playstate_loop(gameCtx *pGame) {
    gfmRV rv;
    playstate psCtx;
    
    memset(&psCtx, 0x0, sizeof(playstate));
    pGame->pState = &psCtx;
    
    rv = playstate_init(pGame);
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
            
            rv = playstate_update(pGame);
            ASSERT(rv == GFMRV_OK, rv);
            
            rv = gfm_fpsCounterUpdateEnd(pGame->pCtx);
            ASSERT(rv == GFMRV_OK, rv);
        }
        
        while (gfm_isDrawing(pGame->pCtx) == GFMRV_TRUE) {
            rv = gfm_drawBegin(pGame->pCtx);
            ASSERT(rv == GFMRV_OK, rv);
            
            rv = playstate_draw(pGame);
            ASSERT(rv == GFMRV_OK, rv);
            
            rv = gfm_drawEnd(pGame->pCtx);
            ASSERT(rv == GFMRV_OK, rv);
        }
    }
    
    rv = GFMRV_OK;
__ret:
    playstate_clean(pGame);
    
    return rv;
}

