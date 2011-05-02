#ifndef ID3_H_INCLUDED
#define ID3_H_INCLUDED


// -- Quick id3 parser by scanff

// -- See http://www.id3.org/d3v2.3.0
struct id3frame
{
    char frame_id[4];
    char frame_size[4];
    char frame_flags[2];
};

struct id3APIC
{
    char    text_encoding;
    string  MIME_type;
    char    picture_type;
    string  description;
    unsigned char* picture_data;
};

class app_wiiradio;
class id3
{
    public:

    app_wiiradio*      theapp;

    FILE*              f;
    unsigned int       filesize;
    unsigned char      id3_header[10];

    string    title;
    string    artist;
    string    album;
    string    year;
    string    genre;
    string    comments;

    int         hdr_size;

    bool        is_id3;

    id3frame    frame;
    id3APIC     frame_APIC;

    SDL_RWops*      APIC_rw_ops;
    SDL_Surface*    APIC_Surface;


    id3(app_wiiradio* _theapp);
    ~id3();

    void clear();
    int newfile(FILE* _f,const unsigned int _filesize);
    int inline id3_APIC_image(const int image_size,FILE* art_file);
    void FreeAPIC();

    private:


};


#endif // ID3_H_INCLUDED
