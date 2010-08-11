#ifndef _GUI_TOGGLE_H_
#define _GUI_TOGGLE_H_


//simple button class
class gui_toggle : public gui_object{
    public:

    gui_toggle(app_wiiradio* _theapp,int x,int y,int w,int h,char* t,long tc)
    {
        theapp = _theapp;
        s_x = x;
        s_y = y;
        pad_x = 30;
        pad_y = 12;
        text_color = tc;
        fnts = theapp->fnts;
        guibuffer = theapp->ui->guibuffer;
        obj_state = B_OFF;
        obj_type = GUI_TOGGLE;
        if (t) strcpy(text_l1,t);

    }

    ~gui_toggle()
    {
    };

    void draw()
    {
        gui_object::draw();

        if (text_l1) {
            fnts->set_size(font_sz);

            if (center_text) {
                int text_len = fnts->get_length_px(text_l1,font_sz);
                int cx = 0;
                text_len > 0 ? cx = (int)((s_w-(text_len))/2): cx = 1;
                fnts->text(guibuffer,text_l1,cx+s_x,s_y+pad_y,limit_text);
            }else{
                fnts->text(guibuffer,text_l1,s_x+pad_x,s_y+pad_y,limit_text);
            }
        }
    }

};

#endif
