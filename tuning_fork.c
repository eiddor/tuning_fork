// include standard modules
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <string.h>
#include <stdlib.h>

//include GUI modules
#include <gui/gui.h>

//remove these when I'm done
#include <gui/elements.h>
#include <gui/canvas.h>

//include View Dispatcher, Scene Manager, and modules
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>

#include <notification/notification.h>
#include <notification/notification_messages.h>

//define notes and tunings in these files
#include "notes.h"
#include "tunings.h"

//define our scenes
typedef enum {
    TuningForkMainMenuScene,
    TuningForkCategoryScene,
    TuningForkTuningScene,
    TuningForkSceneCount
} TuningForkScene;

//define our views
typedef enum { TuningForkSubmenuView, TuningForkWidgetView } TuningForkView;

//used by old code
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

//define our App object
typedef struct App {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Widget* widget;
} App;

//stub functions for each event (enter, event, exit) in each scene
void tuning_fork_main_menu_scene_on_enter(void* context) {
    UNUSED(context);
}
bool tuning_fork_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false; // event not handled.
}
void tuning_fork_main_menu_scene_on_exit(void* context) {
    UNUSED(context);
}

void tuning_fork_category_scene_on_enter(void* context) {
    UNUSED(context);
}
bool tuning_fork_category_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false; // event not handled.
}
void tuning_fork_category_scene_on_exit(void* context) {
    UNUSED(context);
}

void tuning_fork_tuning_input_scene_on_enter(void* context) {
    UNUSED(context);
}
bool tuning_fork_tuning_input_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false; // event not handled.
}
void tuning_fork_tuning_input_scene_on_exit(void* context) {
    UNUSED(context);
}

//array of on_enter handlers/stub functions
void (*const tuning_fork_scene_on_enter_handlers[])(void*) = {
    tuning_fork_main_menu_scene_on_enter,
    tuning_fork_category_scene_on_enter,
    tuning_fork_tuning_input_scene_on_enter};

//array of on_event handlers/stub functions

bool (*const tuning_fork_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    tuning_fork_main_menu_scene_on_event,
    tuning_fork_category_scene_on_event,
    tuning_fork_tuning_input_scene_on_event};

//array of on_exit handlers/stub functions

void (*const tuning_fork_scene_on_exit_handlers[])(void*) = {
    tuning_fork_main_menu_scene_on_exit,
    tuning_fork_category_scene_on_exit,
    tuning_fork_tuning_input_scene_on_exit};

int32_t tuning_fork_app(void* p) {
    UNUSED(p);
    furi_delay_ms(5000);
    return 0;
}