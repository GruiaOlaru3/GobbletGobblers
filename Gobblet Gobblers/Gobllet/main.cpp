#include <deque>
#include <iostream>
#include <fstream>
#include <cstring>
#include <stack>
#include <algorithm>
#include <utility>
#include <stdio.h>
//#include <windows.h>
#include <ctime>
#include <cstdlib>
#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <thread>
#include <chrono>
#include <SFML/Audio.hpp>

#pragma warning(disable:4996)

using namespace std;

#define fwrite_var(x,y) fwrite(&x, sizeof(x), 1, y)
#define fread_var(x,y) fread(&x, sizeof(x), 1, y)

const int END_GAME_TIME = 3;

int aux, aux_err;
char aux_str[22];

struct profil_jucator {
	char nume[22];
	int id, victorii, infrangeri;

	profil_jucator(const char* _nume = aux_str, int _id = -1) {
		strcpy(nume, _nume);
		id = _id;
		victorii = 0; infrangeri = 0;
	}

	bool citire_din_fisier(FILE* file) {
		int lungime;
		aux_err = fread_var(lungime, file);
		if (ferror(file) || aux_err != 1)
			return false;
		aux_err = fread(nume, sizeof(char), lungime, file);
		if (ferror(file) || aux_err != lungime)
			return false;
		aux_err = fread_var(id, file);
		if (ferror(file) || aux_err != 1)
			return false;
		aux_err = fread_var(victorii, file);
		if (ferror(file) || aux_err != 1)
			return false;
		aux_err = fread_var(infrangeri, file);
		if (ferror(file) || aux_err != 1)
			return false;
		if (!(1 <= lungime && lungime <= 30 && 0 <= victorii && 0 <= infrangeri))
			return false;
		return true;
	}

	void scriere_in_fisier(FILE* file) {
		int lungime = strlen(nume) + 1;
		fwrite_var(lungime, file);
		fwrite(nume, sizeof(char), lungime, file);
		fwrite_var(id, file);
		fwrite_var(victorii, file);
		fwrite_var(infrangeri, file);
	}
};

struct pion {
public:
	int jucator;
	int dimensiune;
	bool selectat = 0, winner = 0;
};

struct stare_joc {
public:
	int tura_curenta = 1;
	bool joc_salvat = 0;
	int jucatori_id[2];
	deque<pion> pioni_jucatori[2];
	deque<pion> tabla_de_joc[3][3];
	bool citire_din_fisier() {
		FILE* file = fopen("assets/StareJoc", "rb");
		aux_err = fread_var(joc_salvat, file);
		if (ferror(file) || aux_err != 1) {
			goto flag_rau;
		}
		if (!joc_salvat) {
			goto flag_bun;
		}
		aux_err = fread_var(tura_curenta, file);
		if (ferror(file) || aux_err != 1) {
			goto flag_rau;
		}

		aux_err = fread_var(jucatori_id[0], file);
		if (ferror(file) || aux_err != 1) {
			goto flag_rau;
		}
		aux_err = fread_var(aux, file);
		if (ferror(file) || aux_err != 1) {
			goto flag_rau;
		}

		pioni_jucatori[0].resize(aux, pion{ 1, 0 });
		for (auto& pion : pioni_jucatori[0]) {
			aux_err = fread_var(pion.dimensiune, file);
			if (ferror(file) || aux_err != 1) {
				goto flag_rau;
			}
		}

		aux_err = fread_var(jucatori_id[1], file);
		if (ferror(file) || aux_err != 1) {
			goto flag_rau;
		}
		aux_err = fread_var(aux, file);
		if (ferror(file) || aux_err != 1) {
			goto flag_rau;
		}

		pioni_jucatori[1].resize(aux, { 2, 0 });
		for (auto& pion : pioni_jucatori[1]) {
			aux_err = fread_var(pion.dimensiune, file);
			if (ferror(file) || aux_err != 1) {
				goto flag_rau;
			}
		}

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				aux_err = fread_var(aux, file);
				if (ferror(file) || aux_err != 1) {
					goto flag_rau;
				}
				tabla_de_joc[i][j].resize(aux);
				for (auto& pion : tabla_de_joc[i][j]) {
					aux_err = fread_var(pion.jucator, file);
					if (ferror(file) || aux_err != 1) {
						goto flag_rau;
					}
					aux_err = fread_var(pion.dimensiune, file);
					if (ferror(file) || aux_err != 1) {
						goto flag_rau;
					}
				}
			}
		}
	flag_bun:
		fclose(file);
		return true;
	flag_rau:
		fclose(file);
		return false;

	}
	void scriere_in_fisier(bool joc_terminat) {
		FILE* file = fopen("assets/StareJoc", "wb");
		if (joc_terminat) {
			joc_terminat = 0;
			fwrite_var(joc_terminat, file);
			goto flag;
		}
		joc_terminat = 1;
		fwrite_var(joc_terminat, file);

		fwrite_var(tura_curenta, file);

		fwrite_var(jucatori_id[0], file);
		aux = pioni_jucatori[0].size();
		fwrite_var(aux, file);
		for (auto& pion : pioni_jucatori[0]) {
			fwrite_var(pion.dimensiune, file);
		}

		fwrite_var(jucatori_id[1], file);
		aux = pioni_jucatori[1].size();
		fwrite_var(aux, file);
		for (auto& pion : pioni_jucatori[1]) {
			fwrite_var(pion.dimensiune, file);
		}

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				aux = tabla_de_joc[i][j].size();
				fwrite_var(aux, file);
				for (auto& pion : tabla_de_joc[i][j]) {
					fwrite_var(pion.jucator, file);
					fwrite_var(pion.dimensiune, file);
				}
			}
		}
	flag:
		fclose(file);
	}
	void joc_nou(int _jucator1_id, int _jucator2_id) {
		tura_curenta = 1;
		jucatori_id[0] = _jucator1_id;
		jucatori_id[1] = _jucator2_id;
		pioni_jucatori[0].clear();
		pioni_jucatori[1].clear();
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				tabla_de_joc[i][j].clear();
				tabla_de_joc[i][j].push_back({ -(3 * i) - j, 0 });
			}
		}
		for (int i = 3; i > 0; i--) { //creste i pentru piese de marimi noi
			pioni_jucatori[0].push_back({ 1, i });
			pioni_jucatori[0].push_back({ 1, i });
			pioni_jucatori[1].push_back({ 2, i });
			pioni_jucatori[1].push_back({ 2, i });
		}
	}
};

struct pion_selectat {
public:
	int origine = -1;//0 pentru tabla, 1 pentru player 1, 2 pentru player 2
	int pozitie = -1;
	pion* ref = nullptr;
};

struct buton {
public:
	int left = 0, right = 0, up = 0, down = 0;
	bool in(sf::Vector2i p)const {
		return (left <= p.x && p.x <= right && up <= p.y && p.y <= down);
	}
};

const buton meniu_principal_buton_joaca = { 540, 790, 270 , 340 };
const buton meniu_principal_buton_tutorial = { 560, 770, 540, 610 };
const buton meniu_principal_profile_jucatori = { 560, 770, 360, 430 };
const buton meniu_principal_buton_clasament = { 560, 770, 450, 520 };
const buton meniu_principal_buton_limba = { 980, 1140, 70, 170 };
const buton selectie_jucatori_P1_stanga = { 50, 140, 490, 570 };
const buton selectie_jucatori_P1_dreapta = { 450, 540, 490, 570 };
const buton selectie_jucatori_P2_stanga = { 750, 830, 490, 570 };
const buton selectie_jucatori_P2_dreapta = { 1150, 1230, 490, 570 };
const buton selectie_jucatori_joaca = { 520, 765, 595, 660 };
const buton selectie_jucatori_inapoi = { 1110, 1190, 50, 130 };


stack<stare_joc> history;
stare_joc joc_curent;

deque<profil_jucator> lista_de_jucatori;

pion_selectat activ;

int limba = 0;

string nume_boti[2][2] = { "COM_USOR","COM_GREU","COM_EASY","COM_HARD" };

sf::RenderWindow window(sf::VideoMode(1280, 720), "Gobblet Gobblers");
sf::Font font;
sf::Font font_sancreek;

sf::Sprite Meniu_Sprite;
sf::Texture Meniu_Texture[2];

sf::Sprite Selectie_Jucatori_Sprite;
sf::Texture Selectie_Jucatori_Texture[2];

sf::Texture Tutorial_Texture[2];
sf::Sprite Tutorial_Sprite;

sf::Texture Joc_Texture[2];
sf::Sprite Joc_Sprite;

sf::Texture Gata_Joc_Texture[2];
sf::Sprite Gata_Joc_Sprite;

