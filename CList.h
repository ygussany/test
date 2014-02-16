#ifndef _CList_20140215
#define _CList_20140215

#include <stddef.h>

class CList {
protected:
  void *data;
  CList *prev, *next;

public:
  CList(void *p, CList *pr = NULL, CList *ne = NULL);
  ~CList();

  void* get_data();
  CList* get_prev();
  CList* get_next();

  void insert(CList* list);
  void append(CList* list);
  CList* cut();
  CList* remove();
  void all_delete();
};

#endif
