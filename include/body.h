#include <stdlib.h>

typedef struct {  
    float x; 
    float y; 
    float z;
} Vec3d;

typedef struct {float m; Vec3d pos; Vec3d v; Vec3d a; } body;