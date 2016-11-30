function [ downsampledImageMatrix ] = CreateImageMatrix( image_folder,image_file, resolution_height, resolution_width )

% read in image
originalImage = imread(strcat(image_folder,'/',image_file));

% figure
% imshow(originalImage)
% title('Original Image')

% downsample image and create 3D matrix and display downsampled image
downsampledImageMatrix = DownsampleImage(originalImage,resolution_height, resolution_width);

end

