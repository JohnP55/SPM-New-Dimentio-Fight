#include "evt_cmd.h"
#include "mod.h"
#include "patch.h"
#include "exception.h"
#include "romfontexpand.h"

#include <types.h>
#include <spm/fontmgr.h>
#include <spm/seqdrv.h>
#include <spm/seqdef.h>
#include <wii/OSError.h>
#include <spm/evtmgr_cmd.h>
#include <spm/evt_mario.h>
#include <spm/evt_msg.h>
#include <spm/msgdrv.h>
#include <spm/seqdef.h>
#include <wii/string.h>
#include <wii/types.h>
#include <spm/evt_npc.h>
#include <spm/evt_seq.h>
#include <spm/evt_fade.h>
#include <spm/fadedrv.h>
#include <spm/evt_guide.h>
#include <spm/evt_fairy.h>
#include <spm/evt_mobj.h>
#include <wii/stdio.h>
#include <spm/evt_door.h>
#include <spm/mac_02.h>
#include <spm/npcdrv.h>
#include <spm/evt_snd.h>
#include <spm/mapdata.h>
#include <spm/evt_eff.h>
#include <spm/evt_cam.h>

namespace mod {

/*
    Title Screen Custom Text
    Prints "SPM Rel Loader" at the top of the title screen
*/

static spm::seqdef::SeqFunc *seq_titleMainReal;
static void seq_titleMainOverride(spm::seqdrv::SeqWork *wp)
{
    wii::RGBA green {0, 255, 0, 255};
    f32 scale = 0.8f;
    const char * msg = "SPM Rel Loader";
    spm::fontmgr::FontDrawStart();
    spm::fontmgr::FontDrawEdge();
    spm::fontmgr::FontDrawColor(&green);
    spm::fontmgr::FontDrawScale(scale);
    spm::fontmgr::FontDrawNoiseOff();
    spm::fontmgr::FontDrawRainbowColorOff();
    f32 x = -((spm::fontmgr::FontGetMessageWidth(msg) * scale) / 2);
    spm::fontmgr::FontDrawString(x, 200.0f, msg);
    seq_titleMainReal(wp);
}
static void titleScreenCustomTextPatch()
{
    seq_titleMainReal = spm::seqdef::seq_data[spm::seqdrv::SEQ_TITLE].main;
    spm::seqdef::seq_data[spm::seqdrv::SEQ_TITLE].main = &seq_titleMainOverride;
}
    
/*
    General mod functions
*/

// Define item text
const char * hello_world =
"<system>\n"
"Hello world!\n"
"<k>\n<p>\n"
"Sorry, you probably expected a\n"
"big explosion or something..\n"
"<k>\n<p>\n"
"<wait 50>.<wait 50>.<wait 50>.<wait 50>.<wait 50>.<wait 50>.<wait 50>.<wait 50>.<wait 50>.<wait 50>.<wait 50>.<wait 50>.<wait 50>.<wait 50>.<wait 50>.<wait 50>.<wait 50>.<wait 50>.<wait 50>.<wait 50>.\n"
"<k>\n<p>\n"
"Oh alright fine, here..\n<wait 200>"
"<k>\n<p>\n"
"You get some.. <wait 100><wave>magic!</wave>\n"
"<k>\n<o>\n";
// <select [Initial Selection, Unknown, Width, Y Pos]>
const char * sel_item =
"<select 0 -1 250 40>\n"
"Mushroom\n" // Item ID 6
"Speed Flower\n" // Item ID 3
"Cake\n" // Item ID 133
"Coins"; // Item ID 2

// Define allowed NPCs in Merlon's house

const char * customNameList[] = {
//    "dearu",
    "yogensyo",
//    "dimcut",
    nullptr
};

spm::npcdrv::NPCTribeAnimDef animsDimen[] = {
  {0, "S_2"}, // Flying (ID 0 is idle animation)
  {1, "S_1"}, // Standing
  {20, "W_3"}, // Finger click
  {21, "A_1a"}, // Magic start
  {22, "W_2c"}, // Land
  {23, "A_1c"}, // Magic cast
  {24, "A_2a"}, // Raise hands
  {25, "A_2b"}, // Wave hands
  {26, "A_2c"}, // Raise hands cast
  {27, "W_2a"}, // Fly
  {3, "T_2"}, // Talking (ID 3 is the ID to use when talking)
  {-1, nullptr}
};
spm::npcdrv::NPCTribeAnimDef animsDimenDead[] = {
  {0, "T_7"}, // Dead (ID 0 is idle animation)
  {1, "D_1"}, // About to die
  {20, "D_2"}, // Die
  {21, "D_3"}, // Get back up
  {3, "T_7"}, // Dead (ID 3 is the ID to use when talking)
  {-1, nullptr}
};

spm::npcdrv::NPCTribeAnimDef animsMerlonNormal[] = {
  {0, "S_1"}, // Standing (ID 0 is idle animation)
  {1, "W_1"}, // Walking 
  {21, "S_2"}, // Surprised
  {22, "O_1"}, // Surprised jump
  {23, "Q_2b"}, // Return to normal from surprised jump
  {3, "T_1"}, // Talking (ID 3 is the ID to use when talking)
  {-1, nullptr}
};
spm::npcdrv::NPCTribeAnimDef animsMerlonScared[] = {
  {0, "S_2"}, // Scared (ID 0 is idle animation)
  {1, "O_2a"}, // Scared jump
  {2, "O_2b"}, // Return to normal from scare
  {3, "T_2"}, // Scared Talking (ID 3 is the ID to use when talking)
  {-1, nullptr}
};
spm::npcdrv::NPCTribeAnimDef animsMerlonThinking[] = {
  {0, "S_3"}, // Thinking (ID 0 is idle animation)
  {1, "E_1a"}, // Start thinking
  {2, "E_1b"}, // Stop thinking
  {3, "T_3"}, // Thinking and Talking (ID 3 is the ID to use when talking)
  {-1, nullptr}
};

spm::npcdrv::NPCTribeAnimDef animsMerlonCut2[] = {
  {0, "S_2"}, // Standing (ID 0 is idle animation)
  {1, "W_1"}, // Walking 
  {21, "S_2"}, // Surprised
  {22, "O_1"}, // Surprised jump
  {23, "Q_2b"}, // Return to normal from surprised jump
  {3, "T_2"}, // Talking (ID 3 is the ID to use when talking)
  {-1, nullptr}
};

// Merlon Cutscene Text
const char * merlon_cutscene_1 =
"Ah, Mario, back already? <wait 100>I just\n"
"got back from running some\n"
"errands.\n"
"<k>\n<p>\n"
"By the way, if you ever need\n"
"anything, don't be afraid to\n"
"ask.\n"
"<k>\n<p>\n"
"I have inherited a wealth of\n"
"knowledge from my ancestors\n"
"that may prove valuable.\n"
"<k>\n<o>\n";

const char * merlon_cutscene_2 =
"Wh.. <wait 50>what?! <wait 100>Who are you?!\n"
"<wait 250>What is going on here?!\n"
"<k>\n<o>\n";

const char * merlon_cutscene_3 =
"Ah.. my sincerest apologies,\n"
"allow me to introduce myself.\n"
"<k>\n<p>\n"
"I am Dimentio, <wait 250>master magician.\n"
"<wait 350>I have been watching your\n"
"journey for a while now.\n"
"<k>\n<o>\n";

const char * merlon_cutscene_4 =
"Mmmh, yes.<wait 250> Well, as drab as\n"
"this has been, I am not here to\n"
"make idle chitchat.\n"
"<k>\n<p>\n"
"I am here to offer you an\n"
"opportunity of sorts, <wait 250>I'm sure you\n"
"will oblige.\n"
"<k>\n<p>\n"
"You see, I am under the employ\n"
"of your enemy, Count Bleck.\n"
"<wait 250>But all is not as it seems.\n"
"<k>\n<p>\n"
"I have no intention of going\n"
"along with his plan.<wait 250> Au contraire.\n"
"<k>\n<p>\n"
"What fun would an empty void\n"
"be, after all?\n"
"<k>\n<p>\n"
"No, my vision is much more\n"
"interesting.<wait 250> And your help is\n"
"an important part of it.\n"
"<k>\n<o>\n";

const char * merlon_cutscene_tippi =
 "<fairy>\n"
 "Help?<wait 250><dynamic 3> We would never help you!\n"
 "<wait 250>What on earth makes you think\n"
 "that we would?!</dynamic>\n"
 "<k>\n<o>\n";

