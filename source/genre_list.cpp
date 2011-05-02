#include "globals.h"
#include "application.h"
#include "genre_list.h"


genre_list::genre_list() : first(0), current(0), total(0), position(0), view_max(0) {}

genre_list::~genre_list()
{
    save_file();
    delete_all();
}

void genre_list::SetMax(const int m) {view_max = m; }
const int genre_list::GetSize() { return total; }
const int genre_list::GetPosition() { return position; }
void genre_list::MoveNext()
{
    if (position + view_max > total)
        return;

    position += view_max;
}

void genre_list::MoveBack()
{
    if (position - view_max <= 0)
    {
        position = 0;
        return;
    }

    position -= view_max;
}

const char* genre_list::get_genre(const int num)
{
    int r_pos = num + position;

    _glist * current = first;

    int i = 0;
    while(current)
    {
        if (i==r_pos) break;

        i++;
        current = current->next;
    }

    return current ? current->genre_name : 0;

}

_glist* genre_list::dup_genre(const char* name)
{
    _glist *current = first;
    while(current)
    {
        if (strcmp(current->genre_name,name) == 0) break;

        current = current->next;
    }

    return current ? current : 0;

}

void genre_list::add_genre(const char* name)
{

    _glist* x = dup_genre(name); // duplicate?

    if (!x)
    {
        x = new _glist;
        if (!x) return;

        x->add(name);

        if (!first) current = first = x;
        else
        {
            current->next = x;
            current = x;
        }

        total++;
    }
}

void genre_list::delete_all()
{
    current = first;
    _glist *next = 0;

    while(current)
    {

        if (current->genre_name)
        {
            delete [] current->genre_name;
            current->genre_name = 0;
        }

        next = current->next;

        delete current;

        current = next;
    }

    current = first = 0;

}

void genre_list::parser_file(char* data)
{

    char line[SMALL_MEM];
    char* ptr = data;

    int i = 0;
    while(*ptr)
    {
        memset(line,0,SMALL_MEM);

        i = 0;
        // -- get a line
        while((*ptr != '\n' && *ptr != '\r') && (*ptr) && (i<SMALL_MEM))
            line[i++] = *ptr++;

        if(!line[0])
        {
            *ptr++;
            continue;
        }

        add_genre(line);


        ptr++;
    }

}

void genre_list::save_file()
{
    FILE* f = 0;

    f = fopen(make_path((char*)"data/genres.dat"),"wb");
    if (!f) return;

    _glist *current = first;
    while(current)
    {
        if(current->genre_name != 0 && *current->genre_name)
        {
            fwrite(current->genre_name,1, strlen(current->genre_name), f);
            fwrite("\n",1,1,f);
        }

        current = current->next;
    }
    fclose(f);
}

void genre_list::reload_file()
{
    save_file();
    load_file();
}

bool genre_list::load_file()
{
    delete_all();
    total = 0;

    FILE* f = 0;

    f = fopen(make_path((char*)"data/genres.dat"),"rb");
    if (!f) return false;

    fseek(f,0,SEEK_END);
    unsigned int size = ftell(f);
    fseek(f,0,SEEK_SET);


    char* data = new char[size+1];
    if (!data)
    {
        fclose(f);
        return false;
    }

    memset(data,0,size+1);

    fread(data,1,size,f);

    parser_file(data);

    delete [] data;

    fclose(f);

    return true;
}
