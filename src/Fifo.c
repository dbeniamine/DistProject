/****************************************************************************
 *                 Distributed Systems: Network simulator                   *
 * Implementation of the fifo library (defined in file "Fifo.h").           *
 * Author: David Beniamine                                                  *
 ****************************************************************************/
#include"Fifo.h"
#include<stdlib.h>
#include<stdio.h>

/*
 * Create a new empty Fifo.
 * The allocation is managed by the function, however the memory need to be
 * freed using the function DeleteFifo.
 * The functions returns the new Fifo.
 */
Fifo CreateFifo(void){
  Fifo f = malloc(sizeof(struct _Fifo));

  if(NULL == f){
     fprintf(stderr, "Malloc fail in CreateFifo...\n");
     exit(EXIT_FAILURE);
  }

  f->head=NULL;
  f->tail=NULL;

  return f;
}

/*
 * Deletes a Fifo. The function handles the memory deallocation.
 * f : The Fifo to delete.
 */
void DeleteFifo(Fifo f){
  void *o;

  // Empty the Fifo...
  while(NULL != (o = RemoveHead(f))) free(o);  

  // Free the Fifo.
  free(f);
}

/*
 * Appends an element to a Fifo.
 * The Fifo should have been initialized using CreateFifo previously.
 * e : A pointer to the element to add.
 * f : The Fifo.
 */
void Append(void* e, Fifo f){
  List new = malloc(sizeof(struct _List));

  if(NULL == new){
    fprintf(stderr, "Malloc fail in Append...\n");
    exit(EXIT_FAILURE);
  }

  // Set up new element
  new->elt = e;
  new->next = NULL;

  // Fifo empty is a special case
  if(NULL == f->head){
    f->head = new;
    f->tail = new;
  } else {
    f->tail->next = new;
    f->tail = new;
  }
}

/*
 * Returns a pointer to the head of a Fifo.
 * f : the Fifo.
 * Returns a pointer to the element or NULL if the Fifo is empty.
 */
void* GetHead(Fifo f){
  if(NULL == f->head) return NULL;

  return f->head->elt;
}

/*
 * Removes the element from a Fifo.
 * f : the Fifo.
 * Returns a pointer to the element or NULL if the Fifo is empty.
 */
void* RemoveHead(Fifo f){
  List tmp = f->head;
  void* elt;

  if(NULL == tmp) return NULL;

  // Remove firts element in the Fifo.
  f->head = f->head->next;
  if(NULL == f->head) f->tail = NULL;

  elt = tmp->elt;
  free(tmp);

  return elt;
}

/*
 * Check if a Fifo is empty.
 * f : the Fifo.
 * Returns 1 if the Fifo is empty.
 * Returns O otherwise.
 */
int IsEmpty(Fifo f){ 
  return NULL == f->head;
}
