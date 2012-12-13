#ifndef __Fifo_H__
#define __Fifo_H__

typedef struct _listElt{
    void *elt;
    struct _listElt * next;
}*listElt;

typedef struct _Fifo{
    listElt head;
    listElt tail;
}*Fifo;

//create a Fifo
Fifo CreateFifo(void);

//delete a Fifo
void Delete(Fifo);

//Append an object to the Fifo 
void Append(void *, Fifo);

//Remove the head of the list
//Return NULL if empty
void * RemoveHead(Fifo);

//Return the head of the list without removing it
//Return NULL if empty
void *GetHead(Fifo);

//True if and only if the Fifo is empy
int IsEmpty(Fifo); 

#endif //__Fifo_H__
