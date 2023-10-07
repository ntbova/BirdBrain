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
    
    // Rotate the player based on current rotation value
    LCDBitmap* rotatedBird = state->pd->graphics->rotatedBitmap(state->player_bird->e_bitmap, state->player_bird->e_rotation, 1, 1, NULL);
    state->pd->sprite->setImage(state->player_bird->e_sprite, rotatedBird, kBitmapUnflipped);
    
    // Move background or player based on player position
    float playerGfxDeltaX = 0.0f; float playerGfxDeltaY = 0.0f;
    float bgGfxDeltaX = 0.0f; float bgGfxDeltaY = 0.0f;
    
    if (state->player_bird->e_world_pos_x >= BG_MARGIN_RIGHT || state->player_bird->e_world_pos_x <= BG_MARGIN_LEFT) { playerGfxDeltaX = -state->player_bird->e_pos_delta_x; }
    else { bgGfxDeltaX = state->player_bird->e_pos_delta_x; state->screen_world_pos_left -= state->player_bird->e_pos_delta_x; state->screen_world_pos_right -= state->player_bird->e_pos_delta_x; }
    
    if (state->player_bird->e_world_pos_y <= BG_MARGIN_TOP || state->player_bird->e_world_pos_y >= BG_MARGIN_BOTTOM) { playerGfxDeltaY = -state->player_bird->e_pos_delta_y; }
    else { bgGfxDeltaY = state->player_bird->e_pos_delta_y; state->screen_world_pos_top -= state->player_bird->e_pos_delta_y; state->screen_world_pos_bottom -= state->player_bird->e_pos_delta_y;  }
    
    // Render enemies
    // Go through each of the enemy world positions. If the enemy world position is inside the bounds of the screen world position, render the enemy to the screen offset to the screen world position
    for (int i = 0; i < ENEMY_MAX; i++) {
        if (state->enemy_birds[i]->e_world_pos_x + ENEMY_WIDTH >= state->screen_world_pos_left && state->enemy_birds[i]->e_world_pos_x - ENEMY_WIDTH <= state->screen_world_pos_right
            && state->enemy_birds[i]->e_world_pos_y + ENEMY_HEIGHT >= state->screen_world_pos_top && state->enemy_birds[i]->e_world_pos_y - ENEMY_HEIGHT <= state->screen_world_pos_bottom)
        {
            float enemyScreenPosX = state->enemy_birds[i]->e_world_pos_x - state->screen_world_pos_left;
            float enemyScreenPosY = state->enemy_birds[i]->e_world_pos_y - state->screen_world_pos_top;
            
            if (state->enemy_birds[i]->e_sprite == NULL) {
                state->enemy_birds[i]->e_sprite = loadSpriteFromBitmap(state->pd, state->enemy_birds[i]->e_bitmap, kBitmapUnflipped);
                state->pd->sprite->addSprite(state->enemy_birds[i]->e_sprite);
            }
            
            state->pd->sprite->moveTo(state->enemy_birds[i]->e_sprite, enemyScreenPosX, enemyScreenPosY);
            state->enemy_birds[i]->e_screen_pos_x = enemyScreenPosX;
            state->enemy_birds[i]->e_screen_pos_y = enemyScreenPosY;
        }
        else {
            if (state->enemy_birds[i]->e_sprite != NULL) {
                state->pd->sprite->removeSprite(state->enemy_birds[i]->e_sprite);
                state->pd->sprite->freeSprite(state->enemy_birds[i]->e_sprite);
                state->enemy_birds[i]->e_sprite = NULL;
                state->enemy_birds[i]->e_screen_pos_x = INT32_MIN;
                state->enemy_birds[i]->e_screen_pos_y = INT32_MIN;
            }
        }
    }
    
    state->pd->sprite->moveBy(state->background_sprite, bgGfxDeltaX, bgGfxDeltaY);
    state->pd->sprite->moveBy(state->player_bird->e_sprite, playerGfxDeltaX, playerGfxDeltaY);
    
    state->pd->sprite->getPosition(state->player_bird->e_sprite, &state->player_bird->e_screen_pos_x, &state->player_bird->e_screen_pos_y);
    // Render bullets
    for (int i = 0; i < BULLET_MAX; i++) {
        if (state->player_bullets[i]->e_screen_pos_x != INT32_MIN) {
            if (state->player_bullets[i]->e_sprite == NULL) {
                state->player_bullets[i]->e_bitmap_rotated = state->pd->graphics->rotatedBitmap(state->player_bullets[i]->e_bitmap, state->player_bullets[i]->e_rotation + 180, 1, 1, NULL);
                state->player_bullets[i]->e_sprite = loadSpriteFromBitmap(state->pd, state->player_bullets[i]->e_bitmap_rotated, kBitmapUnflipped);
                state->pd->sprite->addSprite(state->player_bullets[i]->e_sprite);
                state->pd->sprite->moveTo(state->player_bullets[i]->e_sprite, state->player_bullets[i]->e_screen_pos_x, state->player_bullets[i]->e_screen_pos_y);
            }
            state->pd->sprite->moveBy(state->player_bullets[i]->e_sprite, BULLET_SPEED * state->player_bullets[i]->e_rotation_sin, BULLET_SPEED * state->player_bullets[i]->e_rotation_cos);
            state->pd->sprite->getPosition(state->player_bullets[i]->e_sprite, &state->player_bullets[i]->e_screen_pos_x, &state->player_bullets[i]->e_screen_pos_y);
        }
        else {
            if (state->player_bullets[i]->e_sprite != NULL) {
                state->pd->sprite->removeSprite(state->player_bullets[i]->e_sprite);
                state->pd->sprite->freeSprite(state->player_bullets[i]->e_sprite);
                state->pd->graphics->freeBitmap(state->player_bullets[i]->e_bitmap_rotated);
                state->player_bullets[i]->e_sprite = NULL;
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
