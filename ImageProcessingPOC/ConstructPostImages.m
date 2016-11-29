function [ t0data ] = ConstructPostImages(data_packet,num_images, column_num, height)

%for loop to go through all 18 times
%but first......

% height = height;
% numImages = num_images
% sectionLength = section_length;

rgbDim = 3;
numLEDs = size(data_packet,1);

%extract time0 parts of packet
t0data = data_packet(:,1:height*rgbDim*column_num);

rData = zeros(height,1);
gData = zeros(height,1);
bData = zeros(height,1);

for i = 1:numLEDs
    for j = 1:column_num
        
        %calculate indices
        startingIndexRed = (j-1)*height*3 + 1;
        endingIndexRed = (j-1)*height*3 + height;
        startingIndexGreen = startingIndexRed + height;
        endingIndexGreen = endingIndexRed + height;
        startingIndexBlue = startingIndexRed + 2*height;
        endingIndexBlue = endingIndexRed + 2*height;
        
        %extract columns and concatenate into color matrix
        rData = horzcat(rData, transpose(t0data(i, startingIndexRed : endingIndexRed )));
        gData = horzcat(gData, transpose(t0data(i, startingIndexGreen : endingIndexGreen )));
        bData = horzcat(bData, transpose(t0data(i, startingIndexBlue : endingIndexBlue )));
    end
end

%remove initialization column
rData = rData(:,2:end);
gData = gData(:,2:end);
bData = bData(:,2:end);

%create 3D matrix for RGB "image"
t0ImageMatrix = cat(3,rData,gData,bData);


figure
imshow(t0ImageMatrix)
title('t0 Image')

end

