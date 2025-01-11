#include"Abilità.h"
#include"utilità.h"
#include "azioni.h"
#include<string>
#include<vector>

//return 1-> finito ; return 0 -> non finito

//Public
int starts_with_a_pistol(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione)
{
	switch (codice_sottoFunzione)
	{
	case Abilità::CODICE_ESEGUI:
		return (starts_with_a_weapon(stato, "Pistol", stato.giocatore[giocatore_che_la_utilizza].abilità[indice_abilità], giocatore_che_la_utilizza));
		break;
	case Abilità::CODICE_RICARICA:
		break;
	case Abilità::CODICE_SCARICA:
		break;
	}
	return 0;
}
int one_die_melee(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione)
{
	switch (codice_sottoFunzione)
	{
	case Abilità::CODICE_ESEGUI:
		if (stato.indicatore.tipo_arma_selezionata == 0)
		{
			for (int i = 0; i < stato.indicatore.moltiplicatore_dadi; i++)
				stato.indicatore.dadi.push_back(rand() % 6 + 1);
			stato.indicatore.abilità_dadi_usate.push_back("+1 die: Melee");
			stato.giocatore[giocatore_che_la_utilizza].abilità[indice_abilità].scarica = true;
		}
		return 1;
		break;
	case Abilità::CODICE_RICARICA:
		break;
	case Abilità::CODICE_SCARICA:
		break;
	}
	return 0;
}
int one_die_ranged(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione)
{
	switch (codice_sottoFunzione)
	{
	case Abilità::CODICE_ESEGUI:
		if (stato.indicatore.tipo_arma_selezionata == 1)
		{
			for (int i = 0; i < stato.indicatore.moltiplicatore_dadi; i++)
				stato.indicatore.dadi.push_back(rand() % 6 + 1);
			stato.indicatore.abilità_dadi_usate.push_back("+1 die: Ranged");
			stato.giocatore[giocatore_che_la_utilizza].abilità[indice_abilità].scarica = true;
		}
		break;
	case Abilità::CODICE_RICARICA:
		break;
	case Abilità::CODICE_SCARICA:
		break;
	}
	return 0;
}
int one_die_combact(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione)
{
	switch (codice_sottoFunzione)
	{
	case Abilità::CODICE_ESEGUI:
		for (int i = 0; i < stato.indicatore.moltiplicatore_dadi; i++)
			stato.indicatore.dadi.push_back(rand() % 6 + 1);
		stato.indicatore.abilità_dadi_usate.push_back("+1 die: Combact");
		stato.giocatore[giocatore_che_la_utilizza].abilità[indice_abilità].scarica = true;
		break;
	case Abilità::CODICE_RICARICA:
		break;
	case Abilità::CODICE_SCARICA:
		break;
	}
	return 0;
}
int one_to_dice_roll_ranged(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione)
{
	switch (codice_sottoFunzione)
	{
	case Abilità::CODICE_ESEGUI:
		if (stato.indicatore.tipo_arma_selezionata == 1)
		{
			for (size_t i = 0; i < stato.indicatore.dadi.size(); i++)
				if (stato.indicatore.dadi[i] != 6)
					stato.indicatore.dadi[i]++;
			stato.indicatore.abilità_dadi_usate.push_back("+1 to dice roll: Ranged");
			stato.giocatore[giocatore_che_la_utilizza].abilità[indice_abilità].scarica = true;
		}	
		break;
	case Abilità::CODICE_RICARICA:
		break;
	case Abilità::CODICE_SCARICA:
		break;
	}
	return 0;
}
int one_to_dice_roll_melee(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione)
{
	switch (codice_sottoFunzione)
	{
	case Abilità::CODICE_ESEGUI:
		if (stato.indicatore.tipo_arma_selezionata == 0)
		{
			for (size_t i = 0; i < stato.indicatore.dadi.size(); i++)
				if (stato.indicatore.dadi[i] != 6)
					stato.indicatore.dadi[i]++;
			stato.indicatore.abilità_dadi_usate.push_back("+1 to dice roll: Melee");
			stato.giocatore[giocatore_che_la_utilizza].abilità[indice_abilità].scarica = true;
		}	
		break;
	case Abilità::CODICE_RICARICA:
		break;
	case Abilità::CODICE_SCARICA:
		break;
	}
	return 0;
}
int one_to_dice_roll_combact(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione)
{
	switch (codice_sottoFunzione)
	{
	case Abilità::CODICE_ESEGUI:
		for (size_t i = 0; i < stato.indicatore.dadi.size(); i++)
			if (stato.indicatore.dadi[i] != 6)
				stato.indicatore.dadi[i]++;
		stato.indicatore.abilità_dadi_usate.push_back("+1 to dice roll: Combact");
		stato.giocatore[giocatore_che_la_utilizza].abilità[indice_abilità].scarica = true;
		break;
	case Abilità::CODICE_RICARICA:
		break;
	case Abilità::CODICE_SCARICA:
		break;
	}
	return 0;
}
int one_free_move_action(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione)
{
	return free_action(stato, giocatore_che_la_utilizza, indice_abilità, codice_sottoFunzione, "Muovi");
} 
int one_free_search_action(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione)
{
	return free_action(stato, giocatore_che_la_utilizza, indice_abilità, codice_sottoFunzione, "Cerca");
}
int one_free_combact_action(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione)
{
	return free_action(stato, giocatore_che_la_utilizza, indice_abilità, codice_sottoFunzione, "Attacca");
}
int one_re_roll_per_turn(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione)
{
	switch (codice_sottoFunzione)
	{
	case Abilità::CODICE_ESEGUI:
		if (stato.giocatore[giocatore_che_la_utilizza].abilità[indice_abilità].scarica == false)
			stato.indicatore.Lista_re_roll.push_back("1 Re-roll per turn");
		break;
	case Abilità::CODICE_RICARICA:
		stato.giocatore[giocatore_che_la_utilizza].abilità[indice_abilità].scarica = false;
		break;
	case Abilità::CODICE_SCARICA:
		for (size_t j = 0; j < stato.indicatore.Lista_re_roll.size(); j++)
			if (stato.indicatore.Lista_re_roll[j] == "1 Re-roll per turn")
				return 1;
		stato.giocatore[giocatore_che_la_utilizza].abilità[indice_abilità].scarica = true;
	}
	return 0;
}
int lucky(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione)
{
	switch (codice_sottoFunzione)
	{
	case Abilità::CODICE_ESEGUI:
		if (stato.giocatore[giocatore_che_la_utilizza].abilità[indice_abilità].scarica == false)
			stato.indicatore.Lista_re_roll.push_back("Lucky");
		break;
	case Abilità::CODICE_RICARICA:
		break;
	case Abilità::CODICE_SCARICA:
		break;
	}
	return 0;
}
int one_action(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione)
{
	switch (codice_sottoFunzione)
	{
	case Abilità::CODICE_ESEGUI:
		break;
	case Abilità::CODICE_RICARICA:
		break;
	case Abilità::CODICE_SCARICA:
		break;
	}
	return 0;
}
int sniper(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione)
{
	switch (codice_sottoFunzione)
	{
	case Abilità::CODICE_ESEGUI:
		stato.indicatore.tipo_arma_selezionata = 0; //melee
		stato.indicatore.arma_selezionata.dadi_melee = stato.indicatore.arma_selezionata.dadi_ranged;
		stato.indicatore.arma_selezionata.gittata_min_melee = stato.indicatore.arma_selezionata.gittata_min_ranged;
		stato.indicatore.arma_selezionata.gittata_max_melee = stato.indicatore.arma_selezionata.gittata_max_ranged;
		stato.indicatore.arma_selezionata.dadi_melee = stato.indicatore.arma_selezionata.dadi_ranged;
		stato.indicatore.arma_selezionata.danno_melee = stato.indicatore.arma_selezionata.danno_ranged;
		stato.indicatore.arma_selezionata.danno_ranged = 0; //altrimenti può sembrare un gunblade
		stato.indicatore.arma_selezionata.precisione_melee = stato.indicatore.arma_selezionata.precisione_ranged;
		stato.indicatore.arma_selezionata.rumore_dopo_attacco_melee = stato.indicatore.arma_selezionata.rumore_dopo_attacco_ranged;
		stato.indicatore.arma_selezionata.scarica_melee = stato.indicatore.arma_selezionata.scarica_ranged;
		break;
	case Abilità::CODICE_RICARICA:
		break;
	case Abilità::CODICE_SCARICA:
		break;
	}
	return 0;
}
int ambidexytous(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione)
{
	switch (codice_sottoFunzione)
	{
	case Abilità::CODICE_ESEGUI:
		break;
	case Abilità::CODICE_RICARICA:
		break;
	case Abilità::CODICE_SCARICA:
		break;
	}
	return 0;
}
int tough(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione)
{
	switch (codice_sottoFunzione)
	{
	case Abilità::CODICE_ESEGUI:
		if (stato.giocatore[giocatore_che_la_utilizza].abilità[indice_abilità].scarica == false)
		{
			stato.indicatore.ferite--;
			stato.giocatore[giocatore_che_la_utilizza].abilità[indice_abilità].scarica = true;
			return 1;
		}
		return -1;
	case Abilità::CODICE_RICARICA:
		stato.giocatore[giocatore_che_la_utilizza].abilità[indice_abilità].scarica = false;
		break;
	case Abilità::CODICE_SCARICA:
		stato.giocatore[giocatore_che_la_utilizza].abilità[indice_abilità].scarica = true;
		break;
	}
	return 0;
}
int one_zone_per_move(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione)
{
	switch (codice_sottoFunzione)
	{
	case Abilità::CODICE_ESEGUI:
		stato.indicatore.numero_spostamenti_con_un_movimento++;
		break;
	case Abilità::CODICE_RICARICA:
		break;
	case Abilità::CODICE_SCARICA:
		break;
	}
	return 0;
}
int slippery(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione)
{
	switch (codice_sottoFunzione)
	{
	case Abilità::CODICE_ESEGUI:
		stato.indicatore.considerare_zombie_per_zone_disponibili_a_spostamento = false;
		break;
	case Abilità::CODICE_RICARICA:
		break;
	case Abilità::CODICE_SCARICA:
		break;
	}
	return 0;
}
int matching_set(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione)
{
	int numero_ogg_trovati;
	Carta_equipaggiamento c;
	bool verifica_ambidestro = false;
	switch (codice_sottoFunzione)
	{
	case Abilità::CODICE_ESEGUI:
		for (size_t i = 0; i < stato.giocatore[giocatore_che_la_utilizza].abilità.size(); i++)
			if (stato.giocatore[giocatore_che_la_utilizza].abilità[i].nome == "ambidexytous")
				verifica_ambidestro = true;
		numero_ogg_trovati = stato.giocatore[6].equipaggiamento.size();
		for (int i = 0; i < numero_ogg_trovati; i++)
		{
			if ((stato.giocatore[6].equipaggiamento[i].doppia == true || verifica_ambidestro == true) && stato.giocatore[6].equipaggiamento[i].tipologia == Carta_equipaggiamento::ARMA)
			{
				if (cerca_carta_in_un_mazzo(c, stato.giocatore[6].equipaggiamento[i].nome, stato.Mazzo_equipaggiamento, true) == true)
				{
					stato.giocatore[6].equipaggiamento.push_back(c);
					if (stato.Mazzo_equipaggiamento.size() == 0)
					{
						stato.Mazzo_equipaggiamento = stato.Mazzo_equipaggiamento_scarti;
						stato.Mazzo_equipaggiamento_scarti.clear();
					}
				}
			}
		}
		break;
	case Abilità::CODICE_RICARICA:
		break;
	case Abilità::CODICE_SCARICA:
		break;
	}
	return 0;
}
int medic(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione)
{
	return special_action(stato, giocatore_che_la_utilizza, indice_abilità, codice_sottoFunzione, "Medic (Gratis)");
}

