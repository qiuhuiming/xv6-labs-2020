// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

#define LOCKS_NAME_LENGTH ((int)10)

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock locks[NCPU];
  struct run *freelists[NCPU];
} kmem;

static char locks_names[NCPU][LOCKS_NAME_LENGTH];

static void kfree_cpuid(void *pa, uint cpu_id);

static void* kalloc_cpuid(uint cpu_id);

void
kinit()
{
  for (int i = 0; i < NCPU; i++) {
    snprintf(locks_names[i], LOCKS_NAME_LENGTH, "kmem_%d", i);
    initlock(&kmem.locks[i], locks_names[i]);
  }
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  char *start;
  p = start = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE) {
    int cpu_id = (p - start) / PGSIZE % NCPU;
    kfree_cpuid(p, cpu_id);
  }
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  kfree_cpuid(pa, cpuid());
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void) {
  return kalloc_cpuid(cpuid());
}

static void kfree_cpuid(void *pa, uint cpu_id) {
  struct run *r;
  
  memset(pa, 1, PGSIZE);
  r = (struct run*)pa;
  acquire(&kmem.locks[cpu_id]);
  r->next = kmem.freelists[cpu_id];
  kmem.freelists[cpu_id] = r;
  release(&kmem.locks[cpu_id]);
}

static void* kalloc_cpuid(uint cpu_id) {
  struct run *r;

  acquire(&kmem.locks[cpu_id]);
  r = kmem.freelists[cpu_id];
  if (r) {
    kmem.freelists[cpu_id] = r->next;
  }
  release(&kmem.locks[cpu_id]);

  if (r) {
    memset((char*) r, 5, PGSIZE);
  }
  return (void*) r;
}
