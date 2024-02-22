// Incluir la biblioteca DaisyDuino
#include "DaisyDuino.h"

// Declarar una instancia de DaisyHardware y dos Osciladores
DaisyHardware hw;
Oscillator osc1, osc2;

// Variables globales para frecuencia, señal y nivel del mezclador
static float freq1, freq2;
float sig1, sig2;
float mixerLevel;

// Función de devolución de llamada de audio
static void AudioCallback(float **in, float **out, size_t size) {
  // Leer el valor del potenciómetro en A0 para la frecuencia del oscilador 1
  freq1 = analogRead(A0) / 1023.f * 2200 + 100; // Rango de 100Hz a 2300Hz
  osc1.SetFreq(freq1);

  // Leer el valor del potenciómetro en A1 para la frecuencia del oscilador 2
  freq2 = analogRead(A1) / 1023.f * 2200 + 100; // Rango de 100Hz a 2300Hz
  osc2.SetFreq(freq2);

  // Leer el valor del potenciómetro en A2 para el nivel del mezclador
  mixerLevel = analogRead(A2) / 1023.f;

  // Audio Loop
  for (size_t i = 0; i < size; i++) {
    // Procesar señal de los osciladores
    sig1 = osc1.Process();
    sig2 = osc2.Process();

    // Mezclar señales con el nivel del mezclador
    float mix = (sig1 + sig2) * mixerLevel;
    
    // Asignar la señal mezclada a los canales de salida
    out[0][i] = mix;
    out[1][i] = mix;
  }
}

// Configuración inicial
void setup() {
  // Declarar variables de tasa de muestreo
  float samplerate;
  
  // Inicializar Daisy Pod
  hw = DAISY.init(DAISY_POD, AUDIO_SR_48K);
  samplerate = DAISY.get_samplerate();

  // Inicializar los osciladores
  osc1.Init(samplerate);
  osc2.Init(samplerate);

  // Iniciar la función de devolución de llamada de audio
  DAISY.begin(AudioCallback);
}

// Bucle principal
void loop() {}
