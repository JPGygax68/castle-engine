#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <GLES2/gl2.h>

#include "util/bitmap.h"

#include "renderStats.h"
#include "shaderLoader.h"

#define FONT_SIZE 16
#define U_SIZE (1.0f/16.0f)
#define V_SIZE (1.0f/8.0f)
#define MAX_TEXT_LENGTH 2000
#define VERTICES_PER_GLYPH 6
#define VERTEX_BUFFER_SIZE (MAX_TEXT_LENGTH * VERTICES_PER_GLYPH)

struct fontVertexInfo
{
    float x;
    float y;
    float u;
    float v;
};

struct FontState
{
    uint32_t screenWidth, screenHeight;

    GLuint fontTexture;
    GLuint fontBuffer;
    GLuint fontShader;
    GLuint fontScreenHalfSizeId;
    GLint fontPositionId;
    GLint fontUVId;
    GLint fontSamplerId;

    fontVertexInfo* fontBufferData;
    uint32_t charCount;
};

static FontState fs;

void InitFont(uint32_t screenWidth, uint32_t screenHeight)
{
    fs.screenWidth = screenWidth;
    fs.screenHeight = screenHeight;

    fs.fontTexture = LoadBMP("atari-font.bmp");
    glGenBuffers(1, &fs.fontBuffer);
    fs.fontShader = LoadShader("text.vertexShader", "text.fragmentShader");
    assert(fs.fontShader);

    // get location of shader parameters
    fs.fontScreenHalfSizeId = glGetUniformLocation(fs.fontShader, "screenHalfSize");
    assert(fs.fontScreenHalfSizeId != -1);
    fs.fontPositionId = glGetAttribLocation(fs.fontShader, "vertPosition_screenspace");
    assert(fs.fontPositionId != -1);
    fs.fontUVId = glGetAttribLocation(fs.fontShader, "texCoord0");
    assert(fs.fontUVId != -1);
    fs.fontSamplerId = glGetUniformLocation(fs.fontShader, "textureSampler");
    assert(fs.fontSamplerId != -1);
    
    // set invariant uniforms
    glUseProgram(fs.fontShader);
    glUniform1i(fs.fontSamplerId, 0); // sample from texture unit #0
    glUniform2f(fs.fontScreenHalfSizeId, fs.screenWidth/2, fs.screenHeight/2);
    glUseProgram(0);

    // allocate a buffer for the font vertex data
    fs.fontBufferData = (fontVertexInfo*)malloc(sizeof(fontVertexInfo)*VERTEX_BUFFER_SIZE);
    fs.charCount = 0;
}

void DeInitFont()
{
    glDeleteShader(fs.fontShader);
    glDeleteBuffers(1, &fs.fontBuffer);
    glDeleteTextures(1, &fs.fontTexture);
}

void AddText(const char* text, int x, int y)
{
    // TODO
    // 1. add text color to the fragment shader
    // 2. precompute the XY for all verts, since only the UVs and the number of entries changes
    // 3. allow for some kind of static text object for text that never changes
    // 4. combine all text drawn in a single frame into one draw call (conflicts with 2 and 3?)

    // fill vertex/uv buffer 
    fontVertexInfo* pBuffer = &fs.fontBufferData[fs.charCount*VERTICES_PER_GLYPH];  
    for (; *text != 0 && fs.charCount < MAX_TEXT_LENGTH; fs.charCount++, text++, x += FONT_SIZE)
    {
        char character = *text - 32;
        float u = (character%16)*U_SIZE;
        float v = (character/16)*V_SIZE;

        *pBuffer++ = (fontVertexInfo) { x, y, u, v };
        *pBuffer++ = (fontVertexInfo) { x, y+FONT_SIZE, u, v+V_SIZE };
        *pBuffer++ = (fontVertexInfo) { x+FONT_SIZE, y, u+U_SIZE, v };
        *pBuffer++ = (fontVertexInfo) { x+FONT_SIZE, y+FONT_SIZE, u+U_SIZE, v+V_SIZE };
        *pBuffer++ = (fontVertexInfo) { x+FONT_SIZE, y, u+U_SIZE, v };
        *pBuffer++ = (fontVertexInfo) { x, y+FONT_SIZE, u, v+V_SIZE };
    }
}

void RenderText()
{
    glBindBuffer(GL_ARRAY_BUFFER, fs.fontBuffer);
    glBufferData(GL_ARRAY_BUFFER, 
        sizeof(fontVertexInfo)*VERTICES_PER_GLYPH*fs.charCount, fs.fontBufferData, GL_STATIC_DRAW);

    // bind the shader program 
    glUseProgram(fs.fontShader);

    // bind the model's texture to texture unit #0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fs.fontTexture);

    // set OpenGL state
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // set vertex data pointers
    glEnableVertexAttribArray(fs.fontPositionId);
    glVertexAttribPointer(fs.fontPositionId, 2, GL_FLOAT, false, sizeof(fontVertexInfo), 
        (const void*)0);

    glEnableVertexAttribArray(fs.fontUVId);
    glVertexAttribPointer(fs.fontUVId, 2, GL_FLOAT, false, sizeof(fontVertexInfo), 
        (const void*)(sizeof(float)*2));

    // draw!
    glDrawArrays(GL_TRIANGLES, 0, fs.charCount * VERTICES_PER_GLYPH);
    LogDrawCall(fs.charCount * VERTICES_PER_GLYPH, fs.charCount * VERTICES_PER_GLYPH / 3);
    fs.charCount = 0;

    // clean up state - unbind OpenGL resources
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}