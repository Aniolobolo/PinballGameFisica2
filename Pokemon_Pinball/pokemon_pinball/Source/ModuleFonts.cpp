#include "Application.h"
#include "ModuleRender.h"
#include "ModuleFonts.h"
//#include "Defs.h"
//#include "Log.cpp"
#include "raylib.h"

ModuleFonts::ModuleFonts(Application* app) : Module(app), first_character(0), character_size(0), columns(0), rows(0)
{
}

ModuleFonts::~ModuleFonts()
{
    UnloadTexture(font_texture);  // Asegura que la textura se descargue al destruir el módulo
}

bool ModuleFonts::LoadFontTexture(const std::string& file_path, char first_character, int character_size)
{
    font_texture = LoadTexture(file_path.c_str());
    if (font_texture.id == 0)
    {
        LOG("Failed to load font texture");
        return false;
    }

    this->first_character = first_character;
    this->character_size = 32;  // Asegura que el tamaño de los caracteres es 32x32
    columns = font_texture.width / character_size;
    rows = font_texture.height / character_size;

    return true;
}

void ModuleFonts::DrawText(int x, int y, const std::string& text, const Color& col) const
{
    int offset_x = x;
    for (int i = 0; i < text.length(); ++i)
    {
        DrawCharacter(offset_x, y, text[i], col);
        offset_x += character_size;
    }
}

void ModuleFonts::DrawCharacter(int x, int y, char c, const Color& col) const
{
    int char_index = c - first_character;
    int coord_x = char_index % columns;
    int coord_y = char_index / columns;

    if (coord_x < 0 || coord_x >= columns || coord_y < 0 || coord_y >= rows)
    {
        LOG("Invalid character index when drawing text: (%d,%d)", coord_x, coord_y);
        return;
    }

    Rectangle srcRect = { (float)(coord_x * character_size), (float)(coord_y * character_size), 32.0f, 32.0f };
    Vector2 pos = { (float)x, (float)y };

    DrawTextureRec(font_texture, srcRect, pos, col);
}

bool ModuleFonts::CleanUp()
{
    LOG("Unloading font texture");
    UnloadTexture(font_texture);
    return true;
}
