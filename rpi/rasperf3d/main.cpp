#include <string>
#include <stdexcept>
#include <iostream>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <GLES2/gl2.h>

#include "util/bitmap.h"
#include "util/ktx.h"
#include "util/native.h"
#include "util/vecmath.h"

#include "font.h"
#include "cube_model.h"
#include "frog_model.h"
#include "renderStats.h"
#include "shaderLoader.h"

typedef enum {
    FIRST_MODEL_TYPE = 0,
    CUBE_MODEL = 0,
    FROG_MODEL,
    KID_MODEL,
    TREX_MODEL,
    ROBOT_MODEL,
    NUM_MODEL_TYPES  
} ModelType;

struct ModelInfo
{
    float* pVertexData;
    uint16_t* pIndexData;
    GLuint vertexBuffer;
    GLuint indexBuffer;  
    GLuint modelTexture;
    uint32_t numIndices;
    uint32_t numVerts;
};

struct RenderState 
{
    uint32_t fbWidth;
    uint32_t fbHeight;
    
    uint32_t numObjects;
    float yaw;

    bool showStats;
    bool backFaceCull;
    bool depthTest;
    bool renderLines;
    bool linearFiltering;
    bool mipmapping;
    float cameraDistance;
    float cameraYaw;
    bool cameraLookAway;
    bool multisampling;
    
    uint8_t shaderType;
    ShaderInfo shaderInfo[NUM_SHADER_TYPES];

    uint8_t modelType;
    ModelInfo modelInfo[NUM_MODEL_TYPES];
};

static RenderState rs;

bool InitShaderProgram()
{
    for (uint8_t t = FIRST_SHADER_TYPE; t < NUM_SHADER_TYPES; t++)
    {
        LoadShaderByType((ShaderType)t, &rs.shaderInfo[t]);
    }
    
    return true;
}

