/****************************************************************************
 *                 Distributed Systems: Network simulator                   *
 * This file contains a library providing an implementation of LinkedList.  *
 * Author: David Beniamine                                                  *
 ****************************************************************************/
#ifndef __List_H__
#define __List_H__

/*
 * Structure implementing linked lists.
 */
typedef struct _List{
  void* elt;          // Element of the current link
  struct _List* next; // Pointer to the next link
}*List;

#endif //__LIST_H__
