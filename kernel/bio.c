// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

#define BUFFER_SIZE 10
#define BUCKET_SIZE 13

extern uint ticks;

struct {
  struct spinlock lock;
  struct buf buf[BUFFER_SIZE]; 
} bcaches[BUCKET_SIZE];


void
binit(void)
{
  for(int i = 0; i < BUCKET_SIZE; ++i) {
    initlock(&bcaches[i].lock, "bucket_lock");
    for(int j = 0; j < BUFFER_SIZE; ++i) {
      initsleeplock(&bcaches[i].buf[j].lock, "buf_lock");
    }
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;

  int bucket = blockno % BUCKET_SIZE;
  acquire(&bcaches[bucket].lock);


  // =============== find block in cur bucket ==================
  // is the block already cached?
  for(int i = 0; i < BUFFER_SIZE; ++i) {
    b = &bcaches[bucket].buf[i];
    if(b -> dev == dev && b -> blockno == blockno) {
      b -> refcnt++;
      b -> timestamp = ticks;
      release(&bcaches[bucket].lock);
      acquiresleep(&b -> lock);
      return b;
    }
  }

  // Not cached, LRU
  uint least = 0xffffffff;
  int least_idx = -1;
  for(int i = 0; i < BUFFER_SIZE; ++i) {
    b = &bcaches[bucket].buf[i];
    if(b -> refcnt == 0 && b -> timestamp < least) {
      least = b -> timestamp;
      least_idx = i;
    }
  }

  if(least_idx == -1) panic("no more space");

  b = &bcaches[bucket].buf[least_idx];
  b -> dev = dev;
  b -> blockno = blockno;
  b -> timestamp = ticks;
  b -> valid = 0;
  b -> refcnt = 1;

  release(&bcaches[bucket].lock);
  acquiresleep(&b -> lock);
  return b;
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  int bucket = b -> blockno % BUCKET_SIZE;

  acquire(&bcaches[bucket].lock);
  b->refcnt--;
  release(&bcaches[bucket].lock);
  releasesleep(&b -> lock);
}

void
bpin(struct buf *b) {
  int bucket = b -> blockno % BUCKET_SIZE; 
  acquire(&bcaches[bucket].lock);
  b->refcnt++;
  release(&bcaches[bucket].lock);
}

void
bunpin(struct buf *b) {
  int bucket = b -> blockno % BUCKET_SIZE;
  acquire(&bcaches[bucket].lock);
  b->refcnt--;
  release(&bcaches[bucket].lock);
}


