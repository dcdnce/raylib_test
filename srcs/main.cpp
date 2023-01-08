#include "main.hpp"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "FastNoiseLite.h"
#include "Render.hpp"
#include "gui.hpp"

#include <map>
#include <string>
#include <sstream>
#include <iterator>

void	checkEvents(void) {
	if (IsKeyPressed(KEY_ENTER) && (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)))
	{
		if (IsWindowFullscreen())
			SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
		else
			SetWindowSize(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()));
		ToggleFullscreen();
	}
}

Camera3D cameraConfiguration(Render r) {
	Camera3D camera = {0};

	camera.position = r.render[r._height*r._width-1];
	camera.position.y = (float)-(r._width*r._scl);
	camera.target = r.render[5*r._height+r._width/2];
	camera.up = (Vector3){0,-1,0};
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;
	SetCameraMode(camera, CAMERA_FREE);
	return (camera);
}

void drawTerrain(Render & r) {
	Vector3	v[4];
	for (int y = 0 ; y < r._height - 1; y++) {
		for (int x = 0 ; x < r._width - 1; x++) {
			v[0] = r.render[y*r._height+x];
			v[1] = r.render[y*r._height+x+1];
			v[2] = r.render[(y+1)*r._height+x];
			v[3] = r.render[(y+1)*r._height+x+1];
			DrawTriangleStrip3D(v, 4, \
				r.whichBiome(r.elevationMap[y*r._height+x], r.moistureMap[y*r._height+x]));
		}
	}
}

int main(void)
{
    // Initialization
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "noiseTerrain");
    SetTargetFPS(60);

	// Init both FastNoiseObject and both vector<float> noise map.
	Render	r;
	
	// Init the two FastNoiseLite objects.
	r.refreshNoises();
	// Compute elevationNoise values to elevationMap -- with terrain-like tweaks.
	r.storeElevationNoise();
	// moistureNoise values assignation to moistureMap;
	r.storeMoistureNoise();
	// elevationMap values assignation to rendered vectors.
	r.elevationToRender();
	
	// Camera configuration
	Camera3D camera = cameraConfiguration(r);
	
	std::map<std::string, float> guiValues = Gui::init(r);

    // Main loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		UpdateCamera(&camera);
		checkEvents();	
		BeginDrawing();
			ClearBackground(SKYBLUE);
			BeginMode3D(camera);
				drawTerrain(r);	
			EndMode3D();
			// Gui::render(guiValues, r);
			// Gui::refresh(guiValues, r);
		EndDrawing();
    }

    CloseWindow();        // Close window and OpenGL context
    return (0);
}

