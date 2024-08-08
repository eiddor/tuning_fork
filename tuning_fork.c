// include standard modules
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
//#include <string.h>
#include <stdlib.h>

//include GUI modules
#include <gui/gui.h>

//MAYBE remove these when I'm done
#include <gui/elements.h>
#include <gui/canvas.h>

//include View Dispatcher, Scene Manager, and modules
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>

//MAYBE Do I need these?  Probably not.
#include <notification/notification.h>
#include <notification/notification_messages.h>

//define notes and tunings in these files
//I need to dive into tunings.h and categorize them somehow.
//#include "notes.h"
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

//used by old code to keep track of which page the user is on - I can probably get rid of this once
//everything else works but this could be useful for development

enum Page { 
    Tunings, 
    Notes 
};

//Declare my TuningForkState struct for tracking state
typedef struct {
    FuriMutex* mutex;
    bool playing;
    enum Page page;
    //might need an enum before this next line
    TuningCategory category;
    int current_tuning_note_index;
    int current_tuning_index;
    float volume;
    TUNING tuning;
} TuningForkState;

// Function to get current tuning
static TUNING current_tuning(TuningForkState* tuningForkState) {
    return tuningForkState->tuning;
}

// Function to get current tuning category

static TuningCategory get_current_tuning_category(TuningForkState* tuningForkState) {
    return current_tuning(tuningForkState).category;
}

// Function to get current tuning note/string (as in string on an instrument)
static NOTE current_tuning_note(TuningForkState* tuningForkState) {
    return current_tuning(tuningForkState).notes[tuningForkState->current_tuning_note_index];
}

// Get the current note's frequency from notes.h
static float current_tuning_note_freq(TuningForkState* tuningForkState) {
    return current_tuning_note(tuningForkState).frequency;
}

// Get the current note's name
static void current_tuning_note_label(TuningForkState* tuningForkState, char* outNoteLabel) {
    for(int i = 0; i < 20; ++i) {
        outNoteLabel[i] = current_tuning_note(tuningForkState).label[i];
    }
}

// The name of our current tuning

static void current_tuning_label(TuningForkState* tuningForkState, char* outTuningLabel) {
    for(int i = 0; i < 20; ++i) {
        outTuningLabel[i] = current_tuning(tuningForkState).label[i];
    }
}

// Updates the current tuning and tuning note
static void updateTuning(TuningForkState* tuning_fork_state) {
    tuning_fork_state->tuning = TuningList[tuning_fork_state->current_tuning_index];
    tuning_fork_state->current_tuning_note_index = 0;
}

//Move to the next tuning (loop around if at the end) and update tuning
static void next_tuning(TuningForkState* tuning_fork_state) {
    if(tuning_fork_state->current_tuning_index == TUNINGS_COUNT - 1) {
        tuning_fork_state->current_tuning_index = 0;
    } else {
        tuning_fork_state->current_tuning_index += 1;
    }
    updateTuning(tuning_fork_state);
}

//Move to the previous tuning (loop around if at the end) and update tuning
static void prev_tuning(TuningForkState* tuning_fork_state) {
    if(tuning_fork_state->current_tuning_index - 1 < 0) {
        tuning_fork_state->current_tuning_index = TUNINGS_COUNT - 1;
    } else {
        tuning_fork_state->current_tuning_index -= 1;
    }
    updateTuning(tuning_fork_state);
}

//Move to the next note (loop around if at the end)
static void next_note(TuningForkState* tuning_fork_state) {
    if(tuning_fork_state->current_tuning_note_index ==
        current_tuning(tuning_fork_state).notes_length - 1) {
        tuning_fork_state->current_tuning_note_index = 0;
    } else {
        tuning_fork_state->current_tuning_note_index += 1;
    }
}

//Move to the previous note (loop around if at the end)
static void prev_note(TuningForkState* tuning_fork_state) {
    if(tuning_fork_state->current_tuning_note_index == 0) {
        tuning_fork_state->current_tuning_note_index =
            current_tuning(tuning_fork_state).notes_length - 1;
    } else {
        tuning_fork_state->current_tuning_note_index -= 1;
    }
}

