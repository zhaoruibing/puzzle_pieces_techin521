// MIT
// Code based off https://github.com/BriscoeTech/Arduino-FreeRTOS-SAMD21/blob/master/examples/Basic_RTOS_Example/Basic_RTOS_Example.ino
#include <FreeRTOS_SAMD21.h>
#include <Adafruit_CircuitPlayground.h>

#define SERIAL Serial

// Indicate if the puzzle pieces are connected.
bool isConnected = false;

// Set the Neo Pixels that used by the light guide
void setLED(int8_t r, int8_t g, int8_t b) {
	CircuitPlayground.clearPixels();
	CircuitPlayground.setPixelColor(3, r, g, b);
	CircuitPlayground.setPixelColor(4, r, g, b);
	CircuitPlayground.setPixelColor(5, r, g, b);
	CircuitPlayground.setPixelColor(6, r, g, b);
}

// Define tasks
void TaskBlink( void *pvParameters ); // Task for blink the LEDs
void TaskAnalogRead( void *pvParameters ); // Task for monitoring connections

// setup() runs once when you press reset or power the board
void setup() {
	SERIAL.begin(9600);

	vNopDelayMS(1000); // prevents usb driver crash on startup, do not omit this

	CircuitPlayground.begin(255);  // 255 means set pixel colors to max brightness.

	// Now set up two tasks to run independently.
	xTaskCreate(
		TaskBlink
		,  (const portCHAR *)"Blink"
		,  256  // Stack size
		,  NULL
		,  2  // priority
		,  NULL );

	xTaskCreate(
		TaskAnalogRead
		,  (const portCHAR *) "AnalogRead"
		,  256  // Stack size
		,  NULL
		,  1  // priority
		,  NULL );

	// Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
	vTaskStartScheduler();
}

void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskBlink(void *pvParameters)
{
	(void) pvParameters;

	for (;;)
	{
		if(isConnected) {
			// Puzzles are connected
			// Continously glow green light
			setLED(0, 128, 0);
			vTaskDelay( 100 / portTICK_PERIOD_MS );
		}
		else {
			// Puzzles are disconnected
			// Blink white light every half second
			setLED(128, 128, 128);
			vTaskDelay( 500 / portTICK_PERIOD_MS );
			setLED(0, 0, 0);
			vTaskDelay( 500 / portTICK_PERIOD_MS );
		}
	}
}

void TaskAnalogRead(void *pvParameters)
{
	(void) pvParameters;

	for (;;)
	{
		// If the pin's value is ~3.3V/2 = 1.65V, then it is connected.
		int sensorValue = analogRead(A4);
		Serial.println(sensorValue);
		isConnected = 300 < sensorValue && sensorValue < 900;
		vTaskDelay(50 / portTICK_PERIOD_MS);
	}
}
