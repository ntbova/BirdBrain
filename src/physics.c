//
//  physics.c
//  BirdBrain
//
//  Created by Nick Bova on 4/26/23.
//
#include <math.h>

#include "physics.h"
#include "inits.h"

void incrementLevel(GameState* state) {
    state->curr_level += 1;
    if (state->enemy_speed_x > 0) {
        state->enemy_speed_x += ENEMY_SPEED_INCREMENT;
    }
    else {
        state->enemy_speed_x -= ENEMY_SPEED_INCREMENT;
    }
    state->enemy_speed_y += ENEMY_SPEED_INCREMENT;
    state->curr_score_multiplier += SCORE_INCREMENT;
    
    resetEnemyPosition(state);
}

int check_collision(PDRect a, PDRect b) {
    //The sides of the rectangles
    int leftA, leftB, rightA, rightB;
    int topA, topB, bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = a.x; rightA = a.x + a.width;
    topA = a.y; bottomA = a.y + a.height;

    //Calculate the sides of rect B
    leftB = b.x; rightB = b.x + b.width;
    topB = b.y; bottomB = b.y + b.height;
    //If any of the sides from A are outside of B
    if( bottomA <= topB ) { return 0; }
    if( topA >= bottomB ) { return 0; }
    if( rightA <= leftB ) { return 0; }
    if( leftA >= rightB ) { return 0; }

    //If none of the sides from A are outside B
    return 1;
}

void shootEnemyBullets(GameState* state, int bulletPosX, int bulletPosY) {
    for (int i = 0; i < BULLET_MAX; i++) {
        if (state->enemy_bullet_pos_y[i] == INT32_MIN) {
            state->enemy_bullet_pos_x[i] = bulletPosX;
            state->enemy_bullet_pos_y[i] = bulletPosY;
            // Play enemy firing synth after firing bullet
            state->pd->sound->synth->playNote(state->enemy_fire_synth, 220, 1, 0.1, 0);
            break;
        }
    }
}

void shootBullets(GameState* state, float playerRotSin, float playerRotCos) {
    // Only shoot bullets if we haven't reached the max. Will do this by simply
    // checking to see if any bullets are set to INT32_MIN
    for (int i = 0; i < BULLET_MAX; i++) {
        if (state->bullet_screen_pos_y[i] == INT32_MIN) {
            // Set the bullet at and above where the player is
            state->bullet_rots[i] = state->player_rot;
            state->bullet_rots_sin[i] = playerRotSin;
            state->bullet_rots_cos[i] = playerRotCos;
            state->bullet_screen_pos_x[i] = state->player_screen_pos_x + (((PLAYER_WIDTH / 2.0f) * BULLET_SPEED *  state->bullet_rots_sin[i]) / TRIG_MAX);
            state->bullet_screen_pos_y[i] = state->player_screen_pos_y + (((PLAYER_HEIGHT / 2.0f) * BULLET_SPEED *  state->bullet_rots_cos[i]) / TRIG_MAX);
            // After firing bullet, play play firing sound effect with corrresponding synth
            state->pd->sound->synth->playNote(state->player_fire_synth, 220, 1, 0.1, 0);
            // Break out of the loop (only shoot one bullet at a time)
            break;
        }
    }
}

