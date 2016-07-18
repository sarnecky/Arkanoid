#define _USE_MATH_DEFINES
#include<math.h>
#include<time.h>
#include<stdio.h>
#include<string.h>
#include<iostream>
#include<fstream>

using namespace std;
extern "C" {
//#ifdef BIT64
//#include"./sdl64/include/SDL.h"
//#include"./sdl64/include/SDL_main.h"
//#else
#include"./sdl/include/SDL.h"
#include"./sdl/include/SDL_main.h"
//#endif
}

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
#define X 200 //poczatkowe polozenie paletki
#define Y 440 //poczatkowe polozenie paletki
#define LEWY_BRZEG 170
#define PRAWY_BRZEG 578
#define GORNY_BRZEG_Y 55
#define SZER_BRZEGU 30
#define SZER_KLOCKA 28
#define WYS_KLOCKA 16
#define L_ELEMENTOW 13
#define W_PALETKA 90
#define H_PALETKA 10
#define R_KULKA 10
#define PREDKOSC_PALETKI 20
#define NOWEZYCIEPUNKTY 10
// narysowanie napisu txt na powierzchni screen, zaczynaj¹c od punktu (x, y)
// charset to bitmapa 128x128 zawieraj¹ca znaki
void DrawString(SDL_Surface *screen, int x, int y, const char *text,
                SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
		};
	};


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt œrodka obrazka sprite na ekranie
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, double x, double y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
	};


// rysowanie pojedynczego pixela
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color; 
	};


// rysowanie linii o d³ugoœci l w pionie (gdy dx = 0, dy = 1) 
// b¹dŸ poziomie (gdy dx = 1, dy = 0)
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for(int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
		};
	};


// rysowanie prostok¹ta o d³ugoœci boków l i k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
                   Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	};

bool kolizja_z_klockiem(int poz_x_k, int poz_y_k, SDL_Rect r[], int kolor[], int licz_pr)
{

	for (int i = 0; i < licz_pr; i++)
	{		//jezeli kulka dotyka klocka, klocek zostaje usuniety, przez nalozoenie na niego czarnego koloru i wyzerowanie jego dlugosci
		if ((poz_x_k - R_KULKA) >= r[i].x && (poz_x_k + R_KULKA) <= (r[i].x + r[i].w) && (poz_y_k - (r[i].y + r[i].h)) <= R_KULKA)
		{
			kolor[i] = 0;
			r[i].h = 0;
			r[i].w = 0;
			return true;
		}

	}
	return false;
}

int laduj_plansze(SDL_Rect r[], int kolor[], SDL_Surface *screen)
{
	ifstream plansza;
	plansza.open("plansza1.txt");
	char tab[L_ELEMENTOW];
	int licz_pr = 0;
	int x = 190, y = 100;

	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
	int pomaranczowy = SDL_MapRGB(screen->format, 0xFF, 0x80, 0x00);
	int szary = SDL_MapRGB(screen->format, 0xC0, 0xC0, 0xC0);
	int zolty = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0x33);
	int rozowy = SDL_MapRGB(screen->format, 0xFF, 0x99, 0xCC);

	while (!plansza.eof())
	{
		for (int i = 0; i < L_ELEMENTOW; i++)
		{
			plansza >> tab[i];
		}

		for (int i = 0; i < L_ELEMENTOW; i++)
		{
			if (tab[i] != '.'){
				r[licz_pr].x = x;
				r[licz_pr].y = y;
				r[licz_pr].w = SZER_KLOCKA;
				r[licz_pr].h = WYS_KLOCKA;
			}
			if (tab[i] == '.'){
				x += (SZER_KLOCKA + 1);

			}
			else if (tab[i] == 'A')
			{
				//szary kolor
				SDL_FillRect(screen, &r[licz_pr], szary);
				kolor[licz_pr] = szary;
				//DrawRectangle(screen, x, y, SZER_KLOCKA, WYS_KLOCKA, czarny, szary);
				x += (SZER_KLOCKA + 1);
			}
			else if (tab[i] == 'B')
			{
				//pomaranczowy kolor
				SDL_FillRect(screen, &r[licz_pr], pomaranczowy);
				kolor[licz_pr] = pomaranczowy;
				x += (SZER_KLOCKA + 1);
			}
			else if (tab[i] == 'C')
			{
				//zolty kolor
				SDL_FillRect(screen, &r[licz_pr], zolty);
				kolor[licz_pr] = zolty;
				x += (SZER_KLOCKA + 1);
			}
			else if (tab[i] == 'D')
			{
				//niebieski kolor
				SDL_FillRect(screen, &r[licz_pr], niebieski);
				kolor[licz_pr] = niebieski;
				x += (SZER_KLOCKA + 1);
			}
			else if (tab[i] == 'E')
			{
				//rozowy kolor
				SDL_FillRect(screen, &r[licz_pr], rozowy);
				kolor[licz_pr] = rozowy;
				x += (SZER_KLOCKA + 1);
			}
			else if (tab[i] == 'F')
			{
				//zielony kolor
				SDL_FillRect(screen, &r[licz_pr], zielony);
				kolor[licz_pr] = zielony;
				x += (SZER_KLOCKA + 1);
			}
			if (tab[i] != '.')licz_pr++;

		}
		y += WYS_KLOCKA + 2;
		x = 185;
	}
	return licz_pr;
}

