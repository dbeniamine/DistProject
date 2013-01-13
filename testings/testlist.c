#include<stdio.h>
#include<stdlib.h>
#include"../src/SortedList.h"

int comp(void *e1, void *e2){
    int i1=*((int *)e1),i2=*((int *)e2);
    return i1-i2;
}

void aff(SortedList l){
    int i=0;
    List cur;
    cur=l->head;
    printf("%d\n", Size(l));
    while(cur!=NULL){
        printf("%d : %d\n",i,*((int*)(cur->elt)));
        cur=cur->next;
        i++;
    }
}
int main(){
    int i, elts[20];
    int * temp;
    SortedList l=newSortedList(comp);
    srand(42);
    for(i=0;i<20;i++){
        elts[i]=rand()%100;
        printf("add %d, res : %d\n", elts[i], AddSorted(elts+i,l)); 
        aff(l);
    }
    printf("rand remove\n");
    while(Size(l)>10){
        i=rand()%20;
        temp=(int *)Remove(elts+i,l);
        printf("removed %d res :%d \n",elts[i] ,temp==NULL?-1:*temp);
        aff(l);
    }
    printf("last remove\n");
    while(Size(l)>0){
        RemoveFirst(l);
        aff(l);
    }
}
