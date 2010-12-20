/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Boss_Heigan
SD%Complete: 0
SDComment: Place Holder
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

#define NAXXRAMAS				533

#define ACHIEV_SAFETY_DANCE		1996
#define H_ACHIEV_SAFETY_DANCE	2139

#define SAY_AGGRO1				-1533109
#define SAY_AGGRO2				-1533110
#define SAY_AGGRO3				-1533111
#define SAY_SLAY				-1533112
#define SAY_TAUNT1				-1533113
#define SAY_TAUNT2				-1533114
#define SAY_TAUNT3				-1533115
#define SAY_TAUNT4				-1533116
#define SAY_TAUNT5				-1533117
#define SAY_DEATH				-1533118

//Spell used by floor peices to cause damage to players
#define SPELL_ERUPTION			29371

//Spells by boss
#define SPELL_DISRUPTION		29310
#define SPELL_FEAVER			29998
#define H_SPELL_FEAVER			55011
#define SPELL_PLAGUED_CLOUD		29350

#define PLATFORM_X				2793.86f
#define PLATFORM_Y				-3707.38f
#define PLATFORM_Z				276.627f
#define PLATFORM_O				0.593f



struct MANGOS_DLL_DECL boss_heiganAI : public ScriptedAI
{
    boss_heiganAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiEvadeCheckCooldown;

    std::list<uint64> m_PlagueFissureGUID[4];

    uint32 Disruption_Timer;
    uint32 Feaver_Timer;
    uint32 Erupt_Timer;
    uint32 Phase_Timer;

    uint32 m_uiSafeSection;
    bool   m_bEruptDirection;

    bool   m_bCombatPhase;
    bool   m_bDelay;
	bool   m_bIsOpenGate;

	uint32 m_uiDeathCheckTimer;
	bool   m_bIsPlayerDeath;

