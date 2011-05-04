#include "globals.h"
#include <mad.h>
#include "id3.h"
#include "localfiles.h"
#include "application.h"


// -- Quick id3 parser by scanff

// -- See http://www.id3.org/d3v2.3.0


const char *id3_genres[] =
{
    "Blues",
    "Classic Rock",
    "Country",
    "Dance",
    "Disco",
    "Funk",
    "Grunge",
    "Hip-Hop",
    "Jazz",
    "Metal",
    "New Age",
    "Oldies",
    "Other",
    "Pop",
    "R&B",
    "Rap",
    "Reggae",
    "Rock",
    "Techno",
    "Industrial",
    "Alternative",
    "Ska",
    "Death Metal",
    "Pranks",
    "Soundtrack",
    "Euro-Techno",
    "Ambient",
    "Trip Hop",
    "Vocal",
    "Jazz+Funk",
    "Fusion",
    "Trance",
    "Classical",
    "Instrumental",
    "Acid",
    "House",
    "Game",
    "Sound Clip",
    "Gospel",
    "Noise",
    "Alternative Rock",
    "Bass",
    "Soul",
    "Punk",
    "Space",
    "Meditative",
    "Instrumental Pop",
    "Instrumental Rock",
    "Ethnic",
    "Gothic",
    "Darkwave",
    "Techno-Industrial",
    "Electronic",
    "Pop-Folk",
    "Eurodance",
    "Dream",
    "Southern Rock",
    "Comedy",
    "Cult",
    "Gangsta Rap",
    "Top 40",
    "Christian Rap",
    "Pop/Punk",
    "Jungle",
    "Native American",
    "Cabaret",
    "New Wave",
    "Phychedelic",
    "Rave",
    "Showtunes",
    "Trailer",
    "Lo-Fi",
    "Tribal",
    "Acid Punk",
    "Acid Jazz",
    "Polka",
    "Retro",
    "Musical",
    "Rock & Roll",
    "Hard Rock",
    "Folk",
    "Folk/Rock",
    "National Folk",
    "Swing",
    "Fast-Fusion",
    "Bebob",
    "Latin",
    "Revival",
    "Celtic",
    "Blue Grass",
    "Avantegarde",
    "Gothic Rock",
    "Progressive Rock",
    "Psychedelic Rock",
    "Symphonic Rock",
    "Slow Rock",
    "Big Band",
    "Chorus",
    "Easy Listening",
    "Acoustic",
    "Humour",
    "Speech",
    "Chanson",
    "Opera",
    "Chamber Music",
    "Sonata",
    "Symphony",
    "Booty Bass",
    "Primus",
    "Porn Groove",
    "Satire",
    "Slow Jam",
    "Club",
    "Tango",
    "Samba",
    "Folklore",
    "Ballad",
    "power Ballad",
    "Rhythmic Soul",
    "Freestyle",
    "Duet",
    "Punk Rock",
    "Drum Solo",
    "A Capella",
    "Euro-House",
    "Dance Hall",
    "Goa",
    "Drum & Bass",
    "Club-House",
    "Hardcore",
    "Terror",
    "indie",
    "Brit Pop",
    "Negerpunk",
    "Polsk Punk",
    "Beat",
    "Christian Gangsta Rap",
    "Heavy Metal",
    "Black Metal",
    "Crossover",
    "Comteporary Christian",
    "Christian Rock",
    "Merengue",
    "Salsa",
    "Trash Metal",
    "Anime",
    "JPop",
    "Synth Pop"
};

#define MAX_ID3_GENRES  ((int)((sizeof(id3_genres)) / (sizeof(id3_genres[0]))))

id3::id3(app_wiiradio* _theapp) : theapp(_theapp), f(0), hdr_size(0), APIC_Surface(0)
{
    frame_APIC.picture_data = 0;
}

id3::~id3()
{
    clear();
}

void id3::clear()
{
    is_id3 = false;
    title = "";
    artist = "";
    album = "";
    year = "";
    genre = "";
    comments = "";

    if(frame_APIC.picture_data)
    {
        delete [] frame_APIC.picture_data;
        frame_APIC.picture_data = 0;
    }

    if (APIC_Surface)
    {
        SDL_FreeSurface(APIC_Surface);
        APIC_Surface = 0;
    }

}

void id3::FreeAPIC()
{
    if (APIC_Surface)
    {
        SDL_FreeSurface(APIC_Surface);
        APIC_Surface = 0;
    }
}

int inline id3::id3_APIC_image(const int image_size,FILE* art_file)
{
    if(!art_file) return 0;

    int read = 0;

    if(frame_APIC.picture_data)
    {
        delete [] frame_APIC.picture_data;
        frame_APIC.picture_data = 0;
    }

    if (APIC_Surface)
    {
        SDL_FreeSurface(APIC_Surface);
        APIC_Surface = 0;
    }

    frame_APIC.picture_data = new unsigned char[image_size];
    if(!frame_APIC.picture_data)
        return 0;

    // -- finally that data
    int img_read = 0;
    while(img_read < image_size)
    {
        img_read += fread(frame_APIC.picture_data+img_read,1,image_size,art_file);
        read += img_read;
    }

    APIC_rw_ops = SDL_RWFromMem(frame_APIC.picture_data, image_size);
	SDL_Surface* temp = IMG_Load_RW(APIC_rw_ops, 1);
	if(!temp)
        return 0;

    APIC_Surface = SDL_CreateRGBSurface(SDL_SWSURFACE,APIC_SIZE,APIC_SIZE,BITDEPTH,rmask, gmask, bmask,amask);
    if (!APIC_Surface)
    {
        return 0;
    }

    SDL_Rect sr = {0,0,temp->w,temp->h};
    SDL_Rect dr = {0,0,APIC_Surface->w,APIC_Surface->h};

    SDL_SoftStretch(temp,&sr,APIC_Surface,&dr);

    SDL_FreeSurface(temp);

    return read;
}