void InitModelBuffers()
{
    // cube: model data is in header file
    ModelInfo* m  = &rs.modelInfo[CUBE_MODEL];
    m->pVertexData = 0;
    m->pIndexData = 0;

    glGenBuffers(1, &m->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m->vertexBuffer); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_verts), cube_verts, GL_STATIC_DRAW);
    
    glGenBuffers(1, &m->indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);  

    m->numIndices = sizeof(cube_indices) / sizeof(uint16_t);
    m->numVerts = sizeof(cube_verts) / (sizeof(float)*8);

    // frog: model data is in header file
    m  = &rs.modelInfo[FROG_MODEL];
    m->pVertexData = 0;
    m->pIndexData = 0;

    glGenBuffers(1, &m->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m->vertexBuffer); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(frog_verts), frog_verts, GL_STATIC_DRAW);
    
    glGenBuffers(1, &m->indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(frog_indices), frog_indices, GL_STATIC_DRAW);  

    m->numIndices = sizeof(frog_indices) / sizeof(uint16_t);
    m->numVerts = sizeof(frog_verts) / (sizeof(float)*8);

    // kid: too large to compile from a header file - load from an external binary file
    m  = &rs.modelInfo[KID_MODEL];
    FILE* pFile = fopen("kid.dat", "rb");
    if (pFile == NULL)
    {
        printf("Couldn't open kid.dat\n");
        return;
    } 

    fread(&m->numVerts, sizeof(uint32_t), 1, pFile);
    uint32_t vertexBufferSize = m->numVerts * sizeof(float) * 8;
    m->pVertexData = (float*)malloc(vertexBufferSize);
    fread(m->pVertexData, vertexBufferSize, 1, pFile);
    fread(&m->numIndices, sizeof(uint32_t), 1, pFile);
    uint32_t indexBufferSize = m->numIndices * sizeof(uint16_t);
    m->pIndexData = (uint16_t*)malloc(indexBufferSize);
    fread(m->pIndexData, indexBufferSize, 1, pFile);
    fclose(pFile);

    glGenBuffers(1, &m->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m->vertexBuffer); 
    glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, m->pVertexData, GL_STATIC_DRAW);

    glGenBuffers(1, &m->indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, m->pIndexData, GL_STATIC_DRAW); 

    // t-rex: too large to compile from a header file - load from an external binary file
    m  = &rs.modelInfo[TREX_MODEL];
    pFile = fopen("trex.dat", "rb");
    if (pFile == NULL)
    {
        printf("Couldn't open trex.dat\n");
        return;
    } 

    fread(&m->numVerts, sizeof(uint32_t), 1, pFile);
    vertexBufferSize = m->numVerts * sizeof(float) * 8;
    m->pVertexData = (float*)malloc(vertexBufferSize);
    fread(m->pVertexData, vertexBufferSize, 1, pFile);
    fread(&m->numIndices, sizeof(uint32_t), 1, pFile);
    indexBufferSize = m->numIndices * sizeof(uint16_t);
    m->pIndexData = (uint16_t*)malloc(indexBufferSize);
    fread(m->pIndexData, indexBufferSize, 1, pFile);
    fclose(pFile); 

    // transform the data
    /*for (uint32_t i=0; i<m->numVerts; i++)
    {
        m->pVertexData[8*i] *= 0.017f;
        m->pVertexData[8*i+1] *= 0.017f;
        m->pVertexData[8*i+2] *= 0.017f;
    }

    // write out the transformed data
    pFile = fopen("trex2.dat", "wb");
    if (pFile == NULL)
    {
        printf("Couldn't open trex2.dat\n");
        return;
    }    

    fwrite(&m->numVerts, sizeof(uint32_t), 1, pFile);
    fwrite(m->pVertexData, vertexBufferSize, 1, pFile);
    fwrite(&m->numIndices, sizeof(uint32_t), 1, pFile);
    fwrite(m->pIndexData, indexBufferSize, 1, pFile);
    fclose(pFile);
    printf("wrote trex2.dat\n");*/
    
    glGenBuffers(1, &m->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m->vertexBuffer); 
    glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, m->pVertexData, GL_STATIC_DRAW);

    glGenBuffers(1, &m->indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, m->pIndexData, GL_STATIC_DRAW); 

    // robot: too large to compile from a header file - load from an external binary file
    m = &rs.modelInfo[ROBOT_MODEL];
    pFile = fopen("robot.dat", "rb");
    if (pFile == NULL)
    {
        printf("Couldn't open robot.dat\n");
        return;
    } 

    fread(&m->numVerts, sizeof(uint32_t), 1, pFile);
    vertexBufferSize = m->numVerts * sizeof(float) * 8;
    m->pVertexData = (float*)malloc(vertexBufferSize);
    fread(m->pVertexData, vertexBufferSize, 1, pFile);
    fread(&m->numIndices, sizeof(uint32_t), 1, pFile);
    indexBufferSize = m->numIndices * sizeof(uint16_t);
    m->pIndexData = (uint16_t*)malloc(indexBufferSize);
    fread(m->pIndexData, indexBufferSize, 1, pFile);
    fclose(pFile);
    
    glGenBuffers(1, &m->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m->vertexBuffer); 
    glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, m->pVertexData, GL_STATIC_DRAW);

    glGenBuffers(1, &m->indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, m->pIndexData, GL_STATIC_DRAW); 

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);   
}

void InitModel()
{
    InitModelBuffers();
    rs.modelInfo[CUBE_MODEL].modelTexture = LoadKTX("cube.ktx");
    rs.modelInfo[FROG_MODEL].modelTexture = LoadKTX("frog.ktx");
    rs.modelInfo[KID_MODEL].modelTexture = LoadKTX("kid.ktx");
    rs.modelInfo[TREX_MODEL].modelTexture = LoadKTX("trex.ktx");
    rs.modelInfo[ROBOT_MODEL].modelTexture = LoadKTX("robot.ktx");
    if (!rs.modelInfo[CUBE_MODEL].modelTexture || 
        !rs.modelInfo[FROG_MODEL].modelTexture || 
        !rs.modelInfo[KID_MODEL].modelTexture || 
        !rs.modelInfo[TREX_MODEL].modelTexture || 
        !rs.modelInfo[ROBOT_MODEL].modelTexture)
    {
        printf("Failed to load the model textures.\n");
    }

    InitShaderProgram();

    int err = glGetError();
    if (err != 0) throw std::runtime_error(std::string("GL error at end of InitModel():") + std::to_string(err));
}

