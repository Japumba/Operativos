#include "virtual_processor.h"
#include <stdio.h>

extern struct PROCESO proceso[];
extern struct COLAPROC listos,bloqueados;
extern int tiempo;
extern int pars[];
int current_iteration = 1;

int scheduler(int evento)
{
    int quantum =1;
    int cambia_proceso =0;
    int prox_proceso_a_ejecutar;

    prox_proceso_a_ejecutar = pars[1]; // pars[1] = proceso en ejecución

    if(evento==PROCESO_NUEVO)
    {
        // Agregar el nuevo proceso a la cola de listos
        // pars[0] es el proceso nuevo
        proceso[pars[0]].estado=LISTO;
        mete_a_cola(&listos,pars[0]);
        // pars[1] es el proceso en ejecución
        if (pars[1] == NINGUNO)
	  cambia_proceso =1;
    }      

    if(evento==TIMER)
        printf("Llega interrupcion del Timer\n");
	if(current_iteration == quantum)
	{
		cambia_proceso =1;
		current_iteration =1;
		mete_a_cola(&listos,pars[1]);
		proceso[pars[1]].estado = LISTO;
	}
	else
		current_iteration ++;
                                 
    if(evento==SOLICITA_E_S)
    {
        proceso[pars[1]].estado=BLOQUEADO;
        printf("Solicita E/S Proceso %d\n",pars[1]);
	mete_a_cola(&bloqueados,pars[1]);
	cambia_proceso = 1;
    }
    if(evento==TERMINA_E_S)
    {
        // Saber cual proceso terminó E/S
        // pars0 es el proceso desbloqueado
        proceso[pars[0]].estado=LISTO;
        printf("Termina E/S Proceso desbloqueado %d\n",pars[0]);
	sacar_de_cola(&bloqueados);
	mete_a_cola(&listos,pars[0]);
    }
    if(evento==PROCESO_TERMINADO)
    {
        // pars0 = proceso terminado
	printf("Termina el proceso %d \n",pars[0]);
        proceso[pars[0]].estado=TERMINADO;
	current_iteration = 1;
        cambia_proceso=1; // Indíca que puede poner un proceso nuevo en ejecucion
    }
                                        
    if(cambia_proceso)
    {
        // Si la cola no esta vacia obtener de la cola el siguiente proceso listo
        if(!cola_vacia(listos))
        {
            prox_proceso_a_ejecutar=sacar_de_cola(&listos);
	    proceso[prox_proceso_a_ejecutar].estado=EJECUCION;
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

// =================================================================
