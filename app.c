#include "app.h"
#include "utility.h"
#include "config.h"
#include "graphics.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_opengl.h>
#include <allegro5/allegro_image.h>
#include <GL/glu.h>

char* APP_VERSION = "0.9";

int g_showHelp = 0;
int g_createNewConfiguration = 0;
int g_interactiveDialogue = 0;
char g_configFileName[128] = "config.cfg";
char g_bitmapFileName[128] = "1.jpg";
int g_width = 800;
int g_height = 600;
float g_gridScale = 5.0f;
int g_renderMode = 0;
Color g_clearColor = { 0, 0, 0, 255 };

/*
 * => Jezeli natomiast jest -n/--new_configuration to reszta jest zbedna wiec pomijamy wpisywanie i szukanie.
 */

#define ARGUMENTS 18
const char g_arguments[ARGUMENTS][20] =
{
    "-h", "--help"
    "-n", "--new_configuration",
    "-c", "--config",
    "-f", "--file",
    "-ww", "--window_width",
    "-wh", "--window_height",
    "-gs", "--grid_scale",
    "--wireframe", "--solid",
    "-clear", "--clear_color",
};

/* Funkcja zwraca 0 w przypadku wystapienia jakiegos dodatkowego argumentu w momencie potrzeby parametru
 * Zwraca 1 w przypadku braku niepowodzen na tym etapie */
int ValidArgument(int arg, char** argv, int requiredArgs)
{
    int i, j;
    for(i = 1; i <= requiredArgs; i++) for(j = 0; j < ARGUMENTS; j++)
        if(strcmp(argv[arg+i], g_arguments[j]) == 0)
            return 0;
    return 1;
}

void ParseArguments(int argc, char** argv)
{
    int i;
    for(i = 0; i < argc; i++) {
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
            g_showHelp = 1;
        else if(strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--new_configuration") == 0) {
            g_createNewConfiguration = 1;
            g_interactiveDialogue = 1;
        }
        else if(strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--config") == 0) { if(argc > i+1) {
            if(ValidArgument(i, argv, 1) == 0) ErrorOutput("[--config] Nastepny argument zawiera inny argument (czy nie powinien zawierac nazwy pliku?)!");
            else {
                strcpy(g_configFileName, argv[++i]);
            } }
            else ErrorOutput("[--config] Potrzebny jest dodatkowy argument!");
        }
        else if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) { if(argc > i+1) {
            if(ValidArgument(i, argv, 1) == 0) ErrorOutput("[--file] Nastepny argument zawiera inny argument (czy nie powinien zawierac nazwy pliku?)!");
            else {
                strcpy(g_bitmapFileName, argv[++i]);
				g_createNewConfiguration = 1; /* Zapisz nowe dane do pliku */
            } }
            else ErrorOutput("[--file] Potrzebny jest dodatkowy argument!");
        }
        else if((strcmp(argv[i], "-ww") == 0 || strcmp(argv[i], "--window_width") == 0) && g_interactiveDialogue == 0) { if(argc > i+1) {
            if(ValidArgument(i, argv, 1) == 0) ErrorOutput("[--window_width] Nastepny argument zawiera inny argument (czy nie powinien zawierac szerokosci okna?)!");
            else {
                g_width = abs(atoi(argv[++i]));
                g_createNewConfiguration = 1; /* Zapisz nowe dane do pliku */
            } }
            else ErrorOutput("[--window_width] Potrzebny jest dodatkowy argument!");
        }
        else if((strcmp(argv[i], "-wh") == 0 || strcmp(argv[i], "--window_height") == 0) && g_interactiveDialogue == 0) { if(argc > i+1) {
            if(ValidArgument(i, argv, 1) == 0) ErrorOutput("[--window_height] Nastepny argument zawiera inny argument (czy nie powinien zawierac wysokosci okna?)!");
            else {
                g_height = abs(atoi(argv[++i]));
                g_createNewConfiguration = 1; /* Zapisz nowe dane do pliku */
            } }
            else ErrorOutput("[--window_height] Potrzebny jest dodatkowy argument!");
        }
        else if((strcmp(argv[i], "-gs") == 0 || strcmp(argv[i], "--grid_scale") == 0) && g_interactiveDialogue == 0) { if(argc > i+1) {
            if(ValidArgument(i, argv, 1) == 0) ErrorOutput("[--grid_scale] Nastepny argument zawiera inny argument (czy nie powinien zawierac skali siatki?)!");
            else {
                g_gridScale = abs(atoi(argv[++i]));
                g_createNewConfiguration = 1; /* Zapisz nowe dane do pliku */
            } }
            else ErrorOutput("[--grid_scale] Potrzebny jest dodatkowy argument!");
        }
        else if(strcmp(argv[i], "--wireframe") == 0) {
            g_renderMode = 0;
			g_createNewConfiguration = 1; /* Zapisz nowe dane do pliku */
		}
        else if(strcmp(argv[i], "--solid") == 0) {
            g_renderMode = 1;
			g_createNewConfiguration = 1; /* Zapisz nowe dane do pliku */
		}
        else if((strcmp(argv[i], "-clear") == 0 || strcmp(argv[i], "--clear_color") == 0) && g_interactiveDialogue == 0) { if(argc > i+3) {
            if(ValidArgument(i, argv, 3) == 0) ErrorOutput("[--clear_color] Nastepne argumenty zawieraja inne argumenty (czy nie powinien zawierac koloru tla?)!");
            else {
                g_clearColor.Red = abs(atoi(argv[++i]));
                g_clearColor.Green = abs(atoi(argv[++i]));
                g_clearColor.Blue = abs(atoi(argv[++i]));
                g_createNewConfiguration = 1; /* Zapisz nowe dane do pliku */
            } }
            else ErrorOutput("[--clear_color] Potrzebne sa dodatkowe argumenty (R, G, B)!");
        }
    }
}

