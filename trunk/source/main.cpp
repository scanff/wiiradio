#include "globals.h"
#include "application.h"

int main(int argc, char **argv)
{
    app_wiiradio* theapp = new app_wiiradio();

    theapp->wii_radio_main(argc, argv);
    delete theapp;

    SDL_Quit();
    exit(0);
}