void FreeModel()
{
    for (uint8_t t = FIRST_SHADER_TYPE; t < NUM_SHADER_TYPES; t++)
    {
        glDeleteShader(rs.shaderInfo[t].shaderProgram);
    }

    for (uint8_t t = FIRST_MODEL_TYPE; t < NUM_MODEL_TYPES; t++)
    {
        ModelInfo& m = rs.modelInfo[t];
        glDeleteBuffers(1, &m.vertexBuffer);
        glDeleteBuffers(1, &m.indexBuffer);
        if (m.pVertexData)
            free(m.pVertexData);
        if (m.pIndexData)
            free(m.pIndexData);
        glDeleteTextures(1, &m.modelTexture);
    }
}

void Render()
{
    static mat4 proj(mat4::perspective(45, ((float) rs.fbWidth)/rs.fbHeight, 1, 1000));

    // calculate the view matrix
    vec4 lookPos(0,2.3f,rs.cameraLookAway ? 100 : 0,1);
    vec4 eye = mat4::rotate(rs.cameraYaw, vec4(0,1,0,1)) * vec4(0,0,rs.cameraDistance,1);
    mat4 view(mat4::lookAt(eye, lookPos, vec4(0,1,0,1)));

    // light position is fixed in world space
    vec4 lightPosition_worldspace = vec4(-30,10,20,1);
    vec4 lightPosition_viewspace = view * lightPosition_worldspace;
 
    // bind the shader program and set the uniform shader parameters
    ShaderInfo& currentShader = rs.shaderInfo[rs.shaderType];
    glUseProgram(currentShader.shaderProgram);
    glUniform3fv(currentShader.lightLoc, 1, &lightPosition_viewspace.x); // only the light's XYZ is passed
    glUniform1i(currentShader.texSamplerLoc, 0); // sample from texture unit #0

    // bind the model's texture to texture unit #0
    ModelInfo& currentModel = rs.modelInfo[rs.modelType];
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, currentModel.modelTexture);

    // bind the vertex and index buffer
    glBindBuffer(GL_ARRAY_BUFFER, currentModel.vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentModel.indexBuffer);

    // set OpenGL states
    if (rs.backFaceCull)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

    if (rs.depthTest)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    glDepthFunc(GL_LESS);
    glClearColor ( 0.1f, 0.2f, 0.3f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    
    // set vertex data pointers
    const uint32_t posSize = 3;
    const uint32_t normSize = 3;
    const uint32_t uvSize = 2;
    const uint32_t vertBytes = sizeof(float) * (posSize + normSize + uvSize);

    glEnableVertexAttribArray(currentShader.vertLoc);
    glVertexAttribPointer(currentShader.vertLoc, posSize, GL_FLOAT, false, vertBytes, 
        (const void*)0);

    glEnableVertexAttribArray(currentShader.normalLoc);
    glVertexAttribPointer(currentShader.normalLoc, normSize, GL_FLOAT, false, vertBytes, 
        (const void*)(posSize * sizeof(float)));

    glEnableVertexAttribArray(currentShader.texcoordLoc);
    glVertexAttribPointer(currentShader.texcoordLoc, uvSize, GL_FLOAT, false, vertBytes, 
        (const void*)((posSize + normSize) * sizeof(float)));

    // calculate how many objects to draw per row
    uint32_t rowSize = 1;
    while (rowSize * rowSize < rs.numObjects)
        rowSize++;
    const float rowScale = 25.0f;
    float rowStep = rs.numObjects == 1 ? rowScale : rowScale / (rowSize - 1);

    // draw the objects in a grid layout
    uint32_t numObjectsDrawn = 0;
    for (int x=0; x<rowSize && numObjectsDrawn < rs.numObjects; x++)
    {
        float objX = (rs.numObjects == 1) ? 0 : (1.5f * (-rowScale/2.0f + x*rowStep));

        for (int y=0; y<rowSize && numObjectsDrawn < rs.numObjects; y++)
        {    
            float objXs = objX + (y%2)*rowStep/2.0f;
            float objY = (rs.numObjects == 1) ? 0 : (-rowScale/2.0f + y*rowStep);

            // construct a yaw for this object, by combining the global yaw with the grid position
            // calculate the model matrix from the yaw
            float yaw = rs.yaw + x*40 + y*50;
            mat4 modelRotate(mat4::rotate(yaw, vec4(0,1,0,1)));
            mat4 modelTranslate(mat4::translate(vec4(objXs,objY,0,1)));
            mat4 model = modelTranslate * modelRotate;
            
            // calculate the matrices
            mat4 mv = view * model;
            mat4 mvp = proj * view * model;

            // set the per-model parameters
            glUniformMatrix4fv(currentShader.mvLoc, 1, false, &mv.x.x);
            glUniformMatrix4fv(currentShader.mvpLoc, 1, false, &mvp.x.x);

            // draw!
            GLenum mode = rs.renderLines ? GL_LINE_STRIP : GL_TRIANGLES;
            glDrawElements(mode, currentModel.numIndices, GL_UNSIGNED_SHORT, (void*)0);
            LogDrawCall(currentModel.numVerts, currentModel.numIndices/3);

            numObjectsDrawn++;
        }
    } 

    //std::cout << "Number of objects drawn: " << numObjectsDrawn << std::endl;

    // clean up state - unbind OpenGL resources
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    int err = glGetError();
    if (err != 0) throw std::runtime_error(std::string("GL error at end of Render():") + std::to_string(err));
}