int ParseConfigurationFile()
{
    char* configHeader = (char*)malloc((30+strlen(APP_VERSION)+2)*sizeof(char));
    strcpy(configHeader, "[application config --version=");
    strcat(configHeader, APP_VERSION);
    strcat(configHeader, "]\n");

    /* Naglowek musi byc zgodny */
    if(strcmp(GetLineFromFile(g_configFileName, 1), configHeader) != 0)
    {
        ErrorOutput("Naglowek sie nie zgadza :<... Byc moze wersja aplikacji jest nowsza, badz plik jest wadliwy!");
        return -1;
    }
    free(configHeader);

    const char* cpy1 = (const char*)GetStringValueFromConfig("application", "bitmap", "1.jpg");
    strcpy(g_bitmapFileName, cpy1);
    g_bitmapFileName[strlen(cpy1)] = '\0';

    g_width = atoi(GetStringValueFromConfig("window", "width", "800"));
    g_height = atoi(GetStringValueFromConfig("window", "height", "600"));

    g_gridScale = (float)atof(GetStringValueFromConfig("application", "scale", "5.0"));

    g_renderMode = atoi(GetStringValueFromConfig("renderer", "renderMode", "0"));

    float* rgba = MakeRGBAFromString(GetStringValueFromConfig("renderer", "clearColor", "RGBA(0,0,0,255)"));

    ClampFloat(0.0f, &rgba[0], 1.0f);
    ClampFloat(0.0f, &rgba[1], 1.0f);
    ClampFloat(0.0f, &rgba[2], 1.0f);
    ClampFloat(0.0f, &rgba[3], 1.0f);

    g_clearColor.Red = rgba[0];
    g_clearColor.Green = rgba[1];
    g_clearColor.Blue = rgba[2];
    g_clearColor.Alpha = rgba[3];

    free(rgba);

    return 0;
}

