//
//  gfx.c
//  BirdBrain
//
//  Created by Nick Bova on 4/25/23.
//

#include "gfx.h"
#include "consts.h"
#include "helpers.h"

void renderAssets(GameState* state) {
    // Don't render assets if we've switched to a game over or other phase
    if (state->curr_phase != pGameRunning) { return; }
    
    // Rotate the player based on current rotation value
    LCDBitmap* rotatedBird = state->pd->graphics->rotatedBitmap(state->player_bird_bitmap, state->player_rot, 1, 1, NULL);
    state->pd->sprite->setImage(state->player_bird_sprite, rotatedBird, kBitmapUnflipped);
    state->pd->sprite->drawSprites();
    state->pd->system->logToConsole("%f", state->player_rot);
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
