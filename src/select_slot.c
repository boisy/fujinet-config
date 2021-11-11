/**
 * FujiNet for #Adam configuration program
 *
 * Select a Destination Device Slot
 */

#include <string.h>
#include "select_slot.h"
#include "screen.h"
#include "input.h"
#include "globals.h"
#include "io.h"
#include "bar.h"

extern DeviceSlot deviceSlots[8];

static enum
  {
   INIT,
   DISPLAY,
   CHOOSE,
   MODE,
   DONE
  } subState;

static char mode=0;
static char selected_device_slot=0;

void select_slot_init()
{
  subState=DISPLAY;
}

void select_slot_display()
{
  io_open_directory(selected_host_slot,path,filter);

  io_set_directory_position(pos);

  screen_select_slot(io_read_directory(41,0x80));
  
  io_close_directory();

  subState=CHOOSE;
}

void select_slot_choose()
{
  char k;
  
  screen_select_slot_choose();

  while (subState==CHOOSE)
    {
      k=input();
      switch(k)
	{
	case 0x0d:
	  selected_device_slot=bar_get();
	  subState=MODE;
	  break;
	case '1':
	case '2':
	case '3':
	case '4':
	  bar_jump(k-0x31);
	  break;
	case 0xA0:
	  bar_up();
	  break;
	case 0xA2:
	  bar_down();
	  break;
	}
    }
}

void select_slot_mode()
{
  char k;
  
  screen_select_slot_mode();

  while (subState==MODE)
    {
      k=input();
      switch(k)
	{
	case 0x0d:
	case 0x85:
	  mode=0;
	  subState=DONE;
	  break;
	case 0x86:
	  mode=3;
	  subState=DONE;
	case 0xA0:
	  bar_up();
	  break;
	case 0xA2:
	  bar_down();
	  break;
	}
    }  
}

void select_slot_done()
{
  char filename[256];

  strcat(filename,path);
  
  io_open_directory(selected_host_slot,path,filter);

  io_set_directory_position(pos);

  memcpy(deviceSlots[selected_device_slot].file,io_read_directory(31,0),31);
  deviceSlots[selected_device_slot].mode=mode;
  deviceSlots[selected_device_slot].hostSlot=selected_host_slot;
  
  io_put_device_slots(&deviceSlots[0]);
  
  io_set_directory_position(pos);

  strcat(filename,io_read_directory(255,0));

  sleep(1);
  
  io_set_device_filename(selected_device_slot,filename);
  
  io_close_directory();

  state=HOSTS_AND_DEVICES;
}

void select_slot(void)
{
  subState=INIT;
  
  while (state==SELECT_SLOT)
    {
      switch(subState)
	{
	case INIT:
	  select_slot_init();
	  break;
	case DISPLAY:
	  select_slot_display();
	  break;
	case CHOOSE:
	  select_slot_choose();
	  break;
	case MODE:
	  select_slot_mode();
	  break;
	case DONE:
	  select_slot_done();
	}
    }
}