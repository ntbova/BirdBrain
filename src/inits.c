//
//  inits.c
//  BirdBrain
//
//  Created by Nick Bova on 4/25/23.
//

#include "inits.h"
#include "consts.h"


LCDBitmap* loadBitmapFromPath(PlaydateAPI* pd, char* path) {
    const char *err = NULL;
    struct LCDBitmap* bmp = pd->graphics->loadBitmap(path, &err);
    if ( err != NULL ) { pd->system->logToConsole("Error loading image: %s", err); }
    return bmp;
}

LCDSprite* loadSpriteFromBitmap(PlaydateAPI* pd, LCDBitmap* bmp, LCDBitmapFlip flip) {
    struct LCDSprite* pathSprite = pd->sprite->newSprite();
    pd->sprite->setImage(pathSprite, bmp, flip);
    return pathSprite;
}

void initSound(GameState* state) {
    state->player_fire_synth = state->pd->sound->synth->newSynth();
    state->pd->sound->synth->setWaveform(state->player_fire_synth, kWaveformTriangle);
    state->pd->sound->synth->setAttackTime(state->player_fire_synth, 0);
    state->pd->sound->synth->setDecayTime(state->player_fire_synth, 0.1);
    state->pd->sound->synth->setSustainLevel(state->player_fire_synth, 0.2);
    state->pd->sound->synth->setReleaseTime(state->player_fire_synth, 0.2);
    
    state->enemy_fire_synth = state->pd->sound->synth->newSynth();
    state->pd->sound->synth->setWaveform(state->enemy_fire_synth, kWaveformSawtooth);
    state->pd->sound->synth->setAttackTime(state->enemy_fire_synth, 0);
    state->pd->sound->synth->setDecayTime(state->enemy_fire_synth, 0.05);
    state->pd->sound->synth->setSustainLevel(state->enemy_fire_synth, 0.3);
    state->pd->sound->synth->setReleaseTime(state->enemy_fire_synth, 0.1);
}

void initGraphics(GameState* state) {
    state->player_bird_bitmap = loadBitmapFromPath(state->pd, "images/plane1");
    state->bullet_bitmap = loadBitmapFromPath(state->pd, "images/bullet");
    state->player_bird_sprite = loadSpriteFromBitmap(state->pd, state->player_bird_bitmap, kBitmapUnflipped);
//    state->bullet_sprites = loadSpriteFromBitmap(state->pd, state->bullet_bitmap, kBitmapUnflipped);
    for (int i = 0; i < BULLET_MAX; i++) {
        state->bullet_sprites[i] = NULL;
    }
}

void checkInitGameOver(GameState* state) {
    // Check to see if the player still has any lives left. Decrement
    // the lives and reset player position if there are, init game over
    // if there are no lives left
    if (state->curr_lives > 0) {
        state->curr_lives = state->curr_lives - 1;
        resetPlayerPosition(state);
    }
    else {
        state->curr_phase = pGameOver;
        state->pd->graphics->setFont(state->title_font);
        // Reset elapsed time here. After a certain amount of time has passed in
        // game over phase, then return to main menu.
        state->pd->system->resetElapsedTime();
    }
}

void resetEnemyPosition(GameState* state) {
    int currEnemyPosX = SCREEN_MARGIN; int currEnemyPosY = SCREEN_MARGIN;
    state->enemy_pos_x[0] = currEnemyPosX; state->enemy_pos_y[0] = currEnemyPosY;
    for (int i = 1; i < ENEMY_MAX; i++) {
        // For layout of enemies on screen, start from the top of the screen with a certain margin,
        // if we reach the end of the screen, then reset the x position and start a new row
        currEnemyPosX += ENEMY_MARGIN_WIDTH + ENEMY_WIDTH;
        if (currEnemyPosX >= MAX_WIDTH - SCREEN_MARGIN) {
            currEnemyPosX = SCREEN_MARGIN; currEnemyPosY += ENEMY_MARGIN_HEIGHT + ENEMY_HEIGHT;
        }
        state->enemy_pos_x[i] = currEnemyPosX; state->enemy_pos_y[i] = currEnemyPosY;
    }
}

void resetPlayerPosition(GameState* state) {
    state->pd->sprite->moveTo(state->player_bird_sprite, MIDPOINT_WIDTH, MIDPOINT_HEIGHT);
    state->player_rot = 0;
}

void initGameRunning(GameState* state) {
    state->curr_phase = pGameRunning;
    
    state->curr_level = 1;
    state->curr_score = 0;
    state->curr_lives = STARTING_LIVES;
    state->curr_score_multiplier = SCORE_STARTING_MULTIPLIER;
    state->enemy_speed_x = ENEMY_STARTING_SPEED; state->enemy_speed_y = ENEMY_STARTING_SPEED;
    
    state->pd->sprite->addSprite(state->player_bird_sprite);
    state->pd->sprite->addSprite(state->bullet_sprites);
    
    resetPlayerPosition(state);
    
    for (int i = 0; i < BULLET_MAX; i++) {
        state->bullet_pos_x[i] = INT32_MIN; state->bullet_pos_y[i] = INT32_MIN;
        state->enemy_pos_x[i] = INT32_MIN; state->enemy_pos_y[i] = INT32_MIN;
        state->enemy_bullet_pos_x[i] = INT32_MIN; state->enemy_bullet_pos_y[i] = INT32_MIN;
    }
    
    resetEnemyPosition(state);
    
    state->pd->graphics->setFont(state->score_font);
}

void initMainMenu(GameState* state) {
    state->curr_phase = pMainMenu;
}
