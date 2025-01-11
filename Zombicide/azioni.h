#pragma once

#include "dati.h"

bool movimento_fattibile(StatoGioco& stato);
int movimento_esegui(StatoGioco& stato, ALLEGRO_EVENT& evento);
void movimento_grafica(StatoGioco& stato, Immagini& immagini_e_fonts);


bool aprire_porta_fattibile(StatoGioco& stato);
int aprire_porta_esegui(StatoGioco& stato, ALLEGRO_EVENT& evento);
void aprire_porta_grafica(StatoGioco& stato, Immagini& immagini_e_fonts);


bool termina_turno_fattibile(StatoGioco& stato);
int termina_turno_esegui(StatoGioco& stato, ALLEGRO_EVENT& evento);
void termina_turno_grafica(StatoGioco& stato, Immagini& immagini_e_fonts);


bool fuggi_fattibile(StatoGioco& stato);
int fuggi_esegui(StatoGioco& stato, ALLEGRO_EVENT& evento);
void fuggi_grafica(StatoGioco& stato, Immagini& immagini_e_fonts);

bool scambia_equipaggiamento_fattibile(StatoGioco& stato);
int scambia_equipaggiamento_esegui(StatoGioco& stato, ALLEGRO_EVENT& evento);
void scambia_equipaggiamento_grafica(StatoGioco& stato, Immagini& immagini_e_fonts);

bool scambia_da_solo_fattibile(StatoGioco& stato);
int scambia_da_solo_esegui(StatoGioco& stato, ALLEGRO_EVENT& evento);
void scambia_da_solo_grafica(StatoGioco& stato, Immagini& immagini_e_fonts);

bool cerca_fattibile(StatoGioco& stato);
int cerca_esegui(StatoGioco& stato, ALLEGRO_EVENT& evento);
void cerca_grafica(StatoGioco& stato, Immagini& immagini_e_fonts);

bool attacca_fattibile(StatoGioco& stato);
int attacca_esegui(StatoGioco& stato, ALLEGRO_EVENT& evento);
void attacca_grafica(StatoGioco& stato, Immagini& immagini_e_fonts);

bool combina_fattibile(StatoGioco& stato);
int combina_esegui(StatoGioco& stato, ALLEGRO_EVENT& evento);
void combina_grafica(StatoGioco& stato, Immagini& immagini_e_fonts);

bool medic_fattibile(StatoGioco& stato);
int medic_esegui(StatoGioco& stato, ALLEGRO_EVENT& event);
void medic_grafica(StatoGioco& stato, Immagini& immagini_e_fonts);

bool born_leader_fattibile(StatoGioco& stato);
int born_leader_esegui(StatoGioco& stato, ALLEGRO_EVENT& event);
void born_leader_grafica(StatoGioco& stato, Immagini& immagini);
