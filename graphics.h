#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_opengl.h>
#include <allegro5/allegro_image.h>
#include <GL/glu.h>

extern GLfloat* vertices;
extern GLuint* indices;
extern GLuint indicesCount;

void InitializeAllegro(ALLEGRO_DISPLAY** display, ALLEGRO_EVENT_QUEUE** eventQueue, ALLEGRO_TIMER** timer, const float fps);

void LoadHeightmapFromFile(const char* fileName, int* bitmapWidth, int* bitmapHeight);

void CreateIndices(int width, int height);

void SetupCamera();

void Render(GLfloat angle);

int Run();
