/*D�but du script pour la ferme dans micoline.
  Autor : ProjectCoreDevs
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "World.h"
#include "PetAI.h"
#include "PassiveAI.h"
#include "CombatAI.h"
#include "GameEventMgr.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Cell.h"
#include "CellImpl.h"
#include "SpellAuras.h"
#include "Vehicle.h"
#include "Player.h"

enum quest
{
	QUEST_GROWING_THE_FARM_1               = 30516, // A Little Problem (phase 1)
	QUEST_GROWING_THE_FARM_2               = 30524, // Knock on Wood (phase 2)
	QUEST_GROWING_THE_FARM_3               = 30529, // The Mossy Boulder (phase 3)
	QUEST_HAOHANS_VOTE_5                   = 30528, // Chief Yip-Yip (phase 4)
};

enum spell
{
	SPELL_TILL_SOIL                        = 115344, // Labourer le sol
	SPELL_HARVEST_WHITE_TURNIP             = 123570, // r�colte d'un Navet blanc
	SPELL_HARVEST_PLUMP_WHITE_TURNIP       = 123571, // r�colte d'un Navet blanc charnu
	SPELL_HARVEST_RED_BLOSSOM_LEEK         = 123524, // r�colte de Poireau � fleur rouge
	SPELL_HARVEST_PLUMP_RED_BLOSSOM_LEEK   = 123522, // r�colte de Poireau � fleur rouge charnu
	SPELL_HARVEST_WITCHBERRIES             = 129983, // r�colte de Sorcielles
	SPELL_HARVEST_PLUMP_WITCHBERRIES       = 129984, // r�colte de Sorcielles charnu
	SPELL_HARVEST_STRIPED_MELON            = 130042, // r�colte de Melon � rayures
	SPELL_HARVEST_PLUMP_STRIPED_MELON      = 130043, // r�colte de Melon � rayures charnu
	SPELL_HARVEST_MOGU_PUMPKIN             = 123445, // r�colte de Citrouille mogu
	SPELL_HARVEST_PLUMP_MOGU_PUMPKIN       = 123451, // r�colte de Citrouille mogu charnu
	SPELL_HARVEST_SCALLIONS                = 123375, // r�colte d'Echalotes
	SPELL_HARVEST_PLUMP_SCALLIONS          = 123380, // r�colte d'Echalotes charnu
	SPELL_HARVEST_JADE_SQUASH              = 130025, // r�colte de Courge de jade
	SPELL_HARVEST_PLUMP_JADE_SQUASH        = 130026, // r�colte de Courge de jade charnu
	SPELL_HARVEST_JUICYCRUNCH_CARROT       = 123353, // r�colte de Carotte croquejuteuse
	SPELL_HARVEST_PLUMP_JUICYCRUNCH_CARROT = 123356, // r�colte de Carotte croquejuteuse charnu
	SPELL_HARVEST_PINK_TURNIP              = 123548, // r�colte de Navet rose
	SPELL_HARVEST_PLUMP_PINK_TURNIP        = 123549, // r�colte de Navet rose charnu
	SPELL_HARVEST_GREEN_CABBAGE            = 111123, // r�colte de Choux vert
	SPELL_HARVEST_PLUMP_GREEN_CABBAGE      = 123355, // r�colte de Choux vert charnu
};

enum npc
{
	// attaquant
	NPC_SWOOPING_PLAINSHAWK   = 60072, // Faucon des plaines plongeur
	NPC_VORACIOUS_VIRMEN      = 60039, // virmen vorace

	// terres
	NPC_UNTILLED_SOIL         = 58562, // terre non labour�e
	NPC_TILLED_SOIL           = 58563, // terre labour�e

	// Mauvaises-herbes
	NPC_STUBBORN_WEED         = 60153, // herbe tenace
	NPC_ENCROACHING_WEED      = 60185, // herbe envahissante

    /*#########################L�gumes#########################*/

	// Navet blanc
	NPC_ALLURING_WHITE_TURNIP           = 63259, // Navet blanc app�tissant
	NPC_PLUMP_WHITE_TURNIP              = 63264, // Navet blanc charnu
	NPC_PARCHED_WHITE_TURNIP            = 63263, // Navet blanc dess�ch�
	NPC_GROWING_WHITE_TURNIP            = 63261, // Navet blanc en pleine croissance
	NPC_TANGLED_WHITE_TURNIP            = 63269, // Navet blanc �nchev�tr�
	NPC_SMOTHERED_WHITE_TURNIP          = 63268, // Navet blanc �touff�
	NPC_WIGGLING_WHITE_TURNIP           = 63270, // Navet blanc fr�tillant
	NPC_INFESTED_WHITE_TURNIP           = 63262, // Navet blanc infest�
	NPC_RIPE_WHITE_TURNIP               = 63265, // Navet blanc m�re
	NPC_BURSTING_WHITE_TURNIP           = 63260, // Navet blanc plein
	NPC_RUNTY_WHITE_TURNIP              = 63267, // Navet blanc rabougri
	NPC_WILD_WHITE_TURNIP               = 63271, // Navet blanc sauvage

	// Poireau � fleur rouge
	NPC_ALLURING_RED_BLOSSOM_LEEK       = 63222, // Poireau � fleur rouge app�tissant
	NPC_PLUMP_RED_BLOSSOM_LEEK          = 63228, // Poireau � fleur rouge charnu
	NPC_PARCHED_RED_BLOSSOM_LEEK        = 63227, // Poireau � fleur rouge dessech�
	NPC_GROWING_RED_BLOSSOM_LEEK        = 63224, // Poireau � fleur rouge en pleine croissance
	NPC_TANGLED_RED_BLOSSOM_LEEK        = 63232, // Poireau � fleur rouge �nchev�tr�
	NPC_SMOTHERED_RED_BLOSSOM_LEEK      = 63231, // Poireau � fleur rouge �touff�
	NPC_WIGGLING_RED_BLOSSOM_LEEK       = 63233, // Poireau � fleur rouge fr�tillantes
	NPC_INFESTED_RED_BLOSSOM_LEEK       = 63226, // Poireau � fleur rouge infest�
	NPC_RIPE_RED_BLOSSOM_LEEK           = 63229, // Poireau � fleur rouge m�re
	NPC_BURSTING_RED_BLOSSOM_LEEK       = 63223, // Poireau � fleur rouge plein
	NPC_RUNTY_RED_BLOSSOM_LEEK          = 63230, // Poireau � fleur rouge rabougri
	NPC_WILD_RED_BLOSSOM_LEEK           = 63234, // Poireau � fleur rouge sauvage
	
	// Sorcielles
	NPC_ALLURING_WITCHBERRIES           = 66079, // Sorcielles app�tissant
	NPC_PLUMP_WITCHBERRIES              = 66084, // Sorcielles charnu
	NPC_PARCHED_WITCHBERRIES            = 66083, // Sorcielles dessech�es
	NPC_GROWING_WITCHBERRIES            = 66081, // Sorcielles en pleine croissance
	NPC_TANGLED_WITCHBERRIES            = 66088, // Sorcielles �nchev�tr�
	NPC_SMOTHERED_WITCHBERRIES          = 66087, // Sorcielles �touff�
	NPC_WIGGLING_WITCHBERRIES           = 66089, // Sorcielles fr�tillantes
	NPC_INFESTED_WITCHBERRIES           = 66082, // Sorcielles infest�
	NPC_RIPE_WITCHBERRIES               = 66085, // Sorcielles m�re
	NPC_BURSTING_WITCHBERRIES           = 66080, // Sorcielles plein
	NPC_RUNTY_WITCHBERRIES              = 66086, // Sorcielles rabougri
	NPC_WILD_WITCHBERRIES               = 66090, // Sorcielles sauvage
	
	// Melon � rayures
	NPC_ALLURING_STRIPED_MELON          = 66122, // Melon � rayure app�tissant
	NPC_PLUMP_STRIPED_MELON             = 66128, // Melon � rayure charnu
	NPC_PARCHED_STRIPED_MELON           = 66083, // Melon � rayure dessech�es
	NPC_GROWING_STRIPED_MELON           = 66124, // Melon � rayure en pleine croissance
	NPC_TANGLED_STRIPED_MELON           = 66132, // Melon � rayure �nchev�tr�
	NPC_SMOTHERED_STRIPED_MELON         = 66131, // Melon � rayure �touff�
	NPC_WIGGLING_STRIPED_MELON          = 66133, // Melon � rayure fr�tillantes
	NPC_INFESTED_STRIPED_MELON          = 66125, // Melon � rayure infest�
	NPC_RIPE_STRIPED_MELON              = 66129, // Melon � rayure m�re
	NPC_BURSTING_STRIPED_MELON          = 66123, // Melon � rayure plein
	NPC_RUNTY_STRIPED_MELON             = 66130, // Melon � rayure rabougri
	NPC_WILD_STRIPED_MELON              = 66134, // Melon � rayure sauvage
	
	// Citrouille mogu
	NPC_ALLURING_MOGU_PUMPKIN           = 63178, // Citrouille mogu app�tissante
	NPC_PLUMP_MOGU_PUMPKIN              = 63184, // Citrouille mogu charnu
	NPC_PARCHED_MOGU_PUMPKIN            = 63183, // Citrouille mogu dessech�es
	NPC_GROWING_MOGU_PUMPKIN            = 66181, // Citrouille mogu en pleine croissance
	NPC_TANGLED_MOGU_PUMPKIN            = 63188, // Citrouille mogu �nchev�tr�
	NPC_SMOTHERED_MOGU_PUMPKIN          = 63187, // Citrouille mogu �touff�
	NPC_WIGGLING_MOGU_PUMPKIN           = 63189, // Citrouille mogu fr�tillantes
	NPC_INFESTED_MOGU_PUMPKIN           = 63182, // Citrouille mogu infest�
	NPC_RIPE_MOGU_PUMPKIN               = 63185, // Citrouille mogu m�re
	NPC_BURSTING_MOGU_PUMPKIN           = 63180, // Citrouille mogu plein
	NPC_RUNTY_MOGU_PUMPKIN              = 63186, // Citrouille mogu rabougrie
	NPC_WILD_MOGU_PUMPKIN               = 63190, // Citrouille mogu sauvage
	
	// Echalotes
	NPC_ALLURING_SCALLIONS              = 63159, // Echalotes app�tissantes
	NPC_PLUMP_SCALLIONS                 = 63164, // Echalotes charnu
	NPC_PARCHED_SCALLIONS               = 63163, // Echalotes dessech�es
	NPC_GROWING_SCALLIONS               = 63161, // Echalotes en pleine croissance 
	NPC_TANGLED_SCALLIONS               = 63168, // Echalotes �nchev�tr�
	NPC_SMOTHERED_SCALLIONS             = 63167, // Echalotes �touff�
	NPC_WIGGLING_SCALLIONS              = 63169, // Echalotes fr�tillantes
	NPC_INFESTED_SCALLIONS              = 63162, // Echalotes infest�
	NPC_RIPE_SCALLIONS                  = 63165, // Echalotes m�re
	NPC_BURSTING_SCALLIONS              = 63160, // Echalotes pleines
	NPC_RUNTY_SCALLIONS                 = 63166, // Echalotes rabougri
	NPC_WILD_SCALLIONS                  = 63170, // Echalotes sauvage
	
	// Courge de jade
	NPC_ALLURING_JADE_SQUASH            = 66107, // Courge de jade app�tissant
	NPC_PLUMP_JADE_SQUASH               = 66112, // Courge de jade charnu
	NPC_PARCHED_JADE_SQUASH             = 66111, // Courge de jade dessech�es
	NPC_GROWING_JADE_SQUASH             = 66109, // Courge de jade en pleine croissance
	NPC_TANGLED_JADE_SQUASH             = 66116, // Courge de jade �nchev�tr�
	NPC_SMOTHERED_JADE_SQUASH           = 66115, // Courge de jade �touff�
	NPC_WIGGLING_JADE_SQUASH            = 66117, // Courge de jade fr�tillantes
	NPC_INFESTED_JADE_SQUASH            = 66110, // Courge de jade infest�
	NPC_RIPE_JADE_SQUASH                = 66113, // Courge de jade m�re
	NPC_BURSTING_JADE_SQUASH            = 66108, // Courge de jade plein
	NPC_RUNTY_JADE_SQUASH               = 66114, // Courge de jade rabougrie
	NPC_WILD_JADE_SQUASH                = 63190, // Courge de jade sauvage
	
	// Carrotte croquejuteuse
	NPC_ALLURING_JUICYCRUNCH_CARROT     = 63147, // Carotte croquejuteuse app�tissant
	NPC_PLUMP_JUICYCRUNCH_CARROT        = 63156, // Carotte croquejuteuse charnu
	NPC_PARCHED_JUICYCRUNCH_CARROT      = 63144, // Carotte croquejuteuse dess�ch�e
	NPC_GROWING_JUICYCRUNCH_CARROT      = 63153, // Carotte croquejuteuse en pleine croissance
	NPC_TANGLED_JUICYCRUNCH_CARROT      = 63151, // Carotte croquejuteuse �nchev�tr�
	NPC_SMOTHERED_JUICYCRUNCH_CARROT    = 63148, // Carotte croquejuteuse �touff�
	NPC_WIGGLING_JUICYCRUNCH_CARROT     = 63146, // Carotte croquejuteuse fr�tillantes
	NPC_INFESTED_JUICYCRUNCH_CARROT     = 63145, // Carotte croquejuteuse infest�
	NPC_RIPE_JUICYCRUNCH_CARROT         = 63154, // Carotte croquejuteuse m�re
	NPC_BURSTING_JUICYCRUNCH_CARROT     = 63158, // Carotte croquejuteuse pleine
	NPC_RUNTY_JUICYCRUNCH_CARROT        = 63150, // Carotte croquejuteuse rabougri
	NPC_WILD_JUICYCRUNCH_CARROT         = 63149, // Carotte croquejuteuse sauvage
	
	// Navet rose
	NPC_ALLURING_PINK_TURNIP            = 63243, // Navet rose app�tissant
	NPC_PLUMP_PINK_TURNIP               = 63249, // Navet rose charnu
	NPC_PARCHED_PINK_TURNIP             = 63248, // Navet rose dess�ch�e
	NPC_GROWING_PINK_TURNIP             = 63246, // Navet rose en pleine croissance
	NPC_TANGLED_PINK_TURNIP             = 63253, // Navet rose �nchev�tr�
	NPC_SMOTHERED_PINK_TURNIP           = 63252, // Navet rose �touff�
	NPC_WIGGLING_PINK_TURNIP            = 63254, // Navet rose fr�tillant
	NPC_INFESTED_PINK_TURNIP            = 63247, // Navet rose infest�
	NPC_RIPE_PINK_TURNIP                = 63250, // Navet rose m�re
	NPC_BURSTING_PINK_TURNIP            = 63245, // Navet rose plein
	NPC_RUNTY_PINK_TURNIP               = 63251, // Navet rose rabougri
	//NPC_WILD_PINK_TURNIP                = 00000, // ????? Navet rose sauvage
	
	// Choux vert
	NPC_ALLURING_GREEN_CABBAGE          = 60070, // Choux vert app�tissant
	NPC_PLUMP_GREEN_CABBAGE             = 60113, // Choux vert charnu
	NPC_PARCHED_GREEN_CABBAGE           = 58565, // Choux vert dess�ch�e
	NPC_GROWING_GREEN_CABBAGE           = 58566, // Choux vert en pleine croissance
	NPC_TANGLED_GREEN_CABBAGE           = 60270, // Choux vert �nchev�tr�
	NPC_SMOTHERED_GREEN_CABBAGE         = 60181, // Choux vert �touff�
	NPC_WIGGLING_GREEN_CABBAGE          = 60029, // Choux vert fr�tillantes
	NPC_INFESTED_GREEN_CABBAGE          = 60026, // Choux vert infest�
	NPC_RIPE_GREEN_CABBAGE              = 60113, // Choux vert m�re
	NPC_BURSTING_GREEN_CABBAGE          = 63157, // Choux vert plein
	NPC_RUNTY_GREEN_CABBAGE             = 60218, // Choux vert rabougri
	NPC_WILD_GREEN_CABBAGE              = 60207, // Choux vert sauvage
};
/*
class npc_till_soil : public CreatureScript
{
	public:
		npc_till_soil() : CreatureScript("npc_till_soil"){ }

		struct npc_till_soilAI : public ScriptedAI
		{
			void UpdateAI()
			{
				if (SPELL_TILL_SOIL)
				{
					me->SummonCreature(NPC_TILLED_SOIL, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
				}
				else
				{
					me->SummonCreature(NPC_UNTILLED_SOIL, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
				}
			}
		}
};
*/
void AddSC_sunsong_ranch()
{
	//new npc_till_soil();
}