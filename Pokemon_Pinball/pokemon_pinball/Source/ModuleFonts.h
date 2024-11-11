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

    // M�todo para inicializar la fuente personalizada
    bool LoadFontTexture(const std::string& file_path, char first_character, int character_size);

    // M�todo para dibujar texto
    void DrawText(int x, int y, const std::string& text, const Color& col = WHITE) const;

    bool CleanUp();

private:
    // M�todo para dibujar un solo car�cter
    void DrawCharacter(int x, int y, char c, const Color& col = WHITE) const;

    char first_character;
    int character_size;
    int columns;
    int rows;
    Texture2D font_texture;  // Textura de la fuente
};

#endif // __MODULEFONTS_H__