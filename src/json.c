//
//  json.c
//  BirdBrain
//
//  Created by Nick Bova on 10/15/23.
//

#include "json.h"

void SetLevelData(GameState* state, int level) {
    FileStat stat;
    jsmn_parser parser;
    
    int valid = state->pd->file->stat("json/level1.json", &stat);
    state->pd->system->logToConsole("About to load level1.json");
    
    if (valid == 0) {
        state->pd->system->logToConsole("level1.json found: %i", stat.isdir);
        SDFile* file = state->pd->file->open("json/level1.json", kFileRead);
        
        if (file != NULL) {
            char json[stat.size];
            state->pd->file->read(file, json, stat.size);
            state->pd->system->logToConsole(json);
            
            jsmn_init(&parser);
            const int num_tokens = jsmn_parse(&parser, json, stat.size, NULL, 0);
            state->pd->system->logToConsole("Number of tokens: %i", num_tokens);
            
            parser.pos = 0;
            jsmntok_t level_tokens[num_tokens];
            jsmn_parse(&parser, json, stat.size, level_tokens, num_tokens);
            
            jsmntok_t enemy_pos_token = level_tokens[1];
            
            if (strcmp((char*)enemy_pos_token.object, "enemypos") == 0) {
                state->pd->system->logToConsole("Found enemypos object");
                int enemy_num = 0;
                
                for (int i = 3; i < 11 && enemy_num < ENEMY_MAX; i = i + 2) {
                    jsmntok_t token_x = level_tokens[i];
                    jsmntok_t token_y = level_tokens[i + 1];
                    
                    state->enemy_birds[enemy_num]->e_world_pos_x = *(int*)token_x.object;
                    state->enemy_birds[enemy_num]->e_world_pos_y = *(int*)token_y.object;
                    
                    enemy_num++;
                }
            }
        }
        else {
            state->pd->system->logToConsole(state->pd->file->geterr());
        }
    }
}
