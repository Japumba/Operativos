#include "virtual_processor.h"
#include <stdio.h>
#include <math.h>

#define NUMCOLAS	6
#define QMODE_CONST	1
#define QMODE_2TTN	2

#define QVAL_CONST	1

extern struct PROCESO proceso[];
extern struct COLAPROC bloqueados;
struct COLAPROC colas[NUMCOLAS];
extern int tiempo;
extern int pars[];

int QMODE = QMODE_CONST;
int remQuantum;

// =============================================================================
// ESTE ES EL SCHEDULER DE PRIORIDADES
// ============================================================================

int scheduler(int evento)
{
    int cambia_proceso =0;
    int prox_proceso_a_ejecutar;

    prox_proceso_a_ejecutar=pars[1]; // pars[1] = proceso en ejecución

    if(evento==PROCESO_NUEVO)
    {
        // Agregar el nuevo proceso a la cola de prioridad 0
        // pars[0] es el proceso nuevo
        proceso[pars[0]].estado = LISTO;
        mete_a_cola(&colas[proceso[pars[0]].prioridad],pars[0]);
        // pars[1] es el proceso en ejecución
        if(tiempo==0) //if (pars[1] == NINGUNO)
	  cambia_proceso = 1;
    }      

    if(evento==TIMER)
    {
        printf("Llega interrupcion del Timer\n");
	if(remQuantum == 0)
	{
	    cambia_proceso = 1;
	}
	else
	    remQuantum --;
    }
                                 
    if(evento==SOLICITA_E_S)
    {
        proceso[pars[1]].estado=BLOQUEADO;
        printf("Solicita E/S Proceso %d\n",pars[1]);
	mete_a_cola(&bloqueados, pars[1]);
    }
    if(evento==TERMINA_E_S)
    {
        // Saber cual proceso terminó E/S
        // pars0 es el proceso desbloqueado
        proceso[pars[0]].estado=LISTO;
	proceso[pars[0]].prioridad = proceso[pars[0]].prioridad+1;
	mete_a_cola(&colas[proceso[pars[0]].prioridad], pars[0]);
        printf("Termina E/S Proceso desbloqueado %d\n",pars[0]);
    }
    if(evento==PROCESO_TERMINADO)
    {
        // pars0 = proceso terminado
        proceso[pars[0]].estado=TERMINADO;
        cambia_proceso=1; // Indíca que puede poner un proceso nuevo en ejecucion
    }
                                        
    if(cambia_proceso)
    {
        // Si la cola no esta vacia obtener de la cola el siguiente proceso listo
	int i=0;        
	while(i<7 && cola_vacia(colas[i]))
	    i++;
	if(i < 7)
	{
	    prox_proceso_a_ejecutar=sacar_de_cola(&colas[i]);
	    proceso[prox_proceso_a_ejecutar].estado=EJECUCION;
	    remQuantum = getQuantum(i);
	    cambia_proceso=0;
	}
        else
	{
            printf("Ho hay procesos en cola\n");
            prox_proceso_a_ejecutar=NINGUNO;
        }
    }
    return(prox_proceso_a_ejecutar);
}

int getQuantum(int priority)
{
	if(QMODE == QMODE_CONST)
		return QVAL_CONST;
	if(QMODE == QMODE_2TTN)
		return (int)pow(2, priority);
}

// =================================================================
