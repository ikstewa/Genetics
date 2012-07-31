//-----------------------------------------------------------------------------
// ShaderLibrary.h
//
// Author:Ian Stewart
// 08/2008
//
//-----------------------------------------------------------------------------

#ifndef __SHADER_LIBRARY_H__
#define __SHADER_LIBRARY_H__

#include "Shader.h"

#define ENABLE_SHADERS 1
#define SHADER_LIB "./data/shaders/"


enum ShaderType
{
    // placeholder, do not use
    SHADER_TEST = 0,
    // Toon shader
    SHADER_TOON_COLOR,
    SHADER_TOON_TEXTURE,
    // Cloud Shader
    SHADER_CLOUD,
    // Multiple Render Targets
    //SHADER_MRT,
    SHADER_EDGE,
    SHADER_GRADIENT,

    // Fixed pipeline functionality
    SHADER_FIXED
};

class ShaderLibrary
{
public:
    static ShaderLibrary* getInstance();

    bool attachShader(ShaderType t);

    Shader* shader[SHADER_FIXED+1];

protected:
    ShaderLibrary();
    ShaderLibrary(const ShaderLibrary&);
    ShaderLibrary& operator=(const ShaderLibrary&);

    void init();
    Shader* ShaderLibrary::loadShader(ShaderType t);

private:
    static ShaderLibrary* inst;

};



#endif