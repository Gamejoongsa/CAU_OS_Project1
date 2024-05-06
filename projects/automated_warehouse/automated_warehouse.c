#include <stdio.h>
#include <string.h>

#include "threads/init.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"

#include "devices/timer.h"

#include "projects/automated_warehouse/aw_manager.h"
#include "projects/automated_warehouse/aw_message.h"
#include "projects/automated_warehouse/actions.h"

struct robot* robots;

struct message_box* boxes_from_central_control_node;
struct message_box* boxes_from_robots;

struct list* robot_actions;

int abs(int a1, int a2){
        return (a1-a2)>0 ? a1-a2 : a2-a1;
}

// control code for central control node thread
void control_cnt(void* num){
        int blocked = 0;
        int completed = 0;
        int robots_num = *((int *)num);
        int preempted[robots_num][2];
        while(1){
                //wait until all threads being blocked
                while(blocked != robots_num){
                        thread_yield();
                        blocked = 0;
                        for(int i=0;i<robots_num;i++)
                        if(boxes_from_robots[i].dirtyBit == 1) blocked++;
                }
                //control each robots
                print_map(robots, robots_num);
                completed = 0;
                for(int i=0;i<robots_num;i++){
                        // receive message
                        struct message received_msg = boxes_from_robots[i].msg;
                        if(received_msg.required_payload == -1){
                                completed++;
                                continue;
                        }

                        int target = (received_msg.required_payload / 10);
                        char dest = (received_msg.required_payload % 10) + 'A';
                        int next_action = 0;

                        // if first move, load actions for moving to target payload
                        if(received_msg.current_payload == 0 && list_empty(&robot_actions[i])){
                                construct_loading_actions(&robot_actions[i], target);
                        }

                        if(received_msg.current_payload != 0){
                                int robot_row = received_msg.row;
                                int robot_col = received_msg.col;
                                //printf("Thread %d: %c\n", i, dest);
                                switch(dest){
                                        case 'A':
                                                if(robot_col == COL_A && can_robot_go(robots_num, robot_row-1, robot_col, target, dest, preempted, 1)){
                                                        next_action = UP;
                                                        preempted[i][ROW] = robot_row-1;
                                                        preempted[i][COL] = robot_col;
                                                }
                                                else if(robot_col < COL_A && can_robot_go(robots_num, robot_row, robot_col+1, target, dest, preempted, 1)){
                                                        next_action = RIGHT;
                                                        preempted[i][ROW] = robot_row;
                                                        preempted[i][COL] = robot_col+1;
                                                }
                                                else if(robot_col > COL_A && can_robot_go(robots_num, robot_row, robot_col-1, target, dest, preempted, 1)){
                                                        next_action = LEFT;
                                                        preempted[i][ROW] = robot_row;
                                                        preempted[i][COL] = robot_col-1;
                                                }
                                                else if(can_robot_go(robots_num, robot_row, robot_col+1, target, dest, preempted, 1)){
                                                        next_action = RIGHT;
                                                        preempted[i][ROW] = robot_row;
                                                        preempted[i][COL] = robot_col+1;
                                                }
                                                else if(can_robot_go(robots_num, robot_row+1, robot_col, target, dest, preempted, 1)){
                                                        next_action = DOWN;
                                                        preempted[i][ROW] = robot_row+1;
                                                        preempted[i][COL] = robot_col;
                                                }
                                                else if(can_robot_go(robots_num, robot_row-1, robot_col, target, dest, preempted, 1)){
                                                        next_action = UP;
                                                        preempted[i][ROW] = robot_row-1;
                                                        preempted[i][COL] = robot_col;
                                                }
                                                else{
                                                        next_action = WAIT;
                                                }
                                                break;
                                        case 'B':
                                                if(robot_row == ROW_B && can_robot_go(robots_num, robot_row, robot_col-1, target, dest, preempted, 1)){
                                                        next_action = LEFT;
                                                        preempted[i][ROW] = robot_row;
                                                        preempted[i][COL] = robot_col-1;
                                                }
                                                else if(robot_row < COL_B && can_robot_go(robots_num, robot_row+1, robot_col, target, dest, preempted, 1)){
                                                        next_action = DOWN;
                                                        preempted[i][ROW] = robot_row+1;
                                                        preempted[i][COL] = robot_col;
                                                }
                                                else if(robot_row > COL_B && can_robot_go(robots_num, robot_row-1, robot_col, target, dest, preempted, 1)){
                                                        next_action = UP;
                                                        preempted[i][ROW] = robot_row-1;
                                                        preempted[i][COL] = robot_col;
                                                }
                                                else if(can_robot_go(robots_num, robot_row+1, robot_col, target, dest, preempted, 1)){
                                                        next_action = DOWN;
                                                        preempted[i][ROW] = robot_row+1;
                                                        preempted[i][COL] = robot_col;
                                                }
                                                else if(can_robot_go(robots_num, robot_row, robot_col-1, target, dest, preempted, 1)){
                                                        next_action = LEFT;
                                                        preempted[i][ROW] = robot_row;
                                                        preempted[i][COL] = robot_col-1;
                                                }
                                                else if(can_robot_go(robots_num, robot_row, robot_col+1, target, dest, preempted, 1)){
                                                        next_action = RIGHT;
                                                        preempted[i][ROW] = robot_row;
                                                        preempted[i][COL] = robot_col+1;
                                                }
                                                else{
                                                        next_action = WAIT;
                                                }
                                                break;
                                        case 'C':
                                                if(robot_col == COL_C && can_robot_go(robots_num, robot_row+1, robot_col, target, dest, preempted, 1)){
                                                        next_action = DOWN;
                                                        preempted[i][ROW] = robot_row+1;
                                                        preempted[i][COL] = robot_col;
                                                }
                                                else if(robot_col > COL_C && can_robot_go(robots_num, robot_row, robot_col-1, target, dest, preempted, 1)){
                                                        next_action = LEFT;
                                                        preempted[i][ROW] = robot_row;
                                                        preempted[i][COL] = robot_col-1;
                                                }
                                                else if(robot_col < COL_C && can_robot_go(robots_num, robot_row, robot_col+1, target, dest, preempted, 1)){
                                                        next_action = RIGHT;
                                                        preempted[i][ROW] = robot_row;
                                                        preempted[i][COL] = robot_col+1;
                                                }
                                                else if(can_robot_go(robots_num, robot_row+1, robot_col, target, dest, preempted, 1)){
                                                        next_action = DOWN;
                                                        preempted[i][ROW] = robot_row+1;
                                                        preempted[i][COL] = robot_col;
                                                }
                                                else if(can_robot_go(robots_num, robot_row-1, robot_col, target, dest, preempted, 1)){
                                                        next_action = UP;
                                                        preempted[i][ROW] = robot_row-1;
                                                        preempted[i][COL] = robot_col;
                                                }
                                                else{
                                                        next_action = WAIT;
                                                }
                                                break;
                                }
                        }
                        else if(!list_empty(&robot_actions[i])){
                                // wait or move - if robot can move, move / else, wait
                                int robot_row;
                                int robot_col;
                                switch(list_entry(list_front(&robot_actions[i]), struct robot_action, elem)->action){
                                        case WAIT:
                                                // This cannot be happen.
                                                break;
                                        case UP:
                                                robot_row = received_msg.row-1;
                                                robot_col = received_msg.col;
                                                break;
                                        case DOWN:
                                                robot_row = received_msg.row+1;
                                                robot_col = received_msg.col;
                                                break;
                                        case LEFT:
                                                robot_row = received_msg.row;
                                                robot_col = received_msg.col-1;
                                                break;
                                        case RIGHT:
                                                robot_row = received_msg.row;
                                                robot_col = received_msg.col+1;
                                                break;
                                }
                                if(can_robot_go(robots_num, robot_row, robot_col, target, dest, preempted, 0)){
                                        next_action = list_entry(list_pop_front(&robot_actions[i]), struct robot_action, elem)->action;
                                        preempted[i][ROW] = robot_row;
                                        preempted[i][COL] = robot_col;
                                }
                                else{
                                        next_action = WAIT;
                                }
                        }
                        
                        // send message to robot
                        struct message* send_msg = malloc(sizeof(struct message));
                        send_msg->cmd = next_action;
                        boxes_from_central_control_node[i].msg = *send_msg;
                        boxes_from_central_control_node[i].dirtyBit = 1;
                        free(send_msg);
                }
                if(completed == robots_num) break; // escapingh points
                //increase step, and unblock all threads
                increase_step();
                unblock_threads();
                blocked = 0;
        }
        thread_exit();
        return;
}

