#include "score.h"
#include <stdio.h>  // Necessary to read file

void write_scores_bin(){
    FILE *ptr;

    TYPE_SCORE scores[MAX_NUMBER_OF_PLAYERS_IN_SCORE] = {0};

    scores[0] = (TYPE_SCORE){"Jeremy", 10};
    scores[1] = (TYPE_SCORE){"Mat", 5};
    scores[2] = (TYPE_SCORE){"Jake", 2};
    scores[3] = (TYPE_SCORE){"Ba", 1};

    ptr = fopen(SCORE_FILE_NAME,"wb");  // write in binary
    if(ptr == NULL) return;

    fwrite(scores, sizeof(TYPE_SCORE), 4, ptr);
    fclose(ptr);
}

// For debugging
void print_scores(){
    FILE *ptr;
    TYPE_SCORE scores[MAX_NUMBER_OF_PLAYERS_IN_SCORE] = {0};

    ptr = fopen(SCORE_FILE_NAME,"rb");  // read in binary
    if(ptr == NULL) return;
    size_t num_of_names = 0;
    TYPE_SCORE score_buffer;
    while(fread(&score_buffer, sizeof(TYPE_SCORE), 1, ptr) == 1 && num_of_names < MAX_NUMBER_OF_PLAYERS_IN_SCORE){
        printf("%zu : %s, score: %i\n", num_of_names, score_buffer.name, score_buffer.score);
        scores[num_of_names] = score_buffer;
        num_of_names++;
    }
    printf("\n\n");
    fclose(ptr);
}

// Score_list size needs to be bigger than or equal to MAX_NUMBER_OF_PLAYER_IN_SCORE. 
// Otherwise, could occur buffer overflow!
void read_scores(TYPE_SCORE *score_list, unsigned long *num_of_names){
    FILE *ptr;

    ptr = fopen(SCORE_FILE_NAME,"rb");  // read in binary
    if(ptr == NULL) return;
    *num_of_names = 0;
    TYPE_SCORE score_buffer;
    while(fread(&score_buffer, sizeof(TYPE_SCORE), 1, ptr) == 1 && *num_of_names < MAX_NUMBER_OF_PLAYERS_IN_SCORE){
        score_list[*num_of_names] = score_buffer;
        (*num_of_names)++;
    }
    
    fclose(ptr);
}


bool should_be_included_in_list(int score){
    TYPE_SCORE scores[MAX_NUMBER_OF_PLAYERS_IN_SCORE] = {0};
    size_t num_of_names;
    read_scores(scores, &num_of_names);
    
    if(num_of_names < MAX_NUMBER_OF_PLAYERS_IN_SCORE) return true; // List is not full yet!

    int lowest_score_in_list = scores[MAX_NUMBER_OF_PLAYERS_IN_SCORE - 1].score; // List is already in decreasing order

    if(lowest_score_in_list < score) return true;
    else return false;
}

// new_name should be 20 characters long
void insert_in_list(TYPE_SCORE new_score){
    TYPE_SCORE scores[MAX_NUMBER_OF_PLAYERS_IN_SCORE] = {0};
    size_t num_of_names;
    read_scores(scores, &num_of_names);

    size_t i = 0;

    TYPE_SCORE moving_type_score = new_score;
    while(i < num_of_names + 1 && i < MAX_NUMBER_OF_PLAYERS_IN_SCORE){
        if(scores[i].score < moving_type_score.score){
            // Switch places
            TYPE_SCORE temp_score = moving_type_score;
            moving_type_score = scores[i];
            scores[i] = temp_score;
        }
        i++;
        
    }

    FILE *ptr;

    ptr = fopen(SCORE_FILE_NAME,"wb");  // write in binary
    if(ptr == NULL) return;

    if(num_of_names < MAX_NUMBER_OF_PLAYERS_IN_SCORE) fwrite(scores, sizeof(TYPE_SCORE), num_of_names + 1, ptr);
    else fwrite(scores, sizeof(TYPE_SCORE), num_of_names, ptr);
    fclose(ptr);
}