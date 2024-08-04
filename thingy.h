typedef enum {
    ThingyButtonTypeLeft,
    ThingyButtonTypeCenter,
    ThingyButtonTypeRight,
    ThingyButtonTypeUp,
    ThingyButtonTypeDown,
    ThingyButtonTypeCount
} ThingyButtonType;

typedef void (*ThingyCallback)(ThingyButtonType result, InputType type, void* context);

typedef struct {
    FuriString* label; // Optional label.
    ButtonCallback callback;
    void* context;
} ThingyButtonModel;

typedef struct {
  // Array of buttons.  
    ThingyButtonModel buttons[ThingyButtonTypeCount];
  // Text to display in center of screen.
    FuriString main_text;
} ThingyModel;

typedef struct {
    View* view;
    void* context;
} Thingy;


// TODO: Implement these.

Thingy* thingy_alloc();

void thingy_reset(Thingy* thingy);

void thingy_add_string_element(Thingy* thingy, FuriString* label);

void thingy_add_button_element(Thingy* thingy, Direction direction, ThingyCallback callback, void* context);

void thingy_view_draw_callback(Canvas* canvas, void* model);

bool gui_widget_view_input_callback(InputEvent* event, void* context);

View* thingy_get_view(Thingy* thingy);

void thingy_free(Thingy* thingy);

//NOTE: Your draw_callback model will be a ThingyModel*.
//The other methods (like input) the void* context is actually a Thingy*.