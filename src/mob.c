/**
 * @file include/ld33/mob.h
 * 
 * Generic 'object'; May be either the player, a NPC or a shadow
 */
#include <GFraMe/gfmObject.h>
#include <GFraMe/gfmSprite.h>

#include <ld33/collision.h>
#include <ld33/main.h>
#include <ld33/mob.h>

#include <stdlib.h>
#include <string.h>

#define NEG_INF -100000

enum {
    ANIM_STAND = 0,
    ANIM_WALK,
    ANIM_ATK,
    ANIM_HIT,
    ANIM_DASH,
    ANIM_DEATH,
    ANIM_PL_TRANSFORM,
    ANIM_PL_MAX,
};
#define ANIM_MAX ANIM_PL_TRANSFORM

/** Player animation */
int pPlayerAnim[] = {
/*                   */ /*len|fps|loop|frames...*/
/* ANIM_STAND        */    8 , 8 ,  1 , 16,17,16,17,16,18,16,17,
/* ANIM_WALK         */    4 , 10,  1 , 19,22,19,20,
/* ANIM_ATK          */    6 , 12,  0 , 17,22,27,27,22,17,
/* ANIM_HIT          */    8 , 8 ,  0 , 25,26,25,26,25,26,25,26,
/* ANIM_DASH         */    4 , 12,  1 , 22,24,23,21,
/* ANIM_DEATH        */    4 , 8 ,  0 , 25,26,28,29,
/* ANIM_PL_TRANSFORM */    1 , 0 ,  0 , 16,
/* ANIM_PL_MAX       */    0
};

/** Slime animation */
int pSlimeAnim[] = {
/*            */ /*len|fps|loop|frames...*/
/* ANIM_STAND */    2 , 8 ,  1 , 48,49,
/* ANIM_WALK  */    4 , 8 ,  1 , 48,50,51,50,
/* ANIM_ATK   */    6 , 12,  0 , 50,51,51,51,51,50,
/* ANIM_HIT   */    8 , 8 ,  0 , 52,53,52,53,52,53,52,53,
/* ANIM_DASH  */    1 , 0 ,  0 , 48,
/* ANIM_DEATH */    4 , 12,  0 , 52,53,54,55,
/* ANIM_MAX   */    0
};

/** Angry slime animation */
int pAngrySlimeAnim[] = {
/*            */ /*len|fps|loop|frames...*/
/* ANIM_STAND */    2 , 8 ,  1 , 56,57,
/* ANIM_WALK  */    4 , 8 ,  1 , 56,58,59,58,
/* ANIM_ATK   */    6 , 12,  0 , 58,59,59,59,59,58,
/* ANIM_HIT   */    8 , 8 ,  0 , 60,61,60,61,60,61,60,61,
/* ANIM_DASH  */    1 , 0 ,  0 , 56,
/* ANIM_DEATH */    4 , 12,  0 , 60,61,62,63,
/* ANIM_MAX   */    0
};

/** Angry slime animation */
int pSwarmSlimeAnim[] = {
/*            */ /*len|fps|loop|frames...*/
/* ANIM_STAND */    2 , 8 ,  1 , 64,65,
/* ANIM_WALK  */    4 , 8 ,  1 , 64,66,67,66,
/* ANIM_ATK   */    6 , 12,  0 , 66,67,67,67,67,66,
/* ANIM_HIT   */    8 , 8 ,  0 , 68,69,68,69,68,69,68,69,
/* ANIM_DASH  */    1 , 0 ,  0 , 64,
/* ANIM_DEATH */    4 , 12,  0 , 68,69,70,71,
/* ANIM_MAX   */    0
};

/** Wall animation */
int pWallAnim[] = {
/*            */ /*len|fps|loop|frames...*/
/* ANIM_STAND */    1 , 0 ,  0 , 80,
/* ANIM_WALK  */    1 , 0 ,  0 , 80,
/* ANIM_ATK   */    1 , 0 ,  0 , 80,
/* ANIM_HIT   */    1 , 0 ,  0 , 80,
/* ANIM_DASH  */    1 , 0 ,  0 , 80,
/* ANIM_DEATH */    1 , 0 ,  0 , 81,
/* ANIM_MAX   */    0
};

