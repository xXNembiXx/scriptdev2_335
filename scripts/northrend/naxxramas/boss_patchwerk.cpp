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
SDName: Boss_Patchwerk
SD Author: Nembi
SD%Complete: 80
SDComment: Include Frogger!
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

enum
{
    ACHIEV_MAKE_QUICK	  = 1856,
    H_ACHIEV_MAKE_QUICK	  = 1857,

    SAY_AGGRO1            = -1533017,
    SAY_AGGRO2            = -1533018,
    SAY_SLAY              = -1533019,
    SAY_DEATH             = -1533020,

    EMOTE_GENERIC_BERSERK   = -1000004,
    EMOTE_GENERIC_ENRAGED   = -1000003,

    SPELL_HATEFULSTRIKE   = 28308,
    SPELL_HATEFULSTRIKE_H = 59192,
    SPELL_ENRAGE          = 28131,
    SPELL_BERSERK         = 26662,
    SPELL_SLIMEBOLT       = 32309
};

const float MELEE_DISTANCE = 5.0;

struct MANGOS_DLL_DECL boss_patchwerkAI : public ScriptedAI
{
    boss_patchwerkAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiHatefulStrikeTimer;
    uint32 m_uiBerserkTimer;
    uint32 m_uiSlimeboltTimer;
    bool   m_bEnraged;
    bool   m_bBerserk;

    bool   m_bIsInTime;
    uint32 m_uiAchievTimer;

    void Reset()
    {
        m_uiHatefulStrikeTimer = 1000;                      //1 second
        m_uiBerserkTimer = MINUTE*6*IN_MILLISECONDS;         //6 minutes
        m_uiSlimeboltTimer = 10000;
        m_bEnraged = false;
        m_bBerserk = false;

        m_bIsInTime = true;
        m_uiAchievTimer = 180000;
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_PATCHWERK, FAIL);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (urand(0, 4))
            return;

        DoScriptText(SAY_SLAY, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_PATCHWERK, DONE);

        if (m_bIsInTime)
        {
            AchievementEntry const *AchievMakeQuick = GetAchievementStore()->LookupEntry(m_bIsRegularMode ? ACHIEV_MAKE_QUICK : H_ACHIEV_MAKE_QUICK);
            if (AchievMakeQuick)
            {
                Map* pMap = m_creature->GetMap();
                if (pMap && pMap->IsDungeon())
                {
                    Map::PlayerList const &players = pMap->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                        itr->getSource()->CompletedAchievement(AchievMakeQuick);
                }
            }
        }
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(urand(0, 1)?SAY_AGGRO1:SAY_AGGRO2, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_PATCHWERK, IN_PROGRESS);

		m_creature->CallForHelp(50.0f);
    }

    void DoHatefulStrike()
    {
        // The ability is used on highest HP target choosen of the top 2 (3 heroic) targets on threat list being in melee range
        Unit* pTarget = NULL;
        uint32 uiHighestHP = 0;
        uint32 uiTargets = m_bIsRegularMode ? 2 : 3;

        ThreatList const& lThreatList = m_creature->getThreatManager().getThreatList();
        for (ThreatList::const_iterator iter = lThreatList.begin(); iter != lThreatList.end(); ++iter)
        {
            if (!uiTargets)
                break;

            if (Unit* pTempTarget = m_creature->GetMap()->GetUnit((*iter)->getUnitGuid()))
            {
                if (pTempTarget->GetHealth() > uiHighestHP && m_creature->IsWithinDistInMap(pTempTarget, MELEE_DISTANCE))
                {
                    uiHighestHP = pTempTarget->GetHealth();
                    pTarget = pTempTarget;
                }
            }

            --uiTargets;
        }

        if (pTarget)
            DoCast(pTarget, m_bIsRegularMode ? SPELL_HATEFULSTRIKE : SPELL_HATEFULSTRIKE_H);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiAchievTimer < uiDiff)
        {
            m_bIsInTime = false;
            m_uiAchievTimer = 0;
        }else  m_uiAchievTimer -= uiDiff;

        // Hateful Strike
        if (m_uiHatefulStrikeTimer < uiDiff)
        {
            DoHatefulStrike();
            m_uiHatefulStrikeTimer = 1000;
        }
        else
            m_uiHatefulStrikeTimer -= uiDiff;

        // Soft Enrage at 5%
        if (!m_bEnraged)
        {
            if (m_creature->GetHealth()*20 < m_creature->GetMaxHealth())
            {
                DoCast(m_creature, SPELL_ENRAGE);
                DoScriptText(EMOTE_GENERIC_ENRAGED, m_creature);
                m_bEnraged = true;
            }
        }

        // Berserk after 6 minutes
        if (!m_bBerserk)
        {
            if (m_uiBerserkTimer < uiDiff)
            {
                DoCast(m_creature, SPELL_BERSERK);
                DoScriptText(EMOTE_GENERIC_BERSERK, m_creature);
                m_bBerserk = true;
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }
        else
        {
            // Slimebolt - casted only while Berserking to prevent kiting
            if (m_uiSlimeboltTimer < uiDiff)
            {
                DoCast(m_creature->getVictim(), SPELL_SLIMEBOLT);
                m_uiSlimeboltTimer = 5000;
            }
            else
                m_uiSlimeboltTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

/* ************* *
 * Living Poisen *
 * ************* */

#define SPELL_DEATH		5


struct MANGOS_DLL_DECL naxx_mob_poisenAI : public ScriptedAI
{
    naxx_mob_poisenAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset(){}

    void MoveInLineOfSight(Unit *pWho) 
    {
        if(!pWho)
            return;

        if(m_creature->IsWithinDistInMap(pWho, 2.0f))
        {
            if(pWho->GetTypeId() != TYPEID_PLAYER)
                return;

            pWho->CastSpell(pWho,SPELL_DEATH, true);
            m_creature->ForcedDespawn();
       }
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
    }
};

CreatureAI* GetAI_boss_patchwerk(Creature* pCreature)
{
    return new boss_patchwerkAI(pCreature);
}

CreatureAI* GetAI_naxx_mob_poisen(Creature* pCreature)
{
    return new naxx_mob_poisenAI(pCreature);
}

void AddSC_boss_patchwerk()
{
    Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_patchwerk";
    NewScript->GetAI = &GetAI_boss_patchwerk;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "naxx_mob_poisen";
    NewScript->GetAI = &GetAI_naxx_mob_poisen;
    NewScript->RegisterSelf();
}
