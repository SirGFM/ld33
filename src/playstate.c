/**
 * @file src/playstate.c
 * 
 * Game's main state, where all the fun should happen
 */
#include <GFraMe/gfmGenericArray.h>
#include <GFraMe/gfmGroup.h>

#include <ld33/playstate.h>
#include <ld33/main.h>
#include <ld33/mob.h>

gfmGenArr_define(mob);

struct stPlaystate {
    /** Array of objects */
    gfmGenArr_var(mob, pMobs);
    /** Leaf particles */
    gfmGroup *pGrp;
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
    pState->width = 1000;
    pState->height = 120;
    
    // Set camera's dimensions
    rv = gfm_getCamera(&pCam, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmCamera_setWorldDimensions(pCam, pState->width, pState->height);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmCamera_setDeadzone(pCam, 60/*x*/, 0/*y*/, 40/*width*/,
            120/*height*/);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Initialize the player
    gfmGenArr_getNextRef(mob, pState->pMobs, 1, pMob, mob_getNew);
    gfmGenArr_push(pState->pMobs);
    
    rv = mob_init(pMob, pGame, player, 1/*level*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = mob_setAnimations(pMob, 0/*unused*/);
    ASSERT(rv == GFMRV_OK, rv);
    pState->pPlayer = pMob;
    
    rv = gfmGroup_getNew(&(pState->pGrp));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefSpriteset(pState->pGrp, pGame->pSset4x4);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefDimensions(pState->pGrp, 4 /*width*/, 4 /*height*/,
        0/*offX*/, 0/*offY*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefVelocity(pState->pGrp, 0/*vx*/, 32/*vy*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefAcceleration(pState->pGrp, 0/*ax*/, 2/*ay*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDeathOnLeave(pState->pGrp, 1/*doDie*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDrawOrder(pState->pGrp, gfmDrawOrder_linear);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_preCache(pState->pGrp, pGame->maxParts, pGame->maxParts);
    ASSERT(rv == GFMRV_OK, rv);
    
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
    gfmGroup_free(&(pState->pGrp));
}

/**
 * Updates the playstate
 */
static gfmRV playstate_update(gameCtx *pGame) {
    gfmRV rv;
    int i, num;
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
    
    // Add a few particles every frame
    num = 3 + main_getPRNG(pGame) % 7;
    while (num > 0) {
        gfmSprite *pSpr;
        int tile, vx, vy, rng, x, y;
        
        rv = gfmGroup_recycle(&pSpr, pState->pGrp);
        ASSERT(rv == GFMRV_OK || rv == GFMRV_GROUP_MAX_SPRITES, rv);
        if (rv == GFMRV_GROUP_MAX_SPRITES) {
            break;
        }
        
        rng = main_getPRNG(pGame);
        if (rng < 0) rng = -rng;
        tile = 256 + (rng % 4);
        
        rng = main_getPRNG(pGame);
        if (rng < 0) rng = -rng;
        vy = 20 + ((rng % 8) - 6);
        
        rng = main_getPRNG(pGame);
        if (rng < 0) rng = -rng;
        vx = (rng % 8) - 4;
        
        rv = gfm_getCameraPosition(&x, &y, pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
        
        rng = main_getPRNG(pGame);
        if (rng < 0) rng = -rng;
        x += 8 + ((rng % 40) - 2) * 4;
        y = 8;
        
        rv = gfmGroup_setPosition(pState->pGrp, x, y);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmGroup_setFrame(pState->pGrp, tile);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmGroup_setVelocity(pState->pGrp, vx, vy);
        ASSERT(rv == GFMRV_OK, rv);
        
        num--;
    }
    
    // Update particles
    rv = gfmGroup_update(pState->pGrp, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

static gfmRV playstate_drawBG(gameCtx *pGame, int tile, int camX, int width) {
    gfmRV rv;
    int x, y;
    
    camX %= width;
    
    y = 0;
    x = -width;
    while (x - camX < 160) {
        rv = gfm_drawTile(pGame->pCtx, pGame->pSset256x128, x - camX, y, tile, 0);
        ASSERT(rv == GFMRV_OK, rv);
        
        x += width;
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
    int i, iniX, height, tile, width, x, y;
    playstate *pState;
    
    pState = (playstate*)pGame->pState;
    
    // Get the world position (to do paralax)
    rv = gfm_getCameraPosition(&x, &y, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_getBackbufferDimensions(&width, &height, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    x %= width;
    
    // Draw farthest paralax
    tile = 4;
    iniX = x / 4;
    width = 160;
    rv = playstate_drawBG(pGame, tile, iniX, width);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Draw particles
    rv = gfmGroup_draw(pState->pGrp, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Draw nearest paralax
    tile = 5;
    iniX = x / 2;
    width = 160;
    rv = playstate_drawBG(pGame, tile, iniX, width);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Draw background/floor
    tile = 6;
    iniX = x;
    width = 160;
    rv = playstate_drawBG(pGame, tile, iniX, width);
    ASSERT(rv == GFMRV_OK, rv);
    
    i = 0;
    while (i < gfmGenArr_getUsed(pState->pMobs)) {
        mob *pMob;
        
        pMob = gfmGenArr_getObject(pState->pMobs, i);
        
        rv = mob_draw(pMob, pGame);
        ASSERT(rv == GFMRV_OK, rv);
        
        i++;
    }
    
    // Draw foreground
    tile = 7;
    iniX = x;
    width = 160;
    rv = playstate_drawBG(pGame, tile, iniX, width);
    ASSERT(rv == GFMRV_OK, rv);
    
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
    
    //rv = gfm_recordGif(pGame->pCtx, 10000/*ms*/, "anim.gif", 8, 0);
    //ASSERT(rv == GFMRV_OK, rv);
    
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

