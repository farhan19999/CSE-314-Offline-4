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


int next_batter=2, on_strike=0, off_strike=1;
int total_ball = NUM_OF_OVER*6;
int ball_left = total_ball;


pthread_mutex_t cond_lock = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t batting_done = PTHREAD_COND_INITIALIZER;
pthread_cond_t bowling_done = PTHREAD_COND_INITIALIZER;

enum TURN {BALLER, BATTER};
int current_turn;

void *throw_ball(void* ptr) {
    for(int i = 1 ; i<=total_ball ; i++) {
        pthread_mutex_lock(&cond_lock);    
        while(current_turn == BATTER) {
            printf("bowler is waiting\n");
            pthread_cond_wait(&batting_done, &cond_lock);
        }
        if(next_batter > NUM_OF_BATSMAN) {
            printf("The batting team is bowled out\n");
            current_turn = BATTER;
            pthread_cond_broadcast(&bowling_done);
            pthread_mutex_unlock(&cond_lock);      
            printf("Bowler done\n");
            break;
        }
        printf("The bowler balled %dth ball of the over\n", i);
        current_turn = BATTER;
        pthread_cond_broadcast(&bowling_done);
        pthread_mutex_unlock(&cond_lock);
    }
}

void *hit_the_ball(void* ptr) {
    int id = *((int*)ptr);
    
    while(1) {
        pthread_mutex_lock(&cond_lock);    
        
        while(on_strike != id && (next_batter <= NUM_OF_BATSMAN && ball_left > 0)) {
            pthread_cond_wait(&bowling_done, &cond_lock);
        }
        while(current_turn == BATTER) {
            printf("batter %d is waiting for baller\n",id);
            pthread_cond_wait(&bowling_done,&cond_lock);
        }

        if(next_batter > NUM_OF_BATSMAN || ball_left == 0) {
            current_turn = BALLER;
            pthread_cond_signal(&batting_done);
            pthread_mutex_unlock(&cond_lock);
            printf("Leaving thread batter %d\n",id);
            break;
        }
        int run = rand()%8;
        
        if((run >= 0 && run<4) || run == 5) {

            int t = on_strike;
            on_strike = off_strike;
            off_strike = t;

            printf("The batter %d has taken %d run and swapped strike\n", id, run);
        }
        else if(run == 4 || run == 6) {
            printf("The batter %d has hit a **%d**\n", id, run);
        }
        else {
            // 7 run means out
            on_strike = next_batter;
            next_batter++;
            printf("next Batter %d made by batter %d\n", next_batter,id);
            printf("The batter %d is out\n", id);
            current_turn = BALLER;
            pthread_cond_broadcast(&batting_done);
            pthread_mutex_unlock(&cond_lock);
            printf("Leaving thread batter %d\n",id);
            break;
        }
        current_turn = BALLER;
        pthread_cond_broadcast(&batting_done);
        pthread_mutex_unlock(&cond_lock);
        ball_left--;
        
    }
}

int main() {

    srand(0);

    pthread_t baller_thr;
    pthread_t batter_thr[NUM_OF_BATSMAN];

    int *batter_id;
    batter_id = (int*)malloc(sizeof(int));

    current_turn = BALLER;
  


    for(int i = 0 ; i<NUM_OF_BATSMAN ; i++) {
        batter_id[i] = i;
        pthread_create(&batter_thr[i], NULL, hit_the_ball, &batter_id[i]);    
    }
    pthread_create(&baller_thr, NULL, throw_ball, NULL);

    //signaling the bowler thread to start bowling
    pthread_cond_signal(&batting_done);

    pthread_join(baller_thr, NULL);
    for(int i = 0 ; i<NUM_OF_BATSMAN ; i++) {
        pthread_join(batter_thr[i], NULL);
    }
    return 0;
}