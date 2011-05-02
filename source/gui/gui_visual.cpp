#include "../globals.h"
#include "../application.h"
#include "../textures.h"
#include "../local.h"
#include "gui_object.h"
#include "gui_visual.h"
#include "../visuals.h"
#include "../gui.h"

#define MAX_VIS_T (4)

gui_visual::gui_visual(app_wiiradio* _theapp)
: vis_type(0), changable(0)
{
    theapp = _theapp;
    guibuffer = theapp->ui->guibuffer;
    obj_type = GUI_VISUAL;
    theapp->ui->vis_on_screen = 1;
    loopi(MAX_FFT_RES) peakResults[i] = 0;;
}

gui_visual::~gui_visual()
{
}

int gui_visual::draw()
{

    if (!(parent ? parent->visible : 1))
        return 0;

    if (!gui_object::draw())
        return 0;

    const int xoffset = parent ? parent->s_x : 0;
    const int yoffset = parent ? parent->s_y : 0;


    switch(vis_type)
    {
    case 0:
    {

        const int x = xoffset + s_x;
        const int y = yoffset + s_y + s_h;
        const int bar_height = s_h;
        const int bar_width = s_w / MAX_FFT_RES + 1;
        const double percent = ((double)bar_height / (double)32767);

        const int ar = vis_color_A >> 16;
        const int ag = vis_color_A >> 8;
        const int ab = vis_color_A & 0xff;
        const int br = vis_color_B >> 16;
        const int bg = vis_color_B >> 8;
        const int bb = vis_color_B & 0xff;


        loopi(MAX_FFT_RES)
        {
            // peak
            peakResults[i] -= (32767 / bar_height) * 4;

            if(peakResults[i] < 0) peakResults[i] = 0;
            if(peakResults[i] < theapp->ui->vis->fft_results[i]) peakResults[i] = theapp->ui->vis->fft_results[i];

            int peaks_newy = (y - (int)(percent * (double)peakResults[i]));


            // fft
            int newY = (y - (int)(percent * (double)theapp->ui->vis->fft_results[i]));
            int h = abs(y - newY);

            if(peaks_newy > newY) peaks_newy = y;
            if(peaks_newy < s_y) peaks_newy = s_y;
            // draw
            draw_rect_rgb(guibuffer,x+i*bar_width,newY,bar_width / 2,h,ar,ag,ab);
            draw_rect_rgb(guibuffer,x+i*bar_width,peaks_newy,bar_width/2,3,br,bg,bb );

        }
    }
    break;
    case 1: //AA
    {

        int x = xoffset + s_x;
        int y = yoffset + s_y;
        SDL_Rect APIC_dst = {x,y,APIC_SIZE,APIC_SIZE};
        SDL_Rect APIC_src = {0,0,APIC_SIZE,APIC_SIZE};

        SDL_Surface* APIC_Surface = theapp->GetLocFile()->m_id3.APIC_Surface;
        if (!APIC_Surface) APIC_Surface = theapp->GetTextures()->texture_lookup("data/imgs/no_art.png");

        if (APIC_Surface)
        {
            while (APIC_dst.w > s_w || APIC_dst.h > s_h)
            {
                APIC_dst.w /= 2;
                APIC_dst.h /= 2;
            }

            APIC_dst.x = x + ((s_w / 2) - (APIC_dst.w / 2));
            APIC_dst.y = y + ((s_h / 2) - (APIC_dst.h / 2));


            if(SDL_SoftStretch(APIC_Surface,&APIC_src, guibuffer,&APIC_dst) < 0)
            {
                return 1;
            }
        }

    }
    break;
    case 2:
    {
        short *wavedata = (short*)theapp->audio_data;
        int ox = xoffset + s_x;
        int oy = yoffset + s_y;

        int x,ynuml;
        int zerolevel = (s_h >> 1);

        double ts = static_cast<double>(s_w)/static_cast<double>(MAX_FFT_SAMPLE/8);
        double scale = ((double)(s_h >> 1) / 16384.0);


        for(int i = 0; i < s_w; i++)
        {
            x = (int)(double)(i*ts);

            ynuml = abs((int)(scale * wavedata[x * 2 + 0])); // left

            int height = ynuml / 2;
            u32 color = vis_color_A;
            for(int y = zerolevel - height; y < zerolevel + height; y++)
            {
                if (y > 0 && y < s_h)
                {
                    FastSetPixel24(guibuffer,ox+i,oy+y,color);
                }

            }

        }
    }
    break;
    case 3:
    {
        int r = (s_w>>2);
        int iterations=(int)((2*r*PI)+0.5) >> 1;
        double angle;
        int x1;
        int y1;

        int ox = xoffset + s_x + (s_w >> 1);
        int oy = yoffset + s_y + (s_h >> 1);
        u32 color = vis_color_A;
//        u32 color2 = vis_color_B;
        int x2 = ox;
        int y2 = oy;
        double ts = static_cast<double>(s_w)/static_cast<double>(MAX_FFT_SAMPLE/8);
        double scale = ((double)(s_h >> 2) / 16384.0);
        int x, ynuml;
        short *wavedata = (short*)theapp->audio_data;

        for(int i=0;i<=iterations;i++)
        {
             x = (int)(double)(i*ts);
             ynuml = abs((int)(scale * wavedata[x * 2 + 0]));
             angle=(((2*M_PI*i)/iterations)+0.5);

             x1=int(ox+(r*cos(angle))+0.5);
             y1=int(oy+(r*sin(angle))+0.5);

             if(x1 > ox) x1 += (ynuml>>1);
             else x1 -= (ynuml>>1);

             if(y1 > oy) y1 += (ynuml>>1);
             else y1 -= (ynuml>>1);

             if(i > 1)
             {
                 //FastSetPixel24(guibuffer,x2,y2,color);
                bresenham_line(guibuffer,x2,y2,x1,y1,color);
             }

             x2 = x1;
             y2 = y1;
        }

    }
    break;
    }

    return 0;
}

 //-- hit test
int gui_visual::hit_test(const SDL_Event *event)
{
    if(!IsVisible())
        return 0;

    int x;
    int y;

    switch (event->type) {
        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            x = event->button.x;
            y = event->button.y;
            break;
        default:
            return obj_state;
    }

    obj_state = B_OUT;
    if (point_in_rect(x,y))
    {

        if (event->type == SDL_MOUSEBUTTONDOWN && changable)
        {
            obj_state = B_CLICK;
            this->vis_type++;
            if (vis_type >= MAX_VIS_T) vis_type = 0;
           /* if (click_func)
            {
                click_func(this);
                theapp->GetScript()->CallVoidVoidFunction(this->click_script);
            }*/
        }else {
            obj_state = B_OVER;
        }

        // if button has on off images
        if (object_images[GUI_IMG_ON] && obj_state == B_CLICK)
            obj_sub_state = (obj_sub_state == B_OFF ? obj_sub_state = B_ON : obj_sub_state = B_OFF);

    }

    return obj_state;
}