//da fare...
int born_leader(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione)
{
	return special_action(stato, giocatore_che_la_utilizza, indice_abilità, codice_sottoFunzione, "Born Leader (Gratis)");
}
//Private
int starts_with_a_weapon(StatoGioco& stato, string weapon,Abilità a, int giocatore_che_la_utilizza)
{
	Carta_equipaggiamento C_weapon;
	cerca_carta_in_un_mazzo(C_weapon, weapon, stato.Mazzo_equipaggiamento, true);
	stato.giocatore[giocatore_che_la_utilizza].equipaggiamento.push_back(C_weapon);
	a.quando_ricarica = true;
	return 1; //finito
}
int free_action(StatoGioco& stato,int giocatore_che_la_utilizza,int indice_abilità, int codice_sottoFunzione,string nome_azione)
{
	switch (codice_sottoFunzione)
	{
	case Abilità::CODICE_ESEGUI:
		if (stato.giocatore[giocatore_che_la_utilizza].abilità[indice_abilità].scarica == false)
		{
			for (size_t i = 0; i < stato.elenco_azioni.size(); i++)
			{
				if (stato.elenco_azioni[i].nome == nome_azione)
				{
					stato.elenco_azioni[i].costo_di_azioni = 0;
					stato.elenco_azioni[i].nome = nome_azione+" (Gratis)";
				}
			}
		}
		return 1;
		break;
	case Abilità::CODICE_RICARICA:
		stato.giocatore[giocatore_che_la_utilizza].abilità[indice_abilità].scarica = false;
		return 1;
		break;
	case Abilità::CODICE_SCARICA:
		bool scarica = false;
		if (stato.giocatore[giocatore_che_la_utilizza].stato == 0) //se è morto, scarica le free
			scarica = true;
		if (stato.azioni_disponibili.size()>0) 
		{
			//se l'azione usata è quella gratuita o Termina, mentre aveva altre azioni possibili
			if (stato.azioni_disponibili[stato.Cursori["Turno giocatore"]["Scelta azioni"].iteratore].nome == nome_azione+" (Gratis)" ||
				stato.azioni_disponibili[stato.Cursori["Turno giocatore"]["Scelta azioni"].iteratore].nome == "Termina")
				scarica = true;
		}
		else //ha usato Termina e non aveva altre azioni, altrimenti avrebbe almeno 1 azione disponibile (Termina)
			scarica = true;
		
		if (scarica == true)
		{
			stato.giocatore[giocatore_che_la_utilizza].abilità[indice_abilità].scarica = true;
			for (size_t i = 0; i < stato.elenco_azioni.size(); i++)
				if (stato.elenco_azioni[i].nome == nome_azione + " (Gratis)")
				{
					stato.elenco_azioni[i].costo_di_azioni = 1;
					stato.elenco_azioni[i].nome = nome_azione;
				}
		}
		return 1;
		break;
	}
	return 0;
}
int special_action(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione, string nome_azione)
{
	switch (codice_sottoFunzione)
	{
	case Abilità::CODICE_ESEGUI:
		break;
	case Abilità::CODICE_RICARICA:
		stato.giocatore[giocatore_che_la_utilizza].abilità[indice_abilità].scarica = false;
		break;
	case Abilità::CODICE_SCARICA:
		if (stato.azioni_disponibili.size()>0)
		{
			if (stato.azioni_disponibili[stato.Cursori["Turno giocatore"]["Scelta azioni"].iteratore].nome == nome_azione ||
				stato.azioni_disponibili[stato.Cursori["Turno giocatore"]["Scelta azioni"].iteratore].nome == "Termina")
				stato.giocatore[giocatore_che_la_utilizza].abilità[indice_abilità].scarica = true;
		}
		else
			stato.giocatore[giocatore_che_la_utilizza].abilità[indice_abilità].scarica = true;
		return 1;
	}
	return 0;
}
int born_leader_mark(StatoGioco& stato, int giocatore_che_la_utilizza, int indice_abilità, int codice_sottoFunzione)
{
	if (codice_sottoFunzione == Abilità::CODICE_SCARICA)
	{
		if (stato.azioni_disponibili.size()>0)
		{
			if (stato.azioni_disponibili[stato.Cursori["Turno giocatore"]["Scelta azioni"].iteratore].nome == stato.indicatore.azione_gratuita_assegnata_dal_born_leader + " (Gratis)" ||
				stato.azioni_disponibili[stato.Cursori["Turno giocatore"]["Scelta azioni"].iteratore].nome == "Termina")
			{
				for (size_t i = 0; i < stato.elenco_azioni.size(); i++)
					if (stato.elenco_azioni[i].nome == stato.indicatore.azione_gratuita_assegnata_dal_born_leader + " (Gratis)")
					{
						stato.elenco_azioni[i].costo_di_azioni = 1;
						stato.elenco_azioni[i].nome = stato.indicatore.azione_gratuita_assegnata_dal_born_leader;
						return 1;
					}
				stato.giocatore[giocatore_che_la_utilizza].abilità[indice_abilità].scarica = true;
				stato.giocatore[giocatore_che_la_utilizza].abilità.erase
					(stato.giocatore[giocatore_che_la_utilizza].abilità.begin() + indice_abilità);
			}
		}
		else
		{
			for (size_t i = 0; i < stato.elenco_azioni.size(); i++)
				if (stato.elenco_azioni[i].nome == stato.indicatore.azione_gratuita_assegnata_dal_born_leader + " (Gratis)")
				{
					stato.elenco_azioni[i].costo_di_azioni = 1;
					stato.elenco_azioni[i].nome = stato.indicatore.azione_gratuita_assegnata_dal_born_leader;
					
					return 1;
				}
			stato.giocatore[giocatore_che_la_utilizza].abilità[indice_abilità].scarica = true;
			stato.giocatore[giocatore_che_la_utilizza].abilità.erase
				(stato.giocatore[giocatore_che_la_utilizza].abilità.begin() + indice_abilità);
		}
		return 1;
	}
	return free_action(stato, giocatore_che_la_utilizza, indice_abilità, codice_sottoFunzione, stato.indicatore.azione_gratuita_assegnata_dal_born_leader);
}