#pragma once
#include "dati.h"

int Costruttore_arma(StatoGioco& stato, ALLEGRO_EVENT event, bool gunblade, bool ricarica_melee, bool ricarica_ranged, int tipo);
int arma_melee_ranged_o_doppia(Carta_equipaggiamento arma);
bool cerca_zombie(StatoGioco& stato, vector<int>& stanze_di_interesse, int stanza, int verso,
	int peso, int raggio_min, int raggio_max, int contatore_distanza_raggiunta);
bool fattibilità_attacco_arma_selezionata(StatoGioco& stato, int posizione, vector<int>& stanze_di_interesse, int gittata_min, int gittata_max);
int obiettivo_ranged(StatoGioco& stato, int zona);
int Attacca_scegliZona(StatoGioco& stato, ALLEGRO_EVENT event);
int Attacca_lancia_dadi(StatoGioco& stato, ALLEGRO_EVENT event, int dadi, int successo);
bool Attacca_armaMelee(StatoGioco& stato, ALLEGRO_EVENT event);
bool Attacca_armaRanged(StatoGioco& stato, ALLEGRO_EVENT event);
int Attacca_armaGunblade(StatoGioco& stato, ALLEGRO_EVENT event);
int Attacca_doppiaArma(StatoGioco& stato, ALLEGRO_EVENT event);//-1:nessuna doppia arma,0:non ha finito,1:doppia arma presente
int Attacca_molotov(StatoGioco& stato);
int Attacca_ricarica(StatoGioco& stato, ALLEGRO_EVENT event, string melee_o_ranged);
void selezione_obiettivi_in_zona(StatoGioco& stato, ALLEGRO_EVENT event, int zona);