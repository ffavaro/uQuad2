/**
 ******************************************************************************
 *
 * @file       futaba_sbus.h
 * @author     Federico Favaro, Joaquin Berrutti y Lucas Falkenstein
 * @brief      Implementa la codificacion del mensaje futaba sbus en
 *             base a la informacion de los diferentes canales.
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
 *
 * TODO: Basado en https://developer.mbed.org/users/Digixx/notebook/futaba-s-bus-controlled-by-mbed/
 */

#ifndef FUTABA_SBUS_h
#define FUTABA_SBUS_h

#include <quadcop_config.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define SBUS_SIGNAL_OK          0x00
#define SBUS_SIGNAL_LOST        0x01
#define SBUS_SIGNAL_FAILSAFE    0x03
#define SBUS_DATA_LENGTH    	25

// Canales de Futaba SBUS asignados
#define ROLL_CHANNEL		1
#define PITCH_CHANNEL		2
#define YAW_CHANNEL		3
#define THROTTLE_CHANNEL	6
#define FLIGHTMODE_CHANNEL	7

// Indices del buffer que almacena comandos
#define ROLL_CH_INDEX		0
#define PITCH_CH_INDEX		1
#define YAW_CH_INDEX		2
#define THROTTLE_CH_INDEX	3
#define FLIGHTMODE_CH_INDEX	4
#define FAILSAFE_CH_INDEX	5

// Valores neutrales (cero) de los comandos
#define ROLL_NEUTRAL		1500
#define PITCH_NEUTRAL		1500
#define YAW_NEUTRAL		1500
#define THROTTLE_NEUTRAL	1000

// Valores de comando para flight mode
#define FLIGHT_MODE_1		1000
#define FLIGHT_MODE_2		1500
#define FLIGHT_MODE_3		2000

// Valores maximos de los comandos
#define MAX_COMMAND		2000

// Valores minimos de los comandos
#define MIN_COMMAND		1000
#define MIN_THROTTLE		950

// Valores neutrales (cero) de los comandos
#define ROLL_NEUTRAL		1500
#define PITCH_NEUTRAL		1500
#define YAW_NEUTRAL		1500
#define THROTTLE_NEUTRAL	1000

// Valores para el armado y desarmado de la cc3d
#define THROTTLE_ARM		950
#define THROTTLE_DISARM		950
#define YAW_ARM			1000	// "yaw left"
#define YAW_DISARM		2000	// "yaw right"

#define ACTIVATE_FAILSAFE	50
#define DEACTIVATE_FAILSAFE	100

#if PC_TEST
#define START_SBUS_ARG 		"sbusd.log" //test en un PC linux
#else
#define START_SBUS_ARG		"/dev/ttyO0" //en la beagle
#endif

/** 
 * Esto se tiene que ir...no hace nada
 */
#if PC_TEST
int futaba_sbus_begin(void);
#endif //PC_TEST

/**
 * Establece la velocidad de un motor.
 * Luego de modificar la velocidad de un motor se debe llamar
 * a la funcion futaba_sbus_update_motors() para enviar la
 * informacion actualizada a los motores.
 *
 * @param channel canal correspondiente a uno de los motores
 * @param speed velocidad a aplicar al motor
 */
void futaba_sbus_set_channel(uint8_t channel, int16_t value);


/**
 * Reinicia (lleva a cero) la informacion de todos los canales.
 *
 */ 
void futaba_sbus_reset_channels(void);


/**
 * Devuelve la condicion de failsafe
 * 
 * @return condicion de failsafe
 */
uint8_t futaba_sbus_get_failsafe(void);


/**
 * Establece el estado de failsafe. 
 * TODO PROBAR, puede servir para parar los motores
 * 
 * @param fs Estado de failsafe a imponer
 */
void futaba_sbus_set_failsafe(uint8_t fs);


/**
 * Genera el mensaje sbus con la informacion de los canales.
 *
 */ 
void futaba_sbus_update_msg(void);

/**
 * Reinicia (lleva a cero) la informacion del mensaje sbus.
 *
 */ 
void futaba_sbus_reset_msg(void);

/**
 * Genera el mensaje sbus con la informacion de los canales.
 *
 * @param fd file descriptor del puerto serie
 *
 * @return codigo de error
 */ 
int futaba_sbus_write_msg(int fd);

/**
 * Inicia el demonio sbus en paralelo al main
 *
 * @return pid del child process o codigo de error
 */ 
int futaba_sbus_start_daemon(void);

#if PC_TEST
int convert_sbus_data(char* buf_str);
void print_sbus_data(void);
#endif //PC_TEST

#endif



