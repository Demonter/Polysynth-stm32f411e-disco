#include "Synthesizer.h"
#include "DDSOscillator.h"
#include "MidiInput.h"
#include "Voice.h"
#include "LogBuffer.h"

extern I2C_HandleTypeDef hi2c1;
extern I2S_HandleTypeDef hi2s3;
extern "C" {
    extern DMA_HandleTypeDef hdma_spi3_tx;
}

Synthesizer::Synthesizer() {}

Synthesizer::~Synthesizer() {}

void Synthesizer::init() {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);
    HAL_Delay(10);

    codec = new Cs43l22Driver(&hi2c1);
    codec->init();
    codec->powerOn();
    codec->setVolume(50);
    codec->enableHeadphones();

    startAudioDMA();
}

void Synthesizer::startAudioDMA() {
    for (size_t i = 0; i < AUDIO_BUFFER_SIZE; i += 2) {
        audioBuffer[i] = 0;
        audioBuffer[i + 1] = 0;
    }

    HAL_I2S_DeInit(&hi2s3);
    HAL_DMA_DeInit(&hdma_spi3_tx);
    HAL_Delay(10);

    HAL_DMA_Init(&hdma_spi3_tx);
    __HAL_LINKDMA(&hi2s3, hdmatx, hdma_spi3_tx);
    HAL_I2S_Init(&hi2s3);

    HAL_Delay(10);

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
    HAL_StatusTypeDef result = HAL_I2S_Transmit_DMA(&hi2s3, reinterpret_cast<uint16_t*>(audioBuffer), AUDIO_BUFFER_SIZE);
    if (result != HAL_OK) {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
    } else {
        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
    }
}

void Synthesizer::update() {
	extern MidiInput midi;
	midi.processDma();

	// Enable smoothing once (or in constructor if you prefer)
	normalizer.enableSmoothing(true);

	for (size_t i = 0; i < AUDIO_BUFFER_SIZE; i += 2) {
		float sum = 0.0f;
		int activeVoices = 0;

		for (int v = 0; v < MAX_VOICES; ++v) {
			if (voices[v].isActive()) {
				sum += static_cast<float>(voices[v].nextSample()) / 32767.0f;
				activeVoices++;
			}
		}

		// Only calculate float division once per frame
		float targetGain = 1.0f;
		if (activeVoices > 0) {
			targetGain = 1.0f / static_cast<float>(activeVoices);
		}

		// Apply smoothed target gain
		normalizer.setTargetGain(targetGain);


		int16_t sample = normalizer.normalize(sum); // remove normalizer from the pipeline TODO: add compressor
		//        int16_t sample = static_cast<int16_t>(sum * 0.25f * 32767.0f);

		audioBuffer[i] = sample;
		audioBuffer[i + 1] = sample;  // Stereo identical output
	}
}

void Synthesizer::noteOn(uint8_t note, uint8_t velocity) {	//TODO: create proper logic (circular buffer)!!!
	for (int v = 0; v < MAX_VOICES; ++v) {
	    if (voices[v].isPlayingNote(note)) {
	        // Option 1: Restart it
	        voices[v].trigger(note, velocity, currentWaveform);
	        return;
	    }
	}
    for (int i = 0; i < MAX_VOICES; ++i) {
        if (!voices[i].isActive()) {
            voices[i].trigger(note, velocity, currentWaveform);
            return;
        }
    }

    voices[nextVoiceIndex].trigger(note, velocity, currentWaveform);
    nextVoiceIndex = (nextVoiceIndex + 1) % MAX_VOICES;
}

void Synthesizer::noteOff(uint8_t note) {
    for (int i = 0; i < MAX_VOICES; ++i) {
        if (voices[i].isPlayingNote(note)) {
            voices[i].release();
        }
    }
}

void Synthesizer::fillFirstHalf() {
    fillBufferRange(0);
}

void Synthesizer::fillSecondHalf() {
    fillBufferRange(AUDIO_BUFFER_SIZE / 2);
}

void Synthesizer::fillBufferRange(size_t offset) {
    extern MidiInput midi;
    midi.processDma();

    for (size_t i = 0; i < AUDIO_BUFFER_SIZE / 2; i += 2) {
        double sum = 0.0f;
        int activeVoices = 0;

        for (int v = 0; v < MAX_VOICES; ++v) {
            if (voices[v].isActive()) {
                sum += static_cast<float>(voices[v].nextSample());
                activeVoices++;
            }
        }
        int16_t sample = static_cast<int16_t>(sum * 0.25f);

        static int log_cnt;
        if (!(log_cnt++ % 100000)) {
        	LogBuffer::info("Synth output (float sum): %.3f\r\n", sum);
        	LogBuffer::info("Synth output: %d\r\n",sample);
        	LogBuffer::info("Active voices: %d\r\n",activeVoices);
        }
        audioBuffer[offset + i] = sample;
        audioBuffer[offset + i + 1] = sample;
    }
}


void Synthesizer::cycleWaveform() {
    currentWaveform = static_cast<Waveform>((static_cast<int>(currentWaveform) + 1) % 4);
}

Waveform Synthesizer::getCurrentWaveform() const {
    return currentWaveform;
}
