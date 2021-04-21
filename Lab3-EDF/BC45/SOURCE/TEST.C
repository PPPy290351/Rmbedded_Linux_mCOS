/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                          (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                               EXAMPLE #2
*********************************************************************************************************
*/

#include "includes.h"

/*
*********************************************************************************************************
*                                              CONSTANTS
*********************************************************************************************************
*/

#define          TASK_STK_SIZE     512                /* Size of each task's stacks (# of WORDs)       */

#define          TASK_START_ID       0                /* Application tasks IDs                         */
#define          TASK_1_ID           1
#define          TASK_2_ID           2
#define          TASK_3_ID           3
#define          PrintTask_ID        10


#define          TASK_START_PRIO    0                /* Application tasks priorities                  */
#define          TASK_1_PRIO        1
#define          TASK_2_PRIO        2
#define          TASK_3_PRIO        3
#define          PrintTask_PRIO     10

/*
*********************************************************************************************************
*                                              VARIABLES
*********************************************************************************************************
*/

OS_STK        TaskStartStk[TASK_STK_SIZE];            /* Startup    task stack                         */
OS_STK        Task1Stk[TASK_STK_SIZE];                /* Task #1    task stack                         */
OS_STK        Task2Stk[TASK_STK_SIZE];                /* Task #2    task stack                         */
OS_STK        Task3Stk[TASK_STK_SIZE];                /* Task #3    task stack                         */
OS_STK        PrintTaskStk[TASK_STK_SIZE];

char CtxSwMessage[500][30];
int CtxSwMessageTop = 0;
FILE *fptr;

OS_EVENT     *AckMbox;                                /* Message mailboxes for Tasks #4 and #5         */
OS_EVENT     *TxMbox;

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

        void  TaskStart(void *data);                  /* Function prototypes of tasks                  */
static  void  TaskStartCreateTasks(void);
static  void  TaskPrintMsg (void);
        void  Task1(void *data);
        void  Task2(void *data);
        void  Task3(void *data);
        void  PrintTask(void *data);

/*$PAGE*/
/*
*********************************************************************************************************
*                                                  MAIN
*********************************************************************************************************
*/

void main (void)
{
    OS_STK *ptos;
    OS_STK *pbos;
    INT32U  size;


    PC_DispClrScr(DISP_FGND_WHITE);                        /* Clear the screen                         */

    OSInit();                                              /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */

    PC_ElapsedInit();                                      /* Initialized elapsed time measurement     */

    ptos        = &TaskStartStk[TASK_STK_SIZE - 1];        /* TaskStart() will use Floating-Point      */
    pbos        = &TaskStartStk[0];
    size        = TASK_STK_SIZE;
    // OSTaskStkInit_FPE_x86(&ptos, &pbos, &size);            
    OSTaskCreateExt(TaskStart,
                   (void *)0,
                   ptos,
                   TASK_START_PRIO,
                   TASK_START_ID,
                   pbos,
                   size,
                   (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSStart();                                             /* Start multitasking                       */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                               STARTUP TASK
*********************************************************************************************************
*/

void  TaskStart (void *pdata)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
    INT16S     key;


    pdata = pdata;                                         /* Prevent compiler warning                 */


    OS_ENTER_CRITICAL();                                   /* Install uC/OS-II's clock tick ISR        */
    PC_VectSet(0x08, OSTickISR);
    PC_SetTickRate(OS_TICKS_PER_SEC);                      /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();

    OSStatInit();                                          /* Initialize uC/OS-II's statistics         */

    // AckMbox = OSMboxCreate((void *)0);                     /* Create 2 message mailboxes               */
    // TxMbox  = OSMboxCreate((void *)0);

    TaskStartCreateTasks();                                /* Create all other tasks                   */

    for (;;) {

        if (PC_GetKey(&key)) {                             /* See if key has been pressed              */
            if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
                PC_DOSReturn();                            /* Yes, return to DOS                       */
            }
        }
        TaskPrintMsg();
        OSCtxSwCtr = 0;                                    /* Clear context switch counter             */
        OSTimeDly(OS_TICKS_PER_SEC);                       /* Wait one second                          */
    }
}
/*$PAGE*/

/*$PAGE*/
/*
*********************************************************************************************************
*                                           Print the Msg from buffer
*********************************************************************************************************
*/

static  void  TaskPrintMsg (void)
{
    // char   s[80];
    static int index = 0;
    fptr = fopen("RESULT.TXT","a");
    if(fptr == NULL){
        exit(1);
    }else{
        for (; index < CtxSwMessageTop; index++)
        {
            printf("%s\n",CtxSwMessage[index]);
            fprintf(fptr,"%s\n",CtxSwMessage[index]);
        }
    }
    fclose(fptr);
    

    // sprintf(s, "%5d", OSTaskCtr);                                  /* Display #tasks running               */
    // PC_DispStr(18, 22, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

    
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                             CREATE TASKS
*********************************************************************************************************
*/

static  void  TaskStartCreateTasks (void)
{
    OSTaskCreateExt(Task1,
                   (void *)0,
                   &Task1Stk[TASK_STK_SIZE - 1],
                   TASK_1_PRIO,
                   TASK_1_ID,
                   &Task1Stk[0],
                   TASK_STK_SIZE,
                   (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskCreateExt(Task2,
                   (void *)0,
                   &Task2Stk[TASK_STK_SIZE - 1],
                   TASK_2_PRIO,
                   TASK_2_ID,
                   &Task2Stk[0],
                   TASK_STK_SIZE,
                   (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskCreateExt(Task3,
                   (void *)0,
                   &Task3Stk[TASK_STK_SIZE - 1],
                   TASK_3_PRIO,
                   TASK_3_ID,
                   &Task3Stk[0],
                   TASK_STK_SIZE,
                   (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                               TASK #1
*
* Description: This task executes every 100 mS and measures the time it task to perform stack checking
*              for each of the 5 application tasks.  Also, this task displays the statistics related to
*              each task's stack usage.
*********************************************************************************************************
*/

void  Task1 (void *pdata)
{
    int start ; //the start time
    int end ; //the end time
    int toDelay;
    int C = 1;
    OSTCBCur->compTime = C;
    OSTCBCur->period = 3;
    OSTCBCur->deadline = 3;
    start=OSTimeGet();
    while(1)
    {
        while(OSTCBCur->compTime>0) //C ticks
        {

        // do nothing

        }
        end=OSTimeGet() ; // end time
        toDelay=(OSTCBCur->period)-(end-start) ;
        start=start+(OSTCBCur->period) ; // next start time
        OSTCBCur->compTime=C ;// reset the counter (c ticks for computation)
        // @ deadline refresh
        OSTCBCur->deadline=start+(OSTCBCur->period);
        OSTimeDly (toDelay); // delay and wait (P-C) times
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                               TASK #2
*
* Description: This task displays a clockwise rotating wheel on the screen.
*********************************************************************************************************
*/

void  Task2 (void *data)
{
    int start ; //the start time
    int end ; //the end time
    int toDelay;
    int C = 3;
    OSTCBCur->compTime = C;
    OSTCBCur->period = 5;
    OSTCBCur->deadline = 5;
    start=OSTimeGet();
    while(1)
    {
        while(OSTCBCur->compTime>0) //C ticks
        {

        // do nothing

        }
        end=OSTimeGet() ; // end time
        toDelay=(OSTCBCur->period)-(end-start) ;
        start=start+(OSTCBCur->period) ; // next start time
        OSTCBCur->compTime=C ;// reset the counter (c ticks for computation)
        // @ deadline refresh
        OSTCBCur->deadline=start+(OSTCBCur->period);
        OSTimeDly (toDelay); // delay and wait (P-C) times
    }
}
/*$PAGE*/

/* Added by Chi-Shen */
// @ because Task3 priority is the lowest, so we only monitor this task if there is any deadline missed 
// void  Task3(void *data)
// {
//     int start ; //the start time
//     int end ; //the end time
//     int toDelay;
//     int C = 4;
//     OSTCBCur->compTime = C;
//     OSTCBCur->period = 9;
//     start=OSTimeGet();
//     while(1)
//     {
//         while(OSTCBCur->compTime>0) //C ticks
//         {
//         }
//         end=OSTimeGet() ; // end time
//         toDelay=(OSTCBCur->period)-(end-start) ;
//         start=start+(OSTCBCur->period) ; // next start time
//         if(toDelay < 0){
//             sprintf(CtxSwMessage[CtxSwMessageTop++], "Task3 deadline missed : %d", start);
//         }
//         OSTCBCur->compTime=C ;// reset the counter (c ticks for computation)
//         OSTimeDly (toDelay); // delay and wait (P-C) times
//     }
// }