// control code for robot thread
void control_thread(void* aux){
        int idx = *((int *)aux) - 1; //index starts at 1
        while(1){
                struct message received_msg;
                int target = robots[idx].required_payload / 10;
                char dest = (robots[idx].required_payload % 10) + 'A';
                int isTerminated = 0;
                //printf("thread %s working\n", thread_name());

                // receive message
                if(boxes_from_central_control_node[idx].dirtyBit){
                        received_msg = boxes_from_central_control_node[idx].msg;
                        boxes_from_central_control_node[idx].dirtyBit = 0;
                }
                
                // do action
                switch(received_msg.cmd){
                        case WAIT:
                                break;
                        case UP:
                                robots[idx].row--;
                                break;
                        case DOWN:
                                robots[idx].row++;
                                break;
                        case LEFT:
                                robots[idx].col--;
                                break;
                        case RIGHT:
                                robots[idx].col++;
                                break;
                        default:
                                //initial or no message -> break
                                break;
                }

                // printf("%s / %d: (%d %d)\n", thread_name(), received_msg.cmd, robots[idx].row, robots[idx].col);

                // update status
                if(map_draw_default[robots[idx].row][robots[idx].col] == target + '0'){
                        robots[idx].current_payload = robots[idx].required_payload / 10;
                }
                else if(map_draw_default[robots[idx].row][robots[idx].col] == dest && robots[idx].current_payload == target){
                        //printf("Thread %s has reached his own goal!\n", thread_name());
                        robots[idx].required_payload = -1; // It means complete
                        isTerminated = 1;
                }

                // send message
                struct message* msg = malloc(sizeof(struct message));
                msg->row = robots[idx].row;
                msg->col = robots[idx].col;
                msg->required_payload = robots[idx].required_payload;
                msg->current_payload = robots[idx].current_payload;
                boxes_from_robots[idx].msg = *msg;
                boxes_from_robots[idx].dirtyBit = 1;
                //printf("thread %s message sent!\n", thread_name());
                free(msg);

                if(isTerminated) break; // if mission completed, break the loop to exit
                block_thread();
        }
        thread_exit();
        return;
}