enum {
    MOVE_STAND      = 0x0000,
    MOVE_DOWN       = 0x0001,
    MOVE_LEFT       = 0x0002,
    MOVE_RIGHT      = 0x0004,
    MOVE_UP         = 0x0008,
    MOVE_DASH_DOWN  = 0x0010,
    MOVE_DASH_LEFT  = 0x0020,
    MOVE_DASH_RIGHT = 0x0040,
    MOVE_DASH_UP    = 0x0080,
    MOVE_MAX,
    MOVE_WALK       = 0x000F,
    MOVE_DASH       = 0x00F0
};

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
    /** (Redundant) The mob's type */
    int type;
    /** Traits */
    int traits;
    /** Previous movement */
    int lastMove;
    /** For how long we can dash */
    int dashTime;
    /** For how long we've been dashing */
    int curDashTimer;
    int isAttacking;
    int isHurt;
    int invulnerableTime;
    int nearbyShadowCount;
    int dist;
    int distX;
    int distY;
    int plLastPosX;
    int plLastPosY;
    /** Horizontal speed when dashing */
    double dashHorSpeed;
    /** Vertical speed when dashing */
    double dashVerSpeed;
    /** Horizontal speed */
    double horSpeed;
    /** Vertical speed */
    double verSpeed;
};

