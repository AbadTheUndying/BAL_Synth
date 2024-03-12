
// Incluir la biblioteca DaisyDuino
#include "DaisyDuino.h"

// Declarar una instancia de DaisyHardware y dos Osciladores
DaisyHardware hw;


Oscillator osc1, osc2, lfo;
MoogLadder filter; 


size_t num_channels;

// Variables globales para frecuencia, señal, nivel del mezclador y frecuencia de corte del filtro
static float freq1, freq2,lfoFreq;
float sig1, sig2;
float mixerLevel;
float cutoffFreq = 1000.0;
float freqMod; 

// Función de devolución de llamada de audio
static void AudioCallback(float **in, float **out, size_t size) {
  // Leer el valor del potenciómetro en A0 para la frecuencia del oscilador 1
  freq1 = analogRead(A0) / 1023.f * 2200 + 40; // Rango de 40Hz a 2240Hz
  osc1.SetFreq(freq1);
  osc1.SetWaveform(Oscillator::WAVE_SAW);

  // Leer el valor del potenciómetro en A1 para la frecuencia del oscilador 2
  freq2 = analogRead(A1) / 1023.f * 2200 + 40; // Rango de 40Hz a 2240Hz
  osc2.SetFreq(freq2);
  osc2.SetWaveform(Oscillator::WAVE_SQUARE);

  // Leer el valor del potenciómetro en A2 para el nivel del mezclador
  mixerLevel = analogRead(A2) / 1023.f;

  // Calcular la frecuencia de corte del filtro según un potenciómetro en A3 (ejemplo)
  cutoffFreq = analogRead(A3) / 1023.f * 4960 + 40; // Rango de 40Hz a 5000Hz

  // Calcular frecuencia del LFO para modulado del filtro
  lfoFreq = analogRead(A4) / 1023.f * 10 + 0.1;
  lfo.SetFreq(lfoFreq);

  

  // Audio Loop
  for (size_t i = 0; i < size; i++) {
    
    float output;

    // Procesar señal de los osciladores
    sig1 = osc1.Process();
    sig2 = osc2.Process();

    // Mezclar señales con el nivel del mezclador
    float mix = (sig1 + sig2) * mixerLevel;

    freqMod = cutoffFreq + (lfo.Process() * cutoffFreq);
    filter.SetFreq(freqMod);

    // Aplicar el filtro a la señal mezclada justo antes de asignarla a los canales de salida
    output = filter.Process(mix);

    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = output;
    }
  }
}

// Configuración inicial
void setup() {
  // Declarar variables de tasa de muestreo
  float samplerate;
  
  // Inicializar Daisy Pod
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  samplerate = DAISY.get_samplerate();

  // Inicializar los osciladores
  osc1.Init(samplerate);
  osc2.Init(samplerate);
  lfo.Init(samplerate);

  // Inicializar el filtro Moog Ladder con la frecuencia de muestreo
  filter.Init(samplerate);
  filter.SetRes(0.7);

  // Iniciar la función de devolución de llamada de audio
  DAISY.begin(AudioCallback);
}

// Bucle principal
void loop() {}
