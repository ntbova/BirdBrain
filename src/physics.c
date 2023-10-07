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
        if (state->enemy_bullets[i]->e_screen_pos_y == INT32_MIN) {
            state->enemy_bullets[i]->e_screen_pos_x = bulletPosX;
            state->enemy_bullets[i]->e_screen_pos_y = bulletPosY;
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
        if (state->player_bullets[i]->e_screen_pos_y == INT32_MIN) {
            // Set the bullet at and above where the player is
            state->player_bullets[i]->e_rotation = state->player_bird->e_rotation;
            state->player_bullets[i]->e_rotation_sin = playerRotSin;
            state->player_bullets[i]->e_rotation_cos = playerRotCos;
            state->player_bullets[i]->e_screen_pos_x = state->player_bird->e_screen_pos_x + (((PLAYER_WIDTH / 2.0f) * BULLET_SPEED *  state->player_bullets[i]->e_rotation_sin) / TRIG_MAX);
            state->player_bullets[i]->e_screen_pos_y = state->player_bird->e_screen_pos_y + (((PLAYER_HEIGHT / 2.0f) * BULLET_SPEED *  state->player_bullets[i]->e_rotation_cos) / TRIG_MAX);
            // After firing bullet, play play firing sound effect with corrresponding synth
            state->pd->sound->synth->playNote(state->player_fire_synth, 220, 1, 0.1, 0);
            // Break out of the loop (only shoot one bullet at a time)
            break;
        }
    }
}

