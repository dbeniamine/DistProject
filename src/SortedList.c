/****************************************************************************
 *                 Distributed Systems: Network simulator                   *
 * This file contains a library providing an implementation of SortedList.  *
 * Author: David Beniamine                                                  *
 ****************************************************************************/
#include"SortedList.h"
#include<stdio.h>
#include<stdlib.h>

/*
 * Create a new empty List.
 * The allocation is managed by the function, however the memory need to be
 * freed using the function DeleteList.
 * The functions returns the new List.
 * Comp : the function used to compare the elements of the list
 */
SortedList newSortedList(ComparatorFct comp){
    SortedList l=malloc(sizeof(struct _SortedList));
    if(l==NULL){
        fprintf(stderr,"Malloc fail in newSortedList\n");
        exit(EXIT_FAILURE);
    }
    l->comp=comp;
    l->head=NULL;
    l->size=0;
    return l;
}

/*
 * Internal function
 * e : the element
 * l : the list
 * Returns the predecessor of e in l
 * if e isn't in the list,  pred(e,l) is the element such that
 * pred->e < e < pred->next->e
 * Returns NULL if and only if l is empty
 */
List pred(void *e, SortedList l){
    List current, prec;
    current=l->head;
    prec=current;
    while(current !=NULL && l->comp(current->elt,e)<0){
        prec=current;
        current=current->next;
    }
    return prec;
}

/*
 * Add an element to the list at the good place
 * cost O(size(l))
 * The List should have been initialized using newSortedList previously.
 * e : A pointer to the element to add.
 * l : The SortedList.
 * Returns 0 on success
 * 1 if e was already in the list (no duplication)
 */
int AddSorted(void* e, SortedList l){
    List new=malloc(sizeof(struct _List));
    List prec;
    //Initialization of the element
    if(new==NULL){
        fprintf(stderr,"Malloc fail in AddSorted\n");
        exit(EXIT_FAILURE);
    }
    new->elt=e;
    //searching the good emplacement for e
    prec=pred(e,l); 
    if(prec==l->head){
        if(prec==NULL){
            //Special case: empty list
            new->next=NULL;
            l->head=new;
            l->size++;
            return 0;
        }
        if(!l->comp(prec->elt,e)){
            //the first element is e
            return 1;
        }
        if(l->comp(prec->elt,e)>0){
            //e < l->prec
            l->head=new;
            new->next=prec;
            l->size++;
            return 0;
        }
    }
    //pred != NULL and pred->e < e
    if(prec->next!=NULL && !l->comp(prec->next->elt,e)){
        //no duplication allowed
        return 1;
    }
    //here :prec->e < e < current->e
    new->next=prec->next;
    prec->next=new;
    l->size++;
    return 0;
}

/*
 * Removes the element from a List.
 * l : the SortedList.
 * e : the element to remove
 * Returns a pointer to the element or NULL if e isn't in the list
 */
void* Remove(void *e,SortedList l){
    List temp,prec=pred(e,l);
    void * ret;
    if(prec!=NULL && prec->next !=NULL && !l->comp(prec->next->elt,e)){
        //pred->next exists and is the element to remove
        temp=prec->next;
        ret=prec->next->elt;
        prec->next=prec->next->next;
        free(temp);
        l->size--;
        return ret;
    }   
    if(prec==l->head && prec!=NULL && !l->comp(prec->elt,e)){
        //pred is the element to remove
        temp=prec;
        ret=prec->elt;
        l->head=prec->next;
        free(temp);
        l->size--;
        return ret;
    }
    return NULL;
}

/*
 * Removes first the element from a List.
 * l : the SortedList.
 * Returns a pointer to the element or NULL if l is empty.
 */
void* RemoveFirst(SortedList l){
    void *ret;
    List tmp=l->head;
    if(tmp!=NULL){
        ret=tmp->elt;
        l->head=tmp->next;
        free(tmp);
        l->size--;
        return ret;
    }
    return NULL;
}


/*
 * Returns the size of a SortedList. 
 * l : the SortedList.
 */
int Size(SortedList l){
    return l->size;
}
/*
 * Deletes a List. The function handles the memory deallocation.
 * l : The SortedList to delete.
 */
void Delete(SortedList l){
    void *tmp;
    while((tmp=RemoveFirst(l))!=NULL){
        free(tmp);
    }
}

