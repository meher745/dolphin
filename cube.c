#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <float.h>

#define HEIGHT 32
#define WIDTH 140

#define CUBE_HALF_X 10
#define CUBE_HALF_Y 10
#define CUBE_HALF_Z 10

#define FOV 120.0f
#define FRAME_DELAY 50000

typedef struct { float x, y, z; } Vec3;

char buffer[HEIGHT][WIDTH];
float zBuffer[HEIGHT][WIDTH];
char background = ' ';

float A = 0, B = 0, C = 0;

float degreesToRadians(float degrees) {
    return degrees * M_PI / 180.0f;
}

Vec3 rotate(Vec3 p, float a, float b, float c) {
    float sinA = sin(a), cosA = cos(a);
    float sinB = sin(b), cosB = cos(b);
    float sinC = sin(c), cosC = cos(c);

    Vec3 r;
    r.x = p.x * (cosB * cosC) + p.y * (cosA * sinC + sinA * sinB * cosC) + p.z * (sinA * sinC - cosA * sinB * cosC);
    r.y = p.x * (-cosB * sinC) + p.y * (cosA * cosC - sinA * sinB * sinC) + p.z * (sinA * cosC + cosA * sinB * sinC);
    r.z = p.x * sinB + p.y * (-sinA * cosB) + p.z * (cosA * cosB);
    return r;
}

void writeToBuffer(Vec3 p, char ch, float scale, float distance) {
    Vec3 rotated = rotate(p, A, B, C);
    float z = rotated.z + distance;
    if (z <= 0.1f) return;

    int px = (int)(WIDTH / 2 + (rotated.x * scale / z));
    int py = (int)(HEIGHT / 2 - (rotated.y * scale / z));

    if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
        if (z < zBuffer[py][px]) {
            zBuffer[py][px] = z;
            buffer[py][px] = ch;
        }
    }
}

void clearBuffer() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            buffer[i][j] = background;
            zBuffer[i][j] = FLT_MAX;
        }
    }
}

void printBuffer() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) putchar(buffer[i][j]);
        putchar('\n');
    }
}

int main() {
    float fovRad = degreesToRadians(FOV);
    float scale = (WIDTH / 2) / tan(fovRad / 2);
    float distance = 3 * scale;

    while (1) {
        clearBuffer();
        for (int i = -CUBE_HALF_X; i <= CUBE_HALF_X; i++) {
            for (int j = -CUBE_HALF_Y; j <= CUBE_HALF_Y; j++) {
                for (int k = -CUBE_HALF_Z; k <= CUBE_HALF_Z; k++) {
                    writeToBuffer((Vec3){ CUBE_HALF_X, j, k }, '.', scale, distance);  // +X
                    writeToBuffer((Vec3){ -CUBE_HALF_X, j, k }, '#', scale, distance); // -X
                    writeToBuffer((Vec3){ i, CUBE_HALF_Y, k }, '^', scale, distance);  // +Y
                    writeToBuffer((Vec3){ i, -CUBE_HALF_Y, k }, 'v', scale, distance); // -Y
                    writeToBuffer((Vec3){ i, j, CUBE_HALF_Z }, 'O', scale, distance);  // +Z
                    writeToBuffer((Vec3){ i, j, -CUBE_HALF_Z }, 'X', scale, distance); // -Z
                }
            }
        }

        printBuffer();
        A += 0.03f;
        B += 0.04f;
        C += 0.02f;
        printf("\e[1;1H\e[2J");
        usleep(FRAME_DELAY);
    }
}

