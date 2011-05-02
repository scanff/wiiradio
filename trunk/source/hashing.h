#ifndef _HASHING_H_
#define _HASHING_H_



#define TAG_GUI_OBJS \
    N(gui_button) \
    N(gui_toggle) \
    N(gui_textarea) \
    N(gui_list) \
    N(gui_img_list) \
    N(x) \
    N(y) \
    N(w) \
    N(h) \
    N(img) \
    N(screen) \
    N(z_order) \
    N(visible) \
    N(layer) \
    N(items) \
    N(img_pad_x) \
    N(action_lclick) \
    N(img_down) \
    N(img_out) \
    N(img_over) \
    N(limit_text) \
    N(text_pad_y) \
    N(text_pad_x) \
    N(text_color_over) \
    N(text_color_out) \
    N(align) \
    N(text_size) \
    N(text) \
    N(vspacing) \
    N(autoscroll) \
    N(scroll)

#define TABLE_SIZE  100

/*const unsigned long hash_table[TABLE_SIZE] = {
  file.h
};
static int table_pos = 0;*/

/*
FILE* f = 0;
const unsigned long inline create_hash_output(const char* x)
{
    char hashs[50];
    f = fopen("c://hashes.txt","a");
    unsigned long hash = 0;
    const char* ptr = x;

    while (*ptr)
        hash = (hash << 5) + hash + *ptr++;

    hash_table[table_pos++] = hash;

    sprintf(hashs,"#define HASH_%s 0x%x\n",x,hash);
    fwrite(hashs,strlen(hashs),1,f);
    fclose(f);
    return (const unsigned long)hash;
}
*/
const unsigned long inline get_hash(const char* x)
{
    unsigned long hash = 0;
    const char* ptr = x;

    while (*ptr) hash = (hash << 5) + hash + *ptr++;

    return (const unsigned long)hash;
}
/*
#define TABLE_POS(x) \
    for(int v=0;v<TABLE_SIZE;v++) \
        if (get_hash(x)==hash_table[v]) \
            return v;

#define N(n) const unsigned long HASH_##n = get_hash(#n);
TAG_GUI_OBJS
#undef N
*/
//#define N(n) const unsigned long VALUE_HASH_##n = create_hash(#n);
//TAG_GUI_ATTRIBS
//#undef N
// These were output using - create_hash_output
#define HASH_gui_button 0x421d3e60
#define HASH_gui_toggle 0x6ba62e06
#define HASH_gui_textarea 0x14b095c2
#define HASH_gui_list 0x38bc3380
#define HASH_gui_img_list 0x2c90849c
#define HASH_x 0x78
#define HASH_y 0x79
#define HASH_w 0x77
#define HASH_h 0x68
#define HASH_img 0x1cd1d
#define HASH_screen 0x1380f880
#define HASH_z_order 0x94f3e1b5
#define HASH_visible 0x78a1efee
#define HASH_layer 0x7d9975d
#define HASH_items 0x7ad6502
#define HASH_img_pad_x 0x3d7c21e8
#define HASH_action_lclick 0xb1bc04af
#define HASH_img_down 0xa4bf5cf4
#define HASH_img_out 0x98631354
#define HASH_img_over 0xa4c58098
#define HASH_limit_text 0xbbb8fca3
#define HASH_text_pad_y 0x4fdd5c51
#define HASH_text_pad_x 0x4fdd5c50
#define HASH_text_color_over 0x6c21d65e
#define HASH_text_color_out 0xb08c85a
#define HASH_align 0x7184e2b
#define HASH_text_size 0x29370adf
#define HASH_text 0x415985
#define HASH_vspacing 0x8716907b
#define HASH_autoscroll 0x46b69488
#define HASH_scroll 0x138123ef
#define HASH_name 0x3dfcc1
#endif
