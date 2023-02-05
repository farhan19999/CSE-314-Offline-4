#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
  //	Write the code for initializing your read-write lock.
  rw->lock = PTHREAD_MUTEX_INITIALIZER;
  rw->readerCond = PTHREAD_COND_INITIALIZER;
  rw->writerCond = PTHREAD_COND_INITIALIZER;
  rw->isReaderMode = 0;
  rw->isWriterMode = 0;
  rw->readerCount = 0;
  rw->writerCount = 0;
}

void ReaderLock(struct read_write_lock * rw)
{
  pthread_mutex_lock(&rw->lock);
  rw->readerCount++;
  while(rw->isWriterMode){
    pthread_cond_wait(&rw->readerCond,&rw->lock);
  }
  rw->isReaderMode = 1;
  pthread_mutex_unlock(&rw->lock);
}

void ReaderUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
  pthread_mutex_lock(&rw->lock);
  rw->readerCount--;
  if(rw->readerCount == 0) rw->isReaderMode = 0;
  if(rw->readerCount == 0) pthread_cond_signal(&rw->writerCond);
  pthread_mutex_unlock(&rw->lock);
}

void WriterLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
  pthread_mutex_lock(&rw->lock);
  rw->writerCount++;
  while(rw->isReaderMode || rw->isWriterMode){
    pthread_cond_wait(&rw->writerCond,&rw->lock);
  }
  rw->isWriterMode = 1;
  pthread_mutex_unlock(&rw->lock);
}

void WriterUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
  pthread_mutex_lock(&rw->lock);
  rw->writerCount--;
  rw->isWriterMode = 0;
  if(rw->readerCount > 0) {
    pthread_cond_broadcast(&rw->writerCond);
  }
  else if(rw->writerCount > 0){
    pthread_cond_signal(&rw->writerCond);
  }
  pthread_mutex_unlock(&rw->lock);
}