//Play note
static void play(TuningForkState* tuning_fork_state) {
    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(1000)) {
        furi_hal_speaker_start(
            current_tuning_note_freq(tuning_fork_state), tuning_fork_state->volume);
    }
}

//Stop playing note and release speaker device
static void stop() {
    if(furi_hal_speaker_is_mine()) {
        furi_hal_speaker_stop();
        furi_hal_speaker_release();
    }
}

/* //I need to figure out how to handle up/down
//Increase volume
static void increase_volume(TuningForkState* tuning_fork_state) {
    if(tuning_fork_state->volume < 1.0f) {
        tuning_fork_state->volume += 0.1f;
    }
}

//Decrease volume
static void decrease_volume(TuningForkState* tuning_fork_state) {
    if(tuning_fork_state->volume > 0.0f) {
        tuning_fork_state->volume -= 0.1f;
    }
}

//cycle play
static void replay(TuningForkState* tuning_fork_state) {
    stop();
    play(tuning_fork_state);
}
 */
//define our App object
typedef struct App {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Widget* widget;
    TuningForkState* tuning_fork_state;
} App;

// menu item indices

typedef enum {
    TuningForkMainMenuSceneGuitar,
    TuningForkMainMenuSceneBass,
    TuningForkMainMenuSceneMisc,
} TuningForkMainMenuSceneIndex;

// custom events array
typedef enum {
    TuningForkMainMenuSceneGuitarEvent,
    TuningForkMainMenuSceneBassEvent,
    TuningForkMainMenuSceneMiscEvent,
} TuningForkMainMenuEvent;

// category events array
typedef enum {
    TuningForkCategorySelectEvent,
    TuningForkCategoryPrevEvent,
    TuningForkCategoryNextEvent,
} TuningForkCategoryCustomEvent;

//tuning events array
typedef enum {
    TuningForkTuningSelectEvent,
    TuningForkTuningPrevEvent,
    TuningForkTuningNextEvent,
} TuningForkTuningCustomEvent;

//Main menu callback function
/*The code does the following:

When the menu item with the index TuningForkMainMenuSceneGuitar is selected, 
the function fires the TuningForkMainMenuSceneGuitarEvent custom event.
When the menu item with the index TuningForkMainMenuSceneBass is selected, 
the function fires the TuningForkMainMenuSceneBassEvent custom event.
When the menu item with the index TuningForkMainMenuSceneMisc is selected,
the function fires the TuningForkMainMenuSceneMiscEvent custom event.
These custom events will be handled in the tuning_fork_main_menu_scene_on_event function

*/

void tuning_fork_menu_callback(void* context, uint32_t index) {
    App* app = context;

    switch(index) {
    case TuningForkMainMenuSceneGuitar:
        view_dispatcher_send_custom_event(
            app->view_dispatcher, TuningForkMainMenuSceneGuitarEvent);
        break;
    case TuningForkMainMenuSceneBass:
        view_dispatcher_send_custom_event(app->view_dispatcher, TuningForkMainMenuSceneBassEvent);
        break;
    case TuningForkMainMenuSceneMisc:
        view_dispatcher_send_custom_event(app->view_dispatcher, TuningForkMainMenuSceneMiscEvent);
        break;
    }
}

void tuning_fork_category_callback(GuiButtonType result, InputType type, void* context) {
    App* app = context;
    switch(type) {
    case InputTypeShort:
        switch(result) {
        case GuiButtonTypeCenter:
            view_dispatcher_send_custom_event(app->view_dispatcher, TuningForkCategorySelectEvent);
            break;
        case GuiButtonTypeLeft:
            view_dispatcher_send_custom_event(app->view_dispatcher, TuningForkCategoryPrevEvent);
            break;
        case GuiButtonTypeRight:
            view_dispatcher_send_custom_event(app->view_dispatcher, TuningForkCategoryNextEvent);
            break;
        }
    default:
        break;
    }
}

