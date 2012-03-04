#include "globals.h"
#include "application.h"
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include "script.h"
#include "functions.h"
#include "gui/gui_object.h"
#include "gui.h"

static script* pLuaScript = 0;

script::script(app_wiiradio* _theapp) :
    theapp(_theapp)
{
    pLuaScript = this;
}

script::~script()
{
    lua_close(m_LuaScript);

}
#include "hashing.h"
static inline void SetProperty(gui_object* obj, char* value, const u32 hash)
{
    app_wiiradio* theapp = (app_wiiradio*)pLuaScript->theapp;

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

static void c_set_obj_property(const char* objname,const char* propery, const char* value)
{
    const app_wiiradio* obj = (app_wiiradio*)pLuaScript->theapp;
    gui* ui = (gui*)(obj->ui);

    gui_object* obj2 = ui->FindObjByName_Obj(objname);
    if (!obj2) return;

    SetProperty(obj2, (char*)value, MAKE_HASH(propery));


}
static int
GuiInterface(lua_State *L)
{
    c_set_obj_property(luaL_checkstring(L, 1),luaL_checkstring(L, 2),luaL_checkstring(L, 3));
    return 0;
}
static int
getguiobject(lua_State *L)
{
    lua_pushcfunction(L, &GuiInterface);
    return 1;
}


void script::init()
{
#ifdef _WII_
    fatInitDefault();
#endif
    m_LuaScript = lua_open();
    luaL_openlibs( m_LuaScript );

    SetScriptPath(m_LuaScript, make_path("data/scripts/?.lua"));
    luaL_dofile(m_LuaScript, make_path("data/scripts/wiiradio.lua"));
    luaL_loadfile(m_LuaScript, make_path("data/scripts/wiiradio.lua"));

    // Allows us to change gui object properties from within Lua script e.g.
    /*
        p = wrSetObj()

        -- 1. "Name" what we've called the object in the skin file
        -- 2. "property" variable .. i.e. x,y,z-order ... etc.
        -- 3. the value we want to set

        p('main','x','100')

    */
    lua_pushcfunction(m_LuaScript, getguiobject);
    lua_setglobal(m_LuaScript, "wrSetObj");


}

// set the golbal path for Lua, if not correct require'' in lua scripts will fail
int script::SetScriptPath( lua_State* L, const char* path )
{
    // THIS REPLACES ALL OTHER PATHS ... IF you need defaults then append path to the current ones

    lua_getglobal( L, "package" );
    lua_getfield( L, -1, "path" );

    // Current paths .. could append to these if needed like cur_path;path
    //const char* cur_path = lua_tostring( L, -1 );
    lua_tostring( L, -1 );
    lua_pop( L, 1 );
    lua_pushstring( L, path/*cur_path*/ );
    lua_setfield( L, -2, "path" );
    lua_pop( L, 1 );

    return 0; // all done!
}

// SKIN FILE STUFF

// Adds any scripts defined in a skin file
int script::parse_skindata(const char* file)
{
  //  int t = strlen(file);
  //  FILE* f = fopen("c:\\asfdas.txt","wb");
  //  fwrite(file,1,t,f);
  //  fclose(f);
    AddScript(file);

    return 1;

}

inline void script::AddScript(const char* script)
{

    luaL_dostring(m_LuaScript,script);

}

void script::AddVariable(const char* var, const char* val)
{
    char newvar[1024];
    sprintf(newvar,"%s = \"%s\"\n",var,val);

    AddScript(newvar);

}


const char* script::GetVariable(const char* var)
{
    const char* res;


    lua_State* L = this->m_LuaScript;
    lua_getglobal(L, var);
    if(!lua_isstring(L, -1))
    {
        return 0;
    }

    res = lua_tostring(L, -1);



    return res;
}

const int script::GetVariableInt(const char* var)
{

    lua_State* L = this->m_LuaScript;
    lua_getglobal(L, var);
    const int res = lua_tointeger(L, -1);


    return res;
}

const void script::SetVariableInt(const char* var, const int val)
{
    lua_State* L = this->m_LuaScript;

    lua_getglobal(L, var);
    lua_pushinteger(L,val);
    lua_setglobal(L,var);

}

const void script::SetVariableString(const char* var, const char* val)
{

    lua_State* L = this->m_LuaScript;


    lua_getglobal(L, var);
    if(lua_isnoneornil(L, -1))
    {
        return;
    }

    lua_pushstring(L,val);
    lua_setglobal(L,var);


}

void script::CallVoidVoidFunction(const char* name)
{


    lua_State* L = this->m_LuaScript;

    lua_getglobal(L, name);
    if(!lua_isfunction(L, -1))
    {
        return;
    }

    lua_call(L, 0, 0);


}

void script::OnKey(const char* key)
{
    lua_State* L = this->m_LuaScript;

    lua_getglobal(L, "onkey");
    lua_pushstring(m_LuaScript, key);
    lua_call(L, 1, 0);

}

void script::Visualizer_PP(int x, int y)
{
    lua_State* L = this->m_LuaScript;

    lua_getglobal(L, "visual_pp");
    lua_pushnumber(m_LuaScript, x);
    lua_pushnumber(m_LuaScript, y);
    lua_call(L, 2, 0);

}
