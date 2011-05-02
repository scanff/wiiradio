#include "globals.h"
#include "hashing.h"
#include <dirent.h>

#include "application.h"
#include "script.h"

#include "gui.h"
#include "gui/gui_button.h"
#include "gui/gui_img.h"
#include "gui/gui_textbox.h"
#include "gui/gui_list.h"
#include "gui/gui_img_list.h"
#include "gui/gui_screen.h"
#include "gui/gui_popup.h"
#include "gui/gui_img_anim.h"
#include "gui/gui_visual.h"
#include "gui/gui_shape.h"
#include "gui/gui_label.h"
#include "gui/gui_font.h"
#include "gui/gui_keyboard.h"
#include "gui/gui_slider.h"

#include "skins.h"
#include "functions.h"


skins::skins(app_wiiradio* _theapp) :  theapp(_theapp), total_skins(0), current_skin(0), current_objectid(0)
{
    loopi(MAX_SKINS) skin_list[i] = 0;
    find_skins();

}

skins::~skins()
{

    loopi(MAX_SKINS)
    {
        if(skin_list[i])
        {
            delete [] skin_list[i];
            skin_list[i] = 0;
        }

    }
}

unsigned long skins::get_value_color(const char* search)
{
    char buffer[SMALL_MEM] = {0};

    char* str = get_value_string(search,buffer);

    if(!str) return 0;

    unsigned int color = 0;

    if (*str == '#') str++;

    sscanf(str, "%x", &color);

    return color;
}


int skins::get_value_int(const char* search)
{
    int value = 0;

    const char* result = theapp->GetVariables()->search_var((char*)search);
    if (result) value = atoi(result);

    return value;
}

char* skins::get_value_string(const char* search,char* buffer)
{

    memset(buffer,0,SMALL_MEM); // force clear

    const char* result = theapp->GetVariables()->search_var((char*)search);
    if (result) strcpy(buffer,result);

    return buffer;
}

unsigned long inline skins::make_color(char* value)
{
    char* p = value;
    unsigned int clr;

    if (*p == '#') p++;

    sscanf(p, "%x", &clr);

    return clr;
}

char* skins::get_value_file(const char* search,char* buffer,const char* dir)
{

    memset(buffer,0,SMALL_MEM); // force clear

    const char* result = theapp->GetVariables()->search_var((char*)search);
    if (result) sprintf(buffer,"%s%s",dir,result);

    return buffer;
}

int lookup_function_ids(char* value)
{
    loopi(sf_get_size())
    {
        if (!strcmp(on_click_fun[i].name,value))
            return i;
    }

    return 0;
}

