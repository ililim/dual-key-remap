#ifndef INPUT_H
#define INPUT_H

enum Direction {
    UP,
    DOWN,
};

void send_input(int scan_code, int virt_code, enum Direction direction);

#endif
