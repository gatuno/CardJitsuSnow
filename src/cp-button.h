#ifndef __CP_BUTTON_H__
#define __CP_BUTTON_H__

extern int *cp_button_frames;
extern int *cp_button_refresh;

void cp_registrar_botones (int n);
void cp_registrar_boton (int n, int frame);
void cp_button_start (void);
void cp_button_motion (int map);
void cp_button_down (int map);
int cp_button_up (int map);

#endif /* __CP_BUTTON_H__ */

