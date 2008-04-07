/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* ScriptData
SDName: GO_Scripts
SD%Complete: 100
SDComment: Quest support: 4285, 4287, 4288 (crystal pylons in un'goro). Field_Repair_Bot->Teaches spell 22704. Barov_journal->Teaches spell 26089
SDCategory: Game Objects
EndScriptData */
 
#include "sc_creature.h"
#include "sc_gossip.h"
#include "GameObject.h"
 
/*######
## go_crystal_pylons (3x)
######*/

bool GOHello_go_northern_crystal_pylon(Player *player, GameObject* _GO)
{
    if (_GO->GetGoType() == GAMEOBJECT_TYPE_QUESTGIVER)
    {
        player->PrepareQuestMenu(_GO->GetGUID());
        player->SendPreparedQuest(_GO->GetGUID());
    }

    if (player->GetQuestStatus(4285) == QUEST_STATUS_INCOMPLETE)
        player->CompleteQuest(4285);

    return true;
}

bool GOHello_go_eastern_crystal_pylon(Player *player, GameObject* _GO)
{
    if (_GO->GetGoType() == GAMEOBJECT_TYPE_QUESTGIVER)
    {
        player->PrepareQuestMenu(_GO->GetGUID());
        player->SendPreparedQuest(_GO->GetGUID());
    }

    if (player->GetQuestStatus(4287) == QUEST_STATUS_INCOMPLETE)
        player->CompleteQuest(4287);

    return true;
}

bool GOHello_go_western_crystal_pylon(Player *player, GameObject* _GO)
{
    if (_GO->GetGoType() == GAMEOBJECT_TYPE_QUESTGIVER)
    {
        player->PrepareQuestMenu(_GO->GetGUID());
        player->SendPreparedQuest(_GO->GetGUID());
    }

    if (player->GetQuestStatus(4288) == QUEST_STATUS_INCOMPLETE)
        player->CompleteQuest(4288);

    return true;
}

/*######
## go_field_repair_bot_74A
######*/

bool GOHello_go_field_repair_bot_74A(Player *player, GameObject* _GO)
{
    if (player->HasSkill(202) && player->GetSkillValue(202) >= 300 && !player->HasSpell(22704)) 
    {
        player->CastSpell(player,22705,false);
    }
    return true;
}

/*######
## go_barov_journal
######*/

bool GOHello_go_barov_journal(Player *player, GameObject* _GO)
{
    if (player->HasSkill(197) && player->GetSkillValue(197) >= 280 && !player->HasSpell(26086)) 
    {
        player->CastSpell(player,26089,false);
    }
    return true;
}

/*######
## go_orb_of_command
######*/

bool GOHello_go_orb_of_command(Player *player, GameObject* _GO)
{
    if (player->GetQuestRewardStatus(7761)) 
        player->TeleportTo(469,-7672,-1107,399, 0); 

    return true;
}

/*######
## go_teleporter
######*/

bool GOHello_go_teleporter(Player *player, GameObject* _GO)
{
    player->TeleportTo(0, 1807.07f,336.105f,70.3975f,0.0f);
    return false;
}

void AddSC_go_scripts()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="go_northern_crystal_pylon";
    newscript->pGOHello =           &GOHello_go_northern_crystal_pylon;
    m_scripts[nrscripts++] = newscript;

    newscript = new Script;
    newscript->Name="go_eastern_crystal_pylon";
    newscript->pGOHello =           &GOHello_go_eastern_crystal_pylon;
    m_scripts[nrscripts++] = newscript;

    newscript = new Script;
    newscript->Name="go_western_crystal_pylon";
    newscript->pGOHello =           &GOHello_go_western_crystal_pylon;
    m_scripts[nrscripts++] = newscript;

    newscript = new Script;
    newscript->Name="go_field_repair_bot_74A";
    newscript->pGOHello =           &GOHello_go_field_repair_bot_74A;
    m_scripts[nrscripts++] = newscript;

    newscript = new Script;
    newscript->Name="go_barov_journal";
    newscript->pGOHello =           &GOHello_go_barov_journal;
    m_scripts[nrscripts++] = newscript;

    newscript = new Script;
    newscript->Name="go_orb_of_command";
    newscript->pGOHello =           &GOHello_go_orb_of_command;
    m_scripts[nrscripts++] = newscript;

    newscript = new Script;
    newscript->Name="go_teleporter";
    newscript->pGOHello =           GOHello_go_teleporter;
    m_scripts[nrscripts++] = newscript;
}