void skins::object_add_var(char* name, char* value)
{
    string fulllocation;
    gui_object* obj     = (gui_object*)(current_obj);

    const unsigned long hash = get_hash(name);
    obj->isvariable = false;
    char t[255];
    memset(t,0,255);

    if(*value == '$' || *value == '%')
    {
        obj->isvariable = true;
        strncpy(t,value+1,strlen(value)-1);
    }
    /*
        scanff: ALL the strcmp should be hased for speed
    */
    switch(current_objectid)
    {
    case GUI_KEYBOARD:
        if(strcmp(name,"img_key_out") == 0)
        {
            fulllocation = skin_path + value;
            static_cast<gui_keyboard*>(current_obj)->SetSmallKeyOut((char*)fulllocation.c_str());
        }
        if(strcmp(name,"img_key_over") == 0)
        {
            fulllocation = skin_path + value;
            static_cast<gui_keyboard*>(current_obj)->SetSmallKeyOver((char*)fulllocation.c_str());
        }
        if(strcmp(name,"img_key_med_out") == 0)
        {
            fulllocation = skin_path + value;
            static_cast<gui_keyboard*>(current_obj)->SetMedKeyOut((char*)fulllocation.c_str());
        }
        if(strcmp(name,"img_key_med_over") == 0)
        {
            fulllocation = skin_path + value;
            static_cast<gui_keyboard*>(current_obj)->SetMedKeyOver((char*)fulllocation.c_str());
        }
        if(strcmp(name,"img_key_space_out") == 0)
        {
            fulllocation = skin_path + value;
            static_cast<gui_keyboard*>(current_obj)->SetSpaceKeyOut((char*)fulllocation.c_str());
        }
        if(strcmp(name,"img_key_space_over") == 0)
        {
            fulllocation = skin_path + value;
            static_cast<gui_keyboard*>(current_obj)->SetSpaceKeyOver((char*)fulllocation.c_str());
        }
    break;
    case GUI_FONT:
        if(strcmp(name,"set") == 0)
        {
            static_cast<gui_font*>(current_obj)->SetFont((const char*)(value));
        }
        if(strcmp(name,"size") == 0)
        {
            static_cast<gui_font*>(current_obj)->SetSize((const int)(atoi(value)));
        }

    break;

    case GUI_VISUAL:
    {
        if(strcmp(name,"changable") == 0)
        {
            static_cast<gui_visual*>(obj)->changable = atoi(value);
        }
        if(strcmp(name,"vis_type") == 0)
        {
            static_cast<gui_visual*>(obj)->vis_type = atoi(value);
        }
        if(strcmp(name,"vis_color_A") == 0)
        {
            static_cast<gui_visual*>(obj)->vis_color_A = make_color(value);
        }
        if(strcmp(name,"vis_color_B") == 0)
        {
            static_cast<gui_visual*>(obj)->vis_color_B = make_color(value);
        }
        if(strcmp(name,"vis_bgcolor") == 0)
        {
            static_cast<gui_visual*>(obj)->vis_bgcolor = make_color(value);
        }
    }break;
    case GUI_BUTTON:

            if(strcmp(name,"scroll") == 0)
            {
                static_cast<gui_button*>(obj)->can_scroll = atoi(value);
            }

            if(strcmp(name,"autoscroll") == 0)
            {
                static_cast<gui_button*>(obj)->auto_scroll_text = atoi(value);
            }
            // fall through
    case GUI_LIST:
        if (current_objectid==GUI_LIST || current_objectid==GUI_ART_GRID)
        {
            if(strcmp(name,"img_playing") == 0)
            {
                fulllocation = skin_path + value;
                obj->set_image_playing((char*)fulllocation.c_str());

            }

            if(strcmp(name,"items") == 0)
            {
                static_cast<gui_list*>(obj)->number = atoi(value);
            }
            if(strcmp(name,"vspacing") == 0)
            {
                static_cast<gui_list*>(obj)->vspacing = atoi(value);
            }
            if(strcmp(name,"icons") == 0)
            {
                static_cast<gui_list*>(obj)->icons = theapp->ui->FindObjByName_Obj(value);
            }
            if(strcmp(name,"listid") == 0)
            {
                static_cast<gui_list*>(obj)->listid = atoi(value);
            }
        }
        // fall through
    case GUI_SLIDER:
        if(strcmp(name,"img_pos_out") == 0)
        {
            fulllocation = skin_path + value;
            static_cast<gui_slider*>(obj)->set_posimage_out((char*)fulllocation.c_str());
        }
        if(strcmp(name,"img_pos_over") == 0)
        {
            fulllocation = skin_path + value;
            static_cast<gui_slider*>(obj)->set_posimage_over((char*)fulllocation.c_str());
        }
    case GUI_LABEL:
    case GUI_TEXTBOX:

        if (current_objectid==GUI_LIST)
        { // text can be an array !!! specified like 0,1,2,3,4
            if(strcmp(name,"text") == 0)
            {
                obj->set_text(value);
            }

        }else{
            if(strcmp(name,"text") == 0)
            {
                obj->set_text(value);
            }
        }

        if(strcmp(name,"limit_text") == 0)
        {
            obj->limit_text = atoi(value);
        }

        if(strcmp(name,"img_over") == 0)
        {
            fulllocation = skin_path + value;
            obj->set_image_over((char*)fulllocation.c_str());
        }

        if(strcmp(name,"img_out") == 0)
        {
            fulllocation = skin_path + value;
            obj->set_image_out((char*)fulllocation.c_str());
        }

        if(strcmp(name,"img_down") == 0)
        {
            fulllocation = skin_path + value;
            obj->set_image_down((char*)fulllocation.c_str());
        }

         if(strcmp(name,"img_on_out") == 0)
        {
            fulllocation = skin_path + value;
            obj->set_image_onout((char*)fulllocation.c_str());
        }

         if(strcmp(name,"img_on_over") == 0)
        {
            fulllocation = skin_path + value;
            obj->set_image_onover((char*)fulllocation.c_str());
        }

        if(strcmp(name,"img_on_down") == 0)
        {
            fulllocation = skin_path + value;
            obj->set_image_ondown((char*)fulllocation.c_str());
        }

        if(strcmp(name,"script_lclick") == 0)
        {
            strcpy(obj->click_script,value);
        }

        if(strcmp(name,"action_lclick") == 0)
        {
            // split the function and check for args
            int len = strlen(value);
            int args = 0;
            bool inarg = false;

            char arg[100];
            char fun[100];

            memset(arg,0,100);
            memset(fun,0,100);

            char* parg = arg;
            char* fn = fun;

            bool ignore;

            for(int i = 0 ; i < len; i++)
            {
                ignore = false;
                switch(value[i])
                {
                    case '(':
                    {
                        args++;
                        inarg = true;
                        continue;
                    }
                    break;
                    case ')':
                    {
                        inarg = false;
                        continue;

                    }
                    break;
                    case '"':
                        ignore = true;
                    break;
                }

                if (!inarg) *fn++ = value[i];
                else *parg++ = value[i];

            }
            if (*arg)
            {
                const int fun_id = lookup_function_ids(fun);
                obj->click_func = on_click_fun[fun_id].fun;
                if (*arg) strcpy(obj->fun_arg,arg);

            }else{
                const int fun_id = lookup_function_ids(value);
                obj->click_func = on_click_fun[fun_id].fun;
                if (*arg) strcpy(obj->fun_arg,arg);
            }
        }

        if(strcmp(name,"img_pad_x") == 0)
        {
          //  obj->s_y = atoi(value);
        }

        if(strcmp(name,"img_pad_x") == 0)
        {
          //  obj->s_y = atoi(value);
        }

    break;

    case GUI_IMG_LIST:

        if(strcmp(name,"images") == 0)
        {
            static_cast<gui_imglist*>(obj)->set_images((char*)skin_path.c_str(),value);
        }
    break;

    case GUI_IMG_ANIMATION:
        if(strcmp(name,"images") == 0)
        {
            static_cast<gui_imganim*>(obj)->set_images((char*)skin_path.c_str(),value);
        }
        if(strcmp(name,"time") == 0)
        {
            static_cast<gui_imganim*>(obj)->frame_time = atoi(value);
        }
    break;
    case GUI_POPUP:

        if(strcmp(name,"center_img") == 0)
        {
            static_cast<gui_popup*>(obj)->center_img = atoi(value);
        }

    }


    // -- Applies to all
    if(strcmp(name,"text_color_out") == 0)
    {
        obj->text_color = make_color(value);
    }

    if(strcmp(name,"text_color_over") == 0)
    {
        obj->text_color_over = make_color(value);
    }

    if(strcmp(name,"text_color_down") == 0)
    {
        obj->text_color_down = make_color(value);
    }
    if(strcmp(name,"show_on_var") == 0)
    {
        strcpy(obj->show_on_var,&value[1]);
    }
    if(strcmp(name,"align") == 0)
    {
        obj->center_text = atoi(value);
    }

    if(strcmp(name,"text_pad_x") == 0)
    {
        obj->pad_x = atoi(value);
    }

    if(strcmp(name,"text_pad_y") == 0)
    {
        obj->pad_y = atoi(value);
    }


    if(strcmp(name,"text") == 0)
    {
        obj->set_text(value);
    }
    if(strcmp(name,"img") == 0)
    {
        fulllocation = skin_path + value;
        obj->set_image_img((char*)fulllocation.c_str());
    }
    if(strcmp(name,"show_status") == 0)
    {
        obj->set_show_status(value);
    }

    if(strcmp(name,"bgcolor") == 0)
    {
        obj->bgcolor.color = make_color(value);
    }
    if(strcmp(name,"text_size") == 0)
    {
        obj->font_sz = atoi(value);
    }
    if(strcmp(name,"text_color_playing") == 0)
    {
        obj->text_color_playing = make_color(value);
    }


    switch(hash)
    {
        case HASH_name:
            obj->set_name(value,get_hash(value));
        break;
        case HASH_screen:
            //if (obj->isvariable)
            {
               obj->set_binds(value);
            }
        break;

        case HASH_x:
            obj->s_x = atoi(value);
        break;

        case HASH_y:
            obj->s_y = atoi(value);
        break;

        case HASH_w:
            if (obj->isvariable)
            {
                obj->s_w = theapp->GetVariables()->search_var_int(value);
            }else
                obj->s_w = atoi(value);
        break;

        case HASH_h:
            if (obj->isvariable)
            {
                obj->s_h = theapp->GetVariables()->search_var_int(value);
            }else
                obj->s_h = atoi(value);
        break;

        case HASH_z_order:
        {
            int z = atoi(value);
            if(obj->parent)
            {
                z += obj->parent->z_order;
            }

            obj->z_order = z;

        }break;

        case HASH_visible:
            obj->visible = atoi(value);
        break;

    }

}

