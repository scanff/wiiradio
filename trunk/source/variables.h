#ifndef VARIABLES_H_INCLUDED
#define VARIABLES_H_INCLUDED

#include <vector>
#define WRVAR_MAX   (256)

class wrVar
{
    public:

    wrVar();
    ~wrVar();

    void wrAdd(const char* n, const char* v);
    void wrUpdate(const char* v);

    //private:

    u32     vHashName;
    char*   vName;

    u32     vHashValue;
    char*   vValue;

    int     vValueSize;
};

class app_wiiradio;
class wiiradio_variables
{
    public:
    wiiradio_variables(app_wiiradio*);
    ~wiiradio_variables();

    private:

    app_wiiradio*   theapp;

    vector<wrVar*>  var_list;

    public:

    // return the hash of a variable name or zero if variable does not exist
    const u32 search_var_return_name_hash(const char* name);
    const int search_var_int(const char* name);
    const char* search_var(const char* name);
    const u32 search_var_return_hash(const char* name);

    void add_var_array(const char* name, const char* value, const int idx);
    void add_var(const char* name, const char* value);
    void delete_all();


    private:

    wrVar inline *dup_var(const char* name);
};


#endif // VARIABLES_H_INCLUDED
