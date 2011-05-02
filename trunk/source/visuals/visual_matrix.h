#ifndef VISUAL_MATRIX_H_INCLUDED
#define VISUAL_MATRIX_H_INCLUDED

#include "visual_object.h"

class visualizer;
class vis_matrix : public visual_object
{
    public:

    int matrix_map[200][200];
    unsigned long startt;
    vis_matrix(app_wiiradio* _theapp)
    {
        theapp = _theapp;
        f = theapp->fourier;
        loaded = false;
        layer = 1;
        memset(matrix_map,0,200*200*sizeof(int));
    };

    ~vis_matrix()
    {

    };


    void load(void* userdata)
    {
        loaded = true;
        startt = get_tick_count();
    };



    void render(SDL_Surface* s,void* user_data, const int max)
    {

        if (!loaded) load(user_data);

        startt = get_tick_count();


        char text[2];
        text[0] = (rand() % 255);
        text[1] = 0;

        double percent = ((double)10 / (double)32767);
        int max_char = (int)(percent * (double)fft_results[2]);

        theapp->GetFonts()->set_size(FS_SYSTEM_TINY);
        int sizeoffont = theapp->GetFonts()->get_length_px(text,FS_SYSTEM_TINY);
        int divy = sizeoffont+10;
        int i = 0;
        for(int y = 0; y < DRAW_HEIGHT / divy; y++)
        {
            for(int x = 0; x < DRAW_WIDTH / divy; x++)
            {

                text[0] = (rand() % 255);
                text[1] = 0;
                int color = 0;
                if (matrix_map[x][y] <= 0)
                {
                    if((rand() % 100) > 90)
                    {
                        color = rand() % 100;
                        color+=155;

                        theapp->GetFonts()->change_color(0,color,0);
                        theapp->GetFonts()->text(s,text,x*divy ,y*divy ,640);
                        matrix_map[x][y] = color;
                    }

                }else{
                    color = matrix_map[x][y]-= max_char;
                    theapp->GetFonts()->change_color(0,color,0);
                    theapp->GetFonts()->text(s,text,x*divy ,y*divy ,640);

                }

            }

        }
    };
};
#endif