void skins::set_oject_id(char* name)
{
//TODO HASH THE strings to remove the ugly slow strcmp's .. we can also uses a switch then
    last_obj = current_obj;

    if(HASH_gui_button == get_hash(name))
    {
        current_objectid = GUI_BUTTON;
        gui_button* x = new gui_button(this->theapp);
        current_obj = x;
    }

    if(strcmp(name,"gui_slider") == 0)
    {
        current_objectid = GUI_SLIDER;
        gui_slider* x = new gui_slider(this->theapp);
        current_obj = x;
    }


    if(strcmp(name,"gui_font") == 0)
    {
        current_objectid = GUI_FONT;
        gui_font* x = new gui_font(this->theapp);
        current_obj = x;
    }

    if(strcmp(name,"gui_keyboard") == 0)
    {
        current_objectid = GUI_KEYBOARD;
        gui_keyboard* x = new gui_keyboard(this->theapp);
        current_obj = x;
    }

    if(strcmp(name,"gui_img") == 0)
    {
        current_objectid = GUI_IMG;
        gui_img* x = new gui_img(this->theapp);
        current_obj = x;
    }

    if(strcmp(name,"gui_label") == 0)
    {
        current_objectid = GUI_LABEL;
        gui_label* x = new gui_label(this->theapp);
        current_obj = x;
    }

    if(strcmp(name,"gui_shape") == 0)
    {
        current_objectid = GUI_SHAPE;
        gui_shape* x = new gui_shape(this->theapp);
        current_obj = x;

    }
    if(strcmp(name,"gui_textarea") == 0)
    {
        current_objectid = GUI_TEXTBOX;
        gui_textbox* x = new gui_textbox(this->theapp);
        current_obj = x;
    }

    if(strcmp(name,"gui_list") == 0)
    {
        current_objectid = GUI_LIST;
        gui_list* x = new gui_list(this->theapp);
        current_obj = x;
    }
    if(strcmp(name,"gui_imagelist") == 0)
    {
        current_objectid = GUI_IMG_LIST;
        gui_imglist* x = new gui_imglist(this->theapp);
        current_obj = x;
    }
    if(strcmp(name,"gui_imageanim") == 0)
    {
        current_objectid = GUI_IMG_ANIMATION;
        gui_imganim* x = new gui_imganim(this->theapp);
        current_obj = x;
    }

    if(strcmp(name,"gui_screen") == 0)
    {
        current_objectid = GUI_SCREEN;
        gui_screen* x = new gui_screen(this->theapp);
        current_obj = x;
        last_obj = 0; // can't have a parent !!!

        theapp->ui->new_object(current_objectid,current_obj);
    }

    if(strcmp(name,"gui_popup") == 0)
    {
        current_objectid = GUI_POPUP;
        gui_popup* x = new gui_popup(this->theapp);
        current_obj = x;
        theapp->ui->new_object(current_objectid,current_obj);
    }

    if(strcmp(name,"gui_visual") == 0)
    {
        current_objectid = GUI_VISUAL;
        gui_visual* x = new gui_visual(this->theapp);
        current_obj = x;
        theapp->ui->new_object(current_objectid,current_obj);
    }

    // Set parent
    if (current_obj)
    {
        gui_object* c = (gui_object*)current_obj;
        c->parent = (gui_object*)last_obj;
    }

}

