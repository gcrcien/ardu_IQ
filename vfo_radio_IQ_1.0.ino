#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <si5351.h>

#define BUTTON_PIN 4
unsigned int hz;
unsigned int khz;
unsigned int mhz;
String shz;
String skhz;
String smhz;
String fstep = "1khz";
String frequency_string;
int pos = 70;
int adc_value;    //Variable used to store the value read from the ADC converter
Si5351 si5351;
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x38

int correction = 0; // correccion de frecuencia arreglar
String signo;
unsigned long currentFrequency = 7000000; // Start at 1 MHz
unsigned long minFrequency = 1000000; // 1 MHz
unsigned long maxFrequency = 150000000; // 30 MHz
unsigned long stepSize = 1000; // Step size in Hz
unsigned long long pll_freq = 70500000000ULL;
volatile int Even_Divisor = 0;
volatile int oldEven_Divisor = 0;

String banda;
int sensorValue;
int change;
int debounceT = 50;


void setup() {
  pinMode(2, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), knob_ISR1, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), knob_ISR2, FALLING);
  /////////////////////Setting up LCD ///////////////////////
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Jugo Mental");
  lcd.display();
  lcd.setCursor(12, 1);
  lcd.print(":D");
  delay(500);
  /////////////////////////////////////
  si5351.set_ms_source(SI5351_CLK0, SI5351_PLLA);
  si5351.set_ms_source(SI5351_CLK2, SI5351_PLLA);

  ///////////////////////////////////////
  si5351.set_freq_manual(currentFrequency, pll_freq, SI5351_CLK0);
  si5351.set_freq_manual(currentFrequency, pll_freq, SI5351_CLK2);


  pinMode(BUTTON_PIN, INPUT_PULLUP);
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA);
  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
  si5351.drive_strength(SI5351_CLK2, SI5351_DRIVE_2MA);
  si5351.set_phase(SI5351_CLK0, 0);
  si5351.set_phase(SI5351_CLK2, 50);
  si5351.pll_reset(SI5351_PLLA);
  si5351.update_status();

  si5351.set_freq_manual(currentFrequency * SI5351_FREQ_MULT, Even_Divisor * currentFrequency * SI5351_FREQ_MULT, SI5351_CLK0);
  si5351.set_freq_manual(currentFrequency * SI5351_FREQ_MULT, Even_Divisor * currentFrequency * SI5351_FREQ_MULT, SI5351_CLK2);
  si5351.set_phase(SI5351_CLK0, 0);
  si5351.set_phase(SI5351_CLK2, Even_Divisor);
  si5351.pll_reset(SI5351_PLLA);
  actualizar();
}
// Manejador de interrupción para el control de frecuencia (giro de perilla)
void knob_ISR1() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > debounceT) {
    bool dir = digitalRead(3);
    if (dir == true) {
      currentFrequency -= stepSize;
    }
    if (dir == false) {
      currentFrequency += stepSize;
    }
    change = true;

    if  (currentFrequency < 500000) {
      currentFrequency = 500000;
    }
    if  (currentFrequency > 30500000) {
      currentFrequency = 30500000;
    }
    last_interrupt_time = interrupt_time;

  }
}

// Manejador de interrupción para el control de frecuencia (giro de perilla)
void knob_ISR2() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > debounceT) {
    bool dir = digitalRead(2);
    if (dir == true) {
      currentFrequency += stepSize;
    }
    if (dir == false) {
      currentFrequency -= stepSize;
    }
    change = true;

    if  (currentFrequency < 500000) {
      currentFrequency = 500000;
    }
    if  (currentFrequency > 30500000) {
      currentFrequency = 30500000;
    }
  }
  last_interrupt_time = interrupt_time;

}

void actualizar() {
  change = false;

  EvenDivisor();
  fstring();
  banda = getband(currentFrequency);
  ///si5351.set_freq((currentFrequency * SI5351_FREQ_MULT) ,  SI5351_CLK0);
  ///si5351.set_freq((currentFrequency * SI5351_FREQ_MULT) ,  SI5351_CLK2);
  si5351.set_freq_manual((currentFrequency - 22000) * SI5351_FREQ_MULT, Even_Divisor * currentFrequency * SI5351_FREQ_MULT, SI5351_CLK0);
  si5351.set_freq_manual((currentFrequency - 22000) * SI5351_FREQ_MULT, Even_Divisor * currentFrequency * SI5351_FREQ_MULT, SI5351_CLK2);
  si5351.set_phase(SI5351_CLK2, Even_Divisor);
  si5351.pll_reset(SI5351_PLLA);
  si5351.set_phase(SI5351_CLK0, 0);

  if (Even_Divisor != oldEven_Divisor)
  {
    si5351.pll_reset(SI5351_PLLA);
    oldEven_Divisor = Even_Divisor;
  }


  //long ph = 800000000 / currentFrequency;
  //si5351.set_phase(SI5351_CLK2, ph);
  ////////////////////////////////////
  lcd.setCursor(0, 0);
  lcd.print(("VFO") + frequency_string );
  lcd.setCursor(12, 0);
  lcd.print(" MHz");
  lcd.setCursor(0, 1);
  lcd.print(fstep);
  lcd.setCursor(6, 1);
  lcd.print("Banda");
  lcd.setCursor(12, 1);
  lcd.print(banda);
  lcd.display();
}


