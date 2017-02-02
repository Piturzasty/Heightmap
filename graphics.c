#include "graphics.h"
#include "app.h"

GLfloat* vertices = NULL;
GLuint* indices = NULL;
GLuint indicesCount = 0;

void InitializeAllegro(ALLEGRO_DISPLAY** display, ALLEGRO_EVENT_QUEUE** eventQueue, ALLEGRO_TIMER** timer, const float fps)
{
	/* Inicjalizacja allegro oraz allegro_image */
	al_init();
	al_init_image_addon(); /* Do ladowania bitmapy */

	/* Ustawiamy ta flage po to, aby wymusic stworzenie kontekstu OpenGL dla okna */
	al_set_new_display_flags(ALLEGRO_OPENGL);
	/* Wymaga synchronizacji pionowej - Ale w przypadku jej braku okno zostanie normalnie utworzone */
	al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);

	/* Tworzymy okno o wymiarach [screenWidth x screenHeight] (szerokosc x wysokosc )*/
	*display = al_create_display(g_width, g_height);
	/* Tworzymy kolejke zdarzen */
	*eventQueue = al_create_event_queue();
	/* Tworzymy timer o szybkosci, ktora jest odwrotnoscia klatek na sekunde porzadanej */
	*timer = al_create_timer(1.0f / fps);

	/* Rejestrujemy zrodla zdarzen (skad jakiekolwiek zdarzenia beda pobierane przez allegro) */
	al_register_event_source(*eventQueue, al_get_display_event_source(*display));
	al_register_event_source(*eventQueue, al_get_timer_event_source(*timer));
}

void LoadHeightmapFromFile(const char* fileName, int* bitmapWidth, int* bitmapHeight)
{
	/* zaladuj bitmape z pliku */
	ALLEGRO_BITMAP* heightmapBMP = al_load_bitmap(fileName);
	/* Zmienna przechowujaca pixel pobrany w petli z bitmapy */
	ALLEGRO_COLOR heightPixel;

	int x = 0, z = 0, i = 0;		/* Zmienne pomocnicze (iteratory po x,z i przesuniecie w tablicy) */
	GLfloat scale = g_gridScale;	/* Ogolna skala mapy (rozszerzanie mapy) */
	GLfloat heightScale = 255.0f;	/* Skala wysokosci (przez ta skale zostaje przemnozona wysokosc) */
	GLfloat height = 0.0f;			/* Aktualna wysokosc obliczana w petli */
	GLubyte r, g, b;				/* Pomocnicze zmienne wyluskujace kolor z heightPixel */

	/* Zablokujemy zaladowana bitmape aby byla mozliwa wylacznie do odczytu
	 * (Odczyt jest szybszy) */
	al_lock_bitmap(heightmapBMP, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);

	/* Pobierz wymiary bitmapy (by stworzyc plaszczyzne o wymiarach jak plik wejsciowy) */
	*bitmapWidth = al_get_bitmap_width(heightmapBMP);
	*bitmapHeight = al_get_bitmap_height(heightmapBMP);

	/* Srodek */
	GLfloat centerWidth = (GLfloat)((*bitmapWidth) / 2.0f) * scale;
	GLfloat centerHeight = (GLfloat)((*bitmapHeight) / 2.0f) * scale;

	/* Tworzymy tablice wierzcholkow (O rozmiarze width x height x 3 (3 bo (x, y, z))) */
	vertices = (GLfloat*)malloc(*bitmapHeight * *bitmapWidth * 3 * sizeof(GLfloat));

	for (z = 0; z < *bitmapHeight; z++)
	{
		for (x = 0; x < *bitmapWidth; x++)
		{
			/* Pobierz kolor z aktualnej pozycji */
			heightPixel = al_get_pixel(heightmapBMP, x, z);
			/* Przekaz kolor z heightPixel do 3 zmiennych r, g, b */
			al_unmap_rgb(heightPixel, &r, &g, &b);
			/* Wysokosc = wartosc w przedziale 0...1 * heightScale */
			height = (GLfloat)((float)(r) / 255.0f) * heightScale;

			/* Kolejno wrzucamy do tablicy wierzcholkow x, y, z */
			vertices[i++] = (GLfloat)((x * scale) - centerWidth);
			vertices[i++] = height;
			vertices[i++] = (GLfloat)((z * scale) - centerHeight);
		}
	}
	/* Odblokuj bitmape i ja zniszcz (nie bedzie juz nam potrzbna) */
	al_unlock_bitmap(heightmapBMP);
	al_destroy_bitmap(heightmapBMP);
}