int NewConfigurationDialogue()
{
    if(g_interactiveDialogue == 1)
    {
        printf("Wyglada na to ze nie masz niezbednego pliku konfiguracyjnego! :<\n(Lub przypadkiem chcesz zmodyfikowac go przez aplikacje)\nUtworze go dla Ciebie!\n");
        printf("1. Podaj tytul wejsciowej bitmapy: "); fflush(stdin); scanf("%s", g_bitmapFileName);
        printf("2. Podaj skale siatki (odleglosci punktow od siebie) <0;50>: "); fflush(stdin); scanf("%f", &g_gridScale);
        ClampFloat(0, &g_gridScale, 50);
        printf("3. Podaj szerokosc okna: "); fflush(stdin); scanf("%i", &g_width);
        printf("4. Podaj wysokosc okna: "); fflush(stdin); scanf("%i", &g_height);
        int R, G, B;
        printf("5. Podaj Kolor RGB Tla [<0-255> Skladnia: R [SPACJA] G [SPACJA] B]: "); scanf("%i %i %i", &R, &G, &B);
        ClampInt(0, &R, 255);
        ClampInt(0, &G, 255);
        ClampInt(0, &B, 255);
        g_clearColor.Red = (float)((float)R*(1/255.0));
        g_clearColor.Green = (float)((float)G*(1/255.0));
        g_clearColor.Blue = (float)((float)B*(1/255.0));
        printf("6. Tryb rysowania siatki? [0/1]: "); fflush(stdin); scanf("%i", &g_renderMode);
        ClampInt(0, &g_renderMode, 1);
    }

    FILE* file = fopen(g_configFileName, "w");

    /* Sprawdz czy plik otwarto */
    if(!file)
    {
        ErrorOutput("Nie mozna otworzyc pliku konfiguracyjnego :<!");
        return -1;
    }

    /* Wprowadz konkretna strukture. */
    fprintf(file, "[application config --version=%s]\n", APP_VERSION);
    fprintf(file, "[application]\n");
    fprintf(file, "bitmap=%s;\n", g_bitmapFileName);
    fprintf(file, "scale=%f;\n", g_gridScale);
    fprintf(file, "[window]\n");
    fprintf(file, "width=%i;\n", g_width);
    fprintf(file, "height=%i;\n", g_height);
    fprintf(file, "[renderer]\n");
    /* Stworz zmienna przechowujaca kolor RGBA(255,255,255,255) */
    char* rgba = (char*)malloc(21*sizeof(char));
    strcpy(rgba, "RGBA(");
    strcat(rgba, MakeInt3CharsAlways((int)(g_clearColor.Red*255.0f)));
    strcat(rgba, ",");
    strcat(rgba, MakeInt3CharsAlways((int)(g_clearColor.Green*255.0f)));
    strcat(rgba, ",");
    strcat(rgba, MakeInt3CharsAlways((int)(g_clearColor.Blue*255.0f)));
    strcat(rgba, ",");
    strcat(rgba, MakeInt3CharsAlways((int)(g_clearColor.Alpha*255.0f)));
    strcat(rgba, ")");
    fprintf(file, "clearColor=%s;\n", rgba);
    free(rgba);
    fprintf(file, "renderMode=%i;\n", g_renderMode);

    /* Zakoncz */
    fclose(file);

    return 0;
}

int PrintHelp()
{
	char line[128];
	FILE* f = fopen("help", "r");
	if(f) while(fgets(line, 128, f)) printf("%s", line);
	fclose(f);
    getc(stdin);
    return 0;
}

int Run()
{
	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_EVENT_QUEUE* eventQueue = NULL;
	ALLEGRO_TIMER* timer = NULL;

	const GLfloat FPS = 60.0f;
	GLfloat angle = 0.0f;

	int width = 0, height = 0;
	bool drawing = false;

	InitializeAllegro(&display, &eventQueue, &timer, FPS);
	LoadHeightmapFromFile(g_bitmapFileName, &width, &height);

	CreateIndices(width, height);

	/* Wyswietlaj siatke */
	if(g_renderMode == 0)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	al_start_timer(timer);
	SetupCamera();

	ALLEGRO_EVENT event;
	while (event.type != ALLEGRO_EVENT_DISPLAY_CLOSE)
	{
		al_wait_for_event(eventQueue, &event);
		if (event.type == ALLEGRO_EVENT_TIMER)
			drawing = true;
		if (drawing == true && al_event_queue_is_empty(eventQueue))
		{
			drawing = !drawing;
			angle += 0.5f;
			if (angle > 360.0f) angle -= 360.0f; /* Wracamy do stanu poczatkowego */
			Render(angle);
			al_flip_display();
		}
	}

	al_stop_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(eventQueue);
	al_destroy_timer(timer);
	return 0;
}
