#pragma once

typedef struct
{
	float Red;
	float Green;
	float Blue;
	float Alpha;
} Color;

/* Czy pomoc ma byc pokazana (przekazanie argumentu -h, --help) */
extern int g_showHelp;
/* Czy ma zostac utworzona nowa konfiguracja aplikacji (nowy plik .cfg) */
extern int g_createNewConfiguration;
/* Czy ma zostac utworzona nowa konfiguracja aplikacji z interakcja z uzytkownikiem (dodatkowy parametr) (nowy plik .cfg) */
extern int g_interactiveDialogue;
/* Nazwa pliku konfiguracyjnego */
extern char g_configFileName[128];
/* Nazwa pliku z bitmapa */
extern char g_bitmapFileName[128];
/* Szerokosc okna */
extern int g_width;
/* Wysokosc okna */
extern int g_height;
/* Skala siatki (odleglosci punktow od siebie) */
extern float g_gridScale;
/* Tryb rysowania siatki */
extern int g_renderMode;
/* Kolor tla */
extern Color g_clearColor;

/* Przetwarza argumenty wywolania linii polecen
 * Schemat algorytmu:
 * Przeszukaj tablice argv[] w poszukiwaniu ...
 * -> Jezeli znajdziesz -h/--help -> ustaw flage g_showHelp na 1;
 * -> Jezeli znajdziesz -n/--new_configuration -> ustaw flage g_interactiveDialogue na 1 oraz g_createNewConfiguration na 1;
 * -> Jezeli znajdziesz -c/--config -> sprawdz czy argc > i+1 -> sprawdz czy nie jest to kolejny parametr -> g_configFileName = argv[i+1]
 * -> Jezeli znajdziesz -f/--file -> sprawdz czy argc > i+1 -> sprawdz czy nie jest to kolejny parametr -> g_bitmapFileName = argv[i+1]
 * -> Jezeli znajdziesz -ww [--window_width] / -wh [--window_height] -> sprawdz czy argc > i+1 -> sprawdz czy nie jest to kolejny parametr -> g_width/g_height
 * -> Jezeli znajdziesz -gs/--grid_scale -> sprawdz czy argc > i+1 -> sprawdz czy nie jest to kolejny parametr -> g_gridScale
 * -> Jezeli znajdziesz --wireframe/--solid -> ustaw flage g_wireframeMode na 1/0;
 * -> Jezeli znajdziesz -clear/--clear_color -> sprawdz czy argc > i+3 -> sprawdz czy sa to kolejne parametry -> g_clearColor = argv[i+1], argv[i+2], argv[i+3]
 * => Jezeli natomiast jest -n/--new_configuration to reszta jest zbedna wiec pomijamy wpisywanie i szukanie.
 */
void ParseArguments(int argc, char** argv);

/* Przetwarza plik konfiguracyjny (pobiera wartosci zmiennych do programu */
int ParseConfigurationFile();

/* Tworzy nowy plik konfiguracyjny [z interakcja z uzytkownikiem] (wylacznie wtedy kiedy flaga g_interactiveDialogue ustawiona na 1) */
int NewConfigurationDialogue();

/* Wyswietla pomoc. (Nie wraca juz do main() - funkcja zwraca wylacznie 0) */
int PrintHelp();

/* Glowna funkcja uruchomieniowa (W niej zawiera sie cala glowna petla aplikacji) */
int Run();
