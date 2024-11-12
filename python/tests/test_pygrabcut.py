import os
import numpy as np
from PIL import Image

input_image = np.asarray(Image.open("tests/data/flower1.jpg").convert('RGB'))
print(input_image.shape)
selection = [68, 23, 539, 510]

print(__file__)
print(os.getcwd())
import pygrabcut as g

# result = g.make_mask(input_image, selection)
# s = input_image.shape
# result = np.asarray(result).reshape((s[0], s[1]))
# Image.fromarray(result, "1").save("mask.png")

Image.fromarray(g.foo(input_image), "RGB").save("img.png")

#result = g.run_grabcut(input_image, selection, 1)
#grabcut = g.Grabcut()
#grabcut.init(input_image, selection)
#grabcut.run(0)
#result = grabcut.get_result()
#s = input_image.shape
#result = np.asarray(result).reshape((s[0], s[1], 3))
#Image.fromarray(result, "RGB").save("result_image.jpg")
