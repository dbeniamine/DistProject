#include"Fifo.h"
#include<stdlib.h>
#include<stdio.h>

//init a Fifo
Fifo CreateFifo(void)
{
    Fifo f;
    if((f=malloc(sizeof(struct _Fifo)))==NULL){
        fprintf(stderr, "malloc fail CreateFifo.0\n");
        exit(1);
    }
    f->head=NULL;
    f->tail=NULL;
    return f;
}

//Delete the Fifo, free all the elements
void Delete (Fifo f){
    void *o;
    while((o=RemoveHead(f))!=NULL){
        free(o);  
    }
}

// Remove the Head of the list and return its element
void *RemoveHead(Fifo f){
    listElt temp;
    void * ret=NULL;
    temp=f->head;
    if(temp!=NULL){
        f->head=f->head->next;
        if(f->head==NULL){
            f->tail=NULL;
        }
        ret=temp->elt;
        free(temp);
    }

    return ret;
}

void Append(void *obj, Fifo f){
    listElt e;
    if((e=malloc(sizeof(struct _listElt)))==NULL){
        fprintf(stderr,"malloc fail Fifo.Append.0\n");
        exit(1);
    }
    //init
    e->elt=obj;
    e->next=NULL;
    
    if(f->head==NULL){
        //First element on the list
        f->head=e;
        f->tail=e;
    }else{
        //General case
        f->tail->next=e;
        f->tail=e;
    }
}