void ApplyTextureFilter()
{
    GLenum minFilter;
    GLenum magFilter;
    if (rs.linearFiltering)
    {
        magFilter = GL_LINEAR;
        if (rs.mipmapping)
            minFilter = GL_LINEAR_MIPMAP_NEAREST;
        else
            minFilter = GL_LINEAR;
    }
    else
    {
        magFilter = GL_NEAREST;
        if (rs.mipmapping)
            minFilter = GL_NEAREST_MIPMAP_NEAREST;
        else
            minFilter = GL_NEAREST;
    }

    glBindTexture(GL_TEXTURE_2D, rs.modelInfo[rs.modelType].modelTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ResetFramebuffer()
{
    // get a new framebuffer
    ReInitDisplay(&rs.fbWidth, &rs.fbHeight, rs.multisampling);
    
    // all OpenGL resources must be freed and re-acquired when the framebuffer changes
    FreeModel();
    DeInitFont();
    InitModel();
    InitFont(rs.fbWidth, rs.fbHeight);
    ApplyTextureFilter();
}

void SetNextFramebufferResolution()
{
    // shrink the framebuffer to 2/3 its previous size
    if (rs.fbHeight <= 320)
    {
        rs.fbWidth = 0;
        rs.fbHeight = 0; 
    }
    else
    {
        rs.fbWidth = rs.fbWidth * 2 / 3;
        rs.fbHeight = rs.fbHeight * 2 / 3;
    }

    ResetFramebuffer();
}

void SetFramebufferSize(uint32_t w, uint32_t h)
{
    rs.fbWidth = w;
    rs.fbHeight = h;
}

void SetDefaultSettings()
{
    // default settings
    rs.numObjects = 81;
    rs.showStats = true;
    rs.backFaceCull = true;
    rs.depthTest = true;
    rs.renderLines = false;
    rs.linearFiltering = true;
    rs.mipmapping = true;
    rs.cameraDistance = 40.0f;
    rs.cameraYaw = 0.0f;
    rs.shaderType = GOURAUD; // FIRST_SHADER_TYPE;
    rs.cameraLookAway = false;
    rs.modelType = FIRST_MODEL_TYPE;
    rs.multisampling = false;
}

int main(void) 
{
    // create a native frambuffer with dimensions equal to the display screen
    rs.fbWidth = 0;
    rs.fbHeight = 0;
    InitPlatform(&rs.fbWidth, &rs.fbHeight, false);

    SetDefaultSettings();

    // load the model, texture, and shader
    InitModel();

    // load the font resources
    InitFont(rs.fbWidth, rs.fbHeight);

    // initialize the frame timer
    struct timespec now, then;
    clock_gettime(CLOCK_REALTIME, &then);

    while (!ShouldTerminate())
    {
        // compute elapsed time since the last frame
        clock_gettime(CLOCK_REALTIME, &now);
        uint32_t frameTimeMicrosecs = ((now.tv_sec - then.tv_sec) * 1000000) + 
                                    (now.tv_nsec - then.tv_nsec) / 1000;
        then = now;  

        // handle keyboard input
        int32_t key = GetKey();
        if (key != EOF)
        {
            // terminate on ESC (0x1B) or Ctrl-D (0x04)
            if (key == 0x1B || key == 0x04) 
                break;
            // +/- changes the number of objects displayed
            else if (key == '-')
                rs.numObjects = (rs.numObjects > 0) ? (rs.numObjects - 1) : 0;
            else if (key == '=' || key == '+')
                rs.numObjects++;
            // S shows/hides the render statistics
            else if (key == 's' || key == 'S')
                rs.showStats = !rs.showStats;
            // C turns backface culling on/off
            else if (key == 'c' || key == 'C')
                rs.backFaceCull = !rs.backFaceCull;
            // D turns depth testing on/off
            else if (key == 'd' || key == 'D')
                rs.depthTest = !rs.depthTest;
            // W turns wireframe rendering on/off
            else if (key == 'w' || key == 'W')
                rs.renderLines = !rs.renderLines;
            // F changes the framebuffer size
            else if (key == 'f' || key == 'F')
                SetNextFramebufferResolution();  
            // I changes the texture filtering mode  
            else if (key == 'i' || key == 'I')
            {
                rs.linearFiltering = !rs.linearFiltering;
                ApplyTextureFilter();
            } 
            // M toggles mipmaps on and off 
            else if (key == 'm' || key == 'M')
            {
                rs.mipmapping = !rs.mipmapping;
                ApplyTextureFilter();
            } 
            // Z changes the camera distance
            else if (key == 'z' || key == 'Z')
            {
                rs.cameraDistance -= 5.0f;
                if (rs.cameraDistance < 5.0f)
                    rs.cameraDistance = 60.0f;
            }  
            // Y changes the camera rotation
            else if (key == 'y' || key == 'Y')
            {
                rs.cameraYaw += 30.0f;
                if (rs.cameraYaw > 90.0f)
                    rs.cameraYaw = -90.0f;
            }  
            // H changes the shader type
            else if (key == 'h' || key == 'H')
            {
                rs.shaderType++;
                if (rs.shaderType == NUM_SHADER_TYPES)
                    rs.shaderType = FIRST_SHADER_TYPE;
            }       
            // L makes the camera look towards/away from the scene
            else if (key == 'l' || key == 'L')
                rs.cameraLookAway = !rs.cameraLookAway;  
            // X changes the model type
            else if (key == 'x' || key == 'X')
            {
                rs.modelType++;
                if (rs.modelType == NUM_MODEL_TYPES)
                    rs.modelType = FIRST_MODEL_TYPE;
            } 
            // P turns 4x multisampling on/off
            else if (key == 'p' || key == 'P')
            {
                rs.multisampling = !rs.multisampling;
                ResetFramebuffer();
            } 
            // R saves a screenshot to screenshot.ppm
            else if (key == 'r' || key == 'R')
            {
                SaveScreenshot();
            }
        }
        
        // update object transforms using the frame time - this ensures animation won't appear
        // to slow down when the frame rate drops 
        rs.yaw += 0.0001f * frameTimeMicrosecs;

        // draw the scene
        Render();

        // update and display statistics
        uint32_t textRow = 0;
        char textBuf[100] = { 0 };  
        float fps = 1000000.0f/frameTimeMicrosecs;
        uint32_t draws = GetDrawCallCount(), tris = GetTriangleCount(), verts = GetVertexCount();
        uint32_t trisPerSec = tris * fps;
        ResetRenderStats();

        sprintf(textBuf, "%3.0f FPS %.1fM TRIS/SEC %.1fK TRIS/FRAME", fps, (float)trisPerSec/1000000, (float)tris/1000);
        AddText(textBuf, 0, 20*textRow++);

        if (rs.showStats)
        {
            sprintf(textBuf, "%d VERTS/FRAME", verts);
            AddText(textBuf, 0, 20*textRow++); 

            sprintf(textBuf, "+/-: OBJECT COUNT [%d]", rs.numObjects);
            AddText(textBuf, 0, 20*textRow++);             

            sprintf(textBuf, "X: OBJECT COMPLEXITY [%s]", 
                rs.modelType == CUBE_MODEL ? "12 TRIS" : 
                rs.modelType == FROG_MODEL ? "496 TRIS" :
                rs.modelType == KID_MODEL ? "5682 TRIS" :
                rs.modelType == TREX_MODEL ? "18940 TRIS" :
                "55593 TRIS");
            AddText(textBuf, 0, 20*textRow++); 

            sprintf(textBuf, "F: FRAMEBUFFER [%d X %d]", rs.fbWidth, rs.fbHeight);
            AddText(textBuf, 0, 20*textRow++);

            sprintf(textBuf, "P: MULTISAMPLING [%s]", rs.multisampling ? "ON" : "OFF");
            AddText(textBuf, 0, 20*textRow++); 

            sprintf(textBuf, "H: SHADER [%s]", GetShaderNameByType((ShaderType)rs.shaderType));
            AddText(textBuf, 0, 20*textRow++);  

            sprintf(textBuf, "I: TEXTURE FILTER [%s]", rs.linearFiltering ? "LINEAR" : "NEAREST");
            AddText(textBuf, 0, 20*textRow++);   

            sprintf(textBuf, "M: MIPMAPS [%s]", rs.mipmapping ? "ON" : "OFF");
            AddText(textBuf, 0, 20*textRow++);   

            sprintf(textBuf, "C: BACKFACE CULLING [%s]", rs.backFaceCull ? "ON" : "OFF");
            AddText(textBuf, 0, 20*textRow++);

            sprintf(textBuf, "D: DEPTH TEST [%s]", rs.depthTest ? "ON" : "OFF");
            AddText(textBuf, 0, 20*textRow++);       

            sprintf(textBuf, "W: WIREFRAME [%s]", rs.renderLines ? "ON" : "OFF");
            AddText(textBuf, 0, 20*textRow++);    

            sprintf(textBuf, "Z: CAMERA DISTANCE [%d]", (int)rs.cameraDistance);
            AddText(textBuf, 0, 20*textRow++);   

            sprintf(textBuf, "Y: CAMERA YAW [%d]", (int)rs.cameraYaw);
            AddText(textBuf, 0, 20*textRow++); 

            sprintf(textBuf, "L: CAMERA LOOK [%s]", rs.cameraLookAway ? "AWAY" : "TOWARDS");
            AddText(textBuf, 0, 20*textRow++);      

            AddText("S: STATS DISPLAY [ON]", 0, 20*textRow++);

            AddText("ESC: EXIT", 0, 20*textRow++);   
        }

        RenderText();
        SwapBuffers();
    }

    printf("Cleaning up\n");
    FreeModel();
    DeInitFont();
    DeinitPlatform();
    return 0;
}
