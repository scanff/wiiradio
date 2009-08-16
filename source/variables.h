#ifndef VARIABLES_H_INCLUDED
#define VARIABLES_H_INCLUDED
/*
// useful variables
$GENRE
$VOLUME
$STATUS
$URL
$BR
$TITLE
$MUTE
$SKINNAME
$SKINAUTHOR


*/
class wiiradio_variables
{
    public:

    struct var
    {
        var() : name(0), value(0), next(0) {};

        unsigned long name; // store name as hash, for speed
        char* value;
        struct var *next;

        void add(char* iname, char* ivalue)
        {

            unsigned long hash = 0;
            const char* ptr = iname;

            while (*ptr) hash = (hash << 5) + hash + *ptr++;

            name = hash;

            value = new char[SMALL_MEM];
            memset(value,0,SMALL_MEM);

            strcpy(value,ivalue);

        };

        void update(char* ivalue)
        {
            memset(value,0,SMALL_MEM);
            strcpy(value,ivalue);
        };

    };

    struct var *first;
    struct var *current;

    wiiradio_variables() : first(0), current(0) {};

    ~wiiradio_variables()
    {
        delete_all();
    };

    char* search_var(const char* name)
    {
        // -- hash it
        unsigned long hash = 0;
        const char* ptr = name;

        while (*ptr) hash = (hash << 5) + hash + *ptr++;

        var *current = first;
        while(current)
        {
            if (current->name == hash) break;

            current = current->next;
        }

        return current ? current->value : 0;

    };

    var* dup_var(char* name)
    {
        // -- hash it
        unsigned long hash = 0;
        const char* ptr = name;

        while (*ptr) hash = (hash << 5) + hash + *ptr++;

        var *current = first;
        while(current)
        {
            if (current->name == hash) break;

            current = current->next;
        }

        return current ? current : 0;

    };


    void add_var(char* name, char* value)
    {

        var* x = dup_var(name); // duplicate?
        if (!x)
        {
            // create new variable

            x = new var;
            if (!x) return;

            x->add(name,value);

            if (!first) current = first = x;
            else
            {
                current->next = x;
                current = x;
            }
        }else{
            // update
            x->update(value);

        }
    };

    void delete_all()
    {
        current = first;
        var *next = 0;

        while(current)
        {

            if (current->value)
            {
                delete [] current->value;
                current->value = 0;
            }

            next = current->next;

            delete current;

            current = next;
        }

        current = first = 0;

    };

};


#endif // VARIABLES_H_INCLUDED
