#pragma once

#include <string>
#include <vector>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <algorithm>
#include <map>
#include"Draw.h"

using namespace std;

class MATTEO_BITMAP
{
private: 
	static vector<MATTEO_BITMAP*> immagini_da_distruggere;
	static vector<MATTEO_BITMAP*> elenco_oggetti_creati;
	static MATTEO_BITMAP* target_m_bitmap;
	/*vector<Draw_Madre*> vecchia_lista_draw;
	vector<Draw_Madre*> lista_draw;
	void createSubVectorDrawLists(vector<vector<Draw_Madre*>>& V1, vector<vector<Draw_Madre*>>&V2);
	void createSubVectorOldDrawListCommonAndNotCommon(vector<Draw_Madre*>& in_comune, vector<Draw_Madre*>& non_in_comune);
	void checkIfDrawableDrawInvalidOtherRecursive(int i, vector<vector<Draw_Madre*>>V2, vector<Draw_Madre*> in_comune);*/
public:
	enum TIPO
	{
		CARICA = 1 << 0,
		CREA = 1 << 1,
		VUOTO = 1 << 2,
	};
	static MATTEO_BITMAP* get_target_bitmap(); //restituisce il bitmap impostato su cui si sta disegnando
	static bool set_target_bitmap(MATTEO_BITMAP* target); //restituisce true se va a buon fine, e quindi l'imm non è nullpt
	static MATTEO_BITMAP* create();
	static void destroy_all_object();
	void rigenerate_bitmap();
	void init(int width, int height, int carica_crea, string percorso_file);
	void add_to_immagini_da_distruggere();
	void destroy();
	ALLEGRO_BITMAP* immagine;
	int carica_crea;
	string percorso_file;
	int width;
	int height;
		//Non le sto usando
	//bool create_load_m_bitmap();
	static void destroy_vector_m_bitmap();
	/*bool compare(MATTEO_BITMAP* altro);
	void Draw();
	void anti_draw();
	void AddDrawToList(Draw_Madre* d);
	void StartNewDrawList();
	void setDrawableList();
	bool checkIntegrityDrwaList();*/
};

class fonts
{
public:
	string nome;
	string percorso;
	vector<ALLEGRO_FONT*> allegro_font;
	vector<int> dim_font;
	vector<int> height_dim;
};