void tuning_fork_tuning_callback(GuiButtonType result, InputType type, void* context) {
    App* app = context;
    switch(type) {
    case InputTypeShort:
        switch(result) {
        case GuiButtonTypeCenter:
            view_dispatcher_send_custom_event(app->view_dispatcher, TuningForkTuningSelectEvent);
            break;
        case GuiButtonTypeLeft:
            view_dispatcher_send_custom_event(app->view_dispatcher, TuningForkTuningPrevEvent);
            break;
        case GuiButtonTypeRight:
            view_dispatcher_send_custom_event(app->view_dispatcher, TuningForkTuningNextEvent);
            break;
        }
    default:
        break;
    }
}
//functions for each "event" (enter, event, exit) in each scene

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
        app->submenu, "Guitar", TuningForkMainMenuSceneGuitar, tuning_fork_menu_callback, app);
    submenu_add_item(
        app->submenu, "Bass", TuningForkMainMenuSceneBass, tuning_fork_menu_callback, app);
    submenu_add_item(
        app->submenu, "Misc", TuningForkMainMenuSceneMisc, tuning_fork_menu_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, TuningForkSubmenuView);
}

//Main Menu on_event function - This is where we handle inputs on the mainmenu
/*
The code does the following:

When the TuningForkMainMenuSceneGuitarEvent custom event is received,
the function navigates to the TuningForkCategoryScene scene.
When the TuningForkMainMenuSceneBassEvent custom event is received, 
the function navigates to the TuningForkCategoryScene scene.
When the TuningForkMainMenuSceneMiscEvent custom event is received, 
the function navigates to the TuningForkCategoryScene scene.
The function returns true if the event was consumed, otherwise it returns false.
*/

bool tuning_fork_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case TuningForkMainMenuSceneGuitarEvent:
            app->tuning_fork_state->category = Guitar;
            scene_manager_next_scene(app->scene_manager, TuningForkCategoryScene);
            consumed = true;
            break;
        case TuningForkMainMenuSceneBassEvent:
            app->tuning_fork_state->category = Bass;
            scene_manager_next_scene(app->scene_manager, TuningForkCategoryScene);
            consumed = true;
            break;
        case TuningForkMainMenuSceneMiscEvent:
            app->tuning_fork_state->category = Misc;
            scene_manager_next_scene(app->scene_manager, TuningForkCategoryScene);
            consumed = true;
            break;
        default:
            consumed = false;
            break;
        }
        break;
    default:
        break;
    }
    return consumed;
}
void tuning_fork_main_menu_scene_on_exit(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
}

static void tuning_fork_category_scene_helper(App* context) {
    App* app = context;
    char tuningLabel[20];
    current_tuning_label(app->tuning_fork_state, tuningLabel);
    widget_reset(app->widget);
    widget_add_string_element(
        app->widget, 64, 25, AlignCenter, AlignCenter, FontPrimary, tuningLabel);
    widget_add_button_element(
        app->widget, GuiButtonTypeLeft, "Prev", tuning_fork_category_callback, app);
    widget_add_button_element(
        app->widget, GuiButtonTypeCenter, "Select", tuning_fork_category_callback, app);
    widget_add_button_element(
        app->widget, GuiButtonTypeRight, "Next", tuning_fork_category_callback, app);
}

void tuning_fork_category_scene_on_enter(void* context) {
    App* app = context;
    tuning_fork_category_scene_helper(app);
    view_dispatcher_switch_to_view(app->view_dispatcher, TuningForkWidgetView);
}

bool tuning_fork_category_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;

    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case TuningForkCategorySelectEvent:
            scene_manager_next_scene(app->scene_manager, TuningForkTuningScene);
            consumed = true;
            break;
        case TuningForkCategoryPrevEvent:
            prev_tuning(app->tuning_fork_state);
            tuning_fork_category_scene_helper(app);
            consumed = true;
            break;
        case TuningForkCategoryNextEvent:
            next_tuning(app->tuning_fork_state);
            tuning_fork_category_scene_helper(app);
            consumed = true;
            break;
        }
        break;
    default:
        break;
    }
    return consumed; // Did we handle this?
}

void tuning_fork_category_scene_on_exit(void* context) {
    App* app = context;
    widget_reset(app->widget);
}