sf::Texture Blank_Texture;

sf::Texture Clasament_Texture[2];
sf::Sprite Clasament_Sprite;

sf::Texture Profile_Texture[2];
sf::Sprite Profile_Sprite;

sf::Texture Slot_Texture;
sf::Sprite Slot_Sprite;

sf::Texture PopupDelete_Texture[2];
sf::Sprite PopupDelete_Sprite;

sf::Texture Creare_Jucator_Texture[2];
sf::Sprite Creare_Jucator_Sprite;

sf::Texture Jucator_Texture[2][2];
sf::Sprite Jucator_Sprite[2];

sf::Texture Continue_Texture[2];
sf::Sprite Continue_Sprite;

sf::Music music;

sf::SoundBuffer Munch_Buffer;
sf::Sound Munch_Sound;

sf::SoundBuffer Error_Buffer;
sf::Sound Error_Sound;

sf::SoundBuffer Click_Buffer;
sf::Sound Click_Sound;

sf::SoundBuffer Move_Buffer;
sf::Sound Move_Sound;


void map_button(buton x) {
	sf::CircleShape circ;
	circ.setRadius(4);
	circ.setOutlineColor(sf::Color::Green);
	circ.setOutlineThickness(2);
	circ.setPosition(sf::Vector2f(x.left, x.up));
	window.draw(circ);
	circ.setPosition(sf::Vector2f(x.left, x.down));
	window.draw(circ);
	circ.setPosition(sf::Vector2f(x.right, x.up));
	window.draw(circ);
	circ.setPosition(sf::Vector2f(x.right, x.down));
	window.draw(circ);
}

//functionalitate backend

bool citire_jucatori_din_fisier() {
	FILE* file = fopen("assets/Profile", "rb");
	int nrJucatori;
	aux_err = fread_var(nrJucatori, file);
	if (ferror(file) || aux_err != 1) {
		goto flag_rau;
	}
	lista_de_jucatori.resize(nrJucatori);
	for (auto& jucator : lista_de_jucatori) {
		aux_err = jucator.citire_din_fisier(file);
		if (aux_err == 0) {
			goto flag_rau;
		}
	}

	fclose(file);
	return true;

flag_rau:
	fclose(file);
	return false;
}

void scriere_jucatori_in_fisier() {
	FILE* file = fopen("assets/Profile", "wb");
	int nrJucatori = lista_de_jucatori.size();
	fwrite_var(nrJucatori, file);
	for (auto& jucator : lista_de_jucatori) {
		jucator.scriere_in_fisier(file);
	}
	fclose(file);
}
void creare_jucator_nou(const char* _nume, int id) {
	lista_de_jucatori.push_back(profil_jucator(_nume, id));
}
void creare_jucator_nou(const char* _nume) {
	if (lista_de_jucatori.size() <= 2) {
		lista_de_jucatori.push_back(profil_jucator(_nume, 1));
	}
	else {
		lista_de_jucatori.push_back(profil_jucator(_nume, lista_de_jucatori.back().id + 1));
	}
}

void stergere_jucator(int _id) {
	if (_id < 0) return;
	for (deque<profil_jucator>::iterator jucator_it = lista_de_jucatori.begin(); jucator_it != lista_de_jucatori.end(); jucator_it++) {
		if (jucator_it->id == _id) {
			lista_de_jucatori.erase(jucator_it);
			break;
		}
	}
}

void modifica_scor(int _id, int win) {
	for (auto& jucator : lista_de_jucatori) {
		if (jucator.id != _id)
			continue;
		if (win == 1)
			jucator.victorii++;
		else
			jucator.infrangeri++;
	}
}

void modifica_nume(int _id, char* nume = aux_str) {
	for (auto& jucator : lista_de_jucatori) {
		if (jucator.id != _id)
			continue;
		strcpy(jucator.nume, nume);
		cout << jucator.nume << endl;
	}
}

void deselecteaza() {
	if (activ.ref)
		activ.ref->selectat = 0;
	activ.origine = -1;
	activ.pozitie = -1;
	activ.ref = nullptr;
}

void scoate_piesa_activa() {
	if (activ.origine == 1) {
		joc_curent.pioni_jucatori[0].erase(joc_curent.pioni_jucatori[0].begin() + activ.pozitie);
	}
	else if (activ.origine == 2) {
		joc_curent.pioni_jucatori[1].erase(joc_curent.pioni_jucatori[1].begin() + activ.pozitie);

	}
	else {
		joc_curent.tabla_de_joc[activ.pozitie / 3][activ.pozitie % 3].pop_back();
	}
	activ.origine = -1;
	activ.pozitie = -1;
	activ.ref = nullptr;
}

bool pune_piesa(pion piesa, int i, int j) {
	piesa.selectat = 0;
	if (piesa.dimensiune <= joc_curent.tabla_de_joc[i][j].back().dimensiune)
		return false;
	joc_curent.tabla_de_joc[i][j].push_back(piesa);
	return true;
}

int verifica_victorie() {

	/*0 nu a castigat nimeni, 1 castigat player 1 2 castigat player 2
	*/
	int i;
	for (i = 0; i < 3; i++) {
		if (joc_curent.tabla_de_joc[i][0].back().jucator == joc_curent.tabla_de_joc[i][1].back().jucator && joc_curent.tabla_de_joc[i][1].back().jucator == joc_curent.tabla_de_joc[i][2].back().jucator && joc_curent.tabla_de_joc[i][2].back().jucator == joc_curent.tura_curenta) {
			joc_curent.tabla_de_joc[i][0].back().winner = 1;
			joc_curent.tabla_de_joc[i][1].back().winner = 1;
			joc_curent.tabla_de_joc[i][2].back().winner = 1;
			return joc_curent.tabla_de_joc[i][0].back().jucator;
		}
		if (joc_curent.tabla_de_joc[0][i].back().jucator == joc_curent.tabla_de_joc[1][i].back().jucator && joc_curent.tabla_de_joc[1][i].back().jucator == joc_curent.tabla_de_joc[2][i].back().jucator && joc_curent.tabla_de_joc[2][i].back().jucator == joc_curent.tura_curenta) {
			joc_curent.tabla_de_joc[0][i].back().winner = 1;
			joc_curent.tabla_de_joc[1][i].back().winner = 1;
			joc_curent.tabla_de_joc[2][i].back().winner = 1;
			return joc_curent.tabla_de_joc[0][i].back().jucator;
		}
	}
	if (joc_curent.tabla_de_joc[0][0].back().jucator == joc_curent.tabla_de_joc[1][1].back().jucator && joc_curent.tabla_de_joc[1][1].back().jucator == joc_curent.tabla_de_joc[2][2].back().jucator && joc_curent.tabla_de_joc[2][2].back().jucator == joc_curent.tura_curenta) {
		joc_curent.tabla_de_joc[0][0].back().winner = 1;
		joc_curent.tabla_de_joc[1][1].back().winner = 1;
		joc_curent.tabla_de_joc[2][2].back().winner = 1;
		return joc_curent.tabla_de_joc[0][0].back().jucator;
	}

	if (joc_curent.tabla_de_joc[0][2].back().jucator == joc_curent.tabla_de_joc[1][1].back().jucator && joc_curent.tabla_de_joc[1][1].back().jucator == joc_curent.tabla_de_joc[2][0].back().jucator && joc_curent.tabla_de_joc[2][0].back().jucator == joc_curent.tura_curenta) {
		joc_curent.tabla_de_joc[0][2].back().winner = 1;
		joc_curent.tabla_de_joc[1][1].back().winner = 1;
		joc_curent.tabla_de_joc[2][0].back().winner = 1;
		return joc_curent.tabla_de_joc[0][2].back().jucator;
	}

	for (i = 0; i < 3; i++) {
		if (joc_curent.tabla_de_joc[i][0].back().jucator == joc_curent.tabla_de_joc[i][1].back().jucator && joc_curent.tabla_de_joc[i][1].back().jucator == joc_curent.tabla_de_joc[i][2].back().jucator && joc_curent.tabla_de_joc[i][2].back().jucator == 3 - joc_curent.tura_curenta) {
			joc_curent.tabla_de_joc[i][0].back().winner = 1;
			joc_curent.tabla_de_joc[i][1].back().winner = 1;
			joc_curent.tabla_de_joc[i][2].back().winner = 1;
			return joc_curent.tabla_de_joc[i][0].back().jucator;
		}
		if (joc_curent.tabla_de_joc[0][i].back().jucator == joc_curent.tabla_de_joc[1][i].back().jucator && joc_curent.tabla_de_joc[1][i].back().jucator == joc_curent.tabla_de_joc[2][i].back().jucator && joc_curent.tabla_de_joc[2][i].back().jucator == 3 - joc_curent.tura_curenta) {
			joc_curent.tabla_de_joc[0][i].back().winner = 1;
			joc_curent.tabla_de_joc[1][i].back().winner = 1;
			joc_curent.tabla_de_joc[2][i].back().winner = 1;
			return joc_curent.tabla_de_joc[0][i].back().jucator;
		}
	}
	if (joc_curent.tabla_de_joc[0][0].back().jucator == joc_curent.tabla_de_joc[1][1].back().jucator && joc_curent.tabla_de_joc[1][1].back().jucator == joc_curent.tabla_de_joc[2][2].back().jucator && joc_curent.tabla_de_joc[2][2].back().jucator == 3 - joc_curent.tura_curenta) {
		joc_curent.tabla_de_joc[0][0].back().winner = 1;
		joc_curent.tabla_de_joc[1][1].back().winner = 1;
		joc_curent.tabla_de_joc[2][2].back().winner = 1;
		return joc_curent.tabla_de_joc[0][0].back().jucator;
	}

	if (joc_curent.tabla_de_joc[0][2].back().jucator == joc_curent.tabla_de_joc[1][1].back().jucator && joc_curent.tabla_de_joc[1][1].back().jucator == joc_curent.tabla_de_joc[2][0].back().jucator && joc_curent.tabla_de_joc[2][0].back().jucator == 3 - joc_curent.tura_curenta) {
		joc_curent.tabla_de_joc[0][2].back().winner = 1;
		joc_curent.tabla_de_joc[1][1].back().winner = 1;
		joc_curent.tabla_de_joc[2][0].back().winner = 1;
		return joc_curent.tabla_de_joc[0][2].back().jucator;
	}

	return 0;
}