 const char * merlon_cutscene_5 =
"Ahh, <wait 250>such vigorous words from\n"
"a flying butterfly.. <wait 250>yet they\n"
"are spoken out of ignorance.\n"
"<k>\n<p>\n"
"Not to worry, <wait 100>I am sure you will\n"
"see that this is a very <wave>enticing</wave>\n"
"\nproposition.\n"
"<k>\n<o>\n";

 const char * merlon_cutscene_6 =
"I'm afraid your assistance is so\n"
"instrumental to my plan, that I\n"
"simply cannot let you refuse.\n"
"<k>\n<p>\n"
"<wave>Don't worry, <wait 250>my little trainees..\n"
"<wait 250>so long as you do as I ask, he will \n"
"be just fine.</wave>\n\n"
"<k>\n<p>\n"
"With that, I must bid farewell.\n"
"<wait 250>You will know when it is time\n"
"for you to play your part.\n"
"<k>\n<p>\n"
"Until then.. <wait 250>ciao!\n"
"<k>\n<o>\n";


// Patch message system so that it will actually display this text
// correctly (modified from Seeky's Desert Bus mod)
static const char * (*msgSearchReal)(const char * msgName);

static void messagePatch()
{
	msgSearchReal = patch::hookFunction(spm::msgdrv::msgSearch,
		[](const char * msgName)
		{
			if (wii::string::strcmp(msgName, "hello_world") == 0)
				// Add custom message
				return hello_world;
            else if (wii::string::strcmp(msgName, "sel_item") == 0)
				// Add custom message
				return sel_item;
            else if (wii::string::strcmp(msgName, "merlon_cutscene_1") == 0)
                return merlon_cutscene_1;
            else if (wii::string::strcmp(msgName, "merlon_cutscene_2") == 0)
                return merlon_cutscene_2;
            else if (wii::string::strcmp(msgName, "merlon_cutscene_3") == 0)
                return merlon_cutscene_3;
            else if (wii::string::strcmp(msgName, "merlon_cutscene_4") == 0)
                return merlon_cutscene_4;
            else if (wii::string::strcmp(msgName, "merlon_cutscene_5") == 0)
                return merlon_cutscene_5;  
            else if (wii::string::strcmp(msgName, "merlon_cutscene_6") == 0)
                return merlon_cutscene_6;   
            else if (wii::string::strcmp(msgName, "merlon_cutscene_tippi") == 0)
                return merlon_cutscene_tippi;
			else
				return msgSearchReal(msgName);
		}
	);
}

EVT_BEGIN(merlonCutscene)
 USER_FUNC(spm::evt_mario::evt_mario_key_off, 0) // Disable Mario's movement
 //USER_FUNC(spm::evt_fairy::evt_fairy_flag_onoff, 1, 2) // Make pixl invisible; temp for now
 
