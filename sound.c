#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "wave-table.h"
#include "note-table.h"

// Format:
// ____ ____ ____ ____
// 0DDD DPPP Pnnn nnnn
// |  |    |        \- Note: Set to 0 for "Off"
// |  |    \---------- Duration (in LUT events) of the post-note pause
// |  \--------------- Duration (in LUT events) of this note
// \------------------ Word type: 0 for normal note, 1 for effect
//
// 1000 eeee aaaa aaaa
// |     |      \----- Argument to the effect
// |     \------------ Effect number
// \------------------ 1 indicates effect
//
// 1001 tttt dddd dddd - Set LUT delay duration
//        |    \-------- Duration (in ticks)
//        \------------- LUT delay index
//
// 1010 tttt tttt tttt - Set voice attack time
//            \--------- Time (in ticks)
//
// 1011 tttt tttt tttt - Set voice decay time
//            \--------- Time (in ticks)
//
// 1100 tttt tttt tttt - Set voice release time
//            \--------- Time (in ticks)

#define N_32 1
#define N_16 2
#define N_8 3
#define N_EIGHTH N_8
#define N_4 4
#define N_QUARTER N_4
#define N_2 5
#define N_HALF N_2
#define N_DOTTED_HALF 6
#define N_1 7
#define N_WHOLE N_1

// Default tick table
static const uint8_t tick_lut[8] = {
    0,      // None
    1,      // 32nd note
    2,      // Sixteenth note
    4,      // Eigth note
    8,      // Quarter note
    16,     // Half note
    24,     // Dotted half-note
    32,     // Whole note
};

enum ltc_pattern_effect {
    // Delay the next instruction by this amount
    DELAY_TICKS = 1,

    // Jump to a new pattern
    PATTERN_JUMP = 2,

    // Set the current voice's instrument
    SET_INSTRUMENT = 3,

    // Set the current voice's attack level
    SET_ATTACK_LEVEL = 4,

    // Set the current voice's sustain level
    SET_SUSTAIN_LEVEL = 5,

    // Set the global pattern speed
    SET_GLOBAL_SPEED = 6,

    FINAL_EFFECT = 7,
};

#define NN(note, duration, pause) (((uint16_t)((note) & 0x7f)) \
                                | (((duration) & 0x7) << 11) \
                                | (((pause) & 0x7) << 7) )
#define NE(effect, arg) ((((effect) & 0x7f) << 8) | (((arg) & 0xff) << 0) | (1 << 15))
#define NLT(time) (0xa000 | (time & 0xfff)) // Voice LUT time
#define NAT(time) (0xa000 | (time & 0xfff)) // Voice attack time
#define NDT(time) (0xb000 | (time & 0xfff)) // Voice decay time
#define NRT(time) (0xc000 | (time & 0xfff)) // Voice release time

// Enable interpolation to make the output smoother.
// Set to 0 to disable interpolation.
// Note that some instruments don't support interpolation.
#define INTERPOLATION_ENABLED 1

// Sets the maximum value of the phase accumulator, which is
// used to skip through the sample array.
#define PHASEACC_MAX 16384L

// The system is running off of a 32.768 kHz crystal going through
// a 1464x FLL multiplier, giving a system frequency of
// 47.972352 MHz.
// We set the PWM counter to 256, which gives a PWM period of 187.392 kHz.
// Happily, 185.939 is evenly divisible by 24, giving us an actual sample
// rate of 7808 Hz, assuming we delay 24 times.
#define PWM_DELAY_LOOPS 24
#define SAMPLE_RATE (187392/12)

// The number of ticks that the sound system has gone through.
// Overflows after about three days, at 14 kHz.
volatile uint32_t global_tick_counter;

// The next sample to be played, nominally between -128 and 127
static int32_t next_sample;

// Nonzero if a sample has been queued, zero if the sample buffer is empty.
static volatile uint8_t sample_queued;

static const struct ltc_instrument *instruments[] = {
    &triangle_instrument,
    &sawtooth_instrument,
    &sine_instrument,
};