void CreateIndices(int width, int height)
{
	/* Zmienna pomocnicza */
	GLuint i = 0; 
	/* Ilosc punktow */
	size_t pointCount = (width - 1) * (height - 1) * 6; 
	/* Ilosc kwadratow (wewnatrz bitmapy: 3x3 sa 4 kwadraty - ale zeby zapobiec
	 * przenoszeniu z prawej strony do nowego wiersza z lewej dodajemy po 1 kwadracie z prawej
	 * strony by nie przenosic) */
	GLuint rectangleCount = ((width - 1) * (height - 1)) + (height - 1);

	/* Utworz tablice przechowujaca punkty o rozmiarze pointCount */
	indices = (GLuint*)malloc(pointCount * sizeof(GLuint));

	/* zmienna pomocnicza zliczajaca przesuniecie w prawo po utworzeniu kwadratu */
	int w = 0;
	for (i = 0; i < rectangleCount; i++)
	{
		/* jezeli doszlismy do ostatniego kwadratu */
		if (w == (width - 1))
		{
			/* Zeruj width i przejdz do nowego kwadratu */
			w = 0;
			continue;
		}
		else
		{
			/* width = 3
			 * height = 3
			 * 
			 * pointCount = 24 <- (3-1)*(3-1)*6 [ilosc kwadratow w srodku * 2 trojkaty po 3 wierzcholki]
			 * rectangleCount = 6 <- (3-1)*(3-1) + 2
			 * 
			 * 6---7---8...
			 * |\  |\  |...
			 * | 3 | 4 |...
			 * |  \|  \|...
			 * 3---4---5...
			 * |\  |\  |...
			 * | 0 | 1 |...
			 * |  \|  \|...
			 * 0---1---2...
			 *
			 * points = [0] {0, 1, 3}, {1, 4, 3}
			 *			[1] {1, 2, 4}, {2, 5, 4}
			 *			[2] ten kwadrat jest pominiety
			 *			[3] {3, 4, 6}, {4, 7, 6}
			 *			[4] {4, 5, 7}, {5, 8, 7}
			 *			[5] ten kwadrat jest pominiety
			 *
			 */
			/* Pierwszy trojkat */
			indices[indicesCount++] = (i);
			indices[indicesCount++] = (i + 1);
			indices[indicesCount++] = (i + width);
			/* Drugi trojkat */
			indices[indicesCount++] = (i + 1);
			indices[indicesCount++] = (i + width + 1);
			indices[indicesCount++] = (i + width);
			w++;
		}
	}
}

void SetupCamera()
{
	/* Zacznij operowac na macierzy projekcji */
	glMatrixMode(GL_PROJECTION);
	/* Zaladuj do macierzy macierz identycznosciowa */
	glLoadIdentity();
	/* Funkcja tworzaca perspektywe (frustum) */
	gluPerspective(45.0, (GLfloat)g_width / (GLfloat)g_height, 1.0, 5000.0);
	/* Obraca domyslnie o 30 stopni wzgledem osi OX*/
	glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
	/* Przesun "kamere" na pozycje (0, -500, -650) */
	glTranslatef(0.0, -500.0f, -650.0f);
}

void Render(GLfloat angle)
{
	/* Czysc bufory */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/* Wyczysc do koloru g_clearColor */
	al_clear_to_color(al_map_rgb(g_clearColor.Red*(255.0f), g_clearColor.Green*(255.0f), g_clearColor.Blue*(255.0f)));
	/* Zacznij operowac na macierzy modelu*/
	glMatrixMode(GL_MODELVIEW);
	/* Zaladuj macierz identycznosciowa do macierzy modelu */
	glLoadIdentity();
	/* obroc model wzgledem osi OY o kat angle */
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	/* Ogolem rzecz biora funkcje
	 * 1) Aktywuja tablice wierzcholkow zeby moc je rysowac
	 * 2) podpinaja tablice z wierzcholkami (o przesunieciu 3)
	 * 3) rysuja indeksy odpowiednio podpiete pod wierzcholki
	 * 4) Dezaktywuja tablice wierzcholkow
	 * 5) Podmienia obraz
	 */
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, indices);
	glDisableClientState(GL_VERTEX_ARRAY);
	glFlush();
}
