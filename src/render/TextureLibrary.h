//-----------------------------------------------------------------------------
// TextureLibrary.h
//
// Author:Ian Stewart
// 08/2008
//
// Ideas from Orange Book
//-----------------------------------------------------------------------------

#ifndef __TEXTURE_LIBRARY_H__
#define __TEXTURE_LIBRARY_H__

#include <string>

using namespace std;

//const char* TEXTURE_LIB ".//game//data//textures//";
#define TEXTURE_LIB ".//data//textures//"

#define MAX_TEXTURE_COUNT 64




enum TextureId {
    TEXTURE_NULL = 0,
    TEXTURE_CHAR_COLOR,
    TEXTURE_HUD,
	TEXTURE_DOT,
	TEXTURE_DUST,
	TEXTURE_CIRCLE_OUTLINE,
	TEXTURE_ATOM_RING,
    TEXTURE_HUD2,
    TEXTURE_MENU,
    TEXTURE_MAIN_MENU_BACK,
    TEXTURE_WIN,
    TEXTURE_LOSE,
    TEXTURE_TIMELEFT_FRAME,
	TEXTURE_JUMP_INCREASE,
	TEXTURE_TIME_INCREASE,
    TEXTURE_CELL_BRIGHTNESS,
    TEXTURE_3D_NOISE,
    TEXTURE_DISABLE
};

static const string TextureNames[TEXTURE_DISABLE] = 
{
    // External Textures
    "NULL",
    "CharColor.tga",
    "HUD.tga",
	"dot.tga",
	"dust2.tga",
	"circOutline.tga",
	"atom_ring.tga",
    "HUD_ryan.tga",
    "MENU.tga",
    "background.tga",
    "win.tga",
    "lose.tga",
    "HUD_timeleft_frame.tga",

	"jump_increase.tga",
	"time_increase.tga",

    // Internal Textures
    "Cell Brightness",
    "3D Noise"
};

struct Texture {
    unsigned glid;
    //bool alpha;
    unsigned width;
    unsigned height;
    string filename;
};

class TextureLibrary {

  public:

    static TextureLibrary* getInstance();

    //void Init();
    void Activate(unsigned id);
    void Deactivate(unsigned id);
    unsigned int Load(const char* filename);
    //bool HasAlpha(TextureId id) const { return textures[id].alpha; }

    void DisableMipmaps() { useMipmap = false; }
    void EnableMipmaps() { useMipmap = true; }

    unsigned getGLID(unsigned id) const;

  private:

    TextureLibrary();
    void SetFilter();

    Texture textures[MAX_TEXTURE_COUNT];
    bool useMipmap;

    static TextureLibrary* inst;
    //static string Textures[TEXTURE_DISABLE];

    unsigned currentTexID;
};

#endif