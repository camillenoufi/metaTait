function [ imagePacket ] = CreateDataPacket2( im_array, led_num )
%CreateDataPacket reads in the downsampled image matrix and creates a
%   datapacket by formatting the matrix information in a way the MCU will be
%   able to understand
%
% Input:  image_array: a 4D WxHx3x#_of_images matrix holding the image data
% Output: dataPacket:  

% get metadata of matrix
width = size(im_array,1);
height = size(im_array,2);
rbgDim = size(im_array,3);
numImages = size(im_array,4);
% imageHeader = [height width];

sectionLength = width/size(led_num,2);            % remove hardcoding of values later

% for loop:  for each image, depending on Ln value, extract that section of
% each image, and order based on time t0-t17










imagePacket = imageHeader;

%create RGB content vectors for each cross section
for i = 1:width
    rData(1:height) = transpose(image_matrix(:,i,1));
    gData(1:height) = transpose(image_matrix(:,i,2));
    bData(1:height) = transpose(image_matrix(:,i,3));
    imagePacket = horzcat(imagePacket,rData,gData,bData);
    
end

end



