/**
 * @file src/collision.c
 */
#include <ld33/collision.h>
#include <ld33/mob.h>

gfmRV collide_mobXWall(gfmObject *pMob, gfmObject *pWall) {
    gfmRV rv;
    
    rv = gfmObject_separateHorizontal(pMob, pWall);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmObject_separateVertical(pMob, pWall);
    ASSERT(rv == GFMRV_OK, rv);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV doCollide(gameCtx *pGame) {
    gfmRV rv;
    
    rv = GFMRV_QUADTREE_OVERLAPED;
    while (rv != GFMRV_QUADTREE_DONE) {
        gfmObject *pObj1, *pObj2;
        mob *pMob1, *pMob2;
        int type1, type2;
        
        rv = gfmQuadtree_getOverlaping(&pObj1, &pObj2, pGame->pQt);
        ASSERT(rv == GFMRV_OK, rv);
        
        rv = gfmObject_getChild((void**)&pMob1, &type1, pObj1);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmObject_getChild((void**)&pMob2, &type2, pObj1);
        ASSERT(rv == GFMRV_OK, rv);
        
        if (type1 == collideable && (type2 == player || type2 == shadow)) {
            rv = collide_mobXWall(pObj1, pObj2);
            ASSERT(rv == GFMRV_OK, rv);
        }
        else if (type2 == collideable && (type1 == player || type1 == shadow)) {
            rv = collide_mobXWall(pObj2, pObj1);
            ASSERT(rv == GFMRV_OK, rv);
        }
        else {
            // TODO Collide atk vs hitbox and hitbox vs scan
        }
        
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
