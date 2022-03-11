#define MAX_WAIT_FOR_TIMER 1
unsigned int waitFor(int timer, unsigned long period){
  static unsigned long waitForTimer[MAX_WAIT_FOR_TIMER];  // il y a autant de timers que de tâches périodiques
  unsigned long newTime = micros() / period;              // numéro de la période modulo 2^32 
  int delta = newTime - waitForTimer[timer];              // delta entre la période courante et celle enregistrée
  if ( delta < 0 ) delta = 1 + newTime;                   // en cas de dépassement du nombre de périodes possibles sur 2^32 
  if ( delta ) waitForTimer[timer] = newTime;             // enregistrement du nouveau numéro de période
  return delta;
}


#define LUM_PIN 36

enum {EMPTY, FULL};

struct mailbox_s {
  int state;
  int val;
};

struct mailbox_s mb0 = {.state = EMPTY};
struct mailbox_s mb1 = {.state = EMPTY};


struct Lum {
  int timer;
  unsigned long period;
  int pin;
};

void setup_lum(struct Lum* lum, int pin, int timer, unsigned long period){
  lum->pin = pin;
  lum->timer = timer;
  lum->period = period;
  pinMode(pin, INPUT);
}

void loop_lum(struct Lum* lum, struct mailbox_s * mb, struct mailbox_s * mb1) {
  if (!(waitFor(lum->timer,lum->period))) return;
  if (mb->state != EMPTY) return; // attend que la mailbox soit vide
  mb->val = analogRead(lum->pin);
  mb->state = FULL;
  mb1->val = mb->val;
  mb1->state = FULL;
    
}

void setup_serie() {
  Serial.begin(9600);
}

void loop_serie(struct mailbox_s* mb){
  int a;
  if (mb->state != FULL) return; // attend que la mailbox soit pleine
  Serial.print("luminisosite : ");
  a = map(mb->val, 0, 4095, 100, 0);
  //Serial.print(a);
  //Serial.println("%");
  mb->state = EMPTY;
}

struct Led_s {
  int timer;                                              // numéro du timer pour cette tâche utilisé par WaitFor
  unsigned long period;                                   // periode de clignotement
  int pin;                                                // numéro de la broche sur laquelle est la LED
  int etat;                                               // etat interne de la led
}; 

void setup_Led( struct Led_s * ctx, int timer, unsigned long period, byte pin) {
  ctx->timer = timer;
  ctx->period = period;
  ctx->pin = pin;
  ctx->etat = 0;
  pinMode(pin,OUTPUT);
  digitalWrite(pin, ctx->etat);
}

void loop_Led( struct Led_s * ctx, struct mailbox_s* mb) {
  /*if (!waitFor(ctx->timer, ctx->period)) return;          // sort s'il y a moins d'une période écoulée
  digitalWrite(ctx->pin,ctx->etat);                       // ecriture
  ctx->etat = 1 - ctx->etat;                              // changement d'état
  if (mb->state != FULL) return; // attend que la mailbox soit pleine
  ctx->period = map(mb->val, 0, 4095, 100000, 1000000);
  Serial.print("debug : ");
  Serial.println(ctx->period);
  mb->state = EMPTY;*/

  if(waitFor(ctx->timer, ctx->period)) {
      digitalWrite(ctx->pin,ctx->etat);                       // ecriture
      ctx->etat = 1 - ctx->etat;                              // changement d'état
  }

  if(mb->state == FULL) {
    ctx->period = map(mb->val, 0, 4095, 100000, 1000000);
    Serial.print("debug : ");
    Serial.println(ctx->period);
    mb->state = EMPTY;
  }
}


struct Lum lum;
struct Led_s led;

void setup() {
  setup_lum(&lum, LUM_PIN, 0, 500000);
  setup_serie();
  setup_Led(&led, 0, 10000, LED_BUILTIN);
}

void loop() {
  loop_serie(&mb0);
  loop_lum(&lum, &mb0, &mb1);
  loop_Led(&led, &mb1);

}
