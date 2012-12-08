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


#endif //__Fifo_H__