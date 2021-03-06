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

/* ScriptData
SDName: Boss_Baron_Rivendare
SD%Complete: 70
SDComment: aura applied/defined in database
SDCategory: Stratholme
EndScriptData */

#include "precompiled.h"
#include "stratholme.h"

#define SAY_0    "Eindringlinge! Zweifellos mehr Handlanger der Argentumd�mmerung. Es befindet sich bereits eine von ihnen in meiner Gefangenschaft. Zieht Euch aus meiner Dom�ne zur�ck, bevor ich sie hinrichten lasse!"
#define SAY_1    "Ihr seid immer noch hier? Eure Torheit ist wahrhaft am�sant! Das M�dchen der Argentumd�mmerung muss jedoch nicht unn�tig leiden. Verschwindet sofort, und ihr Leben wird verschont werden!"
#define SAY_2    "Es wird mir gro�es Vergn�gen bereiten, dieses armselige M�dchen ihres Lebens zu berauben! Aber es ist noch nicht zu sp�t, sie muss nicht unn�tig leiden. Kehrt um und ich werde ihr einen gnadenvollen Tod bereiten."
#define SAY_3    "M�ge der Tod dieses Gefangenen als ein Beispiel dienen. Niemand, der sich der Gei�el in den Weg stellt, wird �berleben!"
#define SAY_4    "Soso, Ihr haltet es also f�r angebracht, Euch mit den Sch�pfungen des Lichk�nigs anzulegen? Ramstein, die Eindringlinge sollten angemessen begr��t werden."
#define SAY_5    "Es ist an der Zeit, die Dinge selbst in die Hand zu nehmen. Kommt nur. Betretet mein Reich und fordert die Macht der Gei�el heraus!"

#define ADD_1X 4017.403809f
#define ADD_1Y -3339.703369f
#define ADD_1Z 115.057655f
#define ADD_1O 5.487860f

#define ADD_2X 4013.189209f
#define ADD_2Y -3351.808350f
#define ADD_2Z 115.052254f
#define ADD_2O 0.134280f

#define ADD_3X 4017.738037f
#define ADD_3Y -3363.478016f
#define ADD_3Z 115.057274f
#define ADD_3O 0.723313f

#define ADD_4X 4048.877197f
#define ADD_4Y -3363.223633f
#define ADD_4Z 115.054253f
#define ADD_4O 3.627735f

#define ADD_5X 4051.777588f
#define ADD_5Y -3350.893311f
#define ADD_5Z 115.055351f
#define ADD_5O 3.066176f

#define ADD_6X 4048.375977f
#define ADD_6Y -3339.966309f
#define ADD_6Z 115.055222f
#define ADD_6O 2.457497f

#define SPELL_SHADOWBOLT    17393
#define SPELL_CLEAVE        15284
#define SPELL_MORTALSTRIKE  15708

#define SPELL_UNHOLY_AURA   17467
#define SPELL_RAISEDEAD     17473                           //triggers death pact (17471)

#define SPELL_RAISE_DEAD1   17475
#define SPELL_RAISE_DEAD2   17476
#define SPELL_RAISE_DEAD3   17477
#define SPELL_RAISE_DEAD4   17478
#define SPELL_RAISE_DEAD5   17479
#define SPELL_RAISE_DEAD6   17480

struct MANGOS_DLL_DECL boss_baron_rivendareAI : public ScriptedAI
{
    boss_baron_rivendareAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 ShadowBolt_Timer;
    uint32 Cleave_Timer;
    uint32 MortalStrike_Timer;
    //uint32 RaiseDead_Timer;
    uint32 SummonSkeletons_Timer;
    Creature *Summoned;

    void Reset()
    {
        ShadowBolt_Timer = 5000;
        Cleave_Timer = 8000;
        MortalStrike_Timer = 12000;
        //RaiseDead_Timer = 30000;
        SummonSkeletons_Timer = 34000;
    }

    void Aggro(Unit *who)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BARON,IN_PROGRESS);
    }

    void JustSummoned(Creature* summoned)
    {
        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
            summoned->AI()->AttackStart(target);
    }

    void JustDied(Unit* Killer)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BARON,DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //ShadowBolt
        if (ShadowBolt_Timer < diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(m_creature->getVictim(),SPELL_SHADOWBOLT);
            ShadowBolt_Timer = 10000;
        }else ShadowBolt_Timer -= diff;

        //Cleave
        if (Cleave_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(),SPELL_CLEAVE);
            Cleave_Timer = urand(7000, 17000);
        }else Cleave_Timer -= diff;

        //MortalStrike
        if (MortalStrike_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(),SPELL_MORTALSTRIKE);
            MortalStrike_Timer = urand(10000, 25000);
        }else MortalStrike_Timer -= diff;

        //RaiseDead
        //if (RaiseDead_Timer < diff)
        //{
        //    DoCastSpellIfCan(m_creature,SPELL_RAISEDEAD);
        //    RaiseDead_Timer = 45000;
        //}else RaiseDead_Timer -= diff;

        //SummonSkeletons
        if (SummonSkeletons_Timer < diff)
        {
            m_creature->SummonCreature(11197,ADD_1X,ADD_1Y,ADD_1Z,ADD_1O,TEMPSUMMON_TIMED_DESPAWN,29000);
            m_creature->SummonCreature(11197,ADD_2X,ADD_2Y,ADD_2Z,ADD_2O,TEMPSUMMON_TIMED_DESPAWN,29000);
            m_creature->SummonCreature(11197,ADD_3X,ADD_3Y,ADD_3Z,ADD_3O,TEMPSUMMON_TIMED_DESPAWN,29000);
            m_creature->SummonCreature(11197,ADD_4X,ADD_4Y,ADD_4Z,ADD_4O,TEMPSUMMON_TIMED_DESPAWN,29000);
            m_creature->SummonCreature(11197,ADD_5X,ADD_5Y,ADD_5Z,ADD_5O,TEMPSUMMON_TIMED_DESPAWN,29000);
            m_creature->SummonCreature(11197,ADD_6X,ADD_6Y,ADD_6Z,ADD_6O,TEMPSUMMON_TIMED_DESPAWN,29000);

            SummonSkeletons_Timer = 40000;
        }else SummonSkeletons_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_baron_rivendare(Creature* pCreature)
{
    return new boss_baron_rivendareAI(pCreature);
}

void AddSC_boss_baron_rivendare()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_baron_rivendare";
    newscript->GetAI = &GetAI_boss_baron_rivendare;
    newscript->RegisterSelf();
}