 USER_FUNC(spm::evt_npc::evt_npc_set_position, PTR("mercut"), -700, 0, -259)
 //USER_FUNC(spm::evt_npc::evt_npc_flip_to, PTR("mercut"), 0)
 WAIT_MSEC(1000)
 USER_FUNC(spm::evt_npc::evt_npc_face_dir, PTR("mercut"), 1)
 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("mercut"), 1, true) // Play animation 1 (walking)
 USER_FUNC(spm::evt_npc::evt_npc_walk_to, PTR("mercut"), -400, -259, 0, 80, 0x0, 0, 0)
 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("mercut"), 22, true) // Play animation 22 (surprised jump)
 USER_FUNC(spm::evt_npc::evt_wait_anim_end, PTR("mercut"), 1)
 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("mercut"), 0, true) // Play animation 0 (idle)
 WAIT_MSEC(1000)

 USER_FUNC(spm::evt_msg::evt_msg_print, 0, PTR("merlon_cutscene_1"), 0, PTR("mercut"))
 USER_FUNC(spm::evt_msg::evt_msg_continue)
 WAIT_MSEC(500)

 USER_FUNC(spm::evt_npc::evt_npc_set_position, PTR("dimcut"), -270, 90, -260)
 USER_FUNC(spm::evt_npc::evt_npc_set_property, PTR("dimcut"), 0xe, PTR(animsDimen)) // Load animations

 USER_FUNC(spm::evt_snd::evt_snd_bgmoff_f, 0, 2000)

 USER_FUNC(spm::evt_msg::evt_msg_print, 1, PTR("Ahahaha.. <wait 250>so you have.\n<k>\n<o>\n"), 0, 0)
 USER_FUNC(spm::evt_msg::evt_msg_continue)
 USER_FUNC(spm::evt_eff::evt_eff_exclamation_question, 1, PTR("q1"), PTR("mercut"), 1, 0, 0, 0, 0, 0, 0, 0) // Spawn question bubble above Merlon
 WAIT_MSEC(500)
 USER_FUNC(spm::evt_npc::evt_npc_face_dir, PTR("mercut"), 0)
 WAIT_MSEC(500)
 USER_FUNC(spm::evt_npc::evt_npc_face_dir, PTR("mercut"), 1)
 WAIT_MSEC(1000)
 USER_FUNC(spm::evt_cam::evt_cam3d_evt_zoom_in, 1, -300, 110, 400, -300, 110, -260, 0, 11)
 USER_FUNC(spm::evt_snd::evt_snd_bgmon, 0, PTR("BGM_EVT_DMN_APPEAR1"))
 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("dimcut"), 0, true) // Play animation 0 (flying)
 USER_FUNC(spm::evt_npc::evt_npc_teleport_effect, 1, PTR("dimcut"))
 //USER_FUNC(spm::evt_npc::evt_npc_flag8_onoff, PTR("dimcut"), false, 0x40000000) // Make Dimentio visible

 USER_FUNC(spm::evt_npc::evt_npc_set_property, PTR("mercut"), 0xe, PTR(animsMerlonScared)) // Load animations
 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("mercut"), 1, true) // Play animation 1 (scared jump)
 USER_FUNC(spm::evt_npc::evt_wait_anim_end, PTR("mercut"), 1)
 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("mercut"), 0, true) // Play animation 0 (scared)

 WAIT_FRM(60)

 USER_FUNC(spm::evt_mario::evt_mario_face_dir, 90, 200)
 USER_FUNC(spm::evt_mario::evt_mario_set_pose, PTR("E_5"), 0) // Play animation E_5 (Surprised jump)
 USER_FUNC(spm::evt_npc::evt_wait_anim_end, PTR("mario"), 1)
 USER_FUNC(spm::evt_mario::evt_mario_set_pose, PTR("T_7"), 0) // Play animation T_7 (Surprised)

 USER_FUNC(spm::evt_msg::evt_msg_print, 0, PTR("merlon_cutscene_2"), 0, PTR("mercut"))
 USER_FUNC(spm::evt_msg::evt_msg_continue)

 WAIT_MSEC(500)

 USER_FUNC(spm::evt_msg::evt_msg_print, 0, PTR("merlon_cutscene_3"), 0, PTR("dimcut"))
 USER_FUNC(spm::evt_msg::evt_msg_continue)
 //USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("dimcut"), 0, true) // Play animation 0 (flying)
 WAIT_MSEC(500)
 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("mercut"), 2, true) // Play animation 2 (return from scare)
 USER_FUNC(spm::evt_npc::evt_wait_anim_end, PTR("mercut"), 1)
 USER_FUNC(spm::evt_npc::evt_npc_set_property, PTR("mercut"), 0xe, PTR(animsMerlonThinking)) // Load animations
 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("mercut"), 1, true) // Play animation 1 (start thinking)
 USER_FUNC(spm::evt_npc::evt_wait_anim_end, PTR("mercut"), 1)
 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("mercut"), 0, true) // Play animation 0 (thinking)

 USER_FUNC(spm::evt_msg::evt_msg_print, 1, PTR("Dimentio?<wait 250> Hmm.. <wait 250>I feel\nlike I've heard that name\nsomewhere before..\n<k>\n<o>\n"), 0, PTR("mercut"))
 USER_FUNC(spm::evt_msg::evt_msg_continue)
 WAIT_MSEC(500)

 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("mercut"), 2, true) // Play animation 2 (stop thinking)
 USER_FUNC(spm::evt_npc::evt_wait_anim_end, PTR("mercut"), 1)
 USER_FUNC(spm::evt_npc::evt_npc_set_property, PTR("mercut"), 0xe, PTR(animsMerlonNormal)) // Load animations
 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("mercut"), 0, true) // Play animation 0 (Idle)

 USER_FUNC(spm::evt_msg::evt_msg_print, 0, PTR("merlon_cutscene_4"), 0, PTR("dimcut"))
 USER_FUNC(spm::evt_msg::evt_msg_continue)
//USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("dimcut"), 0, true) // Play animation 0 (flying)

 USER_FUNC(spm::evt_msg::evt_msg_print, 0, PTR("merlon_cutscene_tippi"), 0, PTR("__guide__"))
 USER_FUNC(spm::evt_msg::evt_msg_continue)

 USER_FUNC(spm::evt_msg::evt_msg_print, 0, PTR("merlon_cutscene_5"), 0, PTR("dimcut"))
 USER_FUNC(spm::evt_msg::evt_msg_continue)
 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("dimcut"), 0, true) // Play animation 0 (flying)
 USER_FUNC(spm::evt_npc::evt_npc_glide_to, PTR("dimcut"), -199, 100, -259, 0, 100, 0x0, 0, 0, 0)
 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("dimcut"), 22, true) // Play animation 22 (land)
 USER_FUNC(spm::evt_npc::evt_wait_anim_end, PTR("dimcut"), 1)
 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("dimcut"), 1, true) // Play animation 1 (Idle ground)

 WAIT_MSEC(400)
 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("dimcut"), 20, true) // Play animation 3 (magic start)
 USER_FUNC(spm::evt_snd::evt_snd_sfxon_npc, PTR("SFX_EVT_DEMEN_SNAP_FINGER1"), PTR("dimcut"))
 USER_FUNC(spm::evt_mario::evt_mario_face_dir, -90, 200)
 WAIT_MSEC(100)
 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("mercut"), 22, true) // Play animation 22 (surprised jump)
 USER_FUNC(spm::evt_npc::evt_npc_teleport_effect, 0, PTR("mercut"))
 USER_FUNC(spm::evt_npc::evt_wait_anim_end, PTR("dimcut"), 1)
 USER_FUNC(spm::evt_eff::evt_eff_exclamation_question, 0, PTR("q2"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
 WAIT_MSEC(600)
 USER_FUNC(spm::evt_mario::evt_mario_set_pose, PTR("T_8"), 0) // Play animation T_8 (Mad)
 USER_FUNC(spm::evt_mario::evt_mario_face_dir, 90, 200)
 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("dimcut"), 1, true) // Play animation 1 (Idle ground)
 USER_FUNC(spm::evt_npc::evt_wait_anim_end, PTR("dimcut"), 1)
 
 USER_FUNC(spm::evt_msg::evt_msg_print, 1, PTR("<fairy>\nM-<wait 250>Merlon!\n<k>\n<o>\n"), 0, PTR("__guide__"))
 USER_FUNC(spm::evt_msg::evt_msg_continue)

 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("dimcut"), 27, true) // Play animation 27 (fly)
 USER_FUNC(spm::evt_npc::evt_wait_anim_end, PTR("dimcut"), 1)
 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("dimcut"), 0, true) // Play animation 0 (flying)
 USER_FUNC(spm::evt_npc::evt_npc_glide_to, PTR("dimcut"), -140, 0, -259, 0, 100, 20, 0, 0, 0)
 USER_FUNC(spm::evt_npc::evt_npc_face_dir, PTR("dimcut"), 0)

//  USER_FUNC(spm::evt_npc::evt_npc_glide_to, PTR("dimcut"), -400, 80, -259, 0, 100, 20, 0, 0, 0)
//  USER_FUNC(spm::evt_npc::evt_npc_face_dir, PTR("dimcut"), 1) // Make Dimentio face right
//  USER_FUNC(spm::evt_mario::evt_mario_face_dir, -90, 200)

 WAIT_MSEC(500)
 USER_FUNC(spm::evt_msg::evt_msg_print, 0, PTR("merlon_cutscene_6"), 0, PTR("dimcut"))
 USER_FUNC(spm::evt_msg::evt_msg_continue)

 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("dimcut"), 24, true) // Play animation 6 (raise hands start)
 WAIT_MSEC(500)
 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("dimcut"), 26, true) // Play animation 7 (raise hands cast)
 USER_FUNC(spm::evt_snd::evt_snd_sfxon_npc, PTR("SFX_EVT_DEMEN_MAGIC1"), PTR("dimcut"))
 WAIT_MSEC(500)
 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("dimcut"), 0, true) // Play animation 0 (flying)
 USER_FUNC(spm::evt_npc::evt_npc_teleport_effect, 0, PTR("dimcut"))

 USER_FUNC(spm::evt_mario::evt_mario_set_pose, PTR("E_5"), 0) // Play animation E_5 (Surprised jump)
 USER_FUNC(spm::evt_npc::evt_wait_anim_end, PTR("mario"), 1)
 USER_FUNC(spm::evt_mario::evt_mario_set_pose, PTR("T_7"), 0) // Play animation T_7 (Surprised)
 WAIT_FRM(60)
 USER_FUNC(spm::evt_snd::evt_snd_bgmoff_f, 0, 1500)
 USER_FUNC(spm::evt_npc::evt_npc_delete, PTR("dimcut")) // Delete the NPC
 USER_FUNC(spm::evt_npc::evt_npc_delete, PTR("mercut")) // Delete the NPC
 
 USER_FUNC(spm::evt_msg::evt_msg_print, 1, PTR("<fairy>\nMerlon.. <wait 250>what did that creep\ndo to him?!<wait 250> We have to find him!\n<k>\n<p><fairy>\nCome on, Mario, if he's\nworking for Count Bleck, we're\nbound to run into him.\n<k>\n<o>\n"), 0, PTR("__guide__"))
 USER_FUNC(spm::evt_msg::evt_msg_continue)
 
 WAIT_MSEC(1000)
 USER_FUNC(spm::evt_fade::evt_set_transition, 26, 25)
 USER_FUNC(spm::evt_seq::evt_seq_set_seq, spm::seqdrv::SEQ_MAPCHANGE, PTR("bos_01"), 0)
