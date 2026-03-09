# C-CPUShader
Minimal CPU shader multithreaded renderer written in pure C - raw Win32, no libraries, no abstraction layers. Pixels, threads, and math from scratch.</br>
I just felt like doing something with C.</br>
The math library isn't done yet, I just added the functions that I needed for the demo.</br>

## Compile
```bash
gcc src/*.c -o build/bin.exe -lgdi32
```

## Code
Simply init the window and choose the render function with the number of threads you want to use from your CPU.
The last parameter is to enable temporal accumulation for ray tracing shaders.

```c
#include "defines.h"
#include "win.h"
#include "sphere_tracing.h"

vec4_t main_image(vec2_t fragCoord, vec2_t resolution, float time, uint frame)
{
    return sphere_tracing_main(fragCoord, resolution, time, frame);
}

int main(void)
{
    //Init Window
    window_create("Renderer", 600, 600);

    //Render Loop with 10 threads and temporal accumulation enabled
    window_run(main_image, 10, true);

    return 0;
}
```

## Screenshots
<img width="600" height="600" alt="Renderer _ Rendering _ 8 0fps, 125 0ms _ Time _ 191 61s 3_9_2026 6_16_08 PM" src="https://github.com/user-attachments/assets/3687d4c3-d36d-4664-85d7-57085d8ecd13" />

