#include "TextureManager.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad.h>

TextureManager* TextureManager::m_Instance = 0;

TextureManager::TextureManager() 
{
    
}

TextureManager* TextureManager::getInstance()
{
    if (m_Instance)
        return (m_Instance);
    m_Instance = new TextureManager();
    return m_Instance;
}

unsigned int    TextureManager::loadTexture(const std::string filename)
{
    unsigned int id = -1;

    if (m_textureMap.count(filename) != 0) {
        id = m_textureMap[filename];
    }
    else {
         int width, heigth, nrChannels;
        unsigned char *data = stbi_load(filename.c_str(), &width, &heigth, &nrChannels, 0);

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, heigth, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    
        m_textureMap[filename] = id;
    }
    return (id);
}

void    TextureManager::activateTexture(unsigned int level, unsigned int id)
{
    glActiveTexture(level);
    glBindTexture(GL_TEXTURE_2D, id);
}

