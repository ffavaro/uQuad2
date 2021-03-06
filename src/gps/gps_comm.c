/**
 ******************************************************************************
 *
 * @file       gps_comm.c
 * @author     Federico Favaro, Joaquin Berrutti y Lucas Falkenstein
 * @brief      Implementa la comunicacion con el gps a traves de gpsd.
 * @see        ??
 *
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/> or write to the 
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h> //para speed_t (BAUDRATE)
#include <math.h>

#include "gps_comm.h"
#include <serial_comm.h>
#include <quadcop_config.h>
#include <uquad_aux_time.h>
#include <control_yaw.h> // Para YAW_SAMPLE_TIME

//defines para etapa de preconfig
#define DEVICE			"/dev/ttyUSB0"
#define GPS_UPDATE_10HZ 	"$PMTK220,100*2F\r\n"
#define GPS_BAUD_57600	 	"$PMTK251,57600*2C\r\n"

//#define BAUD_9600		B9600
//#define BAUD_57600		B57600

char* hostName = "localhost";
char* hostPort = "1234";     // default port

struct gps_data_t my_gps_data;

int preconfigure_gps(void)
{
//printf("entro preconfigure\n");
   int ret;
   int fd_gps;

   //abro puerto serie del gps
   fd_gps = open_port(DEVICE);
   if (fd_gps < 0) return -1;
   //fputs("GPS conectado\n",stderr); //dbg
   sleep_ms(10);

   //configuro al baudrate inicial: 9600
   ret = configure_port_gps(fd_gps, B9600);
   if (ret < 0) return -1;
   sleep_ms(10);      
   
   //cambio baudrate del gps a 57600
   ret = gps_send_command(fd_gps, GPS_BAUD_57600);     
   if (ret < 0) return -1;
   sleep_ms(50);

   //cambio baudrate del gps a 57600                                                    
//   ret = gps_send_command(fd_gps, GPS_BAUD_57600);     
//   if (ret < 0) return -1;                             
//   sleep_ms(50); 


   //configuro nuevo baudrate: 57600
   ret = configure_port_gps(fd_gps, B57600);
   if (ret < 0) return -1;
   sleep_ms(10);

   //cambio frecuencia datos a: 10Hz
   ret = gps_send_command(fd_gps, GPS_UPDATE_10HZ);     
   if (ret < 0) return -1;
   sleep_ms(10);

   //cambio frecuencia datos a: 10Hz                   
   sleep_ms(5);                                        
   ret = gps_send_command(fd_gps, GPS_UPDATE_10HZ);    
   if (ret < 0) return -1;                             
   sleep_ms(10); 

   //cierro puerto serie, a partir de ahora gpsd se encarga de recibir los datos
   ret = close(fd_gps);
   if(ret < 0)
   {
	err_log_stderr("Failed to close serial port!");
        return -1;
   }                                                             
   //fputs("GPS desconectado\n",stderr); //dbg

   return 0;
}


int init_gps(void)
{                                                                                       
   int ret;
   int child_pid;
   child_pid = start_gpsd();
   if (child_pid < 0)
      return -1;
   
   ret = gps_open(hostName, hostPort, &my_gps_data);
   errno = 0;
   if(ret < 0)
   {
      err_log_num("gps_open fallo con error: ",errno);
      return -1;
   }
  
   (void) gps_stream(&my_gps_data, WATCH_ENABLE | WATCH_JSON, NULL);

   return child_pid;
}


int deinit_gps(void)
{
   int ret = 0;
   /* When you are done... */
   (void)gps_stream(&my_gps_data, WATCH_DISABLE, NULL);
   (void)gps_close(&my_gps_data);

   ret = system(KILL_GPSD);
   if (ret < 0)
   {
      err_log("Failed to kill gpsd!");
      return -1;
   }

   return 0;
}


