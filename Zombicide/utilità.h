#pragma once
#include"dati.h"

void scarica_free_action(StatoGioco& stato, int n_giocatore);
void inizializza_abilità(StatoGioco& stato);
void draw_finestra_con_elenco_e_cursore(StatoGioco& stato, Immagini& immagini_e_fonts, string font, ALLEGRO_COLOR colore, Cursore& c,
	vector<string> elenco, int x_partenza, int y_partenza, int incremento_y, int incremento_x, int numero_colonne, int x_dim_max, int y_dim_max,
	int y_dim_font, int x_dim_cursore, int y_dim_cursore, MATTEO_BITMAP* immagine_finestra, bool abilita_tinta_finestra, ALLEGRO_COLOR tinta_finestra,
	ALLEGRO_COLOR tinta_cornice, float spessore_cornice);
void preparo_mappa_pulita(Missione& m, int dim_tessere);
void disegna_collage_schede_personaggi(Giocatore giocatore[10],MATTEO_BITMAP* collage_bitmap,vector<int> ordine_giocatori);
void logic_movimento_collage_schede_personaggi(int& scroll, int scroll_max);
void al_draw_scaled_text(vector<fonts>& fonts, Immagini& immagini_e_fonts, MATTEO_BITMAP*immagini_su_cui_stampare, string font, ALLEGRO_COLOR colore, float x, float y,
	float x_dim /*negativo per auto proporzione con altezza e diventa x_dim_MAX*/,
	float y_dim/*setta -1 per autoproporzionare con lunghezza*/, int flags, string text);
void inquadra_zona_mappa(StatoGioco& stato, int zona, int larghezza, int altezza);
void input_tastiera_9_direzioni(int& iteratore_cursore, ALLEGRO_EVENT& event, vector<int> vettore_con_le_9_size_di_direzione, bool ruota_90_sx, bool inverti_x, bool inverti_y);
void input_tastiera_classico(int& iteratore_cursore, ALLEGRO_EVENT& event, vector<int> numero_colonne_per_ogni_riga, bool indici_verticali, bool ruota_matrice, bool inverti_x, bool inverti_y);
vector<int> creazione_vettore_per_input_direzioni_con_ordinamento_di_zone(StatoGioco& stato, vector<int>& zone_di_movimento_disponibili,
	int posizione);
bool input_mouse(int n, vector<vector<int>> x_y_w_h /*vettore int [4]*/, Cursore& c, bool attiva_scroll, int risoluzione[2], bool movimento_mouse);
int calcola_pericolo(StatoGioco& stato);
void ricarica_armi(StatoGioco& stato);
bool cerca_abominio(StatoGioco& stato);
bool timer_mio(int &iteratore, float secondi);
bool edificio_aperto(StatoGioco& stato, vector<int> edificio);
void draw_dadi(StatoGioco& stato, Immagini& immagini_e_fonts, int precisione);
void draw_menu_assegna_ferite(StatoGioco& stato, Immagini& immagini_e_fonts);
void inizializza_mazzo_equipaggiamento(StatoGioco& stato, Immagini& immagini_e_fonts);
void mescola_mazzo_equipaggiamento(StatoGioco& stato, Immagini& immagini_e_fonts);
void draw_schede_personaggi(StatoGioco& stato, Immagini& immagini_e_fonts, int indice_giocatore);
void draw_pulsante_pericolo(StatoGioco& stato, Immagini& immagini_e_fonts, fonts f, float dx, float dy, float dw, float dh);
void determina_stanze_edificio(StatoGioco& stato, vector<int>& edificio, vector<int> inserisci_un_vettore_vuoto, int prima_stanza_edificio, int stanza_apertura_porta);
void prepara_carta_equipaggiamento(string nome, vector<Carta_equipaggiamento>& mazzo, MATTEO_BITMAP* immagine, int tipologia,
	int gittata_min_melee, int gittata_max_melee, int gittata_min_ranged, int gittata_max_ranged, int dadi_melee, int dadi_ranged, int precisione_melee, int precisione_ranged, int danno_melee,
	int danno_ranged, bool rumore_dopo_attacco_melee, bool rumore_dopo_attacco_ranged, bool doppia, int apre_porte, int(*speciale)(StatoGioco& stato, ALLEGRO_EVENT evento));
vector<vector<int>> cordinate_elenco_scritte(int x_partenza, int y_partenza, int w, int h, int numero, int numero_colonne, int y_font, int x_incremento, int y_incremento);
void draw_schede_personaggi_con_o_senza_selezione(StatoGioco& stato, Immagini& immagini_e_fonts,
	vector<int>schede_personaggi_da_disegnare, int numero_schede_per_colonna, int spazio_tra_schede_x,
	int spazio_tra_schede_y, bool selezione, int iteratore_curosre);

void draw_cursore_su_equip_personaggio(StatoGioco& stato, Immagini& immagini_e_fonts, int personaggio, int size_equip, ALLEGRO_COLOR colore_selezione, int indice_equip_selezionato);

