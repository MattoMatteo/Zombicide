#pragma once
#include "dati.h"

int scambia(StatoGioco&stato, ALLEGRO_EVENT event);
int cerca(StatoGioco& stato, ALLEGRO_EVENT event);
int modifica_equip(StatoGioco&stato, ALLEGRO_EVENT event, int indice_giocatore);
void Azione_gestisciEquip_ordinamento(StatoGioco&stato, int personaggio, int& indice_primo_oggetto, int& indice_secondo_oggetto);
int Azione_gestisciEqup_selezione_personaggi(StatoGioco& stato, ALLEGRO_EVENT event);
int Azione_gestisciEquip_selezione_equip(StatoGioco&stato, ALLEGRO_EVENT event, string scambia_organizza,
	string selezione_blu_verde, int giocatore, int& indice_equip_arma_selezionata);
void Azione_gestisciEquip_scambioEquip(StatoGioco&stato, ALLEGRO_EVENT event, int giocatore_1, int giocatore_2, int& indice_equip_giocatore_di_turno, int& indice_equip_altro_giocatore);
void gestione_equip_grafica(StatoGioco& stato, Immagini& immagini_e_fonts, int giocatore_1);