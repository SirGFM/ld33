/**
 * @file src/playstate.c
 * 
 * Game's main state, where all the fun should happen
 */
#include <GFraMe/gfmGenericArray.h>
#include <GFraMe/gfmGroup.h>
#include <GFraMe/gfmParser.h>

#include <ld33/playstate.h>
#include <ld33/main.h>
#include <ld33/mob.h>

#include <string.h>

gfmGenArr_define(mob);

struct stPlaystate {
    /** Array of objects */
    gfmGenArr_var(mob, pMobs);
    /** Leaf particles */
    gfmGroup *pGrp;
    /** world bounds */
    gfmObject *pWorld[4];
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
    gfmParser *pParser;
    gfmRV rv;
    int curWorld;
    playstate *pState;
    
    pState = (playstate*)pGame->pState;
    pParser = 0;
    
    // Parse all objects
    rv = gfmParser_getNew(&pParser);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_initStatic(pParser, pGame->pCtx, "map.gfm");
    ASSERT(rv == GFMRV_OK, rv);
    
    pState->width = 0;
    pState->height = 120;
    curWorld = 0;
    while (rv != GFMRV_PARSER_FINISHED) {
        char *pType;
        gfmParserType type;
        int x, y;
        
        rv = gfmParser_parseNext(pParser);
        ASSERT(rv == GFMRV_OK || rv == GFMRV_PARSER_FINISHED, rv);
        if (rv == GFMRV_PARSER_FINISHED) {
            break;
        }
        
        rv = gfmParser_getType(&type, pParser);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmParser_getPos(&x, &y, pParser);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmParser_getIngameType(&pType, pParser);
        ASSERT(rv == GFMRV_OK, rv);
        
#define CHECK_TYPE(str) strcmp(pType, str) == 0
        if (type == gfmParserType_area) {
            int height, width;
            
            if (CHECK_TYPE("collideable")) {
                gfmGenArr_getNextRef(gfmObject, pGame->pObjs, 1,
                        pState->pWorld[curWorld], gfmObject_getNew);
                gfmGenArr_push(pGame->pObjs);

                rv = gfmParser_getDimensions(&width, &height, pParser);
                ASSERT(rv == GFMRV_OK, rv);
                
                rv = gfmObject_init(pState->pWorld[curWorld], x, y, width,
                        height, 0/*child*/, collideable/*type*/);
                ASSERT(rv == GFMRV_OK, rv);
                rv = gfmObject_setFixed(pState->pWorld[curWorld]);
                ASSERT(rv == GFMRV_OK, rv);
                
                // Get the world's dimensions
                if (width > pState->width) {
                    pState->width = width;
                }
                
                curWorld++;
            } // if type == collideable
            else {
                ASSERT(0, GFMRV_INTERNAL_ERROR);
            }
        } // if type == area
        else if (type == gfmParserType_object) {
            mob *pMob;
            
            // Initialize the mob
            gfmGenArr_getNextRef(mob, pState->pMobs, 1, pMob, mob_getNew);
            gfmGenArr_push(pState->pMobs);
            
            if (CHECK_TYPE("player")) {
                rv = mob_init(pMob, pGame, player, 1/*level*/);
                ASSERT(rv == GFMRV_OK, rv);
                
                rv = mob_setAnimations(pMob, 0/*unused*/);
                ASSERT(rv == GFMRV_OK, rv);
                
                rv = mob_setPosition(pMob, x, y);
                ASSERT(rv == GFMRV_OK, rv);
                
                pState->pPlayer = pMob;
            } // if type == player
            else if (CHECK_TYPE("shadow")) {
                int num, dist, level, subtype, traits;
                
                dist = 48;
                level = 1;
                subtype = EN_NONE;
                traits = TR_NONE;
                
                rv = gfmParser_getNumProperties(&num, pParser);
                ASSERT(rv == GFMRV_OK, rv);
                while (num > 0) {
                    char *pKey, *pVal;
                    
                    rv = gfmParser_getProperty(&pKey, &pVal, pParser, num - 1);
                    ASSERT(rv == GFMRV_OK, rv);
                    
#define CHECK_KEY(str) strcmp(pKey, str) == 0
#define CHECK_VAL(str) strcmp(pVal, str) == 0
                    if (CHECK_KEY("level")) {
                        level = 0;
                        while (*pVal) {
                            level = level * 10 + (*pVal) - '0';
                            pVal++;
                        }
                    }
                    else if (CHECK_KEY("subtype")) {
                        if (CHECK_VAL("slime")) subtype = EN_SLIME;
                        else if (CHECK_VAL("angrySlime")) subtype = EN_ANGRYSLIME;
                        else ASSERT(0, GFMRV_INTERNAL_ERROR);
                    }
                    else if (CHECK_KEY("trait")) {
                        if (CHECK_VAL("coward")) {
                            traits |= TR_COWARD;
                        }
                        else if (CHECK_VAL("neutral")) {
                            traits |= TR_NEUTRAL;
                        }
                        else if (CHECK_VAL("angry")) {
                            traits |= TR_ANGRY;
                        }
                        else if (CHECK_VAL("swarmer")) {
                            traits |= TR_SWARMER;
                        }
                        else {
                            ASSERT(0, GFMRV_INTERNAL_ERROR);
                        }
                    }
                    else if (CHECK_KEY("dist")) {
                        dist = 0;
                        while (*pVal) {
                            dist = dist * 10 + (*pVal) - '0';
                            pVal++;
                        }
                    }
                    else {
                        ASSERT(0, GFMRV_INTERNAL_ERROR);
                    }
#undef CHECK_VAL
#undef CHECK_KEY
                    num--;
                } // while num < numProps
                
                rv = mob_init(pMob, pGame, shadow, level);
                ASSERT(rv == GFMRV_OK, rv);
                rv = mob_setPosition(pMob, x, y);
                ASSERT(rv == GFMRV_OK, rv);
                rv = mob_setTraits(pMob, traits);
                ASSERT(rv == GFMRV_OK, rv);
                rv = mob_setAnimations(pMob, subtype);
                ASSERT(rv == GFMRV_OK, rv);
                rv = mob_setDist(pMob, dist);
                ASSERT(rv == GFMRV_OK, rv);
            } // if pType == "shadow"
            else if (CHECK_TYPE("wall")) {
            }
            else {
                ASSERT(0, GFMRV_INTERNAL_ERROR);
            }
        }
        else {
            ASSERT(0, GFMRV_INTERNAL_ERROR);
        }
#undef CHECK_TYPE
    }
    