// USER_FUNC(spm::evt_fade::evt_fade_end_wait, -1)

RETURN()
EVT_END()

EVT_BEGIN(post_dimen_cutscene)
    USER_FUNC(spm::evt_npc::evt_npc_set_property, PTR("dimbos"), 0xe, PTR(animsDimenDead))
    USER_FUNC(spm::evt_npc::evt_npc_flag8_onoff, PTR("dimbos"), 1, 8) // Make Dimentio fall to the ground
    USER_FUNC(spm::evt_npc::func_801072a4, PTR("dimbos"))
    USER_FUNC(spm::evt_npc::evt_npc_flag8_onoff, PTR("dimbos"), 0, 8)
    USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("dimbos"), 1, true)
    USER_FUNC(spm::evt_npc::evt_npc_face_mario, PTR("dimbos"))
    USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("dimbos"), 20, true)
    USER_FUNC(spm::evt_msg::evt_msg_print, 1, PTR("bro you fucked me goddamn\n<k>\n<o>\n"), 0, PTR("dimbos"))
    USER_FUNC(spm::evt_msg::evt_msg_continue)
    
    WAIT_MSEC(1000)
    USER_FUNC(spm::evt_fade::evt_set_transition, 26, 25)
    USER_FUNC(spm::evt_seq::evt_seq_set_seq, spm::seqdrv::SEQ_MAPCHANGE, PTR("mac_02"), 0)
    RETURN()
EVT_END()

EVT_BEGIN(post_dimen_setup)
    USER_FUNC(spm::evt_mario::evt_mario_key_off, 0)
    USER_FUNC(spm::evt_snd::evt_snd_bgmoff_f, 0, 2000)
    USER_FUNC(spm::evt_npc::evt_npc_id_to_name, 123 + 0x10000000, LW(10))
    USER_FUNC(spm::evt_npc::evt_npc_get_position, LW(10), LW(0), LW(1), LW(2))
    USER_FUNC(spm::evt_npc::evt_npc_set_position, PTR("dimbos"), LW(0), LW(1), LW(2))
    RUN_EVT(post_dimen_cutscene)
    USER_FUNC(spm::evt_npc::evt_npc_delete, LW(10)) // Delete the NPC
    RETURN()
EVT_END()

