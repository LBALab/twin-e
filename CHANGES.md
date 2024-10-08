# TwinEngine Changes
## A Little Big Adventure engine changes
- List of all changes during public version releases

## 0.3.0 - 2024-10-05: [NOT RELEASED]
	- Code refactoring.

	- [FIXED] CPU Usage and wrong game FPS

## 0.2.1 - 2015-04-25:
	- Code refactoring.
	
	- [FIXED] (issue #4)  Incorrect inventory 3D models drawing
	- [FIXED] (issue #12) Script condition triggered incorrectly.
	- [FIXED] (issue #13) Running and colliding with actors teleports you through them

## 0.2.0 - 2014-12-30:
	- Prequengine and TwinEngine merged into a single project.
	- Code refactoring.
	- Issue tracking was resetted.
	
	- [FIXED] (issue #2) Correct behavior of F1..F4 keys while pressing.
	- [FIXED] (issue #3) F4 key was making Twinsen throw Magicball.
	- [FIXED] (issue #5) Saber and Pingin inventory items wasn't working correctly.
	- [FIXED] (issue #8) Some keyboard movement code was incorrectly reversed. Some actions can now be done together.
	- [FIXED] Recenter screen pressed key that wasn't working together with other keys (for example arrow keys while running)
	
	- [ADDED] Support for original XMidi format.
	- [ADDED] Config flag to automatic detect midi file, midi_mi.hqr or midi_mi_win.hqr.

## 0.1.3 - 2013-04-11:
	- [FIXED] (issue #37) Multiple voice samples were been playing at the same time.
	- [FIXED] (issue #41) Playing samples according with actors position in scenes.
	- [FIXED] (issue #34) Incorrect text display of special characters.
	- [FIXED] (issue #35) Voices don't stop playing when you skip a text dialog.
	- [FIXED] (issue #39) Key sprite overlay with incorrect Y position when open doors above the ground
	- [FIXED] Previous added palette related script commands added.
	
	- [ADDED] Missing palette related script commands (responsible with palette fadings).
	- [ADDED] Config flag to toggle Twinsen's Wall Collision

## 0.1.2 - 2012-12-30:
	- [FIXED] issue with null animation pointers.
	- [FIXED] issue with actors shadow size.
	- [FIXED] issue with magicball shadow crashing the game.
	- [FIXED] (issue #18) with ANGLE move script.
	- [FIXED] (issue #19) with DISTANCE conditions command.
	- [FIXED] issue with fade out music which prevents all sounds to be played.
	- [FIXED] issue with SAMPLE_ALWAYS move script command.
	- [FIXED] issue with inventory item overlay rotation speed.
	- [FIXED] issue with REPEAT_SAMPLE move script command.
	- [FIXED] issue with music and samples not stopping while playing FLA movies.
	- [FIXED] issue with the CONE_VIEW script command. It was causing an incorrect behavior when Twinsen was near a Guard.

	- [ADDED] Meca-pinguin support.
	- [ADDED] Extra explosions. Meca-pinguins can now explode when they "die".
	- [ADDED] Magicball aiming at key.
	- [ADDED] Extra shadows.	
	- [ADDED] hability to stop a single sample.
	- [ADDED] Pause and resume samples.	
	- [ADDED] Scene music only plays once as original game.
	- [ADDED] Voices are fully supported.
	- [ADDED] Dialog text buble.
	- [ADDED] Palette related script commands (responsible with palette fadings).

## 0.1.1 - 2010-02-13:
	- [FIXED] (issue #8) Application window close button didn't quit the game.
	- [FIXED] (issue #9) Menu "Continue game" was always playing intro screens. It nows only plays if we are at Chapter 1 (Still at Prison in Citaldel Island).
	- [FIXED] (issue #10) Wrong bar size of Magic Level (blue bar).
	- [FIXED] (issue #11) Wrong color and position for Kashes range count overlay.
	- [FIXED] (issue #7) Incorrect 3D characters rendering while in certain scenes. Was made an workaround for now, it must be fixed like original.
	- [FIXED] (issue #12) All FLA movies requested to play by Life Script wasn't working.
	- [FIXED] (issue #14) Prevent bug that shows infinit clover leafs.
	- [FIXED] (issue #17) Crital issue at Clear Water Lake scene which blocks the player to processed in game. The game was freezing due to inventory item set.
	- [FIXED] (issue #15) Text empty lines was drawing with previous displayed text.
	- [FIXED] (issue #20) Reset menu font paremeters.
	- [FIXED] (issue #24) There was a problem in delay function that was incorrectly ask for skipped delays. This adds the issue that blocks skip intros, found objects and text correctly.
	- [FIXED] (issue #21,22,23) Animation initialization didn't work for ids higher than 127.
	- [FIXED] (issue #27) Music was always stopping if Hero die, and didn't restart.

	- [ADDED] Holomap set and clear locations funcionality. It allows to save game properly and to be used in other engines like LbaWin.
	- [ADDED] (issue #4) Hero drowning while standing on water bricks.
	- [ADDED] (issue #16) Gameover 3D model when Hero dies and gameover.
	- [ADDED] Ovelay inventory items at screen corner (ex. when we found holomap or a new locations is set).
	- [ADDED] F1..F4 keys to change Hero behavior.

## 0.1.0 - 2010-01-25:
	- First public release
