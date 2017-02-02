#include "utility.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>

void ClampInt(int min, int* variable, int max)
{
    *variable = (*variable < min) ? min : ((*variable > max) ? max : *variable);
}

void ClampFloat(float min, float* variable, float max)
{
    *variable = (*variable < min) ? min : ((*variable > max) ? max : *variable);
}

char* GetLineFromFile(const char* fileName, int line)
{
    FILE* f = fopen(fileName, "r");
    int i;
    char* output = (char*)malloc(128*sizeof(char));
    for(i = 1; i < line; i++)
        if(!fgets(output, 128, f))
            return "";
    if(fgets(output, 128, f))
        return output;
    fclose(f);
    return "";
}

float* MakeRGBAFromString(char* input)
{
    /* RGBA(Byte1, Byte2, Byte3, Byte4) */
    float* out = (float*)malloc(4*sizeof(float));

    int i = 0;
    char* t = strtok(&input[5], ", "); /* Przechodze juz do pierwszego koloru */
    while(t)
    {
        out[i] = (1/255.0)*atoi(t);
        t = strtok(NULL, ", )");
        i++;
    }
    if(i != 4)
        out[0] = out[1] = out[2] = out[3] = 1.0;
    return out;
}

char* MakeInt3CharsAlways(int number)
{
    if(number >= 999) return "999\0";
    else if(number <= 0) return "000\0";
    char* output = (char*)malloc(4*sizeof(char));
    char num[3];
	sprintf(num, "%i", number);
    if(number > 0 && number < 10)
    {
        output[0] = output[1] = '0';
        output[2] = num[0];
    }
    else if(number >= 10 && number < 100)
    {
        output[0] = '0';
        output[1] = num[0];
        output[2] = num[1];
    }
    else
        strcpy(output, num);
    output[3] = '\0';
    return output;
}

int GetNumberLength(int number)
{
    if(number == 0) return 1;
    int ret = 0;
    if(number < 0) ++ret;
    number = abs(number);
    for(; number > 0; ++ret)
        number/=10;
    return ret;
}

static const char day[7][64] = { "Poniedzialek", "Wtorek", "Sroda", "Czwartek", "Piatek", "Sobota", "Niedizela" };
static const char month[12][64] = { "Styczen", "Luty", "Marzec", "Kwiecien", "Maj", "Czerwiec", "Lipiec", "Sierpien", "Wrzesien", "Pazdziernik", "Listopad", "Grudzien" };

char* GetActualTime(int flag)
{
    time_t t = time(NULL);
    struct tm* timeInfo = localtime(&t);

    int size = 0;
    char* result;

    if(flag == 1) size = GetNumberLength(timeInfo->tm_hour) + 1 + GetNumberLength(timeInfo->tm_min) + 1 + GetNumberLength(timeInfo->tm_sec) + 1;
    else if(flag == 2) size = strlen(day[timeInfo->tm_wday]) + 1 + strlen(day[timeInfo->tm_mon]) + 1 + GetNumberLength(timeInfo->tm_mday) + 1 + 4 + 1;
    else if(flag == 3) size = strlen(day[timeInfo->tm_wday]) + 1 + strlen(day[timeInfo->tm_mon]) + 1 + GetNumberLength(timeInfo->tm_mday) + 1 + GetNumberLength(timeInfo->tm_hour) + 1 + GetNumberLength(timeInfo->tm_min) + 1 + GetNumberLength(timeInfo->tm_sec) + 1 + 4 + 1;

    result = (char*)malloc(size*sizeof(char));

    if(flag == 1) sprintf(result, "%i:%i:%i", timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
    else if(flag == 2) sprintf(result, "%s %s %d %i", day[timeInfo->tm_wday-1], month[timeInfo->tm_mon], timeInfo->tm_mday, 1900 + timeInfo->tm_year);
    else if(flag == 3) sprintf(result, "%s %s %d %i:%i:%i %i", day[timeInfo->tm_wday-1], month[timeInfo->tm_mon], timeInfo->tm_mday, timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec, 1900 + timeInfo->tm_year);

    /* automatycznie sie dodaje przez zapewne sprintf
    result[strlen(result)] = '\0'; */

    return result;
}

void Error(char* input)
{
    /* Log into stderr */
    fprintf(stderr, "%s\n", input);
    /* Log into error.log */
    FILE* errFile = fopen("error.log", "a");
    if(!errFile)
        return;
    /* Get Time (Only Time) */
    char* currentTime = GetActualTime(1);

    fprintf(errFile, "[%s] %s\n", currentTime, input);
    fclose(errFile);

    free(currentTime);
}

/* http://stackoverflow.com/questions/804288/creating-c-formatted-strings-not-printing-them */
void ErrorOutput(char* format, ...)
{
    char buffer[256];
    va_list vl;
    va_start(vl, format);
    vsnprintf(buffer, sizeof(buffer), format, vl);
    va_end(vl);
    Error(buffer);
}
