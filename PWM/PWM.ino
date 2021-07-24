TaskHandle_t Job0;

int dt;
int level = 40;
int sum = 0;
int lastValue = 0;
int64_t lastTime = esp_timer_get_time();
  
void setup() {
  Serial.begin(112500);
  pinMode(23, OUTPUT);
  createJob();
}

void loop() {
  
}

void job(void * parameter) {
  for (;;) {
    int64_t time = esp_timer_get_time();
    dt = time - lastTime;
    int actual = lastValue * dt;
    int expected = level * dt;
    int error = expected - actual;
    sum += error;
    if (sum > expected) {
      lastValue = 255;
      digitalWrite(23, HIGH);
    } else {
      lastValue = 0;
      digitalWrite(23, LOW);
    }
    lastTime = time;
  }
}

void createJob() {
    xTaskCreatePinnedToCore(
      job, /* Function to implement the task */
      "Job0", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &Job0,  /* Task handle. */
      0); /* Core where the task should run */
}
