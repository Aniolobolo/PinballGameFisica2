#ifndef __MODULEFONTS_H__
#define __MODULEFONTS_H__

#include "Module.h"
#include "raylib.h"
#include <string>

class ModuleFonts : public Module
{
public:
    ModuleFonts(Application* app);
    virtual ~ModuleFonts();

    bool LoadFontTexture(const std::string& file_path, char first_character, int character_size);

    void DrawText(int x, int y, const std::string& text, const Color& col = WHITE) const;

    bool CleanUp();

private:
    void DrawCharacter(int x, int y, char c, const Color& col = WHITE) const;

    char first_character;
    int character_size;
    int columns;
    int rows;
    Texture2D font_texture;
};
#endif // __MODULEFONTS_H__