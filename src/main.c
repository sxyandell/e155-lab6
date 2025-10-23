// Sarah Yandell
// syandell@hmc.edu
// 2025-10-23
// main.c
// Description: Top-level application that serves a  web UI via an
// ESP8266 over UART. Allows user to control an LED, select DS1722 resolution, and display
// the current temperature in Celsius. Initializes clock, GPIO, timer, USART,
// and SPI, then has simple request in a loop.


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "C:\Users\syandell\Desktop\lab06\lib\STM32L432KC_SPI.h"
#include "C:\Users\syandell\Desktop\lab06\lib\DS1722.h"


/////////////////////////////////////////////////////////////////
// Provided Constants and Functions
/////////////////////////////////////////////////////////////////


//Defining the web page in two chunks: everything before the current time, and everything after the current time
char* webpageStart = "<!DOCTYPE html><html><head><title>E155 Web Server Demo Webpage</title>\
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
  </head>\
  <body><h1>E155 Web Server Demo Webpage</h1>";

char* ledStr = "<p>LED Control:</p><form action=\"ledon\"><input type=\"submit\" value=\"Turn the LED on!\"></form>\
  <form action=\"ledoff\"><input type=\"submit\" value=\"Turn the LED off!\"></form>";


char* res = "<p>Resolution Control:</p> <form action=\"res8\"><input type=\"submit\" value=\"Resolution 8\"></form>\
           <form action=\"res9\"><input type=\"submit\" value=\"Resolution 9\"></form>\
            <form action=\"res10\"><input type=\"submit\" value=\"Resolution 10\"></form>\
             <form action=\"res11\"><input type=\"submit\" value=\"Resolution 11\"></form>\
  <form action=\"res12\"><input type=\"submit\" value=\"Resolution 12\"></form>";


char* webpageEnd   = "</body></html>";


//determines whether a given character sequence is in a char array request, returning 1 if present, -1 if not present
int inString(char request[], char des[]) {
  if (strstr(request, des) != NULL) {return 1;}
  return -1;
}


// LED control from request and update LED pin; returns 1 if on, 0 if off.
int updateLEDStatus(char request[])
{
  int led_status = 0;
  // The request has been received. now process to determine whether to turn the LED on or off
  if (inString(request, "ledoff")==1) {
    digitalWrite(LED_PIN, PIO_LOW);
    led_status = 0;
  }
  else if (inString(request, "ledon")==1) {
    digitalWrite(LED_PIN, PIO_HIGH);
    led_status = 1;
  }
  return led_status;
}


//  DS1722 resolution selection from request; returns 8, 9, 10, 11,12 or -1 for no change.
int updateRESStatus(char request[])
{
    int res_status = -1; // no change by default
    // The request has been received. now process to determine resolution
    if (inString(request, "res8")==1) {
        res_status = 8;
    }
    else if (inString(request, "res9")==1) {
        res_status = 9;
    }else if (inString(request, "res10")==1) {
        res_status = 10;
    }else if (inString(request, "res11")==1) {
        res_status = 11;
    }else if (inString(request, "res12")==1) {
        res_status = 12;
    }
    return res_status;
}


/////////////////////////////////////////////////////////////////
// Solution Functions
/////////////////////////////////////////////////////////////////


// Main function init peripherals, ESP8266 requests, update LED and DS1722,
// and return HTML including current temperature in Celsius.
int main(void) {
  configureFlash();
  configureClock();

  gpioEnable(GPIO_PORT_A);
  gpioEnable(GPIO_PORT_B);
  gpioEnable(GPIO_PORT_C);

  pinMode(LED_PIN, GPIO_OUTPUT);
 
  RCC->APB2ENR |= (RCC_APB2ENR_TIM15EN);
  initTIM(TIM15);
 
  USART_TypeDef * USART = initUSART(USART1_ID, 125000);
 
  // Initialize SPI1 for DS1722: BR=0b011 (fclk/16, 16MHz -> 1MHz) (max 5MHz), CPOL=0, CPHA=1
  initSPI(0b011, 0, 1);
  // Initialize DS1722
  ds1722_init();
  
  while(1) {
    /* Wait for ESP8266 to send a request.
    Requests take the form of '/REQ:<tag>\n', with TAG begin <= 10 characters.
    Therefore the request[] array must be able to contain 18 characters.
    */

    // Receive web request from the ESP
    char request[BUFF_LEN] = "                  "; // initialize to known value
    int charIndex = 0;
 
    // Keep going until you get end of line character
    while(inString(request, "\n") == -1) {
      // Wait for a complete request to be transmitted before processing
      while(!(USART->ISR & USART_ISR_RXNE));
      request[charIndex++] = readChar(USART);
    }

    // Update string with current LED state
    int led_status = updateLEDStatus(request);

    char ledStatusStr[20];
    if (led_status == 1)
      sprintf(ledStatusStr,"LED is on!");
    else if (led_status == 0)
      sprintf(ledStatusStr,"LED is off!");

    // Apply resolution selection
    static int current_res = 9; // default
    int res_status = updateRESStatus(request);
    if (res_status != -1 && res_status != current_res) {
      current_res = res_status;
      ds1722_res(current_res);
    }

    char resStatusStr[20];
    if (current_res == 8)
      sprintf(resStatusStr,"Resolution 8");
    else if (current_res == 9)
      sprintf(resStatusStr,"Resolution 9");
    else if (current_res == 10)
      sprintf(resStatusStr,"Resolution 10");
    else if (current_res == 11)
      sprintf(resStatusStr,"Resolution 11");
    else if (current_res == 12)
      sprintf(resStatusStr,"Resolution 12");

    // Read temperature from DS1722 after applying resolution change
    float tempC = ds1722_read_celsius();
    char tempStr[32];
    sprintf(tempStr, "%f C", tempC);

    // finally, transmit the webpage over UART
    sendString(USART, webpageStart); // webpage header code
    sendString(USART, ledStr); // button for controlling LED
    sendString(USART, res); // button for controlling res
    sendString(USART, "<h2>LED Status</h2>");
    sendString(USART, "<p>");
    sendString(USART, ledStatusStr);
    sendString(USART, "</p>");
    sendString(USART, "<h2>Resolution Status</h2>");
    sendString(USART, "<p>");
    sendString(USART, resStatusStr);
    sendString(USART, "</p>");
    sendString(USART, "<h2>Temperature</h2>");
    sendString(USART, "<p>");
    sendString(USART, tempStr);
    sendString(USART, "</p>");
    sendString(USART, webpageEnd);
  }
}