    void Reset()
    {
        m_uiEvadeCheckCooldown = 2000;
        Feaver_Timer = 20000;
        Phase_Timer = 90000;
        Erupt_Timer = 10000;
        Disruption_Timer = 0;
        m_uiSafeSection = 0;
        m_bEruptDirection = false;
        m_bCombatPhase = true;
        m_bDelay = false;
        SetCombatMovement(true);
		m_bIsOpenGate = false;

		m_uiDeathCheckTimer = 1000;
		m_bIsPlayerDeath = false;

        if(m_pInstance)
		{
            m_pInstance->SetData(TYPE_HEIGAN, NOT_STARTED);

            if (GameObject* pGate = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_PLAG_HEIG_ENTRY_DOOR)))
                pGate->SetGoState(GO_STATE_ACTIVE);
		}
    }

    void MoveInLineOfSight(Unit *pWho) 
    {
        if(!pWho)
            return;

        if(!m_bIsOpenGate && (m_creature->IsWithinDistInMap(pWho, 100.0f)))
        {
            if (GameObject* pGate = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_PLAG_HEIG_ENTRY_DOOR)))
                pGate->SetGoState(GO_STATE_ACTIVE);
        }
        m_bIsOpenGate = true;
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_HEIGAN, FAIL);
    }

    void Aggro(Unit* pWho)
    {
        for (uint8 i = 0; i < 4; ++i)
            m_PlagueFissureGUID[i].clear();
        for (uint32 i = 10; i <= 52; ++i)
            if (GameObject* pPlagueFissure = GetClosestGameObjectWithEntry(m_creature, 181500+i, 100.0f))
            {
                if (i != 10 && i != 17 && i != 18 && i != 19 && i != 20 && i != 21 && i != 22 && i != 23 && i != 24 && i != 26)
                    m_PlagueFissureGUID[0].push_back(pPlagueFissure->GetGUID());
                if (i != 11 && i != 12 && i != 13 && i != 14 && i != 15 && i != 16 && i != 25 && i != 27 && i != 28 && i != 29 && i != 30 && i != 31)
                    m_PlagueFissureGUID[1].push_back(pPlagueFissure->GetGUID());
                if (i != 32 && i != 33 && i != 34 && i != 35 && i != 36 && i != 40 && i != 41 && i != 42 && i != 43 && i != 44 && i != 45)
                    m_PlagueFissureGUID[2].push_back(pPlagueFissure->GetGUID());
                if (i != 37 && i != 48 && i != 39 && i != 46 && i != 47 && i != 48 && i != 49 && i != 50 && i != 51 && i != 52)
                    m_PlagueFissureGUID[3].push_back(pPlagueFissure->GetGUID());
            }

        std::list<GameObject*> lList;
        GetGameObjectListWithEntryInGrid(lList, m_creature, 181678, 100.0f);
        if (!lList.empty())
            for (std::list<GameObject*>::iterator itr = lList.begin(); itr != lList.end(); ++itr)
            {
                m_PlagueFissureGUID[1].push_back((*itr)->GetGUID());
                m_PlagueFissureGUID[2].push_back((*itr)->GetGUID());
                m_PlagueFissureGUID[3].push_back((*itr)->GetGUID());
            }

        lList.clear();
        GetGameObjectListWithEntryInGrid(lList, m_creature, 181676, 100.0f);
        if (!lList.empty())
            for (std::list<GameObject*>::iterator itr = lList.begin(); itr != lList.end(); ++itr)
            {
                m_PlagueFissureGUID[0].push_back((*itr)->GetGUID());
                m_PlagueFissureGUID[2].push_back((*itr)->GetGUID());
                m_PlagueFissureGUID[3].push_back((*itr)->GetGUID());
            }

        lList.clear();
        GetGameObjectListWithEntryInGrid(lList, m_creature, 181677, 100.0f);
        if (!lList.empty())
            for (std::list<GameObject*>::iterator itr = lList.begin(); itr != lList.end(); ++itr)
            {
                m_PlagueFissureGUID[0].push_back((*itr)->GetGUID());
                m_PlagueFissureGUID[1].push_back((*itr)->GetGUID());
                m_PlagueFissureGUID[3].push_back((*itr)->GetGUID());
            }

        lList.clear();
        GetGameObjectListWithEntryInGrid(lList, m_creature, 181695, 100.0f);
        if (!lList.empty())
            for (std::list<GameObject*>::iterator itr = lList.begin(); itr != lList.end(); ++itr)
            {
                m_PlagueFissureGUID[0].push_back((*itr)->GetGUID());
                m_PlagueFissureGUID[1].push_back((*itr)->GetGUID());
                m_PlagueFissureGUID[2].push_back((*itr)->GetGUID());
            }

        switch (rand()%3)
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
        }

        // Teleport "cheaters" to center of room
        Map* map = m_creature->GetMap();
        if (map->IsDungeon())
        {
            Map::PlayerList const &PlayerList = map->GetPlayers();

            if (!PlayerList.isEmpty())
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    i->getSource()->TeleportTo(NAXXRAMAS, 2804.946f, -3678.315f, 273.666f, 4.385f);
        }

        if(m_pInstance)
            m_pInstance->SetData(TYPE_HEIGAN, IN_PROGRESS);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_SLAY, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if(m_pInstance)
			m_pInstance->SetData(TYPE_HEIGAN, DONE);

        if (!m_bIsPlayerDeath)
        {
            AchievementEntry const *AchievSafetyDance = GetAchievementStore()->LookupEntry(m_bIsRegularMode ? ACHIEV_SAFETY_DANCE : H_ACHIEV_SAFETY_DANCE);
            if (AchievSafetyDance)
            {
                Map* pMap = m_creature->GetMap();
                if (pMap && pMap->IsDungeon())
                {
                    Map::PlayerList const &players = pMap->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                        itr->getSource()->CompletedAchievement(AchievSafetyDance);
                }
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiDeathCheckTimer < diff)
        {
			Map* pMap = m_creature->GetMap();
			if(pMap)
			{
				Map::PlayerList const &lPlayers = pMap->GetPlayers();
				for (Map::PlayerList::const_iterator iter = lPlayers.begin(); iter != lPlayers.end(); ++iter)
				{
					Player* pPlayer = iter->getSource();

					if(!pPlayer->isAlive())
						m_bIsPlayerDeath = true;
				}
			}
            m_uiDeathCheckTimer = 1000;
        }else m_uiDeathCheckTimer -= diff;

        if (m_uiEvadeCheckCooldown < diff)
        {
            if (m_creature->GetDistance2d(2769.68f, -3684.61f) > 48.0f)
                EnterEvadeMode();

            m_uiEvadeCheckCooldown = 2000;
        }else m_uiEvadeCheckCooldown -= diff;

        if (m_bDelay)
        {
            m_bDelay = false;
            DoCast(m_creature, SPELL_PLAGUED_CLOUD);
        }

        if (Phase_Timer < diff)
        {
            if (m_bCombatPhase)
            {
                m_creature->InterruptNonMeleeSpells(true);
                m_creature->StopMoving();
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->GetMotionMaster()->MoveIdle();
                m_creature->GetMap()->CreatureRelocation(m_creature, PLATFORM_X, PLATFORM_Y, PLATFORM_Z, PLATFORM_O);
                m_creature->SendMonsterMove(PLATFORM_X, PLATFORM_Y, PLATFORM_Z, SPLINETYPE_NORMAL, SPLINEFLAG_DONE, 0);
                SetCombatMovement(false);
                m_bDelay = true;
                Phase_Timer = 45000;
            }
            else
            {
                SetCombatMovement(true);
                DoStartMovement(m_creature->getVictim());
                Phase_Timer = 90000;
                Disruption_Timer = 0;
            }

            m_bCombatPhase = !m_bCombatPhase;
            m_uiSafeSection = 0;
            Erupt_Timer = 10000;
            m_bEruptDirection = false;

        }
        else
            Phase_Timer -= diff;

        if (Erupt_Timer < diff)
        {
            for (std::list<uint64>::iterator itr = m_PlagueFissureGUID[m_uiSafeSection].begin(); itr != m_PlagueFissureGUID[m_uiSafeSection].end(); ++itr)
            {
                if (GameObject* pGo = m_creature->GetMap()->GetGameObject(*itr))

                if (pGo)
                {
                    WorldPacket data(SMSG_GAMEOBJECT_CUSTOM_ANIM,8+4);
                    data << pGo->GetGUID();
                    data << 0;
                    pGo->SendMessageToSet(&data,true);
                    if (Creature* pTemp = m_creature->SummonCreature(14667, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 100))
                    {
                        pTemp->SetLevel(80);
                        pTemp->CastSpell(pTemp, SPELL_ERUPTION, true);
                    }
                }
            }

            if (m_uiSafeSection == 0 || m_uiSafeSection == 3)
                m_bEruptDirection = !m_bEruptDirection;


            m_bEruptDirection ? ++m_uiSafeSection : --m_uiSafeSection;

            m_bCombatPhase ? Erupt_Timer = 10000 : Erupt_Timer = 3000;

            // Teleport "cheaters" to center of room (until LOS with GOs will be implemented)
            Map* map = m_creature->GetMap();
            if (map->IsDungeon())
            {
                Map::PlayerList const &PlayerList = map->GetPlayers();

                if (!PlayerList.isEmpty())
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        if ((i->getSource()->GetDistance2d(2769.68f, -3684.61f) > 48.0f) && (i->getSource()->GetDistance2d(2769.68f, -3684.61f) < 100.0f))
                            i->getSource()->TeleportTo(533, 2769.68f, -3684.61f, 273.66f, 5.5f);
            }
        }
        else
            Erupt_Timer -= diff; 

        if (m_bCombatPhase)
        {
            if (Disruption_Timer < diff)
            {
                DoCast(m_creature, SPELL_DISRUPTION);
                Disruption_Timer = 10000;
            }
            else
                Disruption_Timer -= diff;

            if (Feaver_Timer < diff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(pTarget, m_bIsRegularMode ? SPELL_FEAVER : H_SPELL_FEAVER);
                Feaver_Timer = 20000;
            }
            else
                Feaver_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    }
};

CreatureAI* GetAI_boss_heigan(Creature* pCreature)
{
    return new boss_heiganAI(pCreature);
}

void AddSC_boss_heigan()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_heigan";
    newscript->GetAI = &GetAI_boss_heigan;
    newscript->RegisterSelf();
}