// An ltc voice
struct ltc_voice
{
    // The current note's frequency.
    uint32_t frequency;

    // A pointer to the currently-selected instrument.
    const struct ltc_instrument *instrument;

    uint32_t attack_time;
    uint32_t attack_level;
    uint32_t decay_time;
    uint32_t sustain_level;
    uint32_t release_time;

    // Keeps track of the phase in the instrument at the
    // given frequency.
    uint32_t phase_accumulator;

    // The time when the note timer started.
    uint32_t note_timer;

    // What time the note was released
    uint32_t release_timer;

    // A pointer to the currently-operating pattern
    const uint16_t *pattern;
    uint16_t pattern_offset;

    // The number of ticks left until we advance the pattern offset.
    uint16_t countdown;

    uint8_t tick_lut[8];
};

static const uint16_t pattern0_voice0[] = {
    NE(SET_GLOBAL_SPEED, 100),
    NE(SET_INSTRUMENT, 0),
    NAT(200),
    NE(SET_ATTACK_LEVEL, 50),
    NDT(250),
    NE(SET_SUSTAIN_LEVEL, 40),
    NRT(10),
    NE(PATTERN_JUMP, 2),
};

static const uint16_t pattern1_voice0[] = {
    NN(_C_5, N_QUARTER, N_HALF),
    NN(_C_5, N_HALF, N_QUARTER),
    NN(_C_5, N_DOTTED_HALF, 0),
/*
    NN(_G_5, N_QUARTER, N_HALF),
    NN(_A_6, N_QUARTER, N_HALF),
    NN(_E_5, N_EIGHTH, N_EIGHTH),
    NN(_E_5, N_EIGHTH, N_HALF),
    NN(_C_5, N_EIGHTH, N_QUARTER),
    NN(_Ds5, N_EIGHTH, N_QUARTER),
    NN(_D_5, N_EIGHTH, N_QUARTER),
    NN(_C_5, N_EIGHTH, N_QUARTER),
*/
/*
    NN(0, 8),
    NN(_C_5, 8),
    NN(_D_5, 8),
    NN(0, 8),
    NN(_Ds5, 8),
    NN(_E_5, 8),
    NN(_C_5, 8),
    NN(_D_5, 8),
    NN(0, 127),
    NN(_D_4, 64),
    NN(_C_4, 64),
    NN(_D_4, 64),
    NN(_E_4, 64),
*/
    NE(PATTERN_JUMP, 2),
};

static const uint16_t pattern0_voice1[] = {
    NE(PATTERN_JUMP, 1),
};

static const uint16_t *sample_song_patterns[] = {
    pattern0_voice0,
    pattern0_voice1,
    pattern1_voice0,
};

struct ltc_song {
    const uint16_t **patterns;
};

static const struct ltc_song sample_song = {
    .patterns = sample_song_patterns,
};

struct ltc_sound_engine {
    struct ltc_voice voices[2];

    // Global counter
    uint32_t tick_counter;

    // Number of loops-per-tick
    uint16_t loops_per_tick;

    // Currently-selected song
    const struct ltc_song *song;
};

static struct ltc_sound_engine engine;

#define TIME_OFFSET(sec, msec) ((SAMPLE_RATE * sec) + ((msec * SAMPLE_RATE) / 1000))

static void patternDelay(struct ltc_sound_engine *engine, uint8_t channel, uint8_t arg)
{
    //fprintf(stderr, "[Channel %d]: Pattern delay %d ticks\n", channel, arg);
    engine->voices[channel].countdown = arg * engine->loops_per_tick;
}

static void jumpToPattern(struct ltc_sound_engine *engine, uint8_t channel, uint8_t arg)
{
    //fprintf(stderr, "[Channel %d]: Jumping to pattern %d\n", channel, arg);
    engine->voices[channel].pattern = engine->song->patterns[arg];
    engine->voices[channel].pattern_offset = 0;
}

