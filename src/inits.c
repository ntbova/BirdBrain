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

void initEntities(GameState* state) {
    state->player_bird = state->pd->system->realloc(NULL,sizeof(Entity));
    
    for (int i = 0; i < ENEMY_MAX; i++) {
        state->enemy_birds[i] = state->pd->system->realloc(NULL,sizeof(Entity));
    }
    
    for (int i = 0; i < BULLET_MAX; i++) {
        state->player_bullets[i] = state->pd->system->realloc(NULL,sizeof(Entity));
        state->enemy_bullets[i] = state->pd->system->realloc(NULL,sizeof(Entity));
    }
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
    state->player_bird->e_bitmap = loadBitmapFromPath(state->pd, "images/plane1");
    
    LCDBitmap* bullet = loadBitmapFromPath(state->pd, "images/bullet");
    for (int i = 0; i < BULLET_MAX; i++) {
        state->player_bullets[i]->e_bitmap = bullet;
    }
    
    state->background_bitmap = loadBitmapFromPath(state->pd, BG_TESTING_BITMAP);
    
    LCDBitmap* enemy = loadBitmapFromPath(state->pd, "images/enemy");
    for (int i = 0; i < ENEMY_MAX; i++) {
        state->enemy_birds[i]->e_bitmap = enemy;
    }
    
    state->player_bird->e_sprite = loadSpriteFromBitmap(state->pd, state->player_bird->e_bitmap, kBitmapUnflipped);
    state->background_sprite = loadSpriteFromBitmap(state->pd, state->background_bitmap, kBitmapUnflipped);
//    state->bullet_sprites = loadSpriteFromBitmap(state->pd, state->bullet_bitmap, kBitmapUnflipped);
    for (int i = 0; i < BULLET_MAX; i++) {
        state->player_bullets[i]->e_sprite = NULL;
    }
    for (int i = 0; i < ENEMY_MAX; i++) {
        state->enemy_birds[i]->e_sprite = NULL;
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
    int currEnemyPosX = BG_MARGIN_LEFT + ENEMY_MARGIN_WIDTH; int currEnemyPosY = BG_MARGIN_TOP + ENEMY_MARGIN_HEIGHT;
    state->enemy_birds[0]->e_world_pos_x = 0; state->enemy_birds[0]->e_world_pos_y = 0;
    for (int i = 1; i < ENEMY_MAX; i++) {
        // For layout of enemies on screen, start from the top of the screen with a certain margin,
        // if we reach the end of the screen, then reset the x position and start a new row
        currEnemyPosX += BG_SECTION_WIDTH;
        if (currEnemyPosX >= BG_MARGIN_RIGHT) {
            currEnemyPosX = BG_SECTION_WIDTH + ENEMY_WIDTH; currEnemyPosY += BG_SECTION_HEIGHT + ENEMY_HEIGHT;
        }
        state->enemy_birds[i]->e_world_pos_x = 0; state->enemy_birds[i]->e_world_pos_y = 0;
        state->enemy_birds[i]->e_screen_pos_x = INT32_MIN; state->enemy_birds[i]->e_screen_pos_y = INT32_MIN;
    }
}

void resetPlayerPosition(GameState* state) {
    state->player_bird->e_world_pos_x = 0; state->player_bird->e_world_pos_y = 0;
    state->player_bird->e_screen_pos_x = MIDPOINT_WIDTH; state->player_bird->e_screen_pos_y = MIDPOINT_HEIGHT;
    state->screen_world_pos_left = -MIDPOINT_WIDTH; state->screen_world_pos_right = MIDPOINT_WIDTH;
    state->screen_world_pos_top = -MIDPOINT_HEIGHT; state->screen_world_pos_bottom = MIDPOINT_HEIGHT;
    state->pd->sprite->moveTo(state->player_bird->e_sprite, MIDPOINT_WIDTH, MIDPOINT_HEIGHT);
    state->pd->sprite->moveTo(state->background_sprite, 0, 0);
    state->player_bird->e_rotation = 0;
}

void initGameRunning(GameState* state) {
    state->curr_phase = pGameRunning;
    
    state->curr_level = 1;
    state->curr_score = 0;
    state->curr_lives = STARTING_LIVES;
    state->curr_score_multiplier = SCORE_STARTING_MULTIPLIER;
    state->player_fired_shot = 0;
    
    state->pd->sprite->addSprite(state->background_sprite);
    state->pd->sprite->addSprite(state->player_bird->e_sprite);
    
    resetPlayerPosition(state);
    
    for (int i = 0; i < BULLET_MAX; i++) {
        state->player_bullets[i]->e_screen_pos_x = INT32_MIN; state->player_bullets[i]->e_screen_pos_y = INT32_MIN;
        state->enemy_birds[i]->e_world_pos_x = INT32_MIN; state->enemy_birds[i]->e_world_pos_y = INT32_MIN;
        state->enemy_birds[i]->e_screen_pos_x = INT32_MIN; state->enemy_birds[i]->e_screen_pos_y = INT32_MIN;
        state->enemy_bullets[i]->e_screen_pos_x = INT32_MIN; state->enemy_bullets[i]->e_screen_pos_y = INT32_MIN;
    }
    
    resetEnemyPosition(state);
    
    state->pd->graphics->setFont(state->score_font);
}

void initMainMenu(GameState* state) {
    state->curr_phase = pMainMenu;
}
