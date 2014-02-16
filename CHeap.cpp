#include "CHeap.h"



bool get_standard_preceed(void* p, void* q)
{ return (long)p > (long)q; }



CHeap::CHeap(unsigned long res, bool (*prec)(void* p, void* q))
  : size(0), reserve(res), preceed(prec), id_rest(NULL)
{
  data = (void**)calloc(res, sizeof(void*));
  id = (unsigned long*)malloc(res * sizeof(unsigned long));
  position = (unsigned long*)malloc(res * sizeof(unsigned long));
  for (unsigned long k = 0; k < res; k++) {
    id[k] = ULONG_MAX;
    position[k] = ULONG_MAX;
    id_rest = new CList((void*)(res - k - 1), NULL, id_rest);
  }
}

CHeap::~CHeap()
{ free(data); free(id); free(position); id_rest->all_delete(); }



void* CHeap::get_data(unsigned long k)
{ return (k < size)? data[k]: NULL; }

unsigned long CHeap::get_size()
{ return size; }

unsigned long CHeap::get_id(unsigned long k)
{ return (k < size)? id[k]: ULONG_MAX; }

unsigned long CHeap::get_position(unsigned long i)
{ return (i < reserve)? position[i]: ULONG_MAX; }

/*
void CHeap::show_all_ids()
{
  for (unsigned long k = 0, n = 2; k < size; k++) {
    printf("%lu ", id[k]);
    if (k == n - 2 && k < size - 1) {
      printf("\n");
      n *= 2;
    }
  }
  printf("\n");
}
*/



void CHeap::exchange(unsigned long k, unsigned long l)
{
  void *tmp = data[k];
  unsigned long tmp_id = id[k], tmp_posi = position[id[k]];
  position[id[k]] = position[id[l]];
  id[k] = id[l];
  data[k] = data[l];
  position[id[l]] = tmp_posi;
  id[l] = tmp_id;
  data[l] = tmp;
}

unsigned long CHeap::go_up(unsigned long k)
{
  if (k == 0 || k >= size) return 0;
  else if (preceed(data[k], data[HeapParent(k)])) {
    exchange(k, HeapParent(k));
    return HeapParent(k);
  } else return 0;
}

unsigned long CHeap::go_down(unsigned long k)
{
  if (HeapChild(k, 0) >= size) return size;
  else if (HeapChild(k, 1) == size) {
    if (preceed(data[HeapChild(k, 0)], data[k])) {
      exchange(k, HeapChild(k, 0));
      return HeapChild(k, 0);
    } else return size;
  } else if (preceed(data[HeapChild(k, 0)], data[k]) ||
	     preceed(data[HeapChild(k, 1)], data[k])) {
    if (preceed(data[HeapChild(k, 0)], data[HeapChild(k, 1)])) {
      exchange(k, HeapChild(k, 0));
      return HeapChild(k, 0);
    } else {
      exchange(k, HeapChild(k, 1));
      return HeapChild(k, 1);
    }
  } else return size;
}



unsigned long CHeap::insert(void* p)
{
  if (size >= reserve) {
    reserve += HEAP_RESERVE_UNIT;
    data = (void**)realloc(data, reserve * sizeof(void*));
    for (unsigned long k = 0; k < HEAP_RESERVE_UNIT; k++) id_rest = new CList((void*)(reserve - k - 1), NULL, id_rest);
  }
  data[size] = p;

  unsigned long tmp_id = (unsigned long)id_rest->get_data();
  id[size] = tmp_id;
  position[id[size]] = size;
  for (unsigned long k = size++; k > 0; k = go_up(k));

  id_rest = id_rest->get_next();
  delete id_rest->get_prev();
  return tmp_id;
}

void CHeap::remove(unsigned long k)
{
  exchange(k, --size);
  id_rest = new CList((void*)id[size], NULL, id_rest);
  position[id[size]] = ULONG_MAX;
  id[size] = ULONG_MAX;
  data[size] = NULL;

  if (k == 0 || preceed(data[HeapParent(k)], data[k])) for (unsigned long l = k; l < size; l = go_down(l));
  else for (unsigned long l = k; l > 0; l = go_up(l));
}
