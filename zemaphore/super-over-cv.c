#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>


#define NUM_OF_BATSMAN 3
#define NUM_OF_OVER 1


int next_batter=2, on_strike=-1, off_strike=1;
int total_ball = NUM_OF_OVER*6;
int ball_left = total_ball;
int turn = 0;

pthread_mutex_t cond_lock = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t bowling_done[NUM_OF_BATSMAN];
pthread_cond_t batting_done = PTHREAD_COND_INITIALIZER;


void *throw_ball(void* ptr) {
    for(int i = 1 ; i<=total_ball ; i++) {
        pthread_mutex_lock(&cond_lock);    
        while(turn && !(next_batter > NUM_OF_BATSMAN))pthread_cond_wait(&batting_done, &cond_lock);
        
        if(next_batter > NUM_OF_BATSMAN) {
            printf("The batting team is bowled out\n");
            turn =1;
            for(int j = 0 ; j<NUM_OF_BATSMAN ; j++) pthread_cond_signal(&bowling_done[j]);
            pthread_mutex_unlock(&cond_lock);      
            break;
        }
        printf("The bowler balled %dth ball of the over\n", i);
        turn =1;
        pthread_cond_signal(&bowling_done[on_strike]);
        pthread_mutex_unlock(&cond_lock);
    }

    for(int j = 0 ; j<NUM_OF_BATSMAN ; j++) {
        
        pthread_cond_signal(&bowling_done[j]);
    }
}

void *hit_the_ball(void* ptr) {
    int id = *((int*)ptr);
    
    while(1) {
        pthread_mutex_lock(&cond_lock);    
        
        while(turn == 0 && !(next_batter > NUM_OF_BATSMAN || ball_left == 0) )pthread_cond_wait(&bowling_done[id], &cond_lock);
        
        if(next_batter > NUM_OF_BATSMAN || ball_left == 0) {
            turn =0;
            pthread_cond_signal(&batting_done);
            for(int j = 0 ; j<NUM_OF_BATSMAN ; j++) pthread_cond_signal(&bowling_done[j]);
            pthread_mutex_unlock(&cond_lock);
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
            turn =0;
            pthread_cond_signal(&batting_done);
            pthread_mutex_unlock(&cond_lock);
            break;
        }
        turn =0;
        ball_left--;
        pthread_cond_signal(&batting_done);
        pthread_mutex_unlock(&cond_lock);
    }
}

int main() {

    srand(time(0));

    pthread_t baller_thr;
    pthread_t batter_thr[NUM_OF_BATSMAN];

    int *batter_id;
    batter_id = (int*)malloc(sizeof(int));

 
    for(int i = 0 ; i<NUM_OF_BATSMAN ; i++)bowling_done[i] = PTHREAD_COND_INITIALIZER;

    pthread_create(&baller_thr, NULL, throw_ball, NULL);

    for(int i = 0 ; i<NUM_OF_BATSMAN ; i++) {
        batter_id[i] = i;
        pthread_create(&batter_thr[i], NULL, hit_the_ball, &batter_id[i]);    
    }

    //signaling the bowler thread to start bowling
    pthread_cond_signal(&batting_done);

    pthread_join(baller_thr, NULL);
    for(int i = 0 ; i<NUM_OF_BATSMAN ; i++) {
        pthread_join(batter_thr[i], NULL);
    }
    return 0;
}