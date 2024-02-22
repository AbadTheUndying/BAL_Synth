// Incluir la biblioteca DaisyDuino
#include "DaisyDuino.h"

// Declarar una instancia de DaisyHardware y dos Osciladores
DaisyHardware hw;
Oscillator osc1, osc2;
MoogLadder filter; // Filtro Moog Ladder para corte de frecuencia
Oscillator lfo; // Oscilador de baja frecuencia (LFO)

// Variables globales para frecuencia, señal, nivel del mezclador y frecuencia de corte del filtro
static float freq1, freq2;
float sig1, sig2;
float mixerLevel;
float cutoffFreq = 2000.0; // Frecuencia de corte inicial del filtro
float resonance = 0.5; // Resonancia para enfatizar los graves
float lfoFreq = 2; // Frecuencia del LFO
float lfoAmp; // Amplitud del LFO

// Variable para el estado del filtro
bool filterActive = false;

// Función de devolución de llamada de audio cuando la amplitud del LFO es 0
static void AudioCallbackNoLFO(float **in, float **out, size_t size) {
  // Leer el valor del potenciómetro en A0 para la frecuencia del oscilador 1
  freq1 = analogRead(A0) / 1023.f * 2200 + 40; // Rango de 40Hz a 2240Hz
  osc1.SetFreq(freq1);
  osc1.SetWaveform(Oscillator::WAVE_SAW);

  // Leer el valor del potenciómetro en A1 para la frecuencia del oscilador 2
  freq2 = analogRead(A1) / 1023.f * 2200 + 40; // Rango de 40Hz a 2240Hz
  osc2.SetFreq(freq2);
  osc2.SetWaveform(Oscillator::WAVE_SAW);

  // Leer el valor del potenciómetro en A2 para el nivel del mezclador
  mixerLevel = analogRead(A2) / 1023.f;

  // Calcular la frecuencia de corte del filtro según un potenciómetro en A3 (ejemplo)
  cutoffFreq = analogRead(A3) / 1023.f * 4960 + 40; // Rango de 40Hz a 5000Hz
  filter.SetFreq(cutoffFreq);

  // Audio Loop
  for (size_t i = 0; i < size; i++) {
    // Procesar señal de los osciladores
    sig1 = osc1.Process();
    sig2 = osc2.Process();

    // Mezclar señales con el nivel del mezclador
    float mix = (sig1 + sig2) * mixerLevel;

    // Aplicar el filtro a la señal mezclada justo antes de asignarla a los canales de salida
    mix = filter.Process(mix);

    // Amplificar las ondas enfatizando los graves
    mix *= 1.0 + resonance;

    // Asignar la señal procesada a los canales de salida
    out[0][i] = mix;
    out[1][i] = mix;
  }
}

// Función de devolución de llamada de audio principal
static void AudioCallback(float **in, float **out, size_t size) {
  if (lfoAmp == 0.0) {
    // Si la amplitud del LFO es 0, llamar a la función AudioCallbackNoLFO
    AudioCallbackNoLFO(in, out, size);
  } else {
    // Procesar el audio normalmente con el LFO
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

    // Leer el valor del potenciómetro en A3 para la frecuencia de corte del filtro
    cutoffFreq = analogRead(A3) / 1023.f * 5000 + 40; // Rango de 40Hz a 5040Hz
    filter.SetFreq(cutoffFreq);

    // Procesar el LFO y activar/desactivar el filtro en cada ciclo completo de la onda cuadrada
    for (size_t i = 0; i < size; i++) {
      float lfoValue = lfo.Process();
      if (lfoValue >= 0 && filterActive) {
        filterActive = false; // Desactivar el filtro al inicio de cada ciclo positivo de la onda cuadrada
      } else if (lfoValue < 0 && !filterActive) {
        filterActive = true; // Activar el filtro al inicio de cada ciclo negativo de la onda cuadrada
      }
    }

    // Audio Loop
    for (size_t i = 0; i < size; i++) {
      // Procesar señal de los osciladores
      sig1 = osc1.Process();
      sig2 = osc2.Process();

      // Mezclar señales con el nivel del mezclador
      float mix = (sig1 + sig2) * mixerLevel;

      // Aplicar el filtro si está activo
      if (filterActive) {
        mix = filter.Process(mix);
      }

      // Amplificar las ondas enfatizando los graves
      mix *= 1.0 + resonance;

      // Asignar la señal procesada a los canales de salida
      out[0][i] = mix;
      out[1][i] = mix;
    }
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

  // Inicializar el filtro Moog Ladder con la frecuencia de muestreo
  filter.Init(samplerate);

  // Inicializar el LFO
  lfo.Init(samplerate);
  lfo.SetWaveform(Oscillator::WAVE_SAW); // Configurar el LFO como una onda cuadrada
  lfo.SetAmp(1.0); // Amplitud del LFO
  lfo.SetFreq(lfoFreq); // Frecuencia del LFO

  // Iniciar la función de devolución de llamada de audio
  DAISY.begin(AudioCallback);
}

// Bucle principal
void loop() {
  // Leer el valor del potenciómetro en A4 para la amplitud del LFO
  lfoAmp = analogRead(A4) / 1023.0;
}
