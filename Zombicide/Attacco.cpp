#include "Attacco.h"
#include "dati.h"
#include "utilità.h"
#include <algorithm>

//script comuni già composti
int Costruttore_arma(StatoGioco& stato, ALLEGRO_EVENT event,bool gunblade, bool ricarica_melee,bool ricarica_ranged,int tipo)
{
	int risultato;
	switch (stato.indicatore.stato_azione_arma)
	{
	case 0: //Speciale, arma gunblade
		if (gunblade == true)
		{
			stato.indicatore.tipo_arma_selezionata = Attacca_armaGunblade(stato, event);//ha scelto : 0-melee 1-ranged e la scelta era possibile
			if (stato.indicatore.tipo_arma_selezionata == 0|| stato.indicatore.tipo_arma_selezionata == 1)
				stato.indicatore.stato_azione_arma++;
			else if (stato.indicatore.tipo_arma_selezionata == -2)//ha premuto escape
			{
				return -1;
			}
		}
		else //procedura normale, non è un arma tipo gunblade
		{
			stato.indicatore.tipo_arma_selezionata = tipo;
			stato.indicatore.stato_azione_arma++;
		}
		break;
	case 1://doppia
		if (stato.indicatore.arma_selezionata.doppia == true)
		{
			risultato = Attacca_doppiaArma(stato, event);
			if (risultato == 1 || risultato == -1)
				stato.indicatore.stato_azione_arma++;
			if (risultato == 1)
				stato.indicatore.moltiplicatore_dadi = 2;
			else if(risultato==-1)
				stato.indicatore.moltiplicatore_dadi = 1;
		}
		else
		{
			stato.indicatore.moltiplicatore_dadi = 1;
			stato.indicatore.stato_azione_arma++;
		}
		break;
	case 2: //verifica ricarica
		if (ricarica_melee == true && stato.indicatore.tipo_arma_selezionata == 0)
		{
			risultato = Attacca_ricarica(stato, event, "melee");
			if (risultato == 1)
				stato.indicatore.stato_azione_arma++;
			else if (risultato == -1)
			{
				stato.indicatore.stato_azione_attacca = 0;
				stato.indicatore.stato_azione_arma = 0;
				return -1;
			}
		}
		else if (ricarica_ranged == true && stato.indicatore.tipo_arma_selezionata == 1)
		{
			risultato = Attacca_ricarica(stato, event, "ranged");
			if (risultato == 1)
				stato.indicatore.stato_azione_arma++;
			else if (risultato == -1)
			{
				stato.indicatore.stato_azione_attacca = 0;
				stato.indicatore.stato_azione_arma = 0;
				return -1;
			}
		}
		else
			stato.indicatore.stato_azione_arma++;
		break;
	case 3://scegli zona
		risultato = Attacca_scegliZona(stato, event);
		if (risultato == 1)
			stato.indicatore.stato_azione_arma++;
		else if (risultato == -1)
		{
			stato.indicatore.stato_azione_attacca = 0;
			stato.indicatore.stato_azione_arma = 0;
			return -1;
		}
		break;
	case 4://lancia dadi
		if (stato.indicatore.tipo_arma_selezionata == 0) //melee
		{
			risultato = Attacca_lancia_dadi(stato, event, stato.indicatore.arma_selezionata.dadi_melee*stato.indicatore.moltiplicatore_dadi, stato.indicatore.arma_selezionata.precisione_melee);
			if (risultato == 1)//finito
			{
				stato.indicatore.stato_azione_arma++;
				if (ricarica_melee == true)
				{
					if (stato.indicatore.arma_selezionata.doppia == true && stato.indicatore.doppia == true)
					{
						stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[0].scarica_melee = true;
						stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[1].scarica_melee = true;
					}
					else
					{
						stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].
							equipaggiamento[stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore].scarica_melee = true;
						stato.indicatore.arma_selezionata.scarica_melee = true;
					}
				}
			}
		}
		else if (stato.indicatore.tipo_arma_selezionata == 1)
		{
			risultato = Attacca_lancia_dadi(stato, event, stato.indicatore.arma_selezionata.dadi_ranged*stato.indicatore.moltiplicatore_dadi, stato.indicatore.arma_selezionata.precisione_ranged);
			if (risultato == 1)//finito
			{
				stato.indicatore.stato_azione_arma++;
				if (ricarica_ranged == true) //scarica
				{
					if (stato.indicatore.arma_selezionata.doppia == true && stato.indicatore.doppia == true) //tutte e due se è doppia
					{
						stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[0].scarica_ranged = true;
						stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[1].scarica_ranged = true;
					}
					stato.indicatore.arma_selezionata.scarica_ranged = true;
					stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].
						equipaggiamento[stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore].scarica_ranged = true;
				}
			}
		}
		break;
	case 5://attacca
		switch (stato.indicatore.tipo_arma_selezionata)
		{
		case 0://melee
			if (Attacca_armaMelee(stato, event) == true)
			{
				stato.indicatore.stato_azione_arma = 0;
				stato.indicatore.stato_azione_attacca = 0;
				stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore = 0;
				return 1;
			}
			break;
		case 1://ranged
			if (Attacca_armaRanged(stato, event) == true)
			{
				stato.indicatore.stato_azione_arma = 0;
				stato.indicatore.stato_azione_attacca = 0;
				stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore = 0;
				return 1;
			}
			break;
		}
		break;
	}
	return 0;
}

