#ifndef GENRE_LIST_H_INCLUDED
#define GENRE_LIST_H_INCLUDED

struct _glist
{
    _glist() : genre_name(0), next(0) {};

    char* genre_name;
    struct _glist *next;

    void add(const char* iname)
    {

        genre_name = new char[SMALL_MEM];
        memset(genre_name,0,SMALL_MEM);

        strcpy(genre_name,iname);

    }
};

class genre_list{
    public:

    genre_list();
    ~genre_list();

    struct _glist *first;
    struct _glist *current;

private:
    int  total;
    int  position;
    int  view_max;
public:

    void SetMax(const int m);
    const int GetSize();
    void MoveBack();
    void MoveNext();
    const int GetPosition();

    const char* get_genre(const int num);
    _glist* dup_genre(const char* name);
    void add_genre(const char* name);
    void delete_all();
    void parser_file(char* data);
    void save_file();
    bool load_file();
    void reload_file();

};

#endif // GENRE_LIST_H_INCLUDED