static gfmRV mob_getDist(int *pDist, mob *pSelf, int ox, int oy) {
    gfmRV rv;
    
    int sx, sy, w, h;
    
    if (ox == NEG_INF) {
        *pDist = 1000000;
        rv = GFMRV_OK;
        goto __ret;
    }

    rv = gfmSprite_getPosition(&sx, &sy, pSelf->pSelf);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_getDimensions(&w, &h, pSelf->pSelf);
    ASSERT(rv == GFMRV_OK, rv);
    sx += w / 2;
    sy += h / 2;

    pSelf->distX = sx - ox;
    pSelf->distY = oy - sy;
    
    *pDist  = 1.5 * (pSelf->distX * pSelf->distX);
    *pDist += 0.8 * (pSelf->distY * pSelf->distY);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Alloc a new mob
 */
gfmRV mob_getNew(mob **ppMob) {
    gfmRV rv;
    
    ASSERT(ppMob, GFMRV_ARGUMENTS_BAD);
    ASSERT(!(*ppMob), GFMRV_ARGUMENTS_BAD);
    
    *ppMob = (mob*)malloc(sizeof(mob));
    ASSERT(*ppMob, GFMRV_ALLOC_FAILED);
    
    memset(*ppMob, 0x0, sizeof(mob));
    
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
    int height, offX, offY, scanWidth, scanHeight, width;
    
    ASSERT(pMob, GFMRV_ARGUMENTS_BAD);
    ASSERT(pGame, GFMRV_ARGUMENTS_BAD);
    // TODO Add more types
    ASSERT(type == player || type == npc || type == shadow || type == wall,
            GFMRV_ARGUMENTS_BAD);
    
    // Retrieve all needed objects
    pSpr = 0;
    pObj1 = 0;
    pObj2 = 0;
    
    
    width = 12;
    height = 4;
    offX = -10;
    offY = -28;
    if (type != wall) {
        gfmGenArr_getNextRef(gfmObject, pGame->pObjs, 1, pObj1, gfmObject_getNew);
        gfmGenArr_push(pGame->pObjs);
        
        gfmGenArr_getNextRef(gfmObject, pGame->pObjs, 1, pObj2, gfmObject_getNew);
        gfmGenArr_push(pGame->pObjs);
    }
    else {
        width = 32;
        offX = 0;
        height = 12;
        offY = -24;
    }
    rv = gfmGroup_recycle(&pSpr, pGame->pRender);
    ASSERT(rv == GFMRV_OK, rv);
    
    scanWidth = 64;
    scanHeight = 24;
    if (pSpr) {
        pMob->pSelf = pSpr;
        rv = gfmSprite_init(pMob->pSelf, 0/*x*/, 0/*y*/, width, height,
                pGame->pSset32x32, offX, offY, pMob/*pChild*/, type);
        ASSERT(rv == GFMRV_OK, rv);
    }
    if (pObj1) {
        pMob->pAtk = pObj1;
        rv = gfmObject_init(pMob->pAtk, -100/*x*/, -100/*y*/, (32 - width) / 2,
                4/*height*/, pMob/*pChild*/, atk);
        ASSERT(rv == GFMRV_OK, rv);
    }
    if (pObj2) {
        pMob->pScan = pObj2;
        rv = gfmObject_init(pMob->pScan, -100/*x*/, -100/*y*/, scanWidth,
                scanHeight, pMob/*pChild*/, scan);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    if (type == wall) {
        rv = gfmSprite_setFixed(pSpr);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    pMob->level = level;
    pMob->type = type;
    pMob->isAlive = 1;
    pMob->plLastPosX = NEG_INF;
    pMob->plLastPosY = NEG_INF;
    
    rv = gfmSprite_setFrame(pMob->pSelf, 16/*frame*/);
__ret:
    return rv;
}

gfmRV mob_setAnimations(mob *pMob, int subtype) {
    gfmRV rv;
    int len, *pData;
    
#define GET_DATA(data) \
        pData = data; \
        len = sizeof(data) / sizeof(int) - 1
    
    pData = 0;
    switch (pMob->type) {
        case player: GET_DATA(pPlayerAnim); break;
        case shadow: {
            switch (subtype) {
                case EN_SLIME: GET_DATA(pSlimeAnim); break;
                case EN_ANGRYSLIME: GET_DATA(pAngrySlimeAnim); break;
                case EN_SWARMSLIME: GET_DATA(pSwarmSlimeAnim); break;
                default: ASSERT(0, GFMRV_FUNCTION_NOT_IMPLEMENTED);
            }
        } break;
        case npc: {
        } break;
        case wall: GET_DATA(pWallAnim); break;
        default: ASSERT(0, GFMRV_FUNCTION_NOT_IMPLEMENTED);
    }
#undef GET_DATA
    
    // Set stats from level and type
    switch (pMob->type) {
        case player: {
            pMob->dashTime = 96;
            
            pMob->dashHorSpeed = 128;
            pMob->dashVerSpeed = 96;
            pMob->horSpeed = 48;
            pMob->verSpeed = 32;
            
            pMob->atkPower = 1;
            pMob->health = 10;
        } break;
        case npc: {
        } break;
        case shadow: {
            pMob->dashTime = 64;
            
            pMob->dashHorSpeed = 96;
            pMob->dashVerSpeed = 64;
            pMob->horSpeed = 32;
            pMob->verSpeed = 24;
            
            switch (subtype) {
                case EN_SLIME: {
                    pMob->atkPower = 0;
                    pMob->health = 2;
                } break;
                case EN_ANGRYSLIME: {
                    pMob->atkPower = 2;
                    pMob->health = 3;
                } break;
                case EN_SWARMSLIME: {
                    pMob->atkPower = 1;
                    pMob->health = 1;
                } break;
                default: ASSERT(0, GFMRV_FUNCTION_NOT_IMPLEMENTED);
            }
        } break;
        case wall: {
            pMob->health = 2;
        } break;
        default: ASSERT(0, GFMRV_INTERNAL_ERROR);
    }
    
    pMob->atkPower = pMob->atkPower + pMob->atkPower * (pMob->level - 1) * 0.5;
    pMob->health = pMob->health + pMob->health * (pMob->level - 1) * 0.5;
    
    rv = gfmSprite_addAnimations(pMob->pSelf, pData, len);
__ret:
    return rv;
}

gfmRV mob_setPosition(mob *pMob, int x, int y) {
    gfmRV rv;
    int offX, offY;
    
    // Convert from world->weird space?
    rv = gfmSprite_getOffset(&offX, &offY, pMob->pSelf);
    ASSERT(rv == GFMRV_OK, rv);
    
    rv = gfmSprite_setPosition(pMob->pSelf, x, y);
__ret:
    return rv;
}

gfmRV mob_setTraits(mob *pMob, int traits) {
    pMob->traits = traits;
    return GFMRV_OK;
}

gfmRV mob_setDist(mob *pMob, int dist) {
    pMob->dist = dist * dist;
    return GFMRV_OK;
}

gfmRV mob_update(mob *pMob, gameCtx *pGame) {
    double vx, vy;
    gfmRV rv;
    int doAttack, move;
    
    if (!pMob->isAlive) {
        rv = GFMRV_OK;
        goto __ret;
    }
    
    doAttack = 0;
    
    //==========================================================================
    // Check type and 'advance the AI'
    //
    move = MOVE_STAND;
    switch (pMob->type) {
        case player: {
            // Set player's horizontal movement
            if (pGame->num_right >= 2 &&
                    (pGame->state_right & gfmInput_pressed)) {
                move = MOVE_DASH_RIGHT;
            }
            else if (pGame->num_left >= 2 &&
                    (pGame->state_left & gfmInput_pressed)) {
                move = MOVE_DASH_LEFT;
            }
            else if (pGame->state_right & gfmInput_pressed) {
                move = MOVE_RIGHT;
            }
            else if (pGame->state_left & gfmInput_pressed) {
                move = MOVE_LEFT;
            }
            // Set player's horizontal movement
            if (pGame->num_up >= 2 &&
                    (pGame->state_up & gfmInput_pressed)) {
                move |= MOVE_DASH_UP;
            }
            else if (pGame->num_down >= 2 &&
                    (pGame->state_down & gfmInput_pressed)) {
                move |= MOVE_DASH_DOWN;
            }
            else if (pGame->state_up & gfmInput_pressed) {
                move |= MOVE_UP;
            }
            else if (pGame->state_down & gfmInput_pressed) {
                move |= MOVE_DOWN;
            }
            // Set player's attack
            if ((pGame->state_atk & gfmInput_justPressed) ==
                    gfmInput_justPressed) {
                doAttack = 1;
            }
        } break; // player
        case shadow: {
            if (((pMob->traits & TR_SWARMER) && pMob->nearbyShadowCount >= 3) ||
                    (pMob->traits & TR_ANGRY)) {
                int dist;
                
                rv = mob_getDist(&dist, pMob, pMob->plLastPosX,
                        pMob->plLastPosY);
                ASSERT(rv == GFMRV_OK, rv);
                
                if (pMob->distY > -4 && pMob->distY < 4 && (
                        (pMob->distX < -8 && pMob->distX > -20) ||
                        (pMob->distX > 8 && pMob->distX < 20))) {
                    // Attack if the player is close
                    doAttack = 1;
                }
                if (!doAttack && dist <= pMob->dist) {
                    if (pMob->distX < -2) {
                        move |= MOVE_RIGHT;
                    }
                    else if (pMob->distX > 2) {
                        move |= MOVE_LEFT;
                    }
                    if (pMob->distY < -2) {
                        move |= MOVE_UP;
                    }
                    else if (pMob->distY > 2) {
                        move |= MOVE_DOWN;
                    }
                } // if dist
            } // if angry
            else if (((pMob->traits & TR_SWARMER) &&
                    pMob->nearbyShadowCount < 3) || (pMob->traits & TR_COWARD)) {
                int dist;
                
                rv = mob_getDist(&dist, pMob, pMob->plLastPosX,
                        pMob->plLastPosY);
                ASSERT(rv == GFMRV_OK, rv);
                
                if (dist <= pMob->dist) {
                    if (pMob->distX < -2) {
                        move |= MOVE_LEFT;
                    }
                    else if (pMob->distX > 2) {
                        move |= MOVE_RIGHT;
                    }
                    if (pMob->distY < -2) {
                        move |= MOVE_DOWN;
                    }
                    else if (pMob->distY > 2) {
                        move |= MOVE_UP;
                    }
                } // if dist
            } // if coward
        } break; // shadow
        case npc: {
        } break; // npc
        case wall: {
            move = 0;
        } break; // wall
        default: ASSERT(0, GFMRV_INTERNAL_ERROR);
    }
    //= ( AI ) =================================================================
    
    if (pMob->curDashTimer <= 0) {
        if (move & MOVE_DASH_LEFT) {
            vx = -pMob->dashHorSpeed;
            // Set dash timer
            if (!(pMob->lastMove & MOVE_DASH_LEFT)) {
                pMob->curDashTimer += pMob->dashTime;
            }
        }
        else if (move & MOVE_DASH_RIGHT) {
            vx = pMob->dashHorSpeed;
            // Set dash timer
            if (!(pMob->lastMove & MOVE_DASH_RIGHT)) {
                pMob->curDashTimer += pMob->dashTime;
            }
        }
        else if (move & MOVE_LEFT) {
            vx = -pMob->horSpeed;
        }
        else if (move & MOVE_RIGHT) {
            vx = pMob->horSpeed;
        }
        else {
            vx = 0;
        }
        
        if (move & MOVE_DASH_UP) {
            vy = -pMob->dashVerSpeed;
            // Set dash timer
            if (!(pMob->lastMove & MOVE_DASH_UP)) {
                pMob->curDashTimer += pMob->dashTime;
            }
        }
        else if (move & MOVE_DASH_DOWN) {
            vy = pMob->dashVerSpeed;
            // Set dash timer
            if (!(pMob->lastMove & MOVE_DASH_DOWN)) {
                pMob->curDashTimer += pMob->dashTime;
            }
        }
        else if (move & MOVE_UP) {
            vy = -pMob->verSpeed;
        }
        else if (move & MOVE_DOWN) {
            vy = pMob->verSpeed;
        }
        else {
            vy = 0;
        }
        
        if (doAttack == 0 && (!pMob->isHurt || pMob->curDashTimer > 0)) {
            if (pMob->type != player) {
                if (vx != 0) {
                    int rng;
                    
                    rng = main_getPRNG(pGame);
                    if (rng < 0) rng = -rng;
                    
                    vx += rng % 10 - 5;
                }
                if (vy != 0) {
                    int rng;
                    
                    rng = main_getPRNG(pGame);
                    if (rng < 0) rng = -rng;
                    
                    vy += rng % 10 - 5;
                }
            }
            
            // Only move if not attacking
            rv = gfmSprite_setVelocity(pMob->pSelf, vx, vy);
            ASSERT(rv == GFMRV_OK, rv);
        
            if (vx > 0) {
                rv = gfmSprite_setDirection(pMob->pSelf, 0/*isFlipped*/);
                ASSERT(rv == GFMRV_OK, rv);
            }
            else if (vx < 0) {
                rv = gfmSprite_setDirection(pMob->pSelf, 1/*isFlipped*/);
                ASSERT(rv == GFMRV_OK, rv);
            }
        }
        else if (pMob->isHurt) {
            rv = gfmSprite_setVelocity(pMob->pSelf, 0, 0);
            ASSERT(rv == GFMRV_OK, rv);
        }
    }
    // Store the last movement
    pMob->lastMove = move;
    
    // Only move if not attacking
    if (pMob->isAttacking) {
        rv = gfmSprite_setVelocity(pMob->pSelf, 0, 0);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    if (pMob->curDashTimer > 0) {
        int elapsed;
        
        rv = gfm_getElapsedTime(&elapsed, pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
        
        pMob->curDashTimer -= elapsed;
    }
    if (pMob->invulnerableTime > 0) {
        int elapsed;
        
        rv = gfm_getElapsedTime(&elapsed, pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
        
        pMob->invulnerableTime -= elapsed;
    }
    
    // Set the animation
    // Check if hurt
    if (pMob->isHurt) {
        rv = gfmSprite_didAnimationJustLoop(pMob->pSelf);
        ASSERT(rv == GFMRV_TRUE || rv == GFMRV_FALSE, rv);
        
        if (rv == GFMRV_TRUE) {
            if (pMob->health > 0) {
                pMob->isHurt = 0;
            }
            else {
                // Kill it!
                pMob->isAlive = 0;
                
                rv = gfmSprite_setVelocity(pMob->pSelf, 0, 0);
                ASSERT(rv == GFMRV_OK, rv);
            }
        }
        // Set to OK, otherwise the ASSERT fail
        rv = GFMRV_OK;
    }
    // Check if attacking
    else if (doAttack || pMob->isAttacking) {
        if (!pMob->isAttacking) {
            pMob->isAttacking = 1;
            rv = gfmSprite_playAnimation(pMob->pSelf, ANIM_ATK);
        }
    }
    else if (pMob->lastMove & MOVE_WALK) {
        rv = gfmSprite_playAnimation(pMob->pSelf, ANIM_WALK);
    }
    else if (pMob->lastMove & MOVE_DASH) {
        rv = gfmSprite_playAnimation(pMob->pSelf, ANIM_DASH);
    }
    else {
        rv = gfmSprite_playAnimation(pMob->pSelf, ANIM_STAND);
    }
    ASSERT(rv == GFMRV_OK, rv);
    
    // Reset those, so they can be fully calculated until next frame
    pMob->nearbyShadowCount = 0;
    
    rv = GFMRV_OK;
__ret:
    return rv;
}
    
gfmRV mob_postUpdate(mob *pMob, gameCtx *pGame) {
    gfmRV rv;
    int h, w, x, y;
    
    if (!pMob->isAlive) {
        rv = GFMRV_OK;
        goto __ret;
    }
    
    // Get the current position
    rv = gfmSprite_getPosition(&x, &y, pMob->pSelf);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_getDimensions(&w, &h, pMob->pSelf);
    ASSERT(rv == GFMRV_OK, rv);
    // Adjust to the sprite's center
    x += w / 2;
    y += h / 2;
    
    // If attacking, position the attack hitbox accordingly
    if (pMob->isAttacking) {
        rv = gfmSprite_didAnimationJustLoop(pMob->pSelf);
        ASSERT(rv == GFMRV_TRUE || rv == GFMRV_FALSE, rv);

        if (rv == GFMRV_TRUE) {
            pMob->isAttacking = 0;
            rv = gfmObject_setPosition(pMob->pAtk, NEG_INF, NEG_INF);
            ASSERT(rv == GFMRV_OK, rv);
        }
        else {
            int isFlipped;

            rv = gfmSprite_getDirection(&isFlipped, pMob->pSelf);
            ASSERT(rv == GFMRV_OK, rv);

            if (isFlipped) {
                rv = gfmObject_setPosition(pMob->pAtk, x - w / 2 - 8,
                        y - h / 2);
            }
            else {
                rv = gfmObject_setPosition(pMob->pAtk, x + w / 2,
                        y - h / 2);
            }
            ASSERT(rv == GFMRV_OK, rv);
        }
    } // If is attacking (set hitbox)
    
    // Set the scan position
    if (pMob->type != wall) {
        rv = gfmObject_getDimensions(&w, &h, pMob->pScan);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmObject_setPosition(pMob->pScan, x - w / 2, y - h / 2);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    // Add it to the quadtree
    if (pMob->type != wall) {
        rv = collide_obj(pMob->pScan, pGame);
        ASSERT(rv == GFMRV_OK, rv);
        rv = collide_obj(pMob->pAtk, pGame);
        ASSERT(rv == GFMRV_OK, rv);
    }
    rv = collide_spr(pMob->pSelf, pGame);
    ASSERT(rv == GFMRV_OK, rv);
    
    // If it's the player, center the camera on it
    if (pMob->type == player) {
        gfmCamera *pCam;
        
        rv = gfm_getCamera(&pCam, pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmCamera_centerAtPoint(pCam, x, y);
        ASSERT(rv == GFMRV_CAMERA_MOVED || rv ==GFMRV_CAMERA_DIDNT_MOVE, rv);
    }
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV mob_draw(mob *pMob, gameCtx *pGame) {
    return gfmSprite_draw(pMob->pSelf, pGame->pCtx);
}

gfmRV mob_isVulnerable(mob *pMob) {
    if (pMob->invulnerableTime <= 0 && pMob->curDashTimer <= 0) {
        return GFMRV_TRUE;
    }
    return GFMRV_FALSE;
}

gfmRV mob_getType(int *pType, mob *pMob) {
    *pType = pMob->type;
    
    return GFMRV_OK;
}

gfmRV mob_setOnView(mob *pSelf, mob *pMob) {
    gfmRV rv;
    
    rv = GFMRV_OK;
    if (pMob->type == shadow) {
        pSelf->nearbyShadowCount++;
    }
    else if (pMob->type == player) {
        int w, h;
        
        rv = gfmSprite_getPosition(&(pSelf->plLastPosX), &(pSelf->plLastPosY),
                pMob->pSelf);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmSprite_getDimensions(&w, &h, pMob->pSelf);
        ASSERT(rv == GFMRV_OK, rv);
        pSelf->plLastPosX += w / 2;
        pSelf->plLastPosY += h / 2;
    }
    
__ret:
    return rv;
}

/** pSelf attacks pMob */
gfmRV mob_attack(mob *pSelf, mob *pMob, gameCtx *pGame) {
    gfmRV rv;
    
    if (mob_isVulnerable(pMob) == GFMRV_TRUE) {
        pMob->health -= pSelf->atkPower;
        pMob->isHurt = 1;
        
        pMob->invulnerableTime = 500;
        
        if (pMob->health > 0) {
            rv = gfmSprite_playAnimation(pMob->pSelf, ANIM_HIT);
            ASSERT(rv == GFMRV_OK, rv);
            
            rv = gfmSprite_setVelocity(pMob->pSelf, 0, 0);
            ASSERT(rv == GFMRV_OK, rv);
            
            if (pMob->type == wall) {
                rv = gfm_playAudio(0, pGame->pCtx, pGame->wall_hit, 0.6);
                ASSERT(rv == GFMRV_OK, rv);
            }
            else if (pMob->type == shadow) {
                rv = gfm_playAudio(0, pGame->pCtx, pGame->slime_hit, 0.6);
                ASSERT(rv == GFMRV_OK, rv);
            }
            else if (pMob->type == player) {
                rv = gfm_playAudio(0, pGame->pCtx, pGame->pl_hit, 0.6);
                ASSERT(rv == GFMRV_OK, rv);
            }
        }
        else {
            rv = gfmSprite_playAnimation(pMob->pSelf, ANIM_DEATH);
            ASSERT(rv == GFMRV_OK, rv);
            
            if (pMob->type == wall) {
                rv = gfm_playAudio(0, pGame->pCtx, pGame->expl, 0.6);
                ASSERT(rv == GFMRV_OK, rv);
            }
            else if (pMob->type == shadow) {
                rv = gfm_playAudio(0, pGame->pCtx, pGame->slime_death, 0.6);
                ASSERT(rv == GFMRV_OK, rv);
            }
            else if (pMob->type == player) {
                rv = gfm_playAudio(0, pGame->pCtx, pGame->pl_death, 0.6);
                ASSERT(rv == GFMRV_OK, rv);
            }
        }
    }
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV mob_isAlive(mob *pMob) {
    if (pMob->isAlive) {
        return GFMRV_TRUE;
    }
    return GFMRV_FALSE;
}

