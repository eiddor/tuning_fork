#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <string.h>
#include <stdlib.h>

#include <gui/gui.h>
#include <gui/elements.h>
#include <gui/canvas.h>

#include <notification/notification.h>
#include <notification/notification_messages.h>

#include "notes.h"
#include "tunings.h"

typedef enum {
    TuningForkMainMenuScene,
    TuningForkGuitarScene,
    TuningForkBassScene,
    TuningForkMiscScene,
    TuningForkTuningScene,
    TuningForkSceneCount,
} TuningForkScene;

enum Page { Tunings, Notes };

typedef struct {
    FuriMutex* mutex;
    bool playing;
    enum Page page;
    int current_tuning_note_index;
    int current_tuning_index;
    float volume;
    TUNING tuning;
} TuningForkState;

static TUNING current_tuning(TuningForkState* tuningForkState) {
    return tuningForkState->tuning;
}

static NOTE current_tuning_note(TuningForkState* tuningForkState) {
    return current_tuning(tuningForkState).notes[tuningForkState->current_tuning_note_index];
}

static float current_tuning_note_freq(TuningForkState* tuningForkState) {
    return current_tuning_note(tuningForkState).frequency;
}

static void current_tuning_note_label(TuningForkState* tuningForkState, char* outNoteLabel) {
    for(int i = 0; i < 20; ++i) {
        outNoteLabel[i] = current_tuning_note(tuningForkState).label[i];
    }
}

static void current_tuning_label(TuningForkState* tuningForkState, char* outTuningLabel) {
    for(int i = 0; i < 20; ++i) {
        outTuningLabel[i] = current_tuning(tuningForkState).label[i];
    }
}

static void updateTuning(TuningForkState* tuning_fork_state) {
    tuning_fork_state->tuning = TuningList[tuning_fork_state->current_tuning_index];
    tuning_fork_state->current_tuning_note_index = 0;
}

static void next_tuning(TuningForkState* tuning_fork_state) {
    if(tuning_fork_state->current_tuning_index == TUNINGS_COUNT - 1) {
        tuning_fork_state->current_tuning_index = 0;
    } else {
        tuning_fork_state->current_tuning_index += 1;
    }
    updateTuning(tuning_fork_state);
}

static void prev_tuning(TuningForkState* tuning_fork_state) {
    if(tuning_fork_state->current_tuning_index - 1 < 0) {
        tuning_fork_state->current_tuning_index = TUNINGS_COUNT - 1;
    } else {
        tuning_fork_state->current_tuning_index -= 1;
    }
    updateTuning(tuning_fork_state);
}

static void next_note(TuningForkState* tuning_fork_state) {
    if(tuning_fork_state->current_tuning_note_index ==
       current_tuning(tuning_fork_state).notes_length - 1) {
        tuning_fork_state->current_tuning_note_index = 0;
    } else {
        tuning_fork_state->current_tuning_note_index += 1;
    }
}

static void prev_note(TuningForkState* tuning_fork_state) {
    if(tuning_fork_state->current_tuning_note_index == 0) {
        tuning_fork_state->current_tuning_note_index =
            current_tuning(tuning_fork_state).notes_length - 1;
    } else {
        tuning_fork_state->current_tuning_note_index -= 1;
    }
}

static void increase_volume(TuningForkState* tuning_fork_state) {
    if(tuning_fork_state->volume < 1.0f) {
        tuning_fork_state->volume += 0.1f;
    }
}

static void decrease_volume(TuningForkState* tuning_fork_state) {
    if(tuning_fork_state->volume > 0.0f) {
        tuning_fork_state->volume -= 0.1f;
    }
}

static void play(TuningForkState* tuning_fork_state) {
    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(1000)) {
        furi_hal_speaker_start(
            current_tuning_note_freq(tuning_fork_state), tuning_fork_state->volume);
    }
}

static void stop() {
    if(furi_hal_speaker_is_mine()) {
        furi_hal_speaker_stop();
        furi_hal_speaker_release();
    }
}

static void replay(TuningForkState* tuning_fork_state) {
    stop();
    play(tuning_fork_state);
}
int32_t tuning_fork_app(void* p) {
    UNUSED(p);
    furi_delay_ms(5000);
    return 0;
}