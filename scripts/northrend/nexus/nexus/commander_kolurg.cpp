/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* Script Data Start
SDName: Boss Commander Kolurg
SD%Complete: 95%
SDComment:  Only Alliance Heroic
SDCategory:
Script Data End */

#include "precompiled.h"

#define SPELL_BATTLE_SHOUT                                    31403
#define SPELL_CHARGE                                          60067
#define SPELL_FRIGHTENING_SHOUT                               19134
#define SPELL_WHIRLWIND_1                                     38619
#define SPELL_WHIRLWIND_2                                     38618

//not used
//Yell
#define SAY_AGGRO                                          -1576024
#define SAY_KILL                                           -1576025
#define SAY_DEATH                                          -1576026

struct MANGOS_DLL_DECL boss_commander_kolurgAI : public ScriptedAI
{
    boss_commander_kolurgAI(Creature *pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset() {}
    void EnterCombat(Unit* who) {}
    void AttackStart(Unit* who) {}
    void MoveInLineOfSight(Unit* who) {}
    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
    void JustDied(Unit* killer)  {}
};

CreatureAI* GetAI_boss_commander_kolurg(Creature* pCreature)
{
    return new boss_commander_kolurgAI (pCreature);
}

void AddSC_boss_commander_kolurg()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_commander_kolurg";
    newscript->GetAI = &GetAI_boss_commander_kolurg;
    newscript->RegisterSelf();
}
