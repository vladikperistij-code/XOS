#ifndef GUI_H
#define GUI_H

#define WHITE_ON_BLACK 0x0F  // 0 - чорний фон, F - білий текст
#define GREEN_ON_BLACK 0x0A  // 0 - чорний фон, A - зелений текст
#define RED_ON_BLACK   0x0C
#define YELLOW_ON_BLACK 0x0E

void clear_screen();
void kprint(char* message);
void kprint_color(char* message, int color);
void print_char(char c);
void kprint_int(int n);
void kinput(char* buffer);
char get_char_from_scancode(unsigned char scancode);
void gui_set_app_style(char icon, char* title);
void gui_draw_mouse(int x, int y);
int strcmp(char* s1, char* s2);
int strlen(char* s);

#endif
