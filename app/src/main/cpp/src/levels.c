#include "nanovg.h"
#include "orb.h"

static void level0(orb_data *orb)
{
    orb_object_list_init(orb, &orb->list);
    orb->id[0] = orb_object_add_square(orb, &orb->list, 13, 4);

    fsm_init(&orb->fs, 1);

    fsm_assign_id(&orb->fs, 1, orb->id[0]);
    orb_object_set_fsm_pos(orb, orb->id[0], 1);

    fsm_set_state(&orb->fs, 1);
    orb_fsm_update(orb);

    orb_synth_set_notes(orb, 62, 64, 69);
    orb_object_set_note(orb, orb->id[0], 73);

    orb_avatar_set_pos(orb, &orb->av, 3, 4);
    orb_color_set(orb, nvgRGB(211, 186, 169), nvgRGB(39, 27, 20));
}

static void level1(orb_data *orb)
{
    orb_object_list_init(orb, &orb->list);
    orb->id[0] = orb_object_add_offsquare(orb, &orb->list, 15, 0);
    orb->id[1] = orb_object_add_square(orb, &orb->list, 0, 8);

    fsm_init(&orb->fs, 2);
    fsm_add_rule(&orb->fs, 1, 2);

    fsm_assign_id(&orb->fs, 1, orb->id[0]);
    orb_object_set_fsm_pos(orb, orb->id[0], 1);

    fsm_assign_id(&orb->fs, 2, orb->id[1]);
    orb_object_set_fsm_pos(orb, orb->id[1], 2);

    fsm_set_state(&orb->fs, 1);
    orb_fsm_update(orb);

    orb_synth_set_notes(orb, 62, 64, 69);
    orb_object_set_note(orb, orb->id[0], 76);
    orb_object_set_note(orb, orb->id[1], 71);

    orb_avatar_set_pos(orb, &orb->av, 0, 0);

    orb_color_set(orb, nvgRGB(187, 192, 235), nvgRGB(13, 0, 52)); 
}

static void level2(orb_data *orb)
{
    orb_object_list_init(orb, &orb->list);
    orb->id[0] = orb_object_add_offsquare(orb, &orb->list, 2, 2);
    orb->id[1] = orb_object_add_square(orb, &orb->list, 13, 4);
    orb->id[2] = orb_object_add_square(orb, &orb->list, 2, 6);

    fsm_init(&orb->fs, 3);
    fsm_add_rule(&orb->fs, 1, 2);
    fsm_add_rule(&orb->fs, 1, 3);
    fsm_add_rule(&orb->fs, 2, 3);
    fsm_add_rule(&orb->fs, 3, 1);

    fsm_assign_id(&orb->fs, 1, orb->id[0]);
    orb_object_set_fsm_pos(orb, orb->id[0], 1);

    fsm_assign_id(&orb->fs, 2, orb->id[1]);
    orb_object_set_fsm_pos(orb, orb->id[1], 2);
    
    fsm_assign_id(&orb->fs, 3, orb->id[2]);
    orb_object_set_fsm_pos(orb, orb->id[2], 3);

    fsm_set_state(&orb->fs, 2);
    orb_fsm_update(orb);

    orb_synth_set_notes(orb, 61, 62, 69);
    orb_object_set_note(orb, orb->id[0], 74);
    orb_object_set_note(orb, orb->id[1], 81);
    orb_object_set_note(orb, orb->id[2], 76);

    orb_avatar_set_pos(orb, &orb->av, 2, 4);
    orb_color_set(orb, nvgRGB(184, 201, 37), nvgRGB(21, 24, 9));
}

static void level3(orb_data *orb)
{
    orb_object_list_init(orb, &orb->list);
    orb->id[0] = orb_object_add_offsquare(orb, &orb->list, 4, 2);
    orb->id[1] = orb_object_add_square(orb, &orb->list, 11, 6);
    orb->id[2] = orb_object_add_square(orb, &orb->list, 11, 2);
    orb->id[3] = orb_object_add_square(orb, &orb->list, 4, 6);

    fsm_init(&orb->fs, 4);
    fsm_add_rule(&orb->fs, 1, 2);
    fsm_add_rule(&orb->fs, 1, 3);
    fsm_add_rule(&orb->fs, 1, 4);
    fsm_add_rule(&orb->fs, 2, 3);
    fsm_add_rule(&orb->fs, 2, 1);
    fsm_add_rule(&orb->fs, 3, 4);
    fsm_add_rule(&orb->fs, 4, 1);

    fsm_assign_id(&orb->fs, 1, orb->id[0]);
    orb_object_set_fsm_pos(orb, orb->id[0], 1);

    fsm_assign_id(&orb->fs, 2, orb->id[1]);
    orb_object_set_fsm_pos(orb, orb->id[1], 2);
    
    fsm_assign_id(&orb->fs, 3, orb->id[2]);
    orb_object_set_fsm_pos(orb, orb->id[2], 3);
    
    fsm_assign_id(&orb->fs, 4, orb->id[3]);
    orb_object_set_fsm_pos(orb, orb->id[3], 4);

    fsm_set_state(&orb->fs, 4);
    orb_fsm_update(orb);

    orb_synth_set_notes(orb, 60, 67, 69);
    orb_object_set_note(orb, orb->id[0], 74);
    orb_object_set_note(orb, orb->id[1], 76);
    orb_object_set_note(orb, orb->id[2], 83);
    orb_object_set_note(orb, orb->id[3], 72);

    orb_avatar_center_x(orb, &orb->av);
    orb_avatar_center_y(orb, &orb->av);
    orb_color_set(orb, nvgRGB(57, 18, 27), nvgRGB(250, 110, 121));
}

