#!/usr/bin/env python

# Noise Volume Texture in GIMP Python

from gimpfu import *
import random
import time
import sys

def noise_volume_texture(width, height, depth, density, detail, turbulent) :
	random.seed(time.clock())

	# Make a new image
	img = gimp.Image(int(width), int(height), RGB)

	# Save the current foreground color:
	pdb.gimp_context_push()

	for z in range(0, int(depth)):
		# Create a new layer
		layer = pdb.gimp_layer_new(img, width, height, RGB_IMAGE, "Depth " + str(z), 100, NORMAL_MODE)
		# Add the layer to the layer stack
		img.add_layer(layer, int(depth))
		# Generate tilable noise on the current layer
		pdb.plug_in_solid_noise(img, layer, True, turbulent, random.random() * sys.maxint, detail, density, density)

	# Create a new image window
	gimp.Display(img)
	
	# Show the new image window
	gimp.displays_flush()

	# Restore the old foreground color:
	pdb.gimp_context_pop()

register(
	"noise_volume_texture",
	"Noise Volume Texture",
	"Create a new multi-layered image with procedural noise",
	"Bogdan Iftode",
	"Bogdan Iftode",
	"2015",
	"Noise Volume Texture (Py)",
	"",	  # Create a new image, don't work on an existing one
	[
		(PF_SPINNER,	"width",		"Width",		32,		(1, 1024, 1)),
		(PF_SPINNER,	"height",		"Height",		32,		(1, 1024, 1)),
		(PF_SPINNER,	"depth",		"Depth",		32,		(1, 1024, 1)),
		(PF_SPINNER,	"density",		"Density",		5,		(1, 16, 1)),
		(PF_SPINNER,	"detail",		"Detail",		0,		(0, 15, 1)),
		(PF_TOGGLE,		"turbulent",	"Turbulent",	False)
	],
	[],
	noise_volume_texture, menu="<Image>/File/Create")

main()
