#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>


enum hip{0,1,2};

enum smed
{
	0,1,2
};

enum tarj
{
	0,1,2
};

enum seg
{
	0,1,2
};

enum nat
{
	0,1,2
};


struct cuenta_corriente 
{
    char * titular;
    long saldo;
    hip hipoteca;
    smed saldoMedio;
    tarj tarjeta;
    seg seguros;
    nat naturaleza;
};
