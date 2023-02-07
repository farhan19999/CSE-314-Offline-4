# g++ super-over-zem.c zemaphore.c -o super-over-zem.o -lpthread 
# ./super-over-zem.o

g++ super-over-cv.c zemaphore.c -o super-over-cv.o -lpthread 
./super-over-cv.o

rm *.o