static void setInstrument(struct ltc_sound_engine *engine, uint8_t channel, uint8_t arg)
{
    fprintf(stderr, "[Channel %d]: Setting instrument %d\n", channel, arg);
    engine->voices[channel].instrument = instruments[arg];
}

static void setAttackTime(struct ltc_sound_engine *engine, uint8_t channel, uint16_t arg)
{
    engine->voices[channel].attack_time = (arg * SAMPLE_RATE) / 1000;
}

static void setAttackLevel(struct ltc_sound_engine *engine, uint8_t channel, uint8_t arg)
{
    engine->voices[channel].attack_level = arg;
}

static void setDecayTime(struct ltc_sound_engine *engine, uint8_t channel, uint16_t arg)
{
    engine->voices[channel].decay_time = (arg * SAMPLE_RATE) / 1000 + engine->voices[channel].attack_time;
}

static void setSustainLevel(struct ltc_sound_engine *engine, uint8_t channel, uint8_t arg)
{
    engine->voices[channel].sustain_level = arg;
}

static void setReleaseTime(struct ltc_sound_engine *engine, uint8_t channel, uint16_t arg)
{
    engine->voices[channel].release_time = (arg * SAMPLE_RATE) / 1000;
}

static void setGlobalSpeed(struct ltc_sound_engine *engine, uint8_t channel, uint8_t arg)
{
    engine->loops_per_tick = arg;
}

typedef void (*effect_t)(struct ltc_sound_engine *engine, uint8_t channel, uint8_t arg);

static const effect_t effect_lut[] = {
    0,
    patternDelay,
    jumpToPattern,
    setInstrument,
    setAttackLevel,
    setSustainLevel,
    setGlobalSpeed,
};

#if 0
static int pwm0_stable_timer(void)
{
    static int loops = 0;

    loops++;
    if (loops > PWM_DELAY_LOOPS)
    {
        int32_t scaled_sample = next_sample + 129;
        if (scaled_sample > 255)
            scaled_sample = 255;
        if (scaled_sample < 1)
            scaled_sample = 1;
        writel(scaled_sample, TPM0_C1V);
        writel(scaled_sample, TPM0_C0V);

        loops = 0;
        sample_queued = 0;
        //global_tick_counter++;
    }

    static int other_loops;
    if (other_loops++ > 12) {
        global_tick_counter++;
        other_loops = 0;
    }

    /* Reset the timer IRQ, to allow us to fire again next time */
    writel(TPM0_STATUS_CH1F | TPM0_STATUS_CH0F | TPM0_STATUS_TOF, TPM0_STATUS);

    return 0;
}

static void prepare_pwm()
{
    // Write dummy values out, to configure PWM mux
    pinMode(0, OUTPUT);
    analogWrite(0, 63);
    pinMode(1, OUTPUT);
    analogWrite(1, 63);

    // Disable TPM0, allowing us to configure it
    writel(0, TPM0_SC);

    // Also disable both channels, which are running from the
    // calls to analogWrite() above
    writel(0, TPM0_C0SC);
    writel(0, TPM0_C1SC);

    // Configure the TPM to use the MCGFLLCLK (~32 MHz?)
    writel(readl(SIM_SOPT2) | (1 << 24), SIM_SOPT2);

    // We've picked pin 0, which is on TPM0_CH1
    writel(255, TPM0_MOD);
    writel(0, TPM0_CNT);

    writel(TPM0_C0SC_MSB | TPM0_C0SC_ELSB, TPM0_C0SC);
    writel(TPM0_C1SC_MSB | TPM0_C1SC_ELSA, TPM0_C1SC);

    writel(100, TPM0_C1V);
    writel(100, TPM0_C0V);
    writel(TPM0_CONF_TRGSEL(8), TPM0_CONF);
    writel(TPM0_SC_TOF | TPM0_SC_TOIE | TPM0_SC_CMOD(1) | TPM0_SC_PS(0), TPM0_SC); // Enable TPM0

    /* Enable the IRQ in the system-wide interrupt table */
    attachFastInterrupt(PWM0_IRQ, pwm0_stable_timer);
}
#endif

