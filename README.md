# Rmbedded_Linux_mCOS
Education use to learn the real time OS and scheduling theory

- For Lab1&2
```
- handle interrupt : (1) clock tick ISR (2) context SW ISR
> OSTimeDly() -> OS_TASK_SW() , one process give up the CPU
> OSIntExit() -> ISR exit, will check high priority task ready and context switch
  > if high priority task ready, preemption here.
>> TCB_Init() : initialze TCB structure after OS_init
> OSTimeTick -> decrease the execution counter
```

- For Lab3
```
1. add deadline into TCB.
2. need to add deadline with period.
3. need to compare the deadline number on OS_xxx
```