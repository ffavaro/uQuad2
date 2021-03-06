/**
 ******************************************************************************
 *
 * @file       control_altura.h
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


#ifndef CONTROL_ALT_H
#define CONTROL_ALT_H

#include <stdlib.h>


#define CONTROL_ALT_BUFF_SIZE		2 //usar numeros pares
#define ALT_SAMPLE_TIME			0.05 //en segundos

#define INITIAL_ALT			0

#define CONTROL_ALT_MEAN_ENABLE		0 // Realiza promedio antes de derivar. Ver control_alt_derivate_error()
#define CONTROL_ALT_ADD_ZERO		0 // Agrega un cero al filtro de u. Ver control_alt_filter_input() //No USAR!

#define TAKEOFF_ALTITUDE		1 //1m
#define LANDING_ALTITUDE		0.20 //20cm
#define PORCENTAGE_UPDATE_TA		0.05 //5%

typedef struct error_alt {
	double error;
	struct timeval ts;
} error_alt_t;


/*
 * Inicializa el buffer
 */
void control_alt_init_error_buff(void);


/*
 * Agrega un elemento nuevo al buffer y elimina el ultimo.
 */
int control_alt_add_error_buff(error_alt_t new_err);


/*
 * Calcula la derivada discreta del error.
 *
 * Puede hacer promedio opt=1 o saltear muestras opt=0
 */
double control_alt_derivate_error(void);


/*
 * Calcula la senal de error.
 *
 * Control PD
 */
double control_alt_calc_input(double alt_d, double alt_measured);

double control_alt_integral(double alt_d, double alt_measured); 

void set_alt_zero(double alt_measured);

double get_alt_zero(void);

int control_altitude_takeoff(double *h_d);
int control_altitude_land(double *h_d);

#endif // CONTROL_ALT_H