//script
//1:finito,0:non finito,-1:esc
int Attacca_scegliZona(StatoGioco& stato, ALLEGRO_EVENT event)
{
	stato.indicatore.stato_azione_arma_grafica = 0;
	int posizione = stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione;
	vector<int> stanze_di_interesse;
	int gittata_min = 0;
	int gittata_max = 0;
	switch (stato.indicatore.tipo_arma_selezionata)//scegliere le gittate adatte al tipo di arma, per poi trovare le stanze di interesse
	{
	case 0://melee
		gittata_min = stato.indicatore.arma_selezionata.gittata_min_melee;
		gittata_max = stato.indicatore.arma_selezionata.gittata_max_melee;
		break;
	case 1://ranged
		gittata_min = stato.indicatore.arma_selezionata.gittata_min_ranged;
		gittata_max = stato.indicatore.arma_selezionata.gittata_max_ranged;
		break;
	}
	fattibilità_attacco_arma_selezionata(stato, posizione, stanze_di_interesse,gittata_min,gittata_max);
	stato.Cursori["Scegli zona"]["Lista zone"];
	input_tastiera_9_direzioni(stato.Cursori["Scegli zona"]["Lista zone"].iteratore, event,
		creazione_vettore_per_input_direzioni_con_ordinamento_di_zone(stato,stanze_di_interesse, posizione), false, false, false);
	if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE&&event.type == ALLEGRO_EVENT_KEY_DOWN)
	{
		stato.Cursori["Scegli zona"]["Lista zone"].iteratore = 0;
		return -1;
	}
	if (event.keyboard.keycode == ALLEGRO_KEY_ENTER&&event.type == ALLEGRO_EVENT_KEY_DOWN)
	{
		stato.indicatore.stanza_selezionata = stanze_di_interesse[stato.Cursori["Scegli zona"]["Lista zone"].iteratore];
		stato.Cursori["Scegli zona"]["Lista zone"].iteratore = 0;
		return 1;
	}
	return 0;
}
int Attacca_armaGunblade(StatoGioco& stato, ALLEGRO_EVENT event) 
{ //0:melee,1:ranged,-1:non concluso,-2:escape
	stato.indicatore.stato_azione_arma_grafica = 4; //scelta generica
	stato.indicatore.scelte_messaggio_generico.clear();
	stato.indicatore.dadi.clear();
	stato.indicatore.messaggio_generico.clear();
	stato.indicatore.scelte_messaggio_generico.push_back("Ravvicinato");
	stato.indicatore.scelte_messaggio_generico.push_back("A distanza");
	ALLEGRO_KEYBOARD_STATE keyboard_state;
	al_get_keyboard_state(&keyboard_state);
	vector<int> fattibilità_arma_selezionata;
	vector<int> stanze_di_interesse;
	int posizione = stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione;
	stato.Cursori["Azione attacca, scelta generica"]["Lista"];
	input_tastiera_9_direzioni(stato.Cursori["Azione attacca, scelta generica"]["Lista"].iteratore, event, {2,0,0,0,0,0,0,0,0}, true, false, false);
	if (event.type == ALLEGRO_EVENT_KEY_DOWN)
	{
		if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
		{
			if (fattibilità_attacco_arma_selezionata(stato, posizione, stanze_di_interesse,
				stato.indicatore.arma_selezionata.gittata_min_melee, stato.indicatore.arma_selezionata.gittata_max_melee) == true)
				fattibilità_arma_selezionata.push_back(0);
			if (fattibilità_attacco_arma_selezionata(stato, posizione, stanze_di_interesse,
				stato.indicatore.arma_selezionata.gittata_min_ranged, stato.indicatore.arma_selezionata.gittata_max_ranged) == true)
				fattibilità_arma_selezionata.push_back(1);
			for (int i = 0; i<(int)fattibilità_arma_selezionata.size(); i++)
				if (fattibilità_arma_selezionata[i] == stato.Cursori["Azione attacca, scelta generica"]["Lista"].iteratore)
				{
					int tipo = stato.Cursori["Azione attacca, scelta generica"]["Lista"].iteratore;
					return tipo;
				}
		}
		if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
		{
			stato.Cursori["Azione attacca, scelta generica"]["Lista"].iteratore = 0;
			stato.indicatore.stato_azione_arma_grafica = 0;
			return -2;
		}
	}
	return -1;
}
int Attacca_lancia_dadi(StatoGioco& stato, ALLEGRO_EVENT event, int dadi, int successo) //true: almeno 1 dado successo, false: nessun dado successo.
{
	stato.indicatore.stato_azione_arma_grafica = 1;
	switch (stato.indicatore.sub_stato_dadi_azione_arma)
	{
	case 0://carica possibili re-roll in lista
		stato.indicatore.Lista_re_roll.clear();
		for (size_t i = 0; i < stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento.size(); i++)
			if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[i].tipologia == Carta_equipaggiamento::MUNIZIONI)
				if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[i].speciale(stato, event) == 1)
				{
					stato.indicatore.Lista_re_roll.push_back(stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[i].nome);
				}
		for (size_t i = 0; i < stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità.size(); i++)
			if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].tipo == Abilità::RE_ROLL)
				stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].funzione
				(stato, stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1, i, Abilità::CODICE_ESEGUI);
		stato.indicatore.sub_stato_dadi_azione_arma++;
	case 1://lancia i dadi
		stato.indicatore.abilità_dadi_usate.clear();
		stato.indicatore.dadi_successo = 0;
		stato.indicatore.dadi.clear();
		for (int i = 0; i < dadi; i++)
			stato.indicatore.dadi.push_back(rand() % 6 + 1);

		for (size_t i = 0; i < stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità.size(); i++)
			if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].tipo == Abilità::DADI)
				stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].funzione(
					stato, stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1, i, Abilità::CODICE_ESEGUI);

		for (int i = 0; i < (int)stato.indicatore.dadi.size(); i++)
			if (stato.indicatore.dadi[i] >= successo)
				stato.indicatore.dadi_successo++;
		stato.indicatore.sub_stato_dadi_azione_arma++;
	case 2: //scegli re-roll
		if (stato.indicatore.Lista_re_roll.size()>0)
		{
			input_tastiera_classico(stato.Cursori["Lista re-roll"]["Lista"].iteratore, event, { (int)stato.indicatore.Lista_re_roll.size() + 1 }, false, true, false, false);
			if (event.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
				{
					if (stato.Cursori["Lista re-roll"]["Lista"].iteratore != stato.indicatore.Lista_re_roll.size()) //diverso da annulla
					{
						stato.indicatore.Lista_re_roll.erase(stato.indicatore.Lista_re_roll.begin() + stato.Cursori["Lista re-roll"]["Lista"].iteratore);
						stato.Cursori["Lista re-roll"]["Lista"].iteratore = 0;
						stato.indicatore.sub_stato_dadi_azione_arma--;
					}
					else //allora esci e prosegui
					{
						stato.Cursori["Lista re-roll"]["Lista"].iteratore = 0;
						stato.indicatore.sub_stato_dadi_azione_arma++;
					}
				}
				if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
					stato.Cursori["Lista re-roll"]["Lista"].iteratore = (int)stato.indicatore.Lista_re_roll.size(); //ultimo elemento: Annulla
			}
		}
		else //nessun re roll disponibile. Visualizza dadi e dopo Esci e prosegui
			if (event.type == ALLEGRO_EVENT_KEY_DOWN)
				stato.indicatore.sub_stato_dadi_azione_arma++;
		break;
	case 3://scarica alcune abilità di re-roll-> 1 re roll per turn
		for (size_t i = 0; i < stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità.size(); i++)
			if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].quando_si_scarica == Abilità::DOPO_LANCIO_DADI) //sarebbe dopo re-roll in realtà
				stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].funzione
				(stato, stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1, i, Abilità::CODICE_SCARICA);
		stato.indicatore.sub_stato_dadi_azione_arma = 0;
		return 1;
	}
	return 0;
}
bool Attacca_armaMelee(StatoGioco& stato, ALLEGRO_EVENT event)
{
	stato.indicatore.stato_azione_arma_grafica = 2;
	ALLEGRO_KEYBOARD_STATE keyboard_state;
	al_get_keyboard_state(&keyboard_state);
	int posizione = stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione;
	if (stato.indicatore.dadi_successo == 0) //finito..
	{
		if (stato.indicatore.arma_selezionata.rumore_dopo_attacco_melee == true)
			stato.missione.dati[posizione].SommaValoreRumore(1);
		stato.indicatore.doppia = false;
		stato.indicatore.stato_azione_arma_grafica = 0;
		stato.indicatore.sub_stato_melee_azione_attacca = 0;
		stato.indicatore.abilità_dadi_usate.clear();
		return true;
	}
	switch (stato.indicatore.sub_stato_melee_azione_attacca)
	{
	case 0: //selezione
		selezione_obiettivi_in_zona(stato, event, stato.indicatore.stanza_selezionata);
		if (event.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
			{
				if (stato.indicatore.zombie_o_giocatori_selezionabili_melee[stato.Cursori["Seleziona obiettivi in zona"]["Lista"].iteratore] <= 5) //selezione personaggi
				{
					if (stato.indicatore.arma_selezionata.danno_melee > 1) //uccidi direttamente
					{
						stato.giocatore[stato.indicatore.zombie_o_giocatori_selezionabili_melee[stato.Cursori["Seleziona obiettivi in zona"]["Lista"].iteratore]].stato = 0;
						stato.missione.dati[stato.indicatore.stanza_selezionata].ModificaValoreGiocatori(false,
							stato.indicatore.zombie_o_giocatori_selezionabili_melee[stato.Cursori["Seleziona obiettivi in zona"]["Lista"].iteratore]);
						uccidi_giocatore(stato, stato.indicatore.zombie_o_giocatori_selezionabili_melee[stato.Cursori["Seleziona obiettivi in zona"]["Lista"].iteratore]);
						stato.Cursori["Seleziona obiettivi in zona"]["Lista"].iteratore = 0;
						stato.indicatore.dadi_successo--;
					}
					else //cambia stato per aprire menù ferite
					{
						stato.indicatore.ferite = 1;
						stato.indicatore.sub_stato_melee_azione_attacca = 1;
						stato.indicatore.scelta_personaggio_da_ferire.clear();
						stato.indicatore.scelta_personaggio_da_ferire.push_back
							(stato.indicatore.zombie_o_giocatori_selezionabili_melee[stato.Cursori["Seleziona obiettivi in zona"]["Lista"].iteratore]);
						stato.Cursori["Seleziona obiettivi in zona"]["Lista"].iteratore = 0;
					}
				}
				else //selezione zombie
				{
					switch (stato.indicatore.zombie_o_giocatori_selezionabili_melee[stato.Cursori["Seleziona obiettivi in zona"]["Lista"].iteratore])
					{
					case 6: //walker
						stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].esperienza++;
						stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].redraw_scheda = true;
						stato.missione.dati[stato.indicatore.stanza_selezionata].SommaValoreWalker(-1);
						stato.indicatore.dadi_successo--;
						break;
					case 7: //runner
						stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].esperienza++;
						stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].redraw_scheda = true;
						stato.missione.dati[stato.indicatore.stanza_selezionata].SommaValoreRunner(-1);
						stato.indicatore.dadi_successo--;
						break;
					case 8: //fat
						stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].esperienza++;
						stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].redraw_scheda = true;
						stato.missione.dati[stato.indicatore.stanza_selezionata].SommaValoreFat(-1);
						stato.indicatore.dadi_successo--;
						break;
					case 9: //abominio
						stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].esperienza =
							stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].esperienza + 5;
						stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].redraw_scheda = true;
						stato.missione.dati[stato.indicatore.stanza_selezionata].ModificaValoreAbominio(false);
						stato.indicatore.dadi_successo--;
						break;
					}
				}
				if (stato.indicatore.zombie_o_giocatori_selezionabili_melee.size() == 0)
					stato.indicatore.dadi_successo = 0;
				stato.Cursori["Seleziona obiettivi in zona"]["Lista"].iteratore = 0;
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			{
				stato.indicatore.dadi_successo = 0;
			}
		}
		break;
	case 1: //caso in cui hai selezionato un giocatore da ferire
		
		if (assegna_ferite(stato, event, keyboard_state) == true)
		{
			stato.indicatore.dadi_successo--;
			stato.indicatore.sub_stato_melee_azione_attacca = 0;
		}
		if (event.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
		{
			stato.indicatore.ferite = 0;
			stato.indicatore.sub_stato_melee_azione_attacca = 0;
		}
		break;
	}
	return false;
}
bool Attacca_armaRanged(StatoGioco& stato, ALLEGRO_EVENT event)
{
	stato.indicatore.stato_azione_arma_grafica = 3;
	ALLEGRO_KEYBOARD_STATE keyboard_state;
	al_get_keyboard_state(&keyboard_state);
	int posizione = stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione;
	int obiettivo = obiettivo_ranged(stato, stato.indicatore.stanza_selezionata);
	if (stato.indicatore.dadi_successo == 0) //finito..
	{
		if (stato.indicatore.arma_selezionata.rumore_dopo_attacco_ranged == true)
			stato.missione.dati[posizione].SommaValoreRumore(1);
		stato.indicatore.doppia = false;
		stato.indicatore.stato_azione_arma_grafica = 0;
		stato.indicatore.sub_stato_ranged_azione_attacca = 0;
		stato.indicatore.ferite = 0;
		stato.indicatore.abilità_dadi_usate.clear();
		return true;
	}
	switch (stato.indicatore.sub_stato_ranged_azione_attacca)
	{
	case 0:
		if (obiettivo <= 5&&obiettivo>=0) //si tratta di un personaggio
		{
			vector<int> personaggi_da_ferire;
			for (int i = 0; i < stato.numero_giocatori; i++)
				if (stato.missione.dati[stato.indicatore.stanza_selezionata].giocatori[stato.ordine_giocatori[i] - 1] == true &&
					stato.ordine_giocatori[i] - 1 != stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1)
					personaggi_da_ferire.push_back(stato.ordine_giocatori[i] - 1);

			if (stato.indicatore.arma_selezionata.danno_ranged > 1) //uccidi direttamente
			{
				stato.Cursori["Seleziona obiettivi in zona"]["Lista"];
				input_tastiera_9_direzioni(stato.Cursori["Seleziona obiettivi in zona"]["Lista"].iteratore, event, { (int)personaggi_da_ferire.size(),0,0,0,0,0,0,0,0}, false, false, false);
				if (event.type == ALLEGRO_EVENT_KEY_DOWN)
				{
					if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
					{
						uccidi_giocatore(stato, personaggi_da_ferire[stato.Cursori["Seleziona obiettivi in zona"]["Lista"].iteratore]);
						stato.indicatore.dadi_successo--;
						stato.Cursori["Seleziona obiettivi in zona"]["Lista"].iteratore = 0;
					}
					if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
					{
						stato.indicatore.dadi_successo = 0;
						stato.Cursori["Seleziona obiettivi in zona"]["Lista"].iteratore = 0;
					}
				}
			}
			else//apri menu ferite
			{
				if (event.type == ALLEGRO_EVENT_KEY_DOWN)
				{
					stato.Cursori["Seleziona obiettivi in zona"]["Lista"].iteratore = 0;
					stato.indicatore.ferite = stato.indicatore.dadi_successo;
					stato.indicatore.scelta_personaggio_da_ferire.clear();
					stato.indicatore.scelta_personaggio_da_ferire = personaggi_da_ferire;
					stato.indicatore.sub_stato_ranged_azione_attacca++;
				}
			}
		}
		else if(obiettivo>5) //allora si tratta di zombie
		{
			if (event.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				switch (obiettivo)
				{
				case 6: //walker
					stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].esperienza++;
					stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].redraw_scheda = true;
					stato.missione.dati[stato.indicatore.stanza_selezionata].walker--;
					stato.indicatore.dadi_successo--;
					break;
				case 7: //runner
					stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].esperienza++;
					stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].redraw_scheda = true;
					stato.missione.dati[stato.indicatore.stanza_selezionata].runner--;
					stato.indicatore.dadi_successo--;
					break;
				case 8: //fat
					if (stato.indicatore.arma_selezionata.danno_ranged > 1)
					{
						stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].esperienza++;
						stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].redraw_scheda = true;
						stato.missione.dati[stato.indicatore.stanza_selezionata].fat--;
					}
					stato.indicatore.dadi_successo--;
					break;
				case 9: //abominio
					if (stato.indicatore.arma_selezionata.danno_ranged > 2)
					{
						stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].esperienza =
							stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].esperienza + 5;
						stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].redraw_scheda = true;
						stato.missione.dati[stato.indicatore.stanza_selezionata].abominio = false;
					}
					stato.indicatore.dadi_successo--;
					break;
				case 10: //scelta tra fat e abominio
					if (stato.indicatore.arma_selezionata.danno_ranged < 3)
					{
						if (stato.indicatore.arma_selezionata.danno_ranged == 2)
						{
							stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].esperienza++;
							stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].redraw_scheda = true;
							stato.missione.dati[stato.indicatore.stanza_selezionata].fat--;
						}
						stato.indicatore.dadi_successo--;
					}
					else
					{
						if (event.type == ALLEGRO_EVENT_KEY_DOWN)
						{
							stato.Cursori["Scelta tra abominio e fat"]["Abominio o fat"];
							input_tastiera_9_direzioni(stato.Cursori["Scelta tra abominio e fat"]["Abominio o fat"].iteratore, event, { 2,0,0,0,0,0,0,0,0 }, false, false, false);
							if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
							{
								if (stato.Cursori["Scelta tra abominio e fat"]["Abominio o fat"].iteratore == 0) //fat
								{
									stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].esperienza++;
									stato.missione.dati[stato.indicatore.stanza_selezionata].fat--;
								}
								else if (stato.Cursori["Scelta tra abominio e fat"]["Abominio o fat"].iteratore == 1) //abominio
								{
									stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].esperienza =
										stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].esperienza + 5;
									stato.missione.dati[stato.indicatore.stanza_selezionata].abominio = false;
								}
								stato.indicatore.dadi_successo--;
							}
						}
					}
				}
			}
		}	
		else if(obiettivo<0)
			stato.indicatore.dadi_successo = 0;
		break;
	case 1://assegna ferite
		if (assegna_ferite(stato, event, keyboard_state) == true)
		{
			stato.indicatore.dadi_successo--;
			if(stato.indicatore.dadi_successo==0)
				stato.indicatore.sub_stato_ranged_azione_attacca = 0;
		}
		break;
	}
	return false;
}
int Attacca_doppiaArma(StatoGioco& stato, ALLEGRO_EVENT event)//-1:nessuna doppia arma,0:non ha finito,1:doppia arma presente
{
	if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento.size() > 1)
	{
		if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[0].nome !=
			stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[1].nome)
		{
			stato.indicatore.stato_azione_arma_grafica = 0;
			return -1;
		}
	}
	else
	{
		stato.indicatore.stato_azione_arma_grafica = 0;
		return -1;
	}
	stato.indicatore.stato_azione_arma_grafica = 4;
	stato.indicatore.scelte_messaggio_generico.clear();
	stato.indicatore.messaggio_generico.clear();
	stato.indicatore.dadi.clear();
	stato.indicatore.scelte_messaggio_generico.push_back("Doppia");
	stato.indicatore.scelte_messaggio_generico.push_back("Singola");
	stato.Cursori["Azione attacca, scelta generica"]["Lista"];
	input_tastiera_9_direzioni(stato.Cursori["Azione attacca, scelta generica"]["Lista"].iteratore, event, {2,0,0,0,0,0,0,0,0 }, true, false, false);
	if (event.type == ALLEGRO_EVENT_KEY_DOWN)
	{
		if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
		{
			if (stato.Cursori["Azione attacca, scelta generica"]["Lista"].iteratore == 0)
			{
				stato.indicatore.doppia = true;
				return 1;
			}
			else
			{
				stato.indicatore.doppia = false;
				return -1;
			}	
		}
	}
	return 0;
}
int Attacca_molotov(StatoGioco& stato)
{
	stato.indicatore.stato_azione_arma_grafica = 0;
	stato.missione.dati[stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno]-1].posizione].SommaValoreRumore(1);
	stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].esperienza =
		stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].esperienza +
		stato.missione.dati[stato.indicatore.stanza_selezionata].walker +
		stato.missione.dati[stato.indicatore.stanza_selezionata].runner +
		stato.missione.dati[stato.indicatore.stanza_selezionata].fat;
	if (stato.missione.dati[stato.indicatore.stanza_selezionata].abominio == true)
		stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].esperienza =
		stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].esperienza + 5;
	stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].redraw_scheda = true;
	stato.missione.dati[stato.indicatore.stanza_selezionata].ModificaValoreAbominio(false);
	stato.missione.dati[stato.indicatore.stanza_selezionata].SommaValoreWalker(-stato.missione.dati[stato.indicatore.stanza_selezionata].walker);
	stato.missione.dati[stato.indicatore.stanza_selezionata].SommaValoreRunner(-stato.missione.dati[stato.indicatore.stanza_selezionata].runner);
	stato.missione.dati[stato.indicatore.stanza_selezionata].SommaValoreFat(-stato.missione.dati[stato.indicatore.stanza_selezionata].fat);
	int indice_giocatore_di_turno = 0;
	for (size_t i = 0; i < stato.ordine_giocatori.size(); i++)
		if (stato.ordine_giocatori[i] == stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1)
			indice_giocatore_di_turno = i;
	for (int i = 0; i < stato.numero_giocatori; i++)
	{
		if (stato.giocatore[stato.ordine_giocatori[i] - 1].posizione == stato.indicatore.stanza_selezionata)
		{
			uccidi_giocatore(stato, stato.ordine_giocatori[i] - 1);
			i--;
		}
	}
	if (stato.indicatore.giocatore_di_turno >= 0) //se è -1 vuol dire che chi ha usato la molotov si è suicidato.
											   //poi con il prox turno torna a 0, ma per ora evitiamo di controllare un array in posizione -1
	{
		for (int i = 0; i < (int)stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento.size(); i++)
			if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[i].nome == "Molotov")
				stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento.erase(
					stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento.begin() + i);
	}
	return 1;
}
int Attacca_ricarica(StatoGioco& stato, ALLEGRO_EVENT event,string melee_o_ranged)
{
	if (stato.indicatore.doppia == true) //NON sono scariche nessuna delle due
	{
		if ((melee_o_ranged == "melee"&& stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[0].scarica_melee==false &&
			stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[1].scarica_melee==false) ||
			(melee_o_ranged == "ranged"&&stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[0].scarica_ranged==false&&
				stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[1].scarica_ranged==false))
			return 1;
	}//non è scarica l'arma singola
	if((melee_o_ranged=="melee" && stato.indicatore.arma_selezionata.scarica_melee == false)|| (melee_o_ranged == "ranged" && stato.indicatore.arma_selezionata.scarica_ranged == false))
			return 1;
	bool due_armi_uguali = false;
	if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento.size() > 1)
		if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[0].nome ==
			stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[1].nome&&
			stato.indicatore.arma_selezionata.doppia == true)
			due_armi_uguali = true;
		
	stato.indicatore.stato_azione_arma_grafica = 4;
	stato.indicatore.scelte_messaggio_generico.clear();
	stato.indicatore.dadi.clear();
	stato.indicatore.scelte_messaggio_generico.push_back("Ricarica (1 Azione)");
	stato.indicatore.scelte_messaggio_generico.push_back("Non ricaricare");
	stato.Cursori["Azione attacca, scelta generica"]["Lista"];
	input_tastiera_9_direzioni(stato.Cursori["Azione attacca, scelta generica"]["Lista"].iteratore, event, {2,0,0,0,0,0,0,0,0}, true, false, false);
	if (event.type == ALLEGRO_EVENT_KEY_DOWN&&event.keyboard.keycode == ALLEGRO_KEY_ENTER)
	{
		if (stato.Cursori["Azione attacca, scelta generica"]["Lista"].iteratore == 0)
		{
			if (due_armi_uguali == true)
			{
				if (melee_o_ranged == "melee")
				{
					stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[0].scarica_melee = false;
					stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[1].scarica_melee = false;
				}
				else if (melee_o_ranged == "ranged")
				{
					stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[0].scarica_ranged = false;
					stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[1].scarica_ranged = false;
				}
			}
			else
			{
				if (melee_o_ranged == "melee")
				{
					stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].
						equipaggiamento[stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore].scarica_melee = true;
					stato.indicatore.arma_selezionata.scarica_melee = false;
				}
				else if (melee_o_ranged == "ranged")
				{
					stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].
						equipaggiamento[stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore].scarica_ranged = true;
					stato.indicatore.arma_selezionata.scarica_ranged = false;
				}
			}
			stato.indicatore.stato_azione_arma_grafica = 2;
			stato.numero_azioni--;
			if (stato.numero_azioni == 0)
			{
				stato.indicatore.stato_azione_attacca = 0;
				stato.indicatore.stato_azione_arma = 0;
				return 0;
			}
			return 1;
		}
		else
		{
			stato.indicatore.stato_azione_arma_grafica = 0;
			stato.indicatore.stato_azione_attacca = 0;
			stato.indicatore.stato_azione_arma = 0;
			return -1;
		}
	}
	return 0;
}