void moveAssets(GameState* state) {
    // Move player position if player is still within bounds
    float player_rot_sin = sinf(state->player_bird->e_rotation * DEGS_TO_RADS);
    float player_rot_cos = -cosf(state->player_bird->e_rotation * DEGS_TO_RADS);
    
    state->player_bird->e_pos_delta_x = PLAYER_SPEED * player_rot_sin;
    state->player_bird->e_pos_delta_y = PLAYER_SPEED * player_rot_cos;
    state->player_bird->e_world_pos_x = state->player_bird->e_world_pos_x - state->player_bird->e_pos_delta_x;
    state->player_bird->e_world_pos_y = state->player_bird->e_world_pos_y - state->player_bird->e_pos_delta_y;
    
    if (state->player_bird->e_world_pos_x <= BG_MAX_LEFT) { state->player_bird->e_world_pos_x = BG_MAX_LEFT; state->player_bird->e_pos_delta_x = 0; }
    else if (state->player_bird->e_world_pos_x >= BG_MAX_RIGHT) { state->player_bird->e_world_pos_x = BG_MAX_RIGHT; state->player_bird->e_pos_delta_x = 0; }
    
    if (state->player_bird->e_world_pos_y <= BG_MAX_TOP) { state->player_bird->e_world_pos_y = BG_MAX_TOP; state->player_bird->e_pos_delta_y = 0; }
    else if (state->player_bird->e_world_pos_y >= BG_MAX_BOTTOM) { state->player_bird->e_world_pos_y = BG_MAX_BOTTOM; state->player_bird->e_pos_delta_y = 0; }

    // Check if player has fired any shots, call shootBullets if true
    if (state->player_fired_shot) { shootBullets(state, -player_rot_sin, -player_rot_cos); }
    
    // Go through each bullet currently on the screen, move based on rotational value.
    // Once the bullet leaves the boundries of the screen, reset its positional
    // value to INT32_MIN
    for (int i = 0; i < BULLET_MAX; i++) {
        if (state->player_bullets[i]->e_screen_pos_y != INT32_MIN) {
            // After moving bullet, check to see if it collides with any of the enemies.
            // Remove them both in this case.
            PDRect bullet, enemy;
            bullet.x = state->player_bullets[i]->e_screen_pos_x; bullet.y = state->player_bullets[i]->e_screen_pos_y;
            bullet.width = BULLET_WIDTH; bullet.height = BULLET_HEIGHT;
            enemy.width = ENEMY_WIDTH; enemy.height = ENEMY_HEIGHT;
            for (int j = 0; j < ENEMY_MAX; j++) {
                if (state->enemy_birds[j]->e_screen_pos_y != INT32_MIN) {
                    enemy.x = state->enemy_birds[j]->e_screen_pos_x; enemy.y = state->enemy_birds[j]->e_screen_pos_y;
                    // Check if player's bullet has collided with enemy
                    if (check_collision(bullet, enemy)) {
                        state->player_bullets[i]->e_screen_pos_x = INT32_MIN; state->player_bullets[i]->e_screen_pos_y = INT32_MIN;
                        state->enemy_birds[j]->e_screen_pos_x = INT32_MIN; state->enemy_birds[j]->e_screen_pos_y = INT32_MIN;
                        state->enemy_birds[j]->e_world_pos_x = INT32_MIN; state->enemy_birds[j]->e_world_pos_y = INT32_MIN;
                        // Increment score after hit
                        state->curr_score += state->curr_score_multiplier;
                    }
                }
            }
        }
        if (state->player_bullets[i]->e_screen_pos_y < 0 || state->player_bullets[i]->e_screen_pos_y > MAX_HEIGHT ||
            state->player_bullets[i]->e_screen_pos_x < 0 || state->player_bullets[i]->e_screen_pos_x > MAX_WIDTH ) {
            state->player_bullets[i]->e_screen_pos_x = INT32_MIN; state->player_bullets[i]->e_screen_pos_y = INT32_MIN;
        }
        // Do the same for enemy bullets
        if (state->enemy_bullets[i]->e_screen_pos_y != INT32_MIN) {
            state->enemy_bullets[i]->e_screen_pos_y += ENEMY_BULLET_SPEED;
            
            if (state->enemy_bullets[i]->e_screen_pos_y > MAX_HEIGHT) {
                state->enemy_bullets[i]->e_screen_pos_x = INT32_MIN;
                state->enemy_bullets[i]->e_screen_pos_y = INT32_MIN;
            }
            else {
                PDRect bullet, player;
                bullet.x = state->enemy_bullets[i]->e_screen_pos_x; bullet.y = state->enemy_bullets[i]->e_screen_pos_y;
                bullet.width = BULLET_WIDTH; bullet.height = BULLET_HEIGHT;
                player.width = PLAYER_WIDTH; player.height = PLAYER_HEIGHT;
                if (check_collision(player, bullet)) {
                    // Remove bullet after collision. Check to see if game over state has been reached
                    state->enemy_bullets[i]->e_screen_pos_x = INT32_MIN; state->enemy_bullets[i]->e_screen_pos_y = INT32_MIN;
                    checkInitGameOver(state); break;
                }
            }
        }
    }
    
    // Keep track of the number of enemies still onscreen.
    int curr_num_enemies = 0;
    for (int i = 0; i < ENEMY_MAX; i++) {
        if (state->enemy_birds[i]->e_world_pos_y != INT32_MIN) {
            curr_num_enemies += 1;
        }
    }
    //Increment the level and reset if there are no more enemies left
    if (curr_num_enemies == 0) {
        incrementLevel(state);
    }
    else {
        PDRect player, enemy;
        player.x = state->player_bird->e_screen_pos_x; player.y = state->player_bird->e_screen_pos_y;
        player.width = PLAYER_WIDTH; player.height = PLAYER_HEIGHT;
        enemy.width = ENEMY_WIDTH; enemy.height = ENEMY_HEIGHT;
        for (int j = 0; j < ENEMY_MAX; j++) {
            // Check if enemy has collided with player
            if (state->enemy_birds[j]->e_screen_pos_y != INT32_MIN) {
                enemy.x = state->enemy_birds[j]->e_screen_pos_x; enemy.y = state->enemy_birds[j]->e_screen_pos_y;
                if (check_collision(enemy, player)) {
                    checkInitGameOver(state); break;
                }
            }
        }
    }
}
