/**
 * buffer: reserved space to allow for certain byte values to be drawn
 * 
 * 2 reserved buffers:
 * back buffer (does not display to the user) -> draw into something without displaying the drawing process
 * front buffer (does display to the user)    -> display a fully drawn buffer to the user 
 */
#include <inttypes.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

// color helper (packing 4 u8s into u32 RRGGBBAA)
static inline u32 color(u8 r, u8 g, u8 b, u8 a) {
    return (r << 24 | g << 16 | b << 8 | a);
}

// color helpers
#define COLOR_BLACK color(0,   0,   0,   255)
#define COLOR_WHITE color(255, 255, 255, 255)
#define COLOR_RED   color(255, 0,   0,   255)
#define COLOR_GREEN color(0,   255, 0,   255)
#define COLOR_BLUE  color(0,   0,   255, 255)

// vector types
typedef struct {
    u16 x, y;
} Vec2;

typedef struct {
    u16 x, y, z;
} Vec3; // unused for rn (will when z buffering comes in)

// buffer itself
typedef struct {
    u32* data;  // 8 bytes
    u16 width;  // 2 bytes
    u16 height; // 2 bytes

    // u8 padding[4]; // 4 bytes padding, explicit
} Buffer;

// sanity check
static_assert(sizeof(Buffer) == 16, "buffer size doesn't match the expected layout");

// renderer struct
typedef struct {
    // these two will be flipped and the one referenced to front will be displayed to the user
    Buffer* front; // 8 bytes
    Buffer* back;  // 8 bytes

    u16 width;     // 2 bytes
    u16 height;    // 2 bytes

    // potentially add an optional fps cap:
    // u16 fps; // 2 bytes

    // u8 padding[4]; // 4 bytes padding, explicit
} Renderer;

// another sanity check
static_assert(sizeof(Renderer) == 24, "buffer size doesn't match the expected layout");

// buffer lifecycle
Buffer* make_buffer(u16 width, u16 height);      // initialize
void    destroy_buffer(Buffer* buf);             // free
void    clear_buffer(Buffer* buf, u32 color);    // fill a provided buffer with a certain color

// renderer lifecycle
Renderer* make_renderer(u16 width, u16 height);   // initialize
void      destroy_renderer(Renderer* r);          // free
void      clear_renderer(Renderer* r, u32 color); // fills the back buffer with a certain color
void      flip_renderer(Renderer* r);             // swaps back and front

// drawing helpers
void draw_pixel(Renderer* r, u16 x, u16 y, u32 col);
void draw_rectangle(Renderer* r, u16 x, u16 y, u16 w, u16 h, u32 col);
void draw_tri(Renderer* r, Vec2 a, Vec2 b, Vec2 c, u32 col);

// helpers we might need
// void renderer_write_ppm(Renderer* r);  // dump renderer content to stdout using a portable pixelmap