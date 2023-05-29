#ifndef TEXTURE_MANAGER_HPP
# define TEXTURE_MANAGER_HPP

#include <iostream>
#include <map>

class TextureManager
{
public:
    static          TextureManager *getInstance();
    unsigned int    loadTexture(const std::string filename);
    void            activateTexture(unsigned int level, unsigned int id);
private:
    TextureManager();
    static TextureManager* m_Instance;
    std::map<std::string, unsigned int> m_textureMap;
};

#endif