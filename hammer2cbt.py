# Frequency table
CH2freqs = [44,156,262,363,457,547,631,710,786,854,923,986,1046,1102,1155,1205,1253,1297,1339,1379,1417,1452,1486,1517,1546,1575,1602,1627,1650,1673,1694,1714,1732,1750,1767,1783,1798,1812,1825,1837,1849,1860,1871,1881,1890,1899,1907,1915,1923,1930,1936,1943,1949,1954,1959,1964,1969,1974,1978,1982,1985,1988,1992,1995,1998,2001,2004,2006,2009,2011,2013,2015]

"""
FX HAMMER DATA LOCATIONS:

Header:
SFX Priority:	0x200 + fxnum
CH Used:		0x300 + fxnum (0x30 = CH2 used, 0x03 = CH4 used)

SFX data:		0x400 + (fxnum * 256)
- Frame duration	
- CH2 pan		NR51 format
- CH2 vol		NR22 format
- CH2 duty		NR21 format
- CH2 note		Note = (this - 0x40) / 2
- CH4 pan		NR51 format
- CH4 vol		NR42 format
- CH4 freq		NR43 format

WARNING: FX Hammer pan values are inverted
"""

# Taken from some website, ikik
def swapbits(n, p1, p2):
    bit1 = (n >> p1) & 1
    bit2 = (n >> p2) & 1
    x = (bit1 ^ bit2)
    x = (x << p1) | (x << p2)
    result = n ^ x
    return result

def array_to_hex(a):
	b = []
	for i in range(0, len(a)):
		b.append("0x%0.2X" % a[i])

	b = str(b).replace("'","").replace(" ","")[1:-1]
	return '\n'.join(textwrap.wrap(b, 45))

def _header():
	b = """/*

	""" + filename + """

	Sound Effect File.
	
	Info:
		Length			:	""" + str(fxh_len) + """
		Priority		:	""" + str(fxh_pry) + """
		Channels used	:	""" + ch_used_str + """
		SGB Support		:	""" + ["No", "Yes"][(header & 64) >> 6] + """"""
	if sgb:
		b += """
		SGB SFX Table	:	""" + sgb[0] + """
		SGB SFX ID		:	""" + sgb[1] + """
		SGB SFX	Pitch	:	""" + sgb[2] + """
		SGB SFX Volume	:	""" + sgb[3] + """"""
	b += """
*/

"""
	return b

import textwrap
import argparse
parser = argparse.ArgumentParser()
parser.add_argument("fxsav", help="FX Hammer .sav file (Normally called 'hammered.sav').")
parser.add_argument("fxnum", help="Index of the desired SFX to export.")
parser.add_argument("out", help="Folder where .c and .h files will be saved.")
parser.add_argument("--fxammo", help="Number of SFX to export (starts at fxnum, ends at fxnum + fxammo)")
parser.add_argument("--fxnamelist", help="Text file with all the names for the SFX, each on one line (SFX names shouldn't have spaces), you can add 4 values after the SFX name to define SGB sound values (See --sgb parameter).")
parser.add_argument("--sgb", help="Add Super Game Boy support.", nargs=4, metavar=("FX_TAB", "FX_ID", "FX_PITCH", "FX_VOL"))
parser.add_argument("--fxinv", help="Invert pan values for SFX (FX Hammer has them inverted by default, without this flag, the panning will be corrected)", action="store_true")
parser.add_argument("--fxmono", help="Avoid all panning writes and store as mono.", action="store_true")

args = parser.parse_args()

sgb = []
if args.sgb:
	args.sgb.copy()

fxsav = open(args.fxsav,"rb")

loop = 1

size_count = 0

if args.fxnamelist:
	fxnamelist = open(args.fxnamelist,"r")
if args.fxammo:
	loop = int(args.fxammo)
