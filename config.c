#include "config.h"
#include "utility.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* GetStringValueFromConfig(const char* section, const char* variable, char* defaultValue)
{
    /* Dziwny blad ktory zawieszal program - Juz go nie ma */
    if(strlen(section) == 5)
        return defaultValue;

    /* Liczba linii */
    int lineNumber = 0;
    FILE* lineNumberConfigFile = fopen("config.cfg", "r");
    if(!lineNumberConfigFile)
    {
        ErrorOutput("Nie mozna otworzyc pliku [GetStringValueFromConfig]!");
        return defaultValue;
    }
    char* l = (char*)malloc(128*sizeof(char));
    while(fgets(l, 128, lineNumberConfigFile))
        lineNumber++;
    free(l);
    fclose(lineNumberConfigFile);

    FILE* configFile = fopen("config.cfg", "r");

    /* Szukaj linii [section] */
    char line[128];
    int sectionFound = 0;
    int currentLine = 1;

    char* sectionToFind = (char*)malloc((3+strlen(section))*sizeof(char));
    strcpy(sectionToFind, "[");
    strcat(sectionToFind, section);
    strcat(sectionToFind, "]\n");

    while(fgets(line, 128, configFile))
    {
        if(!sectionFound && strcmp(line, sectionToFind) == 0)
        {
            free(sectionToFind);
            sectionFound = 1;
        }
        else if(sectionFound)
        {
            /* Szukaj zmiennej variable, az do napotkania kolejnej sekcji lub konca pliku */
            char *sectionStart, *sectionEnd;
            /* Znaleziono nastepna sekcje przed znalezieniem zmiennej */
            if((sectionStart = strstr(line, "[")) != 0 && (sectionEnd = strstr(line, "]")) != 0) /*Found sth similar to section */
            {
                /* Nie znaleziono podanej zmiennej w sekcji (Cos poszlo nie tak!) */
                return defaultValue;
            }
            char* substring;
            /* -> jezeli znajdziesz: pobierz wartosc (variable=VALUE;) */
            if((substring = strstr(line, variable)) != 0)
            {
                /* width=1024;\n (size=12, varsize=5)
                 * positionof = 0;
                 * substr[12-(0+5+2)] = substr[5]; <- 1024 + '\0'
                 * memcpy(substr, &line[(0+5+1)], 12-(0+5+3)) = memcpy(substr, &line[6], 4)
                 * substr[12-(0+5+2)-1] = substr[4]

                 * back_color=RGBA(255,255,255,255); (size=33, varsize=10) END
                 * positionof = 0;
                 * substr[33-(0+10+1)] = substr[22]; <- RGBA(255,255,255,255) + '\0'
                 * memcpy(substr, &line[(0+10+1)], 33-(0+10+2)) = memcpy(substr, &line[11], 21)
                 * substr[33-(0+10+2)] = substr[22] */
                if(currentLine == lineNumber)
                {
                    int positionOf = substring - line;
                    char* substr = (char*)malloc((strlen(line)-(positionOf+strlen(variable)+1))*sizeof(char)); /* o 1 pole mniej odejmujemy dlatego, ze pomijamy = oraz ; (wyci¹gamy sam¹ wartosc), ale musimy zostawic dodatkowe miejsce dla przerwania */
                    memcpy(substr, &line[positionOf+strlen(variable)+1], strlen(line)-(positionOf+strlen(variable)+2)); /* o 2 pola mniej -||- =, ; (nie ma znaku nowej linii) */
                    substr[strlen(line)-(positionOf+strlen(variable)+2)] = '\0'; /* Rozmiar tablicy - 1 */
                    return substr;
                }
                int positionOf = substring - line;
                char* substr = (char*)malloc((strlen(line)-positionOf-strlen(variable)-2)*sizeof(char)); /* o 2 pola mniej odejmujemy dlatego, ze pomijamy = oraz ; (wyci¹gamy sam¹ wartosc) */
                memcpy(substr, &line[positionOf+strlen(variable)+1], strlen(line)-positionOf-strlen(variable)-3); /* o 3 pola mniej -||- =, ; oraz znak nowej linii \n */
                substr[(strlen(line)-positionOf-strlen(variable)-2)-1] = '\0'; /* Rozmiar tablicy - 1 */
                return substr;
            }
        }
        currentLine++;
    }
    if(!sectionFound)
    {
        /* Nie znaleziono podanej sekcji (Cos poszlo nie tak!) */
        return defaultValue;
    }

    if(sectionToFind)
        free(sectionToFind);
    fclose(configFile);

    return defaultValue;
}
