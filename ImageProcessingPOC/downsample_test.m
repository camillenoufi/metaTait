I = imread('bluewhitered.png');
I_15W = imresize(I, [NaN 15]);

figure
imshow(I)
title('Original Image')
figure
imshow(I_15W)
title('Downsampled Image')