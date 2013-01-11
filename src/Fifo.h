/****************************************************************************
 *                 Distributed Systems: Network simulator                   *
 * This file contains a library providing an implementation of fifos.       *
 * Author: David Beniamine                                                  *
 ****************************************************************************/
#ifndef __Fifo_H__
#define __Fifo_H__

/*
 * Structure implementing linked lists.
 */
typedef struct _List{
  void* elt;          // Element of the current link
  struct _List* next; // Pointer to the next link
}*List;

/*
 * Structure implementing fifos.
 */
typedef struct _Fifo{
  List head;          // Head of the fifo
  List tail;          // Tail of the fifo
}*Fifo;

/*
 * Create a new empty Fifo.
 * The allocation is managed by the function, however the memory need to be
 * freed using the function DeleteFifo.
 * The functions returns the new Fifo.
 */
Fifo CreateFifo(void);

/*
 * Deletes a Fifo. The function handles the memory deallocation.
 * f : The Fifo to delete.
 */
void DeleteFifo(Fifo f);

/*
 * Appends an element to a Fifo.
 * The Fifo should have been initialized using CreateFifo previously.
 * e : A pointer to the element to add.
 * f : The Fifo.
 */
void Append(void* e, Fifo f);

/*
 * Removes the element from a Fifo.
 * f : the Fifo.
 * Returns a pointer to the element or NULL if the Fifo is empty.
 */
void* RemoveHead(Fifo f);

/*
 * Check if a Fifo is empty.
 * f : the Fifo.
 * Returns 1 if the Fifo is empty.
 * Returns O otherwise.
 */
int IsEmpty(Fifo f); 

#endif //__Fifo_H__
