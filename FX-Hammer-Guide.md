# FX-Hammer guide
This is my attempt at a small guide on using the (pretty obtuse) FX-Hammer editor, as of now it's the only reliable editor for using CBT-FX unless you wanna write binary C data manually.
* [Quick Game Boy sound recap](#quick-game-boy-sound-recap)
* [Getting Started](#getting-started)
* [Controls](#controls)
* [Priority system](#priority-system)
* [Exporting to CBT-FX](#exporting-to-cbt-fx)
* [Adding the SFX to your game](#adding-the-sfx-to-your-game)

## Quick driver / Sound hardware explanation
Before we begin, you should be aware of what the Game Boy and this driver are capable of sound-wise, more-so to get your mind in the right place, if you already have an understanding of the sound hardware, you can skip this.

CBT-FX uses the second duty channel and the noise channel, it gives you more variety in the sounds you can make and also doesn't boggle the more used channels (The first duty channel tends to always be in use because of it's hardware sweep function alongside the wave channel cause of it's waveform features).

This means that you can play two types of sound at a time:
- 4 types of square waves on channel 2 (12.5% Duty, 25% Duty, 50% Duty and 75% Duty) with a note range from C-1 to B-5.

- 2 Types of noise on channel 4, "15-bit" noise (What you expect from the noise channel, good for explosions and stuff) and "7-bit" noise (More tone-like, not the most varied though).

CBT-FX also does not make use of the Game Boy's hardware envelope system, every step is retriggered, this is to make editing easier and the data more compact, but it also means that the playback can be a bit crackle-ee due to the duty retriggering going on, there is a way around this as far as I know (The so-called "zombie mode"), but I've heard it's effectiveness varies from Game Boy model to Game Boy model, so this driver does not use it.

Also, if you make an SFX that uses all 32 rows on FX-Hammer, you'll notice that it loops back to the start, I don't know if this behavior is a bug from FX-Hammer's playback or a feature of it's driver, but it is not replicated in CBT-FX.

**I also recommend being familiar with things such as binary numbers and hexadecimal numbers, it's not necessary but it'll help you understand this better since most values used in the editor are hexadecimal.**

## Getting Started
First and foremost, get FX-Hammer from [here](https://www.pouet.net/prod.php?which=17337), you'll see a lot of files since this release includes it's accompanying music driver and other tools, but it's the `hammered.gb` file inside the `SFX` folder. This is (as far as I know) the latest release of the editor, sadly the source is supposedly lost so don't expect a new release any time soon. 

The ROM should work on any Game Boy emulator or real hardware using a flash cart, personally I recommend running it in GBC mode for the pretty colors, but any model should work, just make sure that you can access the save data (Normally called `hammered.sav` or `hammered.srm`) later, also, **make sure to run the ROM alongside the included .sav file**.


![First thing](/img/first_thing.bmp)

The first thing you'll see after you boot it up is this, this is the editor itself, it consists of 7 rows of differently labeled data, don't worry, we'll quickly go through them from left to right:
* Step number: Number of the current row, FX-Hammer has 32 rows per effect.
* Step length: Number of frames that the current frame will last.
* Channel 2 panning and volume: It consists of two parts, the letter to the left is the panning (L = Left panned, R = Right panned, M = Center panned), and the one to the right is the volume (0 = No sound, F = Full volume).
* Channel 2 duty: Duty value for channel 2 (0 = 12.5%, 1 = 25%, 2 = 50%, 3 = 75%).
* Channel 2 tone: The note that channel 2 will play, it ranges from C-1 (65 hz) to B-5 (3951 hz).
* Channel 4 Panning and volume: Same as the one for channel 2 (See above) but for channel 4.
* Channel 4 polynomial counter: The "frequency" byte for channel 4, I say this in commas because it also has some flags that are set in certain bits, I highly recommend reading up on [this](https://gbdev.io/pandocs/Sound_Controller.html#ff22---nr43---channel-4-polynomial-counter-rw) to get a better understanding, otherwise, it's more of a "Fiddle with it until you find something you like" thing.

You'll also see a few things on the bottom blue bar, most of these are labels for the rows but there are two important values:
* FX: Number of the SFX, each save file has 60 SFX slots (0x00 - 0x3B).
* PRI: Priority number (0-15), we'll dig into this later.

The default save file comes with 10 effects already written, you can play around with them to get something you like or clear them to have more space.

![SaveBug](/img/save_bug.bmp)

If the first thing you see after booting it up is something like this instead, **don't worry!** It probably just means you loaded the ROM without the save file included alongside the editor, meaning your emulator of choice made a blank one which is read as garbage data, just delete the .sav file your emulator created and replace it with the one that came with the editor.

## Controls

Controls can be a bit obtuse but after a while you learn em:
	- Move cursor: D-Pad
	- Change current SFX: B + Up / B + Down (Increase, decrease).
	- Edit value: A + Up / A  + Down (Increase, decrease).
	- Edit panning: A + Left / A + Right (Pan left and right).
	- Play SFX: Start.
	- Play SFX + Follow step cursor: Start + Down.
	- Play SFX at half speed: Start + Up.
	- Save SFX to clipboard: Select + Up.
	- Paste SFX from clipboard: Select + Start.
	- Edit priority: B + Left / B + Right (Increase, decrease).

## Priority system
The priority value lets you manage which sounds play over each other, the higher the value, the higher it's priority is, meaning that while it's playing, SFX requests with lesser priority values will be ignored, effects with the same priority will be played normally, this is good if you say, don't want your beautifully crafted explosion effect to be ignored over a little shooting effect.

## Exporting to CBT-FX
After you're done editing your SFX, copy the `hammered.sav` file to this repository's main folder, or just anywhere where it's on the same folder as the `hammer2cbt.py` script, you will need Python 3 to use it. This takes a few arguments and options:
### Positional arguments (Required):
* `fxsav`: Filename for the FX-Hammer save file.
* `fxnum`: SFX index to export (The `FX` value listed in the editor before).
* `out`: Output folder for the SFX files, should be named as `FOLDER/`.
### Optional arguments:
* `--help`: Shows the help dialogue.
* `--fxammo FXAMMO`: Allows you to export multiple SFX at once, starting from your SFX index and ending on your SFX index + this value, not using this means it'll just export the SFX index you mentioned.
* `--fxnamelist FXNAMELIST`: Lets you specify the name of your SFX from a text file, useful if you're exporting multiple SFX at once, each name should be split by a line break on the text file.

## Adding the SFX to your game
If you input your values right, you'll see a few new files, each SFX will have a .C and a .H file, to play it with CBT-FX, include the .H file in your desired .C files and call the `CBTFX_PLAY_YOUR-SFX-NAME;` macro defined in the SFX's .H file.

...And you're done! (ﾉ◕ヮ◕)ﾉ*:･ﾟ✧