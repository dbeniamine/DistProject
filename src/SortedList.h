/****************************************************************************
 *                 Distributed Systems: Network simulator                   *
 * This file contains a library providing an implementation of SortedList.  *
 * Author: David Beniamine                                                  *
 ****************************************************************************/
#ifndef __SortedList_H__
#define __SortedList_H__

#include"List.h"

/*
 * Comparator function
 * Return 0 if e1==e2
 *  >0 if e1>e2
 *  <0 else
 */
typedef int(*ComparatorFct)(void *e1, void *e2);

typedef struct _SortedList{
    int size;               //size of the list
    ComparatorFct comp;
    struct _List* head;  //head of the list
}*SortedList;

/*
 * Create a new empty List.
 * The allocation is managed by the function, however the memory need to be
 * freed using the function DeleteList.
 * The functions returns the new List.
 * Comp : the function used to compare the elements of the list
 */
SortedList newSortedList(ComparatorFct comp);

/*
 * Add an element to the list at the good place
 * cost O(size(l))
 * The List should have been initialized using newSortedList previously.
 * e : A pointer to the element to add.
 * l : The SortedList.
 * Returns 0 on success
 * 1 if e was already in the list (no duplication)
 */
int AddSorted(void* e, SortedList l);

/*
 * Removes the element from a List.
 * l : the SortedList.
 * e : the element to remove
 * Returns a pointer to the element or NULL if e isn't in the list
 */
void* Remove(void *e,SortedList l);

/*
 * Removes first the element from a List.
 * l : the SortedList.
 * Returns a pointer to the element or NULL if l is empty.
 */
void* RemoveFirst(SortedList l);

/*
 * Returns the size of a SortedList. 
 * l : the SortedList.
 */
int Size(SortedList l); 
/*
 * Deletes a List. The function handles the memory deallocation.
 * l : The SortedList to delete.
 */
void Delete(SortedList l);

#endif //__SortedList_H__
