void setup() {
  // put your setup code here, to run once:
PIOB->PIO_OER = PIO_PB27;
  PIOB->PIO_PER = PIO_PB27;
  PIOB->PIO_PUDR = PIO_PB27;
}

void loop() {
  // put your main code here, to run repeatedly:

PIOB->PIO_CODR = PIO_PB27;
delay(1000);
PIOB->PIO_SODR = PIO_PB27;
delay(1000);
}
