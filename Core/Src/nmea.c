#include "nmea.h"

uint8_t ParseGPRMC(uint8_t *src, GPRMC_t *GPRMC)
{
    uint8_t i = 0;
    uint8_t cont = 0;
    const char s[3] = ",*";
    char *token;

    uint8_t aux[NMEA0183_MAX_LENGTH + 2];

    for(i = 0; i < NMEA0183_MAX_LENGTH; i++)
    {
        aux[i] = src[i];
    }
    i = 0;

    // En este punto dest ya debería contener una línea parseable
    while (aux[i] != ',') i++; // 1st ,
    i++;
    while (aux[i] != ',') i++; // After time ,
    i++;

    if (aux[i] == 'A') // Here 'A' Indicates the data is valid, and 'V' indicates invalid data
    { 
        token = strtok_new(aux, s);
        token = strtok_new(NULL, s);

        while (token != NULL)
        {
            switch (cont)
            {
            case 0: // UTC of pos (HHMMSS.SS)
                sscanf(token, "%2d%2d%f", &GPRMC->hour, &GPRMC->minute, &GPRMC->second);
                break;
            case 1: // Validity (A/V)
                sscanf(token, "%c", &GPRMC->valid);
                break;
            case 2: // lat (DDmm.mm)
                sscanf(token, "%2d%2d.%5d", &GPRMC->latdeg, &GPRMC->Hlatmin, &GPRMC->Llatmin);
                break;
            case 3: // lat dir (S/N)
                sscanf(token, "%c", &GPRMC->latdir);
                break;
            case 4: // long (DDDmm.mm)
                sscanf(token, "%3d%2d.%5d", &GPRMC->longdeg, &GPRMC->Hlongmin, &GPRMC->Llongmin);
                break;
            case 5: // long dir (W/E)
                sscanf(token, "%c", &GPRMC->longdir);
                break;
            case 6: // speed (knots) (x.x)
                sscanf(token, "%f", &GPRMC->speed);
                break;
            case 7: // track true (?) (x.x) d
                sscanf(token, "%f", &GPRMC->tracktrue);
                break;
            case 8: // date (DDMMYY)
                sscanf(token, "%2d%2d%2d", &GPRMC->day, &GPRMC->month, &GPRMC->year);
                break;
            case 9: // mag var (deg) (x.x) d
                sscanf(token, "%f", &GPRMC->magvar);
                break;
            case 10: // mag var dir (E/W) (sub/add) d
                sscanf(token, "%c", &GPRMC->magvardir);
                break;
            case 11: // Mode
                sscanf(token, "%c", &GPRMC->mode);
                break;
            case 12: // Checksum
                sscanf(token, "%2x", &GPRMC->checksum);
                break;
            }

            token = strtok_new(NULL, s);

            cont++;
        }

        return VALID;
    }
    else
    {
        return INVALID;
    }
}

uint8_t CalculateChecksum(uint8_t *src)
{
    uint8_t i = 1;
    uint8_t checksum = 0;

    while (src[i] != '*')
    {
        checksum ^= src[i];
        i++;
    }

    return checksum;
}

void BuildGPSCoord(int16_t deg, uint8_t min, uint32_t min2, uint8_t dir, uint8_t *ret) {
    if(dir == 'S' || dir == 'W') deg *= -1;
    sprintf(ret, "%d%02d%05d", deg, min, min2);
}


//taken from https://stackoverflow.com/questions/26522583/c-strtok-skips-second-token-or-consecutive-delimiter
char *strtok_new(char *string, char const *delimiter)
{
    static char *source = NULL;
    char *p, *riturn = 0;
    if (string != NULL)
        source = string;
    if (source == NULL)
        return NULL;

    if ((p = strpbrk(source, delimiter)) != NULL)
    {
        *p = 0;
        riturn = source;
        source = ++p;
    }
    else if (*source)
    {
        riturn = source;
        source = NULL;
    }
    return riturn;
}
