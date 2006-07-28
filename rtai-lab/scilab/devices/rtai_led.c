#include <machine.h>
#include <scicos_block.h>
#include <stdio.h>
#include <stdlib.h>
#include <rtai_netrpc.h>
#include <rtai_msg.h>
#include <rtai_mbx.h>
#include <string.h>
#include "rtmain.h"

#define MAX_RTAI_LEDS               1000
#define MBX_RTAI_LED_SIZE           5000

extern char *TargetLedMbxID;

void getstr(char * str, int par[], int init, int len);

static void init(scicos_block *block)
{
  char ledName[10];
  char name[7];
  int nch = block->nin;
  MBX *mbx;

  getstr(ledName,block->ipar,1,block->ipar[0]);
  rtRegisterLed(ledName,nch);
  get_a_name(TargetLedMbxID,name);

  mbx = (MBX *) RT_typed_named_mbx_init(0,0,name,MBX_RTAI_LED_SIZE/sizeof(unsigned int)*sizeof(unsigned int),FIFO_Q);
  if(mbx == NULL) {
    fprintf(stderr, "Cannot init mailbox\n");
    exit_on_error();
  }

  *block->work=(void *) mbx;
}

static void inout(scicos_block *block)
{
  MBX * mbx = (MBX *) (*block->work);
  int nleds=block->nin;
  int i;
  unsigned int led_mask = 0;

  for (i = 0; i < nleds; i++) {
    if (block->inptr[i][0] > 0.) {
      led_mask += (1 << i);
    } else {
      led_mask += (0 << i);
    }
  }
  RT_mbx_send_if(0, 0, mbx, &led_mask, sizeof(led_mask));
}

static void end(scicos_block *block)
{
  char ledName[10];
  MBX * mbx = (MBX *) (*block->work);
  RT_named_mbx_delete(0, 0, mbx);
  getstr(ledName,block->ipar,1,block->ipar[0]);
  printf("Led %s closed\n",ledName);
}

void rtled(scicos_block *block,int flag)
{
  if (flag==2){          
    inout(block);
  }
  else if (flag==5){     /* termination */ 
    end(block);
  }
  else if (flag ==4){    /* initialisation */
    init(block);
  }
}