    // Set camera's dimensions
    rv = gfm_getCamera(&pCam, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmCamera_setWorldDimensions(pCam, pState->width, pState->height);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmCamera_setDeadzone(pCam, 60/*x*/, 0/*y*/, 40/*width*/,
            120/*height*/);
    ASSERT(rv == GFMRV_OK, rv);
    
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
    rv = gfmGroup_setDeathOnTime(pState->pGrp, 4000/*ttl*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDeathOnLeave(pState->pGrp, 0/*dontDie*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDrawOrder(pState->pGrp, gfmDrawOrder_linear);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_preCache(pState->pGrp, pGame->maxParts, pGame->maxParts);
    ASSERT(rv == GFMRV_OK, rv);
    
    rv = GFMRV_OK;
__ret:
    gfmParser_free(&pParser);
    
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
    
    // Add world to quadtree
    i = 0;
    while (i < 4) {
        rv = gfmQuadtree_populateObject(pGame->pQt, pState->pWorld[i]);
        ASSERT(rv == GFMRV_OK, rv);
        
        i++;
    }
    
    i = 0;
    while (i < gfmGenArr_getUsed(pState->pMobs)) {
        mob *pMob;
        
        pMob = gfmGenArr_getObject(pState->pMobs, i);
        
        rv = mob_update(pMob, pGame);
        ASSERT(rv == GFMRV_OK, rv);
        
        i++;
    }
    
    rv = gfmGroup_update(pGame->pRender, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    
    i = 0;
    while (i < gfmGenArr_getUsed(pState->pMobs)) {
        mob *pMob;
        
        pMob = gfmGenArr_getObject(pState->pMobs, i);
        
        rv = mob_postUpdate(pMob, pGame);
        ASSERT(rv == GFMRV_OK, rv);
        
        i++;
    }
    
    // Add a few particles every frame
    num = 5 + main_getPRNG(pGame) % 10;
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
        x += (rng % 60) * 8 - 160;
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
    int iniX, height, tile, width, x, y;
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
    
    rv = gfmGroup_draw(pGame->pRender, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    /*
    i = 0;
    while (i < gfmGenArr_getUsed(pState->pMobs)) {
        mob *pMob;
        
        pMob = gfmGenArr_getObject(pState->pMobs, i);
        
        rv = mob_draw(pMob, pGame);
        ASSERT(rv == GFMRV_OK, rv);
        
        i++;
    }
    */
    
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

