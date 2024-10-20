#ifndef __MODULEFONTS_H__
#define __MODULEFONTS_H__

#include "Module.h"

class ModuleFonts : public Module
{
public:

	ModuleFonts();

	// Destructor
	virtual ~ModuleFonts();

	// Called before render is available
	bool Awake();

	// Called before quitting
	bool CleanUp();

    Font GetFont() const;
    int GetFontHeight(Font font) const;
	bool GetTextSize(const char* text, int& width, int& height, int spacing = 0) const;

private:

    Font font;
};


#endif // __MODULEFONTS_H__