void setSong(struct ltc_sound_engine *engine, const struct ltc_song *song) {
    int voice_num;
    engine->song = song;

    for (voice_num = 0; voice_num < 2; voice_num++) {
        struct ltc_voice *voice = &engine->voices[voice_num];
        voice->pattern = song->patterns[voice_num];
        voice->pattern_offset = 0;
        voice->countdown = 0;
        voice->note_timer = 0;
        voice->release_timer = 0;
        voice->instrument = 0;
        memcpy(voice->tick_lut, tick_lut, sizeof(tick_lut));
    }
}

void setup(void)
{
    setSong(&engine, &sample_song);
    //prepare_pwm();
    //enableInterrupt(PWM0_IRQ);
    //pinMode(2, OUTPUT);
    //pinMode(3, OUTPUT);
    //pinMode(4, OUTPUT);
    //pinMode(5, OUTPUT);
}

#define ATTACK_PHASE 1
#define DECAY_PHASE 2
#define SUSTAIN_PHASE 3
#define RELEASE_PHASE 4
#ifdef DEBUG
#define DEBUG_PHASE(x) do { \
writel((1 << 12), (x == ATTACK_PHASE) ? FGPIOA_PSOR : FGPIOA_PCOR); \
writel((1 << 13), (x == DECAY_PHASE) ? FGPIOB_PSOR : FGPIOB_PCOR); \
writel((1 << 0), (x == SUSTAIN_PHASE) ? FGPIOB_PSOR : FGPIOB_PCOR); \
writel((1 << 7), (x == RELEASE_PHASE) ? FGPIOA_PSOR : FGPIOA_PCOR); \
} while(0);
#else
#define DEBUG_PHASE(x)
#endif
static int32_t processADSR(struct ltc_voice *voice, int32_t output)
{
    int32_t pct;
    if (voice->release_timer != 0)
    {
        /* Release phase */
        DEBUG_PHASE(RELEASE_PHASE);

        /* The note has expired */
        if ((voice->note_timer - voice->release_timer) > voice->release_time)
            return 0;

        /* Determine what percentage we'll adjust the note to */
        pct = (voice->note_timer - voice->release_timer) * voice->sustain_level / voice->release_time;
    }
    else if (voice->note_timer < voice->attack_time)
    {
        /* Attack phase */
        DEBUG_PHASE(ATTACK_PHASE);

        /* Determine what percentage we'll adjust the note to */
        pct = voice->note_timer * voice->attack_level / voice->attack_time;
    }
    else if (voice->note_timer < voice->decay_time)
    {
        /* Decay phase */
        DEBUG_PHASE(DECAY_PHASE);

        /* Determine what percentage we'll adjust the note to */
        pct = voice->sustain_level + (voice->decay_time - voice->note_timer) * (voice->attack_level - voice->sustain_level) / (voice->decay_time - voice->attack_time);
    }
    else
    {
        /* Sustain phase */
        DEBUG_PHASE(SUSTAIN_PHASE);
        pct = voice->sustain_level;
    }

    /* Scale the note volume to the calcualted percentage */
    output = output * pct / 100;
    return output;
}

