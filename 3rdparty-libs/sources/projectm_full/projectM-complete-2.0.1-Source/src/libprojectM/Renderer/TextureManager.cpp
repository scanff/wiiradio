#ifdef LINUX
#include <GL/gl.h>
#endif
#ifdef WIN32
#include "win/glew.h"
#endif
#ifdef __APPLE__
#include <GL/gl.h>
#endif

#ifdef USE_DEVIL
#include <IL/ilut.h>
#else
#include "SOIL/SOIL.h"
#endif

#ifdef WIN32
#include <dirent.h>
#include <stdlib.h>
#endif
#include "SOIL/SOIL.h"
#ifdef LINUX
#include <dirent.h>
#endif

#ifdef MACOS
#include <dirent.h>
#endif

#ifdef _WII_
#include <dirent.h>
#include <stdlib.h>
#endif

#include "TextureManager.hpp"
#include "Common.hpp"
#include "IdleTextures.hpp"

#ifdef _WII_
#include "stb_image_aug.h"
#include "global_gx.h"
#endif


TextureManager::TextureManager(const std::string _presetURL): presetURL(_presetURL)
{
#ifdef USE_DEVIL
ilInit();
iluInit();
ilutInit();
ilutRenderer(ILUT_OPENGL);
#endif

 Preload();
 loadTextureDir();
}

TextureManager::~TextureManager()
{
 Clear();
}

void TextureManager::Preload()
{

#ifdef USE_DEVIL
	ILuint image;
	ilGenImages(1, &image);
	ilBindImage(image);
	ilLoadL(IL_TYPE_UNKNOWN,(ILvoid*) M_data, M_bytes);
	GLuint tex = ilutGLBindTexImage();
#else
#ifdef _WII_
return; // TODO
    unsigned int tex = 1;
	int x,y;
    int comps = 0;
    unsigned char* nb = 0;
    nb = stbi_load_from_memory(M_data,M_bytes,&x,&y,&comps,0);
    if (nb)
    {
        WII_CreateTexture(tex,(void*)M_data,x,y);
    }
    textures["M.tga"]=tex;

    tex++;
    nb = stbi_load_from_memory(project_data,project_bytes,&x,&y,&comps,0);
    if (nb)
    {
        WII_CreateTexture(tex,(void*)project_data,x,y);
    }
    textures["project.tga"]=tex;

    tex++;

    nb = stbi_load_from_memory(headphones_data,headphones_bytes,&x,&y,&comps,0);
    if (nb)
    {
        WII_CreateTexture(tex,(void*)headphones_data,x,y);
    }
    textures["headphones.tga"]=tex;

    return;

#else

	 unsigned int tex = SOIL_load_OGL_texture_from_memory(
					  M_data,
					  M_bytes,
					  SOIL_LOAD_AUTO,
					  SOIL_CREATE_NEW_ID,

					    SOIL_FLAG_POWER_OF_TWO
					  |  SOIL_FLAG_MULTIPLY_ALPHA
					 // |  SOIL_FLAG_COMPRESS_TO_DXT
					  );
#endif
#endif

  textures["M.tga"]=tex;

#ifdef USE_DEVIL
  ilLoadL(IL_TYPE_UNKNOWN,(ILvoid*) project_data,project_bytes);
  tex = ilutGLBindTexImage();
#else
#ifndef _WII_
  tex = SOIL_load_OGL_texture_from_memory(
					  project_data,
					  project_bytes,
					  SOIL_LOAD_AUTO,
					  SOIL_CREATE_NEW_ID,

					  SOIL_FLAG_POWER_OF_TWO
					  |  SOIL_FLAG_MULTIPLY_ALPHA
					  //|  SOIL_FLAG_COMPRESS_TO_DXT
					  );
#endif
#endif

  textures["project.tga"]=tex;

#ifdef USE_DEVIL
  ilLoadL(IL_TYPE_UNKNOWN,(ILvoid*) headphones_data, headphones_bytes);
  tex = ilutGLBindTexImage();
#else
#ifndef _WII_
  tex = SOIL_load_OGL_texture_from_memory(
					  headphones_data,
					  headphones_bytes,
					  SOIL_LOAD_AUTO,
					  SOIL_CREATE_NEW_ID,

					  SOIL_FLAG_POWER_OF_TWO
					  |  SOIL_FLAG_MULTIPLY_ALPHA
					 // |  SOIL_FLAG_COMPRESS_TO_DXT
					  );
#endif
#endif

  textures["headphones.tga"]=tex;
}

