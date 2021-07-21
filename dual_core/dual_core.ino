TaskHandle_t Job0;
TaskHandle_t Job1;

void setup() {
  Serial.begin(112500);
  createJob();
}

void loop() {
  int m = millis();
  Serial.print("loop ");
  Serial.print(m);
  Serial.print(" ");
  Serial.print(xPortGetCoreID());
  Serial.print("\n");
  delay(1000);
}

void job(void * parameter) {
  for (;;) {
    int m = millis();
    Serial.print("job ");
    Serial.print(m);
    Serial.print(" ");
    Serial.print(xPortGetCoreID());
    Serial.print("\n");
    delay(500);
  }
  // vTaskDelete(NULL);
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
