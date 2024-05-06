#include "projects/automated_warehouse/aw_thread.h"
#include "projects/automated_warehouse/aw_message.h"

//
// You need to read carefully thread/synch.h and thread/synch.c
// 
// In the code, a fucntion named "sema_down" implements blocking thread and 
// makes list of blocking thread
// 
// And a function named "sema_up" implements unblocing thread using blocking list
//
// You must implement blocking list using "blocking_threads" in this code.
// Then you can also implement unblocking thread.
//


struct list blocked_threads;

/**
 * A function blocking a threads and put that in the list "blocked_threads" 
 * It must be called by robot threads
 */
void block_thread(){
    enum intr_level old_level;
    old_level = intr_disable();
    list_push_back (&blocked_threads, &thread_current()->elem);
    thread_block();
    intr_set_level(old_level);
}

/**
 * A function unblocking all blocked threads in "blocked_threads" 
 * It must be called by central control thread
 */
void unblock_threads(){
    enum intr_level old_level;
    old_level = intr_disable();
    while (!list_empty(&blocked_threads)){
        struct thread* blocked = list_entry(list_pop_front(&blocked_threads), struct thread, elem);
        //printf("unblock thread %s...\n", blocked->name);
        thread_unblock(blocked);
    }
    intr_set_level(old_level);
}