//frontend

void delete_jucatori_grafica();
void profile_jucatori_grafica();
void tutorial_grafica();
void clasament_grafica();
void gata_joc_grafica();
void joc_grafica();
void selectie_jucatori_grafica();
void joc_nou_sau_continua_grafica();
void meniu_principal_grafica();
void creare_jucator_grafica();


void delete_jucatori_on_click();
void profile_jucatori_on_click();
void tutorial_on_click();
void clasament_on_click();
void gata_joc_on_click();
void joc_on_click();
void selectie_jucatori_on_click();
void joc_nou_sau_continua_on_click();
void meniu_principal_on_click();
void creare_jucator_on_click();

void load_scene_delete_jucatori();
void load_scene_profile_jucatori(bool);
void load_scene_tutorial();
void load_scene_clasament(bool);
void load_scene_gata_joc();
void load_scene_joc();
void load_scene_selectie_jucatori();
void load_scene_joc_nou_sau_continua();
void load_scene_meniu_principal(bool);
void load_scene_creare_jucator();


void profile_jucatori_on_scroll(int);
void clasament_on_scroll(int);

void creare_jucator_text_entered(char);

void (*on_click)() = meniu_principal_on_click;
void (*grafica)() = meniu_principal_grafica;
void (*on_scroll)(int) = nullptr;
void (*text_entered)(char) = nullptr;

int delta = 0;

void load_scene_delete_jucatori() {
	grafica = delete_jucatori_grafica;
	on_click = delete_jucatori_on_click;
	on_scroll = nullptr;
	text_entered = nullptr;
}

void delete_jucatori_grafica() {
	profile_jucatori_grafica();
	sf::RectangleShape rect;
	rect.setSize(sf::Vector2f(1280, 720));
	rect.setFillColor(sf::Color(0, 0, 0, 160));
	window.draw(rect);
	window.draw(PopupDelete_Sprite);

}