//funzioni "private"
int arma_melee_ranged_o_doppia(Carta_equipaggiamento arma)
{
	if (arma.danno_ranged != 0&& arma.danno_melee==0) //arma ranged
		return 1;
	if (arma.danno_melee != 0&&arma.danno_ranged==0) //arma melee
		return 0;
	if (arma.danno_melee != 0 && arma.danno_ranged != 0) //arma sia ranged che melee (gunblade)
		return 2;
	return 0; //solo perchè devo
}
bool cerca_zombie(StatoGioco& stato, vector<int>& stanze_di_interesse, int stanza, int verso,
	int peso, int raggio_min, int raggio_max, int contatore_distanza_raggiunta)
{
	for (size_t i = 0; i < stato.missione.dati.size(); i++)
	{
		if (stato.missione.collisioni[stanza][i] == 1 && stato.missione.orientamento_connessioni[stanza][i] == verso && peso + stato.missione.dati[i].tipo < 2)
		{
			if (raggio_min > contatore_distanza_raggiunta)
				cerca_zombie(stato, stanze_di_interesse, i, verso, peso + stato.missione.dati[i].tipo, raggio_min, raggio_max, contatore_distanza_raggiunta + 1);
			if (raggio_max < contatore_distanza_raggiunta)
			{
				if (stanze_di_interesse.size()>0)
					return true;
				else
					return false;
			}
			if (stato.missione.dati[i].walker >0 || stato.missione.dati[i].runner>0 || stato.missione.dati[i].fat > 0 || stato.missione.dati[i].abominio == true)
				stanze_di_interesse.push_back(i);
			else
			{
				for (int j = 0; j < stato.numero_giocatori; j++)
					if (stato.missione.dati[i].giocatori[stato.ordine_giocatori[j] - 1] == true)
					{
						stanze_di_interesse.push_back(i);
						break;
					}
			}
			cerca_zombie(stato, stanze_di_interesse, i, verso, peso + stato.missione.dati[i].tipo, raggio_min, raggio_max, contatore_distanza_raggiunta + 1);
		}
	}
	if (stanze_di_interesse.size()>0) //per quando il raggio max è esce fuori la matrice e quindi il contatore non
		return true;				// può controllare all'interno dell'if quando concludere la funzione. Male che va
	else
	{
		return false;				//finisce il ciclo e arriva qui
	}

}
bool fattibilità_attacco_arma_selezionata(StatoGioco& stato, int posizione, vector<int>& stanze_di_interesse, int gittata_min, int gittata_max)
{
	if (gittata_min == 0)
	{
		if (stato.missione.dati[posizione].walker > 0 || stato.missione.dati[posizione].runner>0 || //vanno bene sia zombie
			stato.missione.dati[posizione].fat>0 || stato.missione.dati[posizione].abominio == true)
		{
			if (find(stanze_di_interesse.begin(), stanze_di_interesse.end(), posizione) == stanze_di_interesse.end())
				stanze_di_interesse.push_back(posizione);
		}
		else
			for (int j = 0; j < stato.numero_giocatori; j++) //che giocatori
				if (stato.missione.dati[posizione].giocatori[stato.ordine_giocatori[j] - 1] == true &&
					stato.ordine_giocatori[j] - 1 != stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1)
				{
					if (find(stanze_di_interesse.begin(), stanze_di_interesse.end(), posizione) == stanze_di_interesse.end())
						stanze_di_interesse.push_back(posizione);
				}
	}
	if (gittata_max)
		for (int j = 0; j < 8; j++) //ora controllo in tutte le direzioni
			cerca_zombie(stato, stanze_di_interesse, posizione, j, 0, gittata_min, gittata_max, 1);
	if (stanze_di_interesse.size() > 0)
		return true;
	else
		return false;
}
int obiettivo_ranged(StatoGioco& stato, int zona) //personaggi: 0,1,2,3,4,5 -- 6-walker, 7-runner, 8-fat, 9-abominio, 10-scelta tra abominio e fat
{
	for (int i = 0; i < stato.numero_giocatori; i++)
		if (stato.missione.dati[zona].giocatori[stato.ordine_giocatori[i] - 1] == true &&
			stato.ordine_giocatori[i] - 1 != stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1)
			return stato.ordine_giocatori[i] - 1;
	if (stato.missione.dati[zona].walker>0)
		return 6;
	else if (stato.missione.dati[zona].abominio == true && stato.missione.dati[zona].fat > 0)
		return 10;
	else if (stato.missione.dati[zona].abominio == true)
		return 9;
	else if (stato.missione.dati[zona].fat > 0)
		return 8;
	else if (stato.missione.dati[zona].runner > 0)
		return 7;
	else
		return -1;
}
void selezione_obiettivi_in_zona(StatoGioco& stato,ALLEGRO_EVENT event, int zona)
{
	stato.indicatore.zombie_o_giocatori_selezionabili_melee.clear();
	for (int i = 0; i < stato.numero_giocatori; i++)
	{
		if (stato.missione.dati[zona].giocatori[stato.ordine_giocatori[i] - 1] == true &&
			stato.ordine_giocatori[i] - 1 != stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1)
			stato.indicatore.zombie_o_giocatori_selezionabili_melee.push_back(stato.ordine_giocatori[i] - 1);
	}
	sort(stato.indicatore.zombie_o_giocatori_selezionabili_melee.begin(), stato.indicatore.zombie_o_giocatori_selezionabili_melee.end());
	if (stato.missione.dati[zona].abominio == true && stato.indicatore.arma_selezionata.danno_melee>2)
		stato.indicatore.zombie_o_giocatori_selezionabili_melee.insert(stato.indicatore.zombie_o_giocatori_selezionabili_melee.begin(), 9);
	if (stato.missione.dati[zona].fat>0 && stato.indicatore.arma_selezionata.danno_melee>1)
		stato.indicatore.zombie_o_giocatori_selezionabili_melee.insert(stato.indicatore.zombie_o_giocatori_selezionabili_melee.begin(), 8);
	if (stato.missione.dati[zona].runner>0)
		stato.indicatore.zombie_o_giocatori_selezionabili_melee.insert(stato.indicatore.zombie_o_giocatori_selezionabili_melee.begin(), 7);
	if (stato.missione.dati[zona].walker>0)
		stato.indicatore.zombie_o_giocatori_selezionabili_melee.insert(stato.indicatore.zombie_o_giocatori_selezionabili_melee.begin(), 6);
	int r1 = 0; int r2 = 0;
	for (int i = 0; i < (int)stato.indicatore.zombie_o_giocatori_selezionabili_melee.size(); i++)
		if (stato.indicatore.zombie_o_giocatori_selezionabili_melee[i]>5)
			r1++;
		else if (stato.indicatore.zombie_o_giocatori_selezionabili_melee[i] < 6)
			r2++;
	if (stato.indicatore.zombie_o_giocatori_selezionabili_melee.size() > 0)
	{
		stato.Cursori["Seleziona obiettivi in zona"]["Lista"];
		input_tastiera_9_direzioni(stato.Cursori["Seleziona obiettivi in zona"]["Lista"].iteratore, event, { r1,0,0,0,0,0,0,0,r2 }, false, false, false);
	}
}

