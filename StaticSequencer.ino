#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

static Oscillator osc;
float noteFrequency = 440.0;  // Frecuencia inicial de la nota (440 Hz)
float bpm = 120.0;  // BPM (beats por minuto)
float noteDuration = 60.0 / bpm;    // Duración de la nota en segundos
float silenceDuration = noteDuration; // Duración del silencio entre notas en segundos

void MyCallback(float **in, float **out, size_t size) {
  float sample_rate = DAISY.get_samplerate();
  static float timer = 0.0;
  static bool notePlaying = true;

  for (size_t i = 0; i < size; i++) {
    // Configurar la frecuencia del oscilador
    osc.SetFreq(notePlaying ? noteFrequency : 0.0);

    // Salida a todos los canales
    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = osc.Process();
    }

    // Actualizar el temporizador
    timer += 1.0 / sample_rate;

    // Comprobar si es tiempo de cambiar la nota
    if (timer >= (notePlaying ? noteDuration : silenceDuration)) {
      timer = 0.0;
      notePlaying = !notePlaying;  // Cambiar entre nota y silencio
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
  // Puedes agregar lógica adicional aquí si es necesario.
}
