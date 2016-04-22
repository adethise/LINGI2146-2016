/*
 * April 2016, Powell Jonathan
 *
 */
#include "contiki.h"
#include "net/rime.h"
#include "cfs/cfs.h"
#include "random.h"
#include "dev/leds.h"

#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

#define TIMER 5
#define MSGSIZE 5
#define PERIODIC TRUE

/*---------------------------------------------------------------------------*/
PROCESS(sender_process, "Sender example");
AUTOSTART_PROCESSES(&sender_process);
/*---------------------------------------------------------------------------*/
static void
broadcast_recv(struct broadcast_conn *c, const rimeaddr_t *from)
{
  printf("broadcast message received from %d.%d: '%s'\n",
         from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/


PROCESS_THREAD(sender_process, ev, data)
{ 
  int file;
  int r;
  int i = 0;
  /* Creating and stroring message */
  file = cfs_open("message", CFS_WRITE | CFS_APPEND | CFS_READ);
  if(file < 0) {  printf("Error creating file"); }
  
  while(i < MSGSIZE+1) {   
    if(i == MSGSIZE) {
      r = cfs_write(file, "\0", sizeof(char));
      if(r != sizeof(char)) { printf("Error writing file"); }
    }
    else {
      r = cfs_write(file, "a", sizeof(char));
      if(r != sizeof(char)) { printf("Error writing file"); }
    }
    i++;
  }

  cfs_close(file);  
  
  /* Reading message */
  file = cfs_open("message", CFS_READ);
  if(file < 0) {  printf("Error opening file"); }
  
  char message[MSGSIZE+1];
  r = cfs_read(file, message, sizeof(message));
  if(r != sizeof(message)) { printf("Error reading file"); }   
 
  cfs_close(file);
  
  static struct etimer et;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();

  broadcast_open(&broadcast, 129, &broadcast_call);

  while(1) {

    /* Delay TIMER seconds */
    if(PERIODIC) {
      etimer_set(&et, CLOCK_SECOND * TIMER);
    }
    else {
      etimer_set(&et, CLOCK_SECOND * TIMER + random_rand() % (CLOCK_SECOND * TIMER));
    }

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    packetbuf_copyfrom(message, MSGSIZE+1);
    leds_toggle(LEDS_GREEN);
    broadcast_send(&broadcast);
    printf("broadcast message sent\n");
    leds_toggle(LEDS_GREEN);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
