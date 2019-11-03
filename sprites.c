
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
  0x03,			// screen color

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

static byte x, y, ox, oy, a, len;

// main program
void main() {
  char i;	// actor index
  char oam_id;	// sprite ID
  
  ox = 124;
  oy = 112;
  
  len = 80;
  
  // initialize actors with random values
  for (i=0; i<NUM_ACTORS; i++) {
    actor_x[i] = rand();
    actor_y[i] = rand();
    actor_dx[i] = (rand() & 7) - 3;
    actor_dy[i] = (rand() & 7) - 3;
  }
  // initialize PPU
  setup_graphics();
  // loop forever
  while (1) {

    // wait for next frame
    ppu_wait_nmi();    
    
    // clear all sprites from sprite buffer
    oam_clear(); 

   
    // start with OAMid/sprite 0
    oam_id = 0;
    oam_id = oam_spr(ox, oy, 0xAF, i, oam_id);
    
    x = (cos(a) * len) / 128;
    y = (sin(a) * len) / 128;
    oam_id = oam_spr(ox + x, oy + y, 0xAF, i, oam_id);

    x = (cos(a * 2) * (len + 30)) / 128;
    y = (sin(a * 2) * (len + 30)) / 128;    
    oam_id = oam_spr(ox + x, oy + y, 0xAF, i, oam_id);
    
       
    a += 1;
    
    
    
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
  }
}
