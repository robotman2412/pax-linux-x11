
#include "main.h"

Display  *disp;
int       screen;
Window    window;
GC        gc;
XEvent    event;
KeySym    key;
char      text[256];
pax_buf_t buf;

static float hue = 128;
static bool mouse1 = false;
static bool mouse2 = false;
static int lx;
static int ly;

void event_loop();
void testing_loop();

int main(int argc, char **argv) {
    init_x();
    XWindowAttributes windowAttr;
    XGetWindowAttributes(disp, window, &windowAttr);
    pax_buf_init(&buf, NULL, windowAttr.width, windowAttr.height, PAX_BUF_32_8888ARGB);
    // pax_enable_multicore(0);
    
    // Test FONT stuff.
    // FILE *fd = fopen("/tmp/pax_font_test", "w+");
    // pax_store_font(fd, pax_font_saira_regular);
    // fseek(fd, 0, SEEK_SET);
    // FILE *fd = fopen("/home/julian/Downloads/sky mono(3).pax_font", "rb");
    // pax_font_t *font = pax_load_font(fd);
    // FILE *fd = fopen("/home/julian/Downloads/Sky mono.pax_font", "wb");
    // pax_store_font(fd, pax_font_sky_mono);
    // if (!font) return 1;
    // return;
    
    // Draw using the newly loaded font.
    // while (1) {
    //     pax_background(&buf, 0xff000000);
    //     pax_draw_rect(&buf, 0xffff0000, 5, 5, 10, 10);
    //     pax_draw_text(&buf, 0xffffffff, font, font->default_size, 5, 5, "The quick brown fox jumped over the lazy dog.");
    //     disp_flush();
    //     usleep(16000);
    // }
    testing_loop();
    return 0;
}

void event_loop() {
    XWindowAttributes windowAttr;
    XGetWindowAttributes(disp, window, &windowAttr);
    // look for events forever...
    while(1) {
        // get the next event and stuff it into our event variable.
        // Note:  only events we set the mask for are detected!
        XNextEvent(disp, &event);
        
        if (event.type == ConfigureNotify && (event.xconfigure.width != buf.width || event.xconfigure.height != buf.height)) {
            // The window was resized, resize the PAX buffer.
            pax_buf_destroy(&buf);
            XGetWindowAttributes(disp, window, &windowAttr);
            pax_buf_init(&buf, NULL, windowAttr.width, windowAttr.height, PAX_BUF_32_8888ARGB);
            redraw();
        } else if (event.type == Expose && event.xexpose.count == 0) {
            redraw();
        }
        if (event.type == ButtonPress) {
            if (event.xbutton.button == Button4) hue += 8;
            if (event.xbutton.button == Button5) hue -= 8;
            if (event.xbutton.button == Button1) mouse1 = true;
            if (event.xbutton.button == Button3) mouse2 = true;
            lx = event.xbutton.x;
            ly = event.xbutton.y;
            pax_draw_circle(&buf, pax_col_hsv(hue, 255, 255), event.xbutton.x, event.xbutton.y, 10);
            disp_sync();
        }
        if (event.type == ButtonRelease) {
            if (event.xbutton.button == Button1) mouse1 = false;
            if (event.xbutton.button == Button3) mouse2 = false;
        }
        if (event.type == MotionNotify) {
            float dx = event.xbutton.x - lx;
            float dy = event.xbutton.y - ly;
            float delta = dx * dx + dy * dy;
            
            if (mouse1 || mouse2) {
                int nIter = 1;
                if (delta > 4) {
                    nIter = delta / 2;
                }
                dx /= nIter;
                dy /= nIter;
                delta = dx * dx + dy * dy;
                float x = lx, y = ly;
                for (int i = 0; i < nIter; i++) {
                    x += dx;
                    y += dy;
                    if (mouse2) hue += delta;
                    pax_draw_circle(&buf, pax_col_hsv(hue, 255, 255), x, y, 10);
                }
                disp_sync();
                
                lx = event.xbutton.x;
                ly = event.xbutton.y;
            }
        }
    }
}