void TextureManager::Clear()
{

#ifdef _WII_

#else
  for(std::map<std::string, GLuint>::const_iterator iter = textures.begin(); iter != textures.end(); iter++)
    {
      glDeleteTextures(1,&iter->second);
    }
#endif
  textures.clear();
}

void TextureManager::setTexture(const std::string name, const unsigned int texId, const int width, const int height)
{
		textures[name] = texId;
		widths[name] = width;
		heights[name] = height;
}

//void TextureManager::unloadTextures(const PresetOutputs::cshape_container &shapes)
//{
  /*
   for (PresetOutputs::cshape_container::const_iterator pos = shapes.begin();
	pos != shapes.end(); ++pos)
    {

      if( (*pos)->enabled==1)
	{

	  if ( (*pos)->textured)
	    {
	      std::string imageUrl = (*pos)->getImageUrl();
	      if (imageUrl != "")
		{
		  std::string fullUrl = presetURL + "/" + imageUrl;
		  ReleaseTexture(LoadTexture(fullUrl.c_str()));
		}
	    }
	}
    }
  */
//}

GLuint TextureManager::getTexture(const std::string filename)
{
	std::string fullURL = presetURL + PATH_SEPARATOR + filename;
	return getTextureFullpath(filename,fullURL);
}

GLuint TextureManager::getTextureFullpath(const std::string filename, const std::string imageURL)
{

   if (textures.find(filename)!= textures.end())
     {
       return textures[filename];
     }
   else
     {

#ifdef USE_DEVIL
       GLuint tex = ilutGLLoadImage((char *)imageURL.c_str());
#else
#ifndef _WII_
       int width, height;

       unsigned int tex = SOIL_load_OGL_texture_size(
    		   imageURL.c_str(),
					  SOIL_LOAD_AUTO,
					  SOIL_CREATE_NEW_ID,

					    //SOIL_FLAG_POWER_OF_TWO
					  //  SOIL_FLAG_MIPMAPS
					    SOIL_FLAG_MULTIPLY_ALPHA
					  //|  SOIL_FLAG_COMPRESS_TO_DXT
					  //| SOIL_FLAG_DDS_LOAD_DIRECT
					  ,&width,&height);
#endif
#endif

#ifndef _WII_
       textures[filename]=tex;
       widths[filename]=width;
       heights[filename]=height;
       return tex;
#else
		return 0;
#endif


     }
}

int TextureManager::getTextureWidth(const std::string imageURL)
{
	return widths[imageURL];
}

int TextureManager::getTextureHeight(const std::string imageURL)
{
	return heights[imageURL];
}

unsigned int TextureManager::getTextureMemorySize()
{
  return 0;
}

void TextureManager::loadTextureDir()
{ // TODO
	std::string dirname = "";//CMAKE_INSTALL_PREFIX "/share/projectM/textures";

	  DIR * m_dir;

	 // Allocate a new a stream given the current directory name
	  if ((m_dir = opendir(dirname.c_str())) == NULL)
	  {
//	    std::cout<<"No Textures Loaded from "<<dirname<<std::endl;
	    return; // no files loaded. m_entries is empty
	  }

	  struct dirent * dir_entry;

	  while ((dir_entry = readdir(m_dir)) != NULL)
	  {

	    // Convert char * to friendly string
	    std::string filename(dir_entry->d_name);

	    if (filename.length() > 0 && filename[0] == '.')
		continue;

	    // Create full path name
	    std::string fullname = dirname + PATH_SEPARATOR + filename;

	    unsigned int texId = getTextureFullpath(filename, fullname);
	    if(texId != 0)
	    {
	    	user_textures.push_back(texId);
	    	textures[filename]=texId;
	    	user_texture_names.push_back(filename);
	    }
	  }

	  if (m_dir)
	    {
	      closedir(m_dir);
	      m_dir = 0;
	    }

}

std::string TextureManager::getRandomTextureName(std::string random_id)
{
	if (user_texture_names.size() > 0)
	{
		std::string random_name = user_texture_names[rand() % user_texture_names.size()];
		random_textures.push_back(random_id);
		textures[random_id] = textures[random_name];
		return random_name;
	}
	else return "";
}

void TextureManager::clearRandomTextures()
{
	for (std::vector<std::string>::iterator pos = random_textures.begin(); pos	!= random_textures.end(); ++pos)
				{
					textures.erase(*pos);
					widths.erase(*pos);
					heights.erase(*pos);
				}
	random_textures.clear();

}
