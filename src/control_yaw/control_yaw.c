/******************************************************************************
 *
 * @file       control_yaw.h
 * @author     Federico Favaro, Joaquin Berrutti y Lucas Falkenstein
 * @brief      ??
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
 *
 */

#include <control_yaw.h>
#include <stdlib.h>



/*
 * Buffer para almacenar senales de error
 * 
 * Elemento mas nuevo se almacena el lugar correspondiente
 *  al indice cero
 */
error_yaw_t error_yaw_buff[CONTROL_YAW_BUFF_SIZE];

/*
 * Agrega un elemento nuevo al buffer y elimina el ultimo.
 */
int control_yaw_add_error_buff(error_yaw_t new_err)
{
   int i;
   // Desplazo un lugar todos los elementos
   for(i=0; i < CONTROL_YAW_BUFF_SIZE-1; i++)
   {
      error_yaw_buff[CONTROL_YAW_BUFF_SIZE-1-i] = error_yaw_buff[CONTROL_YAW_BUFF_SIZE-2-i];
   }
   // Agrego elemento nuevo en el primer lugar
   error_yaw_buff[0] = new_err;
   
   return 0;
}

/*
 * Calcula la derivada discreta del error.
 *
 * Puede hacer promedio opt=1 o saltear muestras opt=0
 */
double control_yaw_derivate_error(int8_t opt)
{
   double err_mean_sup = 0;
   double err_mean_inf = 0;
   double mean_time = YAW_SAMPLE_TIME; //TODO calcularlo usando tiempo del timestamp de las muestras (act.ts)
   
   double err_dot;

   if(opt == 1) {
	// Promedio de muestras superiores e inferiores
	int i;
	for(i=0; i < CONTROL_YAW_BUFF_SIZE/2; i++)
	{
	   err_mean_sup += error_yaw_buff[CONTROL_YAW_BUFF_SIZE-1-i].error;
	   err_mean_inf += error_yaw_buff[i].error; 
	}
        // calculo la derivada como diferencia de promedios. Los elementos mas nuevos estan en
        // los indices mas chicos
	//double err_dot = ( (err_inf - err_sup)*(CONTROL_YAW_BUFF_SIZE/2) )/( mean_time*(CONTROL_YAW_BUFF_SIZE/2) );
        err_dot = ( err_mean_inf - err_mean_sup )/ mean_time;
   } else { //opt = 0
	err_dot = ( error_yaw_buff[0].error - error_yaw_buff[CONTROL_YAW_BUFF_SIZE].error )/( mean_time*(CONTROL_YAW_BUFF_SIZE-1) );
   }
   return err_dot;
}


double control_yaw_calc_error(double yaw_d, double yaw_measured) 
{
   double u = 0;
   u = Kp*(yaw_d - yaw_measured);
#if CONTROL_YAW_ADD_DERIVATIVE
   u += Kp*Td*control_yaw_derivate_error(0);
#endif

   return u;
}


