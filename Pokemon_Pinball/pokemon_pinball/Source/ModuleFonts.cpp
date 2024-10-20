#include "Application.h"
#include "ModuleRender.h"
#include "ModuleFonts.h"

#include "Defs.h"
#include "Log.h"

ModuleFonts::ModuleFonts() : Module()
{
}

// Destructor
ModuleFonts::~ModuleFonts()
{}

// Called after window is available
bool ModuleFonts::Awake()
{
	bool ret = true;

    LOG("Init default font");

	const char* path = "fonts/open_sans/OpenSans-Regular.ttf";
	int size = 36;

    font = LoadFontEx(path, size, NULL, 0);

	return ret;
}

// Called before quitting
bool ModuleFonts::CleanUp()
{
	LOG("Freeing font");

    UnloadFont(font);

	return true;
}

Font ModuleFonts::GetFont() const
{
    return font;
}

// calculate the height of a font type
int ModuleFonts::GetFontHeight(Font font) const
{
    return font.baseSize;
}

// calculate size of a text
bool ModuleFonts::GetTextSize(const char* text, int& width, int& height, int spacing) const
{
	bool ret = false;

    MeasureTextEx(font, text, font.baseSize, spacing);

	return ret;
}

