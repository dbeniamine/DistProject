#ifndef __Node_H__
#define __Node_H__

#include "Fifo.h"

typedef struct _Node{
    Fifo receivBuf;
    Fifo sendBuf;
    Fifo eventsBuf;
}*Node;

#endif //__Node_H__ 