int nagrody_losowanie(int licz_pr)
{
	return rand() % licz_pr;
}
void nagroda_tempo(double &speed)
{
	speed = 0.15;
}
void dodaj_punkty(int &punkty, int kolor[], int i, SDL_Surface *screen)
{
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC); 
	int pomaranczowy = SDL_MapRGB(screen->format, 0xFF, 0x80, 0x00);
	int szary = SDL_MapRGB(screen->format, 0xC0, 0xC0, 0xC0);
	int zolty = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0x33);
	int rozowy = SDL_MapRGB(screen->format, 0xFF, 0x99, 0xCC);
	if (kolor[i] == zielony) punkty++;
	else if (kolor[i] == rozowy)punkty += 2;
	else if (kolor[i] == niebieski)punkty += 3;
	else if (kolor[i] == niebieski)punkty += 3;
	else if (kolor[i] == zolty)punkty += 4;
	else if (kolor[i] == pomaranczowy)punkty += 5;
	else if (kolor[i] == szary)punkty += 6;
}
void usuniecie_klocka(int &usuniecie, int &ile_usunac, SDL_Rect r[], int ind_usunietego[])
{
	if (usuniecie == 1)
	{
		for (int i = 0; i < ile_usunac; i++)
		{
			r[ind_usunietego[i]].x = 0;
			r[ind_usunietego[i]].y = 0;
			r[ind_usunietego[i]].h = 0;
			r[ind_usunietego[i]].w = 0;
		}
		usuniecie = 0;
		ile_usunac = 0;
	}
}
// main
#ifdef __cplusplus
extern "C"
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////Main
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////Main
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////Main

