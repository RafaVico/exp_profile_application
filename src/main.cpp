/*********************************************************************
                          main.cpp  -  Exp
                             -------------------
    inicio              : lun jul  26 23:30 CEST 2010
    copyright           : (C) 2010 por Rafa Vico Costa
    email               : saboteur2@hotmail.com
*********************************************************************/

#include <fstream>
#include <vector>
#include <string>
#include <math.h>
#include <sys/stat.h>
//#include <sys/ioctl.h>
#include <fcntl.h>
#include <dirent.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include "../inc/structures.h"
#include "../inc/language.h"
#include "../inc/game_experience.h"
#include "../inc/exp.h"
//#include "../../exp_sdk/exp.h"
//#include "../exp_core.h"
//#include "../exp_sdl.h"
#ifdef PLATFORM_WIN
#include <shlobj.h>
#endif

//#define O_RDONLY           0
/*#define BATT_LEVEL_HIGH 0
#define BATT_LEVEL_MID 1
#define BATT_LEVEL_LOW 2
#define BATT_LEVEL_EMPTY 3
#define IOCTL_GP2X_MMSP2ADC_CONTROL    _IOR('v', 0x00 ,unsigned long)
#define BATT_MODE		0
#define REMOCON_MODE		1
#define BATT_REMOCON_MODE	2

struct MMSP2ADC
{
  unsigned short batt;
  unsigned short remocon;
};*/

#ifdef PLATFORM_GP2X
std::string exp_root="/mnt/sd/exp/";
#endif
#ifdef PLATFORM_WIN
std::string exp_root="c:/exp/";
#endif

///////////////////////////////////
/*  Variables globales           */
///////////////////////////////////
int done=FALSE;                     // con done=1 salimos del programa
int FPS=0;
int program_mode=MODE_SELECT_EXP;
int active_exp=0;
int selected_attribute=0;
int select_bar_posx=24;
int select_bar_accel=1;
int show_keyboard=FALSE;
int show_keyboard_accel=1;
int key_caps=FALSE;
int key_lock=FALSE;
int last_key_line=0;
int last_key_file=0;
int last_key_time=0;
int key_line_cursor=1;
int key_file_cursor=1;
int exp_line_cursor=1;
int exp_file_cursor=1;

// variables de pantalla
SDL_Surface *screen;   		    // superficie de la pantalla
#ifdef PLATFORM_WIN
SDL_Surface *screen2;   		    // superficie de la pantalla, sólo para WINDOWS
#endif
int screen_width=320;
int screen_height=240;
int screen_deep=2;            // bytes de profundidad
#ifdef PLATFORM_GP2X
int screen_multiplier=1;
#endif
#ifdef PLATFORM_WIN
int screen_multiplier=2;
#endif
int fullscreen=0;
int scanlines=0;

// variables gráficas
SDL_Surface *background;
SDL_Surface *card;
SDL_Surface *button_pad;
SDL_Surface *button_pad2;
SDL_Surface *button_y;
SDL_Surface *button_x;
SDL_Surface *button_b;
SDL_Surface *button_l;
SDL_Surface *button_r;
SDL_Surface *select_bar;
SDL_Surface *deletekey;
SDL_Surface *returnkey;
SDL_Surface *capskey;
SDL_Surface *lockkey;
SDL_Surface *selectkey;
SDL_Surface *expery;
SDL_Surface *experyw;
SDL_Surface *star;
std::vector<SDL_Surface*> exp_list_base;
std::vector<SDL_Surface*> exp_list_eyes;
std::vector<SDL_Surface*> exp_list_browns;
std::vector<SDL_Surface*> exp_list_noses;
std::vector<SDL_Surface*> exp_list_ears;
std::vector<SDL_Surface*> exp_list_mouths;
std::vector<SDL_Surface*> exp_list_facial;
std::vector<SDL_Surface*> exp_list_hairs;
std::vector<SDL_Surface*> exp_list_tatoos;
std::vector<SDL_Surface*> exp_list_clothes;

Mix_Chunk *sound_next;

TTF_Font *font;                 // fuente usada
TTF_Font *font2;                // fuente usada
SDL_Joystick *joystick;         // joystick usado
Uint8 *keys=SDL_GetKeyState(NULL);

char version[30];
language lang;

int volume=120;
int max_volume=240;

//listas
std::vector<std::string> list_base;
std::vector<std::string> list_eyes;
std::vector<std::string> list_browns;
std::vector<std::string> list_noses;
std::vector<std::string> list_ears;
std::vector<std::string> list_mouths;
std::vector<std::string> list_facial;
std::vector<std::string> list_hairs;
std::vector<std::string> list_tatoos;
std::vector<std::string> list_clothes;

// exp=perfiles de jugador
std::vector<struct exp*> exp_list;
struct exp present_exp;
std::vector<std::string> list_games;
int active_game=0;

// datos de juegos
std::string title_game1;
std::string author_game1;
game_summary exp_game1;
std::vector<experience> exp_experience1;
std::vector<experience_lang> exp_desc1;
std::vector<game_value> list_favorites;
std::vector<game_value> list_completes;
SDL_Surface* exp_favorites[5];
SDL_Surface* exp_completes[5];
SDL_Surface* exp_icons1[50];

// mensajes de aviso
int show_message=0; // numero del mensaje a mostrar
int done_mess1=FALSE;
int done_mess2=FALSE;

// datos de menu
int menu_show=0;
int menu_sel=0;
int menu_max=3;
int make_shot=0;

// declaracionde funciones
void save_active_exp();
void save_exp();
void save_game_ranking();
void load_game_ranking();
void check_menu();

#ifdef PLATFORM_WIN
void get_root_path()
{
  LPITEMIDLIST pidl;
  HRESULT hr = SHGetSpecialFolderLocation(NULL, 35, &pidl);
  char szPath[_MAX_PATH];
  BOOL f = SHGetPathFromIDList(pidl, szPath);
  LPMALLOC pMalloc;
  hr = SHGetMalloc(&pMalloc);
  pMalloc->Free(pidl);
  pMalloc->Release();
  exp_root=std::string(szPath)+"/exp/";
}
#endif

///////////////////////////////////
/*  Dibuja el texto              */
///////////////////////////////////
void draw_text(SDL_Surface* dst, char* string, int x, int y, int fR, int fG, int fB, TTF_Font *f=font)
{
  if(string)
  {
    SDL_Color foregroundColor={fR,fG,fB};
    SDL_Surface *textSurface=TTF_RenderText_Blended(f,string,foregroundColor);
    //SDL_Surface *textSurface=TTF_RenderUTF8_Blended(f,string,foregroundColor);
    SDL_Rect textLocation={x,y,0,0};
    SDL_BlitSurface(textSurface,NULL,dst,&textLocation);
    SDL_FreeSurface(textSurface);
  }
}

void split_string(char* s, std::string &s1,std::string &s2)
{
  if(s)
  {
    std::string str(s);
    if(str.size()>=49)
    {
      for(unsigned int f=48; f>0; f--)
      {
        if(str.substr(f,1)==" ")
        {
          s1=str.substr(0,f);
          s2=str.substr(f+1);
          break;
        }
      }
    }
    else
      s1=s;
  }
}

void draw_text2(SDL_Surface* dst, char* string, int x, int y, int fR, int fG, int fB, TTF_Font *f=font)
{
  if(string)
  {
    std::string str1;
    std::string str2;
    split_string(string,str1,str2);
    SDL_Color foregroundColor={fR,fG,fB};

    SDL_Surface *textSurface=TTF_RenderText_Blended(f,str1.c_str(),foregroundColor);
    SDL_Rect textLocation={x,y,0,0};
    SDL_BlitSurface(textSurface,NULL,dst,&textLocation);
    SDL_FreeSurface(textSurface);

    textSurface=TTF_RenderText_Blended(f,str2.c_str(),foregroundColor);
    SDL_Rect textLocation2={x,y+10,0,0};
    SDL_BlitSurface(textSurface,NULL,dst,&textLocation2);
    SDL_FreeSurface(textSurface);
  }
}

int need_update()
{
  if(exp_list.size()>0)
  {
    std::string fname=exp_root+exp_list[active_exp]->directory+"/update";
    struct stat st;
    if(stat(fname.c_str(),&st) == 0)
      return 1;
  }
  return 0;
}

///////////////////////////////////
/* Convierte un número en cadena */
///////////////////////////////////
std::string tostring(int n)
{
  char num[10];
  if(n>=0 && n<10)
    sprintf(num,"0%i",n);
  else
    sprintf(num,"%i",n);
  return std::string(num);
}

std::string remove_spaces(std::string str)
{
  std::string s;
  for(int f=0; f<str.size(); f++)
  {
    if(str.substr(f,1)>" ")
      s=s+str.substr(f,1);
  }

  return s;
}

std::string uptolow(std::string str)
{
  for (int i=0;i<strlen(str.c_str());i++)
    if (str[i] >= 0x41 && str[i] <= 0x5A)
      str[i] = str[i] + 0x20;
  return str;
}

std::string todirectory(std::string name)
{
  //std::string n="exp_";
  name=uptolow(name);
  name=remove_spaces(name);
  name="exp_"+name;
  /*for(int f=0; f<name.size(); f++)
  {
    if(name.substr(f,1)!=" ")
      n=n+name.substr(f,1);
  }*/

  return name;
}

///////////////////////////////////
/*  Devuelve un pixel de una     */
/*  superficie.                  */
///////////////////////////////////
Uint32 get_pixel(SDL_Surface* src, Uint32 x, Uint32 y)
{
  Uint32	color = 0;
	Uint8	*ubuff8;
	Uint16	*ubuff16;
	Uint32	*ubuff32;

	color = 0;

	switch(src->format->BytesPerPixel)
	{
		case 1:
			ubuff8 = (Uint8*) src->pixels;
			ubuff8 += (y * src->pitch) + x;
			color = *ubuff8;
			break;

		case 2:
			ubuff8 = (Uint8*) src->pixels;
			ubuff8 += (y * src->pitch) + (x*2);
			ubuff16 = (Uint16*) ubuff8;
			color = *ubuff16;
			break;

		case 3:
			ubuff8 = (Uint8*) src->pixels;
			ubuff8 += (y * src->pitch) + (x*3);
			color = 0;
			#if SDL_BYTEORDER == SDL_LIL_ENDIAN
				color |= ubuff8[2] << 16;
				color |= ubuff8[1] << 8;
				color |= ubuff8[0];
			#else
				color |= ubuff8[0] << 16;
				color |= ubuff8[1] << 8;
				color |= ubuff8[2];
			#endif
			break;

		case 4:
			ubuff8 = (Uint8*) src->pixels;
			ubuff8 += (y*src->pitch) + (x*4);
			ubuff32 = (Uint32*)ubuff8;
			color = *ubuff32;
			break;

		default:
			break;
	}
	return color;
}

///////////////////////////////////
/*  Dibuja un pixel en pantalla  */
///////////////////////////////////
void set_pixel(SDL_Surface* src, int x, int y, SDL_Color& color)
{
	Uint8	*ubuff8;
	Uint16	*ubuff16;
	Uint32	*ubuff32;
  Uint32 c=SDL_MapRGB(src->format, color.r, color.g, color.b);

	switch(src->format->BytesPerPixel)
	{
		case 1:
			ubuff8 = (Uint8*) src->pixels;
			ubuff8 += (y * src->pitch) + x;
			*ubuff8 = (Uint8) c;
			break;

		case 2:
			ubuff8 = (Uint8*) src->pixels;
			ubuff8 += (y * src->pitch) + (x*2);
			ubuff16 = (Uint16*) ubuff8;
			*ubuff16 = (Uint16) c;
			break;

		case 3:
			ubuff8 = (Uint8*) src->pixels;
			ubuff8 += (y * src->pitch) + (x*3);
			#if SDL_BYTEORDER == SDL_LIL_ENDIAN
			ubuff8[0] = (Uint8) color.b;
			ubuff8[1] = (Uint8) color.g;
			ubuff8[2] = (Uint8) color.r;
			#else
			ubuff8[0] = (Uint8) color.r;
			ubuff8[1] = (Uint8) color.g;
			ubuff8[2] = (Uint8) color.b;
			#endif
			break;

		case 4:
			ubuff8 = (Uint8*) src->pixels;
			ubuff8 += (y*src->pitch) + (x*4);
			ubuff32 = (Uint32*)ubuff8;
			*ubuff32=(Uint32)c;
			break;

		default:
			break;
	}
}

///////////////////////////////////
/*  Dibuja una caja de un color  */
///////////////////////////////////
void draw_box(SDL_Rect *dst, Uint32 color)
{
  if(dst->w>5 && dst->h>5)
  {
    SDL_Rect dest;
    dest.x=dst->x+1;
    dest.y=dst->y+1;
    dest.w=dst->w-2;
    dest.h=dst->h-2;
    SDL_FillRect(screen,&dest,color);
    dest.x=dst->x+2;
    dest.y=dst->y;
    dest.w=dst->w-4;
    dest.h=1;
    SDL_FillRect(screen,&dest,color);
    dest.y=dst->y+dst->h-1;
    SDL_FillRect(screen,&dest,color);
    dest.x=dst->x;
    dest.y=dst->y+2;
    dest.w=1;
    dest.h=dst->h-4;
    SDL_FillRect(screen,&dest,color);
    dest.x=dst->x+dst->w-1;
    SDL_FillRect(screen,&dest,color);
  }
}

///////////////////////////////////
/*  Dibuja una caja de un color  */
///////////////////////////////////
void draw_box2(SDL_Rect *dst, Uint32 color)
{
  if(dst->w>3 && dst->h>3)
  {
    SDL_Rect dest;
    dest.x=dst->x+1;
    dest.y=dst->y;
    dest.w=dst->w-2;
    dest.h=dst->h;
    SDL_FillRect(screen,&dest,color);
    dest.x=dst->x;
    dest.y=dst->y+1;
    dest.w=1;
    dest.h=dst->h-2;
    SDL_FillRect(screen,&dest,color);
    dest.x=dst->x+dst->w-1;
    SDL_FillRect(screen,&dest,color);
  }
}

void draw_boxborder(SDL_Rect *dst, Uint32 color, Uint32 border)
{
  SDL_Rect d=*dst;
  draw_box(&d,border);
  d.x++;
  d.y++;
  d.w-=2;
  d.h-=2;
  draw_box(&d,color);
}

///////////////////////////////////
/*  Copia una superficie con     */
/*  el multiplicador             */
///////////////////////////////////
void filter_surface(SDL_Surface *src, SDL_Surface *dst)
{
  int climit=40;
  SDL_LockSurface(dst);
  SDL_LockSurface(src);
  for(int g=0; g<240; g++)
  {
    for(int f=0; f<320; f++)
    {
      Uint32 colour=get_pixel(src,f,g);
      SDL_Color c,c2;
      SDL_GetRGB(colour, src->format, &c.r, &c.g, &c.b);
      if(scanlines)
      {
        if(c.r>climit)
          c2.r=c.r-climit;
        else
          c2.r=0;
        if(c.g>climit)
          c2.g=c.g-climit;
        else
          c2.g=0;
        if(c.b>climit)
          c2.b=c.b-climit;
        else
          c2.b=0;
      }
      else
        c2=c;
      set_pixel(dst,f*screen_multiplier,g*screen_multiplier,c);
      set_pixel(dst,f*screen_multiplier+1,g*screen_multiplier,c);
      set_pixel(dst,f*screen_multiplier,g*screen_multiplier+1,c2);
      set_pixel(dst,f*screen_multiplier+1,g*screen_multiplier+1,c2);
    }
  }
  SDL_UnlockSurface(dst);
  SDL_UnlockSurface(src);
}