void moveAssets(GameState* state) {
    // Move player position if player is still within bounds
    float player_rot_sin = sinf(state->player_rot * DEGS_TO_RADS);
    float player_rot_cos = -cosf(state->player_rot * DEGS_TO_RADS);
    
    state->player_pos_x_delta = PLAYER_SPEED * player_rot_sin;
    state->player_pos_y_delta = PLAYER_SPEED * player_rot_cos;
    state->player_world_pos_x = state->player_world_pos_x - state->player_pos_x_delta;
    state->player_world_pos_y = state->player_world_pos_y - state->player_pos_y_delta;
    
    if (state->player_world_pos_x <= BG_MAX_LEFT) { state->player_world_pos_x = BG_MAX_LEFT; state->player_pos_x_delta = 0; }
    else if (state->player_world_pos_x >= BG_MAX_RIGHT) { state->player_world_pos_x = BG_MAX_RIGHT; state->player_pos_x_delta = 0; }
    
    if (state->player_world_pos_y <= BG_MAX_TOP) { state->player_world_pos_y = BG_MAX_TOP; state->player_pos_y_delta = 0; }
    else if (state->player_world_pos_y >= BG_MAX_BOTTOM) { state->player_world_pos_y = BG_MAX_BOTTOM; state->player_pos_y_delta = 0; }

    // Check if player has fired any shots, call shootBullets if true
    if (state->player_fired_shot) { shootBullets(state, -player_rot_sin, -player_rot_cos); }
    
    // Go through each bullet currently on the screen, move based on rotational value.
    // Once the bullet leaves the boundries of the screen, reset its positional
    // value to INT32_MIN
    for (int i = 0; i < BULLET_MAX; i++) {
        if (state->bullet_screen_pos_y[i] != INT32_MIN) {
            // After moving bullet, check to see if it collides with any of the enemies.
            // Remove them both in this case.
            PDRect bullet, enemy;
            bullet.x = state->bullet_screen_pos_x[i]; bullet.y = state->bullet_screen_pos_y[i];
            bullet.width = BULLET_WIDTH; bullet.height = BULLET_HEIGHT;
            enemy.width = ENEMY_WIDTH; enemy.height = ENEMY_HEIGHT;
            for (int j = 0; j < ENEMY_MAX; j++) {
                if (state->enemy_screen_pos_y[j] != INT32_MIN) {
                    enemy.x = state->enemy_screen_pos_x[j]; enemy.y = state->enemy_screen_pos_y[j];
                    if (check_collision(bullet, enemy)) {
                        state->bullet_screen_pos_x[i] = INT32_MIN; state->bullet_screen_pos_y[i] = INT32_MIN;
                        state->enemy_screen_pos_x[j] = INT32_MIN; state->enemy_screen_pos_y[j] = INT32_MIN;
                        state->enemy_world_pos_x[j] = INT32_MIN; state->enemy_world_pos_y[j] = INT32_MIN;
                        // Increment score after hit
                        state->curr_score += state->curr_score_multiplier;
                    }
                }
            }
        }
        if (state->bullet_screen_pos_y[i] < 0 || state->bullet_screen_pos_y[i] > MAX_HEIGHT ||
            state->bullet_screen_pos_x[i] < 0 || state->bullet_screen_pos_x[i] > MAX_WIDTH ) {
            state->bullet_screen_pos_x[i] = INT32_MIN; state->bullet_screen_pos_y[i] = INT32_MIN;
        }
        // Do the same for enemy bullets
        if (state->enemy_bullet_pos_y[i] != INT32_MIN) {
            state->enemy_bullet_pos_y[i] += ENEMY_BULLET_SPEED;
            
            if (state->enemy_bullet_pos_y[i] > MAX_HEIGHT) {
                state->enemy_bullet_pos_x[i] = INT32_MIN;
                state->enemy_bullet_pos_y[i] = INT32_MIN;
            }
            else {
                PDRect bullet, player;
                bullet.x = state->enemy_bullet_pos_x[i]; bullet.y = state->enemy_bullet_pos_y[i];
                bullet.width = BULLET_WIDTH; bullet.height = BULLET_HEIGHT;
                player.width = PLAYER_WIDTH; player.height = PLAYER_HEIGHT;
                if (check_collision(player, bullet)) {
                    // Remove bullet after collision. Check to see if game over state has been reached
                    state->enemy_bullet_pos_x[i] = INT32_MIN; state->enemy_bullet_pos_y[i] = INT32_MIN;
                    checkInitGameOver(state); break;
                }
            }
        }
    }
    
    // Keep track of the number of enemies still onscreen.
    int curr_num_enemies = 0;
    for (int i = 0; i < ENEMY_MAX; i++) {
        if (state->enemy_world_pos_y[i] != INT32_MIN) {
            curr_num_enemies += 1;
        }
    }
    //Increment the level and reset if there are no more enemies left
    if (curr_num_enemies == 0) {
        incrementLevel(state);
    }
    else {
    }
}
