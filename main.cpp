/**
* @file main.cpp
* @version 1.0
* @date 18/7/24
* @author Ramos Leonardo
*/
#include "mbed.h"
#include <cstdint>
#include <string.h>
#include "arm_book_lib.h"
#include "mis_funciones.hpp"

// main() runs in its own thread in the OS
BusOut keypadRows(PC_6, PC_7, PC_8, PC_9); ///< Declaración de GPIOs asociados a las filas del keypad.
BusIn keypadCols(PC_0, PC_1, PC_2, PC_3);  ///< Declaración de GPIOs asociados a las Columnas del keypad.
DigitalOut workingLed(PA_5);               ///< Declaración de GPIO asociado a LED que indica funcionamiento del sistema.
DigitalOut accessGrantedLed(PA_6);         ///< Declaración de GPIO asociado a LED que indica acceso concedido.
DigitalOut accessDeniedLed(PA_7);          ///< Declaración de GPIO asociado a LED que indica acceso denegado.
UnbufferedSerial uartUSB(USBTX, USBRX);    ///< Declaración de UART para mostrar los mensajes del control de acceso.

bool sequenceDetectedState;                ///< Flag indicador de secuencia completa. Flag en true indica secuencia completa.
bool accessState;                          ///< Flag indicador de estado de acceso. Flag en true indica acceso concedido.
bool timeoutState;                         ///< Flag indicador de timeout. Flag en true indica timeout alcanzado.
bool startTimeout;
int colReading;                            ///< Respaldo del estado de las columnas del keypad luego de detectar tecla presionada.
int rowReading;                            ///< Respaldo del estado de las filas del keypad luego de detectar tecla presionada.
uint8_t currentIndex;                      ///< Indice actual de la secuencia.
uint16_t accumulatedTime;                  ///< Tiempo acumulado utilizado para comparar con tiempo de timeout.
char inputSequence[SEQUENCE_LEN + 1];      ///< Array utilizado para guardar la secuencia de caracteres ingresados.
char keypadAllowedEntries[ROW_LEN][COL_LEN] = {{'1', '2', '3', 'A'},    
                                               {'4', '5', '6', 'B'},
                                               {'7', '8', '9', 'C'},
                                               {'*', '0', '#', 'D'}}; ///< Array que contiene los caracteres asociados a las teclas del keypad.

int main() {
  /// @cond CONTENIDO DEL MAIN
  keypadInit();
  uartInit();
  workingLed = 0;
  while (true) {
    if (accumulatedTime < TIMEOUT) {
      keypadSweepUpdate();
      if (sequenceDetectedState == true) {
        if (areEqual()) {
          accessState = true;
        } else {
          accessState = false;
        }
        if (accessState == true) {
          displayAccessGrantedMsg();
          accessGrantedLed = ON;
          delay(3000);
          accessGrantedLed = OFF;
          accessState = false;
        } else {
          displayAccessDeniedMsg();
          accessDeniedLed = ON;
          delay(1000);
          accessDeniedLed = OFF;
        }
        clearSequence();
        sequenceDetectedState = false;
        timeoutState = false;
        accumulatedTime = 0;
      }
      delay(10);
      accumulatedTime += 10;
    }
    else{
        if(timeoutState == true){
            displayTimeoutMsg();
            clearSequence();
            timeoutState = false;
        } 
        accumulatedTime = 0;
    }
    if((accumulatedTime % BLINKING_TIME) == 0){
        workingLed = !workingLed;
    }
  }
  /// @endcond
}
