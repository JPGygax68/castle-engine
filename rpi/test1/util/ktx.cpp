#include <stdio.h>
#include <stdlib.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

struct KTXHeader 
{
    uint8_t identifier[12];
    uint32_t endianness;
    uint32_t glType;
    uint32_t glTypeSize;
    uint32_t glFormat;
    uint32_t glInternalFormat;
    uint32_t glBaseInternalFormat;
    uint32_t pixelWidth;
    uint32_t pixelHeight;
    uint32_t pixelDepth; 
    uint32_t numberOfArrayElements;
    uint32_t numberOfFaces;
    uint32_t numberOfMipmapLevels;
    uint32_t bytesOfKeyValueData;
}; 

GLuint LoadKTX(const char* filename)
{
    FILE* pFile = fopen(filename, "rb");
    if (pFile == NULL)
    {
        printf("Couldn't open %s\n", filename);
        return 0;
    }    

    KTXHeader header;
    if (fread(&header, 1, sizeof(KTXHeader), pFile) != sizeof(KTXHeader))
    {
        fclose(pFile);
        return 0;
    }

    if (header.glType != 0 || 
        header.glFormat != 0 || 
        header.pixelDepth != 0 || 
        header.numberOfArrayElements != 0 ||
        header.numberOfFaces != 1)
    {
        printf("%s: only compressed 2D textures are supported.\n", filename);
        fclose(pFile);
        return 0;
    }

    if (header.glInternalFormat != GL_ETC1_RGB8_OES)
    {
         printf("%s: loading untested compressed format %X, this may not work.", filename, header.glInternalFormat);
    }   

    // skip over the key/value data
    fseek(pFile, header.bytesOfKeyValueData, SEEK_CUR);

    // create an Open GL texture from the raw image data
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    uint32_t width = header.pixelWidth;
    uint32_t height = header.pixelHeight;

    // load the mipmaps
    for (int mipLevel = 0; mipLevel < header.numberOfMipmapLevels; mipLevel++)
    {
        uint32_t dataSize;
        if (fread(&dataSize, 1, sizeof(uint32_t), pFile) != sizeof(uint32_t))
        {
            fclose(pFile);
            return 0;
        }

        GLbyte* pBits = (GLbyte*)malloc(sizeof(GLbyte)*dataSize);
        if (fread(pBits, 1, dataSize, pFile) != dataSize)
        {
            free(pBits);
            fclose(pFile);
            return 0;
        }

        glCompressedTexImage2D(GL_TEXTURE_2D, mipLevel, header.glInternalFormat, width, height, 0, dataSize, pBits);
        free(pBits);

        width = width == 1 ? 1 : width / 2;
        height = height == 1 ? 1 : height / 2;

        uint32_t padSize = ((dataSize + 3) / 4) * 4 - dataSize;
        if (padSize != 0)
        {
            char padBytes[3];
            if (fread(padBytes, 1, padSize, pFile) != padSize)
            {           
                fclose(pFile);
                return 0;
            }
        }
    }

    fclose(pFile);   

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT /*GL_CLAMP_TO_EDGE*/);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT /*GL_CLAMP_TO_EDGE*/);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}