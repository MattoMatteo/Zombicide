#pragma once
#include "dati.h"

bool spawn_zombie(StatoGioco& stato, ALLEGRO_EVENT event, vector<int>& lista_punti_spawn);

bool attiva_zombie(StatoGioco& stato, ALLEGRO_EVENT event, string zombie_da_attivare);

void umano_in_vista(StatoGioco& stato,vector<int>& umani_visti, int stanza, int verso, int peso);

void zombie_cerca_obiettivo(StatoGioco& stato, vector<int>& obiettivi_zombie, int i);

bool muovi_zombie(StatoGioco& stato, string zombie, vector<DatiStanza>& copia_dati, int i,
	bool azioni_walker, bool  azioni_fat, bool  azioni_abominio, bool azioni_runner);

bool attacca_zombie(StatoGioco& stato, string zombie, int i, bool azioni_walker, bool azioni_fat, bool azioni_abominio, int azioni_runner);

int azione_zombie(StatoGioco& stato, string zombie);

void scelta_movimento_abominio(StatoGioco& stato, vector<int>& zone_di_movimento_abominio,int& zona_abominio);

void tombini_attivi(StatoGioco& stato, vector<int>& tombini_attivi);

int calcola_ferite(StatoGioco& stato, string zombie_da_attivare);

void draw_spawn_zombie(StatoGioco& stato, Immagini& immagini_e_fonts, vector<int>elenco_spawn);
bool mostra_risultati(StatoGioco& stato,ALLEGRO_EVENT event);