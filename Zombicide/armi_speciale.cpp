#include "utilità.h"
#include "armi_speciale.h"
#include "Attacco.h"
#include"Script gestione equip.h"

int plenty_of_ammo_shotgun_speciale(StatoGioco& stato,ALLEGRO_EVENT event)
{
	if (stato.indicatore.arma_selezionata.nome == "Ma's Shotgun" || stato.indicatore.arma_selezionata.nome == "Sawed Off" ||
		stato.indicatore.arma_selezionata.nome == "Shotgun")
	{
		return 1;
	}
	return 0;
}
int pan_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	return Costruttore_arma(stato, event, false, false,false, 0);
}
int ma_s_shotgun_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	return Costruttore_arma(stato, event, true, false, true, 2);
}
int molotov_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	int risultato;
	switch (stato.indicatore.stato_azione_arma)
	{
	case 0:
		risultato = Attacca_scegliZona(stato, event);
		if ( risultato == 1)
			stato.indicatore.stato_azione_arma = 1;
		else if (risultato == -1)
		{
			stato.indicatore.stato_azione_attacca = 0;
			return -1;
		}
		break;
	case 1:
		Attacca_molotov(stato);
		if (stato.indicatore.giocatore_di_turno >= 0) //se è -1 vuol dire che chi ha usato la molotov si è suicidato.
													  //poi con il prox turno torna a 0, ma per ora evitiamo di controllare un array in posizione -1
			stato.indicatore.stato_azione_arma++;
		else
			return 1;
		break;
	case 2://rioganizza gratis
		stato.indicatore.stato_azione_arma_grafica = 5;
		if (modifica_equip(stato, event,stato.ordine_giocatori[stato.indicatore.giocatore_di_turno]-1) == 1)
		{
			stato.indicatore.stato_azione_arma_grafica = 0;
			stato.indicatore.stato_azione_arma = 0;
			stato.indicatore.stato_azione_attacca = 0;
			return 1;
		}
		break;
	}
	return 0;
}
int evil_twins_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	return Costruttore_arma(stato, event, false, false, false, 1);
}
int flashlight_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	return 0;
}
int wounded_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	return 0;
}
int glass_bottles_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	bool almeno_una_combinazione_fattibile;
	Carta_equipaggiamento equipaggiamento;
	vector<Carta_equipaggiamento> reagenti;
	//iniziamo a valutare le sue possibili combinazioni
	almeno_una_combinazione_fattibile = false;
	for (int i = 0; i < (int)stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento.size(); i++)
	{
		//molotov
		if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[i].nome == "Gasoline")
		{
			almeno_una_combinazione_fattibile = true;
			cerca_carta_in_un_mazzo(equipaggiamento, "Glass Bottles", stato.Mazzo_equipaggiamento, false);
			reagenti.push_back(equipaggiamento);
			cerca_carta_in_un_mazzo(equipaggiamento, "Gasoline", stato.Mazzo_equipaggiamento, false);
			reagenti.push_back(equipaggiamento);
			if (verifica_ridondanza_nella_lista_di_combinazioni(stato,reagenti) == false) //verifico non ridondanza
			{
				stato.indicatore.lista_combinabili.push_back(reagenti); //inserisco i reagenti della combninazione nella lista
				cerca_carta_in_un_mazzo(equipaggiamento, "Molotov", stato.Mazzo_speciale, false);
				stato.indicatore.prodotti_combinazione.push_back(equipaggiamento); //inserisco anche il prodotto (molotov) in un altro vettore
			}
		}
	}
	if (almeno_una_combinazione_fattibile == true)
		return 1;
	return 0;
}
int water_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	return 0;
}
int gasoline_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	bool almeno_una_combinazione_fattibile;
	Carta_equipaggiamento equipaggiamento;
	vector<Carta_equipaggiamento> reagenti;
	//iniziamo a valutare le sue possibili combinazioni
	almeno_una_combinazione_fattibile = false;
	for (int i = 0; i < (int)stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento.size(); i++)
	{
		//molotov
		if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[i].nome == "Glass Bottles")
		{
			almeno_una_combinazione_fattibile = true;
			cerca_carta_in_un_mazzo(equipaggiamento, "Glass Bottles", stato.Mazzo_equipaggiamento, false);
			reagenti.push_back(equipaggiamento);
			cerca_carta_in_un_mazzo(equipaggiamento, "Gasoline", stato.Mazzo_equipaggiamento, false);
			reagenti.push_back(equipaggiamento);
			if (verifica_ridondanza_nella_lista_di_combinazioni(stato, reagenti) == false) //verifico non ridondanza
			{
				stato.indicatore.lista_combinabili.push_back(reagenti); //inserisco i reagenti della combninazione nella lista
				cerca_carta_in_un_mazzo(equipaggiamento, "Molotov", stato.Mazzo_speciale, false);
				stato.indicatore.prodotti_combinazione.push_back(equipaggiamento); //inserisco anche il prodotto (molotov) in un altro vettore
			}
		}
	}
	if (almeno_una_combinazione_fattibile == true)
		return 1;
	return 0;
}
int goalie_mask_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	stato.giocatore[stato.indicatore.scelta_personaggio_da_ferire[stato.indicatore.azioneZombie_umanoDaFerire]]
		.equipaggiamento.erase(stato.giocatore[stato.indicatore.scelta_personaggio_da_ferire[stato.indicatore.azioneZombie_umanoDaFerire]]
			.equipaggiamento.begin() + stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore);
	return 0;
}
int scope_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	bool almeno_una_combinazione_fattibile;
	Carta_equipaggiamento equipaggiamento;
	vector<Carta_equipaggiamento> reagenti;
	//iniziamo a valutare le sue possibili combinazioni
	almeno_una_combinazione_fattibile = false;
	for (int i = 0; i < (int)stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento.size(); i++)
	{
		//sniper rifle
		if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[i].nome == "Rifle")
		{
			almeno_una_combinazione_fattibile = true;
			cerca_carta_in_un_mazzo(equipaggiamento, "Scope", stato.Mazzo_equipaggiamento, false);
			reagenti.push_back(equipaggiamento);
			cerca_carta_in_un_mazzo(equipaggiamento, "Rifle", stato.Mazzo_equipaggiamento, false);
			reagenti.push_back(equipaggiamento);
			if (verifica_ridondanza_nella_lista_di_combinazioni(stato, reagenti) == false) //verifico non ridondanza
			{
				stato.indicatore.lista_combinabili.push_back(reagenti); //inserisco i reagenti della combninazione nella lista
				cerca_carta_in_un_mazzo(equipaggiamento, "Sniper Rifle", stato.Mazzo_speciale, false);
				stato.indicatore.prodotti_combinazione.push_back(equipaggiamento); //inserisco anche il prodotto (sniper rifle) in un altro vettore
			}
		}
	}
	if (almeno_una_combinazione_fattibile == true)
		return 1;
	return 0;
}
int plenty_of_ammo_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	if (stato.indicatore.arma_selezionata.nome == "Pistol" || stato.indicatore.arma_selezionata.nome == "Evil Twins" ||
		stato.indicatore.arma_selezionata.nome == "Rifle"|| stato.indicatore.arma_selezionata.nome=="Sniper Rifle")
	{
		return 1;
	}
	return 0;
}
int aaahh_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	return 0;
}
int bag_of_rice_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	return 0;
}
int canned_food_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	return 0;
}
int baseball_bat_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	return Costruttore_arma(stato, event, false, false, false, 0);
}
int katana_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	return Costruttore_arma(stato, event, false, false,false, 0);
}
int machete_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	return Costruttore_arma(stato, event, false, false,false, 0);
}
int fire_axe_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	return Costruttore_arma(stato, event, false, false,false, 0);
}
int crowbar_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	return Costruttore_arma(stato, event, false, false,false, 0);
}
int chainsaw_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	return Costruttore_arma(stato, event, false, false,false, 0);
}
int pistol_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	return Costruttore_arma(stato, event, false, false,false, 1);
}
int sawed_off_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	return Costruttore_arma(stato, event, false, false, true, 1);
}
int sub_mg_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	return Costruttore_arma(stato, event, false, false,false, 1);
}
int shotgun_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	return Costruttore_arma(stato, event, false, false,false, 1);
}
int rifle_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	return Costruttore_arma(stato, event, false, false, false, 1);
}
int sniper_rifle_speciale(StatoGioco& stato, ALLEGRO_EVENT event)
{
	return Costruttore_arma(stato,event,false,false,false,0);
}