static void level4(orb_data *orb)
{
    orb_object_list_init(orb, &orb->list);
    orb->id[0] = orb_object_add_square(orb, &orb->list, 15, 0);
    orb->id[2] = orb_object_add_square(orb, &orb->list, 8, 1);
    orb->id[1] = orb_object_add_square(orb, &orb->list, 4, 8);
    orb->id[3] = orb_object_add_square(orb, &orb->list, 0, 7);
    orb->id[4] = orb_object_add_square(orb, &orb->list, 2, 3);

    fsm_init(&orb->fs, 5);
    fsm_add_rule(&orb->fs, 1, 2);

    fsm_add_rule(&orb->fs, 2, 3);
    fsm_add_rule(&orb->fs, 2, 5);

    fsm_add_rule(&orb->fs, 3, 1);
    fsm_add_rule(&orb->fs, 3, 4);

    fsm_add_rule(&orb->fs, 4, 1);
    fsm_add_rule(&orb->fs, 4, 3);
    fsm_add_rule(&orb->fs, 4, 2);

    fsm_add_rule(&orb->fs, 5, 1);
    fsm_add_rule(&orb->fs, 5, 2);
    fsm_add_rule(&orb->fs, 5, 3);
    fsm_add_rule(&orb->fs, 5, 4);

    fsm_assign_id(&orb->fs, 1, orb->id[0]);
    orb_object_set_fsm_pos(orb, orb->id[0], 1);

    fsm_assign_id(&orb->fs, 2, orb->id[1]);
    orb_object_set_fsm_pos(orb, orb->id[1], 2);
    
    fsm_assign_id(&orb->fs, 3, orb->id[2]);
    orb_object_set_fsm_pos(orb, orb->id[2], 3);
    
    fsm_assign_id(&orb->fs, 4, orb->id[3]);
    orb_object_set_fsm_pos(orb, orb->id[3], 4);
    
    fsm_assign_id(&orb->fs, 5, orb->id[4]);
    orb_object_set_fsm_pos(orb, orb->id[4], 5);

    fsm_set_state(&orb->fs, 24);
    orb_fsm_update(orb);

    orb_synth_set_notes(orb, 50, 64, 69);
    orb_object_set_note(orb, orb->id[0], 78);
    orb_object_set_note(orb, orb->id[1], 74);
    orb_object_set_note(orb, orb->id[2], 71);
    orb_object_set_note(orb, orb->id[3], 76);
    orb_object_set_note(orb, orb->id[4], 69);

    orb_avatar_set_pos(orb, &orb->av, 15, 8);
    orb_color_set(orb, 
            nvgRGB(217, 206, 178),
            nvgRGB(100, 135, 141)); 
}

static void level5(orb_data *orb)
{
    orb_object_list_init(orb, &orb->list);
    orb->id[0] = orb_object_add_avoidsquare(orb, &orb->list, 10, 4);
    orb->id[2] = orb_object_add_avoidsquare(orb, &orb->list, 10, 5);
    orb->id[3] = orb_object_add_avoidsquare(orb, &orb->list, 10, 6);
    orb->id[4] = orb_object_add_avoidsquare(orb, &orb->list, 10, 7);
    orb->id[5] = orb_object_add_avoidsquare(orb, &orb->list, 10, 8);
    orb->id[6] = orb_object_add_avoidsquare(orb, &orb->list, 10, 3);
    orb->id[7] = orb_object_add_avoidsquare(orb, &orb->list, 10, 2);
    
    orb->id[1] = orb_object_add_square(orb, &orb->list, 13, 4);

    fsm_init(&orb->fs, 1);

    fsm_assign_id(&orb->fs, 1, orb->id[1]);
    orb_object_set_fsm_pos(orb, orb->id[1], 1);

    fsm_set_state(&orb->fs, 1);
    orb_fsm_update(orb);

    orb_synth_set_notes(orb, 63, 64, 70);
    orb_object_set_note(orb, orb->id[1], 76);

    orb_avatar_set_pos(orb, &orb->av, 1, 4);
    orb_color_set(orb, nvgRGB(195, 254, 104), nvgRGB(85, 98, 112));
}

void orb_level_init(orb_data *orb)
{
    orb->lvl = 0;
    orb->level[0] = level0;
    orb->level[1] = level1;
    orb->level[2] = level2;
    orb->level[3] = level3;
    orb->level[4] = level4;
    orb->level[5] = level5;
}

void orb_level_next(orb_data *orb)
{
    orb->lvl = (orb->lvl + 1) % NLEVELS;
}

void orb_level_load(orb_data *orb)
{
    orb->level[orb->lvl](orb);
}

void orb_level_set(orb_data *orb, int level)
{
    orb->lvl = level;
}
