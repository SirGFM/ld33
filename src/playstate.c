/**
 * @file src/playstate.c
 * 
 * Game's main state, where all the fun should happen
 */
#include <ld33/playstate.h>

struct stPlaystate {
};
typedef struct stPlaystate playstate;

/**
 * Initialize everything
 */
static gfmRV playstate_init(gameCtx *pGame) {
    gfmRV rv;
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Updates the playstate
 */
static gfmRV playstate_update(gameCtx *pGame) {
    gfmRV rv;
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Draws the playstate
 */
static gfmRV playstate_draw(gameCtx *pGame) {
    gfmRV rv;
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Initialize the playstate and loop it
 */
gfmRV playstate_loop(gameCtx *pGame) {
    gfmRV rv;
    
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
    return rv;
}

