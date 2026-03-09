#pragma once

typedef vec4_t (*RenderFunc)(vec2_t fragCoord, vec2_t resolution, float time, uint frame);

void  window_create   (const char *title, int width, int height);
void  window_run      (RenderFunc render, int num_threads, bool temporal_accumulation);