for n in range(int(args.fxnum), int(args.fxnum) + loop):
	fxnum = n
	bk_pan = -1 # Not an 8 bit integer to assure that panning is stored at least once

	# Get filename from textfile

	filename = "SFX_" + (("{0:X}").format(fxnum)).zfill(2)
	if args.fxnamelist:
		txt = fxnamelist.readline().replace("\n",'').split(" ")
		filename = txt[0]
		# Check length
		if len(txt) > 1: # More parameters on string means SGB
			sgb = [txt[1], txt[2], txt[3], txt[4]]
		else:
			sgb = []
			if args.sgb:
				args.sgb.copy()

	# Export header
	fxh_pry = 0		# Export priority
	fxh_chu = 0		# Export CH used
	fxh_len = 0		# Export length

	# Export data
	fxh_buf = []

	def bufadd(b):
		fxh_buf.append(b)

	def fxh_get():
		return int.from_bytes(fxsav.read(1), "big")

	# Get the priority
	fxsav.seek(0x200 + fxnum)
	fxh_pry = fxh_get()

	# Get channels used
	fxsav.seek(0x300 + fxnum)
	fxh_chu = fxh_get()
	ch = 0 # Temp value to make my life easier
	if fxh_chu & 0x30: # If CH2
		ch |= 8
		ch_used_str = "Noise channel"
	if fxh_chu & 0x03: # If CH4
		ch |= 2
		ch_used_str = "Duty channel 2"
	if fxh_chu == 0x33:
		ch_used_str = "Duty channel 2 & Noise channel"
	fxh_chu = ch << 4
	# This basically converts the 0x33 based FX Hammer mask value into a 0b10100000 value, bit 7 means CH2 and bit 5 means CH4

	if fxh_chu == 0: # If no channels are used, the sfx is empty, abort
		sys.exit("ERROR: SFX #" + (("{0:X}").format(fxnum)).zfill(2) + " is empty, aborting conversion.")

	# Add header
	header = fxh_chu | fxh_pry
	if sgb:
		header |= 64 # 6th bit = SGB Support
	bufadd(header)
	bufadd(0) # Add placeholder value for later

	# Check SGB
	if sgb:
		bufadd(65) # Command byte ((SGB_SOUND << 3) | 1)
		if sgb[0] == "A":
			bufadd(int(sgb[1])) # Sound Effect A
			bufadd(0) # Sound Effect B
		elif sgb[0] == "B":
			bufadd(0) # Sound Effect A
			bufadd(int(sgb[1])) # Sound Effect B
		if sgb[0] == "A":
			bufadd(int(sgb[2]) | (int(sgb[3]) << 2)) # Sound effect attributes (A)
		elif sgb[0] == "B":
			bufadd((int(sgb[2]) << 4) | (int(sgb[3]) << 6)) # Sound effect attributes (B)
		bufadd(0) # Music Score Code (Unused)

	# Get all data (Length)
	fxsav.seek(0x400 + (fxnum * 256))

	# Loop through every frame
	for f in range(32):
		temp_buf = [0] * 8 # The curr frame's data goes here, then converted to CBTFX format
		temp_buf[0] = fxh_get() # Get frame length
		if temp_buf[0] != 0: # if the frame isn't the last frame
			temp_buf[1] = fxh_get()	# CH2 pan
			temp_buf[2] = fxh_get() & 0xf0	# CH2 vol
			temp_buf[3] = fxh_get()	# CH2 duty
			temp_buf[4] = fxh_get()	# CH2 note
			temp_buf[5] = fxh_get()	# CH4 pan
			temp_buf[6] = fxh_get()	# CH4 vol
			temp_buf[7] = fxh_get()	# CH4 freq
			fxh_len += 1
		else:
			break # Got to the end of the SFX, break out of the read loop
		
		# Length and frame pan
		pan = temp_buf[1] | temp_buf[5]
		if not args.fxinv:
			pan = swapbits(pan, 7, 3)
			pan = swapbits(pan, 5, 1)
		if args.fxmono:
			pan = 0xaa
		if pan != bk_pan:
			bufadd((temp_buf[0] - 1) | 0x80)
			bufadd(pan) # NR51 values
			bk_pan = pan
		else:
			bufadd(temp_buf[0] - 1)

		# CH2 Duty (NR21)
		if fxh_chu & 128:
			bufadd(temp_buf[3])

		# Frame Volume		Necessary
		bufadd(temp_buf[2] | (temp_buf[6] >> 4))

		# CH2 Frequency (NR23)
		if fxh_chu & 128:
			bufadd(CH2freqs[(temp_buf[4] - 0x40) >> 1] & 0xff)

		# CH2 Frequency higher 3 bits (NR24)
		if fxh_chu & 128:
			bufadd(CH2freqs[(temp_buf[4] - 0x40) >> 1] >> 8 | 0x80) # Note index - 0x40 / 2 >> 8 | trigger bit

		# CH4 Freq (NR43)
		if fxh_chu & 32:
			bufadd(temp_buf[7])

	# Replace placeholder value for fxh_len
	fxh_buf[1] = fxh_len + 1

	# Write to C file
	if args.out == ".":
		args.out = ""
	Cfile = open(args.out + filename + ".c", "w")
	Cfile.write(_header())
	Cfile.write("const unsigned char " + filename + "[] = {\n")
	Cfile.write(array_to_hex(fxh_buf))
	Cfile.write("\n};")
	Cfile.close()

	Hfile = open(args.out + filename + ".h", "w")
	Hfile.write(_header())
	Hfile.write("#ifndef __" + filename + "_h_INCLUDE\n")
	Hfile.write("#define __" + filename + "_h_INCLUDE\n")
	Hfile.write("#define CBTFX_PLAY_" + filename + " CBTFX_init(&" + filename + "[0])\n")
	Hfile.write("extern const unsigned char " + filename + "[];\n")
	Hfile.write("#endif")
	Hfile.close()

	print(filename + ".c" + " succesfully written.")
	print(filename + " Size: " + str(len(fxh_buf) + 2) + " bytes.\n")

	size_count += len(fxh_buf) + 2

fxsav.close()

if args.fxammo:
	print("Final size: " + str(size_count) + " bytes.\n")

if args.fxnamelist:
	fxnamelist.close()