EVT_BEGIN(bos01_cutscene)
 USER_FUNC(spm::evt_mario::evt_mario_face_dir, 0, 200)
 USER_FUNC(spm::evt_mario::evt_mario_key_off, 0)
 USER_FUNC(spm::evt_mario::evt_mario_flag8_onoff, 1, 1) // Make Mario invisible
 USER_FUNC(spm::evt_guide::evt_guide_flag2_onoff, 1, 1) // Make Tippi invisible
 USER_FUNC(spm::evt_fairy::evt_fairy_flag_onoff, 1, 2) // Make pixl invisible
 USER_FUNC(spm::evt_fade::evt_seq_wait, 2)

 USER_FUNC(spm::evt_cam::evt_cam3d_evt_zoom_in, 1, 50, 101, 644, 0, 101, 0, 0, 11)

 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("merbos"), 0, true) // Play animation 0 (scared)
 USER_FUNC(spm::evt_snd::evt_snd_bgmon, 0, PTR("BGM_EVT_DMN_APPEAR1"))
 USER_FUNC(spm::evt_fade::evt_fade_end_wait, -1)
 WAIT_MSEC(1000)

 USER_FUNC(spm::evt_npc::evt_npc_teleport_effect, 1, PTR("merbos"))
 //USER_FUNC(spm::evt_npc::evt_npc_flag8_onoff, PTR("merbos"), false, 0x40000000) // Make Merlon visible
 WAIT_FRM(60)
 USER_FUNC(spm::evt_msg::evt_msg_print, 1, PTR("W-what?<wait 250> Where am I?\n<k>\n<o>\n"), 0, PTR("merbos"))
 USER_FUNC(spm::evt_msg::evt_msg_continue)

 USER_FUNC(spm::evt_npc::evt_npc_teleport_effect, 1, PTR("dimbos"))
 //USER_FUNC(spm::evt_npc::evt_npc_flag8_onoff, PTR("dimbos"), false, 0x40000000)
 WAIT_FRM(60)

 USER_FUNC(spm::evt_msg::evt_msg_print, 1, PTR("Ahh, <wait 150>welcome.. <wait 250>this is Dimension\nD, a dimension of my own \ncreation. <wait 250>Isn't it wonderful?\n<k>\n<o>"), 0, PTR("dimbos"))
 USER_FUNC(spm::evt_msg::evt_msg_continue)

 USER_FUNC(spm::evt_msg::evt_msg_print, 1, PTR("Of your own creation?! <wait 250>But only\nthe ancients possessed that\nkind of power!<k>\n<o>\n"), 0, PTR("merbos"))
 USER_FUNC(spm::evt_msg::evt_msg_continue)

 USER_FUNC(spm::evt_cam::evt_cam3d_evt_zoom_in, 1, 50, 101, 644, 30, 101, 100, 0, 11)

 USER_FUNC(spm::evt_msg::evt_msg_print, 1, PTR("Mmh, I do find pointless\nconversation quite boring.\n<k>\n<p>\nNot to worry, I know just the\nthing to spice things up.\n<k>\n<p>How about a little.. <wait 250>magic?\n<k>\n<o>"), 0, PTR("dimbos"))
 USER_FUNC(spm::evt_msg::evt_msg_continue)

 USER_FUNC(spm::evt_cam::evt_cam3d_evt_zoom_in, 1, 50, 101, 644, 0, 101, 0, 0, 11)

 USER_FUNC(spm::evt_snd::evt_snd_sfxon_npc, PTR("SFX_EVT_DEMEN_MAGIC1"), PTR("dimbos"))
 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("dimbos"), 24, true) // Play animation 24 (raise hands start)
 USER_FUNC(spm::evt_npc::evt_wait_anim_end, PTR("dimbos"), 1)
 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("dimbos"), 25, true) // Play animation 25 (raise hands start)
 USER_FUNC(spm::evt_npc::evt_wait_anim_end, PTR("dimbos"), 1)
 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("dimbos"), 26, true) // Play animation 26 (raise hands cast) 
 USER_FUNC(spm::evt_npc::evt_wait_anim_end, PTR("dimbos"), 1)
 USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("dimbos"), 0, true) // Play animation 0 (flying)
 WAIT_MSEC(200)

 USER_FUNC(spm::evt_npc::evt_npc_teleport_effect, 1, PTR("dimbos1"))
 WAIT_FRM(20)

 USER_FUNC(spm::evt_npc::evt_npc_face_dir, PTR("merbos"), 0) // Make Merlon face left
 USER_FUNC(spm::evt_npc::evt_npc_teleport_effect, 1, PTR("dimbos2"))
 WAIT_FRM(20)
 USER_FUNC(spm::evt_npc::evt_npc_face_dir, PTR("merbos"), 1) // Make Merlon face right
 WAIT_FRM(40)
 
 USER_FUNC(spm::evt_npc::evt_npc_face_dir, PTR("merbos"), 0) // Make Merlon face left
 USER_FUNC(spm::evt_msg::evt_msg_print, 1, PTR("Don't worry, <wait 250>I'm a natural\nperformer. <wait 250>I would never dream\nof hurting my audience.\n<k>\n<o>"), 0, PTR("dimbos1"))
 
 USER_FUNC(spm::evt_msg::evt_msg_continue)
 //WAIT_FRM(40)
 USER_FUNC(spm::evt_npc::evt_npc_face_dir, PTR("merbos"), 1) // Make Merlon face right
 USER_FUNC(spm::evt_msg::evt_msg_print, 1, PTR("Besides, I require your services..\n<wait 250>I hope you won't mind a little\ninterrogation.<k>\n<o>"), 0, PTR("dimbos2"))

 USER_FUNC(spm::evt_msg::evt_msg_continue)

 USER_FUNC(spm::evt_npc::evt_npc_teleport_effect, 0, PTR("dimbos1"))
 WAIT_FRM(20)
 
 USER_FUNC(spm::evt_npc::evt_npc_teleport_effect, 0, PTR("dimbos2"))
 WAIT_FRM(60)

 USER_FUNC(spm::evt_npc::evt_npc_glide_to, PTR("dimbos"), -145, 70, 0, 0, 100, FLOAT(-25.0), 0xd, 0, 0)
 USER_FUNC(spm::evt_npc::evt_npc_face_dir, PTR("dimbos"), 1) // Make Dimentio face right
 WAIT_FRM(30)
 USER_FUNC(spm::evt_npc::evt_npc_face_dir, PTR("merbos"), 0) // Make Merlon face left
 USER_FUNC(spm::evt_msg::evt_msg_print, 1, PTR("Ahh... <wait 50><wave>I think your friends are\nhere!</wave><k>\n<o>"), 0, PTR("dimbos"))
 USER_FUNC(spm::evt_msg::evt_msg_continue)

 USER_FUNC(spm::evt_cam::evt_cam3d_evt_zoom_in, 1, 50, 101, 644, 0, 101, 0, 0, 11)
 USER_FUNC(spm::evt_mario::evt_mario_set_pos, 30, 0, 0)
 USER_FUNC(spm::evt_mario::evt_mario_flag0_onoff, 1, 1)
 USER_FUNC(spm::evt_mario::evt_mario_flag8_onoff, 0, 1)
 USER_FUNC(spm::evt_mario::evt_mario_face_dir, -90, 200)
 WAIT_FRM(40)
 USER_FUNC(spm::evt_npc::evt_npc_teleport_effect, 1, PTR("mario"))
 USER_FUNC(spm::evt_npc::evt_npc_face_dir, PTR("merbos"), 1) // Make Merlon face right

 USER_FUNC(spm::evt_guide::evt_guide_flag2_onoff, 0, 1)
 USER_FUNC(spm::evt_npc::evt_npc_teleport_effect, 1, PTR("guide"))

 USER_FUNC(spm::evt_fairy::evt_fairy_flag_onoff, 0, 2)
 USER_FUNC(spm::evt_npc::evt_npc_teleport_effect, 1, PTR("fairy"))

 USER_FUNC(spm::evt_mario::evt_mario_set_pose, PTR("T_8"), 0) // Play animation T_8 (Mad)

 USER_FUNC(spm::evt_msg::evt_msg_print, 1, PTR("<fairy>There he is! <wait 50>Dimentio!\n<k>\n<p>Explain what is happening this\ninstant!<k>\n<o>"), 0, PTR("__guide__"))
 USER_FUNC(spm::evt_msg::evt_msg_continue)
 USER_FUNC(spm::evt_npc::evt_npc_face_dir, PTR("merbos"), 0) // Make Merlon face left

 // Notes: Have Dimentio 'interrogate' Merlon, then make him hover in the air and 'corrupt' him like Merlee
 USER_FUNC(spm::evt_msg::evt_msg_print, 1, PTR("Ahh, don't you know the\ngolden rule? <wait 250>A magician never\nexplains his secrets.\n<k>\n<p>Though I suppose I do owe you\nan explanation, of sorts.\n<k>\n<o>"), 0, PTR("dimbos"))
 USER_FUNC(spm::evt_msg::evt_msg_continue)

 USER_FUNC(spm::evt_msg::evt_msg_print, 1, PTR("If you can defeat me, that is!\n<k>\n<p>Now we shall duel, like two\nseagulls fighting over the last\nMcDonalds french fry!\n<k>\n<o>"), 0, PTR("dimbos"))
 USER_FUNC(spm::evt_msg::evt_msg_continue)
 USER_FUNC(spm::evt_npc::evt_npc_teleport_effect, 0, PTR("merbos"))

 USER_FUNC(spm::evt_snd::evt_snd_bgmoff_f, 0, 2000)
 USER_FUNC(spm::evt_cam::evt_reset_camera, 500, 11)
 WAIT_MSEC(1000)
 USER_FUNC(spm::evt_npc::evt_npc_get_position, PTR("dimbos"), LW(0), LW(1), LW(2))
 USER_FUNC(spm::evt_npc::evt_npc_set_position, PTR("dimbos"), 0, -1000, 0)
 USER_FUNC(spm::evt_npc::evt_npc_entry_from_template, 123, DIMENTIO_TEMPLATE_ID, LW(0), LW(1), LW(2), LW(10), EVT_NULLPTR)
 USER_FUNC(spm::evt_npc::evt_npc_face_dir, LW(10), 1) // Make Boss Dimentio face right
 USER_FUNC(spm::evt_npc::evt_npc_set_unitwork, LW(10), 8, PTR(post_dimen_setup))
 USER_FUNC(spm::evt_snd::evt_snd_bgmon, 0, PTR("BGM_BTL_DIMEN1"))
 //USER_FUNC(spm::evt_npc::evt_npc_set_rgba, PTR("merbos"), 255, 255, 255, 0)
 //USER_FUNC(spm::evt_npc::evt_npc_set_disp_func_2, PTR("merbos"), PTR(spm::evt_npc::evt_disp_func_static), 0)
 USER_FUNC(spm::evt_mario::evt_mario_set_pose, PTR("S_1"), 0) 
 USER_FUNC(spm::evt_mario::evt_mario_key_on)

 
