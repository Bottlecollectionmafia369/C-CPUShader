#include "defines.h"
#include "win.h"
#include "shaders/sphere_tracing.h"
#include "shaders/glass_disks.h"

vec4_t main_image(vec2_t fragCoord, vec2_t resolution, float time, uint frame)
{
    return glass_disks_main(fragCoord, resolution, time, frame);
}

int main(void)
{
    //Init Window
    window_create("Renderer", 700, 450);

    //Render Loop with 10 threads and temporal accumulation enabled
    window_run(main_image, 10, true);

    return 0;
}