#include "CList.h"



CList::CList(void *p, CList *pr, CList *ne)
  : data(p), prev(pr), next(ne)
{
  if (pr != NULL) pr->next = this;
  if (ne != NULL) ne->prev = this;
}

CList::~CList()
{
  if (prev != NULL) prev->next = next;
  if (next != NULL) next->prev = prev;
}



void* CList::get_data()
{ return data; }

CList* CList::get_prev()
{ return prev; }

CList* CList::get_next()
{ return next; }



void CList::insert(CList* list)
{
  if (list != NULL) {
    list->prev = this;
    list->next = next;
  }
  if (next != NULL) next->prev = list;
  next = list;
}

void CList::append(CList* list)
{
  next = list;
  if (list != NULL) list->prev = this;
}

CList* CList::cut()
{
  CList *tmp = next;
  if (next != NULL) next->prev = NULL;
  next = NULL;
  return tmp;
}

CList* CList::remove()
{
  CList *tmp = next;
  if (prev != NULL) prev->next = next;
  if (next != NULL) next->prev = prev;
  prev = NULL;
  next = NULL;
  return tmp;
}

void CList::all_delete()
{
  for (CList *p = next, *q; p != NULL; p = q) {
    q = p->next;
    delete p;
  }
  delete this;
}