RETURN()
EVT_END()

// Because the evt_npc_teleport_effect function has lower level flags than the item menu, 
// and also because it doesn't unpause the game automatically, it needs to be run through a fowarder event.
// RUN_EVT runs another event asynchronously, so this script just tells the game to run the script immediately
// but also unpauses the game which would otherwise not occur via the item menu.

EVT_BEGIN(forwarder_evt2)
 //   SET(LW(50), 0)
    RUN_EVT(merlonCutscene)
    //RUN_EVT(mahojin)
    RETURN()
EVT_END()

EVT_BEGIN(forwarder_evt3)
    // Make shit work
    RUN_CHILD_EVT(spm::evt_door::door_init_evt)
    //Make everyone invisible
    USER_FUNC(spm::evt_mario::evt_mario_flag0_onoff, 0, 1)
    USER_FUNC(spm::evt_mario::evt_mario_set_scale, 0, 1, 1)
    USER_FUNC(spm::evt_guide::evt_guide_flag2_onoff, 1, 1)
    USER_FUNC(spm::evt_fairy::evt_fairy_flag_onoff, 1, 2)

    // Spawn Merlon
    SPAWN_CHARACTER("merbos", "n_machi_dearle", animsMerlonScared)
    USER_FUNC(spm::evt_npc::evt_npc_set_position, PTR("merbos"), -50, 0, 0)
    USER_FUNC(spm::evt_npc::evt_npc_set_scale, PTR("merbos"), 0, 1, 1) // Make Merlon invisible
    USER_FUNC(spm::evt_npc::evt_npc_face_dir, PTR("merbos"), 1) // Make Merlon face right
    //USER_FUNC(spm::evt_npc::evt_npc_face_dir, PTR("merbos"), 1)

    // Spawn Dimentio
    SPAWN_CHARACTER("dimbos", "e_dmen", animsDimen)
    USER_FUNC(spm::evt_npc::evt_npc_set_scale, PTR("dimbos"), 0, 1, 1) // Make Dimentio invisible
    USER_FUNC(spm::evt_npc::evt_npc_set_position, PTR("dimbos"), 50, 35, 0)
    USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("dimbos"), 0, true) // Play animation 0 (flying)

    // Spawn fake Dimentio Clone 1
    SPAWN_CHARACTER("dimbos1", "e_dmen", animsDimen)
    USER_FUNC(spm::evt_npc::evt_npc_set_scale, PTR("dimbos1"), 0, 1, 1) // Make Dimentio invisible
    USER_FUNC(spm::evt_npc::evt_npc_set_position, PTR("dimbos1"), -120, 50, 0)
    USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("dimbos1"), 0, true) // Play animation 0 (flying)
    USER_FUNC(spm::evt_npc::evt_npc_face_dir, PTR("dimbos1"), 1) // Make fake Dimentio face right

    // Spawn fake Dimentio Clone 2
    SPAWN_CHARACTER("dimbos2", "e_dmen", animsDimen)
    USER_FUNC(spm::evt_npc::evt_npc_set_anim, PTR("dimbos2"), 0, true) // Play animation 0 (flying)
    USER_FUNC(spm::evt_npc::evt_npc_set_scale, PTR("dimbos2"), 0, 1, 1) // Make Dimentio invisible
    USER_FUNC(spm::evt_npc::evt_npc_set_position, PTR("dimbos2"), 120, 75, 0)
    RUN_EVT(bos01_cutscene)
    RETURN()
