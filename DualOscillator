#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

static Oscillator osc1;
static Oscillator osc2;

void MyCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    // Leer el valor del potenciómetro en el rango de 20 a 1000 Hz para controlar la frecuencia del oscilador 1
    float osc1Frequency = map(analogRead(A0), 0, 1023, 33, 3300);
    osc1.SetFreq(osc1Frequency);

    // Leer el valor del potenciómetro en el rango de 20 a 1000 Hz para controlar la frecuencia del oscilador 2
    float osc2Frequency = map(analogRead(A1), 0, 1023, 33, 3300);
    osc2.SetFreq(osc2Frequency);

    float sig1 = osc1.Process();
    float sig2 = osc2.Process();

    // Suma las señales de los dos osciladores
    float mix = sig1 + sig2;

    // Asigna la señal mixta a los canales de salida
    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = mix;
    }
  }
}

void setup() {
  float sample_rate;
  // Inicializar para Daisy pod a 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  sample_rate = DAISY.get_samplerate();

  osc1.Init(sample_rate);
  osc1.SetAmp(0.5);
  osc1.SetWaveform(osc1.WAVE_SAW); // Puedes ajustar la forma de onda aquí

  osc2.Init(sample_rate);
  osc2.SetAmp(0.5);
  osc2.SetWaveform(osc2.WAVE_SAW); // Puedes ajustar la forma de onda aquí

  DAISY.begin(MyCallback);
}

void loop() {
  // No es necesario hacer nada en el bucle principal, todo el procesamiento se realiza en el callback de audio
}
