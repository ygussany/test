#ifndef _CHeap_20140215
#define _CHeap_20140215

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "CList.h"

#define HEAP_RESERVE_UNIT 0x100
#define HeapParent(k) (((k) - 1) / 2)
#define HeapChild(k, w) ((k) * 2 + (w) + 1)


bool get_standard_preceed(void* p, void* q);


class CHeap {
  void **data;
  unsigned long size, reserve;
  unsigned long *id, *position;
  CList *id_rest;

  bool (*preceed)(void* p, void* q);

public:
  CHeap(unsigned long res = HEAP_RESERVE_UNIT,
	bool (*prec)(void* p, void* q) = get_standard_preceed);
  ~CHeap();

  void* get_data(unsigned long k);
  unsigned long get_size();
  unsigned long get_id(unsigned long k);
  unsigned long get_position(unsigned long i);
  // void show_all_ids();

  void exchange(unsigned long k, unsigned long l);
  unsigned long go_up(unsigned long k);
  unsigned long go_down(unsigned long k);
  
  unsigned long insert(void* p);
  void remove(unsigned long k);
};

#endif
