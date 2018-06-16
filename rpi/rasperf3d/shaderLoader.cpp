#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <GLES2/gl2.h>

#include "shaderLoader.h"

const char* shaderNames[NUM_SHADER_TYPES] = {
    "PER-PIXEL LIGHTING",
    "PER-VERTEX LIGHTING",
    "PER-PIXEL UNTEXTURED",
    "PER-VERTEX UNTEXTURED",
    "FLAT COLORED"
};

const char* shaderFilenames[NUM_SHADER_TYPES][2] = {
    { "phong.vertexShader", "phong.fragmentShader" },
    { "gouraud.vertexShader", "gouraud.fragmentShader" },
    { "phong.vertexShader", "untextured-phong.fragmentShader" },
    { "gouraud.vertexShader", "untextured-gouraud.fragmentShader" },
    { "flat.vertexShader", "flat.fragmentShader" }
};

const char* GetFileText(const char* filename)
{
    FILE* pFile = fopen(filename, "r");
    if (pFile == NULL)
    {
        printf("Failed to read shader file %s\n", filename);
        return 0;
    }

    // obtain file size:
    fseek(pFile , 0 , SEEK_END);
    uint32_t fileSize = ftell(pFile);
    rewind(pFile);

    char* pText = (char*) malloc(sizeof(char)*fileSize+1);

    if (fread(pText, 1, fileSize, pFile) != fileSize)
    {
        printf("Failed to read shader file %s\n", filename);
        fclose(pFile);
        free(pText);
        return 0;
    }

    // add null terminator to string
    pText[fileSize] = 0;

    return pText;
}

bool CompileShader(GLuint shader, const char* shaderSource)
{
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == 0)
    {
        printf("Failed to create a shader.\n");

        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        if (length > 0) 
        {
            char* log = (char*)malloc(length);
            glGetShaderInfoLog(shader, length, NULL, log);
            printf("%s\n", log);
            free(log);
        }

        return false;
    }

    return true;
}

GLint LoadShader(const char* vertexShaderFilename, const char* fragmentShaderFilename)
{
    // vertex shader
    printf("Compiling %s\n", vertexShaderFilename);
    const char* vsSource = GetFileText(vertexShaderFilename);
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    assert(CompileShader(vs, vsSource));
    free((void*)vsSource);

    // fragment shader
    printf("Compiling %s\n", fragmentShaderFilename);
    const char* fsSource = GetFileText(fragmentShaderFilename);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    assert(CompileShader(fs, fsSource));
    free((void*)fsSource);

    // shader program
    GLuint po = glCreateProgram();
    glAttachShader(po, vs);
    glAttachShader(po, fs);
    glLinkProgram(po);

    GLint status;
    glGetProgramiv(po, GL_LINK_STATUS, &status);
    if (status == 0) 
    {
        printf("Failed to link shader program.\n");

        GLint length;
        glGetProgramiv(po, GL_INFO_LOG_LENGTH, &length);
        if (length > 0) 
        {
            char* log = (char *)malloc(length);
            glGetProgramInfoLog(po, length, NULL, log);
            printf("%s\n", log);
            free(log);
        }

        return 0;
    }

    // Free shader resources
    glDeleteShader(vs);
    glDeleteShader(fs);

    return po;
}

bool LoadShaderByType(ShaderType shaderType, ShaderInfo* shaderInfo)
{
    GLint po = LoadShader(shaderFilenames[shaderType][0], shaderFilenames[shaderType][1]);    

    shaderInfo->shaderProgram = po;
    // save the locations of the program inputs
    shaderInfo->vertLoc = glGetAttribLocation( po, "vertPosition_modelspace" );
    //assert(shaderInfo->vertLoc != -1);
    shaderInfo->normalLoc = glGetAttribLocation( po, "vertNormal_modelspace" );
    //assert(shaderInfo->normalLoc != -1);
    shaderInfo->texcoordLoc = glGetAttribLocation( po, "vertTexCoord0" );
    //assert(shaderInfo->texcoordLoc != -1);
    shaderInfo->mvLoc = glGetUniformLocation( po, "mvMatrix" );
    //assert(shaderInfo->mvLoc != -1);
    shaderInfo->mvpLoc = glGetUniformLocation( po, "mvpMatrix" );
    //assert(shaderInfo->mvpLoc != -1);
    shaderInfo->lightLoc = glGetUniformLocation( po, "lightPosition_viewspace" );
    //assert(shaderInfo->lightLoc != -1);
    shaderInfo->texSamplerLoc = glGetUniformLocation( po, "textureSampler" );
    //assert(shaderInfo->texSamplerLoc != -1);

    return true;
}

const char* GetShaderNameByType(ShaderType shaderType)
{
    return shaderNames[shaderType];
}