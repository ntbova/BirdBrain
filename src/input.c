//
//  input.c
//  BirdBrain
//
//  Created by Nick Bova on 4/25/23.
//

#include "input.h"
#include "consts.h"
#include "pd_api.h"

void checkButtons(GameState* state) {
    PDButtons current;
    PDButtons pushed;
    
    state->pd->system->getButtonState(&current, &pushed, NULL);
    
    if (current & kButtonRight) {
        state->player_bird->e_rotation += PLAYER_SPEED;
    }
    else if (current & kButtonLeft) {
        state->player_bird->e_rotation -= PLAYER_SPEED;
    }
    
    if (pushed & kButtonA || pushed & kButtonB || pushed & kButtonUp) { state->player_fired_shot = 1; }
    else { state->player_fired_shot = 0; }
}

void checkCrank(GameState* state) {
    float change = state->pd->system->getCrankChange();
    state->player_bird->e_rotation = state->player_bird->e_rotation + (change * PLAYER_CRANK_SPEED);
}
