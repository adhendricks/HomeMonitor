#include <MapleFreeRTOS821.h> //for freeRTOS
#define MAX_TIMINGS 85
#define DHT_PIN   PB12
#define BOARD_LED_PIN PC13
#define PIRPin PB0
#define ldrPin PA0

float bt_data[4] = { 0, 0, 0, 0 }; //initialized to 0
void PIR(void);
void LDR(void);

static void task1(void *pvParameters) { //test task, blinks on board
  for (;;) {
    vTaskDelay(5000);
    //for (int i = 0; i < 4; i++)
    //{
      //Serial3.print("T");
      Serial3.print(bt_data[0]);
      //Serial3.print(" H");
      Serial3.print(" ");
      Serial3.print(bt_data[1]);
      //Serial3.print(" L");
      Serial3.print(" ");
      if (bt_data[2]<10){
        Serial3.print("00");
      }else if(bt_data[2]<100){
        Serial3.print("0");
      }
      Serial3.print(bt_data[2]);
      //Serial3.print(" M");
      Serial3.print(" ");
      Serial3.print(bt_data[3],0);
    //}
    Serial3.println();
    bt_data[3] = 0;
  }
}

static void task2(void *pvParameters) { //DHT task
  for (;;) {
    vTaskDelay(1000);
    read_dht_data();

  }
}
static void task3(void *pvParameters) { //DHT task
  for (;;) {
    vTaskDelay(1000);
    PIR();

  }
}
static void task4(void *pvParameters) { //DHT task
  for (;;) {
    vTaskDelay(1000);
    LDR();

  }
}

void setup() {

  Serial3.begin(9600); //start BT connection
  Serial3.println(F("Starting Program!")); //intro msg


  pinMode(PIRPin, INPUT);
  pinMode(ldrPin, INPUT);

  xTaskCreate(task1, "Task1",
              configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL); //setup for LED task
  xTaskCreate(task2, "Task2",
              configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL); //setup for DHT task
  xTaskCreate(task3, "Task3",
              configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 4, NULL); //setup for DHT task
  xTaskCreate(task4, "Task4",
              configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 5, NULL); //setup for DHT task

  vTaskStartScheduler(); //start Scheduler
}

void loop() {
  //nothing needed here
}

void read_dht_data() //C code to read DHT
{
  uint8_t laststate = HIGH;
  uint8_t counter   = 0;
  uint8_t j     = 0, i;
  int data[5] = { 0, 0, 0, 0, 0 };
  data[0] = data[1] = data[2] = data[3] = data[4] = 0;

  /* pull pin down for 18 milliseconds */
  pinMode( DHT_PIN, OUTPUT );
  digitalWrite( DHT_PIN, LOW );
  delay( 18 );

  /* prepare to read the pin */
  pinMode( DHT_PIN, INPUT );

  /* detect change and read data */
  for ( i = 0; i < MAX_TIMINGS; i++ )
  {
    counter = 0;
    while ( digitalRead( DHT_PIN ) == laststate )
    {
      counter++;
      delayMicroseconds( 1 );
      if ( counter == 255 )
      {
        break;
      }
    }
    laststate = digitalRead( DHT_PIN );

    if ( counter == 255 )
      break;

    /* ignore first 3 transitions */
    if ( (i >= 4) && (i % 2 == 0) )
    {
      /* shove each bit into the storage bytes */
      data[j / 8] <<= 1;
      if ( counter > 16 )
        data[j / 8] |= 1;
      j++;
    }
  }

  /*
     check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
     print it out if data is good
  */
  if ( (j >= 40) &&
       (data[4] == ( (data[0] + data[1] + data[2] + data[3]) & 0xFF) ) )
  {
    float h = (float)((data[0] << 8) + data[1]) / 10;
    if ( h > 100 )
    {
      h = data[0];  // for DHT11
    }
    float c = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10;
    if ( c > 125 )
    {
      c = data[2];  // for DHT11
    }
    if ( data[2] & 0x80 )
    {
      c = -c;
    }
    float f = c * 1.8f + 32;
    bt_data[0] = c;
    bt_data[1] = h;

  } else  {
    // Serial3.print( "Data not good, skip\n" );
  }
}

void PIR(void) {
  if (digitalRead(PIRPin) == HIGH) {
    bt_data[3] = 1;
  }
  else {
    bt_data [3] = ((int)bt_data [3]) | 0 ;
  }
}

void LDR(void) {
  int ldrStatus = analogRead(ldrPin);
  bt_data[2] = map(ldrStatus,100,4095,100,0);
}