///////////////////////////////////
/*  Carga los gráficos del juego */
///////////////////////////////////
void load_graphics()
{
  SDL_Rect rect;
  SDL_Surface *tmpsurface;
  SDL_Surface *tmpsurface2;

  tmpsurface=SDL_LoadBMP("media/background.bmp");
  if(tmpsurface)
  {
    rect.x=0;
    rect.y=0;
    rect.w=320;
    rect.h=240;
    background=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
    SDL_BlitSurface(tmpsurface,&rect,background,NULL);
    SDL_FreeSurface(tmpsurface);
  }

  tmpsurface=SDL_LoadBMP("media/card.bmp");
  if(tmpsurface)
  {
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    card=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
    SDL_BlitSurface(tmpsurface,&rect,card,NULL);
    SDL_SetColorKey(card,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }

  tmpsurface=SDL_LoadBMP("media/button_pad.bmp");
  if(tmpsurface)
  {
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    button_pad=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
    SDL_BlitSurface(tmpsurface,&rect,button_pad,NULL);
    SDL_SetColorKey(button_pad,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }

  tmpsurface=SDL_LoadBMP("media/button_pad2.bmp");
  if(tmpsurface)
  {
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    button_pad2=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
    SDL_BlitSurface(tmpsurface,&rect,button_pad2,NULL);
    SDL_SetColorKey(button_pad2,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }

  tmpsurface=SDL_LoadBMP("media/button_y.bmp");
  if(tmpsurface)
  {
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    button_y=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
    SDL_BlitSurface(tmpsurface,&rect,button_y,NULL);
    SDL_SetColorKey(button_y,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }

  tmpsurface=SDL_LoadBMP("media/button_x.bmp");
  if(tmpsurface)
  {
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    button_x=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
    SDL_BlitSurface(tmpsurface,&rect,button_x,NULL);
    SDL_SetColorKey(button_x,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }

  tmpsurface=SDL_LoadBMP("media/button_b.bmp");
  if(tmpsurface)
  {
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    button_b=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
    SDL_BlitSurface(tmpsurface,&rect,button_b,NULL);
    SDL_SetColorKey(button_b,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }

  tmpsurface=SDL_LoadBMP("media/button_l.bmp");
  if(tmpsurface)
  {
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    button_l=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
    SDL_BlitSurface(tmpsurface,&rect,button_l,NULL);
    SDL_SetColorKey(button_l,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }

  tmpsurface=SDL_LoadBMP("media/button_r.bmp");
  if(tmpsurface)
  {
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    button_r=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
    SDL_BlitSurface(tmpsurface,&rect,button_r,NULL);
    SDL_SetColorKey(button_r,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }

  tmpsurface=SDL_LoadBMP("media/select_bar.bmp");
  if(tmpsurface)
  {
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    select_bar=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
    SDL_BlitSurface(tmpsurface,&rect,select_bar,NULL);
    SDL_SetColorKey(select_bar,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }

  tmpsurface=SDL_LoadBMP("media/selectkey.bmp");
  if(tmpsurface)
  {
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    selectkey=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
    SDL_BlitSurface(tmpsurface,&rect,selectkey,NULL);
    SDL_SetColorKey(selectkey,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }

  tmpsurface=SDL_LoadBMP("media/deletekey.bmp");
  if(tmpsurface)
  {
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    deletekey=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
    SDL_BlitSurface(tmpsurface,&rect,deletekey,NULL);
    SDL_SetColorKey(deletekey,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }

  tmpsurface=SDL_LoadBMP("media/returnkey.bmp");
  if(tmpsurface)
  {
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    returnkey=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
    SDL_BlitSurface(tmpsurface,&rect,returnkey,NULL);
    SDL_SetColorKey(returnkey,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }

  tmpsurface=SDL_LoadBMP("media/capskey.bmp");
  if(tmpsurface)
  {
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    capskey=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
    SDL_BlitSurface(tmpsurface,&rect,capskey,NULL);
    SDL_SetColorKey(capskey,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }

  tmpsurface=SDL_LoadBMP("media/lockkey.bmp");
  if(tmpsurface)
  {
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    lockkey=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
    SDL_BlitSurface(tmpsurface,&rect,lockkey,NULL);
    SDL_SetColorKey(lockkey,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }

  tmpsurface=SDL_LoadBMP("media/experience.bmp");
  if(tmpsurface)
  {
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    expery=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
    SDL_BlitSurface(tmpsurface,&rect,expery,NULL);
    SDL_SetColorKey(expery,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }

  tmpsurface=SDL_LoadBMP("media/experiencew.bmp");
  if(tmpsurface)
  {
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    experyw=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
    SDL_BlitSurface(tmpsurface,&rect,experyw,NULL);
    SDL_SetColorKey(experyw,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }

  tmpsurface=SDL_LoadBMP("media/star.bmp");
  if(tmpsurface)
  {
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    star=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
    SDL_BlitSurface(tmpsurface,&rect,star,NULL);
    SDL_SetColorKey(star,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }

  exp_list_base.clear();
  for(unsigned int f=0; f<list_base.size(); f++)
  {
    tmpsurface=SDL_LoadBMP(list_base[f].c_str());
    if(tmpsurface)
    {
      rect.x=0;
      rect.y=0;
      rect.w=tmpsurface->w;
      rect.h=tmpsurface->h;
      tmpsurface2=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
      SDL_BlitSurface(tmpsurface,&rect,tmpsurface2,NULL);
      SDL_SetColorKey(tmpsurface2,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
      SDL_FreeSurface(tmpsurface);
      exp_list_base.push_back(tmpsurface2);
    }
    else
      exp_list_base.push_back(NULL);
  }

  exp_list_eyes.clear();
  for(unsigned int f=0; f<list_eyes.size(); f++)
  {
    tmpsurface=SDL_LoadBMP(list_eyes[f].c_str());
    if(tmpsurface)
    {
      rect.x=0;
      rect.y=0;
      rect.w=tmpsurface->w;
      rect.h=tmpsurface->h;
      tmpsurface2=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
      SDL_BlitSurface(tmpsurface,&rect,tmpsurface2,NULL);
      SDL_SetColorKey(tmpsurface2,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
      SDL_FreeSurface(tmpsurface);
      exp_list_eyes.push_back(tmpsurface2);
    }
    else
      exp_list_eyes.push_back(NULL);
  }

  exp_list_browns.clear();
  for(unsigned int f=0; f<list_browns.size(); f++)
  {
    tmpsurface=SDL_LoadBMP(list_browns[f].c_str());
    if(tmpsurface)
    {
      rect.x=0;
      rect.y=0;
      rect.w=tmpsurface->w;
      rect.h=tmpsurface->h;
      tmpsurface2=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
      SDL_BlitSurface(tmpsurface,&rect,tmpsurface2,NULL);
      SDL_SetColorKey(tmpsurface2,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
      SDL_FreeSurface(tmpsurface);
      exp_list_browns.push_back(tmpsurface2);
    }
    else
      exp_list_browns.push_back(NULL);
  }

  exp_list_noses.clear();
  for(unsigned int f=0; f<list_noses.size(); f++)
  {
    tmpsurface=SDL_LoadBMP(list_noses[f].c_str());
    if(tmpsurface)
    {
      rect.x=0;
      rect.y=0;
      rect.w=tmpsurface->w;
      rect.h=tmpsurface->h;
      tmpsurface2=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
      SDL_BlitSurface(tmpsurface,&rect,tmpsurface2,NULL);
      SDL_SetColorKey(tmpsurface2,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
      SDL_FreeSurface(tmpsurface);
      exp_list_noses.push_back(tmpsurface2);
    }
    else
      exp_list_noses.push_back(NULL);
  }

  exp_list_ears.clear();
  for(unsigned int f=0; f<list_ears.size(); f++)
  {
    tmpsurface=SDL_LoadBMP(list_ears[f].c_str());
    if(tmpsurface)
    {
      rect.x=0;
      rect.y=0;
      rect.w=tmpsurface->w;
      rect.h=tmpsurface->h;
      tmpsurface2=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
      SDL_BlitSurface(tmpsurface,&rect,tmpsurface2,NULL);
      SDL_SetColorKey(tmpsurface2,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
      SDL_FreeSurface(tmpsurface);
      exp_list_ears.push_back(tmpsurface2);
    }
    else
      exp_list_ears.push_back(NULL);
  }

  exp_list_mouths.clear();
  for(unsigned int f=0; f<list_mouths.size(); f++)
  {
    tmpsurface=SDL_LoadBMP(list_mouths[f].c_str());
    if(tmpsurface)
    {
      rect.x=0;
      rect.y=0;
      rect.w=tmpsurface->w;
      rect.h=tmpsurface->h;
      tmpsurface2=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
      SDL_BlitSurface(tmpsurface,&rect,tmpsurface2,NULL);
      SDL_SetColorKey(tmpsurface2,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
      SDL_FreeSurface(tmpsurface);
      exp_list_mouths.push_back(tmpsurface2);
    }
    else
      exp_list_mouths.push_back(NULL);
  }

  exp_list_facial.clear();
  for(unsigned int f=0; f<list_facial.size(); f++)
  {
    tmpsurface=SDL_LoadBMP(list_facial[f].c_str());
    if(tmpsurface)
    {
      rect.x=0;
      rect.y=0;
      rect.w=tmpsurface->w;
      rect.h=tmpsurface->h;
      tmpsurface2=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
      SDL_BlitSurface(tmpsurface,&rect,tmpsurface2,NULL);
      SDL_SetColorKey(tmpsurface2,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
      SDL_FreeSurface(tmpsurface);
      exp_list_facial.push_back(tmpsurface2);
    }
    else
      exp_list_facial.push_back(NULL);
  }

  exp_list_hairs.clear();
  for(unsigned int f=0; f<list_hairs.size(); f++)
  {
    tmpsurface=SDL_LoadBMP(list_hairs[f].c_str());
    if(tmpsurface)
    {
      rect.x=0;
      rect.y=0;
      rect.w=tmpsurface->w;
      rect.h=tmpsurface->h;
      tmpsurface2=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
      SDL_BlitSurface(tmpsurface,&rect,tmpsurface2,NULL);
      SDL_SetColorKey(tmpsurface2,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
      SDL_FreeSurface(tmpsurface);
      exp_list_hairs.push_back(tmpsurface2);
    }
    else
      exp_list_hairs.push_back(NULL);
  }

  exp_list_tatoos.clear();
  for(unsigned int f=0; f<list_tatoos.size(); f++)
  {
    tmpsurface=SDL_LoadBMP(list_tatoos[f].c_str());
    if(tmpsurface)
    {
      rect.x=0;
      rect.y=0;
      rect.w=tmpsurface->w;
      rect.h=tmpsurface->h;
      tmpsurface2=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
      SDL_BlitSurface(tmpsurface,&rect,tmpsurface2,NULL);
      SDL_SetColorKey(tmpsurface2,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
      SDL_FreeSurface(tmpsurface);
      exp_list_tatoos.push_back(tmpsurface2);
    }
    else
      exp_list_tatoos.push_back(NULL);
  }

  exp_list_clothes.clear();
  for(unsigned int f=0; f<list_clothes.size(); f++)
  {
    tmpsurface=SDL_LoadBMP(list_clothes[f].c_str());
    if(tmpsurface)
    {
      rect.x=0;
      rect.y=0;
      rect.w=tmpsurface->w;
      rect.h=tmpsurface->h;
      tmpsurface2=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
      SDL_BlitSurface(tmpsurface,&rect,tmpsurface2,NULL);
      SDL_SetColorKey(tmpsurface2,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
      SDL_FreeSurface(tmpsurface);
      exp_list_clothes.push_back(tmpsurface2);
    }
    else
      exp_list_clothes.push_back(NULL);
  }
}

///////////////////////////////////
/*  Libera todos los graficos    */
///////////////////////////////////
void release_graphics()
{
  if (background)
    SDL_FreeSurface(background);
  if (card)
    SDL_FreeSurface(card);
  if (button_pad)
    SDL_FreeSurface(button_pad);
  if (button_pad2)
    SDL_FreeSurface(button_pad2);
  if (button_y)
    SDL_FreeSurface(button_y);
  if (button_x)
    SDL_FreeSurface(button_x);
  if (button_b)
    SDL_FreeSurface(button_b);
  if (button_l)
    SDL_FreeSurface(button_l);
  if (button_r)
    SDL_FreeSurface(button_r);
  if (select_bar)
    SDL_FreeSurface(select_bar);
  if (selectkey)
    SDL_FreeSurface(selectkey);
  if (deletekey)
    SDL_FreeSurface(deletekey);
  if (returnkey)
    SDL_FreeSurface(returnkey);
  if (capskey)
    SDL_FreeSurface(capskey);
  if (lockkey)
    SDL_FreeSurface(lockkey);
  if (expery)
    SDL_FreeSurface(expery);
  if (experyw)
    SDL_FreeSurface(experyw);
  for(unsigned int f=0; f<exp_list_base.size(); f++)
    if(exp_list_base[f])
      SDL_FreeSurface(exp_list_base[f]);
  for(unsigned int f=0; f<exp_list_eyes.size(); f++)
    if(exp_list_eyes[f])
      SDL_FreeSurface(exp_list_eyes[f]);
  for(unsigned int f=0; f<exp_list_browns.size(); f++)
    if(exp_list_browns[f])
      SDL_FreeSurface(exp_list_browns[f]);
  for(unsigned int f=0; f<exp_list_noses.size(); f++)
    if(exp_list_noses[f])
      SDL_FreeSurface(exp_list_noses[f]);
  for(unsigned int f=0; f<exp_list_ears.size(); f++)
    if(exp_list_ears[f])
      SDL_FreeSurface(exp_list_ears[f]);
  for(unsigned int f=0; f<exp_list_mouths.size(); f++)
    if(exp_list_mouths[f])
      SDL_FreeSurface(exp_list_mouths[f]);
  for(unsigned int f=0; f<exp_list_facial.size(); f++)
    if(exp_list_facial[f])
      SDL_FreeSurface(exp_list_facial[f]);
  for(unsigned int f=0; f<exp_list_hairs.size(); f++)
    if(exp_list_hairs[f])
      SDL_FreeSurface(exp_list_hairs[f]);
  for(unsigned int f=0; f<exp_list_tatoos.size(); f++)
    if(exp_list_tatoos[f])
      SDL_FreeSurface(exp_list_tatoos[f]);
  for(unsigned int f=0; f<exp_list_clothes.size(); f++)
    if(exp_list_clothes[f])
      SDL_FreeSurface(exp_list_clothes[f]);
  for(unsigned int f=0; f<50; f++)
    if(exp_icons1[f])
      SDL_FreeSurface(exp_icons1[f]);
  for(unsigned int f=0; f<5; f++)
    if(exp_favorites[f])
      SDL_FreeSurface(exp_favorites[f]);
  for(unsigned int f=0; f<5; f++)
    if(exp_completes[f])
      SDL_FreeSurface(exp_completes[f]);
}

void draw_message2(std::string message)
{
  int tw,th;
  TTF_SizeText(font,message.c_str(),&tw,&th);

  int size_box;
  if(tw>250)
    size_box=250;
  else
    size_box=tw+20;

  SDL_Rect box;
  box.x=160-size_box/2;
  box.y=100;
  box.w=size_box;
  box.h=35;
  draw_boxborder(&box,SDL_MapRGB(screen->format,0,0,255),SDL_MapRGB(screen->format,255,255,255));
  draw_text2(screen,(char*)message.c_str(),box.x+10, box.y+5,255,255,0);
}

void draw_message()
{
  std::string m;
  switch(show_message)
  {
    case 1:
      draw_message2(lang.get_string(26));
      break;
    case 2:
      draw_message2(lang.get_string(27));
      break;
    case 3:
      if(exp_list.size()>0)
      {
        m=exp_list[active_exp]->name+std::string(" ")+std::string(lang.get_string(32));
        draw_message2(m);
      }
      break;
    case 4:
      if(exp_list.size()>0)
      {
        m=std::string(lang.get_string(33))+std::string(" ")+exp_list[active_exp]->directory+std::string(".bmp ")+std::string(lang.get_string(34));
        draw_message2(m);
      }
      break;
    case 5:
      if(exp_list.size()>0 && list_games.size()>0)
      {
        m=std::string(lang.get_string(33))+std::string(" bmp ")+std::string(lang.get_string(34));
        draw_message2(m);
      }
      break;
    case 6:
      draw_message2(lang.get_string(36));
      break;
  }
}

void menu01()
{
  menu_max=3;
  SDL_Rect dst,mn;
  dst.w=100;
  dst.h=45;
  dst.x=160-dst.w/2;
  dst.y=120-dst.h/2;
  draw_boxborder(&dst,SDL_MapRGB(screen->format,0,0,128),SDL_MapRGB(screen->format,255,255,255));
  mn.x=dst.x+1;
  mn.w=dst.w-2;
  mn.h=10;
  mn.y=dst.y+8+menu_sel*10;
  SDL_FillRect(screen,&mn,SDL_MapRGB(screen->format,0,180,180));
  draw_text(screen,lang.get_string(29),dst.x+10,dst.y+5,255,255,0);
  draw_text(screen,lang.get_string(30),dst.x+10,dst.y+15,255,255,0);
  draw_text(screen,lang.get_string(31),dst.x+10,dst.y+25,255,255,0);
}

void menu02()
{
  menu_max=2;
  SDL_Rect dst,mn;
  dst.w=100;
  dst.h=35;
  dst.x=160-dst.w/2;
  dst.y=120-dst.h/2;
  draw_boxborder(&dst,SDL_MapRGB(screen->format,0,0,128),SDL_MapRGB(screen->format,255,255,255));
  mn.x=dst.x+1;
  mn.w=dst.w-2;
  mn.h=10;
  mn.y=dst.y+8+menu_sel*10;
  SDL_FillRect(screen,&mn,SDL_MapRGB(screen->format,0,180,180));
  draw_text(screen,lang.get_string(30),dst.x+10,dst.y+5,255,255,0);
  draw_text(screen,lang.get_string(8),dst.x+10,dst.y+15,255,255,0);
}

void menu03()
{
  menu_max=3;
  SDL_Rect dst,mn;
  dst.w=120;
  dst.h=45;
  dst.x=160-dst.w/2;
  dst.y=120-dst.h/2;
  draw_boxborder(&dst,SDL_MapRGB(screen->format,0,0,128),SDL_MapRGB(screen->format,255,255,255));
  mn.x=dst.x+1;
  mn.w=dst.w-2;
  mn.h=10;
  mn.y=dst.y+8+menu_sel*10;
  SDL_FillRect(screen,&mn,SDL_MapRGB(screen->format,0,180,180));
  draw_text(screen,lang.get_string(29),dst.x+10,dst.y+5,255,255,0);
  draw_text(screen,lang.get_string(35),dst.x+10,dst.y+15,255,255,0);
  draw_text(screen,lang.get_string(31),dst.x+10,dst.y+25,255,255,0);
}

void menu04()
{
  menu_max=2;
  SDL_Rect dst,mn;
  dst.w=120;
  dst.h=35;
  dst.x=160-dst.w/2;
  dst.y=120-dst.h/2;
  draw_boxborder(&dst,SDL_MapRGB(screen->format,0,0,128),SDL_MapRGB(screen->format,255,255,255));
  mn.x=dst.x+1;
  mn.w=dst.w-2;
  mn.h=10;
  mn.y=dst.y+8+menu_sel*10;
  SDL_FillRect(screen,&mn,SDL_MapRGB(screen->format,0,180,180));
  draw_text(screen,lang.get_string(35),dst.x+10,dst.y+5,255,255,0);
  draw_text(screen,lang.get_string(8),dst.x+10,dst.y+15,255,255,0);
}

void draw_menu()
{
  switch(menu_show)
  {
    case 1:
      menu01();
      break;
    case 2:
      menu02();
      break;
    case 3:
      menu03();
      break;
    case 4:
      menu04();
      break;
  }
}

void draw_data()
{
/*  char outstr[200];
  time_t t;
  struct tm *tmp;
  t = time(NULL);
  tmp = localtime(&t);
  strftime(outstr, sizeof(outstr), "%H:%M", tmp);

  draw_text(screen,outstr,6,4,0,0,0);*/

  /*unsigned short int num;
  int average=0;
  int bat = open("/dev/batt", O_RDONLY);
  if(bat)
  {
    for (int i=0;i<12;i++)
    {
      read (bat, &num, 2);
      average += num;
    }
  }
  average=average/12;*/

/*  int bat = open("/dev/mmsp2adc", O_RDONLY);
  int fd, rv;
  MMSP2ADC val;
  fd = open("/dev/mmsp2adc", O_RDONLY);
  ioctl(fd,IOCTL_GP2X_MMSP2ADC_CONTROL,BATT_MODE);

  if (fd>=0)
  {
    rv = read (fd, &val, sizeof(MMSP2ADC));
    close (fd);
  }

  char battery[100];
  sprintf(battery,"%i",val.batt);
  draw_text(screen,battery,6,14,0,0,0);*/
}

void draw_keyboard(int caps=0)
{
  static int y=240; // 113=en pantalla, 240=fuera
  if(last_key_time>0)
    last_key_time--;

  if(!show_keyboard && y>=240)
  {
    show_keyboard_accel=1;
    return;
  }

  if(show_keyboard_accel<16)
    show_keyboard_accel*=2;

  if(show_keyboard && y>113)
  {
    y-=show_keyboard_accel;
    if(y<113)
      y=113;
  }
  if(!show_keyboard && y<240)
  {
    y+=show_keyboard_accel;
    if(y>240)
      y=240;
  }

  SDL_Rect dest;
  dest.x=21;
  dest.y=y;
  dest.w=278;
  dest.h=103;
  draw_box2(&dest,SDL_MapRGB(screen->format,192,192,255));
  //SDL_BlitSurface(backkeyboard,NULL,screen,&dest);

  int px=23;
  dest.y+=2;

  std::string l1;
  std::string l2;
  std::string l3;
  std::string l4;

  l1="1234567890 ";
  if(key_caps || key_lock)
  {
    l2="QWERTYUIOP";
    l3=" ASDFGHJKL ";
    l4=" ZXCVBNM ";
  }
  else
  {
    l2="qwertyuiop";
    l3=" asdfghjkl ";
    l4=" zxcvbnm ";
  }

  for(int f=0; f<l1.size(); f++)
  {
    dest.x=px+f*25;
    dest.w=24;
    dest.h=24;
    draw_box2(&dest,SDL_MapRGB(screen->format,128,0,128));
    draw_text(screen, (char*)l1.substr(f,1).c_str(),dest.x+7,dest.y-3,255,255,0,font2);
    if(last_key_time>0 && last_key_line==1 & last_key_file==f+1)
      draw_text(screen, (char*)l1.substr(f,1).c_str(),dest.x+7,dest.y-3,255,0,0,font2);
    if(key_line_cursor==1 && key_file_cursor==f+1)
      SDL_BlitSurface(selectkey,NULL,screen,&dest);
  }
  SDL_BlitSurface(deletekey,NULL,screen,&dest);
  dest.y+=25;
  for(int f=0; f<l2.size(); f++)
  {
    dest.x=(px+12)+f*25;
    draw_box2(&dest,SDL_MapRGB(screen->format,128,0,128));
    draw_text(screen, (char*)l2.substr(f,1).c_str(),dest.x+7,dest.y-3,255,255,0,font2);
    if(last_key_time>0 && last_key_line==2 & last_key_file==f+1)
      draw_text(screen, (char*)l2.substr(f,1).c_str(),dest.x+7,dest.y-3,255,0,0,font2);
    if(key_line_cursor==2 && key_file_cursor==f+1)
      SDL_BlitSurface(selectkey,NULL,screen,&dest);
  }
  dest.y+=25;
  for(int f=0; f<l3.size(); f++)
  {
    dest.x=px+f*25;
    draw_box2(&dest,SDL_MapRGB(screen->format,128,0,128));
    draw_text(screen, (char*)l3.substr(f,1).c_str(),dest.x+7,dest.y-3,255,255,0,font2);
    if(last_key_time>0 && last_key_line==3 & last_key_file==f+1)
      draw_text(screen, (char*)l3.substr(f,1).c_str(),dest.x+7,dest.y-3,255,0,0,font2);
    if(key_line_cursor==3 && key_file_cursor==f+1)
      SDL_BlitSurface(selectkey,NULL,screen,&dest);
  }
  SDL_BlitSurface(returnkey,NULL,screen,&dest);
  dest.x=px;
  SDL_BlitSurface(lockkey,NULL,screen,&dest);
  dest.y+=25;
  for(int f=0; f<l4.size(); f++)
  {
    dest.x=(px+12)+f*25;
    draw_box2(&dest,SDL_MapRGB(screen->format,128,0,128));
    draw_text(screen, (char*)l4.substr(f,1).c_str(),dest.x+7,dest.y-3,255,255,0,font2);
    if(last_key_time>0 && last_key_line==4 & last_key_file==f+1)
      draw_text(screen, (char*)l4.substr(f,1).c_str(),dest.x+7,dest.y-3,255,0,0,font2);
    if(key_line_cursor==4 && key_file_cursor==f+1)
      SDL_BlitSurface(selectkey,NULL,screen,&dest);
  }
  dest.x=px+12;
  SDL_BlitSurface(capskey,NULL,screen,&dest);

}

///////////////////////////////////
/*  Carga los sonidos            */
///////////////////////////////////
void load_sounds()
{
  sound_next=Mix_LoadWAV("media/next.wav");
}

///////////////////////////////////
/*  Libera la memoria de los     */
/*  sonidos                      */
///////////////////////////////////
void release_sounds()
{
    Mix_FreeChunk(sound_next);
    Mix_CloseAudio();
}

///////////////////////////////////
/*  Recupera los juegos del exp  */
/*  actual                        */
///////////////////////////////////
void load_games()
{
  if(exp_list.size()==0)
    return;

  list_games.clear();
  active_game=0;
  std::string exp_dir;
  exp_dir=exp_root+exp_list[active_exp]->directory+"/";

  DIR* dir;
  struct dirent *direntry;
  dir = opendir(exp_dir.c_str());

  while((direntry=readdir(dir))!=NULL)
  {
    std::string fname=direntry->d_name;
    std::string file_control=exp_dir+fname+"/exp.dat";
    struct stat st;
    if(stat(file_control.c_str(),&st)==0)
      list_games.push_back(fname);
  }
  closedir(dir);
}

void load_game_icons()
{
  if(exp_list.size()==0 || list_games.size()==0)
    return;

  // primero borra los gráficos en memoria
  for(unsigned int f=0; f<50; f++)
    if(exp_icons1[f])
    {
      SDL_FreeSurface(exp_icons1[f]);
      exp_icons1[f]=NULL;
    }

  // ahora carga los iconos
  for(unsigned int f=1; f<=50; f++)
  {
    char fname[100];
    if(f<10)
      sprintf(fname,"00%i",f);
    else if(f<100)
      sprintf(fname,"0%i",f);
    else
      sprintf(fname,"%i",f);
    std::string icon;
    icon=exp_root+exp_list[active_exp]->directory+"/"+list_games[active_game]+"/"+std::string(fname);

    SDL_Surface* tmpsurface=SDL_LoadBMP(icon.c_str());
    if(tmpsurface)
    {
      SDL_Rect rect;
      rect.x=0;
      rect.y=0;
      rect.w=tmpsurface->w;
      rect.h=tmpsurface->h;
      exp_icons1[f-1]=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
      SDL_BlitSurface(tmpsurface,&rect,exp_icons1[f-1],NULL);
      SDL_SetColorKey(exp_icons1[f-1],SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
      SDL_FreeSurface(tmpsurface);
    }
  }
}

int load_game_experience(std::vector<experience>& experience_list)
{
  experience_list.clear();

  if(exp_list.size()==0 || list_games.size()==0)
    return 0;

  std::string game_dir=exp_root+exp_list[active_exp]->directory+"/"+list_games[active_game]+"/";
  std::string fname=game_dir+"exp.title";

  // primero cargamos el nombre del juego
  std::ifstream cfgfile;
  cfgfile.open(fname.c_str());

  if(cfgfile)
  {
    char buffer[1024];
    std::string bfline;
    std::string parm;
    std::string value;
    while(cfgfile.getline(buffer,1024))
    {
      bfline=buffer;
      unsigned int i=bfline.find("=",1);
      parm=bfline.substr(0,i);
      if(i<=parm.size())
        value=bfline.substr(i);
      while(value.substr(value.size()-1,1)<=" ")
        value=value.substr(0,value.size()-1);
      if(parm==std::string("title") && value.size()>0)
        title_game1=value.substr(1);
      if(parm==std::string("author") && value.size()>0)
        author_game1=value.substr(1);
    }
    cfgfile.close();
  }

  // y ahora, cargamos los logros
  fname=game_dir+"exp.dat";
  FILE* exp_file=fopen(fname.c_str(),"rb");
  if(exp_file)
  {
    experience e;
    while(fread(&e,sizeof(experience),1,exp_file)==1)
      experience_list.push_back(e);
    fclose(exp_file);
  }

  // y el idioma




  // y los iconos
  load_game_icons();

  return 1;
}

int load_game_summary(game_summary& game)
{
  if(list_games.size()<=0)
    return 0;

  exp_game1.won=0;
  exp_game1.won_max=0;
  exp_game1.total=0;
  exp_game1.total_max=0;
  exp_game1.played=0;

  std::string game_dir;
  game_dir=exp_root+exp_list[active_exp]->directory+"/"+list_games[active_game]+"/";

  std::string fname=game_dir+"exp.summary";
  FILE* exp_file=fopen(fname.c_str(),"rb");
  if(exp_file)
  {
    fread(&exp_game1.won,sizeof(int),1,exp_file);
    fread(&exp_game1.won_max,sizeof(int),1,exp_file);
    fread(&exp_game1.total,sizeof(int),1,exp_file);
    fread(&exp_game1.total_max,sizeof(int),1,exp_file);
    fread(&exp_game1.played,sizeof(int),1,exp_file);
    fclose(exp_file);
  }

  return 1;
}

std::string first_lang()
{
  std::string result;
  std::ifstream efile;
  std::string exp_file=exp_root+list_games[active_game]+"/"+exp_list[active_exp]->directory+"/exp.lang";
  efile.open(exp_file.c_str());

  if(efile)
  {
    char buffer[1024];
    efile.getline(buffer,1024);
    result=std::string(buffer);
    efile.close();
  }

  return result;
}

void crypt_desc()
{
  // desencriptamos el texto con un XOR para que no se lea con un editor de textos
  for(unsigned int f=0; f<exp_desc1.size(); f++)
  {
    for(unsigned int i=0; i<21; i++)
      exp_desc1[f].name[i]^=255;
    for(unsigned int i=0; i<101; i++)
      exp_desc1[f].desc[i]^=255;
  }
}

///////////////////////////////////
/*  Recupera la descripcion de   */
/*  las experiencias del juego   */
/*  actual                       */
///////////////////////////////////
int load_desc()
{
  exp_desc1.clear();

  if(list_games.size()==0 || exp_list.size()==0)
    return 0;

  std::string dir=exp_root+exp_list[active_exp]->directory+"/"+list_games[active_game]+"/";
  std::string fname;
  struct stat st;
  fname=dir+"exp.desc."+exp_list[active_exp]->lang;
  if(stat((char*)fname.c_str(),&st)!=0)
  {
    fname=dir+"exp.desc.default";
    if(stat((char*)fname.c_str(),&st)!=0)
    {
      fname=dir+"exp.desc."+first_lang();
      if(stat((char*)fname.c_str(),&st)!=0)
        return 0;
    }
  }

  FILE* exp_file;
  exp_file=fopen(fname.c_str(),"rb");
  if(exp_file)
  {
    experience_lang e;
    while(fread(&e, sizeof(experience_lang), 1, exp_file)==1)
      exp_desc1.push_back(e);
    crypt_desc();

    fclose(exp_file);
    return 1;
  }

  return 0;
}

void insert_complete(long int value, std::string directory)
{
  int inserted=FALSE;
  for(int f=0; f<list_completes.size(); f++)
  {
    if(list_completes[f].value<value)
    {
      game_value g;
      g.value=value;
      g.directory=directory;
      list_completes.insert(list_completes.begin()+f,g);
      inserted=TRUE;
      break;
    }
  }
  if(!inserted)
  {
    game_value g;
    g.value=value;
    g.directory=directory;
    list_completes.push_back(g);
  }
}

void insert_favorite(int value, std::string directory)
{
  int inserted=FALSE;
  for(int f=0; f<list_favorites.size(); f++)
  {
    if(list_favorites[f].value<value)
    {
      game_value g;
      g.value=value;
      g.directory=directory;
      list_favorites.insert(list_favorites.begin()+f,g);
      inserted=TRUE;
      break;
    }
  }
  if(!inserted)
  {
    game_value g;
    g.value=value;
    g.directory=directory;
    list_favorites.push_back(g);
  }
}

///////////////////////////////////
/*  Recupera la experiencia del  */
/*  exp actual                   */
///////////////////////////////////
void load_experience()
{
  if(exp_list.size()<=0)
    return;

  exp_list[active_exp]->won=0;
  exp_list[active_exp]->won_max=0;
  exp_list[active_exp]->total=0;
  exp_list[active_exp]->total_max=0;

  std::string exp_dir;
  exp_dir=exp_root+exp_list[active_exp]->directory+"/";

  // recupera la experiencia total
  std::string fname=exp_dir+"exp.erience";
  FILE* exp_file=fopen(fname.c_str(),"rb");
  if(exp_file)
  {
    fread(&exp_list[active_exp]->won,sizeof(int),1,exp_file);
    fread(&exp_list[active_exp]->won_max,sizeof(int),1,exp_file);
    fread(&exp_list[active_exp]->total,sizeof(int),1,exp_file);
    fread(&exp_list[active_exp]->total_max,sizeof(int),1,exp_file);
    fclose(exp_file);
  }
}

///////////////////////////////////
/*  Recupera la experiencia del  */
/*  exp actual                   */
///////////////////////////////////
void update_experience()
{
  if(exp_list.size()==0)
    return;

  list_favorites.clear();
  list_completes.clear();

  exp_list[active_exp]->won=0;
  exp_list[active_exp]->won_max=0;
  exp_list[active_exp]->total=0;
  exp_list[active_exp]->total_max=0;

  std::string exp_dir;
  exp_dir=exp_root+exp_list[active_exp]->directory+"/";

  DIR* dir;
  struct dirent *direntry;
  dir = opendir(exp_dir.c_str());

  while((direntry=readdir(dir))!=NULL)
  {
    int w=0;
    int wm=0;
    int t=0;
    int tm=0;
    int pl=0;
    std::string fname=exp_dir+std::string(direntry->d_name)+"/exp.summary";
    FILE* exp_file;
    exp_file=fopen(fname.c_str(),"rb");
    if(exp_file)
    {
      fread(&w,sizeof(int),1,exp_file);
      fread(&wm,sizeof(int),1,exp_file);
      fread(&t,sizeof(int),1,exp_file);
      fread(&tm,sizeof(int),1,exp_file);
      fread(&pl,sizeof(int),1,exp_file);
      fclose(exp_file);

      exp_list[active_exp]->won+=w;
      exp_list[active_exp]->won_max+=wm;
      exp_list[active_exp]->total+=t;
      exp_list[active_exp]->total_max+=tm;

      insert_favorite(pl,std::string(direntry->d_name));
      if(w==wm)
      {
        std::string fl=exp_dir+std::string(direntry->d_name)+"/exp.dat";
        struct stat st;
        stat(fl.c_str(),&st);
        long int value=st.st_mtime;
        insert_complete(value,std::string(direntry->d_name));
      }
    }
  }
  closedir(dir);

  // guarda la experiencia totl
  std::string fname=exp_dir+"exp.erience";
  FILE* exp_file=fopen(fname.c_str(),"wb");
  if(exp_file)
  {
    fwrite(&exp_list[active_exp]->won,sizeof(int),1,exp_file);
    fwrite(&exp_list[active_exp]->won_max,sizeof(int),1,exp_file);
    fwrite(&exp_list[active_exp]->total,sizeof(int),1,exp_file);
    fwrite(&exp_list[active_exp]->total_max,sizeof(int),1,exp_file);
    fclose(exp_file);
  }
  fname=exp_dir+"update";
  remove(fname.c_str());
  save_game_ranking();
}

void load_active_exp()
{
  std::ifstream act_exp;
  std::string f=exp_root+"/activeexp";
  act_exp.open(f.c_str());
  if(act_exp)
  {
    char buffer[1024];
    act_exp.getline(buffer,1024);
    std::string exp=std::string(buffer);
    for(int f=0; f<exp_list.size(); f++)
    {
      if(exp_list[f]->directory==exp)
      {
        active_exp=f;
        lang.set_language(exp_list[f]->lang);
        break;
      }
    }
  }
  act_exp.close();
  if(need_update())
    update_experience();
  else
    load_experience();
  load_game_ranking();
}

void save_active_exp()
{
  FILE* act_exp;
  std::string f=exp_root+"/activeexp";
  act_exp=fopen(f.c_str(),"wb");
  if(act_exp)
  {
    char buffer[100];
    sprintf(buffer,"%s",exp_list[active_exp]->directory.c_str());
    std::string opt=std::string(buffer);
    fwrite(opt.c_str(), opt.size(), 1, act_exp);
    fclose(act_exp);
  }
}

///////////////////////////////////
/*  Borra el exp actual          */
///////////////////////////////////
void set_present_exp(std::string d="", std::string l="", std::string nm="", std::string bs="01", std::string ey="", std::string br="", std::string ea="", std::string ns="", std::string mt="", std::string fc="", std::string hr="", std::string tt="", std::string cl="")
{
  present_exp.directory=d;
  present_exp.lang=l;
  present_exp.name=nm;
  present_exp.base=bs;
  present_exp.eyes=ey;
  present_exp.browns=br;
  present_exp.ears=ea;
  present_exp.nose=ns;
  present_exp.mouth=mt;
  present_exp.facial=fc;
  present_exp.hair=hr;
  present_exp.tatoo=tt;
  present_exp.clothes=cl;
}

///////////////////////////////////
/*  Carga una estructura exp     */
///////////////////////////////////
int load_exp(struct exp *e)
{
  if(e)
  {
    std::ifstream cfgfile;
    std::string exp_file=exp_root+e->directory+std::string("/exp.cfg");
    cfgfile.open(exp_file.c_str());

    if(cfgfile)
    {
      char buffer[1024];
      std::string bfline;
      std::string parm;
      std::string value;
      while(cfgfile.getline(buffer,1024))
      {
        bfline=buffer;
        unsigned int i=bfline.find("=",1);
        parm=bfline.substr(0,i);
        if(i<=parm.size())
          value=bfline.substr(i);
        while(value.substr(value.size()-1,1)<=" ")
          value=value.substr(0,value.size()-1);

        if(parm==std::string("lang") && value.size()>0)
        {
          e->lang=value.substr(1);
          e->lang_id=lang.language_id(e->lang);
        }
        if(parm==std::string("name") && value.size()>0)
          e->name=value.substr(1);
        if(parm==std::string("base") && value.size()>0)
          e->base=value.substr(1);
        if(parm==std::string("eyes") && value.size()>0)
          e->eyes=value.substr(1);
        if(parm==std::string("browns") && value.size()>0)
          e->browns=value.substr(1);
        if(parm==std::string("nose") && value.size()>0)
          e->nose=value.substr(1);
        if(parm==std::string("ears") && value.size()>0)
          e->ears=value.substr(1);
        if(parm==std::string("mouth") && value.size()>0)
          e->mouth=value.substr(1);
        if(parm==std::string("facial") && value.size()>0)
          e->facial=value.substr(1);
        if(parm==std::string("hair") && value.size()>0)
          e->hair=value.substr(1);
        if(parm==std::string("tatoo") && value.size()>0)
          e->tatoo=value.substr(1);
        if(parm==std::string("clothes") && value.size()>0)
          e->clothes=value.substr(1);
      }
      cfgfile.close();
      return 1;
    }
    else
      return 0;
  }
  else
    return 0;
}

///////////////////////////////////
/*  Inicializa los parámetros    */
/*  juego.                       */
///////////////////////////////////
void init_game()
{
  // si no existe la carpeta /SD/EXP la crea
  struct stat st;
#ifdef PLATFORM_GP2X
  if(stat(exp_root.c_str(),&st) != 0)
    mkdir(exp_root.c_str(),0);
    //mkdir(exp_root.c_str(),S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH | S_ISVTX);
    //system("mkdir /mnt/sd/EXP");
#endif
#ifdef PLATFORM_WIN
  if(stat(exp_root.c_str(),&st) != 0)
    mkdir(exp_root.c_str());
#endif

  char name[100];

  // carga los nombres de los archivos gráficos del exp
  list_base.clear();
  for(int f=1; f<100; f++)
  {
    if(f<10)
      sprintf(name,"media/exp_base_0%i.bmp",f);
    else
      sprintf(name,"media/exp_base_%i.bmp",f);

    if(stat(name,&st) !=0)
      break;
    else
      list_base.push_back(name);
  }

  list_eyes.clear();
  for(int f=1; f<100; f++)
  {
    if(f<10)
      sprintf(name,"media/exp_eyes_0%i.bmp",f);
    else
      sprintf(name,"media/exp_eyes_%i.bmp",f);

    if(stat(name,&st) !=0)
      break;
    else
      list_eyes.push_back(name);
  }

  list_browns.clear();
  for(int f=1; f<100; f++)
  {
    if(f<10)
      sprintf(name,"media/exp_browns_0%i.bmp",f);
    else
      sprintf(name,"media/exp_browns_%i.bmp",f);

    if(stat(name,&st) !=0)
      break;
    else
      list_browns.push_back(name);
  }

  list_noses.clear();
  for(int f=1; f<100; f++)
  {
    if(f<10)
      sprintf(name,"media/exp_nose_0%i.bmp",f);
    else
      sprintf(name,"media/exp_nose_%i.bmp",f);

    if(stat(name,&st) !=0)
      break;
    else
      list_noses.push_back(name);
  }

  list_ears.clear();
  for(int f=1; f<100; f++)
  {
    if(f<10)
      sprintf(name,"media/exp_ears_0%i.bmp",f);
    else
      sprintf(name,"media/exp_ears_%i.bmp",f);

    if(stat(name,&st) !=0)
      break;
    else
      list_ears.push_back(name);
  }

  list_mouths.clear();
  for(int f=1; f<100; f++)
  {
    if(f<10)
      sprintf(name,"media/exp_mouth_0%i.bmp",f);
    else
      sprintf(name,"media/exp_mouth_%i.bmp",f);

    if(stat(name,&st) !=0)
      break;
    else
      list_mouths.push_back(name);
  }

  list_facial.clear();
  for(int f=1; f<100; f++)
  {
    if(f<10)
      sprintf(name,"media/exp_facial_0%i.bmp",f);
    else
      sprintf(name,"media/exp_facial_%i.bmp",f);

    if(stat(name,&st) !=0)
      break;
    else
      list_facial.push_back(name);
  }

  list_hairs.clear();
  for(int f=1; f<100; f++)
  {
    if(f<10)
      sprintf(name,"media/exp_hair_0%i.bmp",f);
    else
      sprintf(name,"media/exp_hair_%i.bmp",f);

    if(stat(name,&st) !=0)
      break;
    else
      list_hairs.push_back(name);
  }

  list_tatoos.clear();
  for(int f=1; f<100; f++)
  {
    if(f<10)
      sprintf(name,"media/exp_tatoo_0%i.bmp",f);
    else
      sprintf(name,"media/exp_tatoo_%i.bmp",f);

    if(stat(name,&st) !=0)
      break;
    else
      list_tatoos.push_back(name);
  }

  list_clothes.clear();
  for(int f=1; f<100; f++)
  {
    if(f<10)
      sprintf(name,"media/exp_clothes_0%i.bmp",f);
    else
      sprintf(name,"media/exp_clothes_%i.bmp",f);

    if(stat(name,&st) !=0)
      break;
    else
      list_clothes.push_back(name);
  }

  // carga en memoria todos los exp
  exp_list.clear();
  DIR* dir;
  struct dirent *direntry;
  dir = opendir(exp_root.c_str());

  while((direntry=readdir(dir))!=NULL)
  {
    if(std::string(direntry->d_name).substr(0,4)==std::string("exp_"))
    {
      struct exp *e=new struct exp;
      e->directory=std::string(direntry->d_name);
      if(load_exp(e))
        exp_list.push_back(e);
      else
        delete e;
    }
  }
  closedir(dir);
  // carga la experiencia de cada exp
  for(int f=0; f<exp_list.size(); f++)
  {
    active_exp=f;
    load_experience();
  }

  // borra los punteros de los iconos
  for(unsigned int f=0; f<50; f++)
    exp_icons1[f]=NULL;
  // borra los punteros de los favoritos
  for(unsigned int f=0; f<5; f++)
    exp_favorites[f]=NULL;
  // borra los punteros de los completos
  for(unsigned int f=0; f<5; f++)
    exp_completes[f]=NULL;

  // si no hay exps, muestra un mensaje de ayuda
  if(exp_list.size()==0 && !done_mess1)
  {
    show_message=1;
    done_mess1=TRUE;
  }
}

///////////////////////////////////
/*  Dibuja un exp                */
///////////////////////////////////
void draw_exp(struct exp *e)
{
  SDL_Rect back;
  back.x=50;
  back.y=60;

  if(e)
  {
    unsigned int i;
    i=atoi(e->base.c_str());
    if(i>0 && i<=exp_list_base.size())
      SDL_BlitSurface(exp_list_base[i-1],NULL,screen,&back);
    i=atoi(e->tatoo.c_str());
    if(i>0 && i<=exp_list_tatoos.size())
      SDL_BlitSurface(exp_list_tatoos[i-1],NULL,screen,&back);
    i=atoi(e->mouth.c_str());
    if(i>0 && i<=exp_list_mouths.size())
      SDL_BlitSurface(exp_list_mouths[i-1],NULL,screen,&back);
    i=atoi(e->facial.c_str());
    if(i>0 && i<=exp_list_facial.size())
      SDL_BlitSurface(exp_list_facial[i-1],NULL,screen,&back);
    i=atoi(e->eyes.c_str());
    if(i>0 && i<=exp_list_eyes.size())
      SDL_BlitSurface(exp_list_eyes[i-1],NULL,screen,&back);
    i=atoi(e->nose.c_str());
    if(i>0 && i<=exp_list_noses.size())
      SDL_BlitSurface(exp_list_noses[i-1],NULL,screen,&back);
    i=atoi(e->browns.c_str());
    if(i>0 && i<=exp_list_browns.size())
      SDL_BlitSurface(exp_list_browns[i-1],NULL,screen,&back);
    i=atoi(e->hair.c_str());
    if(i>0 && i<=exp_list_hairs.size())
      SDL_BlitSurface(exp_list_hairs[i-1],NULL,screen,&back);
    i=atoi(e->ears.c_str());
    if(i>0 && i<=exp_list_ears.size())
      SDL_BlitSurface(exp_list_ears[i-1],NULL,screen,&back);
    i=atoi(e->clothes.c_str());
    if(i>0 && i<=exp_list_clothes.size())
      SDL_BlitSurface(exp_list_clothes[i-1],NULL,screen,&back);
  }
}

///////////////////////////////////
/*  Dibuja la seleccion del exp  */
///////////////////////////////////
int update_select_exp()
{
  // siempre hay que procesar la cola de eventos, o no se podrán
  // comprobar los botones
  std::string dname;
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
#ifdef PLATFORM_GP2X
    switch (event.type)
    {
      case SDL_JOYBUTTONDOWN:
        if(show_message)
        {
          if(show_message==3)
            done=1;
          show_message=0;
          break;
        }
        switch (event.jbutton.button)
        {
          case GP2X_BUTTON_LEFT:
            if(menu_show)
              break;
            active_exp-=1;
            if(active_exp<0)
              active_exp=exp_list.size()-1;
            if(exp_list.size()>0)
              lang.set_language(exp_list[active_exp]->lang);
            if(need_update())
              update_experience();
            load_game_ranking();
            Mix_PlayChannel(-1,sound_next,0);
            break;
          case GP2X_BUTTON_RIGHT:
            if(menu_show)
              break;
            active_exp+=1;
            if(active_exp>=exp_list.size())
              active_exp=0;
            if(exp_list.size()>0)
              lang.set_language(exp_list[active_exp]->lang);
            if(need_update())
              update_experience();
            load_game_ranking();
            Mix_PlayChannel(-1,sound_next,0);
            break;
          case GP2X_BUTTON_UP:
            if(menu_show)
            {
              menu_sel--;
              if(menu_sel<0)
                menu_sel=menu_max-1;
            }
            break;
          case GP2X_BUTTON_DOWN:
            if(menu_show)
            {
              menu_sel++;
              if(menu_sel>=menu_max)
                menu_sel=0;
            }
            break;
          case GP2X_BUTTON_Y:
            if(menu_show)
              break;
            set_present_exp();
            if(exp_list.size()==0)
            {
              show_message=2;
              done_mess2=TRUE;
            }
            program_mode=MODE_CREATE_EXP;
            break;
          case GP2X_BUTTON_X:
            if(menu_show)
            {
              check_menu();
              break;
            }
            if(exp_list.size()>0)
            {
              set_present_exp(exp_list[active_exp]->directory, exp_list[active_exp]->lang, exp_list[active_exp]->name, exp_list[active_exp]->base, exp_list[active_exp]->eyes, exp_list[active_exp]->browns, exp_list[active_exp]->ears, exp_list[active_exp]->nose, exp_list[active_exp]->mouth, exp_list[active_exp]->facial, exp_list[active_exp]->hair, exp_list[active_exp]->tatoo, exp_list[active_exp]->clothes);
              selected_attribute=0;
              program_mode=MODE_CREATE_EXP;
            }
            break;
          case GP2X_BUTTON_B:
            if(menu_show)
            {
              check_menu();
              break;
            }
            load_games();
            load_game_summary(exp_game1);
            load_game_experience(exp_experience1);
            load_desc();
            exp_file_cursor=1;
            exp_line_cursor=1;
            if(list_games.size()>0)
              program_mode=MODE_VIEW_DETAILS;
            break;
          case GP2X_BUTTON_A:
            break;
          case GP2X_BUTTON_START:
            if(!menu_show)
            {
              menu_sel=0;
              menu_show=1;
            }
            else
              menu_show=0;
            break;
        }
    }
#endif
#ifdef PLATFORM_WIN
    switch (event.type)
    {
      case SDL_KEYDOWN:
        if(show_message)
        {
          if(show_message==3)
            done=1;
          show_message=0;
          break;
        }
        switch (event.key.keysym.sym)
        {
          case SDLK_LEFT:
            if(menu_show)
              break;
            active_exp-=1;
            if(active_exp<0)
              active_exp=exp_list.size()-1;
            if(exp_list.size()>0)
              lang.set_language(exp_list[active_exp]->lang);
            if(need_update())
              update_experience();
            load_game_ranking();
            Mix_PlayChannel(-1,sound_next,0);
            break;
          case SDLK_RIGHT:
            if(menu_show)
              break;
            active_exp+=1;
            if(active_exp>=exp_list.size())
              active_exp=0;
            if(exp_list.size()>0)
              lang.set_language(exp_list[active_exp]->lang);
            if(need_update())
              update_experience();
            load_game_ranking();
            Mix_PlayChannel(-1,sound_next,0);
            break;
          case SDLK_UP:
            if(menu_show)
            {
              menu_sel--;
              if(menu_sel<0)
                menu_sel=menu_max-1;
            }
            break;
          case SDLK_DOWN:
            if(menu_show)
            {
              menu_sel++;
              if(menu_sel>=menu_max)
                menu_sel=0;
            }
            break;
          case SDLK_y:
            if(menu_show)
              break;
            if(exp_list.size()==0)
            {
              show_message=2;
              done_mess2=TRUE;
            }
            set_present_exp();
            program_mode=MODE_CREATE_EXP;
            break;
          case SDLK_RETURN:
          case SDLK_x:
            if(menu_show)
            {
              check_menu();
              break;
            }
            if(exp_list.size()>0)
            {
              set_present_exp(exp_list[active_exp]->directory, exp_list[active_exp]->lang, exp_list[active_exp]->name, exp_list[active_exp]->base, exp_list[active_exp]->eyes, exp_list[active_exp]->browns, exp_list[active_exp]->ears, exp_list[active_exp]->nose, exp_list[active_exp]->mouth, exp_list[active_exp]->facial, exp_list[active_exp]->hair, exp_list[active_exp]->tatoo, exp_list[active_exp]->clothes);
              selected_attribute=0;
              program_mode=MODE_CREATE_EXP;
            }
            break;
          case SDLK_b:
            if(menu_show)
            {
              check_menu();
              break;
            }
            load_games();
            load_game_summary(exp_game1);
            load_game_experience(exp_experience1);
            load_desc();
            exp_file_cursor=1;
            exp_line_cursor=1;
            if(list_games.size()>0)
              program_mode=MODE_VIEW_DETAILS;
            break;
          case SDLK_a:
            break;
          case SDLK_ESCAPE:
            if(!menu_show)
            {
              menu_sel=0;
              menu_show=1;
            }
            else
              menu_show=0;
            break;
        }
    }
#endif
  }

  // dibuja los datos del sistema
  draw_data();

  // si existe algún EXP, dibuja la tarjeta
  SDL_Rect dest;
  if(exp_list.size()>0)
  {
    dest.x=24;
    dest.y=24;
    SDL_BlitSurface(card,NULL,screen,&dest);

    draw_text(screen,lang.get_string(5),156,80,0,0,0);
    draw_text(screen,lang.get_string(6),156,128,0,0,0);
  }

  if(exp_list.size()>0 && active_exp>=0 && active_exp<exp_list.size())
  {
    draw_exp(exp_list[active_exp]);
    draw_text(screen,(char*)(exp_list[active_exp])->name.c_str(),31,31,0,0,0);
    draw_text(screen,(char*)(exp_list[active_exp])->name.c_str(),30,30,255,255,255);
    char experience[50];
    sprintf(experience,"%i",exp_list[active_exp]->total);
    int tw,th;
    TTF_SizeText(font,experience,&tw,&th);
    int px=270-tw;
    draw_text(screen,experience,px,60,255,255,255);
    // estrellas
    int prc=0;
    if(exp_list[active_exp]->total_max)
      prc=exp_list[active_exp]->total*100/exp_list[active_exp]->total_max;
    int nstars=0;
    if(prc>0)
      nstars=(prc/20)+1;
    if(nstars>5)
      nstars=5;
    dest.x=180;
    dest.y=28;
    for(int f=0; f<nstars; f++)
      if(star)
      {
        SDL_BlitSurface(star,NULL,screen,&dest);
        dest.x+=22;
      }
    // completos
    dest.x=154;
    dest.y=94;
    for(int f=0; f<5; f++)
    {
      if(exp_completes[f])
        SDL_BlitSurface(exp_completes[f],NULL,screen,&dest);
      dest.x+=24;
    }
    // favoritos
    dest.x=154;
    dest.y=142;
    for(int f=0; f<5; f++)
    {
      if(exp_favorites[f])
        SDL_BlitSurface(exp_favorites[f],NULL,screen,&dest);
      dest.x+=24;
    }
  }

  // si hay que hacer un screenshot, este es el momento
  if(make_shot>0)
  {
    make_shot--;
    if(make_shot==1)
    {
      if(exp_list.size()>0)
      {
        std::string fname=exp_root+exp_list[active_exp]->directory+std::string(".bmp");
        SDL_SaveBMP(screen,fname.c_str());
      }
    }
    show_message=4;
  }

  // mensajes de ayuda
  if(exp_list.size()>1)
  {
    dest.x=24;
    dest.y=220;
    if(button_pad)
      SDL_BlitSurface(button_pad,NULL,screen,&dest);
    draw_text(screen,lang.get_string(1),45,220,255,255,0);
  }
  if(exp_list.size()>0)
  {
    dest.x=110;
    dest.y=220;
    if(button_x)
      SDL_BlitSurface(button_x,NULL,screen,&dest);
    draw_text(screen,lang.get_string(2),125,220,255,255,0);
    dest.x=175;
    dest.y=220;
    if(button_b)
      SDL_BlitSurface(button_b,NULL,screen,&dest);
    draw_text(screen,lang.get_string(3),195,220,255,255,0);
  }
  dest.x=220;
  dest.y=220;
  if(button_y)
    SDL_BlitSurface(button_y,NULL,screen,&dest);
  draw_text(screen,lang.get_string(4),235,220,255,255,0);

  // mensajes de ayuda de manejo
  if(show_message)
    draw_message();

  // menus
  draw_menu();

  return 1;
}

void change_attribute_left()
{
  int i;
  switch(selected_attribute)
  {
    case 0:
      if(present_exp.directory.size()==0)
        show_keyboard=TRUE;
      break;
    case 1:
      present_exp.lang_id--;
      if(present_exp.lang_id<0)
        present_exp.lang_id=lang.languages_count()-1;
      present_exp.lang=lang.language_name(present_exp.lang_id);
      lang.set_language(present_exp.lang_id);
      break;
    case 2: // base
      i=atoi(present_exp.base.c_str());
      i--;
      if(i<0)
        i=exp_list_base.size();
      present_exp.base=tostring(i);
      break;
    case 3: // ojos
      i=atoi(present_exp.eyes.c_str());
      i--;
      if(i<0)
        i=exp_list_eyes.size();
      present_exp.eyes=tostring(i);
      break;
    case 4: // cejas
      i=atoi(present_exp.browns.c_str());
      i--;
      if(i<0)
        i=exp_list_browns.size();
      present_exp.browns=tostring(i);
      break;
    case 5: // nariz
      i=atoi(present_exp.nose.c_str());
      i--;
      if(i<0)
        i=exp_list_noses.size();
      present_exp.nose=tostring(i);
      break;
    case 6: // orejas
      i=atoi(present_exp.ears.c_str());
      i--;
      if(i<0)
        i=exp_list_ears.size();
      present_exp.ears=tostring(i);
      break;
    case 7: // boca
      i=atoi(present_exp.mouth.c_str());
      i--;
      if(i<0)
        i=exp_list_mouths.size();
      present_exp.mouth=tostring(i);
      break;
    case 8: // barba
      i=atoi(present_exp.facial.c_str());
      i--;
      if(i<0)
        i=exp_list_facial.size();
      present_exp.facial=tostring(i);
      break;
    case 9: // pelo
      i=atoi(present_exp.hair.c_str());
      i--;
      if(i<0)
        i=exp_list_hairs.size();
      present_exp.hair=tostring(i);
      break;
    case 10: // tatuaje
      i=atoi(present_exp.tatoo.c_str());
      i--;
      if(i<0)
        i=exp_list_tatoos.size();
      present_exp.tatoo=tostring(i);
      break;
    case 11: // ropa
      i=atoi(present_exp.clothes.c_str());
      i--;
      if(i<0)
        i=exp_list_clothes.size();
      present_exp.clothes=tostring(i);
      break;
  }
}

void change_attribute_right()
{
  int i;
  switch(selected_attribute)
  {
    case 0:
      if(present_exp.directory.size()==0)
        show_keyboard=TRUE;
      break;
    case 1:
      present_exp.lang_id++;
      if(present_exp.lang_id>=lang.languages_count())
        present_exp.lang_id=0;
      present_exp.lang=lang.language_name(present_exp.lang_id);
      lang.set_language(present_exp.lang_id);
      break;
    case 2: // base
      i=atoi(present_exp.base.c_str());
      i++;
      if(i>exp_list_base.size())
        i=0;
      present_exp.base=tostring(i);
      break;
    case 3: // ojos
      i=atoi(present_exp.eyes.c_str());
      i++;
      if(i>exp_list_eyes.size())
        i=0;
      present_exp.eyes=tostring(i);
      break;
    case 4: // cejas
      i=atoi(present_exp.browns.c_str());
      i++;
      if(i>exp_list_browns.size())
        i=0;
      present_exp.browns=tostring(i);
      break;
    case 5: // nariz
      i=atoi(present_exp.nose.c_str());
      i++;
      if(i>exp_list_noses.size())
        i=0;
      present_exp.nose=tostring(i);
      break;
    case 6: // orejas
      i=atoi(present_exp.ears.c_str());
      i++;
      if(i>exp_list_ears.size())
        i=0;
      present_exp.ears=tostring(i);
      break;
    case 7: // boca
      i=atoi(present_exp.mouth.c_str());
      i++;
      if(i>exp_list_mouths.size())
        i=0;
      present_exp.mouth=tostring(i);
      break;
    case 8: // barba
      i=atoi(present_exp.facial.c_str());
      i++;
      if(i>exp_list_facial.size())
        i=0;
      present_exp.facial=tostring(i);
      break;
    case 9: // pelo
      i=atoi(present_exp.hair.c_str());
      i++;
      if(i>exp_list_hairs.size())
        i=0;
      present_exp.hair=tostring(i);
      break;
    case 10: // tatuaje
      i=atoi(present_exp.tatoo.c_str());
      i++;
      if(i>exp_list_tatoos.size())
        i=0;
      present_exp.tatoo=tostring(i);
      break;
    case 11: // ropa
      i=atoi(present_exp.clothes.c_str());
      i++;
      if(i>exp_list_clothes.size())
        i=0;
      present_exp.clothes=tostring(i);
      break;
  }
}

void check_click_exp(int x, int y)
{
  x=x/screen_multiplier;
  y=y/screen_multiplier;

  int exp_line=0;
  int exp_file=0;

  if(y>=35 && y<(35+25*5))
    exp_line=1+((y-35)/25);

  if(x>=35 && x<(35+25*10))
    exp_file=1+((x-35)/25);

  if((exp_line-1)*10+(exp_file-1)<exp_desc1.size())
  {
    exp_line_cursor=exp_line;
    exp_file_cursor=exp_file;
  }
}

void check_key(int x, int y)
{
  x=x/screen_multiplier;
  y=y/screen_multiplier;

  std::string l1;
  std::string l2;
  std::string l3;
  std::string l4;

  l1="1234567890 ";
  if(key_caps || key_lock)
  {
    l2="QWERTYUIOP";
    l3="ASDFGHJKL";
    l4="ZXCVBNM";
  }
  else
  {
    l2="qwertyuiop";
    l3="asdfghjkl";
    l4="zxcvbnm";
  }

  int key_line=0;
  int key_file=0;

  if(y>=115 && y<(115+25*4))
    key_line=1+((y-115)/25);

  if(key_line==1 || key_line==3)
  {
    if(x>=25 && x<(25+11*25))
      key_file=1+((x-25)/25);
  }
  else if(key_line==2 || key_line==4)
  {
    if(x>=37 && x<(37+11*25))
      key_file=1+((x-37)/25);
  }

  switch(key_line)
  {
    case 1:
      if(key_file>0 && key_file<11 && present_exp.name.size()<15)
      {
        present_exp.name+=l1.substr(key_file-1,1);
        key_caps=FALSE;
        last_key_line=key_line;
        last_key_file=key_file;
        last_key_time=8;
        key_file_cursor=key_file;
        key_line_cursor=key_line;
      }
      else if(key_file==11 && present_exp.name.size()>0)
      {
        present_exp.name=present_exp.name.substr(0,present_exp.name.size()-1);
        key_file_cursor=key_file;
        key_line_cursor=key_line;
      }
      break;
    case 2:
      if(key_file>0 && key_file<11 && present_exp.name.size()<15)
      {
        present_exp.name+=l2.substr(key_file-1,1);
        key_caps=FALSE;
        last_key_line=key_line;
        last_key_file=key_file;
        last_key_time=8;
        key_file_cursor=key_file;
        key_line_cursor=key_line;
      }
      break;
    case 3:
      if(key_file>1 && key_file<11 && present_exp.name.size()<15)
      {
        present_exp.name+=l3.substr(key_file-2,1);
        key_caps=FALSE;
        last_key_line=key_line;
        last_key_file=key_file;
        last_key_time=8;
        key_file_cursor=key_file;
        key_line_cursor=key_line;
      }
      else if(key_file==1)
      {
        key_lock=!key_lock;
        key_file_cursor=key_file;
        key_line_cursor=key_line;
      }
      else if(key_file==11)
      {
        show_keyboard=FALSE;
        key_file_cursor=key_file;
        key_line_cursor=key_line;
      }
      break;
    case 4:
      if(key_file>1 && key_file<9 && present_exp.name.size()<15)
      {
        present_exp.name+=l4.substr(key_file-2,1);
        key_caps=FALSE;
        last_key_line=key_line;
        last_key_file=key_file;
        last_key_time=8;
        key_file_cursor=key_file;
        key_line_cursor=key_line;
      }
      else if(key_file==9)
      {
        present_exp.name+=" ";
        key_file_cursor=key_file;
        key_line_cursor=key_line;
      }
      else if(key_file==1)
      {
        key_caps=!key_caps;
        key_file_cursor=key_file;
        key_line_cursor=key_line;
      }
      break;
  }
}

void check_menu()
{
  std::string dname;
  switch(menu_show)
  {
    case 1:
      switch(menu_sel)
      {
        case 0:
          make_shot=4;
          menu_show=0;
          break;
        case 1:
          menu_show=2;
          break;
        case 2:
          show_message=3;
          menu_show=0;
          break;
      }
      break;
    case 2:
      switch(menu_sel)
      {
        case 0:
          menu_show=0;
          dname=std::string("rm -R ")+exp_root+exp_list[active_exp]->directory;
          std::system(dname.c_str());
          active_exp--;
          if(active_exp<0)
            active_exp=0;
          init_game();
          break;
        case 1:
          menu_show=0;
          break;
      }
      break;
    case 3:
      switch(menu_sel)
      {
        case 0:
          make_shot=4;
          menu_show=0;
          break;
        case 1:
          menu_show=4;
          break;
        case 2:
          show_message=3;
          menu_show=0;
          break;
      }
      break;
    case 4:
      switch(menu_sel)
      {
        case 0:
          menu_show=0;
          dname=std::string("rm -R ")+exp_root+exp_list[active_exp]->directory+"/"+list_games[active_game];
          std::system(dname.c_str());
          update_experience();
          load_game_ranking();
          active_game--;
          if(active_game<0)
            active_game=0;
          load_games();
          if(list_games.size()==0)
          {
            program_mode=MODE_SELECT_EXP;
            break;
          }
          load_game_summary(exp_game1);
          load_game_experience(exp_experience1);
          load_desc();
          break;
          break;
        case 1:
          menu_show=0;
          break;
      }
      break;
  }
}

void save_exp()
{
  if(present_exp.directory.size()>0)
  {
    std::string f=exp_root+present_exp.directory+"/exp.cfg";
    FILE* expfile;
    expfile=fopen(f.c_str(),"wb");
    if(expfile)
    {
      char cfgopt[100];
      sprintf(cfgopt,"lang=%s\15\12",present_exp.lang.c_str());
      std::string opt=std::string(cfgopt);
      fwrite(opt.c_str(), opt.size(), 1, expfile);
      sprintf(cfgopt,"name=%s\15\12",present_exp.name.c_str());
      opt=std::string(cfgopt);
      fwrite(opt.c_str(), opt.size(), 1, expfile);
      sprintf(cfgopt,"base=%s\15\12",present_exp.base.c_str());
      opt=std::string(cfgopt);
      fwrite(opt.c_str(), opt.size(), 1, expfile);
      sprintf(cfgopt,"eyes=%s\15\12",present_exp.eyes.c_str());
      opt=std::string(cfgopt);
      fwrite(opt.c_str(), opt.size(), 1, expfile);
      sprintf(cfgopt,"browns=%s\15\12",present_exp.browns.c_str());
      opt=std::string(cfgopt);
      fwrite(opt.c_str(), opt.size(), 1, expfile);
      sprintf(cfgopt,"nose=%s\15\12",present_exp.nose.c_str());
      opt=std::string(cfgopt);
      fwrite(opt.c_str(), opt.size(), 1, expfile);
      sprintf(cfgopt,"ears=%s\15\12",present_exp.ears.c_str());
      opt=std::string(cfgopt);
      fwrite(opt.c_str(), opt.size(), 1, expfile);
      sprintf(cfgopt,"mouth=%s\15\12",present_exp.mouth.c_str());
      opt=std::string(cfgopt);
      fwrite(opt.c_str(), opt.size(), 1, expfile);
      sprintf(cfgopt,"facial=%s\15\12",present_exp.facial.c_str());
      opt=std::string(cfgopt);
      fwrite(opt.c_str(), opt.size(), 1, expfile);
      sprintf(cfgopt,"hair=%s\15\12",present_exp.hair.c_str());
      opt=std::string(cfgopt);
      fwrite(opt.c_str(), opt.size(), 1, expfile);
      sprintf(cfgopt,"tatoo=%s\15\12",present_exp.tatoo.c_str());
      opt=std::string(cfgopt);
      fwrite(opt.c_str(), opt.size(), 1, expfile);
      sprintf(cfgopt,"clothes=%s\15\12",present_exp.clothes.c_str());
      opt=std::string(cfgopt);
      fwrite(opt.c_str(), opt.size(), 1, expfile);

      fclose(expfile);
    }
  }
}

void load_game_ranking()
{
  list_favorites.clear();
  list_completes.clear();

  if(exp_list.size()==0)
    return;

  std::ifstream cfgfile;
  std::string exp_file=exp_root+exp_list[active_exp]->directory+std::string("/exp.ranking");
  cfgfile.open(exp_file.c_str());

  if(cfgfile)
  {
    char buffer[1024];
    std::string bfline;
    std::string parm;
    std::string value;
    while(cfgfile.getline(buffer,1024))
    {
      bfline=buffer;
      unsigned int i=bfline.find("=",1);
      parm=bfline.substr(0,i);
      if(i<=parm.size())
        value=bfline.substr(i);
      while(value.substr(value.size()-1,1)<=" ")
        value=value.substr(0,value.size()-1);

      if(parm==std::string("fav1") && value.size()>0)
      {
        game_value g;
        g.directory=value.substr(1);
        list_favorites.push_back(g);
      }
      if(parm==std::string("fav2") && value.size()>0)
      {
        game_value g;
        g.directory=value.substr(1);
        list_favorites.push_back(g);
      }
      if(parm==std::string("fav3") && value.size()>0)
      {
        game_value g;
        g.directory=value.substr(1);
        list_favorites.push_back(g);
      }
      if(parm==std::string("fav4") && value.size()>0)
      {
        game_value g;
        g.directory=value.substr(1);
        list_favorites.push_back(g);
      }
      if(parm==std::string("fav5") && value.size()>0)
      {
        game_value g;
        g.directory=value.substr(1);
        list_favorites.push_back(g);
      }
      if(parm==std::string("cmp1") && value.size()>0)
      {
        game_value g;
        g.directory=value.substr(1);
        list_completes.push_back(g);
      }
      if(parm==std::string("cmp2") && value.size()>0)
      {
        game_value g;
        g.directory=value.substr(1);
        list_completes.push_back(g);
      }
      if(parm==std::string("cmp3") && value.size()>0)
      {
        game_value g;
        g.directory=value.substr(1);
        list_completes.push_back(g);
      }
      if(parm==std::string("cmp4") && value.size()>0)
      {
        game_value g;
        g.directory=value.substr(1);
        list_completes.push_back(g);
      }
      if(parm==std::string("cmp5") && value.size()>0)
      {
        game_value g;
        g.directory=value.substr(1);
        list_completes.push_back(g);
      }
    }
    cfgfile.close();
  }

  for(unsigned int f=0; f<5; f++)
  {
    if(exp_favorites[f])
      SDL_FreeSurface(exp_favorites[f]);
    exp_favorites[f]=NULL;
  }
  for(unsigned int f=0; f<5; f++)
  {
    if(exp_completes[f])
      SDL_FreeSurface(exp_completes[f]);
    exp_completes[f]=NULL;
  }

  SDL_Surface* tmpsurface;
  SDL_Rect rect;
  for(int f=0; f<list_favorites.size(); f++)
  {
    std::string file=exp_root+exp_list[active_exp]->directory+"/"+list_favorites[f].directory+"/exp.icon";
    tmpsurface=SDL_LoadBMP(file.c_str());
    if(tmpsurface)
    {
      rect.x=0;
      rect.y=0;
      rect.w=tmpsurface->w;
      rect.h=tmpsurface->h;
      exp_favorites[f]=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
      SDL_BlitSurface(tmpsurface,&rect,exp_favorites[f],NULL);
      SDL_SetColorKey(exp_favorites[f],SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
      SDL_FreeSurface(tmpsurface);
    }
  }
  for(int f=0; f<list_completes.size(); f++)
  {
    std::string file=exp_root+exp_list[active_exp]->directory+"/"+list_completes[f].directory+"/exp.icon";
    tmpsurface=SDL_LoadBMP(file.c_str());
    if(tmpsurface)
    {
      rect.x=0;
      rect.y=0;
      rect.w=tmpsurface->w;
      rect.h=tmpsurface->h;
      exp_completes[f]=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
      SDL_BlitSurface(tmpsurface,&rect,exp_completes[f],NULL);
      SDL_SetColorKey(exp_completes[f],SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
      SDL_FreeSurface(tmpsurface);
    }
  }
}

void save_game_ranking()
{
  std::string f=exp_root+exp_list[active_exp]->directory+"/exp.ranking";
  FILE* expfile;
  expfile=fopen(f.c_str(),"wb");
  if(expfile)
  {
    char cfgopt[100];
    if(list_favorites.size())
    {
      int max=list_favorites.size();
      if(max>5)
        max=5;
      for(int f=0; f<max; f++)
      {
        sprintf(cfgopt,"fav%i=%s\15\12",f+1,list_favorites[f].directory.c_str());
        std::string opt=std::string(cfgopt);
        fwrite(opt.c_str(), opt.size(), 1, expfile);
      }
    }

    if(list_completes.size())
    {
      int max=list_completes.size();
      if(max>5)
        max=5;
      for(int f=0; f<max; f++)
      {
        sprintf(cfgopt,"cmp%i=%s\15\12",f+1,list_completes[f].directory.c_str());
        std::string opt=std::string(cfgopt);
        fwrite(opt.c_str(), opt.size(), 1, expfile);
      }
    }
    fclose(expfile);
  }
}

void modify_exp()
{
  if(present_exp.directory.size()==0)
  {
    present_exp.directory=todirectory(present_exp.name);
    struct exp *e=new struct exp;
    exp_list.push_back(e);
    active_exp=exp_list.size()-1;
    exp_list[active_exp]->directory=present_exp.directory;
    std::string full_directory=exp_root+present_exp.directory;
#ifdef PLATFORM_GP2X
    mkdir(full_directory.c_str(),0);
#endif
#ifdef PLATFORM_WIN
    mkdir(full_directory.c_str());
#endif
    exp_list[active_exp]->won=0;
    exp_list[active_exp]->won_max=0;
    exp_list[active_exp]->total=0;
    exp_list[active_exp]->total_max=0;
  }
  exp_list[active_exp]->name=present_exp.name;
  exp_list[active_exp]->lang_id=present_exp.lang_id;
  exp_list[active_exp]->lang=present_exp.lang;
  exp_list[active_exp]->base=present_exp.base;
  exp_list[active_exp]->eyes=present_exp.eyes;
  exp_list[active_exp]->browns=present_exp.browns;
  exp_list[active_exp]->nose=present_exp.nose;
  exp_list[active_exp]->ears=present_exp.ears;
  exp_list[active_exp]->mouth=present_exp.mouth;
  exp_list[active_exp]->facial=present_exp.facial;
  exp_list[active_exp]->hair=present_exp.hair;
  exp_list[active_exp]->tatoo=present_exp.tatoo;
  exp_list[active_exp]->clothes=present_exp.clothes;

  save_exp();
}

///////////////////////////////////
/*  Dibuja la creacion del exp   */
///////////////////////////////////
int update_create_exp()
{
  // siempre hay que procesar la cola de eventos, o no se podrán
  // comprobar los botones
  int i;
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
      case SDL_MOUSEBUTTONUP:
        switch (event.button.button)
        {
          case SDL_BUTTON_LEFT:
            if(show_keyboard)
              check_key(event.button.x,event.button.y);
            break;
        }
        break;
#ifdef PLATFORM_GP2X
      case SDL_JOYBUTTONDOWN:
        if(show_message)
        {
          show_message=0;
          break;
        }
        switch (event.jbutton.button)
        {
          case GP2X_BUTTON_UP:
            if(show_keyboard)
            {
              if(key_line_cursor>1)
                key_line_cursor--;
              if(key_line_cursor==2 && key_file_cursor>10)
                key_file_cursor=10;
            }
            else
              if(selected_attribute>0)
                selected_attribute-=1;
            break;
          case GP2X_BUTTON_DOWN:
            if(show_keyboard)
            {
              if(key_line_cursor<4)
                key_line_cursor++;
              if(key_line_cursor==2 && key_file_cursor>10)
                key_file_cursor=10;
              if(key_line_cursor==4 && key_file_cursor>9)
                key_file_cursor=9;
            }
            else
              if(selected_attribute<11)
                selected_attribute+=1;
            break;
          case GP2X_BUTTON_LEFT:
            if(show_keyboard)
            {
              if(key_file_cursor>1)
                key_file_cursor--;
            }
            else
              change_attribute_left();
            break;
          case GP2X_BUTTON_RIGHT:
            if(show_keyboard)
            {
              if(key_line_cursor==1 || key_line_cursor==3)
              {
                if(key_file_cursor<11)
                  key_file_cursor++;
              }
              else if(key_line_cursor==2)
              {
                if(key_file_cursor<10)
                  key_file_cursor++;
              }
              else if(key_line_cursor==4)
              {
                if(key_file_cursor<9)
                  key_file_cursor++;
              }
            }
            else
              change_attribute_right();
            break;
          case GP2X_BUTTON_Y:
            show_keyboard=FALSE;
            program_mode=MODE_SELECT_EXP;
            break;
          case GP2X_BUTTON_X:
            if(show_keyboard)
            {
              int py=113+12+(key_line_cursor-1)*25;
              int px=(key_file_cursor-1)*25;
              if(key_line_cursor==1 || key_line_cursor==3)
                px+=37;
              else
                px+=50;
              px*=screen_multiplier;
              py*=screen_multiplier;
              check_key(px,py);
            }
            else
            {
              if(present_exp.name.size()>0)
              {
                modify_exp();
                show_keyboard=FALSE;
                program_mode=MODE_SELECT_EXP;
              }
              else
                show_message=6;
            }
            break;
          case GP2X_BUTTON_A:
          case GP2X_BUTTON_B:
            break;
        }
#endif
#ifdef PLATFORM_WIN
      case SDL_KEYDOWN:
        if(show_message)
        {
          show_message=0;
          break;
        }
        switch (event.key.keysym.sym)
        {
          case SDLK_UP:
            if(show_keyboard)
            {
              if(key_line_cursor>1)
                key_line_cursor--;
              if(key_line_cursor==2 && key_file_cursor>10)
                key_file_cursor=10;
            }
            else
              if(selected_attribute>0)
                selected_attribute-=1;
            break;
          case SDLK_DOWN:
            if(show_keyboard)
            {
              if(key_line_cursor<4)
                key_line_cursor++;
              if(key_line_cursor==2 && key_file_cursor>10)
                key_file_cursor=10;
              if(key_line_cursor==4 && key_file_cursor>9)
                key_file_cursor=9;
            }
            else
              if(selected_attribute<11)
                selected_attribute+=1;
            break;
          case SDLK_LEFT:
            if(show_keyboard)
            {
              if(key_file_cursor>1)
                key_file_cursor--;
            }
            else
              change_attribute_left();
            break;
          case SDLK_RIGHT:
            if(show_keyboard)
            {
              if(key_line_cursor==1 || key_line_cursor==3)
              {
                if(key_file_cursor<11)
                  key_file_cursor++;
              }
              else if(key_line_cursor==2)
              {
                if(key_file_cursor<10)
                  key_file_cursor++;
              }
              else if(key_line_cursor==4)
              {
                if(key_file_cursor<9)
                  key_file_cursor++;
              }
            }
            else
              change_attribute_right();
            break;
          case SDLK_y:
            show_keyboard=FALSE;
            program_mode=MODE_SELECT_EXP;
            break;
          case SDLK_x:
            if(show_keyboard)
            {
              int py=125+(key_line_cursor-1)*25;
              int px=(key_file_cursor-1)*25;
              if(key_line_cursor==1 || key_line_cursor==3)
                px+=37;
              else
                px+=50;
              px*=screen_multiplier;
              py*=screen_multiplier;
              check_key(px,py);
            }
            else
            {
              if(present_exp.name.size()>0)
              {
                modify_exp();
                show_keyboard=FALSE;
                program_mode=MODE_SELECT_EXP;
              }
              else
                show_message=6;
            }
            break;
          case SDLK_a:
          case SDLK_b:
            break;
        }
#endif
    }
  }

  // dibuja los datos del sistema
  draw_data();

  // dibuja fondo
  SDL_Rect dest;
  dest.x=50;
  dest.y=60;
  dest.w=88;
  dest.h=105;
  draw_boxborder(&dest,SDL_MapRGB(screen->format,255,199,140),SDL_MapRGB(screen->format,255,255,255));
  dest.x=136;
  dest.y=24;
  dest.w=156;
  dest.h=181;
  draw_boxborder(&dest,SDL_MapRGB(screen->format,255,199,140),SDL_MapRGB(screen->format,0,0,0));

  // dibuja el exp en edicion
  draw_exp(&present_exp);

  // dibuja los textos
  int pos_x=140;
  int pos_y=24;
  std::string attribute_name=std::string(lang.get_string(9))+":";
  draw_text(screen,(char*)attribute_name.c_str(),pos_x,pos_y,0,0,0);
  draw_text(screen,(char*)present_exp.name.c_str(),pos_x+60,pos_y,0,0,0);
  pos_y+=15;
  attribute_name=std::string(lang.get_string(10))+":";
  draw_text(screen,(char*)attribute_name.c_str(),pos_x,pos_y,0,0,0);
  draw_text(screen,(char*)present_exp.lang.c_str(),pos_x+60,pos_y,0,0,0);
  pos_y+=15;
  attribute_name=std::string(lang.get_string(11))+":";
  i=atoi(present_exp.base.c_str());
  if(i>0)
  {
    draw_text(screen,(char*)attribute_name.c_str(),pos_x,pos_y,0,0,0);
    draw_text(screen,(char*)present_exp.base.c_str(),pos_x+60,pos_y,0,0,0);
  }
  else
    draw_text(screen,(char*)attribute_name.c_str(),pos_x,pos_y,128,128,128);
  pos_y+=15;
  attribute_name=std::string(lang.get_string(12))+":";
  i=atoi(present_exp.eyes.c_str());
  if(i>0)
  {
    draw_text(screen,(char*)attribute_name.c_str(),pos_x,pos_y,0,0,0);
    draw_text(screen,(char*)present_exp.eyes.c_str(),pos_x+60,pos_y,0,0,0);
  }
  else
    draw_text(screen,(char*)attribute_name.c_str(),pos_x,pos_y,128,128,128);
  pos_y+=15;
  attribute_name=std::string(lang.get_string(13))+":";
  i=atoi(present_exp.browns.c_str());
  if(i>0)
  {
    draw_text(screen,(char*)attribute_name.c_str(),pos_x,pos_y,0,0,0);
    draw_text(screen,(char*)present_exp.browns.c_str(),pos_x+60,pos_y,0,0,0);
  }
  else
    draw_text(screen,(char*)attribute_name.c_str(),pos_x,pos_y,128,128,128);
  pos_y+=15;
  attribute_name=std::string(lang.get_string(14))+":";
  i=atoi(present_exp.nose.c_str());
  if(i>0)
  {
    draw_text(screen,(char*)attribute_name.c_str(),pos_x,pos_y,0,0,0);
    draw_text(screen,(char*)present_exp.nose.c_str(),pos_x+60,pos_y,0,0,0);
  }
  else
    draw_text(screen,(char*)attribute_name.c_str(),pos_x,pos_y,128,128,128);
  pos_y+=15;
  attribute_name=std::string(lang.get_string(15))+":";
  i=atoi(present_exp.ears.c_str());
  if(i>0)
  {
    draw_text(screen,(char*)attribute_name.c_str(),pos_x,pos_y,0,0,0);
    draw_text(screen,(char*)present_exp.ears.c_str(),pos_x+60,pos_y,0,0,0);
  }
  else
    draw_text(screen,(char*)attribute_name.c_str(),pos_x,pos_y,128,128,128);
  pos_y+=15;
  attribute_name=std::string(lang.get_string(16))+":";
  i=atoi(present_exp.mouth.c_str());
  if(i>0)
  {
    draw_text(screen,(char*)attribute_name.c_str(),pos_x,pos_y,0,0,0);
    draw_text(screen,(char*)present_exp.mouth.c_str(),pos_x+60,pos_y,0,0,0);
  }
  else
    draw_text(screen,(char*)attribute_name.c_str(),pos_x,pos_y,128,128,128);
  pos_y+=15;
  attribute_name=std::string(lang.get_string(17))+":";
  i=atoi(present_exp.facial.c_str());
  if(i>0)
  {
    draw_text(screen,(char*)attribute_name.c_str(),pos_x,pos_y,0,0,0);
    draw_text(screen,(char*)present_exp.facial.c_str(),pos_x+60,pos_y,0,0,0);
  }
  else
    draw_text(screen,(char*)attribute_name.c_str(),pos_x,pos_y,128,128,128);
  pos_y+=15;
  attribute_name=std::string(lang.get_string(18))+":";
  i=atoi(present_exp.hair.c_str());
  if(i>0)
  {
    draw_text(screen,(char*)attribute_name.c_str(),pos_x,pos_y,0,0,0);
    draw_text(screen,(char*)present_exp.hair.c_str(),pos_x+60,pos_y,0,0,0);
  }
  else
    draw_text(screen,(char*)attribute_name.c_str(),pos_x,pos_y,128,128,128);
  pos_y+=15;
  attribute_name=std::string(lang.get_string(19))+":";
  i=atoi(present_exp.tatoo.c_str());
  if(i>0)
  {
    draw_text(screen,(char*)attribute_name.c_str(),pos_x,pos_y,0,0,0);
    draw_text(screen,(char*)present_exp.tatoo.c_str(),pos_x+60,pos_y,0,0,0);
  }
  else
    draw_text(screen,(char*)attribute_name.c_str(),pos_x,pos_y,128,128,128);
  pos_y+=15;
  attribute_name=std::string(lang.get_string(20))+":";
  i=atoi(present_exp.clothes.c_str());
  if(i>0)
  {
    draw_text(screen,(char*)attribute_name.c_str(),pos_x,pos_y,0,0,0);
    draw_text(screen,(char*)present_exp.clothes.c_str(),pos_x+60,pos_y,0,0,0);
  }
  else
    draw_text(screen,(char*)attribute_name.c_str(),pos_x,pos_y,128,128,128);

  // dibuja la barra de seleccion
  SDL_Rect s_bar;
  int posx=24+selected_attribute*15;
  if(select_bar_posx<posx)
  {
    select_bar_posx+=select_bar_accel;
    if(select_bar_accel<4)
      select_bar_accel*=2;
    if(select_bar_posx>=posx)
    {
      select_bar_posx=posx;
      select_bar_accel=1;
    }
  }
  if(select_bar_posx>posx)
  {
    select_bar_posx-=select_bar_accel;
    if(select_bar_accel<4)
      select_bar_accel*=2;
    if(select_bar_posx<=posx)
    {
      select_bar_posx=posx;
      select_bar_accel=1;
    }
  }
  s_bar.x=134;
  s_bar.y=select_bar_posx;
  if(select_bar)
    SDL_BlitSurface(select_bar,NULL,screen,&s_bar);

  // mensajes de ayuda
  dest.h=0;
  dest.w=0;
  dest.x=24;
  dest.y=220;
  if(button_pad2)
    SDL_BlitSurface(button_pad2,NULL,screen,&dest);
  draw_text(screen,lang.get_string(2),45,220,255,255,0);
  dest.x=155;
  dest.y=220;
  if(button_x)
    SDL_BlitSurface(button_x,NULL,screen,&dest);
  draw_text(screen,lang.get_string(7),170,220,255,255,0);
  dest.x=220;
  dest.y=220;
  if(button_y)
    SDL_BlitSurface(button_y,NULL,screen,&dest);
  draw_text(screen,lang.get_string(8),235,220,255,255,0);

  // mensajes de ayuda de manejo
  if(show_message)
    draw_message();
}

///////////////////////////////////
/*  Dibuja los detalles del exp  */
///////////////////////////////////
int update_view_details()
{
  // siempre hay que procesar la cola de eventos, o no se podrán
  // comprobar los botones
  std::string dname;
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
      case SDL_MOUSEBUTTONUP:
        if(menu_show)
          break;
        switch (event.button.button)
        {
          case SDL_BUTTON_LEFT:
            check_click_exp(event.button.x,event.button.y);
            break;
        }
        break;
#ifdef PLATFORM_GP2X
      case SDL_JOYBUTTONDOWN:
        if(show_message)
        {
          if(show_message==3)
            done=1;
          show_message=0;
          break;
        }
        switch (event.jbutton.button)
        {
          case GP2X_BUTTON_UP:
            if(menu_show)
            {
              menu_sel--;
              if(menu_sel<0)
                menu_sel=menu_max-1;
              break;
            }
            if((exp_line_cursor-2)*10+(exp_file_cursor-1)<exp_desc1.size() && exp_line_cursor>1)
              exp_line_cursor--;
            break;
          case GP2X_BUTTON_DOWN:
            if(menu_show)
            {
              menu_sel++;
              if(menu_sel>=menu_max)
                menu_sel=0;
              break;
            }
            if(exp_line_cursor*10+(exp_file_cursor-1)<exp_desc1.size() && exp_line_cursor<10)
              exp_line_cursor++;
            break;
          case GP2X_BUTTON_LEFT:
            if(menu_show)
              break;
            if((exp_line_cursor-1)*10+(exp_file_cursor-2)<exp_desc1.size() && exp_file_cursor>1)
              exp_file_cursor--;
            break;
          case GP2X_BUTTON_RIGHT:
            if(menu_show)
              break;
            if((exp_line_cursor-1)*10+exp_file_cursor<exp_desc1.size() && exp_file_cursor<10)
              exp_file_cursor++;
            break;
          case GP2X_BUTTON_R:
            if(menu_show)
              break;
            active_game++;
            if(active_game>=list_games.size())
              active_game=0;
            load_game_summary(exp_game1);
            load_game_experience(exp_experience1);
            load_desc();
            exp_file_cursor=1;
            exp_line_cursor=1;
            Mix_PlayChannel(-1,sound_next,0);
            break;
          case GP2X_BUTTON_L:
            if(menu_show)
              break;
            active_game--;
            if(active_game<0)
              active_game=list_games.size()-1;
            load_game_summary(exp_game1);
            load_game_experience(exp_experience1);
            load_desc();
            exp_file_cursor=1;
            exp_line_cursor=1;
            Mix_PlayChannel(-1,sound_next,0);
            break;
          case GP2X_BUTTON_Y:
            if(menu_show)
              break;
            program_mode=MODE_SELECT_EXP;
            break;
          case GP2X_BUTTON_X:
          case GP2X_BUTTON_B:
            if(menu_show)
              check_menu();
            break;
          case GP2X_BUTTON_START:
            if(!menu_show)
            {
              menu_sel=0;
              menu_show=3;
            }
            else
              menu_show=0;
            break;
        }
#endif
#ifdef PLATFORM_WIN
      case SDL_KEYDOWN:
        if(show_message)
        {
          if(show_message==3)
            done=1;
          show_message=0;
          break;
        }
        switch (event.key.keysym.sym)
        {
          case SDLK_UP:
            if(menu_show)
            {
              menu_sel--;
              if(menu_sel<0)
                menu_sel=menu_max-1;
              break;
            }
            if((exp_line_cursor-2)*10+(exp_file_cursor-1)<exp_desc1.size() && exp_line_cursor>1)
              exp_line_cursor--;
            break;
          case SDLK_DOWN:
            if(menu_show)
            {
              menu_sel++;
              if(menu_sel>=menu_max)
                menu_sel=0;
              break;
            }
            if(exp_line_cursor*10+(exp_file_cursor-1)<exp_desc1.size() && exp_line_cursor<10)
              exp_line_cursor++;
            break;
          case SDLK_LEFT:
            if(menu_show)
              break;
            if((exp_line_cursor-1)*10+(exp_file_cursor-2)<exp_desc1.size() && exp_file_cursor>1)
              exp_file_cursor--;
            break;
          case SDLK_RIGHT:
            if(menu_show)
              break;
            if((exp_line_cursor-1)*10+exp_file_cursor<exp_desc1.size() && exp_file_cursor<10)
              exp_file_cursor++;
            break;
          case SDLK_r:
            if(menu_show)
              break;
            active_game++;
            if(active_game>=list_games.size())
              active_game=0;
            load_game_summary(exp_game1);
            load_game_experience(exp_experience1);
            load_desc();
            exp_file_cursor=1;
            exp_line_cursor=1;
            Mix_PlayChannel(-1,sound_next,0);
            break;
          case SDLK_l:
            if(menu_show)
              break;
            active_game--;
            if(active_game<0)
              active_game=list_games.size()-1;
            load_game_summary(exp_game1);
            load_game_experience(exp_experience1);
            load_desc();
            exp_file_cursor=1;
            exp_line_cursor=1;
            Mix_PlayChannel(-1,sound_next,0);
            break;
          case SDLK_y:
            if(menu_show)
              break;
            program_mode=MODE_SELECT_EXP;
            break;
          case SDLK_RETURN:
          case SDLK_x:
          case SDLK_b:
            if(menu_show)
              check_menu();
            break;
          case SDLK_ESCAPE:
            if(!menu_show)
            {
              menu_sel=0;
              menu_show=3;
            }
            else
              menu_show=0;
            break;
        }
#endif
    }
  }

  // dibuja los datos del sistema
  draw_data();

  // fondo
  SDL_Rect dest;
  dest.x=31;
  dest.y=10;
  dest.w=257;
  dest.h=153;
  //draw_box(&dest,SDL_MapRGB(screen->format,170,67,22));
  draw_box(&dest,SDL_MapRGB(screen->format,255,199,140));
  dest.y=165;
  dest.h=50;
  //draw_box(&dest,SDL_MapRGB(screen->format,170,67,22));
  draw_box(&dest,SDL_MapRGB(screen->format,255,199,140));

  // logros
  dest.x=35;
  dest.y=35;
  for(unsigned int f=0; f<exp_experience1.size(); f++)
  {
    if(exp_icons1[exp_experience1[f].id-1] && exp_experience1[f].won)
      SDL_BlitSurface(exp_icons1[f],NULL,screen,&dest);
    else if(exp_experience1[f].won)
    {
      if(experyw)
        SDL_BlitSurface(experyw,NULL,screen,&dest);
    }
    else
    {
      if(expery)
        SDL_BlitSurface(expery,NULL,screen,&dest);
    }
    dest.x+=25;
    if(dest.x>=285)
    {
      dest.x=35;
      dest.y+=25;
    }
  }

  dest.x=35+(exp_file_cursor-1)*25;
  dest.y=35+(exp_line_cursor-1)*25;
  if(selectkey)
    SDL_BlitSurface(selectkey,NULL,screen,&dest);

  // textos
  std::string tt=title_game1+" ("+author_game1+")";
  draw_text(screen,(char*)tt.c_str(),36,11,255,255,255);
  draw_text(screen,(char*)tt.c_str(),35,10,0,0,0);

  char ex[50];
  char pt[50];
  sprintf(ex,"%s: %i/%i",lang.get_string(22),exp_game1.won,exp_game1.won_max);
  draw_text(screen,ex,35,21,0,0,0);
  sprintf(pt,"%s: %i/%i",lang.get_string(23),exp_game1.total,exp_game1.total_max);
  int tw,th;
  TTF_SizeText(font,pt,&tw,&th);
  draw_text(screen,pt,284-tw,21,0,0,0);

  if(exp_desc1.size()>0)
  {
    int idx=((exp_line_cursor-1)*10)+(exp_file_cursor-1);
    int id_exp=exp_experience1[idx].id;
    int nexp=0;
    int secret=exp_experience1[idx].secret;
    int won=exp_experience1[idx].won;
    for(int f=0; f<exp_desc1.size(); f++)
      if(exp_desc1[f].id==id_exp)
      {
        nexp=f;
        break;
      }

    if(secret && !won)
    {
      draw_text(screen,lang.get_string(24),35,165,0,0,0);
      draw_text(screen,lang.get_string(25),35,185,0,0,0);
    }
    else
    {
     char nm[50];
      sprintf(nm,"%s (%i)",exp_desc1[nexp].name,exp_experience1[idx].value);
      draw_text(screen,nm,35,165,0,0,0);
      draw_text2(screen,exp_desc1[nexp].desc,35,185,0,0,0);
    }
  }

  // si hay que hacer un screenshot, este es el momento
  if(make_shot>0)
  {
    make_shot--;
    if(make_shot==1)
    {
      if(exp_list.size()>0)
      {
        std::string fname=exp_root+exp_list[active_exp]->directory+"_"+list_games[active_game]+std::string(".bmp");
        SDL_SaveBMP(screen,fname.c_str());
      }
    }
    show_message=5;
  }

  // mensajes de ayuda
  dest.x=24;
  dest.y=220;
  if(button_pad2)
    SDL_BlitSurface(button_pad2,NULL,screen,&dest);
  draw_text(screen,lang.get_string(1),45,220,255,255,0);

  if(list_games.size()>1)
  {
    dest.x=105;
    dest.y=220;
    if(button_l)
      SDL_BlitSurface(button_l,NULL,screen,&dest);
    dest.x+=17;
    if(button_r)
      SDL_BlitSurface(button_r,NULL,screen,&dest);
    draw_text(screen,lang.get_string(28),140,220,255,255,0);
  }

  dest.x=220;
  dest.y=220;
  if(button_y)
    SDL_BlitSurface(button_y,NULL,screen,&dest);
  draw_text(screen,lang.get_string(21),235,220,255,255,0);

  // mensajes de ayuda de manejo
  if(show_message)
    draw_message();

  // menus
  draw_menu();
}

///////////////////////////////////
/*  Inicio del programa          */
///////////////////////////////////
int main(int argc, char *argv[])
{
    sprintf(version,"EXP for GP2X-Wiz (2010) by Rafa Vico");

    for(int f=0; f<argc; f++)
    {
      if(std::string(argv[f])=="-fullscreen")
        fullscreen=SDL_FULLSCREEN;
      if(std::string(argv[f])=="-scanlines")
        scanlines=1;
    }

    lang.set_language("English");

    char fps_count[50];
    Uint32 last_time=SDL_GetTicks();
    Uint32 actual_time=last_time;
    Uint32 diff_time=actual_time;
    Uint32 delay_time=0;

    if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO | SDL_INIT_AUDIO)<0)
        return 0;
    SDL_WM_SetCaption("EXP for GP2X-Wiz (2010) by Rafa Vico", "EXP for GP2X-Wiz (2010) by Rafa Vico");

    // La Wiz solo admite una profundidad de 16 bit, como mucho
#ifdef PLATFORM_GP2X
    screen = SDL_SetVideoMode(screen_width*screen_multiplier, screen_height*screen_multiplier, screen_deep*8, SDL_SWSURFACE);
#endif
#ifdef PLATFORM_WIN
    screen2 = SDL_SetVideoMode(screen_width*screen_multiplier, screen_height*screen_multiplier, screen_deep*8, SDL_DOUBLEBUF | SDL_SWSURFACE | fullscreen);
    if (screen2==NULL)
        return 0;
    screen=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, screen_width, screen_height, screen_deep*8, 0,0,0,0);
#endif
    if (screen==NULL)
        return 0;

    joystick=SDL_JoystickOpen(0);
#ifdef PLATFORM_GP2X
    if (joystick==NULL)
        return 0;
#endif

#ifdef PLATFORM_GP2X
    SDL_ShowCursor(0);
#endif

    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, AUDIO_S16, MIX_DEFAULT_CHANNELS, 256);
    Mix_Volume(-1,volume);

    TTF_Init();
    //font=TTF_OpenFont("media/pixantiqua.ttf", 12);
    //font=TTF_OpenFont("media/arial.ttf", 12);
    font=TTF_OpenFont("media/pfcb.ttf", 8);
    font2=TTF_OpenFont("media/pfcb.ttf", 16);

/*if(exp_init("rafavico","prueba2sdl")) ////////////////////////////////////////// PRUEBA DE LIBRERIA EXP
{
  exp_add(1,10);
  exp_add(2,10);
  exp_add(3,5,1);
  exp_add(4,50,1);
  exp_add_desc(1,"Torero!","Haz todo lo que puedas con los cuernos por delante.","Spanish");
  exp_add_desc(2,"Torpeza natural","Cáete y pierde.","Spanish");
  exp_add_desc(3,"Gorrón","Consíguelo todo en un segundo.","Spanish");
  exp_add_desc(4,"Lento","Tarda todo lo que puedas.","Spanish");
  exp_add_desc(1,"Tourer!","Do it everything, with cournament at front.", "English");
  exp_add_desc(2,"Torpeing!","Tropiecing always.", "English");
  exp_add_desc(3,"Gorrón","A pixel value best approximating the given RGB color value for a given pixel format. If the pixel format bpp (color depth) is less than 32-bpp then the unused upper bits of the return value can safely be ignored (e.g., with a 16-bpp format the return value can be assigned to a Uint16, and similarly a Uint8 for an 8-bpp format).","English");
  exp_add_desc(4,"Slow","Tarda todo lo que you want.","English");
  //exp_set_lang("English");
  exp_screen(screen);
  exp_set_callback(&exp_callback);
  exp_win(1);
  exp_win(2);
  exp_win(3);
  exp_win(4);
  exp_callback(1,10,"Titulo","Descripcion muy larga para que pueda aparecer en dos lineas independientes.");
  exp_callback(2,10,"Titulo vacio","");
  exp_callback(3,10,"","Descripcion muy larga para que pueda aparecer en dos lineas independientes.");
  //exp_callback(4,10,"e",NULL);
}*/

#ifdef PLATFORM_WIN
    get_root_path();
#endif
    init_game();
    load_active_exp();

    load_graphics();
    load_sounds();
    //srand(time(NULL));

    diff_time=SDL_GetTicks();
    while (!done)
    {
        SDL_Rect back;
        back.x=0;
        back.y=0;
        SDL_BlitSurface(background,NULL,screen,&back);
        //SDL_FillRect(screen, NULL, 0x000000);       // limpia la pantalla

        switch (program_mode)
        {
          case MODE_SELECT_EXP:
            update_select_exp();
            break;
          case MODE_CREATE_EXP:
            update_create_exp();
            break;
          case MODE_VIEW_DETAILS:
            update_view_details();
            break;
        }

        FPS++;
        actual_time=SDL_GetTicks();
        if ((actual_time-last_time)>=1000)
        {
            sprintf(fps_count,"FPS: %i    delay:%i",FPS,delay_time);
            FPS=0;
            last_time=actual_time;
        }

        delay_time=16-(actual_time-diff_time);
        if (delay_time<=0)
            delay_time=1;

#ifdef PLATFORM_GP2X
        if (delay_time<100)
        {
          delay_time=delay_time/2;
          SDL_Delay(delay_time);
        }
#endif
#ifdef PLATFORM_WIN
        if (delay_time<100)
            SDL_Delay(delay_time);
#endif
        diff_time=SDL_GetTicks();

/*#ifdef PLATFORM_GP2X
        if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_L))
        {
          draw_text(screen,fps_count,2,15,255,255,255);
          draw_text(screen,fps_count,1,14,0,0,0);
        }
#endif
#ifdef PLATFORM_WIN
        if(keys[SDLK_f])
        {
          draw_text(screen,fps_count,2,15,255,255,255);
          draw_text(screen,fps_count,1,14,0,0,0);
        }
#endif*/
        draw_keyboard();


#ifdef PLATFORM_GP2X
        SDL_Flip(screen);
#endif
#ifdef PLATFORM_WIN
        filter_surface(screen,screen2);
        SDL_Flip(screen2);
#endif

/*#ifdef PLATFORM_GP2X
        if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_START) || SDL_JoystickGetButton(joystick, GP2X_BUTTON_SELECT))
        done=TRUE;
#endif
#ifdef PLATFORM_WIN
        if(keys[SDLK_ESCAPE])
          done=1;
#endif*/
        /*if (SDL_JoystickGetButton(joystick, GP2X_BUTTON_CLICK))
            SDL_SaveBMP(screen,"screenshot.bmp");*/
    }

//exp_end();

    // FIN
#ifdef PLATFORM_GP2X
    SDL_FillRect(screen, NULL, 0x000000);
    SDL_Flip(screen);
#endif
#ifdef PLATFORM_WIN
    SDL_FillRect(screen2, NULL, 0x000000);
    SDL_Flip(screen2);
#endif

    if (SDL_JoystickOpened(0))
        SDL_JoystickClose(joystick);

    release_sounds();
    release_graphics();
    SDL_Quit();

    if(exp_list.size()>0)
      save_active_exp();

#ifdef PLATFORM_GP2X
    chdir("/usr/gp2x");
    execl("/usr/gp2x/gp2xmenu", "/usr/gp2x/gp2xmenu", NULL);
#endif
    return 1;
}
