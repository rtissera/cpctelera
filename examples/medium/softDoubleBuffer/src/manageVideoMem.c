//-----------------------------LICENSE NOTICE------------------------------------
//  This file is part of CPCtelera: An Amstrad CPC Game Engine
//  Copyright (C) 2017 Bouche Arnaud
//  Copyright (C) 2017 ronaldo / Fremos / Cheesetea / ByteRealms (@FranGallegoBR)
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//------------------------------------------------------------------------------

#include <declarations.h>

/////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
//
u8 gSpriteBackBuffer[VIEW_W_BYTES*VIEW_H_BYTES];   // Sprite that acts as Software Back buffer 
u8 gVMem;                                          // Current video mem

/////////////////////////////////////////////////////////////////////////////////
// INITITALIZE VIDEO MEMORY BUFFERS
//    Initializes tracking of video memory buffers
//
void InitializeVideoMemoryBuffers() {
   // Clear the Hardware Back Buffer (Our secondary screen 
   // buffer from 0x8000 to 0xBFFF).
   cpct_memset((u8*)SCREEN_BUFF, 0, 0x4000);   
   // Set Video Memory as current displayed buffer
   gVMem = VIDEO_MEM;   
}

/////////////////////////////////////////////////////////////////////////////////
// FLIPBUFFERS
//    Interchanges present screen video memory for the present buffer memory.
// This change is made instantly, so all the contents of screen buffer are shown
// at once at the screen. Present screen memory now becomes the new screen 
// buffer and viceversa, hence the name "FlipBuffer".
//    In order to prevent intermediate flickering, this function waits until
// VSYNC signal is up before flipping both buffers
//
void FlipBuffers() {
   cpct_waitVSYNC(); // Wait until VSYNC is up

   // Depending on present video memory, flip from 
   // Video to Buffer or from Buffer to Video
   if (gVMem == BUFFER_MEM) {
      cpct_setVideoMemoryPage(cpct_pageC0);
      gVMem = VIDEO_MEM;
   } else {
      cpct_setVideoMemoryPage(cpct_page80);
      gVMem = BUFFER_MEM;
   }
}

/////////////////////////////////////////////////////////////////////////////////
// GET SCREEN PTR
//    Gets a pointer to a (x,y) location in the current video memory
//
u8* GetScreenPtr(u8 xPos, u8 yPos) {
   u8*   screenStart;

   // Screen video memory start depends on which hardware
   // buffer is currently being used as video memory
   if (gVMem == VIDEO_MEM) screenStart = (u8*)CPCT_VMEM_START;
   else                    screenStart = (u8*)SCREEN_BUFF;

   // Calculate and return screen pointer
   return cpct_getScreenPtr(screenStart, xPos, yPos);
}

/////////////////////////////////////////////////////////////////////////////////
// GET BACK BUFFER PTR
//    Gets a pointer to a (x,y) location in the present hardware back buffer
//
u8* GetBackBufferPtr(u8 xPos, u8 yPos) {
   u8*   backBufferStart;

   // Hardware back buffer memory start depends on which hardware
   // buffer is currently being used as video memory
   if (gVMem == VIDEO_MEM) backBufferStart = (u8*)SCREEN_BUFF;
   else                    backBufferStart = (u8*)CPCT_VMEM_START;

   // Calculate and return screen pointer
   return cpct_getScreenPtr(backBufferStart, xPos, yPos);
}

/////////////////////////////////////////////////////////////////////////////////
// GET SPRITE BACK BUFFER PTR
//    Gets a pointer to a (x,y) location in the sprite back buffer
//
u8* GetSpriteBackBufferPtr(u8 xPos, u8 yPos) {
   return cpctm_spriteBufferPtr(gSpriteBackBuffer, VIEW_W_BYTES, xPos, yPos);
}


////////////////////////////////////////////////////////////////////////////////////////////
// DRAW SPRITE BACK BUFFER TO SCREEN
//
//    Waits for VSYNC and then copies the Sprite Back Buffer to its final location at the
// screen (actually, it draws it)
//
void DrawSpriteBackBufferToScreen() {
   // Calculate screen location where Sprite Back Buffer will be drawn
   u8*   pVideoMemLocation = GetScreenPtr(VIEW_X, VIEW_Y);

   // Wait for VSYNC and perform the actual drawing of the Sprite
   cpct_waitVSYNC();
   cpct_drawSprite(gSpriteBackBuffer, pVideoMemLocation, VIEW_W_BYTES, VIEW_H_BYTES);
}