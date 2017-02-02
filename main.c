#include "app.h"
#include "config.h"
#include "graphics.h"
#include "utility.h"

#include <stdio.h>

int main(int argc, char** argv)
{
    /* Przeszukuje argumenty */
    ParseArguments(argc, argv);
    if(g_showHelp)
        return PrintHelp();
    /* Sprawdzamy czy istnieje config.cfg (jezeli istnieje to go przetwarzamy) */
    if(g_createNewConfiguration == 1 || ParseConfigurationFile() != 0) /* Nie znaleziono lub uzytkownik chce nowej */
    {
        /* Jesli nie istnieje -> odpytujemy uzytkownika o konfiguracje (w formie konsoli)
           A takze zapisujemy nowo utworzone parametry */
        if(NewConfigurationDialogue() == -1)
            return -1;
    }

	return Run();
}