void draw_selezione_zona_mappa(StatoGioco& stato, MATTEO_BITMAP* mappa, int zona, ALLEGRO_COLOR colore);
bool cerca_carta_in_un_mazzo(Carta_equipaggiamento& carta, string nome, vector<Carta_equipaggiamento>& mazzo, bool togli_da_mazzo);
void uccidi_giocatore(StatoGioco& stato, int giocatore);
void abort_game(const char* message);

void percorsi(StatoGioco& stato);
void disegna_tessera(Immagini& immagini_e_fonts, vector<fonts>& font, Giocatore* giocatore, MATTEO_BITMAP* Immagine_su_cui_disegnare,
	int i_mappa_matrice, int j_mappa_matrice, Missione& m);
void disegna_zone(Immagini& immagini_e_fonts, vector<fonts> font, Giocatore* giocatore, vector<Personaggio>& Personaggi,
	Missione& m, double zoom, int x_riquadro_mappa, int y_riquadro_mappa, int width_riquadro, int height_riquadro);
int arma_melee_ranged_o_doppia(Carta_equipaggiamento arma);
bool assegna_ferite(StatoGioco& stato, ALLEGRO_EVENT event, ALLEGRO_KEYBOARD_STATE& keyboard_state);
void disegna_mappa(StatoGioco& stato, Immagini immagini_e_fonts, Missione& T, int width_riquadro, int height_riquadro);

void crea_o_ripristina_backup_equipaggiamento(StatoGioco& stato, string crea_o_ripristina, vector<int> personaggi_di_interesse);
void converti_cordinate_mappa_per_draw(double zoom, vector<vector<Tessere_editor>> Mappa_matrice, int x_riquadro, int y_riquadro, int w_finestra_su_cui_stampare, int h_finestra_su_cui_stampare,
	int& dx, int& dy, int& dw, int& dh, int& dw_riquadro_zoom, int& dh_riquadro_zoom);
void draw_selezione_segnalini_zombie_o_personaggi(StatoGioco& stato, Immagini& immagini_e_fonts, vector<int> zombie_o_giocatori_selezionabili, int posizione_cursore,
	int zona, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
bool verifica_ridondanza_nella_lista_di_combinazioni(StatoGioco& stato, vector<Carta_equipaggiamento> reagenti);
bool aggiungi_abilità_giocatore(StatoGioco& stato, ALLEGRO_EVENT event, int indice_giocatore);
bool menu_opzioni_logic(StatoGioco& stato, ALLEGRO_EVENT event, ALLEGRO_DISPLAY*& display);
void draw_menu_opzioni(StatoGioco& stato, Immagini& immagini_e_fonts);
bool logic_spawn_zombie(StatoGioco& stato, ALLEGRO_EVENT event, vector<int> elenco_stanze_spawn);
float rotazione_in_base_direzione_zona(int direzione);
int cerca_direzione_stanza_lontana(StatoGioco& stato, int posizione, int obiettivo);
void draw_lista_tasti(StatoGioco& stato, Immagini& immagini_e_fonts, MATTEO_BITMAP* bitmap_in_cui_disegnare);
void inizializza_azioni(StatoGioco& stato);
void m_draw_scaled_tinted_bitmap(StatoGioco& stato, Immagini& immagini_e_fonts, MATTEO_BITMAP* immagine_da_disegnare, int x, int y, int x_dim, int y_dim, ALLEGRO_COLOR tinta);
int stanze_raggiungibili_fino_a_x_distanza_da_un_punto(StatoGioco& stato, vector<int>& stanze, int numero_passi_disponibili, int partenza, bool peso);
void logic_zoom_e_movimento_mappa(StatoGioco& stato, int larghezza, int altezza);
bool aggiungi_abilità_giocatore(StatoGioco& stato, ALLEGRO_EVENT event, int indice_giocatore);
void draw_aggiungi_abilità_giocatore(StatoGioco& stato, Immagini& immagini_e_fonts, int indice_giocatore);
void input_stringa_da_tastiera(ALLEGRO_EVENT &event, string& s, string::iterator& i);
void draw_input_stringa_tastiera(StatoGioco& stato, Immagini immagini_e_fonts, string s, fonts f, ALLEGRO_COLOR colore, int font_dim, string::iterator i, MATTEO_BITMAP* bitmap_su_cui_stampare, MATTEO_BITMAP* sfondo_finestra, int x_centro_finestra, int y_centro_finestra);
bool conversione_allegro_keycode_in_char(ALLEGRO_KEYBOARD_STATE keyboard_state, char& c);
void draw_selezione_segnalini_porte(StatoGioco& stato, Immagini& immagini_e_fonts, Porta& p, Missione& m,
	double zoom, int x_riquadro_mappa, int y_riquadro_mappa, int width_riquadro, int height_riquadro,
	unsigned char r, unsigned char g, unsigned char b, unsigned char a);