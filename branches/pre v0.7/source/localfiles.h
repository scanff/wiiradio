#ifndef _LOCALFILES_H_
#define _LOCALFILES_H_

class localfile : public station {
  public:
    string       filename;
    bool        isfolder;

    localfile(){};
    localfile(string name, string path, string filename, bool folder);
};

class localfiles {
    public:
    // starting path for local dirs
    #define F_PATH_LOCAL ("music/")

    vector<localfile>   list;
    int                 total_num_files;
    char                current_path[MAX_PATH];
    int                 current_position;

    localfiles();

    string friendly_name(string s);

    private:

    void inline clear_list();
    bool inline add_file(const char* filename,const char* path,const bool folder);

    public:

    void dirup();
    void directory_list(const char* new_path);

};

#endif