int condamnat = 0;
void delete_jucatori_on_click() {
	auto mouse_poz = sf::Vector2i(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
	buton da = { 410, 600, 410, 460 };
	buton nu = { 670, 850, 410, 460 };
	map_button(da);
	map_button(nu);
	if (da.in(mouse_poz)) {
		Click_Sound.play();
		lista_de_jucatori.erase(lista_de_jucatori.begin() + condamnat);
		delta = max(delta, -((int)lista_de_jucatori.size() - 6));
		delta = min(0, delta);
		goto final;
	}
	if (nu.in(mouse_poz)) {
		Click_Sound.play();
		goto final;
	}
	return;
	final:
	condamnat = 0;
	load_scene_profile_jucatori(0);
}

string buff;
void load_scene_creare_jucator() {
	buff.clear();
	grafica = creare_jucator_grafica;
	on_click = creare_jucator_on_click;
	on_scroll = nullptr;
	text_entered = creare_jucator_text_entered;
}

void creare_jucator_grafica() {
	profile_jucatori_grafica();
	sf::RectangleShape rect;
	rect.setSize(sf::Vector2f(1280, 720));
	rect.setFillColor(sf::Color(0, 0, 0, 160));
	window.draw(rect);
	window.draw(Creare_Jucator_Sprite);
	sf::Text txt;
	txt.setFont(font);
	txt.setScale(1.2, 1.2);
	txt.setPosition(415, 335);
	txt.setOutlineThickness(1);
	txt.setColor(sf::Color::Black);
	auto buff2 = buff;
	txt.setString(buff2);
	while (txt.getLocalBounds().getSize().x * 1.2 > 460) {
		reverse(buff2.begin(), buff2.end());
		buff2.pop_back();
		reverse(buff2.begin(), buff2.end());
		txt.setString(buff2);
	}
	window.draw(txt);
}

void creare_jucator_on_click() {
	auto mouse_poz = sf::Vector2i(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
	buton ok = { 550, 730, 425, 480 };
	if (ok.in(mouse_poz))
	{
		Click_Sound.play();
		creare_jucator_nou(buff.c_str());
		scriere_jucatori_in_fisier();
		load_scene_profile_jucatori(0);
		return;
	}
	buton x = { 920, 980, 215, 275 };
	if (x.in(mouse_poz)) {
		Click_Sound.play();
		load_scene_profile_jucatori(0);
		return;
	}
}

void creare_jucator_text_entered(char ch) {
	if (ch == '\b') {
		Move_Sound.play();
		if (!buff.empty()) buff.pop_back();
		return;
	}
	if (ch == 13) {
		Move_Sound.play();
		scriere_jucatori_in_fisier();
		creare_jucator_nou(buff.c_str());
		load_scene_profile_jucatori(0);
	}

	if (ch < 32 || ch > 126) return;
	if (!buff.empty() && isalpha(buff[0]))
		buff[0] = toupper(buff[0]);
	if (buff.size() < 20) Move_Sound.play(), buff.push_back(ch);
}

void load_scene_profile_jucatori(bool reset) {
	if (reset) delta = 0;
	grafica = profile_jucatori_grafica;
	on_click = profile_jucatori_on_click;
	on_scroll = profile_jucatori_on_scroll;
	text_entered = nullptr;
}
void profile_jucatori_grafica() {
	//listeaza toti jucatorii, la fiecare ai buton in dreapta de sters
	//la final ai buton de creat profil nou

	// Gata

	window.clear();
	window.draw(Profile_Sprite);
	int i = 0, cnt = delta;

	sf::Text txt;
	txt.setFont(font);
	txt.setOutlineThickness(1);
	txt.setScale(1.5, 1.5);
	txt.setColor(sf::Color::Black);
	for (i = 0; i < lista_de_jucatori.size(); i++) {
		if (lista_de_jucatori[i].id < 0) continue;
		cnt++;
		if (cnt <= 0) continue;
		Slot_Sprite.setPosition(365, 70 + cnt * 100);
		window.draw(Slot_Sprite);
		//map_button(ax);

		txt.setString(lista_de_jucatori[i].nume);
		auto x = min(1.5, 430.0 / txt.getLocalBounds().getSize().x);
		txt.setScale(x, x);
		txt.setPosition(385, 120 + 100 * cnt - 30 * x / 2);
		window.draw(txt);
		txt.setScale(1.5, 1.5);


		if (cnt > 3) break;
	}
}

void profile_jucatori_on_click() {
	auto mouse_poz = sf::Vector2i(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
	//if (main menu)
	buton X = { 1070, 1190, 40, 170 };
	int i = 0, cnt = delta;
	buton ax, pls;
	if (X.in(mouse_poz))
	{
		Click_Sound.play();
		load_scene_meniu_principal(0);
		goto final;
	}
	for (i = 0; i < lista_de_jucatori.size(); i++) {
		if (lista_de_jucatori[i].id < 0) continue;
		cnt++;
		if (cnt <= 0) continue;
		if (cnt > 4) break;
		ax = { 840, 920, 85 + cnt * 100, 150 + cnt * 100 };
		if (!ax.in(mouse_poz))
			continue;
		Click_Sound.play();
		condamnat = i;
		load_scene_delete_jucatori();
		//eventual un popup
		goto final;

	}
	pls = { 830, 910, 580, 660 };
	if (pls.in(mouse_poz)) {
		Click_Sound.play();
		buff.clear();
		load_scene_creare_jucator();
		return;
	}

	final:
	return;
}

void profile_jucatori_on_scroll(int val) {
	//GATA
	delta += val;
	delta = max(delta, -((int)lista_de_jucatori.size() - 6));
	delta = min(0, delta);
}

void load_scene_tutorial() {
	grafica = tutorial_grafica;
	on_click = tutorial_on_click;
	on_scroll = nullptr;
	text_entered = nullptr;
}
void tutorial_grafica() {
	//GATA
	window.clear(sf::Color::White);
	window.draw(Tutorial_Sprite);
}

void tutorial_on_click() {
	//GATA
	Click_Sound.play();
	load_scene_meniu_principal(0);
}

bool comp(profil_jucator a, profil_jucator b) {
	// a.win/a.lose compar cu b.win/b.lose
	//nume cu strcmp, <0 false
	if (a.victorii * (b.infrangeri + b.victorii) != (a.infrangeri + a.victorii) * b.victorii) {
		return a.victorii * (b.infrangeri + b.victorii) > (a.infrangeri + a.victorii) * b.victorii;
	}
	return a.victorii + a.infrangeri > b.victorii + b.infrangeri;
}
deque<profil_jucator> lista_sortata;
void load_scene_clasament(bool reset) {
	if (reset) delta = 0;
	lista_sortata = lista_de_jucatori;
	sort(lista_sortata.begin(), lista_sortata.end(), comp);
	grafica = clasament_grafica;
	on_click = clasament_on_click;
	on_scroll = clasament_on_scroll;
	text_entered = nullptr;
}

void clasament_grafica() {
	//GATA

	window.clear(sf::Color::White);
	window.draw(Clasament_Sprite);
	int cnt = delta;
	sf::Text txt;
	txt.setFont(font);
	txt.setColor(sf::Color::Black);
	txt.setOutlineThickness(1);
	txt.setScale(1.5, 1.5);
	for (int i = 0; i < lista_sortata.size(); i++) {
		if (lista_sortata[i].id < 0) continue;
		cnt++;
		if (cnt <= 0) continue;

		txt.setString(lista_sortata[i].nume);
		auto x = min(1.5, 360.0 / txt.getLocalBounds().getSize().x);
		txt.setScale(x, x);
		txt.setPosition(240, 210 + 98 * cnt - 30 * x / 2);
		window.draw(txt);
		txt.setScale(1.5, 1.5);

		txt.setString(to_string(lista_sortata[i].victorii));
		txt.setPosition(650, 190 + 98 * cnt);
		window.draw(txt);

		txt.setString(to_string(lista_sortata[i].infrangeri));
		txt.setPosition(790, 190 + 98 * cnt);
		window.draw(txt);
		string s = to_string(float(lista_sortata[i].victorii) / (lista_sortata[i].infrangeri + lista_sortata[i].victorii) * 100);
		if (lista_sortata[i].infrangeri == 0 && lista_sortata[i].victorii == 0)
			s.clear();
		else
			s.resize(5, ' ');
		txt.setString(s);
		txt.setPosition(910, 190 + 98 * cnt);
		window.draw(txt);

		if (cnt >= 4) break;
	}
}

void clasament_on_click() {
	Click_Sound.play();
	// GATA
	load_scene_meniu_principal(0);
}

void clasament_on_scroll(int val) {
	//GATA
	delta += val;
	delta = max(delta, -((int)lista_de_jucatori.size() - 6));
	delta = min(0, delta);
}

int end_game_id;

int stare = 1;
void load_scene_gata_joc() {
	music.openFromFile("assets/SFX/Victorie.mp3");
	music.setLoop(1);
	music.setVolume(10);
	music.play();
	music.setPosition(0, 0, 0);
	stare = 1;
	grafica = gata_joc_grafica;
	on_click = gata_joc_on_click;
	on_scroll = nullptr;
	text_entered = nullptr;
}
void gata_joc_grafica() {
	joc_grafica();
	if (!stare) return;
	sf::RectangleShape rect;
	rect.setSize(sf::Vector2f(1280, 720));
	rect.setFillColor(sf::Color(0, 0, 0, 160));
	window.draw(rect);
	window.draw(Gata_Joc_Sprite);

	int i;
	string num;
	sf::Text txt;
	txt.setFont(font_sancreek);
	txt.setColor(sf::Color::Black);


	for (i = 0; i < lista_de_jucatori.size(); i++) if (lista_de_jucatori[i].id == end_game_id) num = string(lista_de_jucatori[i].nume);
	txt.setString(num);
	auto xx = txt.getLocalBounds().getSize();
	auto x = min(2.0, 450.0 / xx.x);
	txt.setScale(x, x);
	xx = txt.getLocalBounds().getSize();
	txt.setPosition(645 - xx.x * x / 2, 390 - xx.y * x / 2);

	window.draw(txt);
}


void gata_joc_on_click() {
	Click_Sound.play();
	if (stare) {
		stare = 0;
	}
	else
		load_scene_meniu_principal(1);
}

void load_scene_joc() {
	music.openFromFile("assets/SFX/Joc.mp3");
	music.setLoop(1);
	music.setVolume(10);
	music.play();
	music.setPosition(0, 0, 0);
	Joc_Sprite.setTexture(Joc_Texture[joc_curent.tura_curenta - 1]);
	history.push(joc_curent);
	grafica = joc_grafica;
	on_click = joc_on_click;
	on_scroll = nullptr;
	text_entered = nullptr;
	
}

int tabla_Ox = 397, tabla_Oy = 130, tabla_delt_Ox = 160, tabla_delt_Oy = 150, jucator_X[2] = { 40, 970 }, jucatori_Y = 200, jucatori_delt_Ox = 140, jucatori_delt_Oy = 153;
sf::Vector2f dimens[3] = { {33.0, 59.0}, {47.0, 84.0}, {67.0, 120.0}, }; //{87.0, 130.0}

void joc_grafica() {
	window.clear(sf::Color::White);
	window.draw(Joc_Sprite);
	sf::RectangleShape recti;

	int i;
	string num;
	sf::Text txt;
	txt.setFont(font_sancreek);
	txt.setColor(sf::Color::Black);


	for (i = 0; i < lista_de_jucatori.size(); i++) if (lista_de_jucatori[i].id == joc_curent.jucatori_id[0]) num = string(lista_de_jucatori[i].nume);
	txt.setString(num);
	auto xx = txt.getLocalBounds().getSize();
	auto x = min(1.5, 200.0 / xx.x);
	txt.setScale(x, x);
	xx = txt.getLocalBounds().getSize();
	txt.setPosition(150 - xx.x * x / 2, 110 - xx.y * x / 2);
	window.draw(txt);

	for (i = 0; i < lista_de_jucatori.size(); i++) if (lista_de_jucatori[i].id == joc_curent.jucatori_id[1]) num = string(lista_de_jucatori[i].nume);
	txt.setString(num);
	xx = txt.getLocalBounds().getSize();
	x = min(1.5, 200.0 / xx.x);
	txt.setScale(x, x);
	xx = txt.getLocalBounds().getSize();
	txt.setPosition(1135 - xx.x * x / 2, 102 - xx.y * x / 2);
	window.draw(txt);

	Jucator_Sprite[0].setTexture(Jucator_Texture[0][joc_curent.jucatori_id[0] <= -1], 1);
	Jucator_Sprite[1].setTexture(Jucator_Texture[1][joc_curent.jucatori_id[1] <= -1], 1);

	sf::RectangleShape rect;
	rect.setSize(sf::Vector2f(tabla_delt_Ox, tabla_delt_Oy));
	//rect.setOutlineColor(sf::Color::Black);
	rect.setOutlineThickness(-10);
	auto Blue = sf::Color(0, 0, 255, 128);
	auto Green = sf::Color(0, 255, 0, 128);
	auto Yellow = sf::Color(255, 222, 33, 128);
	auto Black = sf::Color(256, 256, 256, 128);
	auto Red = sf::Color(255, 0, 0, 128);
	auto White = sf::Color(0, 0, 0, 128);
	rect.setFillColor(sf::Color(0, 0, 0, 0));
	int juc = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (joc_curent.tabla_de_joc[i][j].back().selectat == 1) {
				//rect.setOutlineColor(sf::Color::Green);
				rect.setOutlineColor(Green);
			}
			else if (joc_curent.tabla_de_joc[i][j].back().winner == 1) {
				// rect.setOutlineColor(sf::Color::Red);
				rect.setOutlineColor(Red);
			}
			else rect.setOutlineColor(sf::Color(0, 0, 0, 0));
			juc = joc_curent.tabla_de_joc[i][j].back().jucator;

			rect.setPosition(tabla_Ox + tabla_delt_Ox * j, tabla_Oy + tabla_delt_Oy * i);
			if (juc == 1 || juc == 2) {
				auto dmn = dimens[joc_curent.tabla_de_joc[i][j].back().dimensiune - 1];
				auto textur = Jucator_Sprite[juc - 1].getTexture()->getSize();
				Jucator_Sprite[juc - 1].setScale(dmn.x / textur.x, dmn.y / textur.y);
				Jucator_Sprite[juc - 1].setPosition(rect.getPosition().x + (rect.getSize().x - dmn.x) / 2, rect.getPosition().y + (rect.getSize().y - dmn.y) / 2);
				window.draw(Jucator_Sprite[juc - 1]);
			}
			window.draw(rect);
		}
	}

	rect.setSize(sf::Vector2f(jucatori_delt_Ox, jucatori_delt_Oy));

	for (int i = 0; i < joc_curent.pioni_jucatori[0].size(); i++) {
		if (joc_curent.pioni_jucatori[0][i].selectat == 1) {
			rect.setOutlineColor(Green);
		}
		else rect.setOutlineColor(sf::Color(0, 0, 0, 0));
		rect.setPosition(jucator_X[0] + (i % 2) * jucatori_delt_Ox, jucatori_Y + jucatori_delt_Oy * (i / 2));
		auto dmn = dimens[joc_curent.pioni_jucatori[0][i].dimensiune - 1];
		auto textur = Jucator_Sprite[0].getTexture()->getSize();
		Jucator_Sprite[0].setScale(dmn.x / textur.x, dmn.y / textur.y);
		Jucator_Sprite[0].setPosition(rect.getPosition().x + (rect.getSize().x - dmn.x) / 2, rect.getPosition().y + (rect.getSize().y - dmn.y) / 2);
		window.draw(Jucator_Sprite[0]);
		window.draw(rect);
	}
	for (int i = 0; i < joc_curent.pioni_jucatori[1].size(); i++) {
		if (joc_curent.pioni_jucatori[1][i].selectat == 1) {
			rect.setOutlineColor(Green);
		}
		else rect.setOutlineColor(sf::Color(0, 0, 0, 0));
		rect.setPosition(jucator_X[1] + (i % 2) * jucatori_delt_Ox, jucatori_Y + jucatori_delt_Oy * (i / 2) - 10);
		auto dmn = dimens[joc_curent.pioni_jucatori[1][i].dimensiune - 1];
		auto textur = Jucator_Sprite[1].getTexture()->getSize();
		Jucator_Sprite[1].setScale(dmn.x / textur.x, dmn.y / textur.y);
		Jucator_Sprite[1].setPosition(rect.getPosition().x + (rect.getSize().x - dmn.x) / 2, rect.getPosition().y + (rect.getSize().y - dmn.y) / 2);
		window.draw(Jucator_Sprite[1]);
		window.draw(rect);
	}
}

int robot_level;

int eval(int jucator) {
	int i, j, coef, rez = 0;
	int cntlin[3][2], cntcol[3][2];
	for (i = 0; i < 3; i++) for (j = 0; j < 2; j++) cntlin[i][j] = 0, cntcol[i][j] = 0;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			if (joc_curent.tabla_de_joc[i][j].back().jucator >= 1)
				cntlin[i][joc_curent.tabla_de_joc[i][j].back().jucator - 1]++;
			if (joc_curent.tabla_de_joc[i][j].back().jucator >= 1)
				cntcol[j][joc_curent.tabla_de_joc[i][j].back().jucator - 1]++;
		}
	}
	int diag1 = 0, diag2 = 0;
	if (joc_curent.tabla_de_joc[1][1].back().jucator > 0) {
		diag1 = diag2 = 0;
	}
	else {
		if (joc_curent.tabla_de_joc[0][0].back().jucator == joc_curent.tabla_de_joc[2][2].back().jucator) diag1 = joc_curent.tabla_de_joc[0][0].back().jucator;
		if (joc_curent.tabla_de_joc[0][2].back().jucator == joc_curent.tabla_de_joc[2][0].back().jucator) diag2 = joc_curent.tabla_de_joc[0][2].back().jucator;
	}
	if (diag1 - 1 == jucator || diag2 - 1 == jucator) rez += 1e8;
	if (diag1 - 1 == 1 - jucator || diag2 - 1 == 1 - jucator) rez -= 100;
	if (diag2 - 1 == 1 - jucator && diag1 - 1 == 1 - jucator) rez -= 1e7;
	for (i = 0; i < 3; i++) {
		if (cntlin[i][jucator] == 2 && cntlin[i][1 - jucator] == 0) rez += 1e8;
		if (cntlin[i][jucator] == 0 && cntlin[i][1 - jucator] == 2) rez -= 100;
		if (cntcol[i][jucator] == 2 && cntcol[i][1 - jucator] == 0) rez += 1e8;
		if (cntcol[i][jucator] == 0 && cntcol[i][1 - jucator] == 2) rez -= 100;
	}
	if (cntlin[1][1 - jucator] == 2 && diag1) rez -= 1e7;
	if (cntlin[1][1 - jucator] == 2 && diag2) rez -= 1e7;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			if (cntlin[i][1 - jucator] == 2 && cntlin[i][jucator] == 0 && cntcol[j][1 - jucator] == 2 && cntcol[j][jucator] == 0 && joc_curent.tabla_de_joc[i][j].back().jucator == 1 - jucator + 1) rez -= -1e7;
		}
	}
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			if (i == 1 && j == 1) coef = 5;
			else if (i != 1 && j != 1) coef = 3;
			else coef = 1;
			if (joc_curent.tabla_de_joc[i][j].back().jucator == jucator + 1)
				coef = coef;
			else if (joc_curent.tabla_de_joc[i][j].back().jucator == 1 - jucator + 1)
				coef = -coef;
			else coef = 0;
			rez += coef * joc_curent.tabla_de_joc[i][j].back().dimensiune * joc_curent.tabla_de_joc[i][j].back().dimensiune;
		}
	}
	return rez;
}
int joaca_robot(int jucator, int level = -1) {
	vector<pair<pion_selectat, pair<int, int>>> mutari;
	vector<pion_selectat> selectabili;
	int i, j;
	pion_selectat aux;
	int maxi = 1e9;
	stare_joc rez = joc_curent, cpy, perm_copy = joc_curent;
	for (i = 0; i < joc_curent.pioni_jucatori[jucator].size(); i++) {
		aux.origine = jucator + 1;
		aux.pozitie = i;
		aux.ref = &perm_copy.pioni_jucatori[jucator][i];
		selectabili.push_back(aux);
	}
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			if (joc_curent.tabla_de_joc[i][j].back().jucator - 1 == jucator) {
				aux.origine = 0;
				aux.pozitie = i * 3 + j;
				aux.ref = &perm_copy.tabla_de_joc[i][j].back();
				selectabili.push_back(aux);
			}
		}
	}
	for (auto pion : selectabili) {
		for (i = 0; i < 3; i++) {
			for (j = 0; j < 3; j++) {
				if (joc_curent.tabla_de_joc[i][j].back().dimensiune >= pion.ref->dimensiune) continue;
				mutari.push_back({ pion, {i, j} });
			}
		}
	}
	if (level == -1) {
		random_shuffle(mutari.begin(), mutari.end());
		activ = mutari[0].first;
		i = mutari[0].second.first;
		j = mutari[0].second.second;
		if (pune_piesa(*(activ.ref), i, j)) {
			scoate_piesa_activa();
		}
		return 0;
	}
	if (level == 0) {
		for (int k = 0; k < mutari.size(); k++) {
			auto pion = mutari[k].first;
			auto poz = mutari[k].second;
			activ = pion;
			i = poz.first;
			j = poz.second;
			if (pune_piesa(*(activ.ref), i, j)) {
				scoate_piesa_activa();
			}
			cpy = joc_curent;
			int aux = verifica_victorie();
			int aux2;
			joc_curent = cpy;
			if (aux - 1 == jucator) aux2 = (int)-1e9;
			else if (aux == -1) aux2 = 0;
			else if (aux == 0) {
				aux2 = eval(1 - jucator);
			}
			else aux2 = 1e9;
			if (aux2 <= maxi) maxi = aux2, rez = cpy;
			joc_curent = perm_copy;

		}
		joc_curent = rez;
		return -maxi;
	}
	else {
		for (int k = 0; k < mutari.size(); k++) {
			auto pion = mutari[k].first;
			auto poz = mutari[k].second;
			activ = pion;
			i = poz.first;
			j = poz.second;
			if (pune_piesa(*(activ.ref), i, j)) {
				scoate_piesa_activa();
			}
			cpy = joc_curent;

			int aux = verifica_victorie();
			int aux2;
			joc_curent = cpy;
			if (aux - 1 == jucator) aux2 = (int)-1e9;
			else if (aux == -1) aux2 = 0;
			else if (aux == 0) {
				aux2 = joaca_robot(1 - jucator, level - 1);
			}
			else aux2 = 1e9;
			joc_curent = cpy;
			if (aux2 <= maxi) maxi = aux2, rez = cpy;
			joc_curent = perm_copy;
		}
		joc_curent = rez;
		return -maxi;
	}
}
void joc_on_click() {
	//GATA 
	auto mouse_poz = sf::Vector2i(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
	buton undo = { 600, 680, 625, 700 };
	buton X = { 1210, 1270, 5, 60 };
	//if (in casuta de inchis)
	if (X.in(mouse_poz))
	{
		Click_Sound.play();
		//inchis joc, posibil cu pop-up
		deselecteaza();
		joc_curent.scriere_in_fisier(0);
		load_scene_meniu_principal(1);
		return;
	}
	//if (in casuta de undo)
	if (undo.in(mouse_poz))
	{
		//undo
		Click_Sound.play();
		deselecteaza();
		if (history.size() > 1) {
			history.pop();
			joc_curent = history.top();
			Joc_Sprite.setTexture(Joc_Texture[joc_curent.tura_curenta - 1]);
		}
		goto scriere;
	}
	int i, j;
	if (joc_curent.jucatori_id[joc_curent.tura_curenta - 1] == -1) {
		joaca_robot(joc_curent.tura_curenta - 1, -1);
		joc_curent.tura_curenta = 3 - joc_curent.tura_curenta;
		for (i = 0; i < 3; i++) {
			for (j = 0; j < 3; j++) {
				if (joc_curent.tabla_de_joc[i][j].size() > history.top().tabla_de_joc[i][j].size()) {
					if (history.top().tabla_de_joc[i][j].back().dimensiune == 0)
						Move_Sound.play();
					else
						Munch_Sound.play();
				}
			}
		}
		history.push(joc_curent);
		Joc_Sprite.setTexture(Joc_Texture[joc_curent.tura_curenta - 1]);
		goto final;
	}
	if (joc_curent.jucatori_id[joc_curent.tura_curenta - 1] == -2) {
		joaca_robot(joc_curent.tura_curenta - 1, robot_level);
		joc_curent.tura_curenta = 3 - joc_curent.tura_curenta;
		for (i = 0; i < 3; i++) {
			for (j = 0; j < 3; j++) {
				if (joc_curent.tabla_de_joc[i][j].size() > history.top().tabla_de_joc[i][j].size()) {
					if (history.top().tabla_de_joc[i][j].back().dimensiune == 0)
						Move_Sound.play();
					else
						Munch_Sound.play();
				}
			}
		}
		history.push(joc_curent);
		Joc_Sprite.setTexture(Joc_Texture[joc_curent.tura_curenta - 1]);
		goto final;
	}

	for (i = 0; i < joc_curent.pioni_jucatori[joc_curent.tura_curenta - 1].size(); i++) {
		/*
		if (mouse not in defining box)
			continue;
		*/
		if (mouse_poz.x < jucator_X[joc_curent.tura_curenta - 1] + (i % 2) * jucatori_delt_Ox || mouse_poz.x > jucator_X[joc_curent.tura_curenta - 1] + (i % 2 + 1) * jucatori_delt_Ox || mouse_poz.y < jucatori_Y + (i / 2) * jucatori_delt_Oy || mouse_poz.y > jucatori_Y + (i / 2 + 1) * jucatori_delt_Oy)
			continue;
		Click_Sound.play();
		deselecteaza();
		joc_curent.pioni_jucatori[joc_curent.tura_curenta - 1][i].selectat = 1;
		activ.ref = &(joc_curent.pioni_jucatori[joc_curent.tura_curenta - 1][i]);
		activ.pozitie = i;
		activ.origine = joc_curent.tura_curenta;
		goto final;
	}

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			if (mouse_poz.x < tabla_Ox + j * tabla_delt_Ox || mouse_poz.x > tabla_Ox + (j + 1) * tabla_delt_Ox || mouse_poz.y < tabla_Oy + i * tabla_delt_Oy || mouse_poz.y > tabla_Oy + (i + 1) * tabla_delt_Oy)
				continue;
			if (activ.origine != -1) {
				if (pune_piesa(*(activ.ref), i, j)) {
					scoate_piesa_activa();
					if (history.top().tabla_de_joc[i][j].back().dimensiune == 0)
						Move_Sound.play();
					else
						Munch_Sound.play();
					joc_curent.tura_curenta = 3 - joc_curent.tura_curenta;
					Joc_Sprite.setTexture(Joc_Texture[joc_curent.tura_curenta - 1]);
					history.push(joc_curent);
				}
				else {
					//play error sound
					Error_Sound.play();
				}
				goto final;
			}
			else {
				if (joc_curent.tabla_de_joc[i][j].back().jucator != joc_curent.tura_curenta)
					continue;
				joc_curent.tabla_de_joc[i][j].back().selectat = 1;
				Click_Sound.play();
				activ.ref = &(joc_curent.tabla_de_joc[i][j].back());
				activ.origine = 0;
				activ.pozitie = i * 3 + j;
				goto final;
			}
		}
	}
	if (activ.pozitie != -1) Click_Sound.play();
	deselecteaza();
	final:
	aux = verifica_victorie();
	if (aux >= 1) {
		if (joc_curent.jucatori_id[0] > 0 && joc_curent.jucatori_id[1] > 0) {
			modifica_scor(joc_curent.jucatori_id[aux - 1], 1);
			modifica_scor(joc_curent.jucatori_id[3 - aux - 1], 0);
		}
		load_scene_gata_joc();
		end_game_id = joc_curent.jucatori_id[aux - 1];
	}