int id3::newfile(FILE* _f,const unsigned int _filesize)
{
    clear();

    if(!_f) return 0;

    f = _f;
    filesize = _filesize;

    // -- id3_header
    fread(id3_header,1,10,f);

    if (id3_header[0] != 'I' || id3_header[1] != 'D' || id3_header[2] != '3')
        return 0;

    is_id3 = true;


    hdr_size = ((id3_header[6] & 0x7F) << 21) | ((id3_header[7] & 0x7F) << 14) | ((id3_header[8] & 0x7F) << 7) | (id3_header[9] & 0x7F);



    // on to id3 frame

   // "APIC" = picture
    int read = 10;
    unsigned int current_size = 0;
    string frame_name = "";
    bool embed_apic = false;

    title = "";
    artist = "";
    album = "";
    year = "";
    genre = "";
    comments = "";

    while(read < hdr_size)
    {
        read += fread(&frame,1,sizeof(id3frame),f);
        frame_name = frame.frame_id;

        current_size = ((frame.frame_size[0] & 0xff) << 24) | ((frame.frame_size[1] & 0xff) << 16) | ((frame.frame_size[2] & 0xff) << 8) | (frame.frame_size[3] & 0xff);

        read += current_size;

        char description = 0xff;
        char temp[MED_MEM];

        if (frame_name == "TIT2")
        {
            while(current_size--)
            {
                fread(&description,1,1,f);
                if(description > 0x20) title += description;
            }

        }
        else if (frame_name == "TPE1")
        {
            while(current_size--)
            {
                fread(&description,1,1,f);
                if(description > 0x20) artist += description;
            }

        }
        else if (frame_name == "TALB")
        {
            while(current_size--)
            {
                fread(&description,1,1,f);
                if(description > 0x20) album += description;
            }
        }
        else if (frame_name == "TDRC")
        {
            while(current_size--)
            {
                fread(&description,1,1,f);
                if(description > 0x20) year += description;
            }
        }
        else if (frame_name == "TCON")
        {
            while(current_size--)
            {
                fread(&description,1,1,f);
                if(description > 0x20) genre += description;
            }
        }
        else if (frame_name == "COMM")
        {
            while(current_size--)
            {
                fread(&description,1,1,f);
                if(description > 0x20) comments += description;
            }
        }
        else if (frame_name == "APIC")
        {
            embed_apic = true;
            read += fread(&frame_APIC.text_encoding,1,1,f);

            char mime_type = 0xff;
            while(mime_type)
            {
                read += fread(&mime_type,1,1,f);
                frame_APIC.MIME_type += mime_type;
            }

            read += fread(&frame_APIC.picture_type,1,1,f);


            while(description)
            {
                read += fread(&description,1,1,f);
                frame_APIC.description += description;
            }

            unsigned int image_size = current_size;//- (read - apic_start);

            if (!(read += id3_APIC_image(image_size,f)))
                break; // -- out of mem ... lets skip the image

            break;
        }

        fseek ( f , read , SEEK_SET );
    }

    // No APIC embed found look for art in folder
    if(!embed_apic)
    {
        string dir = theapp->playing.server;
        dir += "albumartsmall.jpg";

        FILE* aa = fopen(dir.c_str(),"rb");

        if(!aa)
        {   // another option
            dir = theapp->playing.server;
            dir += "Folder.jpg";
            aa = fopen(dir.c_str(),"rb");
        }

        if(aa)
        {
            fseek(aa,0,SEEK_END);
            unsigned int size = ftell(aa);
            fseek(aa,0,SEEK_SET);

            id3_APIC_image(size,aa);

            fclose(aa);
        }

    }


    // Older TAG format
    fseek ( f , filesize - 128 , SEEK_SET );

    char buffer[MAX_TAG_SIZE + 1];

    fread(buffer,1,3,f);

    // OLD Format ID3
    if (buffer[0] == 'T' && buffer[1] == 'A' && buffer[2] == 'G')
    {
        // -- Title
        fread(buffer,1,MAX_TAG_SIZE,f);
        buffer[MAX_TAG_SIZE] = 0;
        if(title == "") title = buffer;

        // -- Artist
        fread(buffer,1,MAX_TAG_SIZE,f);
        buffer[MAX_TAG_SIZE] = 0;
        if(artist == "") artist = buffer;

        // -- Album
        fread(buffer,1,MAX_TAG_SIZE,f);
        buffer[MAX_TAG_SIZE] = 0;
        if(album == "") album = buffer;

        // -- Year
        fread(buffer,1,4,f);
        buffer[4] = 0;
        if(year == "") year = buffer;


         // -- comments
        fread(buffer,1,MAX_TAG_SIZE,f);
        buffer[MAX_TAG_SIZE] = 0;
        if(comments == "") comments = buffer;

        // -- Genre
        if(genre == "")
        {
            fread(buffer,1,1,f);
            buffer[1] = 0;
            int genre_num = buffer[0];

            if((genre_num < 0) || (genre_num > (MAX_ID3_GENRES-1)))
            {
                genre = "Unknown";
            }else{
                genre = id3_genres[genre_num];
            }
        }
        stdstring_trim(title);
        stdstring_trim(artist);
        stdstring_trim(album);
        stdstring_trim(year);
        stdstring_trim(genre);
        stdstring_trim(comments);


    }

    return hdr_size; // bypass hdr

}
