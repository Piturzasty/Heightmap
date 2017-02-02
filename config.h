#pragma once

/* Pobiera wartosc zmiennej w pliku konfiguracyjnym w danej sekcji (jako lancuch),
 * w przypadku jej braku badz bledu zostanie zwrocona wartosc domyslna (defaultValue) */
char* GetStringValueFromConfig(const char* section, const char* variable, char* defaultValue);
