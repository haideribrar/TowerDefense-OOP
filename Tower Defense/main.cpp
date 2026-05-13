#include "game.h"
#include "map.h"
#include <cmath>
#include <string>

best_score best_scores[3] = {{0,0},{0,0},{0,0}};

void save_scores() {
    FILE* f = fopen("scores.txt", "w");
    if (!f) 
    {
        return;
    }
    fprintf(f, "# Tower Defense High Scores\n");
    for (int i = 0; i < 3; i++)
    {
        fprintf(f, "map%d %d %d\n", i, best_scores[i].wave, best_scores[i].gold);
    }
    fclose(f);
}

void load_scores() {
    FILE* f = fopen("scores.txt", "r");
    if (!f) 
    {
        return;
    }
    char buf[64];
    while (fgets(buf, sizeof(buf), f)) 
    {
        if (buf[0] == '#') 
        {
            continue;
        }
        int idx, wave, gold;
        if (sscanf(buf, "map%d %d %d", &idx, &wave, &gold) == 3 && idx >= 0 && idx < 3)
        {
            best_scores[idx] = {wave, gold};
        }
    }
    fclose(f);
}

Game::Game() {
    for (int i = 0; i < MAX_ENEMIES; i++) 
    {
        enemies[i]     = nullptr;
    }
    for (int i = 0; i < MAX_TOWERS; i++)
    {
        towers[i]      = nullptr;
    }
    for (int i = 0; i < MAX_PROJECTILES; i++) 
    {
        projectiles[i] = nullptr;
    }
}

void Game::reset(int map) {
    map_idx = map; gold = 200; lives = 5; wave = 0;
    wave_running = false; spawn_phase = 0; spawn_count = 0; spawn_timer = 0;
    selected_tower = -1; hovered_tower = -1; inspect_tower = nullptr;
    for (int i = 0; i < enemy_count; i++) 
    { 
        delete enemies[i];     
        enemies[i] = nullptr; 
    }
    enemy_count  = 0;
    for (int i = 0; i < tower_count; i++) 
    { 
        delete towers[i];      
        towers[i] = nullptr; 
    } 
    tower_count  = 0;
    for (int i = 0; i < proj_count; i++) 
    { 
        delete projectiles[i]; 
        projectiles[i] = nullptr; 
    } 
    proj_count   = 0;
    expl_count = 0;
    screen = Screen::GAME;
}

Enemy* spawn_enemy(int type, const path_t* path) {
    switch (type) {
        case 0: return new FastEnemy(path);
        case 1: return new BasicEnemy(path);
        case 2: return new TankEnemy(path);
        case 3: return new ShieldedEnemy(path);
        default:return new FastEnemy(path);
    }
}

float dist(float ax, float ay, float bx, float by) {
    float dx = ax - bx, dy = ay - by;
    return sqrtf(dx*dx + dy*dy);
}

void draw_text_centered(const std::string& s, int cx, int y, int fs, Color c) {
    int w = MeasureText(s.c_str(), fs);
    DrawText(s.c_str(), cx - w/2, y, fs, c);
}

