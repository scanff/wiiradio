#ifndef GUI_LABEL_H_INCLUDED
#define GUI_LABEL_H_INCLUDED

class gui_label: public gui_object{

    public:

    gui_label(app_wiiradio* _theapp);

    ~gui_label();

    virtual int draw();

};


#endif
