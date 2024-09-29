# ELEC-H-410 Project - Pandemic, the RTOS game
The present project has been completed for the real-time systems course in Q2 2022.

Keywords: RTOS, real-time systems, tasks management, code design, C, GPIO, lgic analyzer, PSoC

### Introduction

The aim of this project is to beat a pandemic game using FreeRTOS tasks management. The game simulates a real-time pandemic situation where the population decreases as the contaminations grow. To defeat the game, the objective is to produce as fast as possible enough vaccines before the population’s counter reaches 0%. If this counter reaches 0%, the game is over.

The project is composed of one research lab. The lab can either work on a vaccine, or produce medicines. The difficulty in this game is that the lab cannot work on both tasks at the same time. Furthermore, once the lab is working on a task, it sould work on it until completion.

## Specifications

You should only write your code in main.c. You may use as many task as you want as long as their priority is lower than the gameTask (i.e. gameTask has a priority of 20, so your tasks cannot have a priority higher than 19). Write a task that print the populationCntr, vaccineCntr and medicineCntr on the left side of the display. The last 8 characters of the LCD are reserved to the gameTask.

1. **quarTask()**  
   This task is responsible to quarantine the population when a new contamination is released by the game. It checks every 6 ms a boolean flag quar_flag indicating if a contamination occurs.

2. **vaccTask()**  
   This task is waiting to receive a message in the global queue called clueQueue. When it receives a new message from the queue, it checks if it has enough remaining time to produce the vaccine.

3. **medTask()**  
   This task first waits for the 'lab_mutex' to be available before executing its instructions. In the context of the game, this means that before producing medicines, the lab should be free and not producing vaccines.

Please refer to [https://uv.ulb.ac.be/mod/folder/view.php?id=975239](https://uv.ulb.ac.be/mod/folder/view.php?id=975239) for more information.

---

© All codes in this repo are the property of Sacha Meurice and its team members, and should not be copied without written permission.