void draw_left_panel(Game& g) {
    DrawRectangle(0, 0, INFO_W, SH, {20,22,28,230});
    DrawRectangleLines(0, 0, INFO_W, SH, {60,60,80,255});
    DrawText("TOWER INFO", 8, 10, 16, LIGHTGRAY);
    DrawLine(0, 30, INFO_W, 30, {60,60,80,255});

    if (g.inspect_tower) 
    {
        Tower* t = g.inspect_tower;
        draw_text_centered(t->get_name(), INFO_W/2, 40, 15, YELLOW);
        DrawText(("Level: " + std::to_string(t->get_level() + 1)).c_str(), 10, 62, 14, WHITE);
        DrawText(("Dmg:  "  + std::to_string(t->get_damage())).c_str(),    10, 80, 14, WHITE);
        DrawText(("Range:"  + std::to_string((int)t->get_range())).c_str(),10, 98, 14, WHITE);
        DrawLine(0, 138, INFO_W, 138, {60,60,80,200});
        DrawText("Description:", 8, 142, 13, LIGHTGRAY);
        std::string desc = t->get_description();
        int dy = 158;
        std::string line;
        for (char c : desc) 
        {
            if (c == '\n') 
            { 
                DrawText(line.c_str(), 8, dy, 13, {200,220,200,255}); 
                dy += 16; 
                line = ""; 
            }
            else line += c;
        }
        if (!line.empty()) 
        {
            DrawText(line.c_str(), 8, dy, 13, {200,220,200,255});
        }

        if (t->can_upgrade()) 
        {
            int uc = t->get_upgrade_cost();
            Color bc = (g.gold >= uc) ? Color{50,180,80,255} : Color{100,40,40,255};
            DrawRectangle(10, SH-80, INFO_W-20, 32, bc);
            DrawRectangleLines(10, SH-80, INFO_W-20, 32, WHITE);
            draw_text_centered("Upgrade $" + std::to_string(uc), INFO_W/2, SH-72, 14, WHITE);
            if (g.gold >= uc && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
            {
                Vector2 mp = GetMousePosition();
                if (mp.x >= 10 && mp.x <= INFO_W-10 && mp.y >= SH-80 && mp.y <= SH-48)
                { 
                    g.gold -= uc; t->upgrade(); 
                }
            }
        } 
        else 
        {
            DrawRectangle(10, SH-80, INFO_W-20, 32, {60,60,60,200});
            draw_text_centered("MAX LEVEL", INFO_W/2, SH-72, 14, GOLD);
        }
    } 
    else if (g.hovered_tower >= 0) 
    {
        int i = g.hovered_tower;
        DrawRectangle(4, 36, INFO_W-8, 22, {30,32,42,220});
        DrawRectangleLines(4, 36, INFO_W-8, 22, tower_cols[i]);
        draw_text_centered(tower_names[i], INFO_W/2, 40, 15, tower_cols[i]);
        DrawRectangle(10, 64, 18, 18, tower_cols[i]);
        DrawText("Cost:", 34, 66, 13, LIGHTGRAY);
        DrawText(("$" + std::to_string(tower_costs[i])).c_str(), 34, 80, 14, GOLD);
        DrawText(("Dmg:  " + std::to_string(tower_dmg[i])).c_str(),   10, 100, 14, WHITE);
        DrawText(("Range:" + std::to_string(tower_range[i])).c_str(), 10, 118, 14, WHITE);
        DrawLine(0, 140, INFO_W, 140, {60,60,80,200});
        DrawText("Description:", 8, 145, 13, LIGHTGRAY);
        std::string desc = tower_descs[i];
        int dy = 162;
        std::string line;
        for (char c : desc) 
        {
            if (c == '\n') 
            { 
                DrawText(line.c_str(), 8, dy, 13, {200,220,200,255}); 
                dy += 16; 
                line = ""; 
            }
            else line += c;
        }
        if (!line.empty()) 
        {
            DrawText(line.c_str(), 8, dy, 13, {200,220,200,255});
        }
        DrawLine(0, dy+20, INFO_W, dy+20, {60,60,80,200});
        bool afford = (g.gold >= tower_costs[i]);
        DrawText(afford ? "Ready to place" : "Not enough gold", 8, dy+26, 13, afford ? GREEN : RED);
    } 
    else 
    {
        DrawText("Click a tower\nto inspect it,\nor pick one\nfrom the shop.", 8, 50, 14, {160,160,160,255});
    }
}

void draw_right_panel(Game& g) {
    int px = SW - SIDE_W;
    DrawRectangle(px, 0, SIDE_W, SH, {20,22,28,230});
    DrawRectangleLines(px, 0, SIDE_W, SH, {60,60,80,255});
    DrawText("WAVE", px+8, 10, 14, LIGHTGRAY);
    DrawText((std::to_string(g.wave) + "/5").c_str(), px+8, 26, 20, WHITE);
    DrawText("GOLD", px+8, 56, 14, GOLD);
    DrawText(std::to_string(g.gold).c_str(), px+8, 72, 20, GOLD);
    DrawText("LIVES", px+8, 102, 14, LIGHTGRAY);
    Color lc = g.lives > 3 ? GREEN : (g.lives > 1 ? YELLOW : RED);
    DrawText(std::to_string(g.lives).c_str(), px+8, 118, 20, lc);
    DrawLine(px, 148, SW, 148, {60,60,80,255});
    DrawText("SELECT TOWER", px+6, 154, 13, LIGHTGRAY);

    for (int i = 0; i < 5; i++) 
    {
        int by = 172 + i*94;
        bool sel = (g.selected_tower == i);
        DrawRectangle(px+6, by, SIDE_W-12, 88, sel ? Color{60,80,120,255} : Color{30,32,40,220});
        DrawRectangleLines(px+6, by, SIDE_W-12, 88, sel ? WHITE : Color{60,60,80,255});
        DrawRectangle(px+12, by+8, 20, 20, tower_cols[i]);
        DrawText(tower_names[i], px+36, by+10, 14, WHITE);
        DrawText(("$" + std::to_string(tower_costs[i])).c_str(), px+36, by+28, 13, GOLD);
        bool afford = (g.gold >= tower_costs[i]);
        DrawText(afford ? "[CAN BUY]" : "[NEED GOLD]", px+10, by+50, 11, afford ? GREEN : RED);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
        {
            Vector2 mp = GetMousePosition();
            if (mp.x >= px+6 && mp.x <= SW-6 && mp.y >= by && mp.y <= by+88) 
            {
                g.selected_tower = (g.selected_tower == i) ? -1 : i;
                g.hovered_tower  = (g.selected_tower == -1) ? -1 : i;
                g.inspect_tower  = nullptr;
            }
        }
    }

    if (!g.wave_running && g.wave < 5) 
    {
        DrawRectangle(px+10, SH-60, SIDE_W-20, 44, {50,140,80,255});
        DrawRectangleLines(px+10, SH-60, SIDE_W-20, 44, WHITE);
        draw_text_centered("START WAVE", px + SIDE_W/2, SH-48, 16, WHITE);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
        {
            Vector2 mp = GetMousePosition();
            if (mp.x >= px+10 && mp.x <= SW-10 && mp.y >= SH-60 && mp.y <= SH-16)
                { 
                    g.wave++; g.wave_running = true; 
                    g.spawn_phase = 0; 
                    g.spawn_count = 0; 
                    g.spawn_timer = 0; 
                }
        }
    } 
    else if (g.wave_running) 
    {
        DrawRectangle(px+10, SH-60, SIDE_W-20, 44, {60,60,60,200});
        draw_text_centered("WAVE IN PROGRESS", px + SIDE_W/2, SH-48, 13, LIGHTGRAY);
    }
}

void remove_enemy(Game& g, int idx) {
    Enemy* dying = g.enemies[idx];
    for (int i = 0; i < g.proj_count; i++)
    {
        if (g.projectiles[i] && g.projectiles[i]->target == dying)
        {
            g.projectiles[i]->target = nullptr;
        }
    }
    delete dying;
    for (int i = idx; i < g.enemy_count - 1; i++) g.enemies[i] = g.enemies[i+1];
    {
        g.enemies[--g.enemy_count] = nullptr;
    }
}

void remove_projectile(Game& g, int idx) {
    delete g.projectiles[idx];
    for (int i = idx; i < g.proj_count - 1; i++) g.projectiles[i] = g.projectiles[i+1];
    {
        g.projectiles[--g.proj_count] = nullptr;
    }
}

void remove_explosion(Game& g, int idx) {
    for (int i = idx; i < g.expl_count - 1; i++) g.explosions[i] = g.explosions[i+1];
    {
        g.expl_count--;
    }
}

void draw_bg(const Game& g) {
    if (g.bg_loaded)
    {
        DrawTexturePro(g.bg_tex, {0, 0, (float)g.bg_tex.width, (float)g.bg_tex.height},{0, 0, (float)SW, (float)SH}, {0,0}, 0, WHITE);
    }
    else 
    {
        for (int r = 0; r*80 < SH; r++)
        {
            for (int c = 0; c*80 < SW; c++)
            {
                DrawRectangle(c*80, r*80, 80, 80,((r+c)%2==0) ? Color{28,32,48,255} : Color{32,36,56,255});
            }
        }
    }
}
int main(){
    InitWindow(SW,SH,"Tower Defense");
    SetTargetFPS(60);
    InitAudioDevice();

    init_paths();
    init_waves();
    for(int i=0;i<3;i++) build_grid(i);
    load_scores();

    Game g;
    g.bg_tex = LoadTexture("background.png");
    g.bg_loaded = (g.bg_tex.id>0);
    g.bg_music = LoadMusicStream("Towergame.mp3");
    g.music_loaded= (g.bg_music.stream.buffer!=nullptr);
    if(g.music_loaded)
    {
        SetMusicVolume(g.bg_music,0.5f);
        PlayMusicStream(g.bg_music);
    }

    bool quit=false;
    while(!WindowShouldClose()&&!quit){
        float dt=GetFrameTime();
        if(g.music_loaded) 
        {
            UpdateMusicStream(g.bg_music);
        }

        if(g.screen==Screen::GAME)
        {
            if(g.wave_running)
            {
                g.spawn_timer-=dt;
                wave_data& wv=waves_list[g.wave-1];
                if(g.spawn_phase<wv.count)
                {
                    spawn_info& entry=wv.entries[g.spawn_phase];
                    if(g.spawn_timer<=0)
                    {
                        if(g.enemy_count<MAX_ENEMIES)
                        {
                            g.enemies[g.enemy_count++]=spawn_enemy(entry.type,&map_paths[g.map_idx]);
                        }
                        g.spawn_count++; 
                        g.spawn_timer=entry.interval;
                        if(g.spawn_count>=entry.count)
                        {
                            g.spawn_phase++;
                            g.spawn_count=0;
                        }
                    }
                } else if(g.enemy_count==0) 
                {
                    g.wave_running=false;
                }
            }

            for(int i=0;i<g.enemy_count;i++) 
            {
                g.enemies[i]->update(dt);
            }
            for(int i=g.enemy_count-1;i>=0;i--)
            {
                Enemy* e=g.enemies[i];
                if(e->has_reached_end())
                {
                    g.lives-=e->get_damage();
                    remove_enemy(g,i);
                }
                else if(!e->is_alive())
                {
                    g.gold+=e->get_reward();
                    remove_enemy(g,i);
                }
            }
            for(int i=0;i<g.tower_count;i++)
            {
                g.towers[i]->update(dt);

                static Projectile* shot_buf[8];
                int num_shots=g.towers[i]->try_shoot(g.enemies,g.enemy_count,shot_buf,8);
                for(int s=0;s<num_shots;s++)
                {
                    if(shot_buf[s]&&g.proj_count<MAX_PROJECTILES) 
                    {
                        g.projectiles[g.proj_count++]=shot_buf[s];
                    }
                    else if(shot_buf[s])
                    {
                        delete shot_buf[s];
                        shot_buf[s]=nullptr;
                    }
                }
            }
            for(int i=g.proj_count-1;i>=0;i--)
            {
                Projectile* p=g.projectiles[i]; p->update(dt);
                if(!p->active)
                {
                    if(p->is_bomb)
                    {
                        if(g.expl_count<MAX_EXPLOSIONS)
                        {
                            Explosion ex;ex.x=p->x;ex.y=p->y;ex.r=0;
                            ex.max_r=p->radius;ex.timer=0.4f;ex.col={255,140,0,200};
                            g.explosions[g.expl_count++]=ex;
                        }
                        for(int j=0;j<g.enemy_count;j++)
                        {
                            Enemy* e=g.enemies[j];
                            if(e&&e->is_alive()&&dist(p->x,p->y,e->get_x(),e->get_y())<p->radius)
                            {
                                e->take_damage(p->dmg);
                            }
                        }
                    } 
                    else if(p->target&&p->target->is_alive()) p->target->take_damage(p->dmg);
                    {
                        remove_projectile(g,i);
                    }
                }
            }
            for(int i=g.expl_count-1;i>=0;i--)
            {
                g.explosions[i].timer-=dt;
                g.explosions[i].r=g.explosions[i].max_r*(1.0f-g.explosions[i].timer/0.4f);
                if(g.explosions[i].timer<=0) 
                {
                    remove_explosion(g,i);
                }
            }

            if(g.lives<=0) 
            {
                g.screen=Screen::GAME_OVER;
            }
            if(g.wave>=5&&!g.wave_running&&g.enemy_count==0)
            {
                if(g.wave>best_scores[g.map_idx].wave||(g.wave==best_scores[g.map_idx].wave&&g.gold>best_scores[g.map_idx].gold))
                {
                    best_scores[g.map_idx]={g.wave,g.gold};
                    save_scores();
                }
                g.screen=Screen::WIN;
            }

            if(g.selected_tower>=0)
            {
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    Vector2 mp=GetMousePosition();
                    int col,row; float gx,gy;
                    snap_to_grid(mp.x,mp.y,col,row,gx,gy);
                    bool in_map=(mp.x>=MAP_X&&mp.x<=MAP_X+MAP_W&&mp.y>=0&&mp.y<=MAP_H);
                    bool is_path_cell=grid_is_path[g.map_idx][row][col];
                    bool occupied=false;
                    for(int i=0;i<g.tower_count;i++)
                    {
                        if(dist(gx,gy,g.towers[i]->get_x(),g.towers[i]->get_y())<4)
                        {
                            occupied=true;
                            break;
                        }
                    }
                    int cost=tower_costs[g.selected_tower];
                    if(in_map&&!is_path_cell&&!occupied&&g.gold>=cost&&g.tower_count<MAX_TOWERS)
                    {
                        g.towers[g.tower_count++]=build_tower(g.selected_tower,gx,gy);
                        g.gold-=cost; 
                        g.selected_tower=-1; 
                        g.hovered_tower=-1;
                    }
                }
            } 
            else 
            {
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    Vector2 mp=GetMousePosition();
                    if(mp.x>=MAP_X&&mp.x<=MAP_X+MAP_W)
                    {
                        g.inspect_tower=nullptr;
                        g.hovered_tower=-1;
                        for(int i=0;i<g.tower_count;i++)
                        {
                            if(dist(mp.x,mp.y,g.towers[i]->get_x(),g.towers[i]->get_y())<CELL/2)
                            {
                                g.inspect_tower=g.towers[i];
                                break;
                            }
                        }
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground({18,20,26,255});

        if(g.screen==Screen::MENU)
        {
            draw_bg(g);
            draw_text_centered("TOWER DEFENSE",SW/2,120,64,BLACK);
            draw_text_centered("Defend the path from waves of geometric shapes!",SW/2,200,20,BLACK);
            DrawRectangle(SW/2-120,300,240,60,{50,140,80,255});
            DrawRectangleLines(SW/2-120,300,240,60,{20,80,40,255});
            draw_text_centered("PLAY",SW/2,318,28,WHITE);
            DrawRectangle(SW/2-120,380,240,60,{160,40,40,255});
            DrawRectangleLines(SW/2-120,380,240,60,{100,20,20,255});
            draw_text_centered("EXIT",SW/2,398,28,WHITE);
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Vector2 mp=GetMousePosition();
                if(mp.x>=SW/2-120&&mp.x<=SW/2+120&&mp.y>=300&&mp.y<=360)
                    g.screen=Screen::MAP_SELECT;
                if(mp.x>=SW/2-120&&mp.x<=SW/2+120&&mp.y>=380&&mp.y<=440)
                    quit=true;
            }
        }

        else if(g.screen==Screen::MAP_SELECT){
            draw_bg(g);
            draw_text_centered("SELECT MAP",SW/2,40,40,GOLD);
            int card_w=300,card_h=320,gap=40,start_x=(SW-(3*card_w+2*gap))/2;
            static const Color map_cols[3]={{60,120,200,255},{120,60,60,255},{60,160,80,255}};
            static const char* map_names[3]={"Map 1: S-Curve","Map 2: Zigzag","Map 3: Spiral"};
            for(int i=0;i<3;i++)
            {
                int cx=start_x+i*(card_w+gap);
                DrawRectangle(cx,100,card_w,card_h,{20,22,30,200});
                DrawRectangleLines(cx,100,card_w,card_h,map_cols[i]);
                draw_text_centered(map_names[i],cx+card_w/2,112,17,map_cols[i]);
                draw_mini_path(map_paths[i],cx+10,140,0.28f,map_cols[i]);
                DrawLine(cx,300,cx+card_w,300,{60,60,80,200});
                draw_text_centered("Best: Wave "+std::to_string(best_scores[i].wave)+"  $"+std::to_string(best_scores[i].gold),
                    cx+card_w/2,308,13,GOLD);
                DrawRectangle(cx+30,340,card_w-60,44,map_cols[i]);
                DrawRectangleLines(cx+30,340,card_w-60,44,WHITE);
                draw_text_centered("SELECT",cx+card_w/2,354,18,WHITE);
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    Vector2 mp=GetMousePosition();
                    if(mp.x>=cx+30&&mp.x<=cx+card_w-30&&mp.y>=340&&mp.y<=384) g.reset(i);
                }
            }
            DrawRectangle(20,SH-60,120,40,{60,60,60,220});
            DrawRectangleLines(20,SH-60,120,40,WHITE);
            draw_text_centered("BACK",80,SH-44,16,WHITE);
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Vector2 mp=GetMousePosition();
                if(mp.x>=20&&mp.x<=140&&mp.y>=SH-60&&mp.y<=SH-20) 
                {
                    g.screen=Screen::MENU;
                }
            }
        }

        else if (g.screen == Screen::GAME) 
        {
            draw_bg(g);
            draw_grid(g.map_idx);
            draw_path_markers(map_paths[g.map_idx]);
            draw_grid(g.map_idx);
            draw_path_markers(map_paths[g.map_idx]);

            if(g.selected_tower>=0)
            {
                Vector2 mp=GetMousePosition();
                int col,row; float gx,gy;
                snap_to_grid(mp.x,mp.y,col,row,gx,gy);
                bool is_path_cell=(col>=0&&col<grid_cols&&row>=0&&row<grid_rows)
                                 ?grid_is_path[g.map_idx][row][col]:true;
                float ranges[]={130,260,100,115,140};
                DrawCircleLines((int)gx,(int)gy,(int)ranges[g.selected_tower],{255,255,255,80});
                Color hilite=is_path_cell?Color{255,60,60,140}:Color{60,255,60,140};
                DrawRectangle(MAP_X+col*CELL,row*CELL,CELL,CELL,hilite);
                DrawRectangleLines(MAP_X+col*CELL,row*CELL,CELL,CELL,WHITE);
            }

            for(int i=0;i<g.tower_count;i++)
            {
                g.towers[i]->render();
                if(g.towers[i]==g.inspect_tower)
                {
                    DrawCircleLines((int)g.towers[i]->get_x(),(int)g.towers[i]->get_y(),(int)g.towers[i]->get_range(),{255,255,100,120});
                }
            }
            for(int i=0;i<g.enemy_count;i++) 
            {
                g.enemies[i]->render();
            }
            for(int i=0;i<g.proj_count;i++)  
            {
                g.projectiles[i]->render();
            }
            for(int i=0;i<g.expl_count;i++)
            {
                Explosion& ex=g.explosions[i];
                float alpha=ex.timer/0.4f;
                Color c={ex.col.r,ex.col.g,ex.col.b,(unsigned char)(ex.col.a*alpha)};
                DrawCircle((int)ex.x,(int)ex.y,(int)ex.r,c);
                DrawCircleLines((int)ex.x,(int)ex.y,(int)ex.r,{255,220,80,200});
            }
            draw_left_panel(g);
            draw_right_panel(g);
            if(g.selected_tower>=0)
            {
                Vector2 mp=GetMousePosition();
                std::string tip="Place: "; tip+=tower_names[g.selected_tower];
                DrawRectangle((int)mp.x+12,(int)mp.y-18,180,22,{0,0,0,180});
                DrawText(tip.c_str(),(int)mp.x+15,(int)mp.y-14,14,WHITE);
            }
            if(IsKeyPressed(KEY_ESCAPE))
            {
                g.selected_tower=-1;
                g.hovered_tower=-1;
                g.inspect_tower=nullptr;
            }
        }

        else if(g.screen==Screen::GAME_OVER)
        {
            draw_bg(g);
            draw_text_centered("GAME OVER",SW/2,180,72,BLACK);
            draw_text_centered("The shapes broke through!",SW/2,270,22,BLACK);
            draw_text_centered("Survived "+std::to_string(g.wave)+" wave(s)  |  Gold: $"+std::to_string(g.gold),SW/2,310,18,BLACK);
            // Save best wave even on game-over
            if(g.wave>best_scores[g.map_idx].wave|| (g.wave==best_scores[g.map_idx].wave&&g.gold>best_scores[g.map_idx].gold))
            {
                best_scores[g.map_idx]={g.wave,g.gold};
                save_scores();
            }
            DrawRectangle(SW/2-140,390,280,55,{180,40,40,255});
            DrawRectangleLines(SW/2-140,390,280,55,BLACK);
            draw_text_centered("TRY AGAIN",SW/2,408,26,BLACK);
            DrawRectangle(SW/2-140,460,280,55,{50,50,80,255});
            DrawRectangleLines(SW/2-140,460,280,55,BLACK);
            draw_text_centered("MAIN MENU",SW/2,478,26,BLACK);
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Vector2 mp=GetMousePosition();
                if(mp.x>=SW/2-140&&mp.x<=SW/2+140)
                {
                    if(mp.y>=390&&mp.y<=445) 
                    {
                        g.reset(g.map_idx);
                    }
                    if(mp.y>=460&&mp.y<=515) 
                    {
                        g.screen=Screen::MENU;
                    }
                }
            }
        }

        else if(g.screen==Screen::WIN)
        {
            draw_bg(g);
            draw_text_centered("YOU WIN!",SW/2,160,80,BLACK);
            draw_text_centered("All 5 waves of shapes defeated!",SW/2,260,24,BLACK);
            draw_text_centered("Gold remaining: $"+std::to_string(g.gold),SW/2,300,20,BLACK);
            best_scores[g.map_idx]={5,g.gold};
            save_scores();
            draw_text_centered("New Best: Wave 5  $"+std::to_string(g.gold),SW/2,336,18,BLACK);
            DrawRectangle(SW/2-140,400,280,55,{50,140,80,255});
            DrawRectangleLines(SW/2-140,400,280,55,BLACK);
            draw_text_centered("PLAY AGAIN",SW/2,418,26,BLACK);
            DrawRectangle(SW/2-140,470,280,55,{50,50,80,255});
            DrawRectangleLines(SW/2-140,470,280,55,BLACK);
            draw_text_centered("MAIN MENU",SW/2,488,26,BLACK);
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Vector2 mp=GetMousePosition();
                if(mp.x>=SW/2-140&&mp.x<=SW/2+140)
                {
                    if(mp.y>=400&&mp.y<=455) 
                    {
                        g.reset(g.map_idx);
                    }
                    if(mp.y>=470&&mp.y<=525) 
                    {
                        g.screen=Screen::MENU;
                    }
                }
            }
        }

        EndDrawing();
    }

    for(int i=0;i<g.enemy_count;i++)  
    {
        delete g.enemies[i];
    }
    for(int i=0;i<g.tower_count;i++)  
    {
        delete g.towers[i];
    }
    for(int i=0;i<g.proj_count;i++)   
    {
        delete g.projectiles[i];
    }
    if(g.bg_loaded)    
    {
        UnloadTexture(g.bg_tex);
    }
    if(g.music_loaded)
    {
        StopMusicStream(g.bg_music);
        UnloadMusicStream(g.bg_music);
    }
    CloseAudioDevice();
    CloseWindow();
    return 0;
}