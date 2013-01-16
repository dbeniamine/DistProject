/****************************************************************************
 *                 Distributed Systems: Network simulator                   *
 * Implementation of message functions.                                     *
 * Author: David Beniamine                                                  *
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Message.h"

/*
 * Message initialization function.
 * str : content of the message.
 * org : originator of the message.
 * snd : sender of the message.
 * rcv : receiver of the message.
 * Failures are handled internally.
 * Returns a the new created message.
 */
Message initMessage(const char* str, int org, int snd, int rcv){
  Message msg = malloc(sizeof(struct _Message));
  int len = strlen(str) + 1;

  if(NULL == msg){
    fprintf(stderr, "Malloc error in initMessage...\n");
    exit(1);
  }

  msg->msg = malloc(len * sizeof(char));
  if(NULL == msg->msg){
    fprintf(stderr, "Malloc error in initMessage...\n");
    exit(1);
  }

  msg->msg=strncpy(msg->msg, str, len);
  msg->origin = org;
  msg->sender = snd;
  msg->receiv = rcv;

  return msg;
}

/*
 * Delete a message.
 * msg : the message to delete.
 * Failures are handled internally.
 */
void deleteMessage(Message msg){
  if(NULL != msg){
    if(NULL != msg->msg) free(msg->msg);
    free(msg);
  }
}

/*
 * Copy a message.
 * msg : the message.
 * Failures are handled internaly.
 * Returns the copied message.
 */
Message copyMessage(Message msg){
  if(NULL == msg) return NULL;
  return initMessage(msg->msg, msg->origin, msg->sender, msg->receiv);
}