class Immagini
{
public:
	vector<MATTEO_BITMAP*> immagini_da_distruggere;
	ALLEGRO_DISPLAY* display = NULL;
	vector<MATTEO_BITMAP*> missioni;
	vector<MATTEO_BITMAP*> mappe;
	vector<MATTEO_BITMAP*> dadi_insucceso;
	vector<MATTEO_BITMAP*> dadi_successo;
	MATTEO_BITMAP* collage_menu; //bitmap temporaneo dove metto tutte le cose del menù
	MATTEO_BITMAP* collage_schede_personaggi;
	MATTEO_BITMAP* falso_display; //bitmap di grandezza uguale al display, dove metto tutto quello che voglio che venga...
	MATTEO_BITMAP* falso_display_2;
	MATTEO_BITMAP* Mappa_editor;
	MATTEO_BITMAP* Mappa_editor_da_salvare;
	MATTEO_BITMAP* tessera_da_scarabocchiare;
	MATTEO_BITMAP* falso_display_finale;
	MATTEO_BITMAP* cursore_e_elenco;
	MATTEO_BITMAP* finestra_con_cursore_e_elenco;
	MATTEO_BITMAP* scaled_tinted_bitmap;
	MATTEO_BITMAP* segnalini_in_zona;
	MATTEO_BITMAP* menu_opzioni;
	MATTEO_BITMAP* scheda_personaggio;
	MATTEO_BITMAP* tessere_missione;		//mappa pulita
	MATTEO_BITMAP* Personaggio_1;//la uso come bitmap vuoto
	MATTEO_BITMAP* testo_scalato; //la uso per scalare il testo
	MATTEO_BITMAP* barra_esperienza_blank;
	MATTEO_BITMAP* walker;
	MATTEO_BITMAP* runner;
	MATTEO_BITMAP* fat;
	MATTEO_BITMAP* abominio;
	MATTEO_BITMAP* menu;
	MATTEO_BITMAP* title;
	MATTEO_BITMAP* health_sfondo;
	MATTEO_BITMAP* scacchi_sfondo;
	MATTEO_BITMAP* Cute_Zombie_Background;
	MATTEO_BITMAP* Sfondo_dadi;
	MATTEO_BITMAP* Arsenal;
	MATTEO_BITMAP* Zombie_horde;
	MATTEO_BITMAP* Scambia_menu;
	//Cursori
	MATTEO_BITMAP* cursore;
	MATTEO_BITMAP* segna_abilità;
	MATTEO_BITMAP* cursore_vuoto;
	MATTEO_BITMAP* triangolo_scorrimento_giu;
	MATTEO_BITMAP* triangolo_scorrimento_su;
	MATTEO_BITMAP* triangolo_scorrimento_sinistra;
	MATTEO_BITMAP* triangolo_scorrimento_destra;
	MATTEO_BITMAP* freccia_a_destra;
	MATTEO_BITMAP* obiettivo_rosso;
	MATTEO_BITMAP* obiettivo_blu;
	MATTEO_BITMAP* obiettivo_verde;
	MATTEO_BITMAP* exit_segnalino;
	MATTEO_BITMAP* polizia;
	MATTEO_BITMAP* pimp_mobile;
	MATTEO_BITMAP* rumore;
	MATTEO_BITMAP* spawn_rosso;
	MATTEO_BITMAP* spawn_blu;
	MATTEO_BITMAP* porta_neutra;
	MATTEO_BITMAP* porta_verde;
	MATTEO_BITMAP* porta_blu;
	MATTEO_BITMAP* porta_neutra_sfondata;
	MATTEO_BITMAP* porta_blu_sfondata;
	MATTEO_BITMAP* porta_verde_sfondata;
	MATTEO_BITMAP* start_token;
	MATTEO_BITMAP* arrow_right_buttton;
	MATTEO_BITMAP* a_button;
	MATTEO_BITMAP* d_button;
	MATTEO_BITMAP* w_button;
	MATTEO_BITMAP* s_button;
	MATTEO_BITMAP* enter_button;
	MATTEO_BITMAP* escape_button;
	MATTEO_BITMAP* space_bar_button;
	MATTEO_BITMAP* page_down_button;
	MATTEO_BITMAP* page_up_button;
	MATTEO_BITMAP* tab_button;
	MATTEO_BITMAP* aaahh; //iniziano le carte equipaggiamento
	MATTEO_BITMAP* bag_of_rice;
	MATTEO_BITMAP* flashlight;
	MATTEO_BITMAP* baseball_bat;
	MATTEO_BITMAP* canned_food;
	MATTEO_BITMAP* chainsaw;
	MATTEO_BITMAP* crowbar;
	MATTEO_BITMAP* evil_twins;
	MATTEO_BITMAP* fire_axe;
	MATTEO_BITMAP* gasoline;
	MATTEO_BITMAP* glass_bottles;
	MATTEO_BITMAP* goalie_mask;
	MATTEO_BITMAP* katana;
	MATTEO_BITMAP* machete;
	MATTEO_BITMAP* ma_s_shotgun;
	MATTEO_BITMAP* molotov;
	MATTEO_BITMAP* pan;
	MATTEO_BITMAP* pistol;
	MATTEO_BITMAP* plenty_of_ammo_shotgun;
	MATTEO_BITMAP* plenty_of_ammo;
	MATTEO_BITMAP* rifle;
	MATTEO_BITMAP* sniper_rifle;
	MATTEO_BITMAP* sawed_off;
	MATTEO_BITMAP* scope;
	MATTEO_BITMAP* shotgun;
	MATTEO_BITMAP* sub_mg;
	MATTEO_BITMAP* water;
	MATTEO_BITMAP* wounded;
	ALLEGRO_FONT *arial_25; //iniziano i fonts
};

class Cursore
{
private:
	static MATTEO_BITMAP* immagine;
public:
	static int mouse_scroll;

	int iteratore;
	int modificatore_scorrimento_elenco;
	MATTEO_BITMAP* Immagine_cursore;
	Cursore();
};

class DatiStanza
{	
public:
	enum //colore obiettivo
	{
		rosso = 1 << 0,
		blu = 1 << 1,
		verde = 1 << 2,
	};
	vector<int> tessera_numero;
	bool giocatori[7];
	int rumore;
	int walker;
	int runner;
	int fat;
	bool abominio;
	int polizia;
	int pimp_mobile;
	int obiettivi_rossi;
	int obiettivi_blu;
	int obiettivi_verdi;
	int spawn_rosso;
	int spawn_blu;
	int spawn_verde;
	bool exit;
	bool carta_aaa;
	bool cercabile;
	int tipo; //0-strada, 1-stanza interna

