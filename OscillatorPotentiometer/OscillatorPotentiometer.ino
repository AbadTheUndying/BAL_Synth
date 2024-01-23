#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

static Oscillator osc;

void MyCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    float sig = osc.Process();

    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = sig;
    }
  }
}

void setup() {
  float sample_rate;
  // Inicializar para Daisy pod a 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  sample_rate = DAISY.get_samplerate();

  osc.Init(sample_rate);
  osc.SetAmp(0.5);
  osc.SetWaveform(osc.WAVE_TRI);

  DAISY.begin(MyCallback);
}

void loop() {
  // Leer el valor del potenciómetro en el rango de 20 a 1000 Hz
  float potValue = analogRead(A0);
  float frequency = map(potValue, 0, 1023, 20, 1000);

  // Establecer la frecuencia del oscilador según el valor del potenciómetro
  osc.SetFreq(frequency);

  // Puedes agregar un pequeño retardo si es necesario
  //delay(10);
}
