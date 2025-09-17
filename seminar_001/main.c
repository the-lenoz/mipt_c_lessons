#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct _rule 
{
    char readed_character;
    char state[64];
    char char_to_write;
    char next_state[64];
    char move_direction;
    struct _rule* next;
} rule;

int read_machine(rule* machine, FILE* fp, char** init_state);
int interpretate_machine(rule* machine, char** tape);

char* turing_tape_position_to_address(char** tape, int position, int* tape_capacity);


int main(void)
{
    rule* machine = NULL;

    char init[64] = {0};
    char final[64] = {0};

    char* tape = calloc(64, sizeof(char));


    printf("Hello, world!\n");
    return 0;
}

int read_machine(rule* machine, FILE* fp, char** init_state)
{
    assert(fp != NULL);

    rule current_rule = {};
    rule* new_rule = NULL;

    int empty = 1;

    while (fscanf(fp, "<%63[^,:]:'%c'=>%63[^:]:'%c'%c ", 
        current_rule.state, &current_rule.readed_character, current_rule.next_state, &current_rule.char_to_write, &current_rule.move_direction) == 5)
    {
        current_rule.move_direction = (current_rule.move_direction == '<' ? -1 : 1);
        
        if (empty)
        {
            // TODO
        }

        new_rule = calloc(1, sizeof(rule));

        if (new_rule == NULL) {
            fprintf(stderr, "Error while allocation memory in read_machine\n");
            return -1;
        }

        new_rule->next = new_rule;

        *new_rule = current_rule;
        
    }
    return 0;
}

int interpretate_machine(rule* machine, char** tape)
{
    int step = 0;
    int current_tape_position = 0;
    char* current_state = NULL;


    while (1)
    {
        current_state; //TODO

        ++step;
    }

    printf("Machine stopped at step: %d, final state: %s\n", step, current_state);
}


char* turing_tape_position_to_address(char** tape, int position, int* tape_capacity)
{
    assert(tape != NULL);
    assert(*tape != NULL);
    assert(tape_capacity != NULL);


    int calculated_tape_index = 0;
    char* tmp_tape = NULL;
    if (position <= 0)
    {
        calculated_tape_index = position * -2;
    }
    else 
    {
        calculated_tape_index = position * 2 - 1;
    }

    if (calculated_tape_index >= *tape_capacity)
    {
        tmp_tape = calloc(*tape_capacity * 2, sizeof(char));
        if (tmp_tape == NULL) {
            fprintf(stderr, "Error while allocation memory in turing_tape_position_to_address\n");
            return NULL;
        }

        memccpy(tmp_tape, *tape, *tape_capacity, sizeof(char));
        free(*tape);

        *tape = tmp_tape;
    }

    return &((*tape)[calculated_tape_index]);
}