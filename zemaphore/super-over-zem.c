#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "zemaphore.h"


#define NUM_OF_BATSMAN 3
#define NUM_OF_OVER 1


int next_batter=2, on_strike=0, off_strike=1;
int total_ball = NUM_OF_OVER*6;
int ball_left = total_ball;
zem_t batter[NUM_OF_BATSMAN];
zem_t baller;

//mutex 
//might not need it
zem_t mlock; 

void *throw_ball(void* ptr) {
    
    for(int i = 1 ; i<=total_ball+1 ; i++) {
        zem_down(&baller);
        //zem_down(&mlock);
        if(next_batter > NUM_OF_BATSMAN ) {
            printf("The batting team is bowled out\n");
            //zem_up(&mlock);
            break;
        }
        if(i==total_ball+1){
            break;
        }
        printf("The bowler balled %dth ball of the over\n", i);
        //zem_up(&mlock);
        zem_up(&batter[on_strike]);

    }

    //waking up the batter
    for(int i = 0; i<NUM_OF_BATSMAN ; i++)zem_up(&batter[i]);
}

void *hit_the_ball(void* ptr) {
    int id = *((int*)ptr);
    
    while(1) {
        zem_down(&batter[id]);
        //zem_down(&mlock);
        if(next_batter > NUM_OF_BATSMAN || ball_left <= 0) {
            //printf("The batting team is bowled out\n");
            //zem_up(&mlock);
            break;
        }
        
        int run = rand()%8;
        
        if((run >= 0 && run<4) || run == 5) {

           if((run%2)==1) {
                int t = on_strike;
                on_strike = off_strike;
                off_strike = t;
                printf("The batter %d has taken %d run and swapped strike\n", id, run);
            }
            else {
                if(run)printf("The batter %d has taken %d run \n", id, run);
                else printf("The batter %d was unable to take a run\n", id);
            }
        }
        else if(run == 4 || run == 6) {
            printf("The batter %d has hit a **%d**\n", id, run);
        }
        else {
            // 7 run means out
            on_strike = next_batter;
            next_batter++;
            printf("The batter %d is out\n", id);
            ball_left--;
            zem_up(&baller);
            break;
        }
        //zem_up(&mlock);
        ball_left--;
        zem_up(&baller);
    }
}

int main() {

    srand(time(0));

    pthread_t baller_thr;
    pthread_t batter_thr[NUM_OF_BATSMAN];

    int *batter_id;
    batter_id = (int*)malloc(sizeof(int));

    zem_init(&mlock, 1);
    zem_init(&baller, 0);
    for(int i = 0 ; i<NUM_OF_BATSMAN ; i++) {
        zem_init(&batter[i], 0);
    }

    pthread_create(&baller_thr, NULL, throw_ball, NULL);

    for(int i = 0 ; i<NUM_OF_BATSMAN ; i++) {
        batter_id[i] = i;
        pthread_create(&batter_thr[i], NULL, hit_the_ball, &batter_id[i]);    
    }

    //signaling the bowler thread to start bowling
    zem_up(&baller);

    pthread_join(baller_thr, NULL);
    for(int i = 0 ; i<NUM_OF_BATSMAN ; i++) {
        pthread_join(batter_thr[i], NULL);
    }
    return 0;
}