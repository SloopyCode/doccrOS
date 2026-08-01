#include <ui16.h>
#include <ui16buttons.h>
#include <libdesktop.h>
#include <unistd.h>

#define UI16DEMO_CONTENT_WIDTH  320
#define UI16DEMO_CONTENT_HEIGHT 200

int main(void)
{
    int window_width;
    int window_height;
    #define APP_TITLE "ui16 demo"

    desktopWindowSizeForContent(
        UI16DEMO_CONTENT_WIDTH,
        UI16DEMO_CONTENT_HEIGHT,
        DT_WIN,
        &window_width,
        &window_height
    );

    desktop.createWindow(
        APP_TITLE,

        150,   150,
        window_width,
        window_height,
        DT_WIN
    );

    unsigned int *window_buffer = desktop.allocFramebuffer(UI16DEMO_CONTENT_WIDTH, UI16DEMO_CONTENT_HEIGHT);

    for (;;)
    {
        ui16_setRoot(
            style(
                width(fill),
                height(fill),
                bg(rgb(64, 64, 64))
            ),
            window_buffer,
            UI16DEMO_CONTENT_WIDTH,
            UI16DEMO_CONTENT_HEIGHT
        );

        ui16_container(
            style(
                layout(row),
                width(fill),
                height(fill)
            )
        ) {
            ui16_container( style(
                width(percent(30)),
                height(fill),
                bg(rgb(30, 30, 30)),
                padding(8),
                gap(6),
                layout(column),
                font(fontBold)
            )) {
                ui16_button("Settings");
                ui16_button("Save");
                ui16_label(style(font(fontBold)), "font test");
            };

            ui16_container( style(
                width(fill),
                height(fill),
                padding(8),
                layout(column),
                gap(6)
            )) {
                ui16_label(style(font(fontBold)), "Hello from ui16");
                ui16_button("Open");
            };
        }

        ui16_frame();

        desktop.presentFrame();

        dt_event_t incoming_events[8];
        desktop.pollEvents(incoming_events, 8);

        yield();
    }

    return 0;
}