#ifndef RIPPER_H_INCLUDED
#define RIPPER_H_INCLUDED

class stream_ripper
{
public:

    FILE*   rip_file;
    char    curren_file[MED_MEM];

    stream_ripper();
    ~stream_ripper();

    void new_file(char* name);
    void close_file();
    void save_data(char* data,int len);

};


#endif // RIPPER_H_INCLUDED