	DatiStanza();
	void SommaValoreRumore(int valore);
	void SommaValoreWalker(int valore);
	void SommaValoreRunner(int valore);
	void SommaValoreFat(int valore);
	void ModificaValoreAbominio (bool valore);
	void SommaValorePolizia(int valore);
	void SommaValorePimp_mobile(int valore);
	void SommaValoreObiettivo(int valore,int colore);
	void SommaValoreSpawn(int valore,int colore);
	void ModificaValoreExit(bool valore);
	void ModificaValoreCarta_aaa(bool valore);
	void ModificaValoreGiocatori(bool valore, int indice_giocatore);
};

class Porta
{
public:
	int colore; //0-rossa,1-blu,2-verde
	int stato; // 0-chiusa, 1-sfondata
	int x_cordinata;
	int y_cordinata; //per poter fare il draw
	int dim; //ne basta uno perchè è quadrato. La dim dipende dalla risoluzione della mappa
	int zone[2]; //le zone che collega per poterla trovare
};
class Parametri_Tessera
{
public:
	vector<int> n_zona;
	vector<int> x_zona;
	vector<int> y_zona;
	vector<int> x_dim_zona;
	vector<int> y_dim_zona;
	vector<bool> cercabile;
	vector<vector<bool>> direzione_fusione; //[0]:su [1]:dx [2]:giù [3]:sx.  False: non fondibile. True: Fondibile
	vector<vector<bool>> direzione_mura; //[0]:su [1]:dx [2]: giù [3]: sx.  False: no muro. True: muro.
	vector<int> fila_della_zona;
	vector<int> x_porta;
	vector<int> y_porta;
	vector<vector<int>> zone_porta;
	vector<int> zone_tombini;
};
class Tessere_editor
{
public:
	string nome;
	vector<int> angolatura;
	int indice_angolo_da_usare;
	// il primo "vector" è un vettore di [4] che serve a capire a quale angolatura dell'immagine ci si riferisce: [0]:0 [1]:90 [2]:180 [3]:270
	vector<MATTEO_BITMAP*> immagini_in_varie_angolature;
	vector<Parametri_Tessera> parametri_per_angolatura;
};
class Missione
{
public:
	vector<int> elenco_spawn;
	vector<vector<int>> collisioni;
	vector<vector<int>> orientamento_connessioni;
	vector<vector<int>> tombini; //vanno messi in ordine di tessera. le tessere si contano da in alto a sx verso dx e dall'alto verso il basso (come le zone)
	vector<int> x_cordinate_zone_mappa;
	vector<int>y_cordinate_zone_mappa;
	vector<int>x_dimensione_zone_mappa;
	vector<int>y_dimensione_zone_mappa;
	int numero_porte;
	vector<Porta> porta;
	vector<DatiStanza> dati;
	vector<DatiStanza> copia_dati;
	vector<vector<Tessere_editor>> Mappa_matrice;
	string nome_missione;
	MATTEO_BITMAP* mappa;
	MATTEO_BITMAP* mappa_miniatura;
	MATTEO_BITMAP* mappa_pulita;
	vector<int> w_h_mappa;

	void ModificaStatoPorta(int valore, int indice_porta);
};

typedef class azione Azione;
typedef class statogioco StatoGioco;

class Carta_equipaggiamento
{
public:
	enum
	{
		ARMA = 1 << 0,
		DIFESA = 1 << 1,
		FLASHLIGHT = 1 << 2,
		COMBINABILE = 1 << 3,
		RIFORNIMENTO = 1 << 4,
		MUNIZIONI = 1 << 5,
		TRAPPOLA = 1 << 6,
	};
	MATTEO_BITMAP* Immagine{};
	string nome{};
	int tipologia{}; // enum
	int gittata_min_melee{};
	int gittata_max_melee{};
	int gittata_min_ranged{};
	int gittata_max_ranged{};
	int dadi_melee{};
	int dadi_ranged{};
	int precisione_melee{};
	int precisione_ranged{};
	int danno_melee{};
	int danno_ranged{};
	bool rumore_dopo_attacco_melee{};
	bool rumore_dopo_attacco_ranged{};
	bool doppia{};
	bool scarica_melee = false;
	bool scarica_ranged = false;
	int apre_porte{}; // 0-non apre, 1-apre con rumore, 2-apre senza rumore
	int(*speciale)(StatoGioco& stato, ALLEGRO_EVENT evento){};
};

