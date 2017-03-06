function [ dataPacket, sectionLength, height ] = CreateDataPacket2( im_array, led_num )
%CreateDataPacket reads in the downsampled image matrix and creates a
%   datapacket by formatting the matrix information in a way the MCU will be
%   able to understand
%
% Input:  image_array: a 4D WxHx3x#_of_images matrix holding the image data
% Output: dataPacket:  6 lines, organized by bgr->column->section->image
%                       per line

% get metadata of matrix
height = size(im_array,1);
width = size(im_array,2);
rgbDim = size(im_array,3);
numImages = size(im_array,4);
numLEDsPerPost = size(led_num,1);
sectionLength = width/numLEDsPerPost;

%initialize and preallocate arrays
%sectionData = zeros(height*sectionLength,1,rgbDim);    %preallocate for
%speed?
sectionData = [-1; -1];
headerSize = size(sectionData,1);
dataPacket = zeros(1,numImages*height*sectionLength*rgbDim + headerSize);


% for loop:  for each image, depending on Ln value, extract that section of
% each image, and order based on time t0-t17

for n = 1:numLEDsPerPost                            %iterate through image based on number of LEDs per post, creates "sections"
    currentLEDStrip = led_num(n,:);                 %used for debugging
    startingColumn = ((n-1)*sectionLength) + 1;     %connect starting column to LED strip
    startingImage = n;                              %LED strip's 1st image is offset
    endingImage = startingImage+(numImages-1);
    for i = startingImage:endingImage        %iterate forward through image set
        currentImage = mod(i,numImages);     %index through numImages-sized loop
        if currentImage == 0                 %if on last image x, account for mod(x,x)=0
            currentImage = numImages;
        end    
        for j = 1:sectionLength             %iterate forwards through columns
            for l = height:-1:1
                for k = rgbDim:-1:1     %LED strips read in B,G,R
                    sectionData = vertcat(sectionData,im_array(l, startingColumn + (j-1), k, currentImage)); 
                end
            end
        end
    end
    
    sectionData = transpose(sectionData);
    dataPacket = vertcat(dataPacket,sectionData);   %concatenate Ln row onto output packet L1-L6
    
    sectionData = [-1; -1]; %reset section Data
   

end

dataPacket = dataPacket(2:end,headerSize+1:end);
end



