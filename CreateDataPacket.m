function [ imagePacket ] = CreateDataPacket( image_matrix )
%CreateDataPacket reads in the downsampled image matrix and creates a
%   datapacket by formatting the matrix information in a way the MCU will be
%   able to understand
%
% Input:  image_matrix: a HxWx3 matrix holding the image data
% Output: dataPacket:  

% get metadata of matrix
height = size(image_matrix,1);
width = size(image_matrix,2);
imageHeader = [height width];

sectionLength = width/6;

imagePacket = imageHeader;

%create RGB content vectors for each cross section
for i = 1:width
    rData(1:height) = transpose(image_matrix(:,i,1));
    gData(1:height) = transpose(image_matrix(:,i,2));
    bData(1:height) = transpose(image_matrix(:,i,3));
    imagePacket = horzcat(imagePacket,rData,gData,bData);
    
end

end