// entry point of simulator
void run_automated_warehouse(char **argv)
{
        init_automated_warehouse(argv); // do not remove this
        initialize_actions(); // initialize each actions

        printf("implement automated warehouse!\n");
        int robots_num = atoi(argv[1]);
        char* payload_list = argv[2];
        char* next_payload;
        char name[robots_num][200];

        char* payload = strtok_r(payload_list, ":", &next_payload);

        // creating robots
        robots = malloc(sizeof(struct robot) * robots_num);
        for(int i=0;i<robots_num;i++){
                snprintf(name[i], 200, "R%d", i+1);
                payload[1] = payload[1] - 'A' + '0'; // convert alphabetic letter to integer start at 0
                int required_payload = atoi(payload); // first digit: target goods, second digit: target location
                setRobot(&robots[i], name[i], 5, 5, required_payload, 0);
                payload = strtok_r(NULL, ":", &next_payload);
        }

        // creating message box
        boxes_from_central_control_node = malloc(sizeof(struct message_box) * robots_num);
        boxes_from_robots = malloc(sizeof(struct message_box) * robots_num);

        // creating action list
        robot_actions = malloc(sizeof(struct list) * robots_num);
        for(int i=0;i<robots_num;i++) list_init(&robot_actions[i]);

        // example of create thread
        tid_t* threads = malloc(sizeof(tid_t) * (robots_num + 1));
        int* idxs = (int*)malloc(sizeof(int) * robots_num);

        for(int i=0;i<robots_num;i++){
                idxs[i] = i+1;
        }

        threads[0] = thread_create("CNT", 0, &control_cnt, &robots_num);
        for(int i=0;i<robots_num;i++){
                threads[i+1] = thread_create(name[i], 0, &control_thread, &idxs[i]);
        }

        // if you want, you can use main thread as a central control node
}