void loop() {
  if (change != 0) {
    actualizar();
  }
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(50); // Debounce
    lcd.clear();
    stepSize = getNextStepSize(stepSize);
    if (stepSize == 10) {
      fstep = "10Hz";
    }    if (stepSize == 100) {
      fstep = "100Hz";

    }    if (stepSize == 1000) {
      fstep = "1KHz";

    }    if (stepSize == 10000) {
      fstep = "10KHz";

    }    if (stepSize == 100000) {
      fstep = "100KHz";

    }    if (stepSize == 1000000) {
      fstep = "1MHz";

    }
    actualizar();
  }
}



unsigned long getNextStepSize(unsigned long currentStepSize) {
  unsigned long nextStepSize;
  if (currentStepSize == 10) {
    nextStepSize = 100;
  } else if (currentStepSize == 100) {
    nextStepSize = 1000;
  } else if (currentStepSize == 1000) {
    nextStepSize = 10000;
  } else if (currentStepSize == 10000) {
    nextStepSize = 100000;
  } else if (currentStepSize == 100000) {
    nextStepSize = 1000000;
  } else {
    nextStepSize = 10;
  }
  return nextStepSize;
  delay(20);
}
void fstring() {

  mhz = currentFrequency / 1000000;
  khz = (currentFrequency % 1000000) / 1000;
  hz = currentFrequency - ((mhz * 1000000) + (khz * 1000));
  // skhz = String(khz);
  smhz = String(mhz);
  if (mhz < 10 && mhz >= 1) {
    smhz = " " + String(mhz);
  }
  //##############  HZ
  if (hz >= 100) {
    shz = "," + String(hz);
  }
  if (hz < 100 && hz > 10) {
    shz = ",0" + String(hz);
  }
  if (hz < 10 && hz >= 1) {
    shz = ",00" + String(hz);
  }
  if (hz == 0) {
    shz = ",000//";
  }
  //#################### KHZ

  if (khz >= 100) {
    skhz = "," + String(khz);
  }
  if (khz < 100 && khz > 10) {
    skhz = ",0" + String(khz);
  }
  if (khz < 10 && khz >= 1) {
    skhz = ",00" + String(khz);
  }
  if (khz == 0) {
    skhz = ",000";
  }

  frequency_string = smhz + skhz + shz;

}

String getband(unsigned long currentFrequency1) {
  if (currentFrequency1 >= 1800000 && currentFrequency1 <= 2000000) {
    return "160m ";
  } else if (currentFrequency1 >= 3500000 && currentFrequency1 <= 4000000) {
    return "80m ";
  } else if (currentFrequency1 >= 7000000 && currentFrequency1 <= 7300000) {
    return "40m ";
  } else if (currentFrequency1 >= 14000000 && currentFrequency1 <= 14350000) {
    return "20m ";
  } else if (currentFrequency1 >= 21000000 && currentFrequency1 <= 21450000) {
    return "15m ";
  } else if (currentFrequency1 >= 26900000 && currentFrequency1 <= 27500000) {
    return "CB ";
  } else if (currentFrequency1 >= 28000000 && currentFrequency1 <= 29700000) {
    return "10m ";
  } else {
    return "N/A ";
  }
}
void EvenDivisor()
{
  if (currentFrequency < 6850000)
  {
    Even_Divisor = 126;
  }
  if ((currentFrequency >= 6850000) && (currentFrequency < 9500000))
  {
    Even_Divisor = 88;
  }
  if ((currentFrequency >= 9500000) && (currentFrequency < 13600000))
  {
    Even_Divisor = 64;
  }
  if ((currentFrequency >= 13600000) && (currentFrequency < 17500000))
  {
    Even_Divisor = 44;
  }
  if ((currentFrequency >= 17500000) && (currentFrequency < 25000000))
  {
    Even_Divisor = 34;
  }
  if ((currentFrequency >= 25000000) && (currentFrequency < 36000000))
  {
    Even_Divisor = 24;
  }
  if ((currentFrequency >= 36000000) && (currentFrequency < 45000000)) {
    Even_Divisor = 18;
  }
  if ((currentFrequency >= 45000000) && (currentFrequency < 60000000)) {
    Even_Divisor = 14;
  }
  if ((currentFrequency >= 60000000) && (currentFrequency < 80000000)) {
    Even_Divisor = 10;
  }
  if ((currentFrequency >= 80000000) && (currentFrequency < 100000000)) {
    Even_Divisor = 8;
  }
  if ((currentFrequency >= 100000000) && (currentFrequency < 146600000)) {
    Even_Divisor = 6;
  }
  if ((currentFrequency >= 150000000) && (currentFrequency < 220000000)) {
    Even_Divisor = 4;
  }
}
