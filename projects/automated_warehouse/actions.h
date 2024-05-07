#ifndef _PROJECTS_PROJECT1_ACTIONS__
#define _PROJECTS_PROJECT1_ACTIONS__

#include <list.h>

#include "threads/malloc.h"
#include "projects/automated_warehouse/aw_manager.h"

#define ROW 0
#define COL 1

enum action {
        WAIT = 0,
        UP,
        DOWN,
        LEFT,
        RIGHT
};

struct robot_action {
        enum action action;
        struct list_elem elem;
};

extern struct robot_action wait;
extern struct robot_action up;
extern struct robot_action down;
extern struct robot_action left;
extern struct robot_action right;

struct robot_action* new(struct robot_action action);

void construct_loading_actions(struct list* actions, int target);

void initialize_actions(void);

int can_robot_go(int num, int row, int col, int target, char dest, int preempted[][2], int ignore_start);

#endif