void inline skins::release_object_id()
{

    if (current_objectid == GUI_KEYBOARD)
    {
        gui_keyboard* gk = (gui_keyboard*)(current_obj);
        gk->create();
    }
    if (current_objectid == GUI_LIST)
    { // create the list
        gui_list* x = (gui_list*)(current_obj);
        x->create();
    }
    if (current_objectid == GUI_IMG_ANIMATION)
    {
        gui_imganim* x = (gui_imganim*)(current_obj);
        x->create();

    }
    if (current_objectid == GUI_IMG_LIST)
    {
        gui_imglist* x = (gui_imglist*)(current_obj);
        x->create();
    }

    if ((current_objectid != GUI_VISUAL) && (current_objectid != GUI_SCREEN) && (current_objectid != GUI_POPUP)) // -- Screen should be created first
        theapp->ui->new_object(current_objectid,current_obj);

    if (current_obj == last_obj && current_objectid == GUI_UNKNOWN)
    { // end of a parent ?
        last_obj = 0;
    }

    current_objectid = GUI_UNKNOWN;
    current_obj = last_obj;

}

void skins::parser_skin_file(char* skin_data,char** script_start, char** script_end)
{

    char name[MED_MEM] = {0};
    char value[MED_MEM] = {0};
    char line[MED_MEM] = {0};

    char* ptr = skin_data;
    char* dst_ptr; // where the data will go ... var name or value
    bool line_done;
    bool is_string = false;
    char last_char = 0;
    bool ignore = false;
    bool isscript = false;
    int i = 0;

    while(*ptr)
    {
        last_char = 0;
        i = 0;
        // -- get a line
        while((*ptr != '\n' && *ptr != '\r') && (*ptr) && (i<MED_MEM-1))
            line[i++] = *ptr++;

        // -- parse the line
        memset(name,0,MED_MEM);
        memset(value,0,MED_MEM);
        line_done = false;
        dst_ptr = name;

        for(int p=0;p<=i;p++)
        {
            if (p) last_char = line[p-1];


            if(isscript)
            {
                if (line[p] != '}')
                    continue;
            }
/*            if (isspace(line[p]) && !is_string)
            {
                continue; // ignore whitespaces, unless there a value like "hello there"
            }
*/
            switch(line[p])
            {
                //isspace
                case '\t':
                case '\n':
                case '\v':
                case '\f':
                case '\r':
                case ' ':
                {
                    if(!is_string)
                    {
                        continue;
                    }


                }
                break;

                case '/':
                    if (last_char == '/')
                    {
                        line_done = true; // nothing else on this line matters
                    }
                    if (last_char == '*')
                        ignore = false;
                break;

                case '*':
                    if (last_char == '/')
                    {
                        // move pointer back
                        ignore = true;
                    }
                break;

                case '=':
                    if(!is_string) // could be = in a value string
                    {
                        dst_ptr = value;
                    }

                continue; // skip the = char

                case '{':
                    if (strcmp(name,"lua_script")==0)
                    {
                        *script_start = ptr;
                        isscript = true;
                        continue;
                    }
                    set_oject_id(name);
                    line_done = true;
                break;

                case '}':
                    if (isscript)
                    {
                        *script_end = ptr-1;
                        isscript = false;
                        continue;
                    }
                    release_object_id();
                    line_done = true;
                break;

                case '"':
                case '\'':
                        // just ignore these
                    is_string = true;
                continue;

            }

            if (!ignore)
                *dst_ptr++ = line[p];

            if(line_done)
                break;

        }

        /*  Ok we have a variable, we can add to the "general" var stack or if it's bound to an
            object save it there
        */
        if(!isscript)
        {
            if (*name && *value && !current_objectid)
            {
                theapp->GetVariables()->add_var(name,value);
            }
            else if (*name && *value && current_objectid)
            {
                // add to object
                object_add_var(name,value);
            }
        }

        is_string = false; // reset
        memset(line,0,MED_MEM);
        if (*ptr)
            ptr++;
    }



}

