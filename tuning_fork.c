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
typedef enum {
    TuningForkSubmenuView, 
    TuningForkWidgetView 
} TuningForkView;

//used by old code
enum Page {
    Tunings, 
    Notes 
};

typedef struct {
    FuriMutex* mutex;
    bool playing;
    enum Page page;
    int current_tuning_note_index;
    int current_tuning_index;
    float volume;
    TUNING tuning;
} TuningForkState;

// Old code to be brought in later
/*
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
*/
// Old code to be brought in later

//define our App object
typedef struct App {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Widget* widget;
} App;

// menu item indices

typedef enum {
    TuningForkMainMenuSceneGuitar,
    TuningForkMainMenuSceneBass,
    TuningForkMainMenuSceneMisc,
} TuningForkMainMenuSceneIndex;

// custom events array
typedef enum {
    TuningForksMainMenuSceneGuitarEvent,
    TuningForkMainMenuSceneGuitarEvent,
} TuningForkMainMenuEvent;

//stub menu callback function

void tuning_fork_menu_callback(void* context, uint32_t index) {
    UNUSED(context);
    UNUSED(index);
}

//functions for each event (enter, event, exit) in each scene

//Main menu scene
/*
The code does the following:

Reset the submenu.
Set the header of the submenu to "Tuning Categories".
Add a menu item with the text "Guitar" and the index TuningForkMainMenuSceneGuitar (0).
Add a menu item with the text "Bass" and the index TuningForkMainMenuSceneBass (1).
Add a menu item with the text "Misc" and the index TuningForkMainMenuSceneMisc (2).
*/

void tuning_fork_main_menu_scene_on_enter(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Tuning Categories");
    submenu_add_item(
        app->submenu, 
        "Guitar", 
        TuningForkMainMenuSceneGuitar, 
        tuning_fork_menu_callback, 
        app);
    submenu_add_item(
        app->submenu, 
        "Bass", 
        TuningForkMainMenuSceneBass, 
        tuning_fork_menu_callback, 
        app);
    submenu_add_item(
        app->submenu, 
        "Misc", 
        TuningForkMainMenuSceneMisc, 
        tuning_fork_menu_callback, 
        app);
    view_dispatcher_switch_to_view(app->view_dispatcher, TuningForkSubmenuView);
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

//define our scene manager handler object

static const SceneManagerHandlers tuning_fork_scene_manager_handlers = {
    .on_enter_handlers = tuning_fork_scene_on_enter_handlers,
    .on_event_handlers = tuning_fork_scene_on_event_handlers,
    .on_exit_handlers = tuning_fork_scene_on_exit_handlers,
    .scene_num = TuningForkSceneCount,
};

// custom callback function for any input other than back
// we delegate this to scene manager's custom event routine

static bool tuning_fork_custom_callback(void* context, uint32_t custom_event) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

// back event callback function for back button input
// we delegate this to scene manager's back event routine

bool tuning_fork_back_event_callback(void* context) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

/*
The app_alloc function does the following:

Create an app object and allocate memory for it.
Create a scene manager object and allocate memory for it.
Create a view dispatcher object and allocate memory for it.
Enable the view dispatcher queue to handle events.
Set the event callback context for the view dispatcher.
Set the custom event callback for the view dispatcher.
Set the navigation event callback for the view dispatcher (back event).
Create a submenu object and allocate memory for it.
Add the submenu view to the view dispatcher.
Create a widget object and allocate memory for it.
Add the widget view to the view dispatcher.
Return the app object.
*/
static App* app_alloc() {
    App* app = malloc(sizeof(App));
    app->scene_manager = scene_manager_alloc(
        &tuning_fork_scene_manager_handlers, 
        app);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, tuning_fork_custom_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, tuning_fork_back_event_callback);
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, 
        TuningForkSubmenuView, 
        submenu_get_view(app->submenu));
    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, 
        TuningForkWidgetView, 
        widget_get_view(app->widget));
    return app;
}

/*
app_free function to free memory allocated for our application. 
This function will remove all of the views from the view dispatcher, 
and then free the memory allocated for the scene manager, view dispatcher, 
submenu, and widget.
*/

static void app_free(App* app) {
    furi_assert(app);
    view_dispatcher_remove_view(app->view_dispatcher, TuningForkSubmenuView);
    view_dispatcher_remove_view(app->view_dispatcher, TuningForkWidgetView);
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);
    submenu_free(app->submenu);
    widget_free(app->widget);
    free(app);
}

/*
Our entry point.
This code does the following:

Allocate memory for the app object.
Give us a copy of the GUI object.
Attach the view dispatcher to the GUI.
Navigate to the main menu scene.
Run the view dispatcher (which won't return until the user presses BACK enough times to exit the application).
Free memory allocated for the app object.
Return 0 to indicate successful exit.

*/
int32_t tuning_fork_app(void* p) {
    UNUSED(p);
    App* app = app_alloc();

    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(
        app->view_dispatcher, 
        gui, 
        ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(
        app->scene_manager, 
        TuningForkMainMenuScene);
    view_dispatcher_run(app->view_dispatcher);

    app_free(app);
    return 0;
}