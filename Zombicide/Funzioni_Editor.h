#pragma once
#include "dati.h"

void aggiungi_parametri_ruotati_di_oggetto_tessere_editor(Parametri_Tessera p, Parametri_Tessera& t, int y_dim_tessera);
void trova_zone_adiacenti_fondibili(int i, int j, int k, vector<vector<Tessere_editor>> Mappa_matrice, vector<vector<int>>& zone_fondibili);
void unisci_tessere_a_formare_mappa_editor(vector<Tessere_editor> Mappa_array, Missione& t, vector<Porta>& porte_inseribili);
void inserisci_oggetto_in_zona(StatoGioco& stato, int indice_oggetto, int zona, Missione& t, string inserisci_elimina);
void salva_mappa(Missione& t, string nome);