scriere:
	joc_curent.scriere_in_fisier((aux != 0));
}

void joc_nou(int _id1, int _id2) {
	while (!history.empty()) history.pop();
	joc_curent.joc_nou(_id1, _id2);
	load_scene_joc();
	/*reverse(joc_curent.pioni_jucatori[0].begin(), joc_curent.pioni_jucatori[0].end());
	reverse(joc_curent.pioni_jucatori[1].begin(), joc_curent.pioni_jucatori[1].end());*/
}

int jucator1_selectie = 1, jucator2_selectie = 0;

void load_scene_selectie_jucatori() {
	jucator1_selectie = 1;
	jucator2_selectie = 0;
	grafica = selectie_jucatori_grafica;
	on_click = selectie_jucatori_on_click;
	on_scroll = nullptr;
	text_entered = nullptr;
}

void selectie_jucatori_grafica() {
	//buton pt selectie profil 
	window.clear(sf::Color::Yellow);
	window.draw(Selectie_Jucatori_Sprite);

	Jucator_Sprite[0].setTexture(Jucator_Texture[0][lista_de_jucatori[jucator1_selectie].id <= -1], 1);
	Jucator_Sprite[1].setTexture(Jucator_Texture[1][lista_de_jucatori[jucator2_selectie].id <= -1], 1);

	auto dmns0 = Jucator_Texture[0][lista_de_jucatori[jucator1_selectie].id <= -1].getSize();
	Jucator_Sprite[0].setScale(100.0 * 0.9 / dmns0.x, 180.0 * 0.9 / dmns0.y);
	auto dmns1 = Jucator_Texture[1][lista_de_jucatori[jucator2_selectie].id <= -1].getSize();
	Jucator_Sprite[1].setScale(100.0 * 0.9 / dmns1.x, 180.0 * 0.9 / dmns1.y);

	Jucator_Sprite[0].setPosition(260, 300);
	Jucator_Sprite[1].setPosition(940, 300);
	window.draw(Jucator_Sprite[0]);
	window.draw(Jucator_Sprite[1]);
	sf::Text text;
	text.setFont(font);
	text.setColor(sf::Color::Black);
	text.setScale(sf::Vector2f(0.8, 0.8));

	text.setString(lista_de_jucatori[jucator1_selectie].nume);
	auto xx = text.getLocalBounds().getSize();
	auto x = min(0.8, 200.0 / xx.x);
	text.setScale(x, x);
	text.setPosition(sf::Vector2f(300 - xx.x * x / 2, 525 - x * xx.y / 2));
	window.draw(text);

	text.setString(lista_de_jucatori[jucator2_selectie].nume);
	xx = text.getLocalBounds().getSize();
	x = min(0.8, 200.0 / xx.x);
	text.setScale(x, x);
	text.setPosition(sf::Vector2f(980 - xx.x * x / 2, 525 - x * xx.y / 2));
	window.draw(text);
}