class Carta_zombie
{
public:
	MATTEO_BITMAP* Immagine;
	int numero;
	int tipologia; //0 normale, 1 tombino, 2 attivazione extra
	int zombie[4][2];
};
class Abilità
{
public:
	enum //tipologia
	{
		START_WITH_A_WEAPON = 1 << 0,
		AZIONE = 1 << 1,
		DADI = 1 << 2,
		RE_ROLL = 1<<3,
		FREE = 1 << 4,
		DIFESA = 1<<5,
		MODIFICATORE_MOVIMENTO = 1<<6,
		PASSIVA = 1 << 7,
		MODIFICATORE_CERCA = 1<<8,
	};
	enum //carica e scarica
	{
		MAI = 1 << 0,
		SUBITO =1 <<1,
		A_INIZIO_TURNO_GIOCATORE = 1 << 2,
		A_INIZIO_TURNO_ZOMBIE = 1 << 3,
		DOPO_ESECUZIONE_AZIONE = 1<<4,
		DOPO_LANCIO_DADI =1<<5,
	};
	enum //codici per attivare diverse parti della funzione
	{
		CODICE_ESEGUI=1<<0,
		CODICE_SCARICA=1<<1,
		CODICE_RICARICA=1<<2,
	};
	string nome;
	int tipo;
	bool scarica;

	int quando_si_scarica;
	int quando_ricarica;
	int(*funzione)(StatoGioco& stato,int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione);
};
class Giocatore
{
public:
	int stato; //0 inattivo // attivo
	bool redraw_scheda;
	string personaggio; //quale
	vector<Abilità> abilità; //quelle che man mano scegli
	vector<Carta_equipaggiamento> equipaggiamento;
	vector<Carta_equipaggiamento> equipaggiamento_di_backup;
	MATTEO_BITMAP* Scheda_personaggio_con_equip;
	MATTEO_BITMAP* Barra_esperienza_con_segnalino;
	int esperienza;
	int ferite;
	int posizione;
} ;
class Personaggio
{
public:
	string nome;
	vector<Abilità> abilità[4]; //[0][]elenco abilità blù, [1][] elenco abilità giallo, etc..
	MATTEO_BITMAP* immagine_scheda;
	MATTEO_BITMAP* immagine_esperienza;
	MATTEO_BITMAP* immagine_pedina;
	MATTEO_BITMAP* immagine_solo_personaggio_senza_scheda;
	MATTEO_BITMAP* immagine_segna_esperienza;
	//si potrebbero pure aggiungere cose come effetti sonori, animazioni, diverse skin dei menù, etc..
};

