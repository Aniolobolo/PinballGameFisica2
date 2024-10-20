#pragma once

#include "Module.h"

class Application;

enum WindowEvent
{
    WINDOW_EVENT_QUIT = 0,
    WINDOW_EVENT_HIDE = 1,
    WINDOW_EVENT_SHOW = 2,
    WINDOW_EVENT_RESIZE = 3,
    WINDOW_EVENT_COUNT       // Max window events
};

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	void SetTitle(const char* title);

	// Retrieve window size
    uint GetWidth() const;
    uint GetHeight() const;

    // Gather relevant win events
    bool GetWindowEvent(WindowEvent ev);

private:
	uint width;
	uint height;

    bool windowEvents[WINDOW_EVENT_COUNT];
};
