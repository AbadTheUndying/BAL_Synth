
#include "DaisyDuino.h"
#include <MIDI.h>


DaisyHardware hw;

Oscillator osc1, osc2, lfo;
MoogLadder filter; 
AdEnv env;

MIDI_CREATE_DEFAULT_INSTANCE();

size_t num_channels;


static float freq1, freq2, lfoFreq;
float note1, note2;
float sig1, sig2;
float mixerLevel;
float cutoffFreq;
float freqMod; 


static void AudioCallback(float **in, float **out, size_t size) {


  mixerLevel = analogRead(A2) / 1023.f;

  cutoffFreq = analogRead(A3) / 1023.f * 4960 + 40; // Rango de 40Hz a 5000Hz

  lfoFreq = analogRead(A4) / 1023.f * 10 + 0.1;
  lfo.SetFreq(lfoFreq);


  // Audio Loop
  for (size_t i = 0; i < size; i++) {
    
    float output;

    float mix = (osc1.Process() + osc2.Process()) * mixerLevel;

    freqMod = cutoffFreq + (lfo.Process() * cutoffFreq);
    filter.SetFreq(freqMod);

    output = filter.Process(mix) * env.Process();

    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = output;
    }
  }
}

void handleNoteOn(byte inChannel, byte inNote, byte inVelocity) {
  env.Trigger();

  note1 = mtof(inNote) * (analogRead(A0) / 1023.f * 3.5 + 0.5);
  osc1.SetFreq(note1);
  note2 = mtof(inNote) * (analogRead(A1) / 1023.f * 3.5 + 0.5);
  osc2.SetFreq(note2);
}


void setup() {

  float samplerate;
  

  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  samplerate = DAISY.get_samplerate();


  osc1.Init(samplerate);
  osc1.SetWaveform(Oscillator::WAVE_SAW);
  osc2.Init(samplerate);
  osc2.SetWaveform(Oscillator::WAVE_SQUARE);
  lfo.Init(samplerate);
  lfo.SetWaveform(Oscillator::WAVE_TRI);


  filter.Init(samplerate);
  filter.SetRes(0.7);

  env.Init(samplerate);
  env.SetTime(ADSR_SEG_ATTACK, .1);
  env.SetTime(ADSR_SEG_DECAY, .5);
  env.SetTime(ADSR_SEG_RELEASE, .01);
  env.SetMax(1);
  env.SetMin(0);
  env.SetCurve(0);

  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.begin(MIDI_CHANNEL_OMNI); // Listen to all incoming messages


  DAISY.begin(AudioCallback);
}

// Bucle principal
void loop() {
  MIDI.read();
}
