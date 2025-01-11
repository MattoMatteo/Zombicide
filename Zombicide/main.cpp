#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <vector>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5\allegro_primitives.h>
#include <boost/filesystem.hpp>
#include "dati.h"
#include <map>
#include "azioni.h"
#include "utilità.h"
#include "armi_speciale.h"
#include "Azioni_zombie.h"
#include "Abilità.h"
#include "Funzioni_Editor.h"
#include "azioni.h"
#include "Matteo Draw.h"
#include <time.h>

using namespace std;
int giocatore_numero;
bool done = false;
bool mostra = false;

ALLEGRO_EVENT_QUEUE* event_queue=NULL;
ALLEGRO_TIMER* timer=NULL;
Immagini immagini_e_fonts;
StatoGioco stato;

void crea_display(int w, int h, int tag)
{
	al_unregister_event_source(event_queue, al_get_display_event_source(immagini_e_fonts.display));
	al_destroy_display(immagini_e_fonts.display);
	al_set_new_display_flags(tag);
	immagini_e_fonts.display = al_create_display(stato.indicatore.risoluzione[0], stato.indicatore.risoluzione[1]);
	al_register_event_source(event_queue, al_get_display_event_source(immagini_e_fonts.display));
}
vector<int> calcola_risoluzione_mappa_da_file(string percorso_file_dat)
{
	percorso_file_dat.erase(percorso_file_dat.end() - 4, percorso_file_dat.end());
	percorso_file_dat = percorso_file_dat + "_cordinate.dat";
	ifstream leggi_dim = ifstream(percorso_file_dat);
	int w = 0;
	int h = 0;
	string per;
	leggi_dim >> w; leggi_dim >> per; leggi_dim >> h;
	return{ w,h };
}
void assegna_armi_iniziali()
{
	vector<Carta_equipaggiamento> mazzo_carte_iniziali;
	for (int i = 0; i < stato.numero_giocatori && i < 3; i++)
	{
		mazzo_carte_iniziali.push_back(stato.Mazzo_equipaggiamento[0]);
		stato.Mazzo_equipaggiamento.erase(stato.Mazzo_equipaggiamento.begin());
	}
	for (int i = 3; i < stato.numero_giocatori && i < 6; i++)
	{
		mazzo_carte_iniziali.push_back(stato.Mazzo_speciale[0]);
		stato.Mazzo_speciale.erase(stato.Mazzo_speciale.begin());
	}
	for (int i = 0; i < stato.numero_giocatori; i++)
	{
		for (size_t j = 0; j<stato.giocatore[i].abilità.size(); j++)
			if (stato.giocatore[i].abilità[j].tipo == Abilità::START_WITH_A_WEAPON)
			{
				stato.giocatore[i].redraw_scheda = true;
				stato.giocatore[i].abilità[j].funzione(stato, i, j, Abilità::CODICE_ESEGUI);
			}
		int random = rand() % mazzo_carte_iniziali.size();
		stato.giocatore[i].redraw_scheda = true;
		stato.giocatore[i].equipaggiamento.push_back(mazzo_carte_iniziali[random]);
		mazzo_carte_iniziali.erase(mazzo_carte_iniziali.begin() + random);
	}
}
void shutdown()
{
	MATTEO_BITMAP::destroy_all_object();
	if (timer)
		al_destroy_timer(timer);

	if (immagini_e_fonts.display)
	al_destroy_display(immagini_e_fonts.display);

	if (event_queue)
		al_destroy_event_queue(event_queue);
}
void carica_missioni_da_file()
{
	vector<string> elenco_missioni;
	boost::filesystem::directory_iterator end;
	boost::filesystem::path percorso_missioni("Missioni"); //crea indirizzo statico c:\.....missioni
	boost::filesystem::directory_iterator iteratore_cartelle_missioni(percorso_missioni); //crea un iteratore nell'indirizzo dato (c:\...missioni)
	while (iteratore_cartelle_missioni != end) //continua ad iterare finchè l'iteratore non troverà niente e quindi sarà uguale ad un iteratore vuoto (size.() ==0 si può dire)
	{
		boost::filesystem::path percorso_cartella_missione = *iteratore_cartelle_missioni; //crea un nuovo indirizzo che punta all'indirizzo corrente dell'iteratore, e quindi: c:\...missioni\cartella
		boost::filesystem::directory_iterator file_missioni(percorso_cartella_missione);//crea un nuovo iteratore associato all'indirizzo precedentemente creato
		while (file_missioni != end) //continua ad iterare finchè non finisco i file della cartella
		{
			boost::filesystem::path percorso_file_missione = *file_missioni; //crea un nuovo percorso riferito all'iteratore che scorre i file dentro la cartella (c:\....missioni\cartella\file.xxx
			if (is_regular_file(percorso_file_missione) && percorso_file_missione.extension() == ".txt") //se ne trova una .thx
				elenco_missioni.push_back(percorso_file_missione.stem().string());//inserisce il suo nome dentro un vettore di stringhe
			file_missioni++; //incrementa l'iteratore per cercare il prossimo file
		}
		iteratore_cartelle_missioni++; //incrementa l'iteratore per cercare la prossima cartella
	}
	for (size_t i = 0; i < elenco_missioni.size(); i++)
	{
		Missione m;
		stato.missioni.push_back(m);
	}
	for (size_t n = 0; n < elenco_missioni.size(); n++)
	{
		vector<int> vuoto;
		int stanza_1;
		int stanza_2;
		int risultato;
		int stanza;
		string cose;
		int quanti = 0;
		string percorso_txt_missione;
		percorso_txt_missione = "missioni/" + elenco_missioni[n] + "/" + elenco_missioni[n] + ".txt";
		ifstream leggi = ifstream(percorso_txt_missione);
		Tessere_editor t;
		vector<vector<Tessere_editor>> vvt;
		leggi >> cose;
		while (cose != "-2")
		{
			vector<Tessere_editor> vt;
			while (cose != "-1"&&cose != "-2")
			{
				for (size_t i = 0; i < stato.Elenco_Tessere.size(); i++)
					if (stato.Elenco_Tessere[i].nome == cose)
						t = stato.Elenco_Tessere[i];
				leggi >> cose;
				for (size_t i = 0; i < t.angolatura.size(); i++)
					if (cose == to_string(t.angolatura[i]).c_str())
						t.indice_angolo_da_usare = i;
				vt.push_back(t);
				leggi >> cose;
			}
			vvt.push_back(vt);
			if (cose == "-1")
				leggi >> cose;
		}
		int lunghezza_max_mappa = 0;
		for (size_t i = 0; i < vvt.size(); i++)
			if (lunghezza_max_mappa < (int)vvt[i].size())
				lunghezza_max_mappa = (int)vvt[i].size();
		int width_mappa = lunghezza_max_mappa * 982;
		int height_mappa = vvt.size() * 982;

		int indice_missione = n;
		stato.missioni[indice_missione].mappa = MATTEO_BITMAP::create();
		stato.missioni[indice_missione].mappa->init(1600, 1080, MATTEO_BITMAP::CREA, ""); //inizializzo le 2 mappe di missione
		stato.missioni[indice_missione].mappa_pulita = MATTEO_BITMAP::create();
		stato.missioni[indice_missione].mappa_pulita->init(width_mappa, height_mappa, MATTEO_BITMAP::CREA, "");
		stato.missioni[indice_missione].w_h_mappa.push_back(width_mappa);
		stato.missioni[indice_missione].w_h_mappa.push_back(height_mappa);
		stato.missioni[indice_missione].nome_missione = elenco_missioni[n];
		string percorso;
		percorso = "Missioni/" + stato.missioni[indice_missione].nome_missione;
		boost::filesystem::path files_missione(percorso);
		boost::filesystem::directory_iterator iteratore_cartelle_missioni(files_missione);
		while (iteratore_cartelle_missioni != end)
		{
			boost::filesystem::path files = *iteratore_cartelle_missioni;
			if (is_regular_file(files) && files.extension() == ".bmp")
			{
				stato.missioni[indice_missione].mappa_miniatura = MATTEO_BITMAP::create();
				stato.missioni[indice_missione].mappa_miniatura->init(0, 0, MATTEO_BITMAP::CARICA, files.string());
			}
			iteratore_cartelle_missioni++;
		}
		stato.missioni[indice_missione].Mappa_matrice = vvt;
		int dim_matrice = 0;
		leggi >> dim_matrice;
		for (int i = 0; i < dim_matrice; i++)
			vuoto.push_back(0);
		DatiStanza dati_vuoto;
		for (int i = 0; i < dim_matrice; i++)
		{
			stato.missioni[indice_missione].collisioni.push_back(vuoto);
			stato.missioni[indice_missione].orientamento_connessioni.push_back(vuoto);
			stato.missioni[indice_missione].dati.push_back(dati_vuoto);
		}
		for (int i = 0; i < dim_matrice; i++)
			stato.missioni[indice_missione].orientamento_connessioni[i][i] = 8; //centro
		leggi >> stanza_1;
		while (stanza_1 != -1)
		{
			leggi >> stanza_2;
			leggi >> risultato;
			stato.missioni[indice_missione].collisioni[stanza_1][stanza_2] = risultato / 10;
			stato.missioni[indice_missione].orientamento_connessioni[stanza_1][stanza_2] = risultato % 10;
			leggi >> stanza_1;
		}
		leggi >> quanti;
		for (int i = 0; i < dim_matrice; i++) //stanze appartenenti a quali tessere: partono da 1 perchè lo 0 serve a separare le stanze tra di loro
		{
			if (quanti == 0)
				leggi >> quanti;
			while (quanti != 0)
			{
				stato.missioni[indice_missione].dati[i].tessera_numero.push_back(quanti);
				leggi >> quanti;
			}

		}
		int numero_tombini=0;
		leggi >> numero_tombini;
		vector<int> tomb;
		for (int i = 0; i < numero_tombini; i++)
		{
			leggi >> quanti;
			while (quanti != -1) //Tombini vettore di 2 int: [0] Zona tombino, [1] Numero Tessera
			{ 
				tomb.push_back(quanti);
				leggi >> quanti;
			}
			stato.missioni[indice_missione].tombini.push_back(tomb);
			tomb.clear();
		}
		
		leggi >> cose;		// legge |
		if (cose == "|")
		{
			while (cose != "\"")
			{
				leggi >> stanza;	//legge numero stanza
				cose = "";
				while (cose != "|" && cose != "\"")
				{
					leggi >> cose;
					if (cose == "obiettivo_rosso")
					{
						leggi >> quanti;
						stato.missioni[indice_missione].dati[stanza].SommaValoreObiettivo(quanti,DatiStanza::rosso);
					}
					if (cose == "obiettivo_blu")
					{
						leggi >> quanti;
						stato.missioni[indice_missione].dati[stanza].SommaValoreObiettivo(quanti, DatiStanza::blu);
					}
					if (cose == "obiettivo_verde")
					{
						leggi >> quanti;
						stato.missioni[indice_missione].dati[stanza].SommaValoreObiettivo(quanti, DatiStanza::verde);
					}
					if (cose == "start")
					{
						leggi >> quanti;
						for (int i = 0; i < quanti; i++)
						{
							stato.missioni[indice_missione].dati[stanza].ModificaValoreGiocatori(true,i);
							stato.giocatore[i].posizione = stanza;
						}
					}
					if (cose == "rumore")
					{
						leggi >> quanti;
						stato.missioni[indice_missione].dati[stanza].SommaValoreRumore(quanti);
					}
					if (cose == "exit")
						stato.missioni[indice_missione].dati[stanza].ModificaValoreExit(true);
					if (cose == "polizia")
					{
						leggi >> quanti;
						stato.missioni[indice_missione].dati[stanza].SommaValorePolizia(quanti);
					}
					if (cose == "pimp_mobile")
					{
						leggi >> quanti;
						stato.missioni[indice_missione].dati[stanza].SommaValorePimp_mobile(quanti);
					}
					if (cose == "spawn_rosso")
					{
						leggi >> quanti;
						stato.missioni[indice_missione].dati[stanza].SommaValoreSpawn(quanti,DatiStanza::rosso);
						for (int i = 0; i < quanti; i++)
							stato.missioni[indice_missione].elenco_spawn.push_back(stanza);
					}
					if (cose == "spawn_blu")
					{
						leggi >> quanti;
						stato.missioni[indice_missione].dati[stanza].SommaValoreSpawn(quanti, DatiStanza::blu);
					}
					if (cose == "cercabile")
					{
						stato.missioni[indice_missione].dati[stanza].cercabile = true;
					}
					if (cose == "walker")
					{
						leggi >> quanti;
						stato.missioni[indice_missione].dati[stanza].SommaValoreWalker(quanti);
					}
					if (cose == "runner")
					{
						leggi >> quanti;
						stato.missioni[indice_missione].dati[stanza].SommaValoreRunner(quanti);
					}
					if (cose == "fat")
					{
						leggi >> quanti;
						stato.missioni[indice_missione].dati[stanza].SommaValoreFat(quanti);
					}
					if (cose == "abominio")
					{
						stato.missioni[indice_missione].dati[stanza].ModificaValoreAbominio(true);
					}
				}
			}
		}
		if (cose == "\"")
		{
			leggi >> cose;
			while (cose != "\"")
			{
				if (cose == "cercabile")
				{
					leggi >> quanti;
					while (quanti != -1)
					{
						stato.missioni[indice_missione].dati[quanti].cercabile = true;
						leggi >> quanti;
					}
				}
				if (cose == "stanze")
				{
					leggi >> quanti;
					while (quanti != -1)
					{
						stato.missioni[indice_missione].dati[quanti].tipo = 1;
						leggi >> quanti;

					}
				}
				leggi >> cose;
			}
		}
		percorso_txt_missione = "missioni/" +
			elenco_missioni[n] + "/" + elenco_missioni[n] + "_cordinate.dat";
		ifstream leggi_cordinate = ifstream(percorso_txt_missione);
		int cordinata = 0;
		string stringa;
		for (int i = 0; i < dim_matrice; i++)
		{
			leggi_cordinate >> cordinata;
			stato.missioni[indice_missione].x_cordinate_zone_mappa.push_back(cordinata);
			leggi_cordinate >> cordinata;
			stato.missioni[indice_missione].y_cordinate_zone_mappa.push_back(cordinata);
			leggi_cordinate >> cordinata;
			stato.missioni[indice_missione].x_dimensione_zone_mappa.push_back(cordinata);
			leggi_cordinate >> cordinata;
			stato.missioni[indice_missione].y_dimensione_zone_mappa.push_back(cordinata);
		}
		leggi_cordinate >> cose;
		stato.missioni[indice_missione].numero_porte = 0;
		Porta porta = { 0 };
		while (cose != "end")
		{
			if (cose == "porta_blu")
			{
				porta.colore = 0;
				porta.stato = 0;
			}
			if (cose == "porta_verde")
			{
				porta.colore = 2;
				porta.stato = 0;
			}
			if (cose == "porta_neutra")
			{
				porta.colore = 1;
				porta.stato = 0;
			}
			if (cose == "porta_verde_sfondata")
			{
				porta.colore = 2;
				porta.stato = 1;
			}
			if (cose == "porta_blu_sfondata")
			{
				porta.colore = 0;
				porta.stato = 1;
			}
			if (cose == "porta_neutra_sfondata")
			{
				porta.colore = 1;
				porta.stato = 1;
			}
			leggi_cordinate >> cordinata;
			porta.x_cordinata = cordinata;
			leggi_cordinate >> cordinata;
			porta.y_cordinata = cordinata;
			porta.dim = 90;
			leggi_cordinate >> cordinata;
			porta.zone[0] = cordinata;
			leggi_cordinate >> cordinata;
			porta.zone[1] = cordinata;
			stato.missioni[indice_missione].porta.push_back(porta);
			stato.missioni[indice_missione].numero_porte++;
			leggi_cordinate >> cose;
		}
	}
}
void carica_tessere_da_file()
{
	string zero="0.png";
	string novanta="90.png";
	string centoottanta="180.png";
	string duecentosettanta="270.png";
	vector<string> nome_angolazione;
	nome_angolazione.push_back(zero); //0
	nome_angolazione.push_back(novanta); //1
	nome_angolazione.push_back(centoottanta); //2
	nome_angolazione.push_back(duecentosettanta); //3

	boost::filesystem::directory_iterator end;
	boost::filesystem::path percorso_Tessere("Tessere"); //crea indirizzo statico c:\.....Tessere
	boost::filesystem::directory_iterator iteratore_cartelle_Tessere(percorso_Tessere); //crea un iteratore nell'indirizzo dato (c:\...Tessere)
	int contatore_tessere = 0;
	while (iteratore_cartelle_Tessere != end) //continua ad iterare finchè l'iteratore non troverà niente e quindi sarà uguale ad un iteratore vuoto (.size() ==0 si può dire)
	{
		Tessere_editor t;
		Parametri_Tessera Tessera_x;
		for (int i = 0; i < 4; i++)
		{
			t.angolatura.push_back(0);
			t.immagini_in_varie_angolature.push_back(MATTEO_BITMAP::create());
		}
		boost::filesystem::path percorso_cartella_Tessere = *iteratore_cartelle_Tessere; //crea un nuovo indirizzo che punta all'indirizzo corrente dell'iteratore, e quindi: c:\...Tessere\cartella
		boost::filesystem::directory_iterator file_Tessere(percorso_cartella_Tessere);//crea un nuovo iteratore associato all'indirizzo precedentemente creato
		string nome = percorso_cartella_Tessere.stem().string();
		t.nome = nome;
		while (file_Tessere != end) //continua ad iterare finchè non finisco i file della cartella
		{
			boost::filesystem::path percorso_file_Tessere = *file_Tessere;

			if (is_regular_file(percorso_file_Tessere) && percorso_file_Tessere.extension() == ".png") //se ne trova una .png
			{
				for (size_t i = 0; i < 4; i++)
				{
					string nome_1 = nome + " " + nome_angolazione[i];
					if (nome_1 == percorso_file_Tessere.stem().string() + ".png")
					{
						switch (i)
						{
						case 0: //0
							t.angolatura[0] = 0;
							t.immagini_in_varie_angolature[0]->init(0, 0, MATTEO_BITMAP::CARICA, percorso_file_Tessere.string().c_str());
							break;
						case 1: //90
							t.angolatura[1] = 90;
							t.immagini_in_varie_angolature[1]->init(0, 0, MATTEO_BITMAP::CARICA, percorso_file_Tessere.string().c_str());
							break;
						case 2: //180
							t.angolatura[2] = 180;
							t.immagini_in_varie_angolature[2]->init(0, 0, MATTEO_BITMAP::CARICA, percorso_file_Tessere.string().c_str());
							break;
						case 3: //270
							t.angolatura[3] = 270;
							t.immagini_in_varie_angolature[3]->init(0, 0, MATTEO_BITMAP::CARICA, percorso_file_Tessere.string().c_str());
							break;
						}
					}
					nome_1.clear();
				}
			}
			else if (is_regular_file(percorso_file_Tessere) && percorso_file_Tessere.extension() == ".txt")
			{
				ifstream leggi = ifstream(percorso_file_Tessere.string());
				int numero_letto = 0;
				leggi >> numero_letto;
				while (numero_letto != -1) //codice di fine lettura zone
				{
					Tessera_x.n_zona.push_back(numero_letto);
					leggi >> numero_letto; Tessera_x.x_zona.push_back(numero_letto);
					leggi >> numero_letto; Tessera_x.y_zona.push_back(numero_letto);
					leggi >> numero_letto; Tessera_x.x_dim_zona.push_back(numero_letto);
					leggi >> numero_letto; Tessera_x.y_dim_zona.push_back(numero_letto);
					leggi >> numero_letto;
					if (numero_letto == 0)
						Tessera_x.cercabile.push_back(false);
					else
						Tessera_x.cercabile.push_back(true);
					vector<bool> fusione;
					for (int i = 0; i < 4; i++)
					{
						leggi >> numero_letto;
						if (numero_letto == 0) //false
							fusione.push_back(false);
						else
							fusione.push_back(true);
					}
					Tessera_x.direzione_fusione.push_back(fusione);
					leggi >> numero_letto; Tessera_x.fila_della_zona.push_back(numero_letto);
					fusione.clear(); //lo uso anche per vedere se ci sono mura nelle sue 4 direzioni
					for (int i = 0; i < 4; i++)
					{
						leggi >> numero_letto;
						if (numero_letto == 0) //false
							fusione.push_back(false);
						else
							fusione.push_back(true);
					}
					Tessera_x.direzione_mura.push_back(fusione);

					leggi >> numero_letto;
				}
				leggi >> numero_letto;
				while (numero_letto != -1) //codice di fine lettura porte
				{
					Tessera_x.x_porta.push_back(numero_letto);
					leggi >> numero_letto; Tessera_x.y_porta.push_back(numero_letto);
					vector<int> zone_porte;
					leggi >> numero_letto; zone_porte.push_back(numero_letto);
					leggi >> numero_letto; zone_porte.push_back(numero_letto);
					Tessera_x.zone_porta.push_back(zone_porte);
					leggi >> numero_letto;
				}
				leggi >> numero_letto;
				while (numero_letto != -2) //Codice di fine lettura tombini e anche del file
				{
					Tessera_x.zone_tombini.push_back(numero_letto);
					leggi >> numero_letto;
				}
			}
			file_Tessere++; //incrementa l'iteratore per cercare il prossimo file
		}
		t.parametri_per_angolatura.push_back(Tessera_x); //parametri 0 gradi
														 //creo copia di parametri 0, li ruoto di 90 con la funzione e li push back 					 
		aggiungi_parametri_ruotati_di_oggetto_tessere_editor
		(t.parametri_per_angolatura[0], Tessera_x, t.immagini_in_varie_angolature[0]->height);
		t.parametri_per_angolatura.push_back(Tessera_x);
		//90 gradi a dx l'oggetto già ruotato di 90 --> 180
		aggiungi_parametri_ruotati_di_oggetto_tessere_editor
		(Tessera_x, Tessera_x, t.immagini_in_varie_angolature[0]->height);
		t.parametri_per_angolatura.push_back(Tessera_x);
		//90 gradi a dx l'oggetto già ruotato di 180 --> 270
		aggiungi_parametri_ruotati_di_oggetto_tessere_editor
		(Tessera_x, Tessera_x, t.immagini_in_varie_angolature[0]->height);
		t.parametri_per_angolatura.push_back(Tessera_x);

		stato.Elenco_Tessere.push_back(t);
		iteratore_cartelle_Tessere++; //incrementa l'iteratore per cercare la prossima cartella
		contatore_tessere++;
	}
	Tessere_editor vuota;
	vuota.nome = "VUOTA";
	MATTEO_BITMAP* m_b=MATTEO_BITMAP::create();
	m_b->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/Tessera_vuota_editor.png");
	for (int i = 0; i < 4; i++)
	{
		vuota.immagini_in_varie_angolature.push_back(m_b);
	}
	stato.Elenco_Tessere.push_back(vuota);
}
void carica_personaggi_da_file()
{
	boost::filesystem::directory_iterator end;
	boost::filesystem::path percorso_personaggi("Personaggi");
	boost::filesystem::directory_iterator iteratore_cartelle_personaggi(percorso_personaggi);
	while (iteratore_cartelle_personaggi != end)
	{
		boost::filesystem::path percorso_cartella_personaggio = *iteratore_cartelle_personaggi;
		boost::filesystem::directory_iterator file_personaggi(percorso_cartella_personaggio);
		while (file_personaggi != end)
		{
			boost::filesystem::path percorso_file_personaggio = *file_personaggi;
			if (is_regular_file(percorso_file_personaggio) && percorso_file_personaggio.extension() == ".txt")
				stato.elenco_personaggi.push_back(percorso_file_personaggio.stem().string());
			file_personaggi++;
		}
		iteratore_cartelle_personaggi++;
	}
	Personaggio p;
	for (size_t i = 0; i < stato.elenco_personaggi.size(); i++)
		stato.personaggio.push_back(p);
	string stringa;
	vector<string> colori;
	colori.push_back("blu"); colori.push_back("giallo"); colori.push_back("arancione"); colori.push_back("rosso");
	for (size_t iteratore_file_personaggi=0; iteratore_file_personaggi < stato.elenco_personaggi.size(); iteratore_file_personaggi++)
	{
		string percorso_txt_personaggi = "Personaggi/" + stato.elenco_personaggi[iteratore_file_personaggi] + "/" + stato.elenco_personaggi[iteratore_file_personaggi] + ".txt";
		string percorso_scheda_personaggio = "Personaggi/" + stato.elenco_personaggi[iteratore_file_personaggi] + "/" + stato.elenco_personaggi[iteratore_file_personaggi] + ".png";
		string percorso_esperienza_personaggio = "Personaggi/" + stato.elenco_personaggi[iteratore_file_personaggi] + "/" +
			"esperienza "+stato.elenco_personaggi[iteratore_file_personaggi] + ".png";
		string percorso_segnalino_segna_esperienza = "Personaggi/" + stato.elenco_personaggi[iteratore_file_personaggi] + "/" +
			"segna esperienza "+ stato.elenco_personaggi[iteratore_file_personaggi] + ".png";
		string percorso_pedina_personaggio = "Personaggi/" + stato.elenco_personaggi[iteratore_file_personaggi] + "/" + 
			"pedina " + stato.elenco_personaggi[iteratore_file_personaggi] + ".png";

		stato.personaggio[iteratore_file_personaggi].immagine_scheda = MATTEO_BITMAP::create();
		stato.personaggio[iteratore_file_personaggi].immagine_scheda->init(0,0,MATTEO_BITMAP::CARICA, percorso_scheda_personaggio.c_str());
		stato.personaggio[iteratore_file_personaggi].immagine_solo_personaggio_senza_scheda = MATTEO_BITMAP::create();
		stato.personaggio[iteratore_file_personaggi].immagine_solo_personaggio_senza_scheda->init
			(stato.personaggio[iteratore_file_personaggi].immagine_scheda->width * 27 / 100,
				stato.personaggio[iteratore_file_personaggi].immagine_scheda->height * 75 / 100,
				MATTEO_BITMAP::CREA, "");
		stato.personaggio[iteratore_file_personaggi].immagine_esperienza = MATTEO_BITMAP::create();
		stato.personaggio[iteratore_file_personaggi].immagine_esperienza->init(0, 0, MATTEO_BITMAP::CARICA, percorso_esperienza_personaggio.c_str());
		stato.personaggio[iteratore_file_personaggi].immagine_pedina = MATTEO_BITMAP::create();
		stato.personaggio[iteratore_file_personaggi].immagine_pedina->init(0, 0, MATTEO_BITMAP::CARICA, percorso_pedina_personaggio.c_str());
		stato.personaggio[iteratore_file_personaggi].immagine_segna_esperienza = MATTEO_BITMAP::create();
		stato.personaggio[iteratore_file_personaggi].immagine_segna_esperienza->init(0, 0, MATTEO_BITMAP::CARICA, percorso_segnalino_segna_esperienza.c_str());

		ifstream leggi = ifstream(percorso_txt_personaggi);
		stringa = "";
		while (stringa != "stop_personaggio")
		{
			leggi >> stringa;
			if (stringa == "nome")
			{
				leggi >> stringa;
				while (stringa != "stop_nome")
				{
					if (stringa != "_")
						stato.personaggio[iteratore_file_personaggi].nome = stato.personaggio[iteratore_file_personaggi].nome + stringa;
					else
						stato.personaggio[iteratore_file_personaggi].nome = stato.personaggio[iteratore_file_personaggi].nome + " ";
					leggi >> stringa;
				}
			}
			for (size_t i = 0; i < colori.size(); i++)
			{
				if (stringa == colori[i])
				{
					leggi >> stringa;
					while (stringa != "stop_"+colori[i])
					{
						for (size_t j = 0; j < stato.elenco_abilità.size(); j++)
							if (stato.elenco_abilità[j].nome == stringa)
							{
								stato.personaggio[iteratore_file_personaggi].abilità[i].push_back(stato.elenco_abilità[j]);
								leggi >> stringa;
							}
					}
				}
			}
		}
	}
}
void carica_carte_spaw_da_file()
{
	boost::filesystem::directory_iterator end;
	boost::filesystem::path percorso_carte_spawn("Carte spawn");
	boost::filesystem::directory_iterator iteratore_cartelle_carte_spawn(percorso_carte_spawn);
	while (iteratore_cartelle_carte_spawn != end)
	{
		boost::filesystem::path percorso_cartella_carte_spawn = *iteratore_cartelle_carte_spawn;
		boost::filesystem::directory_iterator file_carte_spawn(percorso_cartella_carte_spawn);
		while (file_carte_spawn != end)
		{
			boost::filesystem::path percorso_file_carte_spawn = *file_carte_spawn;
			if (is_regular_file(percorso_file_carte_spawn) && percorso_file_carte_spawn.extension() == ".txt")
				stato.elenco_carte_spawn.push_back(percorso_file_carte_spawn.stem().string());
			file_carte_spawn++;
		}
		iteratore_cartelle_carte_spawn++;
	}
	int numero=0;
	Carta_zombie carta;
	stato.Mazzo_spawn_originale.clear();
	stato.Mazzo_spawn.clear();
	for (size_t iteratore_carte_spawn = 0; iteratore_carte_spawn < stato.elenco_carte_spawn.size(); iteratore_carte_spawn++)
	{
		string percorso_txt_carte_spawn = "Carte spawn/" + stato.elenco_carte_spawn[iteratore_carte_spawn] + "/" + stato.elenco_carte_spawn[iteratore_carte_spawn] + ".txt";
		string percorso_immagine_carte_spawn = "Carte spawn/" + stato.elenco_carte_spawn[iteratore_carte_spawn] + "/" + stato.elenco_carte_spawn[iteratore_carte_spawn] + ".bmp";
		carta.Immagine = MATTEO_BITMAP::create();
		carta.Immagine->init(0,0,MATTEO_BITMAP::CARICA, percorso_immagine_carte_spawn.c_str());
		carta.numero = stoi(stato.elenco_carte_spawn[iteratore_carte_spawn]);
		ifstream leggi = ifstream(percorso_txt_carte_spawn);
		leggi >> numero; carta.tipologia = numero;
		leggi >> numero; carta.zombie[0][0] = numero; leggi >> numero; carta.zombie[0][1]=numero;
		leggi >> numero; carta.zombie[1][0] = numero; leggi >> numero; carta.zombie[1][1]=numero;
		leggi >> numero; carta.zombie[2][0] = numero; leggi >> numero; carta.zombie[2][1]=numero;
		leggi >> numero; carta.zombie[3][0] = numero; leggi >> numero; carta.zombie[3][1]=numero;
		stato.Mazzo_spawn_originale.push_back(carta);
	}
	stato.Mazzo_spawn = stato.Mazzo_spawn_originale;
}
void carica_configurazioni_da_file()
{
	stato.indicatore.risoluzioni.clear();
	stato.indicatore.risoluzioni.push_back({ 800,600 });
	stato.indicatore.risoluzioni.push_back({ 1280,720 });
	stato.indicatore.risoluzioni.push_back({ 1366,768 });
	stato.indicatore.risoluzioni.push_back({ 1600,900 });
	stato.indicatore.risoluzioni.push_back({ 1920,1080 });
	ifstream leggi = ifstream("configurazioni.txt");
	string stringa="";
	int numero;
	while (stringa != "fine_file")
	{
		leggi >> stringa;
		if (stringa == "risoluzione")
		{
			leggi >> numero; stato.indicatore.risoluzione[0] = numero;
			leggi >> numero; stato.indicatore.risoluzione[1] = numero;
		}
		if (stringa == "schermo")
		{
			leggi >> stringa;
			stato.indicatore.schermo = stringa;
		}
		if (stringa == "font")
		{
			leggi >> stringa;
			for (size_t i = 0; i < stato.font.size(); i++)
				if (stato.font[i].nome == stringa)
					stato.Cursori["Menu opzioni"]["Fonts"].iteratore = i;
		}
	}
	for (size_t i = 0; i < stato.indicatore.risoluzioni.size(); i++)
		if (stato.indicatore.risoluzione[0] == stato.indicatore.risoluzioni[i][0] && stato.indicatore.risoluzione[1] == stato.indicatore.risoluzioni[i][1])
			stato.Cursori["Menu opzioni"]["Risoluzioni"].iteratore = i;
	if (stato.indicatore.schermo == "full-screen")
		stato.Cursori["Menu opzioni"]["Schermo"].iteratore = 0;
	else if (stato.indicatore.schermo == "windowed")
		stato.Cursori["Menu opzioni"]["Schermo"].iteratore = 1;
	stato.Cursori["Menu opzioni"]["Fonts_provvisorio"].iteratore = stato.Cursori["Menu opzioni"]["Fonts"].iteratore;
}
void carica_fonts()
{
	vector<string> stringhe_percorsi;
	boost::filesystem::directory_iterator end;
	boost::filesystem::path percorso_fonts("Fonts");
	boost::filesystem::directory_iterator iteratore_fonts(percorso_fonts);
	while (iteratore_fonts != end)
	{
		boost::filesystem::path percorso_file_ttf = *iteratore_fonts;
		fonts font;
		if (is_regular_file(percorso_file_ttf) && percorso_file_ttf.extension() == ".ttf")
		{
			font.nome = percorso_file_ttf.stem().string(); //segno il nome. Esempio: Arial
			stringhe_percorsi.push_back(percorso_file_ttf.string());
			int x = 5;
			for (int i = 0; i < 30; i++)
			{
				font.allegro_font.push_back(al_load_ttf_font(stringhe_percorsi[stringhe_percorsi.size()-1].c_str(), x, 0)); //carico il font ad una certa dimensione
				font.height_dim.push_back(al_get_font_line_height(font.allegro_font[i])); //mi segno l'altezza
				font.dim_font.push_back(x); //mi segno la dim. Esempio: Arial_25 -> 25
				x = x + 10;
			}
			stato.font.push_back(font);
		}
		iteratore_fonts++;
	}
}
void logic_menu(ALLEGRO_EVENT event)
{
	vector<int> vettore_input;
	vector<int> input_numero_personaggi;
	int finito=0;
	bool in_posizione = false;
	bool in_posizione_su_triangoli_scorrimento = false;
	switch (stato.Stato_menu)
	{
	case 0:  //Menu principale
		input_tastiera_classico(stato.Cursori["Menu iniziale"]["Lista"].iteratore, event, {5}, false, true, false, false);
		in_posizione=input_mouse(5, cordinate_elenco_scritte(700, 500, 500, 300, 5, 1, 60, 0, 0), stato.Cursori["Menu iniziale"]["Lista"],true,
			stato.indicatore.risoluzione,event.type==ALLEGRO_EVENT_MOUSE_AXES);
		if (event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
		{
			if (event.keyboard.keycode == ALLEGRO_KEY_ENTER || (in_posizione==true&&event.mouse.button==1))
			{
				switch (stato.Cursori["Menu iniziale"]["Lista"].iteratore)
				{
				case 0://scelto missioni
					stato.Stato_menu++;
					break;
				case 1://scelto opzioni
					stato.Stato_menu = 6;
					break;
				case 2://scelto guarda tasti
					stato.Stato_menu = 7;
					break;
				case 3://scelto editor
					stato.Stato_menu = 8;
					break;
				case 4: //scelto esci
					shutdown();
					abort_game("");
					break;
				}
			}
		}
		break;
	case 1://hai selezionato segli missione: segli quale
		if (stato.missioni.size() > 0)
		{
			input_tastiera_classico(stato.Cursori["Scegli Missione"]["Lista"].iteratore, event, { (int)stato.missioni.size() }, false, true, false, false);
			in_posizione=input_mouse((int)stato.missioni.size(), cordinate_elenco_scritte(130, 300, 700, 700, (int)stato.missioni.size(), 1, 100, 0, 100),
				stato.Cursori["Scegli Missione"]["Lista"],true, stato.indicatore.risoluzione, event.type == ALLEGRO_EVENT_MOUSE_AXES);
			if (event.type == ALLEGRO_EVENT_KEY_DOWN||event.type==ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
			{
				if (event.keyboard.keycode == ALLEGRO_KEY_ENTER || (in_posizione==true&&event.mouse.button==1))
				{
					stato.Stato_menu++;
					MATTEO_BITMAP::destroy_vector_m_bitmap();
					stato.missione = stato.missioni[stato.Cursori["Scegli Missione"]["Lista"].iteratore];
					stato.missione.mappa->init((1920 - immagini_e_fonts.collage_schede_personaggi->width - 318), 1080, MATTEO_BITMAP::CREA, "");
					stato.Cursori["Scegli Missione"]["Lista"].iteratore = 0;
					percorsi(stato);
				}
				if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
				{
					stato.Stato_menu--;
					stato.Cursori["Scegli Missione"]["Lista"].iteratore = 0;
				}
			}
		}
		else
			if (event.type == ALLEGRO_EVENT_KEY_DOWN&&event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			{
				stato.Stato_menu--;
				stato.Cursori["Scegli Missione"]["Lista"].iteratore = 0;
			}
		break;
	case 2://scegli numero personaggi, max 6
		input_tastiera_classico(stato.Cursori["Scegli numero pesonaggi"]["Lista"].iteratore, event, { 6 }, false, true, false, false);
		in_posizione = input_mouse(6, cordinate_elenco_scritte(900, 600, 200,200, 6, 1, 200, 0, 0), stato.Cursori["Scegli numero pesonaggi"]["Lista"],true,
			stato.indicatore.risoluzione, event.type == ALLEGRO_EVENT_MOUSE_AXES);
		if (event.type == ALLEGRO_EVENT_KEY_DOWN || event.type==ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
		{
			if (event.keyboard.keycode == ALLEGRO_KEY_ENTER||(in_posizione==true&&event.mouse.button==1))
			{
				stato.Stato_menu++;
				stato.numero_giocatori = stato.Cursori["Scegli numero pesonaggi"]["Lista"].iteratore + 1;
				stato.Cursori["Scegli numero pesonaggi"]["Lista"].iteratore = 0;
				stato.personaggi_selezionabili = stato.elenco_personaggi;
				giocatore_numero = 0;
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			{
				stato.Stato_menu --;
				stato.Cursori["Scegli numero pesonaggi"]["Lista"].iteratore = 0;
			}
		}
		break;
	case 3://selezione personaggi
		input_tastiera_classico(stato.Cursori["Scegli pesonaggi"]["Lista"].iteratore, event, {(int)stato.personaggi_selezionabili.size()}, false, false, false, false);
		{ //tutto il suo particolare input con mouse
			vector<vector<int>> v;
			v.push_back({ 876,824,876 + 172,824 + 274 });
			Cursore c;
			in_posizione = input_mouse(1,v,c, false, stato.indicatore.risoluzione, event.type == ALLEGRO_EVENT_MOUSE_AXES);
			v.clear();
			if (stato.Cursori["Scegli pesonaggi"]["Lista"].iteratore == 0)
			{
				v.push_back({});
				v.push_back({ 1548,943,1548 + 100,943 + 100 });
			}
			else if (stato.Cursori["Scegli pesonaggi"]["Lista"].iteratore == (int)stato.personaggi_selezionabili.size() - 1)
			{
				v.push_back({ 276,943,276 + 100,943 + 100 });
				v.push_back({});
			}
			else
			{
				v.push_back({ 276,943,276 + 100,943 + 100 });
				v.push_back({ 1548,943,1548 + 100,943 + 100 });
			}
			Cursore c1;
			in_posizione_su_triangoli_scorrimento =
				input_mouse((int)stato.personaggi_selezionabili.size(), v, c1, false, stato.indicatore.risoluzione, event.type == ALLEGRO_EVENT_MOUSE_AXES);
			if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN&&in_posizione_su_triangoli_scorrimento == true && event.mouse.button == 1)
				switch (c1.iteratore)
				{
				case 0: //triangolo scorrimento sx
					stato.Cursori["Scegli pesonaggi"]["Lista"].iteratore--;
					break;
				case 1://troangolo scorrimento dx
					stato.Cursori["Scegli pesonaggi"]["Lista"].iteratore++;
					break;
				}
		} //fine input da mouse
		if (event.type == ALLEGRO_EVENT_KEY_DOWN||event.type==ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
		{
			if (event.keyboard.keycode == ALLEGRO_KEY_ENTER||(in_posizione==true&&event.mouse.button==1))
			{
				for (size_t i = 0; i < stato.elenco_personaggi.size(); i++)
					if (stato.elenco_personaggi[i] == stato.personaggi_selezionabili[stato.Cursori["Scegli pesonaggi"]["Lista"].iteratore])
						stato.giocatore[giocatore_numero].personaggio = stato.personaggio[i].nome;

				stato.giocatore[giocatore_numero].stato = 1;
				for (size_t i = 0; i < stato.missione.dati.size(); i++)
					if (stato.missione.dati[i].giocatori[giocatore_numero] == true)
						stato.giocatore[giocatore_numero].posizione = i;
				stato.personaggi_selezionabili.erase(stato.personaggi_selezionabili.begin()+ stato.Cursori["Scegli pesonaggi"]["Lista"].iteratore);
				if (stato.Cursori["Scegli pesonaggi"]["Lista"].iteratore == stato.personaggi_selezionabili.size())
				{
					stato.Cursori["Scegli pesonaggi"]["Lista"].iteratore = stato.personaggi_selezionabili.size() - 1;
					if (stato.Cursori["Scegli pesonaggi"]["Lista"].modificatore_scorrimento_elenco > (int)stato.personaggi_selezionabili.size() - 1)
						stato.Cursori["Scegli pesonaggi"]["Lista"].modificatore_scorrimento_elenco = stato.personaggi_selezionabili.size() - 1;
				}
				giocatore_numero++;
				if (stato.numero_giocatori - giocatore_numero == 0) //finito
				{
					for (int i = 0; i < stato.numero_giocatori; i++)
						for (size_t j = 0; j < stato.elenco_personaggi.size(); j++)
							if (stato.giocatore[i].personaggio==stato.elenco_personaggi[j]&&stato.giocatore[i].stato==1)
							{
								stato.giocatore[i].Scheda_personaggio_con_equip = MATTEO_BITMAP::create();
								stato.giocatore[i].Scheda_personaggio_con_equip->init(stato.personaggio[j].immagine_scheda->width,
									stato.personaggio[j].immagine_scheda->height, MATTEO_BITMAP::CREA, "");
								stato.giocatore[i].Barra_esperienza_con_segnalino = MATTEO_BITMAP::create();
								stato.giocatore[i].Barra_esperienza_con_segnalino->init(stato.personaggio[j].immagine_esperienza->width,
									stato.personaggio[j].immagine_esperienza->height, MATTEO_BITMAP::CREA, "");
							}
					stato.giocatore[6].Scheda_personaggio_con_equip = MATTEO_BITMAP::create();
					stato.giocatore[6].Scheda_personaggio_con_equip->init(immagini_e_fonts.scheda_personaggio->width,
						immagini_e_fonts.scheda_personaggio->height, MATTEO_BITMAP::CREA, "");
					stato.Cursori["Scegli pesonaggi"]["Lista"].iteratore = 0;
					stato.Stato_menu ++;
				}
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			{
				stato.Stato_menu --;
				for (int i = 0; i < 6; i++)
					stato.giocatore[i].stato = 0;
				stato.Cursori["Scegli pesonaggi"]["Lista"].iteratore = 0;
			}
		}
		break;
	case 4: //assegna abilità iniziali
		for (int i = stato.Cursori["Aggiungi abilità"]["Lista giocatori"].iteratore; i < stato.numero_giocatori; i++)
			if (aggiungi_abilità_giocatore(stato, event, i) == true)
				stato.Cursori["Aggiungi abilità"]["Lista giocatori"].iteratore++;
			else
				break;
		if (stato.Cursori["Aggiungi abilità"]["Lista giocatori"].iteratore == stato.numero_giocatori)
		{
			stato.Cursori["Aggiungi abilità"]["Lista giocatori"].iteratore = 0;
			stato.Stato_menu++;
			assegna_armi_iniziali();
		}
		break;
	case 5://seleziona il primo giocatore
		switch (stato.Stato_primo_giocatore)
		{
		case 0: //Menù di selezione tra: Scegli primo giocatore(0), esplora mappa(1)
			input_tastiera_classico(stato.Cursori["Schermata preliminare"]["Lista"].iteratore, event, {2}, false, true, false, false);
			in_posizione = input_mouse(2,cordinate_elenco_scritte(1500+70,100,230,500,2,1,50,0,50), stato.Cursori["Schermata preliminare"]["Lista"],false,
				stato.indicatore.risoluzione, event.type == ALLEGRO_EVENT_MOUSE_AXES);
			if (event.type == ALLEGRO_EVENT_KEY_DOWN||event.type==ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
			{
				if (event.keyboard.keycode == ALLEGRO_KEY_ENTER || event.mouse.button==1)
				{
					if (stato.Cursori["Schermata preliminare"]["Lista"].iteratore == 0)
					{
						stato.missione.mappa->init((1920 - immagini_e_fonts.collage_schede_personaggi->width - 318),1080, MATTEO_BITMAP::CREA, "");
						stato.Stato_primo_giocatore++;
					}
					else
					{
						stato.missione.mappa->init(1497, 1040, MATTEO_BITMAP::CREA, "");
						stato.Stato_primo_giocatore = stato.Stato_primo_giocatore + 2;
					}
					
					
				}
				if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
				{
					stato.Cursori["Schermata preliminare"]["Lista"].iteratore = 0;
					for (int i = 0; i < 6; i++)
					{
						stato.giocatore[i].equipaggiamento.clear();
						stato.giocatore[i].abilità.clear();
						stato.giocatore[i].stato = 0;
					}
					stato.Stato_menu = 0;
					stato.Mazzo_equipaggiamento = stato.Mazzo_equipaggiamento_originale;
					stato.Mazzo_speciale = stato.Mazzo_speciale_originale;
				}
			}
			break;
		case 1:	// Seleziona il primo giocatore
		{
			//vettore cordinate delle scede dei personaggi
			vector<vector<int>> cordinate;
			vector<int> colonnexrighe;
			int n = 0;
			if (stato.numero_giocatori < 3)
			{
				colonnexrighe.push_back(stato.numero_giocatori);
			}
			else if (stato.numero_giocatori < 5)
			{
				colonnexrighe.push_back(2);
				colonnexrighe.push_back(stato.numero_giocatori - 2);
			}
			else
			{
				colonnexrighe.push_back(2);
				colonnexrighe.push_back(2);
				colonnexrighe.push_back(stato.numero_giocatori - 4);
			}
			int x = 0, y = 0, w = 640, h = 366;
			for (size_t i = 0; i < colonnexrighe.size(); i++, y = y + 366, h = h + 366)
			{
				for (int j = 0; j <colonnexrighe[i]; j++, x = x + 640, w = w + 640)
					cordinate.push_back({ x,y,w,h });
				x = 0, w = 640;
			}
			in_posizione = input_mouse(stato.numero_giocatori, cordinate, stato.Cursori["Scelta primo giocatore"]["Lista"], false, stato.indicatore.risoluzione,
				event.type == ALLEGRO_EVENT_MOUSE_AXES);
		}
			if (event.type == ALLEGRO_EVENT_KEY_DOWN || event.type==ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
			{
				if (event.keyboard.keycode == ALLEGRO_KEY_UP)
				{
					if (stato.Cursori["Scelta primo giocatore"]["Lista"].iteratore > 1)
						stato.Cursori["Scelta primo giocatore"]["Lista"].iteratore = stato.Cursori["Scelta primo giocatore"]["Lista"].iteratore - 2;
				}
				if (event.keyboard.keycode == ALLEGRO_KEY_DOWN)
				{

					if (stato.Cursori["Scelta primo giocatore"]["Lista"].iteratore < stato.numero_giocatori - 1 - 1)
						stato.Cursori["Scelta primo giocatore"]["Lista"].iteratore = stato.Cursori["Scelta primo giocatore"]["Lista"].iteratore + 2;
				}
				if (event.keyboard.keycode == ALLEGRO_KEY_LEFT)
				{
					if (stato.Cursori["Scelta primo giocatore"]["Lista"].iteratore % 2 != 0)
						stato.Cursori["Scelta primo giocatore"]["Lista"].iteratore--;
				}
				if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT)
				{
					if (stato.Cursori["Scelta primo giocatore"]["Lista"].iteratore % 2 == 0)
						stato.Cursori["Scelta primo giocatore"]["Lista"].iteratore++;
				}
				if (event.keyboard.keycode == ALLEGRO_KEY_ENTER || (event.mouse.button==1&&in_posizione==true))
				{
					stato.ordine_giocatori.clear();
					for (int i = 0; i < stato.numero_giocatori; i++)
					{
						if (stato.Cursori["Scelta primo giocatore"]["Lista"].iteratore + i < stato.numero_giocatori)
							stato.ordine_giocatori.push_back(stato.Cursori["Scelta primo giocatore"]["Lista"].iteratore + 1 + i);
						if (stato.Cursori["Scelta primo giocatore"]["Lista"].iteratore + i >= stato.numero_giocatori)
							stato.ordine_giocatori.push_back(stato.Cursori["Scelta primo giocatore"]["Lista"].iteratore + 1 + i - stato.numero_giocatori);
						stato.giocatore[i].stato = 1;
					}
					stato.Stato_primo_giocatore = 0;
					stato.indicatore.giocatore_di_turno = -1;
					stato.Cursori["Scelta primo giocatore"]["Lista"].iteratore = 0;
					stato.Cursori["Schermata preliminare"]["Lista"].iteratore = 0;
					stato.Stato_gioco = 1;
					stato.Stato_menu = 0;
					stato.Stato_gioco_gioco = 1;
					for (int i = 0; i < stato.numero_giocatori; i++)
						for (size_t j = 0; j < stato.giocatore[i].abilità.size(); j++)
							if (stato.giocatore[i].abilità[j].quando_si_scarica == Abilità::A_INIZIO_TURNO_GIOCATORE)
								stato.giocatore[i].abilità[j].funzione
									(stato, i, j, Abilità::CODICE_SCARICA);
				}
				if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
				{
					stato.Stato_primo_giocatore --;
					stato.Cursori["Scelta primo giocatore"]["Lista"].iteratore = 0;
				}
			}
			break;
		case 2:	//esplora la mappa
			logic_zoom_e_movimento_mappa(stato, stato.missione.w_h_mappa[0], stato.missione.w_h_mappa[1]);
			if (event.type == ALLEGRO_EVENT_KEY_DOWN&&(event.keyboard.keycode == ALLEGRO_KEY_ESCAPE|| event.keyboard.keycode == ALLEGRO_KEY_ENTER))
					stato.Stato_primo_giocatore = 0;
			break;
		}
		break;
	case 6://opzioni
		if (menu_opzioni_logic(stato, event,immagini_e_fonts.display) == true)
			stato.Stato_menu = 0;
		break;
	case 7://lista tasti
		if (event.type == ALLEGRO_EVENT_KEY_DOWN||event.type==ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
			stato.Stato_menu = 0;
		break;
	case 8: //Editor
		MATTEO_BITMAP::destroy_vector_m_bitmap();
		stato.Stato_menu = 0;
		stato.Stato_gioco = 2;
		break;
	}
}
void logic_gioco(ALLEGRO_EVENT event)
{
	Missione mx;
	vector<DatiStanza> copia_dati;
	int risultato;
	vector<int> azioni_disponibili_input;
	switch (stato.Stato_gioco_gioco)
	{
	case 0:// Inizio turno giocatori
		   //nuovo ordine giocatori
		stato.indicatore.giocatore_di_turno = -1; //così poi diventa 0 nel prox stato
		if (stato.indicatore.primo_giocatore_morto == false) //perchè se è morto, l'ordine è già giusto senza modificarlo
		{
			stato.ordine_giocatori.push_back(stato.ordine_giocatori[0]);
			stato.ordine_giocatori.erase(stato.ordine_giocatori.begin());
		}
		else
			stato.indicatore.primo_giocatore_morto = false;
		//altri reset
		ricarica_armi(stato);
		for (int i = 0; i < 6; i++)
			for (size_t j = 0; j < stato.giocatore[i].abilità.size(); j++)
				if (stato.giocatore[i].abilità[j].tipo == Abilità::FREE)
					stato.giocatore[i].abilità[j].funzione(stato, i, j, Abilità::CODICE_SCARICA);
		stato.Stato_gioco_gioco++;
	case 1://Preparazione Turno: giocatore di turno
		if (stato.indicatore.giocatore_di_turno < (int)stato.ordine_giocatori.size() - 1) //prossimo giocatore di turno
			stato.indicatore.giocatore_di_turno++;
		else
		{
			stato.Stato_gioco_gioco=4; //fine turno, tocca agli zombie
			stato.Stato_turno_zombie = 0;
			break;
		}
		stato.numero_azioni = 3;
		for (size_t i = 0; i < stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità.size(); i++)
			if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].nome == "one_action")
				stato.numero_azioni++;
		stato.indicatore.cercabile = true;
		for (size_t j = 0; j < stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno]-1].abilità.size(); j++)
			if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[j].quando_ricarica == Abilità::A_INIZIO_TURNO_GIOCATORE)
				stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[j].funzione
				(stato, stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1,j, Abilità::CODICE_RICARICA);
		for (size_t j = 0; j < stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità.size(); j++)
			if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[j].quando_si_scarica == Abilità::A_INIZIO_TURNO_GIOCATORE)
				stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[j].funzione
				(stato, stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1, j, Abilità::CODICE_SCARICA);
		stato.Stato_gioco_gioco++;
		stato.indicatore.sub_stato_turno_giocatori = 0;
		stato.Cursori["Turno giocatore"]["Scelta azioni"].iteratore = 0;
	case 2://Scelta azioni disponibili
		switch (stato.indicatore.sub_stato_turno_giocatori)
		{
		case 0: //aggiungi abilità + inquadra telecamera su giocatore
			if(aggiungi_abilità_giocatore(stato, event,stato.ordine_giocatori[stato.indicatore.giocatore_di_turno]-1)==true)
				stato.indicatore.sub_stato_turno_giocatori++;
			inquadra_zona_mappa(stato, stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione,stato.missione.w_h_mappa[0],stato.missione.w_h_mappa[1]);
			break;
		case 1://prosegui con le azioni
				//calcolo azioni disponibili e se non ce ne sono si torna indietro
			stato.elenco_azioni_disponibili.clear();
			stato.azioni_disponibili.clear();
						//Faccio partire abilità di azioni gratuite che mod. alcune caratteristiche delle azioni, che verranno poi ripristinate dopo l'azione durante la loro "scarica"
			for (size_t i = 0; i < stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità.size(); i++)
				if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].tipo == Abilità::FREE)
					stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].funzione
					(stato, stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1,i,Abilità::CODICE_ESEGUI);

			for (size_t j = 0; j < stato.elenco_azioni.size(); j++)
				if (stato.elenco_azioni[j].fattibile(stato) == true && stato.elenco_azioni[j].costo_di_azioni <= stato.numero_azioni)
					stato.azioni_disponibili.push_back(stato.elenco_azioni[j]);
			for (size_t i = 0; i < stato.azioni_disponibili.size(); i++)
				stato.elenco_azioni_disponibili.push_back(stato.azioni_disponibili[i].nome);
			
			if (stato.azioni_disponibili.size() == 0)
			{
				stato.Stato_gioco_gioco--;
				stato.indicatore.sub_stato_turno_giocatori = 0;
				break;
			}
			else if (stato.azioni_disponibili.size() == 1 && stato.azioni_disponibili[0].nome == "Termina")
			{
				//scarica alcune abilità, tenendo conto di scaricare un solo doppione
				scarica_free_action(stato,stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1);
				stato.Stato_gioco_gioco--;
				stato.indicatore.sub_stato_turno_giocatori = 0;
				break;
			}
				//input
			logic_zoom_e_movimento_mappa(stato, stato.missione.w_h_mappa[0], stato.missione.w_h_mappa[1]);
			logic_movimento_collage_schede_personaggi(stato.Cursori["Movimento schede personaggi"]["iteratore"].iteratore,
				(404*stato.numero_giocatori)-stato.indicatore.h_riquadro_mappa);
			for (int i = 0; i < (int)stato.elenco_azioni_disponibili.size(); i++)
				azioni_disponibili_input.push_back(1);
			input_tastiera_classico(stato.Cursori["Turno giocatore"]["Scelta azioni"].iteratore, event, { (int)azioni_disponibili_input.size() }, false, true, false, false);
			if (event.keyboard.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
					stato.indicatore.sub_stato_turno_giocatori++; //menu pausa
				if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
				{
					stato.Stato_gioco_gioco++;
					stato.indicatore.sub_stato_turno_giocatori = 0;
				}
			}
			if (event.keyboard.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode== ALLEGRO_KEY_TAB)
				mostra = true;
			if (event.keyboard.type == ALLEGRO_EVENT_KEY_UP && event.keyboard.keycode == ALLEGRO_KEY_TAB)
				mostra = false;
			break;
		case 2://menu pausa
			switch (stato.indicatore.sub_stato_menu_pausa)
			{
			case 0://scelta: opzioni e torna al menu principale
				input_tastiera_classico(stato.Cursori["Menu di pausa"]["Lista"].iteratore, event, { 3 }, false, true, false, false);
				if (event.type == ALLEGRO_EVENT_KEY_DOWN)
				{
					if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
					{
						stato.indicatore.sub_stato_turno_giocatori--;
						stato.Cursori["Menu di pausa"]["Lista"].iteratore = 0;
					}
					if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
					{
						switch (stato.Cursori["Menu di pausa"]["Lista"].iteratore)
						{
						case 0://menu opzioni
							stato.indicatore.sub_stato_menu_pausa++;
							break;
						case 1://lista tasti
							stato.indicatore.sub_stato_menu_pausa = 2;
							break;
						case 2://torna al menu principale
							stato.Cursori["Menu di pausa"]["Lista"].iteratore = 0;
							stato.indicatore.sub_stato_turno_giocatori = 0;
							stato.Stato_gioco_gioco = 0;
							stato.Stato_gioco = 0;
							stato.indicatore.sub_stato_menu_pausa = 0; 
							stato.Stato_primo_giocatore = 0;

							//scarica alcune abilità, tenendo conto di scaricare un solo doppione
							scarica_free_action(stato,stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1);
							
							Giocatore giocatore_vuoto;
							for (int i = 0; i < 6; i++)
								stato.giocatore[i] = giocatore_vuoto;
							inizializza_mazzo_equipaggiamento(stato, immagini_e_fonts);
							stato.missione=mx;
							MATTEO_BITMAP::destroy_vector_m_bitmap();
							break;
						}
					}
				}
				break;
			case 1://menu opzioni
				if (menu_opzioni_logic(stato, event, immagini_e_fonts.display) == true)
					stato.indicatore.sub_stato_menu_pausa--;
				break;
			case 2://lista tasti
				if (event.type == ALLEGRO_EVENT_KEY_DOWN)
					stato.indicatore.sub_stato_menu_pausa = 0;
				break;
			}
			break;
		}
		break;
	case 3: //Esecuzione azione
		logic_zoom_e_movimento_mappa(stato, stato.missione.w_h_mappa[0], stato.missione.w_h_mappa[1]);
		risultato=stato.azioni_disponibili[stato.Cursori["Turno giocatore"]["Scelta azioni"].iteratore].esegui(stato, event);
		switch (risultato)
		{
		case -1://annulla
			stato.Stato_gioco_gioco--;
			break;
		case 0: //non ha finito
			break;
		case 1://ha finito
			stato.numero_azioni = stato.numero_azioni - stato.azioni_disponibili[stato.Cursori["Turno giocatore"]["Scelta azioni"].iteratore].costo_di_azioni;
			if (stato.numero_azioni < 0) //passa il turno
				stato.Stato_gioco_gioco = 1;
			else if (stato.numero_azioni >= 0)//ricalcola azioni dispoinibili
				stato.Stato_gioco_gioco--;

			if (stato.indicatore.giocatore_di_turno >= 0)
			{
				//scarica alcune abilità, tenendo conto di scaricare un solo doppione
				vector<string> lista_abilità_da_scaricare;
				for (size_t i = 0; i < stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità.size(); i++)
				{
					if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].scarica == false &&
						stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].quando_si_scarica == Abilità::DOPO_ESECUZIONE_AZIONE)
					{
						bool nessuna_corrispondenza = true;
						for (size_t j = 0; j < lista_abilità_da_scaricare.size(); j++)
							if (lista_abilità_da_scaricare[j] == stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].nome)
								nessuna_corrispondenza = false;
						if (nessuna_corrispondenza == true)
						{
							lista_abilità_da_scaricare.push_back(stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].nome);
							stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].
								funzione(stato, stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1, i, Abilità::CODICE_SCARICA);
						}
					}
				}
			}
			//fine
			stato.Cursori["Turno giocatore"]["Scelta azioni"].iteratore = 0;
			break;
		}
		break;
	case 4: //turno zombie
		logic_zoom_e_movimento_mappa(stato, stato.missione.w_h_mappa[0], stato.missione.w_h_mappa[1]);
		switch (stato.Stato_turno_zombie)
		{
		case 0://ricaricare alcune abilità
			for (int i = 0; i < stato.numero_giocatori; i++)
				for (size_t j = 0; j < stato.giocatore[i].abilità.size(); j++)
					if (stato.giocatore[i].abilità[j].quando_ricarica == Abilità::A_INIZIO_TURNO_ZOMBIE)
						stato.giocatore[i].abilità[j].funzione(stato, i, j, Abilità::CODICE_RICARICA);
			stato.Stato_turno_zombie++;
			break;
		case 1:	//attivazione zombie
			if (attiva_zombie(stato, event, "tutti") == true)
				stato.Stato_turno_zombie ++;
			break;
		case 2: //generazione zombie
			if (logic_spawn_zombie(stato, event, stato.missione.elenco_spawn) == true)
			{
				for (size_t i = 0; i < stato.missione.dati.size(); i++)
					stato.missione.dati[i].SommaValoreRumore(-stato.missione.dati[i].rumore);
				stato.Stato_gioco_gioco = 0;
				stato.Stato_turno_zombie = 0;
			}
		}
		break;
	}
	if (stato.numero_giocatori == 0)
	{
		shutdown();
		abort_game("Fine gioco");
	}
}
bool logic_editor(ALLEGRO_EVENT event)
{
	Tessere_editor a;
	vector<Tessere_editor>va;
	bool tessere_tutte_collegate = true;			  //usate per verificare
	int tessere_inserite = 0;				 // se una mappa è salvabile
	bool tessera_collegata = false;			//
	int lunghezza_max_mappa = 0;
	int x_tessera = 0; int y_tessera = 0;
	vector<vector<Tessere_editor>> mm;
	string salva_immagine;
	int w = 0, h = 0;
	switch (stato.Cursori["Editor"]["Fasi"].iteratore)
	{
	case 0: //inizializzazione
		for (int i = 0; i < 9; i++)
			va.push_back(a);
		for (int j = 0; j < 9; j++)
			stato.missione_editor.Mappa_matrice.push_back(va);
		for (size_t j = 0; j < stato.Elenco_Tessere.size(); j++)
			if (stato.Elenco_Tessere[j].nome == "VUOTA")
				for (int i = 0; i < 9; i++)
					for (int k = 0; k < 9; k++)
						stato.missione_editor.Mappa_matrice[i][k] = stato.Elenco_Tessere[j];
		for (int i = 0; i < 6; i++)
			stato.giocatore[i].posizione = 0;
		stato.Cursori["Editor"]["Fasi"].iteratore++;	
		stato.missione_editor.mappa = MATTEO_BITMAP::create();
		stato.missione_editor.mappa->init(1600, 1080,MATTEO_BITMAP::CREA,"");
	case 1: //Editing
		logic_zoom_e_movimento_mappa(stato, 982*9,982*9);
		switch (stato.Cursori["Scelta tipo di editing"]["Elenco"].iteratore)
		{
		case 0: //Editing Tessere
			switch (stato.Cursori["Editing Tessere"]["Focus Cursore"].iteratore)
			{
			case 0://Scelta Tessere
				input_tastiera_classico(stato.Cursori["Editing Tessere"]["Scelta Tessere"].iteratore, event, { (int)stato.Elenco_Tessere.size() }, false, true, false, false);
				if (event.type == ALLEGRO_EVENT_KEY_DOWN)
				{
					if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT)
					{
						stato.Cursori["Scelta tipo di editing"]["Elenco"].iteratore++;
					}
					if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
					{
						stato.Cursori["Editing Tessere"]["Focus Cursore"].iteratore++;
					}
					if (event.keyboard.keycode == ALLEGRO_KEY_SPACE)
					{
						if (stato.Elenco_Tessere[stato.Cursori["Editing Tessere"]["Scelta Tessere"].iteratore].indice_angolo_da_usare < 3)
							stato.Elenco_Tessere[stato.Cursori["Editing Tessere"]["Scelta Tessere"].iteratore].indice_angolo_da_usare++;
						else
							stato.Elenco_Tessere[stato.Cursori["Editing Tessere"]["Scelta Tessere"].iteratore].indice_angolo_da_usare = 0;
					}
					if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
					{
						stato.Cursori["Editor"]["Fasi"].iteratore = 2;
					}
				}
				break;
			case 1://Inserisci tessera in mappa
				input_tastiera_classico(stato.Cursori["Editing Tessere"]["Inserisci in mappa"].iteratore, event, { 9,9,9,9,9,9,9,9,9 }, false, false, false, false);
				if (event.type == ALLEGRO_EVENT_KEY_DOWN)
				{
					if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
					{
						stato.Mappa_editor[stato.Cursori["Editing Tessere"]["Inserisci in mappa"].iteratore] =
							stato.Elenco_Tessere[stato.Cursori["Editing Tessere"]["Scelta Tessere"].iteratore];

						unisci_tessere_a_formare_mappa_editor(stato.Mappa_editor, stato.missione_editor, stato.porte_inseribili_editor);
						for (int i = 0; i < 6; i++)
							stato.giocatore[i].posizione=0;
						percorsi(stato);
						stato.Cursori["Editing Tessere"]["Focus Cursore"].iteratore--;
					}
					if (event.keyboard.keycode == ALLEGRO_KEY_DELETE)
					{
						for (size_t j = 0; j < stato.Elenco_Tessere.size(); j++)
							if (stato.Elenco_Tessere[j].nome == "VUOTA")
							{
								stato.Mappa_editor[stato.Cursori["Editing Tessere"]["Inserisci in mappa"].iteratore] = stato.Elenco_Tessere[j];
							}
					}
					if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
					{
						stato.Cursori["Editing Tessere"]["Focus Cursore"].iteratore--;
					}
				}
				break;
			}
			break;
		case 1: //Editing Oggetti
			switch (stato.Cursori["Editing Oggetti"]["Focus Cursore"].iteratore)
			{
			case 0: //Scelta Oggetto
				input_tastiera_classico(stato.Cursori["Editing Oggetti"]["Scelta Oggetto"].iteratore, event, { 20 }, false, true, false, false);
				if (event.type == ALLEGRO_EVENT_KEY_DOWN)
				{
					if (event.keyboard.keycode == ALLEGRO_KEY_LEFT)
					{
						stato.Cursori["Scelta tipo di editing"]["Elenco"].iteratore--;
					}
					if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
					{
						stato.Cursori["Editing Oggetti"]["Focus Cursore"].iteratore++;
					}
					if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
					{
						stato.Cursori["Editor"]["Fasi"].iteratore = 2;
					}
				}
				break;
			case 1: //Scegli zona o punto dove inserire una porta
				if (stato.Cursori["Editing Oggetti"]["Scelta Oggetto"].iteratore > 5) //Non hai scelto di mettere una porta. Quindi selezioni le zone
					input_tastiera_classico(stato.Cursori["Editing Oggetti"]["Scelta Zona"].iteratore, event, { (int)stato.missione_editor.dati.size() }, false, false, false, false);
				else
					input_tastiera_classico(stato.Cursori["Editing Oggetti"]["Scelta Zona"].iteratore, event, { (int)stato.porte_inseribili_editor.size() }, false, false, false, false);

				if (event.type == ALLEGRO_EVENT_KEY_DOWN)
				{
					if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
					{
						inserisci_oggetto_in_zona(stato, stato.Cursori["Editing Oggetti"]["Scelta Oggetto"].iteratore,
							stato.Cursori["Editing Oggetti"]["Scelta Zona"].iteratore, stato.missione, "inserisci");
					}
					if (event.keyboard.keycode == ALLEGRO_KEY_DELETE)
					{
						inserisci_oggetto_in_zona(stato, stato.Cursori["Editing Oggetti"]["Scelta Oggetto"].iteratore,
							stato.Cursori["Editing Oggetti"]["Scelta Zona"].iteratore, stato.missione, "elimina");
					}
					if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
					{
						stato.Cursori["Editing Oggetti"]["Focus Cursore"].iteratore--;
						stato.Cursori["Editing Oggetti"]["Scelta Zona"].iteratore = 0;
					}
				}
				break;
			}
			break;
		}
		break;
	case 2://Menu esc
		switch (stato.Cursori["Menu Editor"]["Uscita dall'editor"].iteratore)
		{
		case 0: //Elenco scelte
			input_tastiera_classico(stato.Cursori["Menu Editor"]["Elenco"].iteratore, event, { 3 }, false, true, false, false);
			if (event.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
				{
					switch (stato.Cursori["Menu Editor"]["Elenco"].iteratore)
					{
					case 0: //Continua
						stato.Cursori["Editor"]["Fasi"].iteratore = 1; //Editing
						break;
					case 1: //Salva Missione
						for (int i = 0; i < (9 * 9); i++)
						{
							if (stato.Mappa_editor[i].nome != "VUOTA")
							{
								tessere_inserite++;
								tessera_collegata = false;
								if (i + 1 < (9 * 9))
									if (stato.Mappa_editor[i + 1].nome != "VUOTA")
										tessera_collegata = true;
								if (i - 1 >= 0)
									if (stato.Mappa_editor[i - 1].nome != "VUOTA")
										tessera_collegata = true;
								if (i + 9 < (9 * 9))
									if (stato.Mappa_editor[i + 9].nome != "VUOTA")
										tessera_collegata = true;
								if (i - 9 >= 0)
									if (stato.Mappa_editor[i - 9].nome != "VUOTA")
										tessera_collegata = true;

								if (tessera_collegata == false)
									tessere_tutte_collegate = false;
							}
						}
						if (tessere_tutte_collegate == false && tessere_inserite > 1 || tessere_inserite == 0)
						{
							stato.Cursori["Menu Editor"]["Uscita dall'editor"].iteratore++;
						}
						else
						{
							stato.Cursori["Menu Editor"]["Uscita dall'editor"].iteratore = 2;
							stato.indicatore.iter = stato.indicatore.inserisci_stringa_input_tastiera.begin();
						}
						break;
					case 2:  //Esci
						MATTEO_BITMAP::destroy_vector_m_bitmap();
						stato.Cursori["Editor"]["Fasi"].iteratore = 0;
						stato.Cursori["Scelta tipo di editing"]["Elenco"].iteratore = 0;
						stato.Cursori["Editing Tessere"]["Focus Cursore"].iteratore = 0;
						stato.Cursori["Editing Tessere"]["Scelta Tessere"].iteratore = 0;
						stato.Cursori["Editing Tessere"]["Inserisci in mappa"].iteratore = 0;
						stato.Cursori["Editing Oggetti"]["Focus Cursore"].iteratore = 0;
						stato.Cursori["Editing Oggetti"]["Scelta Oggetto"].iteratore = 0;
						stato.Cursori["Editing Oggetti"]["Scelta zona"].iteratore = 0;
						stato.Cursori["Menu Editor"]["Uscita dall'editor"].iteratore = 0;
						stato.Cursori["Menu Editor"]["Elenco"].iteratore = 0;

						stato.indicatore.x_riquadro_mappa = 0;
						stato.indicatore.y_riquadro_mappa = 0;
						stato.indicatore.zoom = 0;
						stato.Stato_gioco = 0;
						break;
					}
					break;
				}
				if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
				{
					stato.Cursori["Menu Editor"]["Elenco"].iteratore = 0;
					stato.Cursori["Editor"]["Fasi"].iteratore = 1; //Editing
				}
			}
			break;
		case 1: //Messaggio
			if (event.type == ALLEGRO_EVENT_KEY_DOWN)
				stato.Cursori["Menu Editor"]["Uscita dall'editor"].iteratore--;
			break;
		case 2: //Salvataggio dati mappa
			if (event.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				input_stringa_da_tastiera(event, stato.indicatore.inserisci_stringa_input_tastiera, stato.indicatore.iter);
				if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
				{
					salva_mappa(stato.missione, stato.indicatore.inserisci_stringa_input_tastiera);
					//Salvo su file la miniatura
					stato.missione_editor.mappa_pulita = MATTEO_BITMAP::create();
					lunghezza_max_mappa = 0;
					for (size_t i = 0; i < stato.missione_editor.Mappa_matrice.size(); i++)
						if (lunghezza_max_mappa < (int)stato.missione_editor.Mappa_matrice[i].size())
							lunghezza_max_mappa = stato.missione_editor.Mappa_matrice[i].size();
					stato.missione_editor.mappa_pulita->init(lunghezza_max_mappa*982, stato.missione_editor.Mappa_matrice.size()*982,MATTEO_BITMAP::CREA,"");
					stato.missione_editor.mappa = MATTEO_BITMAP::create();
					stato.missione_editor.mappa->init(lunghezza_max_mappa * 982, stato.missione_editor.Mappa_matrice.size() * 982, MATTEO_BITMAP::CREA, "");
					if (stato.missione_editor.mappa->width >= stato.missione_editor.mappa->height)
					{
						w = 700;
						h = ((float)stato.missione_editor.mappa->height / (float)stato.missione_editor.mappa->width) * 700;
					}
					else
					{
						h = 700;
						w= (stato.missione_editor.mappa->width / stato.missione_editor.mappa->height) * 700;
					}
					stato.missione_editor.mappa_miniatura->init(w, h, MATTEO_BITMAP::CREA, "");
					disegna_mappa(stato, immagini_e_fonts, stato.missione_editor,w,h);
					MATTEO_BITMAP::set_target_bitmap(stato.missione_editor.mappa_miniatura);
					m_draw_scaled_bitmap(stato.missione_editor.mappa, 0, 0, stato.missione_editor.mappa->width, stato.missione_editor.mappa->height,
						0, 0, stato.missione_editor.mappa_miniatura->width, stato.missione_editor.mappa_miniatura->height, 0);
						
					string percorso;
					percorso = "Missioni/" + stato.missione_editor.nome_missione + '/' + stato.missione_editor.nome_missione + " miniatura.bmp";
					m_save_bitmap(percorso, stato.missione_editor.mappa_miniatura);

					stato.indicatore.inserisci_stringa_input_tastiera.clear();
					stato.missioni.push_back(stato.missione);
					stato.Cursori["Menu Editor"]["Uscita dall'editor"].iteratore = 0;
					stato.Cursori["Editor"]["Fasi"].iteratore = 0;
					stato.Stato_gioco = 0;
					Missione m_vuota;
					stato.missione = m_vuota;
				}
			}
			break;
		}
		break;
	}
	return false;
}
void draw_menu()
{
	al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));
	al_clear_to_color(al_map_rgb(0, 0, 0));
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.collage_menu);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display_2);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display_finale);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));

	vector<MATTEO_BITMAP*> Immagini_personaggi_selezionabili_visibili;
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
	m_draw_scaled_bitmap(immagini_e_fonts.title, 0, 0, immagini_e_fonts.title->width, immagini_e_fonts.title->height, 0, 0, 1920, 1080, 0);
	for (int i = 0; i < 6; i++)
		if (stato.giocatore[i].redraw_scheda == true)
		{
			draw_schede_personaggi(stato, immagini_e_fonts, i);
			stato.giocatore[i].redraw_scheda = false;
		}
	vector<string> scritte_menu;
	vector<string> scritte;
	vector<int> giocatori;
	vector<string> elenco_personaggi_visualizzabili;
	scritte_menu.clear();
	scritte_menu.push_back("Scegli missione");
	scritte_menu.push_back("Opzioni");
	scritte_menu.push_back("Lista tasti");
	scritte_menu.push_back("Editor");
	scritte_menu.push_back("Esci");
	int contatore_colonne;
	int contatore_righe;
	int x = -500;
	vector<string> elenco_missioni;
	for (size_t i = 0; i < stato.missioni.size(); i++)
		elenco_missioni.push_back(stato.missioni[i].nome_missione);
	switch (stato.Stato_menu)
	{
	case 0://menu iniziale: 0-Scegli missione
		draw_finestra_con_elenco_e_cursore(stato, immagini_e_fonts, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome, al_map_rgb(255, 255, 255),
			stato.Cursori["Menu iniziale"]["Lista"], scritte_menu, 700, 500, 0, 0, 1, 500, 300,60, 70, 56, NULL, false, al_map_rgb(0, 0, 0), al_map_rgba(0, 0, 0, 0), 0);
		break;
	case 1://Scegli missione
		draw_finestra_con_elenco_e_cursore(stato, immagini_e_fonts, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome, al_map_rgba(255, 255, 255, 0),
			stato.Cursori["Scegli Missione"]["Lista"], elenco_missioni, 130, 300, 100,0, 1, 700, 700, 100, 70, 56, NULL, false, al_map_rgb(0, 0, 0),
			al_map_rgba(0, 0, 0, 0), 0);
		if (stato.missioni.size() > 0)
		{
			MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
			if (stato.missioni[stato.Cursori["Scegli Missione"]["Lista"].iteratore].mappa_miniatura != nullptr)
				m_draw_bitmap(stato.missioni[stato.Cursori["Scegli Missione"]["Lista"].iteratore].mappa_miniatura, 950, 300, 0);
			else
				al_draw_scaled_text(stato.font, immagini_e_fonts, immagini_e_fonts.falso_display, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome,
					al_map_rgb(255, 255, 255), 950, 650, 700, -1, 0, "Anteprima non disponibile");
		}
		break;
	case 2://Seleziona numero giocatori
		al_draw_scaled_text(stato.font, immagini_e_fonts, immagini_e_fonts.falso_display, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome,
			al_map_rgb(255, 255, 255), 500, 400, 1000, -1, 0, "Quanti Giocatori?");
		draw_finestra_con_elenco_e_cursore(stato, immagini_e_fonts, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome, al_map_rgba(255, 255, 255, 0),
			stato.Cursori["Scegli numero pesonaggi"]["Lista"], { "1","2","3","4","5","6" }, 900, 600, 0, 0, 1, 200, 200, 200,
			stato.Cursori["Menu opzioni"]["Fonts"].Immagine_cursore->width, stato.Cursori["Menu opzioni"]["Fonts"].Immagine_cursore->height,
			NULL, false, al_map_rgb(0, 0, 0), al_map_rgb(0, 0, 0), 0);
		break;
	case 3://Seleziona personaggi
		if (stato.Cursori["Scegli pesonaggi"]["Lista"].iteratore > 0)//se non hai selezionato il primo
			elenco_personaggi_visualizzabili.push_back(stato.personaggi_selezionabili[stato.Cursori["Scegli pesonaggi"]["Lista"].iteratore - 1]);//inserisci il precedente
		else
			elenco_personaggi_visualizzabili.push_back("");
		elenco_personaggi_visualizzabili.push_back(stato.personaggi_selezionabili[stato.Cursori["Scegli pesonaggi"]["Lista"].iteratore]);//inserisci il selezionato sempre
		if (stato.Cursori["Scegli pesonaggi"]["Lista"].iteratore < (int)stato.personaggi_selezionabili.size() - 1)//se non hai selezionato l'ultimo
			elenco_personaggi_visualizzabili.push_back(stato.personaggi_selezionabili[stato.Cursori["Scegli pesonaggi"]["Lista"].iteratore + 1]);//inserisci il successivo
		else
			elenco_personaggi_visualizzabili.push_back("");
		MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
		if (stato.Cursori["Scegli pesonaggi"]["Lista"].iteratore == 0)
		{
			m_draw_scaled_bitmap(immagini_e_fonts.triangolo_scorrimento_destra, 0, 0, immagini_e_fonts.triangolo_scorrimento_destra->width,
				immagini_e_fonts.triangolo_scorrimento_destra->height, 876 + 500 + (640 * 27 / 100), 1080 - ((366 * 75 / 100) / 2), 100, 100, 0);
		}
		else if (stato.Cursori["Scegli pesonaggi"]["Lista"].iteratore == (int)stato.personaggi_selezionabili.size()-1)
		{
			m_draw_scaled_bitmap(immagini_e_fonts.triangolo_scorrimento_sinistra, 0, 0, immagini_e_fonts.triangolo_scorrimento_sinistra->width,
				immagini_e_fonts.triangolo_scorrimento_sinistra->height, 876 - 500 - 100, 1080 - ((366 * 75 / 100) / 2), 100, 100, 0);
		}
		else if (stato.Cursori["Scegli pesonaggi"]["Lista"].iteratore > 0)
		{
			m_draw_scaled_bitmap(immagini_e_fonts.triangolo_scorrimento_sinistra, 0, 0, immagini_e_fonts.triangolo_scorrimento_sinistra->width,
				immagini_e_fonts.triangolo_scorrimento_sinistra->height, 876 - 500 - 100, 1080 - ((366 * 75 / 100) / 2), 100, 100, 0);
			m_draw_scaled_bitmap(immagini_e_fonts.triangolo_scorrimento_destra, 0, 0, immagini_e_fonts.triangolo_scorrimento_destra->width,
				immagini_e_fonts.triangolo_scorrimento_destra->height, 876 + 500 + (640 * 27 / 100), 1080 - ((366 * 75 / 100) / 2), 100, 100, 0);
		}
		Immagini_personaggi_selezionabili_visibili.clear();
		for (size_t i = 0; i < elenco_personaggi_visualizzabili.size(); i++) //max 3
			for (size_t j = 0; j < stato.elenco_personaggi.size(); j++)
				if (stato.personaggio[j].nome == elenco_personaggi_visualizzabili[i])
				{
					//immagine del personaggio senza scheda
					MATTEO_BITMAP::set_target_bitmap(stato.personaggio[j].immagine_solo_personaggio_senza_scheda);
					m_draw_bitmap_region(stato.personaggio[j].immagine_scheda,
						stato.personaggio[j].immagine_scheda->width * 3 / 100,
						stato.personaggio[j].immagine_scheda->height * 26 / 100,
						stato.personaggio[j].immagine_solo_personaggio_senza_scheda->width,
						stato.personaggio[j].immagine_solo_personaggio_senza_scheda->height, 0, 0, 0);
					Immagini_personaggi_selezionabili_visibili.push_back(stato.personaggio[j].immagine_solo_personaggio_senza_scheda);
				}
				else if (elenco_personaggi_visualizzabili[i] == "")
				{
					Immagini_personaggi_selezionabili_visibili.push_back(immagini_e_fonts.Personaggio_1);
					break;
				}
		MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
		for (int i = 0; i < 3; i++, x = x + 500)
		{
			m_draw_scaled_bitmap(Immagini_personaggi_selezionabili_visibili[i], 0, 0, Immagini_personaggi_selezionabili_visibili[i]->width,
				Immagini_personaggi_selezionabili_visibili[i]->height,
				876.6 + x, 1080 - (366 * 75 / 100), 640 * 27 / 100, 366 * 75 / 100, 0);
		}
		al_draw_filled_rectangle(876.6, 1080 - (366 * 75 / 100), 876.6 + 640 * 27 / 100, 1080, al_map_rgba(100, 0, 0, 0.2));
		for (size_t i = 0; i < stato.elenco_personaggi.size(); i++)
			if (stato.personaggi_selezionabili[stato.Cursori["Scegli pesonaggi"]["Lista"].iteratore] == stato.elenco_personaggi[i])
				m_draw_scaled_bitmap(stato.personaggio[i].immagine_scheda,
					0, 0, stato.personaggio[i].immagine_scheda->width, stato.personaggio[i].immagine_scheda->height,
					(1920 / 2) - (960 / 2), 1080 * 10 / 100, 960, 549, 0);
		contatore_colonne = 0;
		contatore_righe = 0;
		for (int i = 0; i < 6; i++)
		{
			if (stato.giocatore[i].stato == 1)
			{
				contatore_colonne++;
				if (contatore_colonne > 2)
				{
					contatore_colonne = 1;
					contatore_righe++;
				}
				for (size_t j = 0; j < stato.elenco_personaggi.size(); j++)
				{
					if (stato.giocatore[i].personaggio == stato.personaggio[j].nome)
					{
						m_draw_scaled_bitmap(stato.personaggio[j].immagine_solo_personaggio_senza_scheda, 0, 0,
							stato.personaggio[j].immagine_solo_personaggio_senza_scheda->width,
							stato.personaggio[j].immagine_solo_personaggio_senza_scheda->height,
							1920 - (640 * 27 / 100)*contatore_colonne,
							366 * 75 / 100 * contatore_righe, 640 * 27 / 100, 366 * 75 / 100, 0);
					}
				}

			}
		}
		break;
	case 4: //inserisci abilità iniziali
		draw_aggiungi_abilità_giocatore(stato, immagini_e_fonts, stato.Cursori["Aggiungi abilità"]["Lista giocatori"].iteratore);
		break;
	case 5: //seleziona primo giocatore
		MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display); //falso display
		m_draw_bitmap(immagini_e_fonts.Cute_Zombie_Background, 0, 0, 0);
		scritte.push_back("Seleziona primo giocatore");
		scritte.push_back("Esplora mappa");
		MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display_2);
		draw_finestra_con_elenco_e_cursore(stato, immagini_e_fonts, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome,
			al_map_rgba(255, 255, 255, 0), stato.Cursori["Schermata preliminare"]["Lista"], scritte, 1500, 100, 50, 0, 1, 230, 500, 50, 70, 56,
			NULL, false, al_map_rgb(0, 0, 0), al_map_rgba(0, 0, 0, 0), 0);
		switch (stato.Stato_primo_giocatore)
		{
		case 0://scegli: seleziona primo giocatore o Esplora mappa
			for (int i = 0; i < stato.numero_giocatori; i++)
				giocatori.push_back(i);
			draw_schede_personaggi_con_o_senza_selezione(stato, immagini_e_fonts, giocatori, 3, 0, 0, false, NULL);
			break;
		case 1: //scegli tra i giocatori
			for (int i = 0; i < stato.numero_giocatori; i++)
				giocatori.push_back(i);
			draw_schede_personaggi_con_o_senza_selezione(stato, immagini_e_fonts, giocatori, 3, 0, 0, true, stato.Cursori["Scelta primo giocatore"]["Lista"].iteratore);
			break;
		case 2://esplora mappa
			disegna_mappa(stato, immagini_e_fonts, stato.missione, 1497, 1040);
			MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
			m_draw_scaled_bitmap(stato.missione.mappa,
				0, 0, stato.missione.mappa->width,stato.missione.mappa->height,//MAPPA
				23,19,1497,1040, 0);
			break;
		}
		break;
	case 6://menu opzioni
		MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
		draw_menu_opzioni(stato, immagini_e_fonts);
		break;
	case 7://lista tasti
		draw_lista_tasti(stato, immagini_e_fonts, immagini_e_fonts.falso_display);
		break;
	}

	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display_finale);
	m_draw_scaled_bitmap(immagini_e_fonts.falso_display, //falso display
		0, 0, immagini_e_fonts.falso_display->width, immagini_e_fonts.falso_display->height,
		0, 0, 1920, 1080, 0);
	m_draw_bitmap_region(immagini_e_fonts.falso_display_2, 0, 0, 1920, 1080, 80, 5, 0); //falso display 2
	al_set_target_bitmap(al_get_backbuffer(al_get_current_display())); //su schermo
	m_draw_scaled_bitmap(immagini_e_fonts.falso_display_finale, 0, 0, immagini_e_fonts.falso_display_finale->width, immagini_e_fonts.falso_display_finale->height,
		0, 0, stato.indicatore.risoluzione[0], stato.indicatore.risoluzione[1], 0);
}
void draw_gioco()
{
	al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));
	al_clear_to_color(al_map_rgb(0, 0, 0));
	MATTEO_BITMAP::set_target_bitmap(stato.missione.mappa);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.collage_menu);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display_2);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display_finale);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	disegna_mappa(stato, immagini_e_fonts, stato.missione, stato.indicatore.w_riquadro_mappa, stato.indicatore.h_riquadro_mappa);
	//disegno il menù
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.collage_menu);
	m_draw_bitmap(immagini_e_fonts.menu, 0, 0, 0);
	//disegno le schede dei personaggi
	for (int i = 0; i < 6; i++)
		if (stato.giocatore[i].redraw_scheda == true)
		{
			draw_schede_personaggi(stato, immagini_e_fonts, i);
			stato.giocatore[i].redraw_scheda = false;
		}
	disegna_collage_schede_personaggi(stato.giocatore, immagini_e_fonts.collage_schede_personaggi, stato.ordine_giocatori);
	vector<int> giocatori;
	vector<string> scritte;
	vector<int>zone_di_movimento_abominio;
	vector<int>personaggi_da_ferire;
	int zona_abominio;
	string frase;
	switch (stato.Stato_gioco_gioco)
	{
	case 2: //Scelta azioni
		MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.collage_menu);
		draw_pulsante_pericolo(stato, immagini_e_fonts, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore],
			(318 / 2) - (100 / 2), 0, 100, 40);
		draw_selezione_segnalini_zombie_o_personaggi(stato, immagini_e_fonts,
		{ stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1 }, 0,
			stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione,
			0, 80, 0, 0);
		MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.collage_menu);
		for (int i = 0; i < stato.numero_giocatori; i++)
		{
			for (size_t j = 0; j < stato.elenco_personaggi.size(); j++)
				if (stato.giocatore[stato.ordine_giocatori[i] - 1].personaggio == stato.personaggio[j].nome)
				{
					m_draw_scaled_bitmap(stato.personaggio[j].immagine_pedina, 0, 0,
						stato.personaggio[j].immagine_pedina->width, stato.personaggio[j].immagine_pedina->height,
						i * 50, 50, 50, 50, 0);
					if (stato.ordine_giocatori[i] - 1 == stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1)
						al_draw_filled_rectangle(i * 50, 50,
							i * 50 + 50, 50 + 50, al_map_rgba(0, 50, 0, 0.2));
				}
		}
		al_draw_scaled_text(stato.font, immagini_e_fonts, immagini_e_fonts.collage_menu, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome, al_map_rgba(255, 255, 255, 0),
			10, 140, 190, -1, 0, "Sei il giocatore:");
		for (size_t i = 0; i < stato.elenco_personaggi.size(); i++)
			if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].personaggio == stato.personaggio[i].nome)
				m_draw_scaled_bitmap(stato.personaggio[i].immagine_pedina, 0, 0, stato.personaggio[i].immagine_pedina->width,
					stato.personaggio[i].immagine_pedina->height, 210, 120, 80, 80, 0);
		frase = "Hai ancora ";
		frase = frase + to_string(stato.numero_azioni).c_str();
		frase = frase + " azioni.";
		al_draw_scaled_text(stato.font, immagini_e_fonts, immagini_e_fonts.collage_menu, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome, al_map_rgba(255, 255, 255, 0),
			10, 200, 280, 40, 0, frase);
		switch (stato.indicatore.sub_stato_turno_giocatori)
		{
		case 0://aggiungi abilità
			draw_aggiungi_abilità_giocatore(stato, immagini_e_fonts, stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1);
			/*al_set_target_bitmap(stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].Scheda_personaggio_con_equip);
			al_draw_scaled_bitmap(immagini_e_fonts.segna_abilità,0,0,al_get_bitmap_width(immagini_e_fonts.segna_abilità),al_get_bitmap_height(immagini_e_fonts.segna_abilità),
			357+(stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno]-1].abilità.size()*407),
			183+(57* (stato.Cursori["Aggiungi abilità giocatore"]["Lista"].iteratore)),86,49,0);
			al_set_target_bitmap(immagini_e_fonts.falso_display);
			al_draw_scaled_bitmap(stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].Scheda_personaggio_con_equip, 0, 0,
			al_get_bitmap_width(stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].Scheda_personaggio_con_equip),
			al_get_bitmap_height(stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].Scheda_personaggio_con_equip),
			320, 174, 1280, 732, 0);*/
			break;
		case 1://disegna zioni disponibili
			MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.collage_menu);
			draw_finestra_con_elenco_e_cursore(stato, immagini_e_fonts, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome,
				al_map_rgba(255, 255, 255, 0), stato.Cursori["Turno giocatore"]["Scelta azioni"], stato.elenco_azioni_disponibili, 0, 300, 30, 0, 1, 310,
				1070 - 260, 30, 70, 56, NULL, false, al_map_rgba(0, 0, 0, 0), al_map_rgba(0, 0, 0, 0), 0);
			break;
		case 2://menu pausa
			switch (stato.indicatore.sub_stato_menu_pausa)
			{
			case 0://scelta tra: opzioni ed torna al menu principale
				MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.collage_menu);
				draw_finestra_con_elenco_e_cursore(stato, immagini_e_fonts, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome,
					al_map_rgba(255, 255, 255, 0), stato.Cursori["Turno giocatore"]["Scelta azioni"], stato.elenco_azioni_disponibili, 0, 300, 30, 0, 1, 310,
					1070 - 260, 30, 70, 56, NULL, false, al_map_rgba(0, 0, 0, 0), al_map_rgba(0, 0, 0, 0), 0);
				MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
				m_draw_scaled_bitmap(stato.missione.mappa,
					0, 0, stato.missione.mappa->width,
					stato.missione.mappa->height,//MAPPA
					immagini_e_fonts.collage_schede_personaggi->width, 0,
					(1920 - immagini_e_fonts.collage_schede_personaggi->width - 318), 1080, 0);
				m_draw_bitmap_region(immagini_e_fonts.collage_menu, 0, 0, 318, 1080, 1602, 0, 0); //Collage menu
				m_draw_bitmap_region(immagini_e_fonts.collage_schede_personaggi, 0, stato.Cursori["Movimento schede personaggi"]["iteratore"].iteratore,
					immagini_e_fonts.collage_schede_personaggi->width, 1080, 0, 0, 0); //Collage Schede Personaggi
				m_draw_tinted_bitmap(immagini_e_fonts.falso_display, al_map_rgba(100, 100, 100, 255), 0, 0, 0);
				scritte.clear();
				scritte.push_back("Opzioni");
				scritte.push_back("Lista tasti");
				scritte.push_back("Esci dalla partita");
				draw_finestra_con_elenco_e_cursore(stato, immagini_e_fonts, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome,
					al_map_rgba(200, 0, 0, 0), stato.Cursori["Menu di pausa"]["Lista"], scritte, 700, 400, 100, 0, 1,
					immagini_e_fonts.collage_menu->width, immagini_e_fonts.collage_menu->height, 50, 70, 56,
					NULL, false, al_map_rgb(0, 0, 0), al_map_rgba(0, 0, 0, 0), 0);
				break;
			case 1://menu opzioni
				MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
				draw_menu_opzioni(stato, immagini_e_fonts);
				break;
			case 2://lista tsti
				draw_lista_tasti(stato, immagini_e_fonts, immagini_e_fonts.falso_display);
				break;
			}
			break;
		}
		break;
	case 3://disegna azione
		draw_selezione_segnalini_zombie_o_personaggi(stato, immagini_e_fonts,
		{ stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1 }, 0,
			stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione,
			0, 80, 0, 0.2);
		stato.azioni_disponibili[stato.Cursori["Turno giocatore"]["Scelta azioni"].iteratore].grafica(stato, immagini_e_fonts);
		break;
	case 4://turo zombie
		MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
		personaggi_da_ferire.clear();
		zone_di_movimento_abominio.clear();
		zona_abominio = 0;
		switch (stato.Stato_turno_zombie)
		{
		case 0: //aggiunta abilità
			break;
		case 1://attivazioni
			switch (stato.indicatore.stato_azioniZombie)
			{
			case 1: //uccidi umano
				draw_menu_assegna_ferite(stato, immagini_e_fonts);
				break;
			case 2: //muovi abominio
				inquadra_zona_mappa(stato, zona_abominio, stato.missione.mappa->width, stato.missione.mappa->height);
				scelta_movimento_abominio(stato, zone_di_movimento_abominio, zona_abominio);
				creazione_vettore_per_input_direzioni_con_ordinamento_di_zone(stato, zone_di_movimento_abominio, zona_abominio);
				MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.collage_menu);
				al_draw_text(immagini_e_fonts.arial_25, al_map_rgb(255, 255, 255), 0, 15, 0, "Scegli dove spostare l'abominio");
				MATTEO_BITMAP::set_target_bitmap(stato.missione.mappa);
				for (size_t i = 0; i < zone_di_movimento_abominio.size(); i++)
					if (i != stato.Cursori["Muovi abominio"]["Lista"].iteratore)
						draw_selezione_zona_mappa(stato, stato.missione.mappa, zone_di_movimento_abominio[i],al_map_rgba(0, 0, 100, 0.2));
				draw_selezione_zona_mappa(stato, stato.missione.mappa, zone_di_movimento_abominio[stato.Cursori["Muovi abominio"]["Lista"].iteratore],
					al_map_rgba(0, 100, 0, 0.2));
				break;
			case 3://mostra risultati
				MATTEO_BITMAP::set_target_bitmap(stato.missione.mappa);
				float rotazione;
				for (size_t i = 0; i < stato.indicatore.zone_spostamenti_zombie.size(); i++)
					for (size_t j = 0; j < stato.indicatore.direzioni_spostamenti_zombie[i].size(); j++)
					{
						rotazione = rotazione_in_base_direzione_zona(stato.indicatore.direzioni_spostamenti_zombie[i][j]);
			
						int width_finale = 0; int height_finale = 0;
						{
							int cont = 0;
							for (size_t i = 0; i < stato.missione.Mappa_matrice.size(); i++)
								if (cont < (int)stato.missione.Mappa_matrice[i].size())
									cont = stato.missione.Mappa_matrice[i].size();
							int width_1 = 982 * cont; int height_1 = 982 * stato.missione.Mappa_matrice.size();
							if ((double)width_1 / height_1 >(double)stato.indicatore.w_riquadro_mappa / stato.indicatore.h_riquadro_mappa)
							{
								//width_finale deve essere uguale a width_1 (della mappa)
								//mentre height_finale devessere aumentato con dello spazione nero
								width_finale = width_1;
								height_finale = width_1 * stato.indicatore.h_riquadro_mappa / stato.indicatore.w_riquadro_mappa;
							}
							else
							{
								height_finale = height_1;
								width_finale = height_1 * stato.indicatore.w_riquadro_mappa / stato.indicatore.h_riquadro_mappa;
							}
						}
						
						int w = (width_finale / stato.indicatore.zoom), h = (height_finale / stato.indicatore.zoom);
						if (rotazione != 360)
						{
							int dx = (stato.missione.x_cordinate_zone_mappa[stato.indicatore.zone_spostamenti_zombie[i]] +
								(stato.missione.x_dimensione_zone_mappa[stato.indicatore.zone_spostamenti_zombie[i]] / 2)-stato.indicatore.x_riquadro_mappa)*stato.indicatore.w_riquadro_mappa / w;
							int dy = (stato.missione.y_cordinate_zone_mappa[stato.indicatore.zone_spostamenti_zombie[i]] +
								(stato.missione.y_dimensione_zone_mappa[stato.indicatore.zone_spostamenti_zombie[i]] / 2)-stato.indicatore.y_riquadro_mappa)*stato.indicatore.h_riquadro_mappa / h;
							
							m_draw_scaled_rotated_bitmap(immagini_e_fonts.freccia_a_destra, 0, 28,
								dx,dy,stato.indicatore.zoom*0.70, stato.indicatore.zoom*0.7,rotazione, 0);
						}
							

					}
				break;
			}
			break;
		case 2://spawn
			if (stato.indicatore.azioneZombie_contatore_spawn < (int)stato.missione.elenco_spawn.size())
				draw_spawn_zombie(stato, immagini_e_fonts, stato.missione.elenco_spawn);
			break;
		}
		break;
	}
	//disegno mappa, menù e falso display, nel display
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display_finale);

	m_draw_scaled_bitmap(stato.missione.mappa,
		0, 0, stato.missione.mappa->width,
		stato.missione.mappa->height,//MAPPA
		immagini_e_fonts.collage_schede_personaggi->width,0,
		(1920 - immagini_e_fonts.collage_schede_personaggi->width - 318),1080,0);

	m_draw_bitmap_region(immagini_e_fonts.collage_menu, 0, 0, 318, 1080, 1602, 0, 0); //Collage menu
	m_draw_bitmap_region(immagini_e_fonts.collage_schede_personaggi,0, stato.Cursori["Movimento schede personaggi"]["iteratore"].iteratore,
		immagini_e_fonts.collage_schede_personaggi->width,1080,0,0,0); //Collage Schede Personaggi
	m_draw_bitmap(immagini_e_fonts.falso_display, 0, 0, 0);	//Falso display
	al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));
	m_draw_scaled_bitmap(immagini_e_fonts.falso_display_finale, 0, 0, immagini_e_fonts.falso_display_finale->width, immagini_e_fonts.falso_display_finale->height,
		0, 0, stato.indicatore.risoluzione[0], stato.indicatore.risoluzione[1], 0);	
}
void draw_editor()
{
	if (stato.Cursori["Editor"]["Fasi"].iteratore == 0) //Deve passare prima dalla logica... a quanto pare non sempre succedeva
		return;
	//clear dei bitmap display che uso
	al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));
	al_clear_to_color(al_map_rgb(0, 0, 0));
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display_2);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display_finale);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.tessera_da_scarabocchiare);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	
	//Mappa in costruzione sempre da stampare, in qualunque fase di editing
	int x = 0; int y = 0; //uso per disegnare la griglia 9x9 dove inserire le varie tessere
	int contatore = 0; //per tenere traccia degli indici della matrice che in realtà è un semplice erray
	if (stato.Cursori["Editor"]["Fasi"].iteratore > 0)
		disegna_mappa(stato, immagini_e_fonts,stato.missione_editor,1600,1080);
	int tessere_visualizzabili = 0;
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
	vector<string> scritte;
	vector<ALLEGRO_BITMAP*> oggetti;
	vector<int> y_oggetti;
	vector<int> indici_tessere; //i e j di una tessera (esempio la tessera 3) nella matrice mappa editor
	switch (stato.Cursori["Editor"]["Fasi"].iteratore)
	{
	case 0: //inizializzazione
		break;
	case 1: //Editing
		y = 50; x = 80; //dimensione Massima del cursore che vorrò
		al_draw_filled_rectangle(0, 0, 245 + x, 1080, al_map_rgb(162, 46, 55));
		scritte.clear();
		scritte.push_back("Tessere");
		scritte.push_back("Oggetti");
		al_draw_scaled_text(stato.font, immagini_e_fonts, immagini_e_fonts.falso_display, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome, al_map_rgb(255, 255, 255),
			((245 + x) / 2)-50, 0, 100, 50, 0, scritte[stato.Cursori["Scelta tipo di editing"]["Elenco"].iteratore]);
		switch (stato.Cursori["Scelta tipo di editing"]["Elenco"].iteratore)
		{
		case 0: //Editing Tessere
			//Scroll a destra della scritta in alto							
			m_draw_scaled_bitmap(immagini_e_fonts.triangolo_scorrimento_destra,
				0, 0, immagini_e_fonts.triangolo_scorrimento_destra->width, immagini_e_fonts.triangolo_scorrimento_destra->height,
				((245 + x) / 2) + 50, 5, 40, 40, 0);
			//gestisci Scroll
			tessere_visualizzabili = 4;
			while (stato.Cursori["Editing Tessere"]["Scelta Tessere"].iteratore< stato.Cursori["Editing Tessere"]["Scelta Tessere"].modificatore_scorrimento_elenco)
				stato.Cursori["Editing Tessere"]["Scelta Tessere"].modificatore_scorrimento_elenco--; //per salire di scroll
			while (stato.Cursori["Editing Tessere"]["Scelta Tessere"].iteratore >=
				(stato.Cursori["Editing Tessere"]["Scelta Tessere"].modificatore_scorrimento_elenco + tessere_visualizzabili)) //quando scendi di scroll
				stato.Cursori["Editing Tessere"]["Scelta Tessere"].modificatore_scorrimento_elenco++; //per scendere di scoll
																									  //disegno tutte le tessere in un unica colonna
			contatore = 0; //ora usato per contare quando si sono fatti 4 cicli e quindi ci si deve fermare (programmato male questo for)
			for (size_t i = stato.Cursori["Editing Tessere"]["Scelta Tessere"].modificatore_scorrimento_elenco; i < stato.Elenco_Tessere.size(); i++)
			{
				m_draw_scaled_bitmap(stato.Elenco_Tessere[i].immagini_in_varie_angolature[stato.Elenco_Tessere[i].indice_angolo_da_usare],
					0, 0, stato.Elenco_Tessere[i].immagini_in_varie_angolature[stato.Elenco_Tessere[i].indice_angolo_da_usare]->width,
					stato.Elenco_Tessere[i].immagini_in_varie_angolature[stato.Elenco_Tessere[i].indice_angolo_da_usare]->height,
					x, y, 240, 240, 0);
				// disegno il cursore se è presente
				if (i == stato.Cursori["Editing Tessere"]["Scelta Tessere"].iteratore)
					m_draw_scaled_bitmap(stato.Cursori["Editing Tessere"]["Scelta Tessere"].Immagine_cursore, 0, 0,
						stato.Cursori["Editing Tessere"]["Scelta Tessere"].Immagine_cursore->width,
						stato.Cursori["Editing Tessere"]["Scelta Tessere"].Immagine_cursore->height,
						0, y + ((240 / 2) - (x / 2)), x, x, 0);

				y = y + 245;
				contatore++;
				if (contatore == tessere_visualizzabili&&i < stato.Elenco_Tessere.size() - 1) // non ne entrano più
				{
					m_draw_scaled_bitmap(immagini_e_fonts.triangolo_scorrimento_giu, 0, 0, immagini_e_fonts.triangolo_scorrimento_giu->width,
						immagini_e_fonts.triangolo_scorrimento_giu->height, 0, 1080 - 70, 70, 70, 0);	//triangolo scorrimento giù
					break;
				}
			}
			if (stato.Cursori["Editing Tessere"]["Scelta Tessere"].modificatore_scorrimento_elenco > 0) //ce ne sono altri sopra che non si vedono
			{
				m_draw_scaled_bitmap(immagini_e_fonts.triangolo_scorrimento_su, 0, 0, immagini_e_fonts.triangolo_scorrimento_su->width,
					immagini_e_fonts.triangolo_scorrimento_su->height, 0, 0, 70, 70, 0);	//triangolo scorrimento su
			}
			switch (stato.Cursori["Editing Tessere"]["Focus Cursore"].iteratore)
			{
			case 0: //Scelta Tessera da inserire
				break;
			case 1: //Inserisci Tessera nella mappa
				MATTEO_BITMAP::set_target_bitmap(stato.missione_editor.mappa);
				x = 0; y = 0; contatore = 0;
				for (int i = 0; i < 9; i++)
				{
					for (int j = 0; j < 9; j++)
					{
						if (stato.Cursori["Editing Tessere"]["Inserisci in mappa"].iteratore == contatore)
							al_draw_filled_rectangle(x, y, x + immagini_e_fonts.Mappa_editor->width/ 9,
								y + immagini_e_fonts.Mappa_editor->height / 9, al_map_rgba(227, 66, 52, 0.2));
						x = x + immagini_e_fonts.Mappa_editor->width/ 9;
						contatore++;
					}
					y = y + (immagini_e_fonts.Mappa_editor->height / 9);
					x = 0;
				}
				break;	
			}
			break;		
		case 1: //Editing Oggetti
			m_draw_scaled_bitmap(immagini_e_fonts.triangolo_scorrimento_sinistra,
				0, 0, immagini_e_fonts.triangolo_scorrimento_destra->width, immagini_e_fonts.triangolo_scorrimento_sinistra->height,
				((245+x)/2)-50-40, 5, 40, 40, 0);
			switch (stato.Cursori["Editing Oggetti"]["Focus Cursore"].iteratore)
			{
			case 0: //scegli oggetto
				draw_finestra_con_elenco_e_cursore(stato, immagini_e_fonts, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome,
					al_map_rgb(255, 255, 255), stato.Cursori["Editing Oggetti"]["Scelta Oggetto"],
					{"Porta","Porta Sfondata","Porta blu","Porta blu Sfondata","Porta verde","Porta verde Sfondata","Start","Spawn rosso","Spawn blu",
					"Obiettivo rosso","Obiettivo blu","Obiettivo verde","Rumore","Macchina della Polizia","Pimp Mobile","Exit","Walker","Runner","Fat",
					"Abominio" },0,50, 0, 0, 1, 250, 1080 - 50, 50, 70, 50, NULL, false, al_map_rgb(0, 0, 0), al_map_rgb(0, 0, 0), 0);
				break;
			case 1: //scegli zona
				if (stato.Cursori["Editing Oggetti"]["Scelta Oggetto"].iteratore > 5) //Non hai selezionato una porta. Quindi seleziona una zona
				{
					contatore = 1;
					indici_tessere.clear();
					indici_tessere = stato.missione_editor.dati[stato.Cursori["Editing Oggetti"]["Scelta Zona"].iteratore].tessera_numero;
					for (int i = 0; i < 9 * 9; i++)
					{
						if (stato.Mappa_editor[i].nome != "VUOTA")
						{
							for (size_t j = 0; j < stato.missione_editor.dati[stato.Cursori["Editing Oggetti"]["Scelta Zona"].iteratore].tessera_numero.size(); j++)
								if (stato.missione_editor.dati[stato.Cursori["Editing Oggetti"]["Scelta Zona"].iteratore].tessera_numero[j] == contatore)
									indici_tessere[j] = contatore-1;
							contatore++;
						}
					}	
					//metto le modifiche in un bitmap della stessa dim delle tessere, così le cordinate coincidono
					MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.Mappa_editor);
					al_draw_filled_rectangle(stato.missione_editor.x_cordinate_zone_mappa[stato.Cursori["Editing Oggetti"]["Scelta Zona"].iteratore] / 2,
						stato.missione_editor.y_cordinate_zone_mappa[stato.Cursori["Editing Oggetti"]["Scelta Zona"].iteratore] / 2,
						((stato.missione_editor.x_cordinate_zone_mappa[stato.Cursori["Editing Oggetti"]["Scelta Zona"].iteratore] / 2) +
						(stato.missione_editor.x_dimensione_zone_mappa[stato.Cursori["Editing Oggetti"]["Scelta Zona"].iteratore] / 2)),
							((stato.missione_editor.y_cordinate_zone_mappa[stato.Cursori["Editing Oggetti"]["Scelta Zona"].iteratore] / 2) +
						(stato.missione_editor.y_dimensione_zone_mappa[stato.Cursori["Editing Oggetti"]["Scelta Zona"].iteratore] / 2)),
						al_map_rgba(50, 50, 150, 0.2));
				}
				else //Scegli zona porta
				{
					MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.Mappa_editor);
					for(size_t i=0; i<stato.porte_inseribili_editor.size(); i++)
						if(i!= stato.Cursori["Editing Oggetti"]["Scelta Zona"].iteratore)
							al_draw_filled_rectangle(stato.porte_inseribili_editor[i].x_cordinata / 2,
								stato.porte_inseribili_editor[i].y_cordinata / 2,
								((stato.porte_inseribili_editor[i].x_cordinata / 2) +
								(stato.porte_inseribili_editor[i].dim / 2)),
									((stato.porte_inseribili_editor[i].y_cordinata / 2) +
								(stato.porte_inseribili_editor[i].dim / 2)),
								al_map_rgba(100,100, 100, 0.2)); //grigio
						
					al_draw_filled_rectangle(stato.porte_inseribili_editor[stato.Cursori["Editing Oggetti"]["Scelta Zona"].iteratore].x_cordinata / 2,
						stato.porte_inseribili_editor[stato.Cursori["Editing Oggetti"]["Scelta Zona"].iteratore].y_cordinata / 2,
						((stato.porte_inseribili_editor[stato.Cursori["Editing Oggetti"]["Scelta Zona"].iteratore].x_cordinata / 2) +
						(stato.porte_inseribili_editor[stato.Cursori["Editing Oggetti"]["Scelta Zona"].iteratore].dim / 2)),
							((stato.porte_inseribili_editor[stato.Cursori["Editing Oggetti"]["Scelta Zona"].iteratore].y_cordinata / 2) +
						(stato.porte_inseribili_editor[stato.Cursori["Editing Oggetti"]["Scelta Zona"].iteratore].dim / 2)),
						al_map_rgba(50, 50, 150, 0.2));  //celeste
				}
				break;
			}
			break;
		}
		break;
	case 2: //Menù uscita editor
		MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
		m_draw_tinted_bitmap(immagini_e_fonts.falso_display, al_map_rgba(100, 100, 100, 255), 0, 0, 0);
		MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.Mappa_editor);
		m_draw_tinted_bitmap(immagini_e_fonts.Mappa_editor, al_map_rgba(100, 100, 100, 255), 0, 0, 0);
		switch (stato.Cursori["Menu Editor"]["Uscita dall'editor"].iteratore)
		{
		case 0: //Elenco scelte
			MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display_2);
			draw_finestra_con_elenco_e_cursore(stato, immagini_e_fonts, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome,
				al_map_rgb(255, 255, 255), stato.Cursori["Menu Editor"]["Elenco"], { "Continua","Salva Missione","Esci" },
				(1920 / 2) - (500 / 2), (1080 / 2) - (500 / 2), 0, 0, 1, 500, 500, 140, 70, 50, immagini_e_fonts.menu, true,
				al_map_rgb(255, 255, 255), al_map_rgb(0, 0, 0), 5);
			break;
		case 1: //Messaggio di errore di salvataggio. Tessere non collegate
			MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display_2);
			al_draw_scaled_text(stato.font, immagini_e_fonts, immagini_e_fonts.falso_display_2, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome,
				al_map_rgb(255, 255, 255),
				(1920 / 2) - (800 / 2), (1080 / 2) - (140 / 2), 800, 140, 0, "Le tessere devono essere tutte collegate tra di loro");
			break;
		case 2: //Salvataggio dati mappa
			MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display_2);
			al_draw_scaled_text(stato.font, immagini_e_fonts, immagini_e_fonts.falso_display_2, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome,
				al_map_rgb(255, 255, 255), (1920 / 2) - 150, (1080 / 2) - 200, 300, 50, 0, "Inserisci nome:");
			draw_input_stringa_tastiera(stato, immagini_e_fonts, stato.indicatore.inserisci_stringa_input_tastiera,
				stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore], al_map_rgb(255, 255, 255), 50, stato.indicatore.iter,
				immagini_e_fonts.falso_display_2, immagini_e_fonts.menu, 1920 / 2, 1080 / 2);
			break;
		}
	}
	//Disegna su schermo
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display_finale);
	m_draw_scaled_bitmap(immagini_e_fonts.falso_display, //falso display
		0, 0, immagini_e_fonts.falso_display->width, immagini_e_fonts.falso_display->height,
		0, 0, 1920, 1080, 0);

	m_draw_scaled_bitmap(stato.missione_editor.mappa,0,0,stato.missione_editor.mappa->width,stato.missione_editor.mappa->height,
		320, 0,1600,1080, 0);
	m_draw_scaled_bitmap(immagini_e_fonts.falso_display_2,			//falso display 2
		0, 0, immagini_e_fonts.falso_display_2->width, immagini_e_fonts.falso_display_2->height,
		0, 0, 1920, 1080, 0);
	
	al_set_target_bitmap(al_get_backbuffer(al_get_current_display())); //su schermo
	m_draw_scaled_bitmap(immagini_e_fonts.falso_display_finale, 0, 0, immagini_e_fonts.falso_display_finale->width, immagini_e_fonts.falso_display_finale->height,
		0, 0, stato.indicatore.risoluzione[0], stato.indicatore.risoluzione[1], 0);
	
}
void game_loop()
{
	event_queue = al_create_event_queue();
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_mouse_event_source());
	al_register_event_source(event_queue, al_get_display_event_source(immagini_e_fonts.display));

	bool redraw = true;
	al_start_timer(timer);
	ALLEGRO_EVENT event;
	al_get_next_event(event_queue, &event);
	while (!done)
	{
		if (stato.indicatore.crea_nuovo_display == true)
		{
			stato.indicatore.crea_nuovo_display = false;
			crea_display(stato.indicatore.risoluzione[0], stato.indicatore.risoluzione[1], stato.indicatore.tag_nuovo_display);
		}
		al_wait_for_event(event_queue, &event);
		if (event.type == ALLEGRO_EVENT_TIMER)
		{
			redraw = true;
			//update_logic();
		}
		switch (stato.Stato_gioco)
		{
		case 0: //menu
			logic_menu(event);
			break;
		case 1: //gioco
			logic_gioco(event);
			break;
		case 2: //Editor
			logic_editor(event);
			break;
		default:
			done = true;
		}
		if (redraw == true&&al_is_event_queue_empty(event_queue))
		{
			switch (stato.Stato_gioco)
			{
			case 0: //menu
				draw_menu();
				break;
			case 1: //gioco
				draw_gioco();
				break;
			case 2: //Editor
				draw_editor();
				break;
			}
			redraw = false;
			al_flip_display();
		}
	}
	
}
void init()
{
	al_init();
	al_init_image_addon();
	al_install_keyboard();
	al_install_mouse();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();
	srand(time(NULL));
	timer = al_create_timer(1.0 / 60);

	carica_fonts();
	carica_configurazioni_da_file();

	immagini_e_fonts.display = al_create_display(stato.indicatore.risoluzione[0], stato.indicatore.risoluzione[1]);
	if (stato.indicatore.schermo == "windowed")
		al_set_new_display_flags(ALLEGRO_WINDOWED);
	else if (stato.indicatore.schermo == "full-screen")
		al_set_new_display_flags(ALLEGRO_FULLSCREEN);
	//sfondi
	immagini_e_fonts.menu = MATTEO_BITMAP::create();
	immagini_e_fonts.menu->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/sfondi/menu.bmp");
	immagini_e_fonts.title = MATTEO_BITMAP::create();
	immagini_e_fonts.title->init(0, 0, MATTEO_BITMAP::CARICA,"Immagini/sfondi/title_full_HD.bmp");
	immagini_e_fonts.Cute_Zombie_Background = MATTEO_BITMAP::create();
	immagini_e_fonts.Cute_Zombie_Background->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/sfondi/Cute_Zombie_Background.bmp");
	immagini_e_fonts.Arsenal = MATTEO_BITMAP::create();
	immagini_e_fonts.Arsenal->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/sfondi/Arsenal.bmp");
	immagini_e_fonts.Zombie_horde = MATTEO_BITMAP::create();
	immagini_e_fonts.Zombie_horde->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/sfondi/Zombie_horde.bmp");
	immagini_e_fonts.Scambia_menu = MATTEO_BITMAP::create();
	immagini_e_fonts.Scambia_menu->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/sfondi/menu_scambia.bmp");
	immagini_e_fonts.menu_opzioni = MATTEO_BITMAP::create();
	immagini_e_fonts.menu_opzioni->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/sfondi/menu_opzioni.png");
	immagini_e_fonts.Sfondo_dadi = MATTEO_BITMAP::create();
	immagini_e_fonts.Sfondo_dadi->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/sfondi/Sfondo_dadi.png");
	immagini_e_fonts.health_sfondo = MATTEO_BITMAP::create();
	immagini_e_fonts.health_sfondo->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/sfondi/health.png");
	immagini_e_fonts.scacchi_sfondo = MATTEO_BITMAP::create();
	immagini_e_fonts.scacchi_sfondo->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/sfondi/sfondo_scacchi.png");

	//equip
	immagini_e_fonts.aaahh = MATTEO_BITMAP::create();
	immagini_e_fonts.aaahh->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/aaahh.bmp");
	immagini_e_fonts.bag_of_rice = MATTEO_BITMAP::create();
	immagini_e_fonts.bag_of_rice->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/bag_of_rice.bmp");
	immagini_e_fonts.baseball_bat = MATTEO_BITMAP::create();
	immagini_e_fonts.baseball_bat->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/baseball_bat.bmp");
	immagini_e_fonts.canned_food = MATTEO_BITMAP::create();
	immagini_e_fonts.canned_food->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/canned_food.bmp");
	immagini_e_fonts.chainsaw = MATTEO_BITMAP::create();
	immagini_e_fonts.chainsaw->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/chainsaw.bmp");
	immagini_e_fonts.crowbar = MATTEO_BITMAP::create();
	immagini_e_fonts.crowbar->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/crowbar.bmp");
	immagini_e_fonts.evil_twins = MATTEO_BITMAP::create();
	immagini_e_fonts.evil_twins->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/evil_twins.bmp");
	immagini_e_fonts.fire_axe = MATTEO_BITMAP::create();
	immagini_e_fonts.fire_axe->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/fire_axe.bmp");
	immagini_e_fonts.gasoline = MATTEO_BITMAP::create();
	immagini_e_fonts.gasoline->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/gasoline.bmp");
	immagini_e_fonts.glass_bottles = MATTEO_BITMAP::create();
	immagini_e_fonts.glass_bottles->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/glass_bottles.bmp");
	immagini_e_fonts.goalie_mask = MATTEO_BITMAP::create();
	immagini_e_fonts.goalie_mask->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/goalie_mask.bmp");
	immagini_e_fonts.katana = MATTEO_BITMAP::create();
	immagini_e_fonts.katana->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/katana.bmp");
	immagini_e_fonts.ma_s_shotgun = MATTEO_BITMAP::create();
	immagini_e_fonts.ma_s_shotgun->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/ma's_shotgun.bmp");
	immagini_e_fonts.machete = MATTEO_BITMAP::create();
	immagini_e_fonts.machete->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/machete.bmp");
	immagini_e_fonts.molotov = MATTEO_BITMAP::create();
	immagini_e_fonts.molotov->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/molotov.bmp");
	immagini_e_fonts.pan = MATTEO_BITMAP::create();
	immagini_e_fonts.pan->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/pan.bmp");
	immagini_e_fonts.pistol = MATTEO_BITMAP::create();
	immagini_e_fonts.pistol->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/pistol.bmp");
	immagini_e_fonts.plenty_of_ammo_shotgun = MATTEO_BITMAP::create();
	immagini_e_fonts.plenty_of_ammo_shotgun->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/plenty_of_ammo_shotgun.bmp");
	immagini_e_fonts.plenty_of_ammo = MATTEO_BITMAP::create();
	immagini_e_fonts.plenty_of_ammo->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/plenty_of_ammo.bmp");
	immagini_e_fonts.rifle = MATTEO_BITMAP::create();
	immagini_e_fonts.rifle->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/rifle.bmp");
	immagini_e_fonts.sniper_rifle = MATTEO_BITMAP::create();
	immagini_e_fonts.sniper_rifle->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/sniper_rifle.bmp");
	immagini_e_fonts.sawed_off = MATTEO_BITMAP::create();
	immagini_e_fonts.sawed_off->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/sawed_off.bmp");
	immagini_e_fonts.scope = MATTEO_BITMAP::create();
	immagini_e_fonts.scope->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/scope.bmp");
	immagini_e_fonts.shotgun = MATTEO_BITMAP::create();
	immagini_e_fonts.shotgun->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/shotgun.bmp");
	immagini_e_fonts.sub_mg = MATTEO_BITMAP::create();
	immagini_e_fonts.sub_mg->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/sub_mg.bmp");
	immagini_e_fonts.water = MATTEO_BITMAP::create();
	immagini_e_fonts.water->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/water.bmp");
	immagini_e_fonts.flashlight = MATTEO_BITMAP::create();
	immagini_e_fonts.flashlight->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/flashlight.bmp");
	immagini_e_fonts.wounded = MATTEO_BITMAP::create();
	immagini_e_fonts.wounded->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/equipaggiamento/wounded.bmp");

	//Caricamento files
	carica_tessere_da_file();
	carica_missioni_da_file();
	inizializza_abilità(stato);
	carica_personaggi_da_file();
	carica_carte_spaw_da_file();
	inizializza_mazzo_equipaggiamento(stato, immagini_e_fonts);

	immagini_e_fonts.collage_menu = MATTEO_BITMAP::create();
	immagini_e_fonts.collage_menu->init(immagini_e_fonts.menu->width, immagini_e_fonts.menu->height, MATTEO_BITMAP::CREA, "");
	immagini_e_fonts.collage_schede_personaggi = MATTEO_BITMAP::create();
	immagini_e_fonts.collage_schede_personaggi->init(700,404*6,MATTEO_BITMAP::CREA,"");
	immagini_e_fonts.falso_display = MATTEO_BITMAP::create();
	immagini_e_fonts.falso_display->init(1920,1080, MATTEO_BITMAP::CREA, "");
	immagini_e_fonts.falso_display_2 = MATTEO_BITMAP::create();
	immagini_e_fonts.falso_display_2->init(1920, 1080, MATTEO_BITMAP::CREA, "");
	immagini_e_fonts.Mappa_editor = MATTEO_BITMAP::create();
	immagini_e_fonts.Mappa_editor->init(4419, 4419, MATTEO_BITMAP::CREA, "");// 1080x4

	Tessere_editor a;
	a.nome = "VUOTA";
	for (int i = 0; i <9 * 9; i++)
	{
		stato.Mappa_editor.push_back(a);
	}
	immagini_e_fonts.tessera_da_scarabocchiare = MATTEO_BITMAP::create();
	immagini_e_fonts.tessera_da_scarabocchiare->init(982, 982, MATTEO_BITMAP::CREA, "");
	immagini_e_fonts.falso_display_finale = MATTEO_BITMAP::create();
	immagini_e_fonts.falso_display_finale->init(1920, 1080, MATTEO_BITMAP::CREA, "");
	immagini_e_fonts.testo_scalato = MATTEO_BITMAP::create();
	immagini_e_fonts.testo_scalato->init(1920, 1080, MATTEO_BITMAP::CREA, "");
	immagini_e_fonts.cursore_e_elenco = MATTEO_BITMAP::create();
	immagini_e_fonts.cursore_e_elenco->init(1920, 1080, MATTEO_BITMAP::CREA, "");
	immagini_e_fonts.segnalini_in_zona = MATTEO_BITMAP::create();
	immagini_e_fonts.segnalini_in_zona->init(200, 200, MATTEO_BITMAP::CREA, "");
	immagini_e_fonts.finestra_con_cursore_e_elenco = MATTEO_BITMAP::create();
	immagini_e_fonts.finestra_con_cursore_e_elenco->init(1920, 1080, MATTEO_BITMAP::CREA, "");
	immagini_e_fonts.scaled_tinted_bitmap = MATTEO_BITMAP::create();
	immagini_e_fonts.scaled_tinted_bitmap->init(1920, 1080, MATTEO_BITMAP::CREA, "");

	//segnalini
	immagini_e_fonts.segna_abilità = MATTEO_BITMAP::create();
	immagini_e_fonts.segna_abilità->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/segna_abilita.png");
	immagini_e_fonts.cursore = MATTEO_BITMAP::create();
	immagini_e_fonts.cursore->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/cursore.png");
	immagini_e_fonts.cursore_vuoto = MATTEO_BITMAP::create();
	immagini_e_fonts.cursore_vuoto->init(70, 56, MATTEO_BITMAP::CREA, "");
	immagini_e_fonts.start_token = MATTEO_BITMAP::create();
	immagini_e_fonts.start_token->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/start_token.png");
	immagini_e_fonts.obiettivo_rosso = MATTEO_BITMAP::create();
	immagini_e_fonts.obiettivo_rosso->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/obiettivo_rosso.png");
	immagini_e_fonts.obiettivo_blu = MATTEO_BITMAP::create();
	immagini_e_fonts.obiettivo_blu->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/obiettivo_blu.png");
	immagini_e_fonts.obiettivo_verde = MATTEO_BITMAP::create();
	immagini_e_fonts.obiettivo_verde->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/obiettivo_verde.png");
	immagini_e_fonts.exit_segnalino = MATTEO_BITMAP::create();
	immagini_e_fonts.exit_segnalino->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/exit_segnalino.png");
	immagini_e_fonts.polizia = MATTEO_BITMAP::create();
	immagini_e_fonts.polizia->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/polizia.png");
	immagini_e_fonts.pimp_mobile = MATTEO_BITMAP::create();
	immagini_e_fonts.pimp_mobile->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/pimp_mobile.png");
	immagini_e_fonts.rumore = MATTEO_BITMAP::create();
	immagini_e_fonts.rumore->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/rumore.png");
	immagini_e_fonts.spawn_rosso = MATTEO_BITMAP::create();
	immagini_e_fonts.spawn_rosso->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/spawn_rosso.png");
	immagini_e_fonts.spawn_blu = MATTEO_BITMAP::create();
	immagini_e_fonts.spawn_blu->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/spawn_blu.png");
	immagini_e_fonts.porta_neutra = MATTEO_BITMAP::create();
	immagini_e_fonts.porta_neutra->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/porta_neutra.png");
	immagini_e_fonts.porta_verde = MATTEO_BITMAP::create();
	immagini_e_fonts.porta_verde->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/porta_verde.png");
	immagini_e_fonts.porta_blu = MATTEO_BITMAP::create();
	immagini_e_fonts.porta_blu->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/porta_blu.png");
	immagini_e_fonts.porta_neutra_sfondata = MATTEO_BITMAP::create();
	immagini_e_fonts.porta_neutra_sfondata->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/porta_neutra_sfondata.png");
	immagini_e_fonts.porta_verde_sfondata = MATTEO_BITMAP::create();
	immagini_e_fonts.porta_verde_sfondata->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/porta_verde_sfondata.png");
	immagini_e_fonts.porta_blu_sfondata = MATTEO_BITMAP::create();
	immagini_e_fonts.porta_blu_sfondata->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/porta_blu_sfondata.png");
	immagini_e_fonts.walker = MATTEO_BITMAP::create();
	immagini_e_fonts.walker->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/walker.png");
	immagini_e_fonts.runner = MATTEO_BITMAP::create();
	immagini_e_fonts.runner->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/runner.png");
	immagini_e_fonts.fat = MATTEO_BITMAP::create();
	immagini_e_fonts.fat->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/fat.png");
	immagini_e_fonts.abominio = MATTEO_BITMAP::create();
	immagini_e_fonts.abominio->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/abominio.png");
	immagini_e_fonts.freccia_a_destra = MATTEO_BITMAP::create();
	immagini_e_fonts.freccia_a_destra->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/Freccia_a_destra.png");
	immagini_e_fonts.triangolo_scorrimento_giu = MATTEO_BITMAP::create();
	immagini_e_fonts.triangolo_scorrimento_giu->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/triangolo scorrimento giu.png");
	immagini_e_fonts.triangolo_scorrimento_su = MATTEO_BITMAP::create();
	immagini_e_fonts.triangolo_scorrimento_su->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/triangolo scorrimento su.png");
	immagini_e_fonts.triangolo_scorrimento_sinistra = MATTEO_BITMAP::create();
	immagini_e_fonts.triangolo_scorrimento_sinistra->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/triangolo scorrimento sinistra.png");
	immagini_e_fonts.triangolo_scorrimento_destra = MATTEO_BITMAP::create();
	immagini_e_fonts.triangolo_scorrimento_destra->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/triangolo scorrimento destra.png");

	string percorso_cartella_dadi = "Immagini/segnalini/Dadi/";
	for (int i = 1; i < 7; i++)
	{
		string percorso_dadi;
		percorso_dadi = percorso_cartella_dadi + to_string(i);
		percorso_dadi = percorso_dadi + "_insuccesso.png";
		MATTEO_BITMAP* dadi_insuccesso=MATTEO_BITMAP::create();
		dadi_insuccesso->init(0, 0, MATTEO_BITMAP::CARICA, percorso_dadi.c_str());
		immagini_e_fonts.dadi_insucceso.push_back(dadi_insuccesso);
		percorso_dadi = percorso_cartella_dadi + to_string(i);
		percorso_dadi = percorso_dadi + "_successo.png";
		MATTEO_BITMAP* dadi_successo=MATTEO_BITMAP::create();
		dadi_successo->init(0, 0, MATTEO_BITMAP::CARICA, percorso_dadi.c_str());
		immagini_e_fonts.dadi_successo.push_back(dadi_successo);
	}
	//tasti
	immagini_e_fonts.w_button = MATTEO_BITMAP::create();
	immagini_e_fonts.w_button->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/w_button.png");
	immagini_e_fonts.a_button = MATTEO_BITMAP::create();
	immagini_e_fonts.a_button->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/a_button.png");
	immagini_e_fonts.s_button = MATTEO_BITMAP::create();
	immagini_e_fonts.s_button->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/s_button.png");
	immagini_e_fonts.d_button = MATTEO_BITMAP::create();
	immagini_e_fonts.d_button->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/d_button.png");
	immagini_e_fonts.arrow_right_buttton = MATTEO_BITMAP::create();
	immagini_e_fonts.arrow_right_buttton->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/arrow_right_buttton.png");
	immagini_e_fonts.enter_button = MATTEO_BITMAP::create();
	immagini_e_fonts.enter_button->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/enter_button.png");
	immagini_e_fonts.escape_button = MATTEO_BITMAP::create();
	immagini_e_fonts.escape_button->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/escape_button.png");
	immagini_e_fonts.space_bar_button = MATTEO_BITMAP::create();
	immagini_e_fonts.space_bar_button->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/space_bar_button.png");
	immagini_e_fonts.page_down_button = MATTEO_BITMAP::create();
	immagini_e_fonts.page_down_button->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/page_down_button.png");
	immagini_e_fonts.page_up_button = MATTEO_BITMAP::create();
	immagini_e_fonts.page_up_button->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/page_up_button.png");
	immagini_e_fonts.tab_button = MATTEO_BITMAP::create();
	immagini_e_fonts.tab_button->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/tab_button.png");

	//personaggi
	immagini_e_fonts.scheda_personaggio = MATTEO_BITMAP::create();
	immagini_e_fonts.scheda_personaggio->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/scheda_personaggio_bianca.bmp");
	immagini_e_fonts.Personaggio_1 = MATTEO_BITMAP::create();
	immagini_e_fonts.Personaggio_1->init(immagini_e_fonts.scheda_personaggio->width * 27 / 100,
		immagini_e_fonts.scheda_personaggio->height * 75 / 100, MATTEO_BITMAP::CREA, "");
	immagini_e_fonts.barra_esperienza_blank = MATTEO_BITMAP::create();
	immagini_e_fonts.barra_esperienza_blank->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/barra_esperienza_neutra.png");

	//fonts
	immagini_e_fonts.arial_25 = al_load_font("arial.ttf", 25, 0);
	//Azioni
	inizializza_azioni(stato);
	done = false;
	stato.indicatore.cercabile = true;
	stato.indicatore.w_riquadro_mappa = 1920-immagini_e_fonts.collage_schede_personaggi->width-318;
	stato.indicatore.h_riquadro_mappa = 1080;
	stato.indicatore.zoom = 1;
}
int main()
{
	init();
	game_loop();
	shutdown();
	abort_game("Fine partita");
}
