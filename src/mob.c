/**
 * @file include/ld33/mob.h
 * 
 * Generic 'object'; May be either the player, a NPC or a shadow
 */
#include <GFraMe/gfmObject.h>
#include <GFraMe/gfmSprite.h>

#include <ld33/mob.h>

#include <stdlib.h>
#include <string.h>

/** 'Export' the mob struct */
struct stMob {
    /** The mob's sprite and main hitbox */
    gfmSprite *pSelf;
    /** Hitbox to detect what around the mob */
    gfmObject *pScan;
    /** Hitbox to attack stuff */
    gfmObject *pAtk;
    /** Whether this mob is alive */
    int isAlive;
    /** The mob's level defines it's health and attack */
    int level;
    /** Current health (max can be calculated from the level */
    int health;
    /** Current attack power (i.e., how much damage it deal) */
    int atkPower;
};

/**
 * Alloc a new mob
 */
gfmRV mob_getNew(mob **ppMob) {
    gfmRV rv;
    
    ASSERT(ppMob, GFMRV_ARGUMENTS_BAD);
    ASSERT(!(*ppMob), GFMRV_ARGUMENTS_BAD);
    
    *ppMob = (mob*)malloc(sizeof(ppMob));
    ASSERT(*ppMob, GFMRV_ALLOC_FAILED);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Free a mob's memory
 */
gfmRV mob_free(mob **ppMob) {
    gfmRV rv;
    
    ASSERT(ppMob, GFMRV_ARGUMENTS_BAD);
    ASSERT(*ppMob, GFMRV_ARGUMENTS_BAD);
    
    // TODO Check if anything else should be freed
    free(*ppMob);
    *ppMob = 0;
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Initializa a mob; To ease memory management, it doesn't alloc memory;
 * Instead, it used objects from the game's array (which are cleaned when the
 * game exits)
 * 
 * @param  pMob  The mob
 * @param  pGame The game's global contex
 * @param  type  Main mob type (e.g., player, NPC, shadow, ...); Its scan and
 *               attack type will be infered from this
 */
gfmRV mob_init(mob *pMob, gameCtx *pGame, int type, int level) {
    gfmRV rv;
    gfmSprite *pSpr;
    gfmObject *pObj1, *pObj2;
    int atkType, scanType, scanWidth, scanHeight, width;
    
    ASSERT(pMob, GFMRV_ARGUMENTS_BAD);
    ASSERT(pGame, GFMRV_ARGUMENTS_BAD);
    // TODO Add more types
    ASSERT(type == player || type == npc || type == shadow,
            GFMRV_ARGUMENTS_BAD);
    
    // Retrieve all needed objects
    gfmGenArr_getNextRef(gfmObject, pGame->pObjs, 1, pObj1, gfmObject_getNew);
    gfmGenArr_push(pGame->pObjs);
    gfmGenArr_getNextRef(gfmObject, pGame->pObjs, 1, pObj2, gfmObject_getNew);
    gfmGenArr_push(pGame->pObjs);
    gfmGenArr_getNextRef(gfmSprite, pGame->pSprs, 1, pSpr, gfmSprite_getNew);
    gfmGenArr_push(pGame->pSprs);
    
    pMob->pSelf = pSpr;
    pMob->pScan = pObj1;
    pMob->pAtk = pObj2;
    
    // Initialize the mob according to its type
    switch (type) {
        case player: {
            atkType = player_atk;
            scanType = player_scan;
            
            width = 12;
            
            scanWidth = 64;
            scanHeight = 24;
        } break;
        case npc: {
            atkType = npc_atk;
            scanType = npc_scan;
        } break;
        case shadow: {
            atkType = shadow_atk;
            scanType = shadow_scan;
        } break;
        default: ASSERT(0, GFMRV_INTERNAL_ERROR);
    }
    rv = gfmSprite_init(pMob->pSelf, 0/*x*/, 0/*y*/, width, 4/*height*/,
            pGame->pSset32x32, -10/*offX*/, -28/*offY*/, pMob/*pChild*/, type);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmObject_init(pMob->pAtk, -100/*x*/, -100/*y*/, (32 - width) / 2,
            4/*height*/, pMob/*pChild*/, atkType);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmObject_init(pMob->pScan, -100/*x*/, -100/*y*/, scanWidth,
            scanHeight, pMob/*pChild*/, scanType);
    ASSERT(rv == GFMRV_OK, rv);
    
    // TODO Load and play animation
    
    rv = gfmSprite_setFrame(pMob->pSelf, 16/*frame*/);
    
    // TODO Set stats from level and type
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV mob_setPosition(mob *pMob, int x, int y) {
    // TODO Convert from world->weird space?
    return gfmSprite_setPosition(pMob->pSelf, x, y);
}

gfmRV mob_update(mob *pMob, gameCtx *pGame) {
    gfmRV rv;
    int h, w, x, y;
    
    // TODO Check type and 'advance the AI'
    
    rv = gfmSprite_update(pMob->pSelf, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Get the current position
    rv = gfmSprite_getPosition(&x, &y, pMob->pSelf);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_getDimensions(&w, &h, pMob->pSelf);
    ASSERT(rv == GFMRV_OK, rv);
    // Adjust to the sprite's center
    x += w / 2;
    y += h / 2;
    
    // TODO If attacking, position the attack hitbox accordingly
    
    // Set the scan position
    rv = gfmObject_getDimensions(&w, &h, pMob->pScan);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmObject_setPosition(pMob->pScan, x - w / 2, y - h / 2);
    ASSERT(rv == GFMRV_OK, rv);
    
    // TODO Add it to the quadtree
    rv =  gfmQuadtree_populateObject(pGame->pQt, pMob->pScan);
    ASSERT(rv == GFMRV_OK, rv);
    rv =  gfmQuadtree_populateObject(pGame->pQt, pMob->pAtk);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmQuadtree_populateSprite(pGame->pQt, pMob->pSelf);
    ASSERT(rv == GFMRV_OK, rv);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV mob_draw(mob *pMob, gameCtx *pGame) {
    return gfmSprite_draw(pMob->pSelf, pGame->pCtx);
}

