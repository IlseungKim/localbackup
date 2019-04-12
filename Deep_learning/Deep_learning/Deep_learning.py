import matplotlib.pyplot as plt
from matplotlib.image import imread
from scipy.misc import imread, imresize, imsave

img = imread('lena.jpg')
plt.imshow(img)
#plt.show()
print(img.dtype,img.shape)
img_new = img*[[1.0,0.7,0.5]]
img_new = imresize(img_new,img.shape)
plt.imshow(img_new)
plt.show()