void testing_loop() {
    while (true) {
        uint64_t now = millis();
        pax_background(&buf, 0);
        
        float x = sinf(now % 8000 / 8000.0 * 2 * M_PI);
        float y = cosf(now % 8000 / 8000.0 * 2 * M_PI);
        float w = 10 + 10 * sinf(now % 6000 / 6000.0 * 2 * M_PI);
        float h = 10 + 10 * cosf(now % 9000 / 9000.0 * 2 * M_PI);
        // pax_enable_shape_aa = true;
        // pax_draw_rect(&buf, 0xffffffff, 90 + x*20, 80 + y*20, w, h);
        // pax_enable_shape_aa = false;
        // pax_draw_rect(&buf, 0xffffffff, 180 + x*20, 80 + y*20, w, h);
        
        pax_enable_shape_aa = true;
        pax_push_2d(&buf);
            pax_apply_2d(&buf, matrix_2d_translate(100, 100));
            pax_apply_2d(&buf, matrix_2d_rotate(now % 8000 / 8000.0 * 2 * M_PI));
            pax_apply_2d(&buf, matrix_2d_scale(20, 20));
            pax_draw_tri(&buf, 0xffffffff, -1, -1, 1, -1, 0, 1);
        pax_pop_2d(&buf);
        pax_enable_shape_aa = false;
        pax_push_2d(&buf);
            pax_apply_2d(&buf, matrix_2d_translate(180, 100));
            pax_apply_2d(&buf, matrix_2d_rotate(now % 8000 / 8000.0 * 2 * M_PI));
            pax_apply_2d(&buf, matrix_2d_scale(20, 20));
            pax_draw_tri(&buf, 0xffffffff, -1, -1, 1, -1, 0, 1);
        pax_pop_2d(&buf);
        
        disp_flush();
        usleep(16000);
    }
}

uint64_t millis() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void init_x() {
    // get the colors black and white (see section for details)
    unsigned long black, white;

    // use the information from the environment variable DISPLAY to create the X connection:
    disp   = XOpenDisplay((char *)0);
    screen = DefaultScreen(disp);
    black  = BlackPixel(disp, screen); // get color black
    white  = WhitePixel(disp, screen); // get color white

    // Once the display is initialized, create the window.
    // This window will be have be 200 pixels across and 300 down.
    // It will have the foreground white and background black.
    window = XCreateSimpleWindow(disp, DefaultRootWindow(disp), 0, 0, 280, 200, 5, white, black);

    // here is where some properties of the window can be set.
    // The third and fourth items indicate the name which appears at the top of the window and the name of the minimized window respectively.
    XSetStandardProperties(disp, window, "PAX on X11", "pax11.png", None, NULL, 0, NULL);

    // this routine determines which types of input are allowed in the input.
    // see the appropriate section for details...
    XSelectInput(disp, window, ExposureMask | ButtonPressMask | ButtonReleaseMask | StructureNotifyMask | PointerMotionMask);

    // create the Graphics Context
    gc = XCreateGC(disp, window, 0, 0);

    // here is another routine to set the foreground and background colors _currently_ in use in the window.
    XSetBackground(disp, gc, white);
    XSetForeground(disp, gc, black);

    // clear the window and bring it on top of the other windows
    XClearWindow(disp, window);
    XMapRaised(disp, window);
};

void close_x() {
    XFreeGC(disp, gc);
    XDestroyWindow(disp, window);
    XCloseDisplay(disp);
    exit(0);
}

void disp_flush() {
    // Very dum dum.
    pax_join();
    for (int y = 0; y < buf.height; y++) {
        for (int x = 0; x < buf.width; x++) {
            XSetForeground(disp, gc, pax_get_pixel(&buf, x, y));
            XDrawPoint(disp, window, gc, x, y);
        }
    }
    pax_mark_clean(&buf);
}

void disp_sync() {
    pax_join();
    for (int y = buf.dirty_y0; y <= buf.dirty_y1; y++) {
        for (int x = buf.dirty_x0; x <= buf.dirty_x1; x++) {
            XSetForeground(disp, gc, pax_get_pixel(&buf, x, y));
            XDrawPoint(disp, window, gc, x, y);
        }
    }
    pax_mark_clean(&buf);
}

void redraw() {
    pax_background(&buf, 0);
    
    // Center some text.
    const pax_font_t *font = pax_font_saira_condensed;
    const char *text = "Hello, paX!";
    
    pax_vec1_t dims = pax_text_size(font, 1, text);
    float scale = buf.width / dims.x;
    pax_draw_text(&buf, 0xffffffff, font, scale, 0, (buf.height - dims.y * scale) / 2, text);
    
    uint64_t now = millis();
    float x = sinf(now * M_PI / 2000.0);
    float y = sinf(now * M_PI / 1700.0);
    // pax_background(&buf, 0);
    pax_draw_rect(&buf, 0xffffffff, buf.width / 2.0 * (-1 + x), buf.height / 2.0 * (-1 + y), buf.width / 4.0, buf.height / 4.0);
    
    // And flush.
    disp_flush();
}
