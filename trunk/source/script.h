#ifndef SCRIPT_H_INCLUDED
#define SCRIPT_H_INCLUDED

class app_wiiradio;
struct lua_State;
class script
{
    public:

    script(app_wiiradio* _theapp);
    ~script();

    void init();

    app_wiiradio *theapp;

    lua_State* m_LuaScript;
    int SetScriptPath( lua_State* L, const char* path );
    int parse_skindata(const char* file);

    const char* GetVariable(const char* var);
    const int GetVariableInt(const char* var);

    const void SetVariableInt(const char* var, const int val);
    const void SetVariableString(const char* var, const char* val);

    void AddVariable(const char* var, const char* value);
    inline void AddScript(const char* script);
    void CallVoidVoidFunction(const char* name); // call a void (void) script function

    void OnKey(const char* key); // calls the lua onkey if defined

    private:




};


#endif // SCRIPT_H_INCLUDED
