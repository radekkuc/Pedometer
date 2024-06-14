#include "lcd1602.h"
#include "i2c.h"
#include <stdio.h>
#include <math.h>

#define ZYXDR_Mask (1 << 3) // Maska bitu ZYXDR w rejestrze STATUS
#define SENSITIVITY 1.2     // Czulosc do wykrywania kroku

static uint8_t arrayXYZ[6];
static uint8_t sens;
static uint8_t status;
double X, Y, Z;
int steps = 0;
unsigned long prevTime = 0;
unsigned long currentTime = 0;
float spm = 0;

volatile unsigned long tick = 0; // Zmienna do przechowywania czasu

void SysTick_Handler(void) {
    tick++; // Inkrementacja wartosci czasu w przerwaniu SysTick
}

void InitSysTick(void) {
    SysTick->LOAD = SystemCoreClock / 1000 - 1; // Konfiguracja licznika SysTick na 1ms
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk; // Konfiguracja SysTick
}

unsigned long GetTickCount(void) {
    return tick; // Zwracanie wartosci czasu
}

void countSteps(double x, double y, double z) {
    if (fabs(x) > SENSITIVITY || fabs(y) > SENSITIVITY || fabs(z) > SENSITIVITY) {
        steps++;
    }
}

int main(void) {
    char display[] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};

    // Inicjalizacja licznika SysTick
    InitSysTick();

    // Inicjalizacja wyswietlacza LCD
    LCD1602_Init();
    LCD1602_Backlight(TRUE);

    sens = 0; // Wybór czulosci: 0 - 2g; 1 - 4g; 2 - 8g
    // Konfiguracja czujnika
    I2C_WriteReg(0x1d, 0x2a, 0x0); // ACTIVE=0 - stan czuwania
    I2C_WriteReg(0x1d, 0xe, sens); // Ustaw czulosc zgodnie ze zmienna sens
    I2C_WriteReg(0x1d, 0x2a, 0x1); // ACTIVE=1 - stan aktywny

    prevTime = GetTickCount(); // Ustalenie poczatkowego czasu

    while (1) {
        currentTime = GetTickCount(); // Aktualny czas
        if (currentTime - prevTime >= 60000) { // Co minute oblicz SPM
            spm = (float)steps / ((float)(currentTime - prevTime) / 60000.0);
            prevTime = currentTime;
            steps = 0; // Zerowanie kroków
        }

        I2C_ReadReg(0x1d, 0x0, &status);
        status &= ZYXDR_Mask;
        if (status) // Czy dane gotowe do odczytu?
        {
            I2C_ReadRegBlock(0x1d, 0x1, 6, arrayXYZ);
            X = ((double)((int16_t)((arrayXYZ[0] << 8) | arrayXYZ[1]) >> 2) / (4096 >> sens));
            Y = ((double)((int16_t)((arrayXYZ[2] << 8) | arrayXYZ[3]) >> 2) / (4096 >> sens));
            Z = ((double)((int16_t)((arrayXYZ[4] << 8) | arrayXYZ[5]) >> 2) / (4096 >> sens));
            countSteps(X, Y, Z); // Zliczanie kroków na podstawie odczytanego przyspieszenia

            LCD1602_SetCursor(0, 0);
            sprintf(display, "Kroki: %d    ", steps);
            LCD1602_Print(display);

            LCD1602_SetCursor(0, 1);
            sprintf(display, "SPM: %.2f  ", spm);
            LCD1602_Print(display);
        }
    }
}