const bool skins::load_skin(const char* dir)
{
    last_obj = current_obj = 0;

    FILE* f = 0;
    char fullpath[SMALL_MEM] = {0};

    sprintf(fullpath,"%sskin.txt",dir);

    skin_path = dir;

    f = fopen(make_path(fullpath),"rb");
    if (!f) return false;

    fseek(f,0,SEEK_END);
    unsigned int size = ftell(f);
    fseek(f,0,SEEK_SET);


    char* skin_data = new char[size+1];
    if (!skin_data)
    {
        fclose(f);
        return false;
    }


    int read = 1;
    int pos = 0;

    while(read > 0)
    {
        read = fread(skin_data+pos,1,size,f);
        if(read > 0)
        {
            pos+=read;
        }
    }

    skin_data[size] = '\0';

    char* script_s = NULL;
    char* script_e = NULL;

    parser_skin_file(skin_data,&script_s,&script_e);

    // load any lua script define in the skin file
    if(script_s && script_e)
    {
        *script_e = '\0';
        theapp->GetScript()->parse_skindata(script_s);

    }

    delete [] skin_data;

    fclose(f);

    return true;
}

void skins::find_skins()
{
    DIR *dir,*check;
    struct dirent *ent;

    char path[SMALL_MEM]={0};

    total_skins = 0; // clear

    dir = opendir (make_path((char*)"data/skins/"));

    if (dir != NULL) {

    while ((ent = readdir (dir)) != NULL)
    {

      if(strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0)
      {
          continue;
      }

        // -- check this is a skin dir....

        sprintf(path,"data/skins/%s",ent->d_name);
        check = 0;
        check = opendir (make_path(path));

        if (check)
        {

          while ((ent = readdir (check)) != NULL)
          {
              string_lower(ent->d_name);

               if(strcmp(ent->d_name,"skin.txt") == 0) //ok this is a skin dir
               {

                    char* p = new char[SMALL_MEM];
                    if (!p) break;

                    memset(p,0,SMALL_MEM);

                    strcpy(p,path);

                    skin_list[total_skins] = p;

                    total_skins++;
                    break;
               }

          }
          closedir (check);

        }

    }
      closedir (dir);

    } else { return; }


}
// -- get_current_skin()
// -- return the skin list
char* skins::get_current_skin()
{
    // return a friendly name
    memset(friendly,0,SMALL_MEM);
    char* c = skin_list[current_skin];
    while(*c++ != '/' && *c != 0);

    strcpy(friendly,c);

    string_lower(friendly);

    return friendly;
}

char* skins::get_current_skin_path()
{
    if (current_skin < 0 || current_skin >= total_skins) return 0; //safety

    char *value = new char[SMALL_MEM];
    snprintf(value,SMALL_MEM-1,"%s/",skin_list[current_skin]);
    value[SMALL_MEM-1] = '\0';

    return value;
}

void skins::goto_next_skin()
{
    if (current_skin < total_skins-1) current_skin++;
    else current_skin = 0;
}

void skins::set_current_skin(char* path)
{
    char search_value[SMALL_MEM] = {0};

    loopi(total_skins)
    {
        sprintf(search_value,"%s/",skin_list[i]);
        if(strcmp(search_value,path) == 0) {
            current_skin = i;
            break;
        }
    }
}

char* skins::string_lower(char* x)
{
    char* p = x;

    while(*p) {
      *p = tolower(*p);
      p++;
    }

    return x;
}