int32_t get_sample(struct ltc_voice *voice)
{
    uint32_t period;
    int32_t output;
    int32_t v1, v2, v1_weight, v2_weight;

    if (!voice->instrument)
        return 0;

    // calculate the phase accumulator distance
    // we divide the frequency by the sample rate to give us how much of a cycle occurs
    // between successive samples... assuming a frequency range of 20Hz-20kHz this would
    // be on the order of 0.0004 to 0.4, so we multiply it to give us a meaningful range

    period = (voice->frequency * PHASEACC_MAX) / SAMPLE_RATE;

    // add this to the phase accumulator
    voice->phase_accumulator += period;

    // wrap the phase accumulator around
    voice->phase_accumulator &= (PHASEACC_MAX - 1);

    // and now get the sine output values for each of the allowed harmonics
    // to be elegant (and to improve the sound quality) we should really
    // interpolate between the current table entry and the next one by an amount
    // proportional to the position between the two entries, but this is just
    // an example so we won't bother for now
    uint32_t position = (voice->phase_accumulator * voice->instrument->length) / PHASEACC_MAX;

    // Interpolation happens because there are "gaps" that are between the phase
    // accumulator and the table.
    if (INTERPOLATION_ENABLED && (voice->instrument->flags & INSTRUMENT_CAN_INTERPOLATE))
    {
        // This is how far off we are.  I.e. the error.
        int32_t distance = voice->phase_accumulator - ((position * PHASEACC_MAX) / voice->instrument->length);

        // And this is how many "Gaps" there are total between two entries in the table
        const int32_t gap = PHASEACC_MAX / voice->instrument->length;

        v1 = voice->instrument->samples[position];
        position++;
        if (position >= voice->instrument->length)
            position -= voice->instrument->length;
        v2 = voice->instrument->samples[position];

        v1_weight = gap - distance;
        v2_weight = gap - v1_weight;

        output = ((v1 * v1_weight) + (v2 * v2_weight)) / gap;
    }
    else
    {
        output = voice->instrument->samples[position];
    }

    output = processADSR(voice, output);

    voice->note_timer++;
    return output;
}

static void note_on(struct ltc_voice *voice, uint32_t freq)
{
    fprintf(stderr, "Note on %p: %d\n", voice, freq);
    voice->frequency = freq;
    voice->note_timer = 0;
    voice->release_timer = 0;
}

static void note_off(struct ltc_voice *voice)
{
    voice->release_timer = voice->note_timer;
}

static void play_routine_step(struct ltc_sound_engine *engine) {
    int voice_num;
    for (voice_num = 0; voice_num < 2; voice_num++) {
        struct ltc_voice *voice = &engine->voices[voice_num];
        if (voice->countdown == 0) {
            uint16_t op = voice->pattern[voice->pattern_offset++];
            if ((op & 0xf000) == 0x8000) {
                int effect_num = (op >> 8) & 0x7f;
                if (effect_num >= FINAL_EFFECT) {
                    fprintf(stderr, "Invalid effect");
                }
                effect_lut[effect_num](engine, voice_num, op & 0xff);
            }
            else if ((op & 0xf000) == 0x9000) {
                voice->tick_lut[(op & 0xf00) >> 8] = (op & 0xff);
            }
            else if ((op & 0xf000) == 0xa000) {
                setAttackTime(engine, voice_num, op & 0xfff);
            }
            else if ((op & 0xf000) == 0xb000) {
                setDecayTime(engine, voice_num, op & 0xfff);
            }
            else if ((op & 0xf000) == 0xc000) {
                setReleaseTime(engine, voice_num, op & 0xfff);
            }
            else {
                if (op & 0x3f) {
                    note_on(voice, note_lut[op & 0x3f]);
                    voice->release_timer = voice->tick_lut[(op >> 7) & 0xf];// * engine->loops_per_tick;
                }
                else {
                    note_off(voice);
                }
                fprintf(stderr, "Setting countdown to %d\n", voice->tick_lut[(op >> 11) & 0xf] * engine->loops_per_tick);
                voice->countdown = voice->release_timer + (voice->tick_lut[(op >> 11) & 0xf] * engine->loops_per_tick);
            }
        }
        if (voice->countdown)
            voice->countdown--;
    }
}

void loop(void)
{
    // If a sample is still in the buffer, don't do anything.
    if (sample_queued)
        return;

    play_routine_step(&engine);

    next_sample = 0;
    next_sample += get_sample(&engine.voices[0]);
    next_sample += get_sample(&engine.voices[1]);
    sample_queued = 1;

    {
        int32_t scaled_sample = next_sample + 129;
        if (scaled_sample > 255)
            scaled_sample = 255;
        if (scaled_sample < 1)
            scaled_sample = 1;
        putchar(scaled_sample);
        sample_queued = 0;
    }
}

int main(int argc, char **argv) {
    setup();
    while (1) {
        loop();
        global_tick_counter++;
    }
    return 0;
}
