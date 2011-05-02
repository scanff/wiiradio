#include "globals.h"
#include "application.h"
#include "variables.h"

/*

bool inline is_integer(const char* ivalue )
{
    unsigned char* ptr = (unsigned char*)ivalue;
    while(*ptr)
    {
        const unsigned char c = *ptr++;
        if (c < 0x30 || c > 0x39) return false;
    }

    return true;
}
*/

wrVar::wrVar() : vHashName( 0 ), vName( NULL ), vHashValue( 0 ), vValue( NULL), vValueSize( 0 ) {}

wrVar::~wrVar()
{
    if (vName) free(vName);
    if (vValue) free(vValue);

    vValue = vName = NULL;
}

void wrVar::wrAdd(const char* n, const char* v)
{
    vHashName   = MAKE_HASH(n);
    vHashValue  = MAKE_HASH(v);

    vName   = (char*)malloc(WRVAR_MAX);
    if (!vName)
        exit(0); // handle

    int len_name = strlen(n);
    if(len_name >= WRVAR_MAX)
        exit(0);

    strcpy(vName,n);
    vName[len_name] = '\0';



    vValue  = (char*)malloc(WRVAR_MAX);
    if(!vValue)
        exit(0); // handle

    int lenval = strlen(v);
    if(lenval >= WRVAR_MAX)
        exit(0);

    strcpy(vValue,v);
    vName[lenval] = '\0';

}

void wrVar::wrUpdate(const char* v)
{
    if(!v)
        return;


    const int len = strlen(v);
    if(len >= WRVAR_MAX || len <= 0 || !vValue)
        return;


    strncpy(vValue,v,len);
    vValue[len] = '\0';

    vHashValue  = MAKE_HASH(vValue);
}


// access class
wiiradio_variables::wiiradio_variables(app_wiiradio* _theapp) :
    theapp(_theapp)
{
}

wiiradio_variables::~wiiradio_variables()
{
    delete_all();
}

const int wiiradio_variables::search_var_int(const char* name)
{
     // -- hash it
    const u32 hash = MAKE_HASH(name);

    loopi(var_list.size())
    {
        if (var_list[i]->vHashName == hash)
             return atoi(var_list[i]->vValue);
    }

    return 0;
}

const char* wiiradio_variables::search_var(const char* name)
{
    // -- hash it
    const u32 hash = MAKE_HASH(name);

    loopi(var_list.size())
    {
        if (var_list[i]->vHashName == hash)
             return var_list[i]->vValue;
    }

    return 0;

};

const u32 wiiradio_variables::search_var_return_hash(const char* name)
{
    // -- hash it
    const u32 hash = MAKE_HASH(name);

    loopi(var_list.size())
    {
        if (var_list[i]->vHashName == hash)
        {
            return var_list[i]->vHashName;
        }
    }

    return 0;

};

const u32 wiiradio_variables::search_var_return_name_hash(const char* name)
{
    // -- hash it
    const u32 hash = MAKE_HASH(name);

    loopi(var_list.size())
    {
        if (var_list[i]->vHashName == hash)
        {
            return var_list[i]->vHashName;
        }
    }

    return 0;

};


wrVar inline *wiiradio_variables::dup_var(const char* name)
{
    // -- hash it
    const u32 hash = MAKE_HASH(name);

    loopi(var_list.size())
    {
        if (hash == var_list[i]->vHashName)
            return var_list[i]; // found
    }

    return 0;
}

void wiiradio_variables::add_var_array(const char* name, const char* value, const int idx)
{
    char new_var[MED_MEM];
    sprintf(new_var,"%s%d",name,idx);

    add_var(new_var,value);
}

void wiiradio_variables::add_var(const char* name, const char* value)
{
    if (name[0] == '%') return; // external variable

    wrVar* x = dup_var(name); // duplicate?

    if (!x)
    {
        // create new variable

        x = new wrVar;
        if (!x) return;

        x->wrAdd(name, value);
        var_list.push_back(x);

    }else{
        // update
        x->wrUpdate(value);

    }
}

void wiiradio_variables::delete_all()
{
    loopi(var_list.size())
    {
        delete var_list[i];
    }

    var_list.clear();

}