static void tuning_fork_tuning_scene_helper(App* context) {
    App* app = context;
    char tuningNoteLabel[20];
    current_tuning_note_label(app->tuning_fork_state, tuningNoteLabel);
    widget_reset(app->widget);
    widget_add_string_element(
        app->widget,
        64,
        25,
        AlignCenter,
        AlignCenter,
        FontPrimary,
        tuningNoteLabel
    );
    widget_add_button_element(
        app->widget,
        GuiButtonTypeLeft,
        "Prev",
        tuning_fork_tuning_callback,
        app);
    widget_add_button_element(
        app->widget,
        GuiButtonTypeCenter,
        "Play",
        tuning_fork_tuning_callback,
        app);
    widget_add_button_element(
        app->widget,
        GuiButtonTypeRight,
        "Next",
        tuning_fork_tuning_callback,
        app);
}

void tuning_fork_tuning_scene_on_enter(void* context) {
    App* app = context;
    tuning_fork_tuning_scene_helper(app);
    view_dispatcher_switch_to_view(app->view_dispatcher, TuningForkWidgetView);
}
bool tuning_fork_tuning_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;

    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case TuningForkTuningSelectEvent:
            app->tuning_fork_state->playing = !app->tuning_fork_state->playing;
            if(app->tuning_fork_state->playing) {
                play(app->tuning_fork_state);
            } else {
                stop();
            }
            consumed = true;
            break;
        case TuningForkTuningPrevEvent:
            prev_note(app->tuning_fork_state);
            tuning_fork_tuning_scene_helper(app);
            consumed = true;
            break;
        case TuningForkTuningNextEvent:
            next_note(app->tuning_fork_state);
            tuning_fork_tuning_scene_helper(app);
            consumed = true;
            break;
        }
        break;
    default:
        break;
    }
    return consumed; // Did we handle this?
}
void tuning_fork_tuning_scene_on_exit(void* context) {
    App* app = context;
    widget_reset(app->widget);
}

//array of on_enter handlers/stub functions
void (*const tuning_fork_scene_on_enter_handlers[])(void*) = {
    tuning_fork_main_menu_scene_on_enter,
    tuning_fork_category_scene_on_enter,
    tuning_fork_tuning_scene_on_enter};

//array of on_event handlers/stub functions

bool (*const tuning_fork_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    tuning_fork_main_menu_scene_on_event,
    tuning_fork_category_scene_on_event,
    tuning_fork_tuning_scene_on_event};

//array of on_exit handlers/stub functions

void (*const tuning_fork_scene_on_exit_handlers[])(void*) = {
    tuning_fork_main_menu_scene_on_exit,
    tuning_fork_category_scene_on_exit,
    tuning_fork_tuning_scene_on_exit};

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
    app->scene_manager = scene_manager_alloc(&tuning_fork_scene_manager_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, tuning_fork_custom_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, tuning_fork_back_event_callback);
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, TuningForkSubmenuView, submenu_get_view(app->submenu));
    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, TuningForkWidgetView, widget_get_view(app->widget));
    return app;
}

TuningForkState* tuning_fork_state_alloc() {
    TuningForkState* state = malloc(sizeof(TuningForkState));
    return state;
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
    free(app->tuning_fork_state);
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

// Initialize our state variable or array or struct or whatever

static void tuning_fork_state_init(TuningForkState* const tuning_fork_state) {
    tuning_fork_state->playing = false;
    tuning_fork_state->page = Tunings;
    tuning_fork_state->category = Guitar;
    tuning_fork_state->volume = 1.0f;
    tuning_fork_state->tuning = GuitarStandard6;
    tuning_fork_state->current_tuning_index = 0;
    tuning_fork_state->current_tuning_note_index = 0;
}

// Initialize our app

int32_t tuning_fork_app(void* p) {
    UNUSED(p);

    App* app = app_alloc();
    app->tuning_fork_state = tuning_fork_state_alloc();
    tuning_fork_state_init(app->tuning_fork_state);

    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(app->scene_manager, TuningForkMainMenuScene);
    view_dispatcher_run(app->view_dispatcher);

    app_free(app);

    return 0;
}