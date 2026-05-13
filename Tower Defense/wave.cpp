#include "wave.h"

wave_data waves_list[5];

void init_waves() {
    waves_list[0].count = 1;
    waves_list[0].entries[0] = {0, 6, 1.2f};

    waves_list[1].count = 2;
    waves_list[1].entries[0] = {0, 5, 1.0f};
    waves_list[1].entries[1] = {1, 4, 0.7f};

    waves_list[2].count = 3;
    waves_list[2].entries[0] = {0, 6, 0.9f};
    waves_list[2].entries[1] = {1, 5, 0.6f};
    waves_list[2].entries[2] = {3, 2, 1.5f};

    waves_list[3].count = 3;
    waves_list[3].entries[0] = {1, 6, 0.5f};
    waves_list[3].entries[1] = {2, 3, 2.0f};
    waves_list[3].entries[2] = {3, 3, 1.0f};

    waves_list[4].count = 4;
    waves_list[4].entries[0] = {0, 8, 0.6f};
    waves_list[4].entries[1] = {1, 8, 0.4f};
    waves_list[4].entries[2] = {2, 4, 1.8f};
    waves_list[4].entries[3] = {3, 4, 0.8f};
}