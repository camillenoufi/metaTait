function [ I_matrix ] = DownsampleImage( image_matrix, resolution_height, resolution_width )
% Input: image: 3D matrix corresponding to original image
% Process: downsample the image, display the original and new imageoutput the
%matrix


I_matrix = imresize(image_matrix, [NaN resolution_width]);    %downsample to 15-pixel width and corresponding height resolution

% figure
% imshow(I_matrix)
% title('Downsampled Image by Width')

if size(I_matrix,1) > resolution_height
    I_matrix = imresize(image_matrix, [resolution_height NaN]);
%     figure
%     imshow(I_matrix)
%     title('Downsampled Image by Height')
end

I_matrix = AdjustToAspectRatio(I_matrix,resolution_height,resolution_width);

end

