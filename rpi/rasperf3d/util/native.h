#ifndef NATIVE_H
#define NATIVE_H

// InitPlatform(): Init keyboard and create a native window framebuffer configured for 
// full-screen display.
// in: fbWidth, fbHeight - Desired framebuffer dimensions. Pass in 0,0 to create a framebuffer
// that is the same dimesions as the screen.
//     multisampling - True if 4x full-screen multisampling is desired.
// out: fbWidth, fbHeight - Actual framebuffer dimensions.
// returns: True if initialization was successful.
void InitPlatform(uint32_t* fbWidth, uint32_t* fbHeight, bool multisampling);

// DeinitPlaform(): Reset keyboard and free the previously-created native window framebuffer.
void DeinitPlatform();

// ReInitDisplay(): Release the previously-created native window framebuffer, and create a new one
void ReInitDisplay(uint32_t* fbWidth, uint32_t* fbHeight, bool multisampling);

// GetKey(): Get a keypress from the keyboard buffer. Returns EOF (-1) if no character is available
int32_t GetKey(); 

// SwapBuffers(): Perform an OpenGL frambuffer swap.
void SwapBuffers();

// SaveScreenshot(): Take a screenshot of the current framebuffer image and save it to screenshot.ppm.
void SaveScreenshot();

// ShouldTerminate(): True if the program should terminate (interrupt or terminate signal was caught) 
bool ShouldTerminate();

#endif
