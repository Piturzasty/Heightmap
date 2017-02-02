#pragma once

/* Funkcja obcinajaca liczbe calkowita do przedzialu <min; max> */
void ClampInt(int min, int* variable, int max);

/* Funkcja obcinajaca liczbe zmiennoprzecinkowa do przedzialu <min; max> */
void ClampFloat(float min, float* variable, float max);

/* Zwraca konkretna linie [line] z pliku "fileName" */
char* GetLineFromFile(const char* fileName, int line);

/* Zwraca ze znaku 4-elementowa tablice float'ow zawierajaca kolor z lancucha w formacie RGBA(255, 255, 255, 255) */
float* MakeRGBAFromString(char* input);

/* Zwraca 3 znakowy reprezentant liczby calkowitej
 * - W przypadku liczby > 999 zwraca 999 (w < 0 zwraca 000)
 */
char* MakeInt3CharsAlways(int number);

/* zwraca ilosc znakow w liczbie (Przykladowo 100 -> 3, 91032 -> 5) */
int GetNumberLength(int number);

/*
 * - Zwraca Date/Czas (lub wszystko razem) jako lancuch znakow
 * - @Argument flag: { 1, 2, 3 }
 * - 1 - Zbuduj tylko czas
 * - 2 - Zbuduj tylko date
 * - 3 - Zbuduj calkowita date (data + czas)
 */
char* GetActualTime(int flag);

/*
 * - Zwraca blad (ewentualnie log) do pliku "error.log" oraz na standardowe wyjscie bledu
 * - Dziala w ten sam sposob co funkcje standardowe (C-style format)
 */
void ErrorOutput(char* format, ...);
