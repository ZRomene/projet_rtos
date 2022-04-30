// Projet RTOS  04/2022
// Réalisé par Zaynab Romene


#include <Arduino_FreeRTOS.h>                     //pour que FreeRtos soit lancé dans le code 
#include <semphr.h>                               // inclure mutex support


const int Button1 = 2 ;                          //Déclaration des variables
const int Button2 = 3 ;                          //les deux boutons sont branchés aux pins 2 et 3
const int Potentiometer = A0;                    //le potentiometre est branché à la pin analogique A0

int ValPotentiometer = 0;
int ValButtons = 0;

struct valeurCapteursInStructure {               //nom de la stucture
int analogique;                                  //le champs de la structure
int numerique;
double tempsEnMillisecondes;
};


QueueHandle_t ValeurBoutonsQueue;                //File d’attente entre tache1 et tache3
QueueHandle_t ValeursPotentiometreQueue;         //File d’attente entre tache2 et tache3
QueueHandle_t StructureQueue;                    //File d’attente entre tache3 et tache4
QueueHandle_t NewStructureQueue;                 //File d’attente entre tache4 et tache5

SemaphoreHandle_t xSerialSemaphore;             



void setup() {
  
  Serial.begin(9600);

      
xTaskCreate(FirstTask, "Buttons's value Task", 100, NULL, 1, NULL );                      //Création de la tache 1
xTaskCreate(SecondTask, "Patentiometer's Task", 100, NULL, 1, NULL );                     //Création de la tache 1

if (StructureQueue == NULL) {
     xTaskCreate(ThirdTask, "Structure Task", 100, NULL, 1, NULL );                        //Création de la tache 3
     xTaskCreate(FourthTask, "Display Task", 100, NULL, 1, NULL );                         //Création de la tache 4

     if (NewStructureQueue == NULL) {
     xTaskCreate(FifthTask , "Conversion & final display Task", 100, NULL, 1, NULL );      //Création de la tache 5
     } 
  }
  
ValeurBoutonsQueue = xQueueCreate(5,sizeof(int32_t));
ValeursPotentiometreQueue  = xQueueCreate(5,sizeof(int32_t));
StructureQueue = xQueueCreate(3,sizeof(valeurCapteursInStructure));
NewStructureQueue = xQueueCreate(3,sizeof(valeurCapteursInStructure));

  if ( xSerialSemaphore == NULL )                                                      // Check to confirm that the Serial Semaphore has not already been created.
  {
    xSerialSemaphore = xSemaphoreCreateMutex();                                        // Create a mutex semaphore we will use to manage the Serial Port
    if ( ( xSerialSemaphore ) != NULL )
      xSemaphoreGive( ( xSerialSemaphore ) );                                          // Make the Serial Port available for use, by "Giving" the Semaphore.
  }
}

void loop() {}


//-----------------------------------------------------------------------First Task------------------------------------------------------------------------------------------//

void FirstTask(void *pvParameters) 
{
  pinMode(Potentiometer, INPUT);


  BaseType_t qStatus1;
  qStatus1 =0;
  
  while(1)
  {
      ValPotentiometer = analogRead(Potentiometer);
      qStatus1 =xQueueSend(ValeursPotentiometreQueue,&ValPotentiometer,0);
      

//       if(qStatus1 !=  pdPASS){
//
//      // See if we can obtain or "Take" the Serial Semaphore.
//      // If the semaphore is not available, wait 5 ticks of the Scheduler to see if it becomes free.
//      
//      if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )                       //Take the semaphore = Accés à la ressource commune. Le port série est réservé pour cette partie uniquement jusqu'à le libérer 
//      {
//        Serial.println("Could not send  to the queue");
//        xSemaphoreGive( xSerialSemaphore );                                                      //Give the Serial Port= libération du port série. Peut-etre maintenant utilisé par les autres 
//      }
//    }
    
       vTaskDelay(100 / portTICK_PERIOD_MS );                                                      //déclaration dun délai pour la stabilité
  }
}

//-----------------------------------------------------------------------Second Task------------------------------------------------------------------------------------------//

void SecondTask (void *pvParameters)
{
  pinMode(Button1, INPUT);
  pinMode(Button2, INPUT);
  int val1 = 0;
  int val2 = 0;
  int etat1;
  int etat2;

  BaseType_t qStatus2;
  qStatus2=0;
 
  while(1){

      etat1 = digitalRead(Button1);                          //  bouton1 = 2
      if(etat1 == HIGH)
      val1= 1;                                             // le bouton1 est relaché
      else
      val1= 0;                                            // le bouton1 est appuyé
       
      etat2 = digitalRead(Button2);                         //  bouton2 = 3
      if(etat2 == HIGH)
      val2= 1;                                           // le bouton1 est relaché
      else
      val2= 0;                                           // le bouton1 est appuyé

    ValButtons = val1 +val2 ; 
     //OU BIEN 
     // ValButtons = digitalRead(Button1)+digitalRead(Button2);  
     
    qStatus2 =xQueueSend(ValeurBoutonsQueue,&ValButtons,0);

//
//      if(qStatus2 !=  pdPASS){
//      
//          if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )                       //Take the semaphore = Accés à la ressource commune. Le port série est réservé pour cette partie uniquement jusqu'à le libérer 
//          {
//            Serial.println("Could not send  to the queue");
//            xSemaphoreGive( xSerialSemaphore );                                                      //Give the Serial Port= libération du port série. Peut-etre maintenant utilisé par les autres 
//            }
//      }

      
    vTaskDelay( 100 / portTICK_PERIOD_MS );                                                            //déclaraction dun délai pour la stabilité et pour terminer le cycle d'avant 
  }
  
}