void selectie_jucatori_on_click() {
	auto mouse_poz = sf::Vector2i(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
	//if (incepe joc)
	if (selectie_jucatori_joaca.in(mouse_poz))
	{
		Click_Sound.play();
		joc_nou(lista_de_jucatori[jucator1_selectie].id, lista_de_jucatori[jucator2_selectie].id);
		return;
	}
	//if (main menu)
	if (selectie_jucatori_inapoi.in(mouse_poz))
	{
		Click_Sound.play();
		load_scene_meniu_principal(0);
		return;
	}
	//if (ciclare stanga player1)
	if (selectie_jucatori_P1_stanga.in(mouse_poz))
	{
		Click_Sound.play();
		jucator1_selectie--;
		jucator1_selectie = (jucator1_selectie + lista_de_jucatori.size()) % lista_de_jucatori.size();
		if (jucator1_selectie == jucator2_selectie && jucator1_selectie > 1) {
			jucator1_selectie--;
			jucator1_selectie = (jucator1_selectie + lista_de_jucatori.size()) % lista_de_jucatori.size();
		}
	}
	//if (ciclare dreapta player1)
	if (selectie_jucatori_P1_dreapta.in(mouse_poz))
	{
		Click_Sound.play();
		jucator1_selectie++;
		jucator1_selectie = (jucator1_selectie + lista_de_jucatori.size()) % lista_de_jucatori.size();
		if (jucator1_selectie == jucator2_selectie && jucator1_selectie > 1) {
			jucator1_selectie++;
			jucator1_selectie = (jucator1_selectie + lista_de_jucatori.size()) % lista_de_jucatori.size();
		}
	}
	//if (ciclare stanga player2)
	if (selectie_jucatori_P2_stanga.in(mouse_poz))
	{
		Click_Sound.play();
		jucator2_selectie--;
		jucator2_selectie = (jucator2_selectie + lista_de_jucatori.size()) % lista_de_jucatori.size();
		if (jucator1_selectie == jucator2_selectie && jucator1_selectie > 1) {
			jucator2_selectie--;
			jucator2_selectie = (jucator2_selectie + lista_de_jucatori.size()) % lista_de_jucatori.size();
		}
	}
	//if (ciclare dreapta player2)
	if (selectie_jucatori_P2_dreapta.in(mouse_poz))
	{
		Click_Sound.play();
		jucator2_selectie++;
		jucator2_selectie = (jucator2_selectie + lista_de_jucatori.size()) % lista_de_jucatori.size();
		if (jucator1_selectie == jucator2_selectie && jucator1_selectie > 1) {
			jucator2_selectie++;
			jucator2_selectie = (jucator2_selectie + lista_de_jucatori.size()) % lista_de_jucatori.size();
		}
	}
}

void load_scene_joc_nou_sau_continua() {
	grafica = joc_nou_sau_continua_grafica;
	on_click = joc_nou_sau_continua_on_click;
	on_scroll = nullptr;
	text_entered = nullptr;
}
void joc_nou_sau_continua_grafica() {
	meniu_principal_grafica();

	sf::RectangleShape rect;
	rect.setSize(sf::Vector2f(1280, 720));
	rect.setFillColor(sf::Color(0, 0, 0, 160));
	window.draw(rect);


	window.draw(Continue_Sprite);
}

void joc_nou_sau_continua_on_click() {
	auto mouse_poz = sf::Vector2i(window.mapPixelToCoords(sf::Mouse::getPosition(window)));

	buton yes = { 410, 600, 450, 525 };
	map_button(yes);

	buton no = { 675, 865, 450, 525 };
	map_button(no);

	buton X = { 920, 970, 205, 263 };
	map_button(X);

	//if (joc_nou)
	if (no.in(mouse_poz))
	{
		Click_Sound.play();
		load_scene_selectie_jucatori();
		return;
	}
	//if (continue)
	if (yes.in(mouse_poz))
	{
		Click_Sound.play();
		load_scene_joc();
		return;
	}
	if (X.in(mouse_poz)) {
		Click_Sound.play();
		load_scene_meniu_principal(0);
		return;
	}
}

void load_scene_meniu_principal(bool reset) {
	if (reset) {
		music.openFromFile("assets/SFX/Menu.mp3");
		music.setLoop(1);
		music.setVolume(10);
		music.play();
		music.setPosition(0, 0, 0);
	}
	grafica = meniu_principal_grafica;
	on_click = meniu_principal_on_click;
	on_scroll = nullptr;
	text_entered = nullptr;
}
void meniu_principal_grafica() {
	//buton pt joc,tutorial,jucatori,limbi
	window.clear(sf::Color::Yellow);
	window.draw(Meniu_Sprite);
}
void meniu_principal_on_click() {
	auto mouse_poz = sf::Vector2i(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
	if (meniu_principal_buton_joaca.in(mouse_poz))
	{
		Click_Sound.play();
		joc_curent.citire_din_fisier();
		if (joc_curent.joc_salvat == 0) {
			load_scene_selectie_jucatori();
			goto final;
		}
		load_scene_joc_nou_sau_continua();
		goto final;
	}
	//if (jucatorul este prost)
	if (meniu_principal_buton_tutorial.in(mouse_poz))
	{
		Click_Sound.play();
		load_scene_tutorial();
		goto final;
	}
	//if (lista jucatori)
	if (meniu_principal_profile_jucatori.in(mouse_poz))
	{
		Click_Sound.play();
		load_scene_profile_jucatori(1);
		goto final;
	}
	//if (clasament)
	if (meniu_principal_buton_clasament.in(mouse_poz))
	{
		Click_Sound.play();
		load_scene_clasament(1);
		delta = 0;
		goto final;
	}

	if (meniu_principal_buton_limba.in(mouse_poz))
	{
		Click_Sound.play();
		//0 rom 1 eng
		limba = 1 - limba;
		strcpy(aux_str, nume_boti[limba][0].c_str());
		modifica_nume(-1);
		strcpy(aux_str, nume_boti[limba][1].c_str());
		modifica_nume(-2);
		Meniu_Sprite.setTexture(Meniu_Texture[limba]);
		Selectie_Jucatori_Sprite.setTexture(Selectie_Jucatori_Texture[limba]);
		Tutorial_Sprite.setTexture(Tutorial_Texture[limba]);
		Clasament_Sprite.setTexture(Clasament_Texture[limba]);
		Profile_Sprite.setTexture(Profile_Texture[limba]);
		PopupDelete_Sprite.setTexture(PopupDelete_Texture[limba]);
		Creare_Jucator_Sprite.setTexture(Creare_Jucator_Texture[limba]);
		Continue_Sprite.setTexture(Continue_Texture[limba]);
		Gata_Joc_Sprite.setTexture(Gata_Joc_Texture[limba]);

		goto final;
	}
	/*//if (inchide aplicatione)
	{
		exit(0);
	}*/
	final:
	return;
}

void Load_Sprites() {
	font.loadFromFile("assets/arial.ttf");
	font_sancreek.loadFromFile("assets/Sancreek-Regular.ttf");

	sf::Text txt;
	txt.setFont(font);
	txt.setString("Loading...");
	txt.setOutlineThickness(1);
	txt.setColor(sf::Color::Black);
	txt.setScale(2, 2);
	auto xxx = txt.getLocalBounds().getSize();
	txt.setPosition(640 - xxx.x, 360 - xxx.y);
	window.clear(sf::Color::White);
	window.draw(txt);
	window.display();

	Error_Buffer.loadFromFile("assets/SFX/Error.wav");
	Error_Sound.setBuffer(Error_Buffer);

	Munch_Buffer.loadFromFile("assets/SFX/Munch.wav");
	Munch_Sound.setBuffer(Munch_Buffer);

	Click_Buffer.loadFromFile("assets/SFX/Click.wav");
	Click_Sound.setBuffer(Click_Buffer);

	Move_Buffer.loadFromFile("assets/SFX/Mutare.wav");
	Move_Sound.setBuffer(Move_Buffer);

	strcpy(aux_str, nume_boti[limba][0].c_str());
	modifica_nume(-1);
	strcpy(aux_str, nume_boti[limba][1].c_str());
	modifica_nume(-2);

	Blank_Texture.loadFromFile("assets/Blank.png");

	Jucator_Texture[0][0].loadFromFile("assets/Player1.png");
	Jucator_Texture[1][0].loadFromFile("assets/Player2.png");
	Jucator_Texture[0][1].loadFromFile("assets/CPU1.png");
	Jucator_Texture[1][1].loadFromFile("assets/CPU2.png");
	Jucator_Sprite[0].setTexture(Jucator_Texture[0][0]);
	Jucator_Sprite[1].setTexture(Jucator_Texture[1][0]);


	Meniu_Texture[0].loadFromFile("assets/MeniuRO.png");
	Meniu_Texture[1].loadFromFile("assets/MeniuEN.png");
	Meniu_Sprite.setTexture(Meniu_Texture[0]);
	auto x = Meniu_Texture[0].getSize();
	auto y = window.getSize();
	Meniu_Sprite.setScale(sf::Vector2f(float(y.x) / x.x, float(y.y) / x.y));

	Creare_Jucator_Texture[0].loadFromFile("assets/CreareRO.png");
	Creare_Jucator_Texture[1].loadFromFile("assets/CreareEN.png");
	Creare_Jucator_Sprite.setTexture(Creare_Jucator_Texture[0]);
	x = Creare_Jucator_Texture[0].getSize();
	y = window.getSize();
	Creare_Jucator_Sprite.setScale(sf::Vector2f(float(y.x) / x.x * 0.6, float(y.y) / x.y * 0.5));
	x = window.getSize();
	y = Creare_Jucator_Texture[0].getSize();
	auto z = Creare_Jucator_Sprite.getScale();
	Creare_Jucator_Sprite.setPosition(sf::Vector2f((x.x - y.x * z.x) / 2, (x.y - y.y * z.y) / 2));

	Continue_Texture[0].loadFromFile("assets/ContinueRO.png");
	Continue_Texture[1].loadFromFile("assets/ContinueEN.png");
	Continue_Sprite.setTexture(Continue_Texture[0]);
	x = Continue_Texture[0].getSize();
	y = window.getSize();
	Continue_Sprite.setScale(sf::Vector2f(float(y.x) / x.x * 0.6, float(y.y) / x.y * 0.5));
	x = window.getSize();
	y = Continue_Texture[0].getSize();
	z = Continue_Sprite.getScale();
	Continue_Sprite.setPosition(sf::Vector2f((x.x - y.x * z.x) / 2, (x.y - y.y * z.y) / 2));

	Gata_Joc_Texture[0].loadFromFile("assets/VictorieRO.png");
	Gata_Joc_Texture[1].loadFromFile("assets/VictorieEN.png");
	Gata_Joc_Sprite.setTexture(Gata_Joc_Texture[0]);
	x = Gata_Joc_Texture[0].getSize();
	y = window.getSize();
	Gata_Joc_Sprite.setScale(sf::Vector2f(float(y.x) / x.x * 0.6, float(y.y) / x.y * 0.55));
	x = window.getSize();
	y = Gata_Joc_Texture[0].getSize();
	z = Gata_Joc_Sprite.getScale();
	Gata_Joc_Sprite.setPosition(sf::Vector2f((x.x - y.x * z.x) / 2, (x.y - y.y * z.y) / 2));

	Selectie_Jucatori_Texture[0].loadFromFile("assets/SelectieJucatoriRO.png");
	Selectie_Jucatori_Texture[1].loadFromFile("assets/SelectieJucatoriEN.png");
	Selectie_Jucatori_Sprite.setTexture(Selectie_Jucatori_Texture[0]);
	x = Selectie_Jucatori_Texture[0].getSize();
	y = window.getSize();
	Selectie_Jucatori_Sprite.setScale(sf::Vector2f(float(y.x) / x.x, float(y.y) / x.y));

	Profile_Texture[0].loadFromFile("assets/ProfileRO.png");
	Profile_Texture[1].loadFromFile("assets/ProfileEN.png");
	Profile_Sprite.setTexture(Profile_Texture[0]);
	x = Profile_Texture[0].getSize();
	y = window.getSize();
	Profile_Sprite.setScale(sf::Vector2f(float(y.x) / x.x, float(y.y) / x.y));

	PopupDelete_Texture[0].loadFromFile("assets/StergeRO.png");
	PopupDelete_Texture[1].loadFromFile("assets/StergeEN.png");
	PopupDelete_Sprite.setTexture(PopupDelete_Texture[0]);
	x = PopupDelete_Texture[0].getSize();
	y = window.getSize();
	PopupDelete_Sprite.setScale(sf::Vector2f(float(y.x) / x.x * 0.6, float(y.y) / x.y * 0.5));
	x = window.getSize();
	y = PopupDelete_Texture[0].getSize();
	z = PopupDelete_Sprite.getScale();
	PopupDelete_Sprite.setPosition(sf::Vector2f((x.x - y.x * z.x) / 2, (x.y - y.y * z.y) / 2));

	Slot_Texture.loadFromFile("assets/SlotProfil.png");
	Slot_Sprite.setTexture(Slot_Texture);
	x = Profile_Texture[0].getSize();
	y = window.getSize();
	Slot_Sprite.setScale(sf::Vector2f(float(y.x) / x.x, float(y.y) / x.y));

	Clasament_Texture[0].loadFromFile("assets/ClasamentRO.png");
	Clasament_Texture[1].loadFromFile("assets/ClasamentEN.png");
	Clasament_Sprite.setTexture(Clasament_Texture[0]);
	x = Clasament_Texture[0].getSize();
	y = window.getSize();
	Clasament_Sprite.setScale(sf::Vector2f(float(y.x) / x.x, float(y.y) / x.y));

	Tutorial_Texture[0].loadFromFile("assets/TutorialRO.png");
	Tutorial_Texture[1].loadFromFile("assets/TutorialEN.png");
	Tutorial_Sprite.setTexture(Tutorial_Texture[0]);
	x = Tutorial_Texture[0].getSize();
	y = window.getSize();
	Tutorial_Sprite.setScale(sf::Vector2f(float(y.x) / x.x, float(y.y) / x.y));

	Joc_Texture[0].loadFromFile("assets/MeniuTurn1.png");
	Joc_Texture[1].loadFromFile("assets/MeniuTurn2.png");
	Joc_Sprite.setTexture(Joc_Texture[0]);
	x = Joc_Texture[0].getSize();
	y = window.getSize();
	Joc_Sprite.setScale(sf::Vector2f(float(y.x) / x.x, float(y.y) / x.y));


}
void debug() {
	int n;
	cin >> n;
	char nume[22];
	int id{};
	for (int i = 0; i < n; i++) {
		cin >> nume;
		creare_jucator_nou(nume, id);
	}
	scriere_jucatori_in_fisier();
	citire_jucatori_din_fisier();
	for (auto& jucator : lista_de_jucatori) {
		cout << jucator.nume << ' ' << jucator.id << ' ' << jucator.victorii << ' ' << jucator.infrangeri << endl;
	}
}
bool compi(profil_jucator a, profil_jucator b) { return a.id < b.id; }
sf::Image icon;
int main(int argc, char* argv[]) {
	icon.loadFromFile("assets/Player1.png");
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr()); {
		Load_Sprites();
		citire_jucatori_din_fisier();
		srand(time(nullptr));
		robot_level = 0;
		if (argc == 2 && !strcmp(argv[1], "-db"))
			debug();
		load_scene_meniu_principal(1);
		grafica();
		window.display();
		sf::Event event;
		for (auto jucator : lista_de_jucatori) {
			cout << jucator.nume << ' ' << jucator.id << ' ' << jucator.victorii << ' ' << jucator.infrangeri << endl;
		}
		// while there are pending events...
		while (window.isOpen())
			while (window.pollEvent(event))
			{
				// check the type of the event...
				switch (event.type)
				{
					// window closed
				case sf::Event::Closed:
					window.close();
					break;

					// key pressed
				case sf::Event::TextEntered:
					if (text_entered) {
						text_entered(char(event.text.unicode));
					}
					break;
				case sf::Event::MouseButtonPressed:
					if (event.mouseButton.button == sf::Mouse::Left) {
						on_click();
					}
					break;
				case sf::Event::MouseWheelScrolled:
					if (on_scroll) {
						Move_Sound.play();
						on_scroll(event.mouseWheelScroll.delta > 0 ? 1 : -1);
					}
					break;
					// we don't process other types of events
				default:
					break;
				}
				grafica();
				window.display();
			}
		sort(lista_de_jucatori.begin(), lista_de_jucatori.end(), compi);
		scriere_jucatori_in_fisier();
		return 0;
	}
}