class Indicatori_di_situazione
{
public:
	//variabili grafica
	int x_cursore;
	int y_cursore;
	int x_riquadro_mappa;
	int y_riquadro_mappa;
	int w_riquadro_mappa;
	int h_riquadro_mappa;
	int x_riquadro_mappa_backup;
	int y_riquadro_mappa_backup;
	double zoom;
	//variabili generali di gioco
	bool primo_giocatore_morto;
	bool crea_nuovo_display;
	int tag_nuovo_display;
	bool cercabile;
	int random;
	int timer;
	int risoluzione[2];
	vector<vector<int>> risoluzioni;
	string schermo; //full screen o windowed
	int giocatore_di_turno;
	int ferite;
	bool check_extraturn;
	bool esplora_mappa;
	string inserisci_stringa_input_tastiera;
	string::iterator iter;
	//vari stati di tutto il gioco
	int stato_spawn;
	int stato_assegna_prima_abilità;
	int stato_assegna_abilità;
		int numero_abilità_da_inserire;
		int colore_abilità_da_inserire;
	int sub_stato_turno_giocatori;
	int sub_stato_menu_pausa;
	int stato_azioniZombie;
	int sub_stato_azioniZombie_attivazione;
		vector<int> umani_visti;
		vector<int>scelta_personaggio_da_ferire;
	int stato_menu_ferite;
		vector<int>personaggi_morti;
	int azioneZombie_contatore_zona;
	int azioneZombie_contatore_spawn;
	int azioneZombie_umanoDaFerire;
	bool azioneZombie_amobinio_mosso_manualmente;
	int stato_apri_porte;
	int numero_spostamenti_con_un_movimento;
	bool considerare_zombie_per_zone_disponibili_a_spostamento;
	Carta_equipaggiamento equipaggiamento_selezionato;
		int prima_stanza_edificio;
	int stato_combina;
		vector<vector<Carta_equipaggiamento>> lista_combinabili; //a+b
		vector<Carta_equipaggiamento> prodotti_combinazione;//=c
	int stato_menu_scambia_e_organizza_grafica;
	int stato_menu_scambia_e_organizza;
	int sub_stato_scambia_organizza_grafica;
		vector<int> disponibili_a_scambio;
		int sub_stato_menu_scambia_e_organizza;
		int scheda_personaggio_posizionato;
		bool selezione_blu_verde;
	int stato_azione_attacca;
		int stato_azione_arma;
			int sub_stato_dadi_azione_arma;
			int tipo;
		int stato_azione_arma_grafica;
		vector<string> scelte_messaggio_generico;
		string messaggio_generico;
		int stanza_selezionata;
		vector<int> zombie_o_giocatori_selezionabili_melee; //personaggi: 0,1,2,3,4,5 -- 6-walker, 7-runner, 8-fat, 9-abominio
		Carta_equipaggiamento arma_selezionata;
		int tipo_arma_selezionata; //0-melee,1-ranged,2-entrambe
		int moltiplicatore_dadi;
		bool doppia=false;
		vector<int>dadi;
		int dadi_successo;
		vector<string> abilità_dadi_usate;
	int sub_stato_melee_azione_attacca;
	int sub_stato_ranged_azione_attacca;
	int sub_stato_gunblade_azione_attacca;
	int sub_stato_re_roll_azione_attacca;
		vector<string> Lista_re_roll;
		int giocatore_selezionato;
	int stato_azione_medic;
		int giocatore_curato;
	int stato_azione_born_leader;
		string azione_gratuita_assegnata_dal_born_leader;
	bool check_azione_attacca;
	//altre varie variabili specifiche per alcune azioni che non contengono stati di gioco
	Carta_zombie pescata;
	vector<vector<int>> direzioni_spostamenti_zombie;
	vector<int> zone_spostamenti_zombie;
};
class statogioco
{
public:
	int Stato_gioco = 0;	//Menù(0)->gioco(1).  gioco-->pausa(2)  pausa-->gioco o menù
	int Stato_gioco_gioco = 0;		//0. fase preparatoria 1. Fase turno giocatori
	int Stato_menu = 0;			// Selezione missione e Personaggi (0), Selezione numero giocatori(1), Selezione personaggi(2), Selezione missione (3)
	int Stato_primo_giocatore = 0; // 0 - menù di selezione: primo giocatore(0) e esplora mappa(1);
	int Stato_turno_zombie;
	vector<fonts> font;
	vector<Tessere_editor> Elenco_Tessere;
	vector<Tessere_editor> Mappa_editor;
	vector<Porta> porte_inseribili_editor;
	map<string, map<string, Cursore>> Cursori;
	vector <string> personaggi_selezionabili;
	vector<vector<vector<int>>> percorsi_minimi;
	int numero_azioni;
	int numero_giocatori;
	vector<Missione> missioni;
	Missione missione;
	Missione missione_editor;
	Giocatore giocatore[10];
	vector<Personaggio> personaggio;
	vector<string> elenco_personaggi;
	vector<string> elenco_carte_spawn;
	vector<Carta_equipaggiamento> Mazzo_equipaggiamento;
	vector<Carta_equipaggiamento> Mazzo_equipaggiamento_originale;
	vector<Carta_equipaggiamento> Mazzo_equipaggiamento_scarti;
	vector<Carta_equipaggiamento> Mazzo_speciale;
	vector<Carta_equipaggiamento> Mazzo_speciale_originale;
	vector<Carta_zombie> Mazzo_spawn;
	vector<Carta_zombie> Mazzo_spawn_originale;
	vector<int> ordine_giocatori;
	vector<Azione> elenco_azioni;
	vector<Azione> elenco_azioni_originale;
	vector<Abilità> elenco_abilità;
	vector <Azione> azioni_disponibili;
	vector<string> elenco_azioni_disponibili;
	Indicatori_di_situazione indicatore;
};

class azione
{
public:
	string nome;
	int costo_di_azioni;
	bool(*fattibile)(StatoGioco& stato);
	int(*esegui)(StatoGioco& stato, ALLEGRO_EVENT& evento);
	void(*grafica)(StatoGioco& stato, Immagini& immagini_e_fonts);
};



