#ifndef GENRE_LIST_H_INCLUDED
#define GENRE_LIST_H_INCLUDED


/*#define MAX_GENRE (19)
const char* genres[] = {
  "all","ambient","country","classical",
  "dance","indie","jazz","metal","pop","rave",
  "rap","rock","techno","trance","90s","80s","70s","60s",
  "50s"
};*/
class genre_list{
    public:

    struct _glist
    {
        _glist() : genre_name(0), next(0) {};

        char* genre_name;
        struct _glist *next;

        void add(char* iname)
        {

            genre_name = new char[SMALL_MEM];
            memset(genre_name,0,SMALL_MEM);

            strcpy(genre_name,iname);

        };


    };

    genre_list() : first(0), current(0), total(0) {};

    ~genre_list()
    {
        save_file();
        delete_all();
    };

    struct _glist *first;
    struct _glist *current;
    int  total;

    char* get_genre(int num)
    {
        _glist * current = first;

        int i = 0;
        while(current)
        {
            if (i==num) break;

            i++;
            current = current->next;
        }

        return current ? current->genre_name : 0;

    };

    _glist* dup_genre(char* name)
    {
        _glist *current = first;
        while(current)
        {
            if (strcmp(current->genre_name,name) == 0) break;

            current = current->next;
        }

        return current ? current : 0;

    };


    void add_genre(char* name)
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
    };

    void delete_all()
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

    };

    void parser_file(char* data)
    {

        char line[MED_MEM] = {0};
        char* ptr = data;

        int i = 0;
        while(*ptr)
        {
            i = 0;
            // -- get a line
            while((*ptr != '\n' && *ptr != '\r') && (*ptr) && (i<MED_MEM))
                line[i++] = *ptr++;

            add_genre(line);

            memset(line,0,MED_MEM);
            ptr++;
        }


    };

    void save_file()
    {
        FILE* f = 0;

        f = fopen(make_path((char*)"genres.dat"),"w");
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
    };


    bool load_file()
    {
        delete_all();

        FILE* f = 0;

        f = fopen(make_path((char*)"genres.dat"),"r");
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

        fread(data,size,1,f);

        parser_file(data);

        delete [] data;

        fclose(f);

        return true;
    };

};



#endif // GENRE_LIST_H_INCLUDED
