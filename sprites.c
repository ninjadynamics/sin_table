
/*
Sprite demo.
Animate all 64 hardware sprites.
*/

#include <stdlib.h>
#include <string.h>

// include NESLIB header
#include "neslib.h"

// link the pattern table into CHR ROM
//#link "chr_generic.s"

/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] = { 
  0xF,			// screen color

  0x11,0x30,0x27,0x0,	// background palette 0
  0x1c,0x20,0x2c,0x0,	// background palette 1
  0x00,0x10,0x20,0x0,	// background palette 2
  0x06,0x16,0x26,0x0,   // background palette 3

  0x16,0x35,0x24,0x0,	// sprite palette 0
  0x00,0x37,0x25,0x0,	// sprite palette 1
  0x0d,0x2d,0x3a,0x0,	// sprite palette 2
  0x0d,0x27,0x2a	// sprite palette 3
};

// setup PPU and tables
void setup_graphics() {
  // clear sprites
  oam_clear();
  // set palette colors
  pal_all(PALETTE);
  // turn on PPU
  ppu_on_all();
}

// number of actors
#define NUM_ACTORS 16		// 64 sprites (maximum)

// actor x/y positions
byte actor_x[NUM_ACTORS];	// horizontal coordinates
byte actor_y[NUM_ACTORS];	// vertical coordinates

// actor x/y deltas per frame (signed)
sbyte actor_dx[NUM_ACTORS];	// horizontal velocity
sbyte actor_dy[NUM_ACTORS];	// vertical velocity

const signed char sin_table[256]={
  0,2,5,8,11,13,16,19,
  22,24,27,30,33,35,38,41,
  43,46,48,51,53,56,58,60,
  63,65,67,70,72,74,76,78,
  80,82,84,86,88,89,91,93,
  94,96,97,99,100,101,103,104,
  105,106,107,108,109,109,110,111,
  111,112,112,113,113,113,113,113,
  114,113,113,113,113,113,112,112,
  111,111,110,109,109,108,107,106,
  105,104,103,101,100,99,97,96,
  94,93,91,89,88,86,84,82,
  80,78,76,74,72,70,67,65,
  63,60,58,56,53,51,48,46,
  43,41,38,35,33,30,27,24,
  22,19,16,13,11,8,5,2,
  0,-2,-5,-8,-11,-13,-16,-19,
  -22,-24,-27,-30,-33,-35,-38,-41,
  -43,-46,-48,-51,-53,-56,-58,-60,
  -63,-65,-67,-70,-72,-74,-76,-78,
  -80,-82,-84,-86,-88,-89,-91,-93,
  -94,-96,-97,-99,-100,-101,-103,-104,
  -105,-106,-107,-108,-109,-109,-110,-111,
  -111,-112,-112,-113,-113,-113,-113,-113,
  -114,-113,-113,-113,-113,-113,-112,-112,
  -111,-111,-110,-109,-109,-108,-107,-106,
  -105,-104,-103,-101,-100,-99,-97,-96,
  -94,-93,-91,-89,-88,-86,-84,-82,
  -80,-78,-76,-74,-72,-70,-67,-65,
  -63,-60,-58,-56,-53,-51,-48,-46,
  -43,-41,-38,-35,-33,-30,-27,-24,
  -22,-19,-16,-13,-11,-8,-5,-2
};

signed char sin(unsigned char angle)
{
  if (angle < 128) return sin_table[angle]; else return -sin_table[angle & 127];
}

signed char cos(unsigned char angle)
{
  return sin(angle+64);
}

static sbyte x, y;
static byte  ox, oy, a, len;

long f;

byte pad;

static sbyte cam_x, cam_y;

// main program
void main() {
  char i;	// actor index
  char oam_id;	// sprite ID
  
  ox = 124;
  oy = 112;
  
  len = 100;
  
  // initialize actors with random values
  for (i=0; i<NUM_ACTORS; i++) {
    actor_x[i] = rand();
    actor_y[i] = rand();
    actor_dx[i] = (rand() & 7) - 3;
    actor_dy[i] = (rand() & 7) - 3;
  }
  
  // write text to name table
  vram_adr(NTADR_A(12,10));		// set address
  //vram_write("BANANAS!!!", 10);	// write bytes to video RAM  
  
  // initialize PPU
  setup_graphics(); 
  
  // loop forever
  while (1) {
   
    // clear all sprites from sprite buffer
    //oam_clear(); 

   
    // start with OAMid/sprite 0
    oam_id = 0;
    //oam_id = oam_spr(ox, oy, 0xAE, i, oam_id);
    
    for (i = 10; i != 0; --i) {      
      x = (cos(8*a - i*16) * (0 + (i * 4)) ) / 128;
      y = (sin(i*2      ) * (32 + (i * cam_y / 2)) ) / 128;      
      oam_id = oam_spr(ox + x - cam_x, oy + y - cam_y, 0xAF, 0 | OAM_BEHIND, oam_id);
    }   
    ++a;
    //a = 50;
    //if (++f % 2 == 0) a+= 1;
    
    pad = pad_poll(0);
    if (pad & PAD_LEFT) ++cam_x; 
    else if (pad & PAD_RIGHT) --cam_x;
    
    if (pad & PAD_UP) --cam_y; 
    else if (pad & PAD_DOWN) ++cam_y;
    
    
    /*
    // draw and move all actors
    for (i=0; i<NUM_ACTORS; i++) {
      oam_id = oam_spr(actor_x[i], actor_y[i], 0xAF, i, oam_id);
      actor_x[i] += actor_dx[i];
      actor_y[i] += actor_dy[i];
    }
    */
    // hide rest of sprites
    // if we haven't wrapped oam_id around to 0
    if (oam_id!=0) oam_hide_rest(oam_id);
    
    //pal_spr_bright(2 + (nesclock() / 2) % 4);
    
    // wait for next frame
    ppu_wait_nmi();    
    
  }
}
