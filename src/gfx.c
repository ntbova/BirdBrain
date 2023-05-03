//
//  gfx.c
//  BirdBrain
//
//  Created by Nick Bova on 4/25/23.
//

#include "gfx.h"
#include "consts.h"
#include "helpers.h"
#include "inits.h"

void renderAssets(GameState* state) {
    // Don't render assets if we've switched to a game over or other phase
    if (state->curr_phase != pGameRunning) { return; }
    
    // Render Background
//    state->pd->sprite->moveTo(state->background_sprite, state->player_pos_x, state->player_pos_y);
    
    // Rotate the player based on current rotation value
    LCDBitmap* rotatedBird = state->pd->graphics->rotatedBitmap(state->player_bird_bitmap, state->player_rot, 1, 1, NULL);
    state->pd->sprite->setImage(state->player_bird_sprite, rotatedBird, kBitmapUnflipped);
    
    // Move player based on current angle
    state->pd->sprite->moveBy(state->background_sprite, PLAYER_SPEED * sinf(state->player_rot * DEGS_TO_RADS), PLAYER_SPEED * -cosf(state->player_rot * DEGS_TO_RADS));
    
    // Render bullets
    for (int i = 0; i < BULLET_MAX; i++) {
        if (state->bullet_pos_x[i] != INT32_MIN) {
            if (state->bullet_sprites[i] == NULL) {
                state->bullet_bitmaps_rot[i] = state->pd->graphics->rotatedBitmap(state->bullet_bitmap, state->bullet_rots[i] + 180, 1, 1, NULL);
                state->bullet_sprites[i] = loadSpriteFromBitmap(state->pd, state->bullet_bitmaps_rot[i], kBitmapUnflipped);
                state->pd->sprite->addSprite(state->bullet_sprites[i]);
                state->pd->sprite->moveTo(state->bullet_sprites[i], state->bullet_pos_x[i], state->bullet_pos_y[i]);
            }
            state->pd->sprite->moveBy(state->bullet_sprites[i], BULLET_SPEED * state->bullet_rots_sin[i], BULLET_SPEED * state->bullet_rots_cos[i]);
            state->pd->sprite->getPosition(state->bullet_sprites[i], &state->bullet_pos_x[i], &state->bullet_pos_y[i]);
        }
        else {
            if (state->bullet_sprites[i] != NULL) {
                state->pd->sprite->removeSprite(state->bullet_sprites[i]);
                state->pd->sprite->freeSprite(state->bullet_sprites[i]);
                state->pd->graphics->freeBitmap(state->bullet_bitmaps_rot[i]);
                state->bullet_sprites[i] = NULL;
            }
        }
    }
    
    state->pd->sprite->drawSprites();
    
    // Free any images created in this block
    state->pd->graphics->freeBitmap(rotatedBird);
    
    // Render score
    // Get number of digits in the score
    int digits = num_places(state->curr_score);
    char scoreStr[digits]; get_dec_str(scoreStr, digits, state->curr_score);
    state->pd->graphics->drawText(scoreStr, digits, kASCIIEncoding, SCORE_POS_X, SCORE_POS_Y);
    // Render level
    digits = num_places(state->curr_level);
    char levelStr[digits]; get_dec_str(levelStr, digits, state->curr_level);
    state->pd->graphics->drawText(levelStr, digits, kASCIIEncoding, LEVEL_POS_X, LEVEL_POS_Y);
    // Render number of lives
    digits = LIVES_MAX_DIGITS; char livesStr[digits];
    get_dec_str(livesStr, digits, state->curr_lives);
    state->pd->graphics->drawText(livesStr, digits, kASCIIEncoding, LIVES_POS_X, LIVES_POS_Y);
}
