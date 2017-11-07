#ifndef GUI_H
#define GUI_H

typedef enum{
    GUI_SPLASHSCREEN,
    GUI_IDLE
} gui_state_t;

void gui_init(void);
void gui_loop(void);


#endif