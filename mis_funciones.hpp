/**
* @file mis_funciones.hpp
* @version 1.0
* @date 18/7/24
* @author Ramos Leonardo
*/
#ifndef MIS_FUNCIONES_H
#define MIS_FUNCIONES_H

#include "mbed.h"
#include <cstdint>
#include <string.h>
#include "arm_book_lib.h"

#define ROW_LEN 4                         ///< Cantidad de filas del keypad.
#define COL_LEN 4                         ///< Cantidad de columnas del keypad.
#define SEQUENCE_LEN 4                    ///< Largo de la secuencia ingresada.
#define ACCESS_SEQUENCE "ABCD"            ///< Secuencia de acceso guardada en memoria.
#define USERNAME "Leonardo\n"             ///< Usuario asociado a la secuencia de acceso en memoria.
#define TIMEOUT 5000                      ///< Tiempo de timeout en ms
#define BLINKING_TIME 500                 ///< Tiempo de parpadeo (ms) del led de funcionamiento del sistema

/*Defino prototipos de funciones*/
void keypadInit();
void uartInit();
void keypadSweepUpdate();
char getKeyPressed();
void displayAccessGrantedMsg();
void displayAccessDeniedMsg();
void displayTimeoutMsg();
bool areEqual();
void clearSequence();

#endif // MIS_FUNCIONES_H