//-----------------------------------------------------------------------Third Task------------------------------------------------------------------------------------------//

void ThirdTask (void *pvParameters)
{
  BaseType_t qStatus1; 
  BaseType_t qStatus2; 
  
  int analogReading;
  int buttonReading;
  while(1)
  {
      valeurCapteursInStructure S1 = {0,0,0.0};                                                    //initialisation des valeurs dans la structure

      qStatus1 =  xQueueReceive(ValeursPotentiometreQueue,&ValPotentiometer ,0);
      qStatus2 =  xQueueReceive(ValeurBoutonsQueue,&ValButtons ,0);


//
// if ((qStatus1  == pdPASS)&& (qStatus2  == pdPASS))
//   {
//    
//    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )                       //Take the semaphore = Accés à la ressource commune. Le port série est réservé pour cette partie uniquement jusqu'à le libérer 
//          {
//            Serial.println("Data received successfully ! ");  
//            xSemaphoreGive( xSerialSemaphore );                                                 //Give the Serial Port= libération du port série. Peut-etre maintenant utilisé par les autres 
//            }
// 
    
    S1.analogique = ValPotentiometer;                                                            //affectation de champs  
    S1.numerique = ValButtons;
    S1.tempsEnMillisecondes = millis();
    
    xQueueSend(StructureQueue,&S1,0);                                                            //envoyer le contenu de la structure S1 dans le queue StructureQueue
 
    vTaskDelay( 100 / portTICK_PERIOD_MS );

// } 
//   else
//    {
//      if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )                   //Take the semaphore = Accés à la ressource commune. Le port série est réservé pour cette partie uniquement jusqu'à le libérer 
//          {
//             Serial.println("Could not receive from queue ");
//             xSemaphoreGive( xSerialSemaphore );  
//            }    
//    };

  }
}

//-----------------------------------------------------------------------Fourth Task------------------------------------------------------------------------------------------//

void FourthTask (void *pvParameters)
{
  valeurCapteursInStructure S1;

  while (!Serial) {                                                                                  // Wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards
  vTaskDelay(0); 
  }    
      
  while(1)
  {
   xQueueReceive(StructureQueue, &S1,0); 
   if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )                            //Take the semaphore = Accés à la ressource commune. Le port série est réservé pour cette partie uniquement jusqu'à le libérer 
    {
         Serial.print("La valeur de potentiometre est : ");
         Serial.println(S1.analogique);                                                            //affichage de la valeur du potentiometre
         Serial.print("La valeur réultante des boutons est : ");
         Serial.println(S1.numerique);                                                             //affichage des valeurs des boutons
         Serial.print("Le temps en millisecondes dés l'activation de la carte est : ");
         Serial.println(S1.tempsEnMillisecondes);                                                  //affichage du temps en Millisecondes

         xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }
    xQueueSend(NewStructureQueue, &S1, 0);                                                         //envoyer le contenu de la structure S1 dans le queue StructureQueue

    vTaskDelay(100 / portTICK_PERIOD_MS );
  }
}

//-----------------------------------------------------------------------Fifth Task------------------------------------------------------------------------------------------//
 

void FifthTask (void *pvParameters)
{
  valeurCapteursInStructure S1;
        while (!Serial) {                                                                     // Wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards
        vTaskDelay(0); 
        }   
  double tempsEnMinutes;
  
  while(1)
  {
    tempsEnMinutes =0;
    tempsEnMinutes = (S1.tempsEnMillisecondes) * 0.0000167;                                     //1 Millisecondes = 1.7×10-5 Minutes= 0.0000167 Minutes c'est l'équivalent de /1000 puis / 60 = /60000 pour faire la conversion
    xQueueReceive(NewStructureQueue, &S1, 0);                                                  //Lecture de structure dans la structureQueue
    

    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )                      // Accés à la ressource commune : Le port série est réservé pour cette partie uniquement jusqu'à le libérer 
    {
   
      Serial.print("Le temps en minutes dés l'activation de la carte est : ");                 //affichage du temps en minutes
      Serial.print(tempsEnMinutes);

      xSemaphoreGive( xSerialSemaphore );                                                      // Libération du sémaphore pour qu'il puisse etre utilisé par les autres 
    }

    vTaskDelay( 100 / portTICK_PERIOD_MS );
  }
}
