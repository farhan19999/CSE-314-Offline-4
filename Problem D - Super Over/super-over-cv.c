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

enum TURN {BALLER, BATTER};
int current_turn;

void *throw_ball(void* ptr) {
    while(1) {
        pthread_mutex_lock(&cond_lock);    
<<<<<<< HEAD:Problem D - Super Over/super-over-cv.c
        while(current_turn == BATTER && next_batter<=NUM_OF_BATSMAN && ball_left >0) {
            // printf("bowler is waiting\n");
            pthread_cond_wait(&batting_done, &cond_lock);
        }
        if(next_batter > NUM_OF_BATSMAN) {
            printf("The batting team is bowled out\n");
            current_turn = BATTER;
            pthread_cond_broadcast(&bowling_done);
=======
        pthread_cond_wait(&batting_done, &cond_lock);
        if(on_strike < 0 ) {
            //game hasn't started
            on_strike = 0;
        }
        if(next_batter > NUM_OF_BATSMAN) {
            printf("The batting team is bowled out\n");
            turn = 1;
            for(int j = 0 ; j<NUM_OF_BATSMAN ; j++) pthread_cond_signal(&bowling_done[j]);
>>>>>>> master:zemaphore/super-over-cv.c
            pthread_mutex_unlock(&cond_lock);      
            printf("Bowler done\n");
            break;
        }
<<<<<<< HEAD:Problem D - Super Over/super-over-cv.c
        if(ball_left <= 0){
            // printf("NO BALLS LEFT");
            current_turn = BATTER;
            pthread_cond_broadcast(&bowling_done);
            pthread_mutex_unlock(&cond_lock);
            printf("Bowler done\n");
            break;
        }
        printf("The bowler balled %dth ball of the over\n", total_ball-ball_left+1);
        current_turn = BATTER;
        pthread_cond_broadcast(&bowling_done);
=======
        printf("The bowler balled %dth ball of the over\n", i);
        turn=1;
        pthread_cond_signal(&bowling_done[on_strike]);
>>>>>>> master:zemaphore/super-over-cv.c
        pthread_mutex_unlock(&cond_lock);
    }
}

void *hit_the_ball(void* ptr) {
    int id = *((int*)ptr);
    
    while(1) {
        pthread_mutex_lock(&cond_lock);    
        
<<<<<<< HEAD:Problem D - Super Over/super-over-cv.c
        while(on_strike != id && (next_batter <= NUM_OF_BATSMAN && ball_left > 0)) {
            // printf("batter %d is waiting\n",id);
            pthread_cond_wait(&bowling_done, &cond_lock);
        }
        while(current_turn == BALLER && next_batter<=NUM_OF_BATSMAN && ball_left > 0) {
            // printf("batter %d is waiting for baller\n",id);
            pthread_cond_wait(&bowling_done,&cond_lock);
        }

        if(next_batter > NUM_OF_BATSMAN || ball_left <= 0) {
            current_turn = BALLER;
=======
        pthread_cond_wait(&bowling_done[id], &cond_lock);
        
        if(next_batter > NUM_OF_BATSMAN || ball_left == 0) {
            turn = 0;
>>>>>>> master:zemaphore/super-over-cv.c
            pthread_cond_signal(&batting_done);
            pthread_mutex_unlock(&cond_lock);
            // printf("Leaving thread batter %d %d\n",id,current_turn);
            break;
        }
        int run = rand()%8;
<<<<<<< HEAD:Problem D - Super Over/super-over-cv.c
        if(run == 0){
            printf("Batter didnot score any run\n");
        }
        if((run > 0 && run<4) || run == 5) {
            if(run%2){
=======
        
        if((run >= 0 && run<4) || run == 5) {

            if((run%2)==1) {
>>>>>>> master:zemaphore/super-over-cv.c
                int t = on_strike;
                on_strike = off_strike;
                off_strike = t;
                printf("The batter %d has taken %d run and swapped strike\n", id, run);
            }
<<<<<<< HEAD:Problem D - Super Over/super-over-cv.c
            else printf("The batter %d has taken %d run \n", id, run);
=======
            else {
                if(run)printf("The batter %d has taken %d run \n", id, run);
                else printf("The batter %d was unable to take a run\n", id);
            }
>>>>>>> master:zemaphore/super-over-cv.c
        }
        else if(run == 4 || run == 6) {
            printf("The batter %d has hit a **%d**\n", id, run);
        }
        else {
            // 7 run means out
            on_strike = next_batter;
            next_batter++;
            ball_left--;
            printf("next Batter %d made by batter %d\n", next_batter,id);
            printf("The batter %d is out\n", id);
<<<<<<< HEAD:Problem D - Super Over/super-over-cv.c
            current_turn = BALLER;
            pthread_cond_broadcast(&batting_done);
=======
            ball_left--;
            pthread_cond_signal(&batting_done);
>>>>>>> master:zemaphore/super-over-cv.c
            pthread_mutex_unlock(&cond_lock);
            // printf("Leaving thread batter %d %d\n",id,current_turn);
            break;
        }
<<<<<<< HEAD:Problem D - Super Over/super-over-cv.c
        current_turn = BALLER;
        pthread_cond_broadcast(&batting_done);
        pthread_mutex_unlock(&cond_lock);
=======
        turn = 0;
>>>>>>> master:zemaphore/super-over-cv.c
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

<<<<<<< HEAD:Problem D - Super Over/super-over-cv.c
    current_turn = BALLER;
=======
 
    for(int i = 0 ; i<NUM_OF_BATSMAN ; i++)bowling_done[i] = PTHREAD_COND_INITIALIZER;

>>>>>>> master:zemaphore/super-over-cv.c
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