#include "projects/automated_warehouse/actions.h"

struct robot_action wait;
struct robot_action up;
struct robot_action down;
struct robot_action left;
struct robot_action right;

struct robot_action* new(struct robot_action action){
        struct robot_action* new = malloc(sizeof(struct robot_action));
        new->action = action.action;
        return new;
}

void construct_loading_actions(struct list* actions, int target){
        switch(target){
                case 1:
                        list_push_back(actions, &(new(up)->elem));
                        list_push_back(actions, &(new(up)->elem));
                        list_push_back(actions, &(new(left)->elem));
                        list_push_back(actions, &(new(left)->elem));
                        list_push_back(actions, &(new(left)->elem));
                        list_push_back(actions, &(new(left)->elem));
                        list_push_back(actions, &(new(up)->elem));
                        list_push_back(actions, &(new(up)->elem));
                        return;
                case 2:
                        list_push_back(actions, &(new(up)->elem));
                        list_push_back(actions, &(new(up)->elem));
                        list_push_back(actions, &(new(left)->elem));
                        list_push_back(actions, &(new(left)->elem));
                        list_push_back(actions, &(new(up)->elem));
                        list_push_back(actions, &(new(up)->elem));
                        return;
                case 3:
                        list_push_back(actions, &(new(up)->elem));
                        list_push_back(actions, &(new(up)->elem));
                        list_push_back(actions, &(new(left)->elem));
                        list_push_back(actions, &(new(up)->elem));
                        list_push_back(actions, &(new(up)->elem));
                        return;
                case 4:
                        list_push_back(actions, &(new(up)->elem));
                        list_push_back(actions, &(new(up)->elem));
                        list_push_back(actions, &(new(up)->elem));
                        list_push_back(actions, &(new(up)->elem));
                        return;
                case 5:
                        list_push_back(actions, &(new(up)->elem));
                        list_push_back(actions, &(new(up)->elem));
                        list_push_back(actions, &(new(left)->elem));
                        list_push_back(actions, &(new(left)->elem));
                        list_push_back(actions, &(new(left)->elem));
                        list_push_back(actions, &(new(left)->elem));
                        list_push_back(actions, &(new(down)->elem));
                        return;
                case 6:
                        list_push_back(actions, &(new(up)->elem));
                        list_push_back(actions, &(new(up)->elem));
                        list_push_back(actions, &(new(left)->elem));
                        list_push_back(actions, &(new(left)->elem));
                        list_push_back(actions, &(new(down)->elem));
                        return;
                case 7:
                        list_push_back(actions, &(new(up)->elem));
                        list_push_back(actions, &(new(left)->elem));
                        return;
                default:
                        printf("\nERROR ERROR ERROR ERROR - that payload %d is not exist. - ERROR ERROR ERROR ERROR\n\n", target);
                        return;
        }
}

void initialize_actions(){
    wait.action = WAIT;
    up.action = UP;
    down.action = DOWN;
    left.action = LEFT;
    right.action = RIGHT;
}

int can_robot_go(int num, int row, int col, int target, char dest, int preempted[][2], int ignore_start){
        if(map_draw_default[row][col] == dest) return 1;
        for(int i=0;i<num;i++){
                if(row == preempted[i][ROW] && col == preempted[i][COL]) return 0;
        }

        if((map_draw_default[row][col] != ' ' &&
            map_draw_default[row][col] != (ignore_start ? ' ' : 'S') &&
            map_draw_default[row][col] != (target + '0'))
        ) {
                // printf("(%d, %d): Error occured cause %c\n",row, col, map_draw_default[row][col]);
                return 0;
        }
        
        return 1;
}