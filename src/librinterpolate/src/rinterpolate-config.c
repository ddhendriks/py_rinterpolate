#ifdef __CONFIG__
#include <stdio.h>
#include <stdlib.h>
#include "rinterpolate.h"
#include "rinterpolate_internal.h"
static void help(void);

int main (int argc,
          char **  argv)
{
    if(argc>1)
    {
        char *c = *(argv+1);
        while(*c == '-')
        {
            c++;
        }
        if(strncmp(c,"prefix",6)==0)
        {
            printf("%s\n",PREFIX);
        }
        else if(strncmp(c,"destdir",7)==0)
        {
            printf("%s\n",DESTDIR);
        }
        else if(strncmp(c,"libs",4)==0)
        {
            printf("-L%s%s/%s\n",DESTDIR,PREFIX,"lib");
        }
        else if(strncmp(c,"cflags",6)==0)
        {
            printf("-lrinterpolate -I%s%s/%s\n",DESTDIR,PREFIX,"include");
        }
        else if(strncmp(c,"version",7)==0)
        {
            printf("%s\n",RINTERPOLATE_VERSION);
        }
        else
        {
            help();
        }
    }
    else
    {
        help();
    }
    exit(0);
}

static void help(void)
{
       printf("Usage:\n\nrinterpolate-config <flags>\n\nwhere <flags> are:\n\n--prefix : show PREFIX\n--destdir : show DESTDIR\n--libs : show linker information\n--cflags : show compiler flags\n--version : show librinterpolate version\n\n\n");
}

#endif//__CONFIG__
typedef int prevent_ISO_C_warning;
