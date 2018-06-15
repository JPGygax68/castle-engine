#ifndef FONT_H
#define FONT_H

void InitFont(uint32_t screenWidth, uint32_t screenHeight);
void DeInitFont();
void AddText(const char* text, int x, int y);
void RenderText();

#endif