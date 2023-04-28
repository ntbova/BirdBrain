//
//  input.c
//  BirdBrain
//
//  Created by Nick Bova on 4/25/23.
//

#include "input.h"
#include "consts.h"
#include "pd_api.h"
#include "physics.h"

void checkButtons(GameState* state) {
    PDButtons current;
    PDButtons pushed;
    
    state->pd->system->getButtonState(&current, &pushed, NULL);
    
    if (current & kButtonRight) {
        state->player_rot += PLAYER_SPEED;
    }
    else if (current & kButtonLeft) {
        state->player_rot -= PLAYER_SPEED;
    }
    
    if (pushed & kButtonA || pushed & kButtonB || pushed & kButtonUp) { shootBullets(state); }
}

void checkCrank(GameState* state) {
    float change = state->pd->system->getCrankChange();
    state->player_rot = state->player_rot + (change * PLAYER_CRANK_SPEED);
}
