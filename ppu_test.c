#include <stdio.h>
#include <stdint.h>

/* 
Defining Object Attribute Memory (OAM) Structure
Each byte represents an aspect of a sprite
Reference: https://www.nesdev.org/wiki/PPU_OAM
*/
struct OAM {
    uint8_t y_pos;                  // Byte 0 - Y position of top of sprite
    uint8_t tile_index_num;         // Byte 1 - Depends on whether we are dealing with 8x8 or 8x16 sprites
    uint8_t attributes;             // Byte 2 - Attributes
    uint8_t x_pos;                  // Byte 3 - X position of left side of sprite
};


int main(void) {

    // Creating sprite OAM
    struct OAM sprite_OAM;

    // Setting current sprite's values
    uint8_t current_sprite_y = 0xAB;
    uint8_t current_sprite_tile_index = 0xBC;
    uint8_t current_sprite_attributes = 0xCD;
    uint8_t current_sprite_x = 0xDE;

    // Saving current sprite's information to OAM structure
    sprite_OAM.y_pos = current_sprite_y;
    sprite_OAM.tile_index_num = current_sprite_tile_index;
    sprite_OAM.attributes = current_sprite_attributes;
    sprite_OAM.x_pos = current_sprite_x;

    // Printing sprite information
    printf("Y Pos: %02X\nTile Index Num: %02X\nAttributes: %02X\nX Pos: %02X\n", sprite_OAM.y_pos, sprite_OAM.tile_index_num, sprite_OAM.attributes, sprite_OAM.x_pos);

    return 0;
}