/**
 * @file src/collision.c
 */
#include <ld33/collision.h>
#include <ld33/mob.h>
#include <ld33/playstate.h>

static gfmRV collide_atkXMob(gfmObject *pAtk, mob *pMob, gameCtx *pGame) {
    gfmRV rv;
    int type;
    mob *pSelf;
    
    rv = gfmObject_getChild((void**)&pSelf, &type, pAtk);
    ASSERT(rv == GFMRV_OK, rv);
    if (pSelf == pMob) {
        rv = GFMRV_OK;
        goto __ret;
    }
    
    rv = mob_attack(pSelf, pMob, pGame);
    ASSERT(rv == GFMRV_OK, rv);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

static gfmRV collide_scanXMob(gfmObject *pScan, mob *pMob) {
    gfmRV rv;
    int type;
    mob *pSelf;
    
    // If the scanner is "scanning itself" or is the player, stop
    rv = gfmObject_getChild((void**)&pSelf, &type, pScan);
    ASSERT(rv == GFMRV_OK, rv);
    if (pSelf == pMob || type == player) {
        rv = GFMRV_OK;
        goto __ret;
    }
    
    rv = mob_setOnView(pSelf, pMob);
    ASSERT(rv == GFMRV_OK, rv);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

static gfmRV collide_mobXWall(gfmObject *pMob, gfmObject *pWall) {
    gfmCollision dir;
    gfmRV rv;
    int x, y;
    
    rv = gfmObject_collide(pMob, pWall);
    ASSERT(rv == GFMRV_TRUE || rv == GFMRV_FALSE, rv);
    
    rv = gfmObject_getCurrentCollision(&dir, pMob);
    ASSERT(rv == GFMRV_OK, rv);
    rv =  gfmObject_getPosition(&x, &y, pMob);
    ASSERT(rv == GFMRV_OK, rv);
    
    if (dir & gfmCollision_left) {
        x += 1;
    }
    else if (dir & gfmCollision_left) {
        x -= 1;
    }
    if (dir & gfmCollision_up) {
        y += 2;
    }
    else if (dir & gfmCollision_down) {
        y -= 1;
    }
    
    rv = gfmObject_setPosition(pMob, x, y);
__ret:
    return rv;
}

static gfmRV doCollide(gameCtx *pGame) {
    gfmRV rv;
    
    rv = GFMRV_QUADTREE_OVERLAPED;
    while (rv != GFMRV_QUADTREE_DONE) {
        gfmObject *pObj1, *pObj2;
        gfmSprite *pSpr1, *pSpr2;
        mob *pMob1, *pMob2;
        int type1, type2;
        
        rv = gfmQuadtree_getOverlaping(&pObj1, &pObj2, pGame->pQt);
        ASSERT(rv == GFMRV_OK, rv);
        
        rv = gfmObject_getChild((void**)&pSpr1, &type1, pObj1);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmObject_getChild((void**)&pSpr2, &type2, pObj2);
        ASSERT(rv == GFMRV_OK, rv);
        
        if (type1 == gfmType_sprite) {
            rv = gfmSprite_getChild((void**)&pMob1, &type1, pSpr1);
            ASSERT(rv == GFMRV_OK, rv);
        }
        if (type2 == gfmType_sprite) {
            rv = gfmSprite_getChild((void**)&pMob2, &type2, pSpr2);
            ASSERT(rv == GFMRV_OK, rv);
        }
        
        if (type1 == win && type2 == player) {
            if (pGame->state == state_playstate) {
                rv = playstate_setWin(pGame);
            }
        }
        else if (type2 == win && type1 == player) {
            if (pGame->state == state_playstate) {
                rv = playstate_setWin(pGame);
            }
        }
        else if ((type1 == wall || type1 == collideable) &&
                (type2 == player || type2 == shadow)) {
            rv = collide_mobXWall(pObj2, pObj1);
        }
        else if ((type2 == wall || type2 == collideable) &&
                (type1 == player || type1 == shadow)) {
            rv = collide_mobXWall(pObj1, pObj2);
        }
        else if (type1 == scan && (type2 == player || type2 == shadow)) {
            rv = collide_scanXMob(pObj1, pMob2);
        }
        else if (type2 == scan && (type1 == player || type1 == shadow)) {
            rv = collide_scanXMob(pObj2, pMob1);
        }
        else if (type1 == atk && (type2 == player || type2 == shadow ||
                type2 == wall)) {
            rv = collide_atkXMob(pObj1, pMob2, pGame);
        }
        else if (type2 == atk && (type1 == player || type1 == shadow ||
                type1 == wall)) {
            rv = collide_atkXMob(pObj2, pMob1, pGame);
        }
        else {
            // Collision between mob's hitboxes, do nothing!
        }
        ASSERT(rv == GFMRV_OK, rv);
        
        rv = gfmQuadtree_continue(pGame->pQt);
        ASSERT(rv == GFMRV_QUADTREE_OVERLAPED || rv == GFMRV_QUADTREE_DONE,
                rv);
    }
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV collide_obj(gfmObject *pObj, gameCtx *pGame) {
    gfmRV rv;
    
    rv = gfmQuadtree_collideObject(pGame->pQt, pObj);
    ASSERT(rv == GFMRV_QUADTREE_OVERLAPED || rv == GFMRV_QUADTREE_DONE,
            rv);
    
    if (rv == GFMRV_QUADTREE_OVERLAPED) {
        rv = doCollide(pGame);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV collide_spr(gfmSprite *pSpr, gameCtx *pGame) {
    gfmRV rv;
    
    rv = gfmQuadtree_collideSprite(pGame->pQt, pSpr);
    ASSERT(rv == GFMRV_QUADTREE_OVERLAPED || rv == GFMRV_QUADTREE_DONE,
            rv);
    
    if (rv == GFMRV_QUADTREE_OVERLAPED) {
        rv = doCollide(pGame);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

