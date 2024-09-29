/* ========================================
 *
 *              ELECH410 labs
 *          FreeRTOS pandemic project
 * 
 *                   2022
 *
 *
 *      Students:
 *          Haythem     BOUGHARDAIN
 *          Ihssane     JELLAL
 *          Sacha       MEURICE
 *
 *
 *      Best score:
 *          Population counter  :   17
 *          Vaccine counter     :   102
 *          Game status         :   WIN
 *
 * ========================================
 */


#include "project.h"
#include "pandemic.h"

// RTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"



#define TASK_STACK_SIZE (1024)

// Task definitions
#define GAME_TASK_NAME  ("game_task")
#define QUAR_TASK_NAME  ("quar_task")
#define VACC_TASK_NAME  ("vacc_task")
#define MED_TASK_NAME   ("med_task")
#define LCD_TASK_NAME   ("lcd_task")


// Tasks priorities
#define GAME_PRIORITY   (20)
#define QUAR_PRIORITY   (19)
#define VACC_PRIORITY   (18)
#define MED_PRIORITY    (17)
#define LCD_PRIORITY    (16)


// Tasks delays (in ms)
#define QUAR_PERIOD     (6)
#define MED_PERIOD      (15)
#define LCD_PERIOD      (200)





// Global variables
uint8 quar_flag;
TickType_t time_cnt;


// Queues, mutex and Semaphore
QueueHandle_t clueQueue;
SemaphoreHandle_t lab_mutex;


// Task handlers
TaskHandle_t gameHandler;
TaskHandle_t quarHandler;
TaskHandle_t vaccHandler;
TaskHandle_t medHandler;
TaskHandle_t lcdHandler;


// Tasks declarations
void quarTask();
void vaccTask();
void medTask();
void lcdTask();


//Installs the RTOS interrupt handlers
static void freeRTOSInit(void);





int main(void)
{
    /* Enable global interrupts. */
    CyGlobalIntEnable;
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    LCD_Start();
    //KB_Start();
    freeRTOSInit();
    
    quar_flag = 0;
    
    
    
    // Create tasks
    xTaskCreate(gameTask, GAME_TASK_NAME, TASK_STACK_SIZE, NULL, GAME_PRIORITY, &gameHandler);
    xTaskCreate(quarTask, QUAR_TASK_NAME, TASK_STACK_SIZE, NULL, QUAR_PRIORITY, &quarHandler);
    xTaskCreate(vaccTask, VACC_TASK_NAME, TASK_STACK_SIZE, NULL, VACC_PRIORITY, &vaccHandler);
    xTaskCreate(medTask, MED_TASK_NAME, TASK_STACK_SIZE, NULL, MED_PRIORITY, &medHandler);
    xTaskCreate(lcdTask, LCD_TASK_NAME, TASK_STACK_SIZE, NULL, LCD_PRIORITY, &lcdHandler);
    
    
    // Create queues, semaphore and mutex
    lab_mutex = xSemaphoreCreateMutex();
    clueQueue = xQueueCreate(1, sizeof(Token));
    
    // Launch freeRTOS
    vTaskStartScheduler();  
    
    for(;;)
    {
    }
}



void freeRTOSInit(void)
{
    /* Port layer functions that need to be copied into the vector table. */
    extern void xPortPendSVHandler( void );
    extern void xPortSysTickHandler( void );
    extern void vPortSVCHandler( void );
    extern cyisraddress CyRamVectors[];

	/* Install the OS Interrupt Handlers. */
	CyRamVectors[ 11 ] = ( cyisraddress ) vPortSVCHandler;
	CyRamVectors[ 14 ] = ( cyisraddress ) xPortPendSVHandler;
	CyRamVectors[ 15 ] = ( cyisraddress ) xPortSysTickHandler;
}




/** 
  *     Wait for a clue in the queue and check if enough time is available.
  *     If yes, produce the vaccine.
  */
void vaccTask()
{
    Token clue;
    Token result;
    
    for (;;) {
        
        xQueueReceive(clueQueue, &clue, portMAX_DELAY);
        
        if (xTaskGetTickCount() - time_cnt < 500) {
            xSemaphoreTake(lab_mutex, portMAX_DELAY);
            
            GPIOJ12_Write(1);
            result = assignMissionToLab(clue);
            GPIOJ12_Write(0);
            
            xSemaphoreGive(lab_mutex);
            shipVaccine(result);
        }
    }
}





/** 
  *     Produce medicines when nothing else is being done.
  */
void medTask()
{
    Token result;
    
    for (;;) {
        xSemaphoreTake(lab_mutex, portMAX_DELAY);
        
        GPIOJ13_Write(1);
        result = assignMissionToLab(0);
        GPIOJ13_Write(0);
        
        xSemaphoreGive(lab_mutex);
        shipMedicine(result);
        vTaskDelay(MED_PERIOD);
    }
}




/** 
  *     Quarantine the population when a contamination is released.
  */
void quarTask()
{
    for (;;) {
        
        vTaskDelay(QUAR_PERIOD);
        
        if (quar_flag) {
            
            GPIOJ14_Write(1);
            quarantine();
            GPIOJ14_Write(0);
            
            quar_flag = 0;
        }
    }
}





// When a contamination occurs, gameTask calls this function.
void releaseContamination(void) {
    quar_flag = 1;
}


// Function called when gameTask releases a clue.
void releaseClue(Token clue) {
    
    time_cnt = xTaskGetTickCount();
    
    GPIOJ11_Write(1);
    xQueueSend(clueQueue, &clue, portMAX_DELAY);
    GPIOJ11_Write(0);
}







/** 
  *     Manage the display on the LCD screen.
  */
void lcdTask()
{
    for (;;)
    {
        LCD_ClearDisplay();
        
        LCD_PrintNumber(getPopulationCntr());
        LCD_PutChar(' ');
        LCD_PrintNumber(getVaccineCntr());
        LCD_PutChar(' ');
        LCD_PrintNumber(getMedicineCntr());
        
        vTaskDelay(LCD_PERIOD);
    }
}








/* [] END OF FILE */
