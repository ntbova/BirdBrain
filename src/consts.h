//
//  consts.h
//  BirdBrain
//
//  Created by Nick Bova on 4/25/23.
//

#ifndef consts_h
#define consts_h

#include "pd_api.h"

#define DEGS_TO_RADS 0.01745f
#define TRIG_MAX 5.0f
#define PLAYER_HEIGHT 59.0f
#define PLAYER_WIDTH 74.0f
#define BG_MIDPOINT_HEIGHT 252.0f
#define BG_MIDPOINT_WIDTH 450.0f
#define BG_MARGIN_LEFT -750.0f
#define BG_MARGIN_RIGHT 400.0f
#define BG_MAX_LEFT -950.0f
#define BG_MAX_RIGHT 600.0f
#define BG_MARGIN_TOP -450.0f
#define BG_MARGIN_BOTTOM 260.0f
#define BG_MAX_TOP -575.0f
#define BG_MAX_BOTTOM 375.0f
#define BG_SECTION_WIDTH 310.0f
#define BG_SECTION_HEIGHT 177.5f
#define BULLET_HEIGHT 5.0f
#define BULLET_WIDTH 5.0f
#define ENEMY_HEIGHT 18.0f
#define ENEMY_WIDTH 18.0f
#define BULLET_MAX 2
#define ENEMY_MAX 16
#define MAX_FRAMERATE 50
#define PLAYER_SPEED 3.0f
#define PLAYER_CRANK_SPEED 0.5f
#define BULLET_SPEED 5.0f
#define ENEMY_BULLET_SPEED 2.0f
#define MAX_HEIGHT 240.0f
#define MAX_WIDTH 400.0f
#define MIDPOINT_HEIGHT 120.0f
#define MIDPOINT_WIDTH 200.0f
#define SCREEN_MARGIN 36.0f
#define ENEMY_STARTING_SPEED 5.0f
#define ENEMY_SPEED_INCREMENT 1.0f
#define ENEMY_MARGIN_WIDTH 30.0f
#define ENEMY_SCREEN_WIDTH_MARGIN 10.0f
#define ENEMY_MARGIN_HEIGHT 30.0f
#define ENEMY_MOVEMENT_FREQ 1000.0f // ms
#define ENEMY_MOVEMENT_FREQ_OFFSET 32.0f
#define ENEMY_FIRING_INTERVAL 3000.0f // ms
#define SCORE_STARTING_MULTIPLIER 5.0f
#define SCORE_INCREMENT 1
#define TIME_DURING_GAME_OVER 5.0f
#define STARTING_LIVES 3
#define LIVES_MAX_DIGITS 1
#define SCORE_POS_X 360.0f
#define SCORE_POS_Y 3.0f
#define LEVEL_POS_X 15.0f
#define LEVEL_POS_Y 3.0f
#define LIVES_POS_X 200.0f
#define LIVES_POS_Y 3.0f

enum GamePhases {
    pMainMenu = 0,
    pGameRunning = 1,
    pGameOver = -1
};

typedef struct GameStates {
    PlaydateAPI* pd;
    LCDFont* title_font;
    LCDFont* score_font;
    PDSynth* player_fire_synth;
    PDSynth* enemy_fire_synth;
    LCDBitmap* player_bird_bitmap;
    LCDSprite* player_bird_sprite;
    LCDBitmap* bullet_bitmap;
    LCDBitmap* bullet_bitmaps_rot[BULLET_MAX];
    LCDSprite* bullet_sprites[BULLET_MAX];
    LCDBitmap* background_bitmap;
    LCDSprite* background_sprite;
    enum GamePhases curr_phase;
    uint32_t curr_score;
    uint32_t curr_score_multiplier;
    uint8_t curr_level;
    uint8_t curr_lives;
    float screen_world_pos_left;
    float screen_world_pos_right;
    float screen_world_pos_top;
    float screen_world_pos_bottom;
    float player_rot;
    float player_world_pos_x;
    float player_world_pos_y;
    float player_screen_pos_x;
    float player_screen_pos_y;
    float player_pos_x_delta;
    float player_pos_y_delta;
    uint8_t player_fired_shot;
    float bullet_screen_pos_x[BULLET_MAX];
    float bullet_screen_pos_y[BULLET_MAX];
    float bullet_rots[BULLET_MAX];
    float bullet_rots_sin[BULLET_MAX];
    float bullet_rots_cos[BULLET_MAX];
    float enemy_world_pos_x[ENEMY_MAX];
    float enemy_world_pos_y[ENEMY_MAX];
    float enemy_screen_pos_x[ENEMY_MAX];
    float enemy_screen_pos_y[ENEMY_MAX];
    int enemy_bullet_pos_x[BULLET_MAX];
    int enemy_bullet_pos_y[BULLET_MAX];
    int enemy_speed_x;
    int enemy_speed_y;
    int enemy_move_time;
    int enemy_fire_time;
} GameState;

#endif /* consts_h */
