#pragma once

static const int max_entries = 8;

struct spawn_info {
    int type;
    int count;
    float interval;
};

struct wave_data {
    spawn_info entries[max_entries];
    int count;
};

extern wave_data waves_list[5];

void init_waves();