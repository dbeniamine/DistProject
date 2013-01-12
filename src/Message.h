/****************************************************************************
 *                 Distributed Systems: Network simulator                   *
 * This file contains the definition of messages and of some functions to   *
 * create, copy and delete them.                                            *
 * Author: David Beniamine                                                  *
 ****************************************************************************/
#ifndef __MESSAGE_H__
#define __MESSAGE_H__

/*
 * Structure implementing messages.
 */
typedef struct _Message{
  int sender; // Identifier of the sender (external event if -1)
  int receiv; // Destination address (-1 if ip multicast)
  char *msg;  // Content of the message
}*Message;

/*
 * Message initialization function.
 * str : content of the message.
 * snd : sender of the message.
 * rcv : receiver of the message.
 * Failures are handled internally.
 * Returns a the new created message.
 */
Message initMessage(const char* str, int snd, int rcv);

/*
 * Delete a message.
 * msg : the message to delete.
 * Failures are handled internally.
 */
void deleteMessage(Message msg);

/*
 * Copy a message.
 * msg : the message.
 * Failures are handled internaly.
 * Returns the copied message.
 */
Message copyMessage(Message msg);

#endif //__MESSAHE_H__