EVT_END()

EVT_BEGIN(run_cutscene)
    DO(0)
        USER_FUNC(spm::evt_mario::evt_mario_get_pos, LW(1), LW(2), LW(3))
        IF_SMALL_EQUAL(LW(1), -280)
            IF_SMALL_EQUAL(LW(2), 1000)
                RUN_EVT(forwarder_evt2)
                DO_BREAK()
            END_IF()
        END_IF()
        WAIT_FRM(1)
    WHILE()
    RETURN()
EVT_END()

EVT_BEGIN(mac_02_custom_init)
    // Spawn Merlon
    USER_FUNC(spm::evt_npc::evt_npc_entry, PTR("mercut"), PTR("n_machi_dearle"), 0) // Create an NPC with the specified name, model, and index
    USER_FUNC(spm::evt_npc::evt_npc_set_property, PTR("mercut"), spm::evt_npc::NPCProperty::ANIMS, PTR(animsMerlonNormal)) // Load animations
    USER_FUNC(spm::evt_npc::evt_npc_set_position, PTR("mercut"), 0, -1000, 0)
    USER_FUNC(spm::evt_npc::evt_npc_add_flip_part, PTR("mercut"))
    WAIT_MSEC(1000)
    //USER_FUNC(spm::evt_npc::evt_npc_flip_to, PTR("mercut"), 1)
    USER_FUNC(spm::evt_npc::evt_npc_finish_flip_instant, PTR("mercut"))

    // Spawn Dimentio
    USER_FUNC(spm::evt_npc::evt_npc_entry, PTR("dimcut"), PTR("e_dmen"), 0) // Create an NPC with the specified name, model, and index
    //USER_FUNC(spm::evt_npc::evt_npc_flag8_onoff, PTR("dimcut"), true, 0x40000000) // Make Dimentio invisible
    USER_FUNC(spm::evt_npc::evt_npc_set_position, PTR("dimcut"), 0, -1000, 0)
    USER_FUNC(spm::evt_npc::evt_npc_set_scale, PTR("dimcut"), 0, 1, 1) // Make Dimentio invisible
    USER_FUNC(spm::evt_npc::evt_npc_set_property, PTR("dimcut"), 0xe, PTR(animsDimen)) // Load animations

    // Run original script
    RUN_CHILD_EVT(spm::mac_02::mac_02_init_evt)
    // Custom script here
    RUN_EVT(run_cutscene)

    RETURN()
EVT_END()

EVT_BEGIN(nullevt)

    RETURN()
EVT_END()

void main()
{
    wii::OSError::OSReport("SPM Rel Loader: the mod has ran!\n");
    wii::OSError::OSReport("%x\n",spm::mapdata::mapDataPtr("bos_01")->script);

    titleScreenCustomTextPatch();
    messagePatch();
    exceptionPatch(); // Seeky's exception handler from Practise Codes
    romfontExpand(); // Font expansion patch from Practise Codes
    // Seq value is currently 019
//    while(spm::seqdrv::seqGetSeq() == 3)
//    {
//        if(spm::evtmgr_cmd::evtGetValue(nullptr, LW(50)) == 0)
//        {
//            spm::evtmgr_cmd::evtSetValue(nullptr, GSW(0), 412); // Set it so that the cutscene in BOS_01 doesn't play
//        }
//        if(spm::evtmgr_cmd::evtGetValue(nullptr, LW(50)) == 1)
//        {
//            spm::evtmgr_cmd::evtSetValue(nullptr, GSW(0), 19); // Set it back to 19
//            spm::evtmgr_cmd::evtSetValue(nullptr, LW(50), 0);
//        }
//    }
    
    spm::mapdata::MapData * mac_02_md = spm::mapdata::mapDataPtr("mac_02"); // Get the mapdata of mac_02
    mac_02_md->script = mac_02_custom_init; // Replace the init script with ours

    spm::mapdata::MapData * bos_01_md = spm::mapdata::mapDataPtr("bos_01"); // Get the mapdata of bos_01
    bos_01_md->script = forwarder_evt3;

    // Modify unused Ch5 Dimentio's HP
    spm::npcdrv::npcTribes[spm::npcdrv::npcEnemyTemplates[DIMENTIO_TEMPLATE_ID].tribeId].maxHp = 150;
    spm::npcdrv::npcTribes[spm::npcdrv::npcEnemyTemplates[DIMENTIO_TEMPLATE_ID].tribeId].attack = 8;

    // Modify unused Ch5 Dimentio's magic's attack
#ifdef SPM_US0
    // it's a number in an evt script so this is the most convenient way of doing it and i'm too lazy to port it
    *(int*)0x80419c74 = 8;
#endif

    //spm::mac_02::merlonDoorDesc.npcNameList = customNameList; // Actually replace merlon's door script with ours

    //writeBranch(spm::mac_02::evt_mac_merlon_door_npcs_override, 0, doorStub); // Replace Merlon's door script with the door stub
}

}
