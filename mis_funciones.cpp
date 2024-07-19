/**
* @file mis_funciones.cpp
* @version 1.0
* @date 18/7/24
* @author Ramos Leonardo
*/
#include "mis_funciones.hpp"

/// @cond VARIABLES EXTERNAS
extern BusOut keypadRows;                         
extern BusIn keypadCols;                         
extern DigitalOut workingLed;                     
extern DigitalOut accessGrantedLed;              
extern DigitalOut accessDeniedLed;               
extern UnbufferedSerial uartUSB;                 

extern bool sequenceDetectedState;                      
extern bool accessState;                                
extern bool timeoutState;                               
extern int colReading;                                
extern int rowReading;                                  
extern uint8_t currentIndex;                            
extern uint16_t accumulatedTime;                        
extern char inputSequence[SEQUENCE_LEN + 1];            
extern char keypadAllowedEntries[ROW_LEN][COL_LEN];    
/// @endcond

/**
 * @brief keypadInit Inicializa el keypad 4x4 para su uso. Principalmente
 *        configura los GPIO de columnas como input PULL DOWN y da valores iniciales
 *        a ciertos flags. 
 */
void keypadInit() {
  keypadCols.mode(PullDown);
  inputSequence[SEQUENCE_LEN] = '\0';
  sequenceDetectedState = false;
  currentIndex = 0;
  accumulatedTime = 0;
}

/**
 * @brief uartInit Configura velocidad de transmisión y formato de la trama
 *        de datos de UART
 */
void uartInit() {
  uartUSB.baud(9600);
  uartUSB.format(/*bits*/ 8,
                 /*paridad*/ SerialBase::None,
                 /*bits de stop*/ 1);
}

/**
 * @brief keypadSweepUpdate Barre el keypad 4x4 hasta encontrar
 *        una tecla presionada, guarda el caracter asociado
 *        a esa tecla e indica mediante flag sequenceDetectedState
 *        que se ha ingresado la secuencia completa
 */
void keypadSweepUpdate() {
  for (uint8_t i = 0; i < ROW_LEN; i++) {
    keypadRows = (1 << i);
    if (keypadCols != 0) {
      delay(100);
      if (keypadCols != 0) {
        timeoutState = true;
        accumulatedTime = 0;
        rowReading = keypadRows.read();
        colReading = keypadCols.read();
        inputSequence[currentIndex] = getKeyPressed();
        currentIndex++;
        if (currentIndex >= SEQUENCE_LEN) {
          uartUSB.write("\n", 1);  
          sequenceDetectedState = true;
        }
        i = ROW_LEN;
      }
    }
  }
  keypadRows = 0;
  return;
}

/**
 * @brief getKeyPressed Realiza la conversion de posicion en keypad
 *        (fila y columna) a caracter asociado
 * @return Caracter asociado a la tecla presionada
 */
char getKeyPressed() {
  uint8_t currentRow;
  uint8_t currentCol;
  for (uint8_t i = 0; i < ROW_LEN; i++) {
    if ((rowReading & (1 << i)) != 0) {
      currentRow = i;
      i = ROW_LEN;
    }
  }
  for (uint8_t i = 0; i < COL_LEN; i++) {
    if ((colReading & (1 << i)) != 0) {
      currentCol = i;
      i = COL_LEN;
    }
  }
  uartUSB.write("*", 1);
  return keypadAllowedEntries[currentRow][currentCol];
}

/**
 * @brief areEqual Compara igualdad elemento a elemento entre el array
 *        de la secuencia ingresada y el string de usuario guardado en memoria
 * @return true si son iguales, false si no lo son.
 */
bool areEqual(){
    bool equalState = true;
    if(strcmp(inputSequence, ACCESS_SEQUENCE) != 0){
        equalState = false;
    }
    return equalState; 
}

/**
 * @brief displayAccessGrantedMsg Muestra mensajes asociados a un acceso concedido en terminal
 */
void displayAccessGrantedMsg() {
  uartUSB.write("Acceso concedido\n", 17);
  uartUSB.write("Usuario:", 8);
  uartUSB.write(USERNAME, strlen(USERNAME));
  uartUSB.write("Clave:", 6);
  uartUSB.write(ACCESS_SEQUENCE, strlen(ACCESS_SEQUENCE));
  uartUSB.write("\n", 1);
}

/**
 * @brief displayAccessDeniedMsg Muestra mensajes asociados a un acceso denegado en terminal
 */
void displayAccessDeniedMsg() {
  uartUSB.write("Acceso denegado\n", 16);
  uartUSB.write("Usuario desconocido\n", 20);
  uartUSB.write("Clave:", 6);
  uartUSB.write(inputSequence, SEQUENCE_LEN);
  uartUSB.write("\n", 1);
}

/**
 * @brief displayTimeoutMsg Muestra mensaje de timeout en terminal
 */
void displayTimeoutMsg(){
    uartUSB.write("\nTimeout!!!\n", 12);
}

/**
 * @brief clearSequence Llena de ceros el array que recibe la secuencia
          desde el keypad y reinicia su índice.
 */
void clearSequence(){
    for(uint8_t i = 0; i < SEQUENCE_LEN; i++){
        inputSequence[i] = '\0';
    }
    currentIndex = 0;
}