int start_gpsd(void)
{
   //Forks main program and starts client
   int child_pid = fork();  

   //-- -- -- El child ejecuta el siguiente codigo -- -- --
   if (child_pid == 0)
   {
      int retval;
      //starts sbus daemon
      retval = execl(START_GPSD_PATH, "gpsd",START_GPSD_DEV,"-S",START_GPSD_PORT, "-N", (char*)0);

      //only get here if execl failed 
      if(retval < 0)
      {
         err_log_stderr("Failed to run gpsd (execl)!");
         return -1;
      }
   }

   //-- -- -- El parent (main) ejecuta el siguiente codigo -- -- --
   
   sleep_ms(100);
   return child_pid;

}


int get_gps_data(gps_t* gps)
{
   /* Put this in a loop with a call to a high resolution sleep () in it. */
   if (gps_waiting(&my_gps_data, 1000)) {
      errno = 0;
      if (gps_read(&my_gps_data) == -1)
      {
         //que hago si falla...
         err_log("No se pudo leer datos del gps");
         return -1;

      } else {
         /* Display data from the GPS receiver. */
         //if (gps_data.set & ...
            gps->latitude	= my_gps_data.fix.latitude;
            gps->longitude	= my_gps_data.fix.longitude;
            gps->altitude	= my_gps_data.fix.altitude;
            gps->speed		= my_gps_data.fix.speed;
            gps->track		= my_gps_data.fix.track;
      }
   } 
#if DEBUG
   else err_log("No entro a gps_waiting() ... ???");
#endif
   return 0;
}

/**
 * Funciones para pre configurar el gps (baudrate y tasa de envio de datos)
 */

int gps_connect(const char *device, int baud)
{
    char str[128];
    int retval;
    int fd=0;

    fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(fd < 0)
    {
	fputs("open()\n", stderr);
        return -1;
    }

    retval = sprintf(str,"stty -F %s raw -echo -echoe -echok %d", device, baud);
    if(retval < 0)
    {
	fputs("sprintf()\n", stderr);
        return -1;
    }

    retval = system(str);
    if(retval != 0)
    {
	fputs("system()\n", stderr);
        return -1;
    }

    fputs("GPS conectado\n",stderr);
    
    return fd;

}


int gps_disconnect(int fd)
{
    int retval = 0;

    if(fd > 0)
    {
	retval = close(fd);
        if(retval < 0) {
           err_log("Failed to close device!");
           return -1;
        }
    } else {
        err_log("no se puede cerrar ya que fd < 0");
        return -1;
    }
 
    fputs("GPS desconectado\n",stderr);
    return retval;
}


int gps_send_command(int fd, const char *command)
{
   int retval;
   int length = 0;
   length = strlen(command);
   
   if(length < 0)
   {
      err_log("Error en strlen()");
      return -1;
   }
   retval = write(fd, command, length);
   if(retval < length)
   {
      err_log_num("write fallo con return value: ",retval); //write devuelve cant de bytes escritos
      return -1;
   }

   return 0;
}

#if SIMULATE_GPS
/**
 * Simula movimiento del quad en base a modelo fisico
 */
void gps_simulate_position(position_t* pos, velocity_t* vel, double yaw_measured, double pitch)
{

   double F_x = MASA*G*sin(-pitch)*cos(-yaw_measured); // proyeccion en x fuerza motores
   double F_y = MASA*G*sin(-pitch)*sin(-yaw_measured); // proyeccion en y fuerza motores

   int i = 0;
   for(i=0;i<5;i++) {
  
	// Fuerza de rozamiento
	double r_x = -B_ROZ*vel->x;
	double r_y = -B_ROZ*vel->y;

	// Aceleracion
	double a_x = (r_x + F_x)/MASA;
	double a_y = (r_y + F_y)/MASA;

	// Velocidad
	vel->x = vel->x + a_x*YAW_SAMPLE_TIME/5;
	vel->y = vel->y + a_y*YAW_SAMPLE_TIME/5;

	// Posicion
	pos->x = pos->x + vel->x*YAW_SAMPLE_TIME/5;
	pos->y = pos->y + vel->y*YAW_SAMPLE_TIME/5;
   }

   return;
}
#endif

