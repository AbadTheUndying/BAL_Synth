
// Incluir la biblioteca DaisyDuino
#include "DaisyDuino.h"
#include <MIDI.h>

// Declarar una instancia de DaisyHardware y dos Osciladores
DaisyHardware hw;


Oscillator osc1, osc2, lfo;
MoogLadder filter; 
AdEnv env;

MIDI_CREATE_DEFAULT_INSTANCE();

size_t num_channels;

// Variables globales para frecuencia, señal, nivel del mezclador y frecuencia de corte del filtro
static float freq1, freq2, lfoFreq;
float note1, note2, msg;
float sig1, sig2;
float mixerLevel;
float cutoffFreq;
float freqMod; 

// Función de devolución de llamada de audio
static void AudioCallback(float **in, float **out, size_t size) {

  //freq1 = analogRead(A0) / 1023.f * 2200 + 40; // Rango de 40Hz a 2240Hz
  //note1 = freq1 * msg;
  //osc1.SetFreq(note1);


  //freq2 = analogRead(A1) / 1023.f * 2200 + 40; // Rango de 40Hz a 2240Hz
  //note2 = freq2 * msg;
  //osc2.SetFreq(note2);

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
    output = filter.Process(mix) * env.Process();

    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = output;
    }
  }
}

void handleNoteOn(byte inChannel, byte inNote, byte inVelocity) {
  env.Trigger();
  osc1.SetFreq(mtof(inNote));
  osc2.SetFreq(mtof(inNote) / 2);
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
  osc1.SetWaveform(Oscillator::WAVE_SAW);
  osc2.Init(samplerate);
  osc2.SetWaveform(Oscillator::WAVE_SQUARE);
  lfo.Init(samplerate);
  lfo.SetWaveform(Oscillator::WAVE_TRI);

  // Inicializar el filtro Moog Ladder con la frecuencia de muestreo
  filter.Init(samplerate);
  filter.SetRes(0.7);

  env.Init(samplerate);
  env.SetTime(ADSR_SEG_ATTACK, .1);
  env.SetTime(ADSR_SEG_DECAY, .5);
  env.SetTime(ADSR_SEG_RELEASE, .01);

  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.begin(MIDI_CHANNEL_OMNI); // Listen to all incoming messages

  // Iniciar la función de devolución de llamada de audio
  DAISY.begin(AudioCallback);
}

// Bucle principal
void loop() {
  MIDI.read();
}
