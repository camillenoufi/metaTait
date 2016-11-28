function [ image_matrix ] = AdjustToAspectRatio( image_matrix, resolution_height, resolution_width )
%AdjustToAspectRatio reads in the downsampled image matrix and creates a
%   datapacket by formatting the matrix information in a way the MCU will be
%   able to understand
%
% Input:  image_matrix: a Hx15x3 matrix holding the image data
% Output: dataPacket:  

matrixHeight = size(image_matrix,1);
matrixWidth = size(image_matrix,2);

if matrixHeight < resolution_height
    heightDifference = resolution_height - matrixHeight;
    topPadding = zeros(floor(heightDifference/2),resolution_width,3,'uint8');
    bottomPadding = zeros(ceil(heightDifference/2),resolution_width,3,'uint8');
    
    image_matrix = vertcat(topPadding,image_matrix,bottomPadding);

    
else if matrixWidth < resolution_width
    widthDifference = resolution_width - matrixWidth;
    leftPadding = zeros(resolution_height,floor(widthDifference/2),3,'uint8');
    rightPadding = zeros(resolution_height,ceil(widthDifference/2),3,'uint8');
    
    image_matrix = horzcat(leftPadding,image_matrix,rightPadding);
    end
end

%check dimensions
if size(image_matrix,1) ~= resolution_height
    errordlg('image matrix has incorrect height')
end
if size(image_matrix,2) ~= resolution_width
    errordlg('image matrix has incorrect width')
end
if size(image_matrix,2) ~= resolution_width
    errordlg('image matrix has incorrect rbg dimension')
end
    
end

