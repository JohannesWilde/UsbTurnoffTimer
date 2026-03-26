#include <stc8h.h>


static void delay(unsigned int t)
{
  while (t--) {
    volatile int i = 0xFFF;
    while (i--);
  }
}

#define DIO_MODE_BIDIRECTIONAL_M0 0
#define DIO_MODE_BIDIRECTIONAL_M1 0
#define DIO_MODE_PUSH_PULL_OUTPUT_M0 1
#define DIO_MODE_PUSH_PULL_OUTPUT_M1 0
#define DIO_MODE_HIGH_Z_INPUT_M0 0
#define DIO_MODE_HIGH_Z_INPUT_M1 1
#define DIO_MODE_OPEN_DRAIN_M0 1
#define DIO_MODE_OPEN_DRAIN_M1 1


void main()
{
    P1M0 = (P5M0 & ~(0b1 << 2)) | (DIO_MODE_PUSH_PULL_OUTPUT_M0 << 2);
    P1M1 = (P5M1 & ~(0b1 << 2)) | (DIO_MODE_PUSH_PULL_OUTPUT_M1 << 2);

    P1_2 = 1; // LED at dev board.
    // WKTCL = 0xFE; // Set the power-down wake-up clock to be about 10 seconds
    // WKTCH = 0x87;
    // EA = 1;

    while (1)
    {
        // PCON |= 0x02;  // Enter power-down mode
        P1_2 = (0 != P1_2) ? 0 : 1;  // Toggle P5.5
        delay(50);
    }
}
