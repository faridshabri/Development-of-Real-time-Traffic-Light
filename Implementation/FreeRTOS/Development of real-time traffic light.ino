#include <Arduino.h>

// Traffic Light based on density of pedestrian to reduce waiting time for car
// Include Arduino FreeRTOS library
#include <Arduino_FreeRTOS.h>
#include <FreeRTOSConfig.h>

// Include semaphore support
#include <semphr.h>

int pedestrian[4]={1,2,3,4} ;
int pedestrian1[4]={1,2,3,4} ;






//Function Declaration
void Task1(void *pvParameters);
void Task2(void *pvParameters);
void Go_East_west();
void Yellow_Ready();
void Go_Pedestrian();
//void Pedestrian();

//Task & Queue Handle
TaskHandle_t TaskHandle_1;
TaskHandle_t TaskHandle_2;
//QueueHandle_t Pedestrian_QueueA;
//QueueHandle_t Pedestrian_QueueB;

// Time
int tgn = 5000, tgw = 2500, tcross = 10000; // In miliseconds



void setup() 
{

  // initialize serial communication at 500000 bits per second:
  Serial.begin(500000);
  
  pinMode(11, OUTPUT);//red for vehicle
  pinMode(10, OUTPUT);//yellow for vehicle
  pinMode(9, OUTPUT);//green for vehicle
  pinMode(5, OUTPUT);//green for pedestrian
  pinMode(4, OUTPUT);//red for pedestrian
  pinMode(2, INPUT); // button interrupt for pedestrian
  attachInterrupt(0,InterruptHandler,RISING); // Interrupt will occcur if the button from low to high

  //Task Creation
  xTaskCreate(Task1,// Task function
              "Vehicle Traffic Light",// Task name
              128,// Stack size 
              NULL,
              2,// Priority
              &TaskHandle_1);

  xTaskCreate(Task2,// Task function
              "Pedestrian Traffic Light",// Task name
              128,// Stack size 
              NULL,
              1,// Priority
              &TaskHandle_2);

  // Queue Creation
 
  




vTaskStartScheduler(); 

}


// Vehicle Traffic Light Task
void Task1(void *pvParameters)
{ 
(void) pvParameters;
  for(;;)
  {  
      Go_East_west();
      vTaskDelay(tgn / portTICK_PERIOD_MS);
  }
}

// Pedestrian Traffic Light Task
void Task2(void *pvParameters)
{ 
(void) pvParameters;
 
  for(;;)
  {       
    vTaskSuspend(TaskHandle_2);
    //Serial.println("Task 2");    
      if(pedestrian[1]!=0 && pedestrian1[1]==0)
      {
          Yellow_Ready();
          vTaskDelay(1000/ portTICK_PERIOD_MS);
          Go_Pedestrian();
          Serial.println("Task 2A");
          vTaskDelay(1000/ portTICK_PERIOD_MS);
      }

      if(pedestrian[1]!=0 && pedestrian1[1]!=0)
      {
          Yellow_Ready();
          vTaskDelay(1000/ portTICK_PERIOD_MS);
          Go_Pedestrian();
          Serial.println("Task 2B");
          vTaskDelay(5000/ portTICK_PERIOD_MS);
      }

     
  }
}

// Interrupt Service Routine
void InterruptHandler() 
{
  //after interrupt Task 2 has higher priority than Task 1
 Serial.println("button pressed");
 vTaskPrioritySet( TaskHandle_1, 2 +1 );// Priority Task 1 = 3
 vTaskPrioritySet( TaskHandle_2, 1+ 3 );// Priority Task 2 = 4
 vTaskResume(TaskHandle_2);

}

void Go_East_west()
{ Serial.println("GO");
	// turn off all leds
  digitalWrite(4,LOW);
  digitalWrite(10,LOW);
	
	// turn ped led to red
	digitalWrite(4,HIGH);

	// turn on green for east to west
	digitalWrite(9,HIGH);
}

void Yellow_Ready()
{ Serial.println("YELLOW");
  // turn off green for east to west
	digitalWrite(9,LOW);

  // maintain ped led to red
	digitalWrite(4,HIGH);

  // turn on yellow for east to west
	digitalWrite(10,HIGH);

}

void Go_Pedestrian()
{ Serial.println("STOP");
	// turn off all leds
  digitalWrite(4,LOW);
  digitalWrite(9,LOW);
  digitalWrite(10,LOW);
	
	// turn on red for east to west vehicle
	digitalWrite(11,HIGH);
  // turn ped led to green
	digitalWrite(5,HIGH);
}



void loop() {}

