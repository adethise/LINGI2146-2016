/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Testing the broadcast layer in Rime
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include "net/rime.h"
#include "random.h"


#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>
#include <string.h>


char *message = " \
Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec id nisi at dolor porttitor pulvinar. Integer aliquam leo vel ipsum faucibus viverra. Nunc vulputate luctus quam nec pretium. Morbi sed euismod magna, ac tristique felis. Pellentesque viverra libero pellentesque, mattis lectus commodo, porta leo. Nullam sit amet nisl ultrices, malesuada ligula vulputate, congue massa. Suspendisse tempor dignissim viverra. Suspendisse potenti. Sed sit amet lectus eget nunc rutrum pellentesque.\
Aenean facilisis dui augue, vel pretium lectus imperdiet rutrum. Phasellus ante ipsum, efficitur nec magna nec, bibendum maximus tellus. Etiam imperdiet rhoncus odio, ac porta ante lobortis sed. Vestibulum rutrum, lacus eget efficitur blandit, quam nisl tristique sapien, auctor rhoncus ligula erat vel lorem. Nunc lacinia sem tincidunt, fermentum risus eget, facilisis enim. Nulla porta nibh arcu, ac placerat leo cursus ut. Sed tincidunt sapien eu lectus vehicula, sit amet vehicula ex rhoncus. Duis et sagittis ligula, id aliquam risus. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Nunc felis ante, lacinia molestie egestas sit amet, rhoncus vitae erat. Duis imperdiet, massa vitae venenatis vehicula, justo urna aliquam sem, in fringilla mi ex eu velit. Ut laoreet, nisl id rhoncus rhoncus, lorem dolor lobortis purus, ut bibendum dolor nisi a nibh. Pellentesque ut pulvinar justo, in posuere sapien.\
Integer in rutrum leo. Nullam eu tristique felis, quis maximus sem. Nam aliquet sit amet velit sollicitudin tempus. Etiam dictum diam nec aliquam consequat. Curabitur eget velit est. Fusce ante ligula, sollicitudin et enim non, vehicula interdum ex. Donec placerat laoreet augue, in pellentesque sem malesuada vel. Mauris euismod enim sed malesuada malesuada. Fusce lacus purus, dictum et ex in, faucibus rhoncus libero. Nam vel vulputate turpis, non lobortis eros. Praesent aliquet orci vel molestie dapibus. Proin sed velit ut lacus sollicitudin viverra sed nec leo. Mauris enim tortor, finibus sit amet tellus in, laoreet suscipit nisl. Sed quam ex, auctor sed tristique in, rutrum nec risus.\
Proin porttitor eros eros, ut semper dolor laoreet ut. Pellentesque vitae neque at risus consectetur pharetra id id mi. Sed in pellentesque diam, et scelerisque dui. Aliquam et dui vitae risus pharetra consectetur non sit amet nisl. Sed dictum tortor hendrerit mollis vestibulum. Phasellus finibus, dolor vel gravida scelerisque, mauris dolor pharetra leo, quis condimentum mauris erat laoreet odio. Ut congue euismod mi, et efficitur dolor accumsan quis. Duis vel magna finibus, eleifend magna porta, bibendum nibh. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras tellus ipsum, pellentesque vel dapibus vel, auctor id arcu. Morbi sed auctor mi, nec placerat mi. Nulla lacinia varius velit in varius. Nulla non pulvinar quam, at vestibulum magna. Mauris lectus augue, lobortis elementum facilisis sit amet, convallis vel magna. Cras quam odio, sodales tempor mollis et, pharetra ultrices est. Maecenas condimentum iaculis neque quis aliquet.\
Fusce interdum ligula sed urna ultricies accumsan. Integer auctor efficitur laoreet. Vestibulum congue nec orci at molestie. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Phasellus metus augue, egestas quis tempor et, placerat vitae turpis. Duis lectus massa, elementum in quam quis, pharetra luctus lacus. Sed vitae risus eu mauris tempor molestie. Pellentesque pellentesque tincidunt tempor. Cras pellentesque lorem massa. Sed accumsan tempor ex bibendum mattis.";

/*---------------------------------------------------------------------------*/
PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);
/*---------------------------------------------------------------------------*/
static void
broadcast_recv(struct broadcast_conn *c, const rimeaddr_t *from)
{
  printf("broadcast message received from %d.%d: '%s'\n",
         from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
  		 if(strcmp((char *)packetbuf_dataptr(),message)==0)
         leds_toggle(LEDS_YELLOW);
}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_broadcast_process, ev, data)
{
  static struct etimer et;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();

  broadcast_open(&broadcast, 129, &broadcast_call);

  while(1) {

    etimer_set(&et, CLOCK_SECOND + random_rand() % CLOCK_SECOND);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    packetbuf_copyfrom(message, strlen(message)+1);
    broadcast_send(&broadcast);
    printf("broadcast message sent\n");
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
