#ifndef GUI_TEXTBOX_H_INCLUDED
#define GUI_TEXTBOX_H_INCLUDED

class gui_textbox : public gui_object{

    public:

    SDL_Surface*    scroll_area;

    int             scroll_x;
    bool            auto_scroll_text;
    int             scroll_speed;
    bool            can_scroll;


    gui_textbox(app_wiiradio* _theapp,int x,int y,char* t,long tc,bool scroll) :
        scroll_area(0),
        scroll_x(0),
        auto_scroll_text(false),
        scroll_speed(3),
        can_scroll(true)

    {
       theapp = _theapp;

        s_x = x;
        s_y = y;
        pad_x = 30;
        pad_y = 12;
        text_color = tc;
        fnts = theapp->fnts;
        guibuffer = theapp->ui->guibuffer;
        obj_state = B_OUT;
        obj_type = GUI_TEXTBOX;

        if (t) strcpy(text_l1,t);


        if (scroll)
        {
            scroll_area = SDL_CreateRGBSurface(SDL_SWSURFACE,SCREEN_WIDTH,FONT_SIZE,BITDEPTH, rmask, gmask, bmask,amask);
            SDL_SetColorKey(scroll_area,SDL_SRCCOLORKEY, SDL_MapRGB(scroll_area->format,200,200,200));
        }

    }

    ~gui_textbox()
    {
        if (scroll_area) SDL_FreeSurface(scroll_area);
    };


    void scroll_reset() { scroll_x = 0; };

    void scroll_text()
    {
        draw_rect(scroll_area,0,0,SCREEN_WIDTH,FONT_SIZE,0xC8C8C8);
        int fontwidth = fnts->text(scroll_area,text_l1,scroll_x,0,0);

        if (obj_state == B_OVER || auto_scroll_text) scroll_x-=scroll_speed; //scroll speed

        SDL_Rect ds = {s_x+pad_x,s_y+pad_y,( limit_text ? limit_text : scroll_area->w ),scroll_area->h};
        SDL_Rect sr = {0,0,( limit_text ? limit_text : scroll_area->w ),scroll_area->h};
        SDL_BlitSurface( scroll_area,&sr , guibuffer,&ds );

        if (scroll_x < -(fontwidth)) scroll_x = limit_text;

    };



    void draw()
    {

        gui_object::draw();

        if (*text_l1)
        {
            fnts->set_size(font_sz);
            fnts->change_color((text_color >> 16)& 0xff,(text_color >> 8)& 0xff,text_color & 0xff);

            if (scroll_area && can_scroll)
            {
                scroll_text();
            }else{
                char* real_text;

                if(isvariable) real_text = vars.search_var(text_l1);
                else real_text = text_l1;

                if (center_text) {
                    int text_len = fnts->get_length_px(real_text,font_sz);
                    int cx = 0;
                    text_len>0 ? cx = (int)((s_w-(text_len))/2): cx=1;
                    fnts->text(guibuffer,real_text,cx+s_x,s_y+pad_y,limit_text);
                }else{
                    fnts->text(guibuffer,real_text,s_x+pad_x,s_y+pad_y,limit_text);
                }
            }
        }

    };





};


#endif // GUI_TEXTBOX_H_INCLUDED