int main(int argc, char **argv) {
	srand(time(NULL));
	int t1, t2 , quit, frames, rc, x_paletki = 200, uwolnic_pilke = 0, x_kulki = 0, punkty = 0, zycia = 3, mod=0, liczba_zbitych=0;
	int kolor[182];
	double d_y = 0, d_x = 0, dd_y = 0, dd_x = 0, speed = 0.3, poz_x_k = (X + x_paletki + W_PALETKA / 2), poz_y_k = Y - R_KULKA ;
	double delta, worldTime, fpsTimer, fps, distance, etiSpeed;
	int odbicie = 0;
	double czas = 0;
	double p_y = 0.3;
	int dr_czy = 0;
	int dr_x;
	int dr_y;
	int usuniecie = 0;
	int ile_usunac = 0;
	int ind_usunietego[2];
	double dodatni = 0.5, ujemny = -0.5;
	double speed_x = dodatni, speed_y = ujemny;
	int start = 0, wyjscie = 0;
	int nagrodasp=0;

	SDL_Event event;
	SDL_Surface *screen, *charset, *brzeg_pion, *brzeg_poziom, *dot_bmp, *plus;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Rect r[182];
	SDL_Rect paletka;
	
	if(SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
		}

	// tryb pe³noekranowy
	//rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
	    //                             &window, &renderer);
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
	                                 &window, &renderer);
	if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
		};
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "Szablon do zdania drugiego 2014");


	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
	                              0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
	                           SDL_TEXTUREACCESS_STREAMING,
	                           SCREEN_WIDTH, SCREEN_HEIGHT);


	// wy³¹czenie widocznoœci kursora myszy
	//SDL_ShowCursor(SDL_DISABLE);
	charset = SDL_LoadBMP("./cs8x8.bmp");
	if (charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	SDL_SetColorKey(charset, true, 0x000000);

	brzeg_pion = SDL_LoadBMP("./brzeg_pion.bmp");
	if (brzeg_pion == NULL) {
		printf("SDL_LoadBMP(brzeg_pion.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	brzeg_poziom = SDL_LoadBMP("./brzeg_poziom.bmp");
	if (brzeg_poziom == NULL) {
		printf("SDL_LoadBMP(brzeg_poziom.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	dot_bmp = SDL_LoadBMP("./dot.bmp");
	if (dot_bmp == NULL) {
		printf("SDL_LoadBMP(dot.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	plus = SDL_LoadBMP("./plus.bmp");
	if (plus == NULL) {
		printf("SDL_LoadBMP(plus.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	//usuwanie tla z kropki i plusa
	Uint32 colorkey = SDL_MapRGB(dot_bmp->format, 0xFF, 0xFF, 0xFF);
	Uint32 colorkeyy = SDL_MapRGB(plus->format, 0xFF, 0xFF, 0xFF);
	//Set all pixels of color R 0, G 0xFF, B 0xFF to be transparent
	SDL_SetColorKey(dot_bmp, 1, colorkey);
	SDL_SetColorKey(plus, 1, colorkeyy);


	char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
	int pomaranczowy = SDL_MapRGB(screen->format, 0xFF, 0x80, 0x00);
	int szary = SDL_MapRGB(screen->format, 0xC0, 0xC0, 0xC0);
	int zolty = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0x33);
	int rozowy = SDL_MapRGB(screen->format, 0xFF, 0x99, 0xCC);

	int licz_pr = laduj_plansze(r,kolor, screen);
	t1 = SDL_GetTicks();

	frames = 0;
	fpsTimer = 0;
	fps = 0;
	quit = 0;
	worldTime = 0;
	distance = 0;
	etiSpeed = 1;

	/////////////////////////////////////////////////////////losowanie nagrod
	int dr = nagrody_losowanie(licz_pr); //dodatkowa rozgrywka
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// PETLA G£ÓWNA
	while(!quit) {
		t2 = SDL_GetTicks();

		// w tym momencie t2-t1 to czas w milisekundach,
		// jaki uplyna³ od ostatniego narysowania ekranu
		// delta to ten sam czas w sekundach
		delta = (t2 - t1) * 0.001;
		t1 = t2;
		czas += delta;
		//ekran
		SDL_FillRect(screen, NULL, czarny);

		//rysuje wszystkie klocki
		
		for (int i = 0; i < licz_pr; i++)
		{
			SDL_FillRect(screen, &r[i], kolor[i]);
		}

		//aktualizuje pozycje paletki
	
			paletka.x = X + x_paletki;
			paletka.y = Y;
			paletka.w = W_PALETKA;
			paletka.h = H_PALETKA;
			nagrodasp = 0;

		SDL_FillRect(screen, &paletka, czerwony);

		//liczba zyc
		for (int i = 0; i < zycia; i++)
		{
			DrawRectangle(screen, X + 10 * i, Y+10, 8, 5, czarny, czerwony);
		}

		//rysuje sciany
		DrawSurface(screen, brzeg_poziom, 374, GORNY_BRZEG_Y);
		DrawSurface(screen, brzeg_pion, LEWY_BRZEG , 270 );
		DrawSurface(screen, brzeg_pion, PRAWY_BRZEG, 270);

		//ruch pilka, usuwanie elementow i odbicia od scian i paletki

		if (uwolnic_pilke == 0) //pilka w stanie nieruchomym
		{
			DrawSurface(screen, dot_bmp, paletka.x + W_PALETKA/2, paletka.y-R_KULKA);
		}
		else
		{
			if (odbicie == 0)//LOT PILKI DO GORY
			{
				if (poz_y_k + R_KULKA >= paletka.y) //odbicie od paletki po starcie gry
				{
					    //pilka nadlatuje w lewa strone i odbija w prawa
					odbicie = 0;
					speed_x = ujemny;
					speed_y = ujemny;
				}
				if (poz_x_k - R_KULKA <= LEWY_BRZEG + SZER_BRZEGU / 2) //odbicie od lewej sciany
				{
					odbicie = 0;
					speed_x = dodatni;
					speed_y = ujemny;
				}
				if (poz_x_k + R_KULKA >= PRAWY_BRZEG - SZER_BRZEGU / 2) //odbicie od prawej sciany
				{
					odbicie = 0;
					speed_x = ujemny;
					speed_y = ujemny;
				}
				if (poz_y_k + R_KULKA <= GORNY_BRZEG_Y + SZER_BRZEGU ) //odbicie od gornej sciany
				{
					odbicie = 1;
					if (speed_x == dodatni) //pilka nadlatuje z lewej strony i odbija w prawa
					{
						speed_x = dodatni;
						speed_y = dodatni;
					}	
					if (speed_x == ujemny) //pilka nadlatuje z prawej strony i odbija w lewa
					{
						speed_x = ujemny;
						speed_y = dodatni;
					}
				}

				//Sprawdzam czy pilka uderza w klocek
				ile_usunac = 0;
				for (int i = 0; i < licz_pr; i++)
				{		//jezeli kulka dotyka klocka, klocek zostaje usuniety, przez wyzerowanie jego wspolrzednych x,y,w,h
					if ((poz_x_k + R_KULKA) >= r[i].x && (poz_x_k - R_KULKA) <= (r[i].x + r[i].w) && ((poz_y_k - (r[i].y + r[i].h)) <= R_KULKA ) && poz_y_k > r[i].y) //uderzenie w kolocek z dolu
					{
						
						dodaj_punkty(punkty, kolor, i, screen);

						if (i == dr)
						{
							dr_czy = 1;
							dr_x = r[i].x;
							dr_y = r[i].y;
						}
						
						odbicie = 1;
						liczba_zbitych++;
						usuniecie = 1;
						ind_usunietego[ile_usunac] = i;
						ile_usunac++;
						if (r[i].y + r[i].h >= poz_y_k)//odbicie kulki od boku klocka
						{
							if (speed_x == ujemny)
							{
								speed_x = dodatni;
								speed_y = ujemny;
							}
							else
							{
								speed_x = ujemny;
								speed_y = ujemny;
							}
							odbicie = 0;
						}
						else //odbicie od do³u kolcka
						{
							if (speed_x == ujemny)
							{
								speed_x = ujemny;
								speed_y = dodatni;
							}
							else
							{
								speed_x = dodatni;
								speed_y = dodatni;
							}
						}
					}

				}
				
				//aktualizacja pozycji
				poz_x_k += speed_x;
				poz_y_k += speed_y;
				
			}
			else if (odbicie == 1)//LOT PIKI W DOL
			{
				if (poz_x_k + R_KULKA >= PRAWY_BRZEG - SZER_BRZEGU / 2) //odbicie od prawej sciany
				{
					odbicie = 1;
					speed_x = ujemny;
					speed_y = dodatni;
				}
				if (poz_x_k - R_KULKA <= LEWY_BRZEG + SZER_BRZEGU / 2) //odbicie od lewej sciany
				{
					odbicie = 1;
					speed_x = dodatni;
					speed_y = dodatni;
				}

				if (poz_y_k + R_KULKA >= paletka.y && R_KULKA + poz_x_k >= paletka.x &&  poz_x_k - R_KULKA <= paletka.x + W_PALETKA) //odbicie od paletki
				{
					odbicie = 0;
					if (speed_x == dodatni) //pilka nadlatuje z lewej strony i odbija w prawa
					{
						speed_x = dodatni;
						speed_y = ujemny;
					}
					if (speed_x == ujemny) //pilka nadlatuje z prawej strony i odbija w lewa
					{
						speed_x = ujemny;
						speed_y = dodatni;
					}
				}
				if (poz_y_k + R_KULKA >= Y + paletka.h + 1) //modyfikacja stanu ¿ycia
				{
					//liczba zyc
					zycia--;
					for (int i = 0; i <= zycia ; i++)
					{
						if (i != 0)DrawRectangle(screen, X + 10 * i, Y + 10, 8, 5, czarny, czerwony);
						else DrawRectangle(screen, X + 10 * i, Y + 10, 8, 5, czarny, czarny);
					}
					uwolnic_pilke = 0;
					mod = 1;
					poz_x_k = paletka.x + W_PALETKA / 2;
				    poz_y_k = paletka.y - R_KULKA;
				}
				//uderzenie w klocek po odbicu
				ile_usunac = 0;
				for (int i = 0; i < licz_pr; i++)
				{	
					//jezeli kulka dotyka klocka, klocek zostaje usuniety, przez nalozoenie na niego czarnego koloru i wyzerowanie jego dlugosci
					if ((poz_x_k + R_KULKA) >= r[i].x && (poz_x_k - R_KULKA) <= (r[i].x + r[i].w) && poz_y_k + R_KULKA >= r[i].y ) //uderzenie w kolocek
					{
						if (poz_y_k < r[i].y) //odbicie od gory klocka
						{
							dodaj_punkty(punkty, kolor,i, screen);
							if (i == dr)
							{
								dr_czy = 1;
								dr_x = r[i].x;
								dr_y = r[i].y;
							}
							usuniecie = 1;
							ind_usunietego[ile_usunac] = i;
							ile_usunac++;
							speed_y=ujemny;
							odbicie = 0;
						}
						else if (poz_y_k > r[i].y && r[i].y + r[i].h >= poz_y_k) //odbicie od boku klocka
						{
							dodaj_punkty(punkty, kolor, i, screen);
							if (i == dr)
							{
								dr_czy = 1;
								dr_x = r[i].x;
								dr_y = r[i].y;
							}
							usuniecie = 1;
							ind_usunietego[ile_usunac] = i;
							ile_usunac++;
							if (speed_x == ujemny)
							{
								speed_x = dodatni;
								speed_y = dodatni;
							}
							else
							{
								speed_x = ujemny;
								speed_y = dodatni;
							}
							odbicie = 1;
						}

					}

				}
				poz_x_k += speed_x;
				poz_y_k += speed_y;
			}
		
			DrawSurface(screen, dot_bmp, poz_x_k, poz_y_k);
			usuniecie_klocka(usuniecie, ile_usunac, r, ind_usunietego);
		}
		

		//OBS£UGA NAGRÓD

		//nagroda nowe zycie
		
		if (dr_czy == 1 && (dr_y +p_y) <= Y)
		{
			DrawSurface(screen, plus, dr_x, dr_y + p_y);
			if ((dr_x + 10) >= paletka.x && (dr_x + 10) <= (paletka.x + paletka.w) && dr_y + p_y + 10 >= paletka.y)//ODBICIE OD PALETKI
			{
				zycia++;
				punkty += NOWEZYCIEPUNKTY;
				dr_czy = 0;
			}
			p_y += 0.15;

		}

		
	
//		DrawScreen(screen, plane, ship, charset, worldTime, delta, vertSpeed);

		// naniesienie wyniku rysowania na rzeczywisty ekran
//		SDL_Flip(screen);

		
		// tekst informacyjny
		DrawRectangle(screen, LEWY_BRZEG-SZER_BRZEGU/2, 4, PRAWY_BRZEG-LEWY_BRZEG + SZER_BRZEGU, 36, czerwony, niebieski);
		if (mod == 1)
		{
			sprintf(text, "Przegrales, esc-wyjscie, s-kontynuacja, k-nowa gra");
			DrawString(screen, LEWY_BRZEG - SZER_BRZEGU / 2+1, 10, text, charset);
		}
		else if (liczba_zbitych == licz_pr)
		{
			sprintf(text, "Wygrales");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);
			DrawSurface(screen, dot_bmp, paletka.x + W_PALETKA / 2, paletka.y - R_KULKA);
		}
			
		sprintf(text, "Punkty %d", punkty);
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
//		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);

		// obs³uga zdarzeñ (o ile jakieœ zasz³y)
		
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_KEYDOWN: 
					if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
					else if (event.key.keysym.sym == SDLK_RIGHT && (x_paletki + X) <(PRAWY_BRZEG - SZER_BRZEGU / 2 - paletka.w) ){
						x_paletki += PREDKOSC_PALETKI;
						if (uwolnic_pilke == 0)
						{
							poz_x_k += PREDKOSC_PALETKI;
						}
					}
					else if (event.key.keysym.sym == SDLK_LEFT && (x_paletki + X) > (LEWY_BRZEG + 10)){
						x_paletki -= PREDKOSC_PALETKI;
						if (uwolnic_pilke == 0)
						{
							poz_x_k -= PREDKOSC_PALETKI;
						}
					}
					else if (event.key.keysym.sym == SDLK_s && mod==0) uwolnic_pilke = 1;
					else if (event.key.keysym.sym == SDLK_s && zycia != 0 && mod==1) //kolejna rozgrywka i utrata jednego zycia
					{
						uwolnic_pilke = 1;
						odbicie = 0;
						mod = 0;
					}
					else if (event.key.keysym.sym == SDLK_k && zycia == 0 && mod == 1) //rozgrywka od "zera"
					{
						licz_pr = laduj_plansze(r, kolor, screen);
						uwolnic_pilke = 1;
						odbicie = 0;
						zycia = 3;
						punkty = 0;
						mod = 0;
						liczba_zbitych = 0;
					}
					break;
				case SDL_QUIT:
					quit = 1;
					break;
				};
			};
		
		};
	
	// zwolnienie powierzchni
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_FreeSurface(dot_bmp);
	SDL_FreeSurface(brzeg_pion);
	SDL_FreeSurface(brzeg_poziom);
	SDL_FreeSurface(plus);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	
		


	SDL_Quit();
	return 0;
	};
