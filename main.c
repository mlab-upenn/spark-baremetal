#include "stm32f10x.h"

volatile int optimizer;

// implements software button debouncing
char isButtonPressed() {
  
  // sample the button
  char first_sample = GPIOA->IDR & 1;
  for (int i = 0; i < 512; i++) {
    optimizer = 0xDEAD; // break the optimizer so we can waste time properly
  }
  char second_sample = GPIOA->IDR & 1;
  
  // compare samples
  if (first_sample == second_sample) {
    return first_sample;
  } else {
    return 2;
  }
}

int main() {
  // initialization
  char count = 0;
  char new_button = 0;
  char old_button = 0;

  GPIOB->CRL = 0x11111111; // set all port B pins to output push-pull
  GPIOB->CRH = 0x11111111;
  
  GPIOA->CRL = 0x44444444; // set all port A pins to floating input
  GPIOA->CRH = 0x44444444;
  
  for (;;) {
    
    // set D0-2 to match count
    GPIOB->ODR = (GPIOB->ODR & 0xFFFF0000) | (count << 5);
    
    // update button vars
    old_button = new_button;
    do {
      new_button = isButtonPressed();
    } while (new_button == 2);
    
    // if there was a button press, increment count
    if (new_button && !old_button) {
      count++;
      count &= 7;
    }
    
    // feed the watchdog
    IWDG->KR = 0xAAAA;
  }
}
