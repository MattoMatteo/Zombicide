#pragma once
#include"dati.h"

//Public:
int starts_with_a_pistol(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione);
int medic(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione);
int born_leader(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione);
int one_die_melee(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione);
int one_die_ranged(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione);
int one_die_combact(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione);
int one_to_dice_roll_ranged(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione);
int one_to_dice_roll_melee(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione);
int one_to_dice_roll_combact(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione);
int one_re_roll_per_turn(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione);
int lucky(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione);
int one_free_move_action(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione);
int one_free_search_action(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione);
int one_free_combact_action(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione);
int matching_set(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione);
int ambidexytous(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione);
int slippery(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione);
int one_zone_per_move(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione);
int tough(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione);
int sniper(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione);
int one_action(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione);
//Private
int starts_with_a_weapon(StatoGioco& stato, string weapon, Abilità a, int giocatore_che_la_utilizza);
int free_action(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione, string nome_azione);
int born_leader_mark(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione);
int special_action(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione, string nome_azione);